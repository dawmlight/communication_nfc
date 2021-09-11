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

#include "card_emulation_agent.h"

#include <algorithm>
#include <cstdio>

#include "card_emulation_error.h"
#include "card_emulation_service_info.h"
#include "card_emulation_service_info_lite.h"
#include "element_name.h"
#include "icard_emulation_service.h"

namespace OHOS::nfc::cardemulation {
CardEmulationAgent::CardEmulationAgent(std::shared_ptr<nfc::cardemulation::ICardEmulationService> ceService)
    : ceService_(ceService)
{
}
CardEmulationAgent::~CardEmulationAgent()
{
}
int CardEmulationAgent::GetServices(
    int userId,
    const std::string& type,
    std::vector<std::unique_ptr<OHOS::nfc::sdk::cardemulation::CardEmulationServiceInfoLite>>& result)
{
    if (ceService_) {
        result = ceService_->GetServicesByType(userId, type);

        return ERR_OK;
    }

    return ERR_CARD_EMULATION_SERVICE_NOT_INIT;
}
int CardEmulationAgent::GetAidsForService(int userId,
                                          const OHOS::AppExecFwk::ElementName& elementName,
                                          const std::string& type,
                                          std::vector<std::string>& result)
{
    if (ceService_) {
        result = ceService_->GetAidsForService(userId, elementName, type);
        return ERR_OK;
    }
    return ERR_CARD_EMULATION_SERVICE_NOT_INIT;
}

int CardEmulationAgent::RegisterRemoteObject(int userId,
                                             const OHOS::AppExecFwk::ElementName& elementName,
                                             const OHOS::sptr<OHOS::IRemoteObject>& apduChannel)
{
    if (ceService_) {
        return ceService_->RegisterRemoteObject(userId, elementName, apduChannel);
    }
    return ERR_CARD_EMULATION_SERVICE_NOT_INIT;
}
int CardEmulationAgent::AddAidsForService(int userId,
                                          const OHOS::AppExecFwk::ElementName& elementName,
                                          std::vector<std::string> aids,
                                          std::string type)
{
    if (ceService_) {
        return ceService_->AddAidsForService(userId, elementName, type, aids);
    }
    return ERR_CARD_EMULATION_SERVICE_NOT_INIT;
}
int CardEmulationAgent::RemoveAidsForService(int userId,
                                             const OHOS::AppExecFwk::ElementName& elementName,
                                             std::string type)
{
    if (ceService_) {
        return (ceService_->RemoveAidsForService(userId, elementName, type));
    }
    return ERR_CARD_EMULATION_SERVICE_NOT_INIT;
}
int CardEmulationAgent::RegisterOffHostService(int userId,
                                               const OHOS::AppExecFwk::ElementName& elementName,
                                               std::string offHostSecureElement)
{
    if (ceService_) {
        return (ceService_->MarkOffHostForService(userId, elementName, offHostSecureElement));
    }
    return ERR_CARD_EMULATION_SERVICE_NOT_INIT;
}
int CardEmulationAgent::UnregisterOffHostService(int userId, const OHOS::AppExecFwk::ElementName& elementName)
{
    if (ceService_) {
        return (ceService_->UnmarkOffHostForService(userId, elementName));
    }
    return ERR_CARD_EMULATION_SERVICE_NOT_INIT;
}
int CardEmulationAgent::IsSupportedAidPrefixMode(bool& result)
{
    if (ceService_) {
        result = ceService_->SupportsAidPrefix();
        return ERR_OK;
    }
    return ERR_CARD_EMULATION_SERVICE_NOT_INIT;
}
int CardEmulationAgent::GetDescriptionForPreferredService(int userId, std::string type, std::string& result)
{
    if (ceService_) {
        result = ceService_->GetDescriptionForPreferredService(userId, type);
        return ERR_OK;
    }
    return ERR_CARD_EMULATION_SERVICE_NOT_INIT;
}
int CardEmulationAgent::SetPreferredForegroundService(int userId, const OHOS::AppExecFwk::ElementName& elementName)
{
    if (ceService_) {
        return ceService_->SetPreferredService(userId, elementName);
    }
    return ERR_CARD_EMULATION_SERVICE_NOT_INIT;
}
int CardEmulationAgent::UnsetPreferredForegroundService(int userId)
{
    if (ceService_) {
        return ceService_->UnsetPreferredService(userId);
    }
    return ERR_CARD_EMULATION_SERVICE_NOT_INIT;
}
int CardEmulationAgent::GetPrimaryServiceForType(int userId,
                                                 const std::string& type,
                                                 OHOS::AppExecFwk::ElementName& result)
{
    if (ceService_) {
        result = ceService_->GetPrimaryServiceForType(userId, type);
        return ERR_OK;
    }
    return ERR_CARD_EMULATION_SERVICE_NOT_INIT;
}
int CardEmulationAgent::SetPrimaryServiceForType(int userId,
                                                 const OHOS::AppExecFwk::ElementName& elementName,
                                                 const std::string& type)
{
    if (ceService_) {
        return ceService_->SetPrimaryServiceForType(userId, elementName, type);
    }
    return ERR_CARD_EMULATION_SERVICE_NOT_INIT;
}
}  // namespace OHOS::nfc::cardemulation
