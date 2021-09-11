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
    static const std::string TYPE_SECURE;
    static const std::string TYPE_OTHER;

    static std::shared_ptr<CardEmulation> GetInstance(std::shared_ptr<NfcAgent>);

    std::vector<std::unique_ptr<CardEmulationServiceInfoLite>> GetServices(const std::string& type);

    std::vector<std::string> GetAidsForService(const OHOS::AppExecFwk::ElementName& elementName,
                                               const std::string& type);
    bool RegisterRemoteObject(int userId,
                              const OHOS::AppExecFwk::ElementName& elementName,
                              const OHOS::sptr<OHOS::IRemoteObject>& apduChannel);

    bool AddAidsForService(const OHOS::AppExecFwk::ElementName& elementName,
                           std::vector<std::string> aids,
                           std::string type);
    bool RemoveAidsForService(const OHOS::AppExecFwk::ElementName& elementName, std::string type);
    bool RegisterOffHostService(const OHOS::AppExecFwk::ElementName& elementName, std::string offHostSecureElement);
    bool UnregisterOffHostService(const OHOS::AppExecFwk::ElementName& elementName);
    bool IsSupportedAidPrefixMode();
    std::string GetDescriptionForPreferredService();
    bool SetPreferredForegroundService(const OHOS::AppExecFwk::ElementName& elementName);
    bool UnsetPreferredForegroundService();
    std::string GetPrimaryServiceForType(std::string type);
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
