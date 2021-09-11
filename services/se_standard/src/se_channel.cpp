/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "se_channel.h"

/*
 * Import OSAL Functions
 */
#include "access_control_enforcer.h"
#include "apdu_command_validator.h"
#include "channel_access_rule.h"
#include "loghelper.h"
#include "se_common_exception.h"
#include "secure_element_session.h"
#include "terminal.h"

namespace OHOS {
namespace se {
using security::ChannelAccessRule;

void SeChannel::SetUpRelationship(sptr<ISEChannel> iChannel, wptr<SecureElementSession> session)
{
    iChannel_ = iChannel;
    seSession_ = session;
}
/**
 * @brief Register SeChannel To APP Binder to receive the 'BinderDied'
 */
void SeChannel::Initialize(std::weak_ptr<SESChannelAccess> channelAccess)
{
    SetChannelAccess(channelAccess);
}

void SeChannel::Close()
{
    std::lock_guard<std::mutex> lock(mLock_);
    if (IsBasicChannel()) {
        InfoLog("Close basic channel - Select without AID ...");
        mTerminal_.lock()->SelectDefaultApplication();
    }
    mTerminal_.lock()->CloseChannel(shared_from_this());
    mIsClosed_ = true;
    if (seSession_ != nullptr) {
        seSession_->RemoveChannel(shared_from_this());
    }
    iChannel_ = nullptr;
}

std::string SeChannel::Transmit(std::string command)
{
    if (IsClosed()) {
        throw IllegalStateError("SeChannel is closed");
    }
    if (command.empty()) {
        throw NullPointerError("Command must not be null");
    }
    if (!mChannelAccess_) {
        throw SecurityError("SeChannel access not set");
    }
    if (mChannelAccess_->GetCallingPid() != mCallingPid) {
        throw SecurityError("Wrong Caller PID.Calling Pid: " + std::to_string(mCallingPid) +
                            " CA Pid: " + std::to_string(mChannelAccess_->GetCallingPid()));
    }

    // Validate the APDU command format
    ApduCommandValidator::Validate(command);

    // specifies 01xx xxxx as further interindustry values.
    if (((command[0] & 0x80) == 0) && ((command[0] & 0x60) != 0x20)) {
        // ISO command
        if (command[1] == 0x70) {
            throw SecurityError("MANAGE CHANNEL command not allowed");
        }
        if ((command[1] == 0xA4) && (command[2] == 0x04)) {
            // SELECT by DF name is only allowed for CarrierPrivilege applications
            // or system privilege applications
            if (ChannelAccessRule::ACCESSRULE::ALWAYS != mChannelAccess_->GetPrivilegeAccessRule()) {
                throw SecurityError("SELECT by DF name command not allowed");
            }
        }
    }

    CheckCommand(command);
    {
        std::lock_guard<std::mutex> lock(mLock_);
        // set channel number bits
        command[0] = SetChannelToClassByte(command[0], mChannelNumber_);
        return mTerminal_.lock()->Transmit(command);
    }
}

std::weak_ptr<SESChannelAccess> SeChannel::GetChannelAccess()
{
    return mChannelAccess_;
}

void SeChannel::SetChannelAccess(std::weak_ptr<SESChannelAccess> channelAccess)
{
    mChannelAccess_ = channelAccess.lock();
}

bool SeChannel::HasSelectedAid()
{
    return (!mAid_.empty());
}

int SeChannel::GetChannelNumber()
{
    return mChannelNumber_;
}

std::string SeChannel::GetSelectResponse()
{
    return (HasSelectedAid() ? mSelectResponse_ : std::string());
}

bool SeChannel::IsBasicChannel()
{
    return (mChannelNumber_ == 0) ? true : false;
}

bool SeChannel::IsClosed()
{
    return mIsClosed_;
}

bool SeChannel::SelectNext()
{
    if (IsClosed()) {
        throw IllegalStateError("SeChannel is closed");
    } else if (!mChannelAccess_) {
        throw IllegalStateError("SeChannel access not set.");
    } else if (mChannelAccess_->GetCallingPid() != mCallingPid) {
        throw SecurityError("Wrong Caller PID.Calling Pid: " + std::to_string(mCallingPid) +
                            " CA Pid: " + std::to_string(mChannelAccess_->GetCallingPid()));
    } else if (mAid_.empty()) {
        throw UnsupportedOperationError("No aid given");
    }
    std::string selectCommand{0x00, (char)0xA4, 0x04, 0x02, (char)(0xff & mAid_.length())};
    selectCommand.append(mAid_, 0, mAid_.length());

    // set channel number bits
    selectCommand[0] = SetChannelToClassByte(selectCommand[0], mChannelNumber_);

    std::string selectResponse = mTerminal_.lock()->Transmit(selectCommand);
    int sw1 = selectResponse[selectResponse.length() - 2] & 0xFF;
    int sw2 = selectResponse[selectResponse.length() - 1] & 0xFF;
    int sw = (sw1 << 8) | sw2;

    if (((sw & 0xF000) == 0x9000) || ((sw & 0xFF00) == 0x6200) || ((sw & 0xFF00) == 0x6300)) {
        mSelectResponse_ = selectResponse;
        return true;
    } else if ((sw & 0xFF00) == 0x6A00) {
        return false;
    } else {
        throw UnsupportedOperationError("Unsupported operation");
    }
}

void SeChannel::SetCallingPid(int pid)
{
    mCallingPid = pid;
}

void SeChannel::CheckCommand(std::string command)
{
    if (!mTerminal_.lock()->GetAccessControlEnforcer().expired()) {
        // check command if it complies to the access rules.
        // if not an exception is thrown
        mTerminal_.lock()->GetAccessControlEnforcer().lock()->CheckCommand(shared_from_this(), command);
    } else {
        // Allow access to Privileged App even if Access Control Enforcer is
        // not initialized
        if (ChannelAccessRule::ACCESSRULE::ALWAYS != mChannelAccess_->GetPrivilegeAccessRule()) {
            char what[64]{'\0'};
            sprintf(what, "Access Controller not set for Terminal:: %s", mTerminal_.lock()->GetName().c_str());
            throw SecurityError(what);
        }
    }
}

char SeChannel::SetChannelToClassByte(char cla, int channelNumber)
{
    if (channelNumber < 4) {
        // b7 = 0 indicates the first interindustry class byte coding
        cla = (cla & 0xBC) | channelNumber;
    } else if (channelNumber < 20) {
        // b7 = 1 indicates the further interindustry class byte coding
        bool isSm = (cla & 0x0C) != 0;
        cla = (cla & 0xB0) | 0x40 | (channelNumber - 4);
        if (isSm) {
            cla |= 0x20;
        }
    } else {
        throw IllegalArgumentError("SeChannel number must be within [0..19]");
    }
    return cla;
}

SeChannel::SeChannel(std::weak_ptr<Terminal> terminal,
                     int channelNumber,
                     const std::string& selectResponse,
                     const std::string& aid)
    : mChannelNumber_(channelNumber),
      mIsClosed_(false),
      mTerminal_(terminal),
      mSelectResponse_(selectResponse),
      mAid_(aid)
{
}
}  // namespace se
}  // namespace OHOS
