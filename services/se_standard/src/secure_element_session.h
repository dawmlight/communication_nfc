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
#ifndef SECURE_ELEMENT_SESSION_H
#define SECURE_ELEMENT_SESSION_H

#include <mutex>
#include <vector>

#include "isecure_element_channel.h"
#include "secure_element_reader.h"
#include "secure_element_session_stub.h"

using ISEReader = OHOS::se::omapi::ISecureElementReader;
using ISEChannel = OHOS::se::omapi::ISecureElementChannel;

namespace OHOS {
namespace se {
class SeEndService;
class SeChannel;
class SecureElementReader;

class SecureElementSession final : public omapi::SecureElemntSessionStub {
public:
    ~SecureElementSession();
    /**
     * @brief Get the ATR of this SE. A Null value SHALL be returned if the ATR for this SE is not available.
     * @return The ATR as a byte array or Null.
     */
    std::string GetATR() override;
    /**
     * @brief Close the connection with the SE. This will close any channels opened by this application with this SE.
     */
    void Close() override;
    /**
     * @brief Tells if this session is closed.
     * @return True if the session is closed, false otherwise.
     */
    bool IsClosed() override;
    /**
     * @brief Close any channels opened on this session.
     */
    void CloseChannels() override;
    /**
     * @brief Get access to the basic channel, as defined in [ISO 7816-4] (the one that has number 0). The obtained
     * object is an instance of the channel class.
     * @param aid The AID of the applet to be selected on this channel, as a byte array, or Null if no applet is to be
     * selected.
     * @param p2 The P2 parameter of the SELECT APDU executed on this channel.
     * @return An instance of channel if available or Null.
     */
    sptr<ISEChannel> OpenBasicChannel(std::string const& aid, int p2) override;
    /**
     * @brief Open a logical channel with the SE, selecting the applet represented by the given AID (when the AID is not
     * Null and the length of the AID is not 0).
     * @param aid The AID of the applet to be selected on this channel, as a byte array.
     * @param p2 The P2 parameter of the SELECT APDU executed on this channel.
     * @return An instance of channel. Null if the SE is unable to provide a new logical channel or is unable to
     * retrieve Access Control rules due to the lack of an available logical channel.
     */
    sptr<ISEChannel> OpenLogicalChannel(std::string const& aid, int p2) override;

    /**
     * @brief remove the special channel on this session
     * @param channel the removed channel object
     */
    void RemoveChannel(std::weak_ptr<SeChannel> channel);

private:
    explicit SecureElementSession(std::weak_ptr<SeEndService> seService, wptr<SecureElementReader> reader);
    void CloseChannelsUnlock();

private:
    std::weak_ptr<SeEndService> mSEService_{};
    wptr<SecureElementReader> mReader_{};
    std::vector<std::shared_ptr<SeChannel>> mChannels_{};
    std::recursive_mutex mLock_{};
    bool mIsClosed_{true};
    std::string mAtr_{};

    friend class SecureElementReader;
};
}  // namespace se
}  // namespace OHOS
#endif  // !SECURE_ELEMENT_SESSION_H
