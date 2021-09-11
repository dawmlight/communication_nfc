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
#ifndef ISECURE_ELEMENT_CHANNEL_H
#define ISECURE_ELEMENT_CHANNEL_H

#include <iostream>
#include <memory>

#include "iremote_broker.h"

namespace OHOS::se::omapi {
class ISecureElementSession;

class ISecureElementChannel : public OHOS::IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.nfc.se.ISecureElementChannel");
    virtual ~ISecureElementChannel() {}
    /**
     * @brief Closes this channel to the SE. If the method is called when the channel is already closed,
     * this method SHALL be ignored.
     */
    virtual void Close() = 0;
    /**
     * @brief Tells if this channel is closed.
     * @return True if the channel is closed, false otherwise.
     */
    virtual bool IsClosed() = 0;
    /**
     * @brief Returns a Bool indicating whether this channel is the basic channel.
     * @return True if this channel is a basic channel. False if this channel is a logical channel.
     */
    virtual bool IsBasicChannel() = 0;
    /**
     * @brief Returns the data as received from the application select command, including the status word received at
     * applet selection.
     * @return The returned byte array contains the data bytes in the following order:
     *          [<first data byte>, …, <last data byte>, <sw1>, <sw2>]
     */
    virtual std::string GetSelectResponse() = 0;
    /**
     * @brief Transmit an APDU command (as per ISO/IEC 7816) to the SE.
     * @param command The APDU command to be transmitted, as a byte array.
     * @return The response received, as a byte array. The returned byte array contains the data bytes in the following
     * order: {<first data byte>, …, <last data byte>, <sw1>, <sw2>}.
     */
    virtual std::string Transmit(const std::string& command) = 0;
    /**
     * @brief Performs a selection of the next applet on this channel that matches to the partial AID specified in the
     * openBasicChannel() or openLogicalChannel() method.
     * @return true, a new applet was successfully selected on this channel.
     * @return false, if no further applet exists with matches to the partial AID, and the applet already selected stays
     * selected.
     */
    virtual bool SelectNext() = 0;
};
}  // namespace OHOS::se::omapi
#endif  // ISECURE_ELEMENT_CHANNEL_H
