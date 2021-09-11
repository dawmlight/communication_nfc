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

#ifndef NFC_CARD_EMULATION_H
#define NFC_CARD_EMULATION_H

#include <memory>
#include <string>
#include <vector>

#include "iremote_object.h"
namespace OHOS {
namespace AppExecFwk {
class ElementName;
}
class IRemoteBroker;
namespace nfc::sdk {
class NfcAgent;

namespace cardemulation {
class ICardEmulationAgent;
class CardEmulationServiceInfoLite;
class CardEmulation final {
public:
    // the card emulation service type: secure
    static const std::string TYPE_SECURE;
    // the card emulation service type: normal
    static const std::string TYPE_NORMAL;

    /**
     * brief: get CardEmulation instance
     * parameter:
     *   nfcAgent -- NfcAgent object
     * return: CardEmulation instance
     */
    static std::shared_ptr<CardEmulation> GetInstance(std::shared_ptr<NfcAgent>);

    /**
     * brief: get card emulation service infomations by type
     * parameter:
     *   type -- service type
     * return: card emulation service infomations
     */
    std::vector<std::unique_ptr<CardEmulationServiceInfoLite>> GetServices(const std::string& type);

    /**
     * brief: get aids for the card emulation service
     * parameter:
     *   elementName -- card emulation service name
     *   type -- service type
     * return: aids of card emulation service
     */
    std::vector<std::string> GetAidsForService(const OHOS::AppExecFwk::ElementName& elementName,
                                               const std::string& type);
    /**
     * brief: register remote object with the NFC service
     * parameter:
     *   elementName -- card emulation service name
     *   apduChannel -- apdu channel object
     * return: 0 -- succeeded, not 0 -- error code
     */
    bool RegisterRemoteObject(int userId,
                              const OHOS::AppExecFwk::ElementName& elementName,
                              const OHOS::sptr<OHOS::IRemoteObject>& apduChannel);
    /**
     * brief: add aids for service
     * parameter:
     *   elementName -- card emulation service name
     *   aids -- aids
     *   type -- service type
     * return: true -- succeeded, false -- failed
     */
    bool AddAidsForService(const OHOS::AppExecFwk::ElementName& elementName,
                           std::vector<std::string> aids,
                           std::string type);
    /**
     * brief: remove aids for service
     * parameter:
     *   elementName -- card emulation service name
     *   type -- service type
     * return: true -- succeeded, false -- failed
     */
    bool RemoveAidsForService(const OHOS::AppExecFwk::ElementName& elementName, std::string type);
    /**
     * brief: register offhost service execution environment
     * parameter:
     *   elementName -- card emulation service name
     *   offHostSecureElement -- secure element name (execution environment)
     * return: true -- succeeded, false -- failed
     */
    bool RegisterOffHostService(const OHOS::AppExecFwk::ElementName& elementName, std::string offHostSecureElement);
    /**
     * brief: unregister offhost service execution environment
     * parameter:
     *   elementName -- card emulation service name
     * return: true -- succeeded, false -- failed
     */
    bool UnregisterOffHostService(const OHOS::AppExecFwk::ElementName& elementName);
    /**
     * brief: whether the NCI supports prefix mode
     * parameter: void
     * return: true -- supported, false -- not supported
     */
    bool IsSupportedAidPrefixMode();
    /**
     * brief: get the service description
     * parameter:
     *   type -- service type
     * return: description
     */
    std::string GetDescriptionForPreferredService();
    /**
     * brief: set preferred foreground service
     * parameter:
     *   elementName -- card emulation service name
     * return: true -- succeeded, false -- failed
     */
    bool SetPreferredForegroundService(const OHOS::AppExecFwk::ElementName& elementName);
    /**
     * brief: unset preferred foreground service
     * parameter: void
     * return: true -- succeeded, false -- failed
     */
    bool UnsetPreferredForegroundService();
    /**
     * brief: get primary service by type
     * parameter:
     *   type -- service type
     * return: card emulation service name
     */
    std::string GetPrimaryServiceForType(std::string type);
    /**
     * brief: set primary service by type
     * parameter:
     *   elementName -- card emulation service name
     *   type -- service type
     * return: true -- succeeded, false -- failed
     */
    bool SetPrimaryServiceForType(const OHOS::AppExecFwk::ElementName& elementName, std::string type);

private:
    explicit CardEmulation(const OHOS::wptr<ICardEmulationAgent>& ces);

    int GetCurrentUserId() const;

private:
    OHOS::wptr<ICardEmulationAgent> ces_;
};
}  // namespace cardemulation
}  // namespace nfc::sdk
}  // namespace OHOS
#endif
