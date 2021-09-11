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
#include "secure_element_channel.h"

#include "ipc_skeleton.h"
#include "se_channel.h"
#include "se_common_exception.h"

namespace OHOS {
namespace se {
/**
 * @brief Closes this channel to the SE. If the method is called when the channel is already closed,
 * this method SHALL be ignored.
 */
void SecureElementChannel::Close()
{
    if (mChannel_.expired()) {
        return;
    }
    mChannel_.lock()->Close();
}

/**
 * @brief Tells if this channel is closed.
 * @return True if the channel is closed, false otherwise.
 */
bool SecureElementChannel::IsClosed()
{
    if (mChannel_.expired()) {
        return true;
    }
    return mChannel_.lock()->IsClosed();
}

/**
 * @brief Returns a Bool indicating whether this channel is the basic channel.
 * @return True if this channel is a basic channel. False if this channel is a logical channel.
 */
bool SecureElementChannel::IsBasicChannel()
{
    if (mChannel_.expired()) {
        return false;
    }
    return mChannel_.lock()->IsBasicChannel();
}
/**
 * @brief Returns the data as received from the application select command, including the status word received at
 * applet selection.
 * @return The returned byte array contains the data bytes in the following order:
 *          [<first data byte>, …, <last data byte>, <sw1>, <sw2>]
 */
std::string SecureElementChannel::GetSelectResponse()
{
    if (mChannel_.expired()) {
        return std::string();
    }
    return mChannel_.lock()->GetSelectResponse();
}

/**
 * @brief Transmit an APDU command (as per ISO/IEC 7816) to the SE.
 * @param command The APDU command to be transmitted, as a byte array.
 * @return The response received, as a byte array. The returned byte array contains the data bytes in the following
 * order: {<first data byte>, …, <last data byte>, <sw1>, <sw2>}.
 */
std::string SecureElementChannel::Transmit(std::string const& command)
{
    if (mChannel_.expired()) {
        return std::string();
    }
    std::shared_ptr<SeChannel> channel = mChannel_.lock();
    channel->SetCallingPid(IPCSkeleton::GetCallingPid());
    try {
        return channel->Transmit(command);
    } catch (const IOError& e) {
        throw ServiceSpecificError(SE_SER_ERR_IO, e.what());
    }
}

/**
 * @brief Performs a selection of the next applet on this channel that matches to the partial AID specified in the
 * openBasicChannel() or openLogicalChannel() method.
 * @return true, a new applet was successfully selected on this channel.
 * @return false, if no further applet exists with matches to the partial AID, and the applet already selected stays
 * selected.
 */
bool SecureElementChannel::SelectNext()
{
    if (mChannel_.expired()) {
        return false;
    }
    std::shared_ptr<SeChannel> channel = mChannel_.lock();
    channel->SetCallingPid(IPCSkeleton::GetCallingPid());
    try {
        return channel->SelectNext();
    } catch (const IOError& e) {
        throw ServiceSpecificError(SE_SER_ERR_IO, e.what());
    }
}

SecureElementChannel::SecureElementChannel(std::weak_ptr<SeChannel> channel) : mChannel_(channel) {}
}  // namespace se
}  // namespace OHOS