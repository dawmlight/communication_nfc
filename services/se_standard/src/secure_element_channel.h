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
#ifndef SECURE_ELEMENT_CHANNEL_H
#define SECURE_ELEMENT_CHANNEL_H

#include "secure_element_channel_stub.h"
#include "secure_element_session.h"

namespace OHOS {
namespace se {
class SeChannel;

class SecureElementChannel final : public omapi::SecureElemntChannelStub {
public:
    /**
     * @brief Closes this channel to the SE. If the method is called when the channel is already closed,
     * this method SHALL be ignored.
     */
    void Close() override;
    /**
     * @brief Tells if this channel is closed.
     * @return True if the channel is closed, false otherwise.
     */
    bool IsClosed() override;
    /**
     * @brief Returns a Bool indicating whether this channel is the basic channel.
     * @return True if this channel is a basic channel. False if this channel is a logical channel.
     */
    bool IsBasicChannel() override;
    /**
     * @brief Returns the data as received from the application select command, including the status word received at
     * applet selection.
     * @return The returned byte array contains the data bytes in the following order:
     *          [<first data byte>, …, <last data byte>, <sw1>, <sw2>]
     */
    std::string GetSelectResponse() override;
    /**
     * @brief Transmit an APDU command (as per ISO/IEC 7816) to the SE.
     * @param command The APDU command to be transmitted, as a byte array.
     * @return The response received, as a byte array. The returned byte array contains the data bytes in the following
     * order: {<first data byte>, …, <last data byte>, <sw1>, <sw2>}.
     */
    std::string Transmit(std::string const& command) override;
    /**
     * @brief Performs a selection of the next applet on this channel that matches to the partial AID specified in the
     * openBasicChannel() or openLogicalChannel() method.
     * @return true, a new applet was successfully selected on this channel.
     * @return false, if no further applet exists with matches to the partial AID, and the applet already selected stays
     * selected.
     */
    bool SelectNext() override;

private:
    explicit SecureElementChannel(std::weak_ptr<SeChannel> channel);

private:
    std::weak_ptr<SeChannel> mChannel_{};

    friend class SeChannel;
    friend class SecureElementSession;
};
}  // namespace se
}  // namespace OHOS
#endif  // !SECURE_ELEMENT_CHANNEL_H
