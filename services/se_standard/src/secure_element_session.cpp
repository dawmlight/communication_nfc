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
#include "secure_element_session.h"

#include "ipc_skeleton.h"
#include "loghelper.h"
#include "se_channel.h"
#include "se_common_exception.h"
#include "se_end_service.h"
#include "secure_element_channel.h"
#include "secure_element_reader.h"
#include "secure_element_service.h"
#include "terminal.h"

namespace OHOS {
namespace se {
/**
 * @brief Get the ATR of this SE. A Null value SHALL be returned if the ATR for this SE is not available.
 * @return The ATR as a byte array or Null.
 */
std::string SecureElementSession::GetATR()
{
    if (!mIsClosed_ && mReader_.promote()) {
        // session isn't closed
        mAtr_ = mReader_.promote()->GetAtr();
    }
    return mAtr_;
}
/**
 * @brief Close the connection with the SE. This will close any channels opened by this application with this SE.
 */
void SecureElementSession::Close()
{
    DebugLog("SecureElementSession::Close Begin");
    std::lock_guard<std::recursive_mutex> lock(mLock_);
    if (mIsClosed_ || !mReader_.promote()) {
        return;
    }
    //  Close All Channels
    CloseChannelsUnlock();
    // remove this session
    mReader_.promote()->RemoveSession(this);
    // the session is closed
    mIsClosed_ = true;
    DebugLog("SecureElementSession::Close End");
}
/**
 * @brief Tells if this session is closed.
 * @return True if the session is closed, false otherwise.
 */
bool SecureElementSession::IsClosed()
{
    std::lock_guard<std::recursive_mutex> lock(mLock_);
    return mIsClosed_;
}
/**
 * @brief Close any channels opened on this session.
 */
void SecureElementSession::CloseChannels()
{
    std::lock_guard<std::recursive_mutex> lock(mLock_);
    DebugLog("SecureElementSession::CloseChannels Begin");
    CloseChannelsUnlock();
    DebugLog("SecureElementSession::CloseChannels End");
}

void SecureElementSession::CloseChannelsUnlock()
{
    DebugLog("SecureElementSession::CloseChannelsUnlock Begin");
    while (mChannels_.size() > 0) {
        try {
            mChannels_.at(0)->Close();
        } catch (const std::exception& ignore) {
            ErrorLog("SecureElementSession SeChannel - close Exception %s", ignore.what());
        }
    }
    DebugLog("SecureElementSession::CloseChannelsUnlock End");
}

/**
 * @brief Get access to the basic channel, as defined in [ISO 7816-4] (the one that has number 0). The obtained
 * object is an instance of the channel class.
 * @param aid The AID of the applet to be selected on this channel, as a byte array, or Null if no applet is to be
 * selected.
 * @param p2 The P2 parameter of the SELECT APDU executed on this channel.
 * @return the Basic SeChannel instance
 */
sptr<ISEChannel> SecureElementSession::OpenBasicChannel(std::string const& aid, int p2)
{
    if (IsClosed()) {
        throw IllegalStateError("Session is closed");
    } else if ((p2 != 0x00) && (p2 != 0x04) && (p2 != 0x08) && (p2 != (char)0x0C)) {
        char what[32]{'\0'};
        sprintf(what, "p2 not supported: %02x", p2 & 0xFF);
        throw UnsupportedOperationError(what);
    }

    std::string packageName = mSEService_.lock()->GetBundleNameFromCallingUid(IPCSkeleton::GetCallingUid());
    std::shared_ptr<SeChannel> channel{};

    try {
        channel = mReader_.promote()->GetTerminal().lock()->OpenBasicChannel(
            aid, p2, packageName, IPCSkeleton::GetCallingPid());
    } catch (const IOError& e) {
        throw ServiceSpecificError(SE_SER_ERR_IO, e.what());
    } catch (const NoSuchElementError& e) {
        throw ServiceSpecificError(SE_SER_ERR_NO_SUCH_ELEMENT, e.what());
    }

    if (!channel) {
        InfoLog("OpenBasicChannel() - returning null");
        return nullptr;
    }
    InfoLog("Open basic channel success. SeChannel: %d", channel->GetChannelNumber());

    sptr<SecureElementChannel> iChannel = new SecureElementChannel(channel);
    channel->SetUpRelationship(iChannel, this);
    mChannels_.push_back(channel);
    return iChannel;
}
/**
 * @brief Open a logical channel with the SE, selecting the applet represented by the given AID (when the AID is not
 * Null and the length of the AID is not 0).
 * @param aid The AID of the applet to be selected on this channel, as a byte array.
 * @param p2 The P2 parameter of the SELECT APDU executed on this channel.
 * @return An instance of channel. Null if the SE is unable to provide a new logical channel or is unable to
 * retrieve Access Control rules due to the lack of an available logical channel.
 */
sptr<ISEChannel> SecureElementSession::OpenLogicalChannel(std::string const& aid, int p2)
{
    if (IsClosed()) {
        throw IllegalStateError("Session is closed");
    } else if (aid.empty() &&
               0 == mReader_.promote()->GetTerminal().lock()->GetName().compare(
                        0, std::string(SeEndService::UICC_TERMINAL).length(), SeEndService::UICC_TERMINAL)) {
        return nullptr;
    } else if ((p2 != 0x00) && (p2 != 0x04) && (p2 != 0x08) && (p2 != (char)0x0C)) {
        char what[32]{'\0'};
        sprintf(what, "p2 not supported: %02x", p2 & 0xFF);
        throw UnsupportedOperationError(what);
    }

    std::string packageName = mSEService_.lock()->GetBundleNameFromCallingUid(IPCSkeleton::GetCallingUid());
    std::shared_ptr<SeChannel> channel{};

    try {
        channel = mReader_.promote()->GetTerminal().lock()->OpenLogicalChannel(
            aid, p2, packageName, IPCSkeleton::GetCallingPid());
    } catch (const IOError& e) {
        throw ServiceSpecificError(SE_SER_ERR_IO, e.what());
    } catch (const NoSuchElementError& e) {
        throw ServiceSpecificError(SE_SER_ERR_NO_SUCH_ELEMENT, e.what());
    }

    if (!channel) {
        InfoLog("openLogicalChannel() - returning null");
        return nullptr;
    }
    InfoLog("openLogicalChannel() Success. SeChannel: %d", channel->GetChannelNumber());

    sptr<SecureElementChannel> iChannel = new SecureElementChannel(channel);
    channel->SetUpRelationship(iChannel, this);
    mChannels_.push_back(channel);
    return iChannel;
}

/**
 * @brief remove the special channel on this session
 * @param channel the removed channel object
 */
void SecureElementSession::RemoveChannel(std::weak_ptr<SeChannel> channel)
{
    DebugLog("SecureElementSession::RemoveChannel");
    if (channel.expired()) {
        return;
    }

    std::lock_guard<std::recursive_mutex> lock(mLock_);
    if (mChannels_.empty()) {
        return;
    }
    auto iChannel = channel.lock();
    std::vector<std::shared_ptr<SeChannel>>::iterator iter = mChannels_.begin();
    while (iter != mChannels_.end()) {
        if ((*iter) == iChannel) {
            mChannels_.erase(iter);
            break;
        }
        ++iter;
    }
}

SecureElementSession::SecureElementSession(std::weak_ptr<SeEndService> seService, wptr<SecureElementReader> reader)
    : mSEService_(seService), mReader_(reader), mIsClosed_(false)
{
}

SecureElementSession::~SecureElementSession()
{
    mChannels_.clear();
    DebugLog("SecureElementSession::~SecureElementSession");
}
}  // namespace se
}  // namespace OHOS