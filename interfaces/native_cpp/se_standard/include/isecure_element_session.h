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
#ifndef ISECURE_ELEMENT_SESSION_H
#define ISECURE_ELEMENT_SESSION_H

#include <iostream>
#include <memory>

#include "iremote_broker.h"

namespace OHOS {
class IRemoteObject;

namespace se::omapi {
class Channel;
class ISecureElementChannel;
class ISecureElementListener;
class ISecureElementSession : public OHOS::IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.nfc.se.ISecureElementSession");
    virtual ~ISecureElementSession() {}
    /**
     * @brief Get the ATR of this SE. A Null value SHALL be returned if the ATR for this SE is not available.
     * @return The ATR as a byte array or Null.
     */
    virtual std::string GetATR() = 0;
    /**
     * @brief Close the connection with the SE. This will close any channels opened by this application with this SE.
     */
    virtual void Close() = 0;
    /**
     * @brief Tells if this session is closed.
     * @return True if the session is closed, false otherwise.
     */
    virtual bool IsClosed() = 0;
    /**
     * @brief Close any channels opened on this session.
     */
    virtual void CloseChannels() = 0;
    /**
     * @brief Get access to the basic channel, as defined in [ISO 7816-4] (the one that has number 0). The obtained
     * object is an instance of the channel class.
     * @param aid The AID of the applet to be selected on this channel, as a byte array, or Null if no applet is to be
     * selected.
     * @param p2 The P2 parameter of the SELECT APDU executed on this channel.
     * @return An instance of channel if available or Null.
     */
    virtual sptr<ISecureElementChannel> OpenBasicChannel(const std::string& aid, int p2) = 0;
    /**
     * @brief Open a logical channel with the SE, selecting the applet represented by the given AID (when the AID is not
     * Null and the length of the AID is not 0).
     * @param aid The AID of the applet to be selected on this channel, as a byte array.
     * @param p2 The P2 parameter of the SELECT APDU executed on this channel.
     * @return An instance of channel. Null if the SE is unable to provide a new logical channel or is unable to
     * retrieve Access Control rules due to the lack of an available logical channel.
     */
    virtual sptr<ISecureElementChannel> OpenLogicalChannel(const std::string& aid, int p2) = 0;
};
}  // namespace se::omapi
}  // namespace OHOS
#endif  // ISECURE_ELEMENT_SESSION_H
