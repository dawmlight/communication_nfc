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

#include "card_emulation.h"

#include "card_emulation_service_info_lite.h"
#include "context.h"
#include "element_name.h"
#include "icard_emulation_agent.h"
#include "ipc_skeleton.h"
#include "iremote_object.h"
#include "loghelper.h"
#include "nfc_agent.h"
#include "nfc_sdk_common.h"

namespace OHOS::nfc::sdk {
namespace cardemulation {
const std::string CardEmulation::TYPE_SECURE = "secure";
const std::string CardEmulation::TYPE_NORMAL = "normal";

std::shared_ptr<CardEmulation> CardEmulation::GetInstance(std::shared_ptr<NfcAgent> nfcAgent)
{
    std::shared_ptr<CardEmulation> rv;
    if (!nfcAgent) {
        return rv;
    }
    if (!rv) {
        auto ces = nfcAgent->GetNfcCardEmulationService();
        rv = std::shared_ptr<CardEmulation>(new CardEmulation(ces));
    }
    return rv;
}
std::vector<std::unique_ptr<CardEmulationServiceInfoLite>> CardEmulation::GetServices(const std::string& type)
{
    std::vector<std::unique_ptr<CardEmulationServiceInfoLite>> rv;

    OHOS::sptr<ICardEmulationAgent> ces = ces_.promote();
    if (ces) {
        ces->GetServices(GetCurrentUserId(), type, rv);
    }

    return rv;
}
std::vector<std::string> CardEmulation::GetAidsForService(const OHOS::AppExecFwk::ElementName& elementName,
                                                          const std::string& type)
{
    OHOS::sptr<ICardEmulationAgent> ces = ces_.promote();
    if (ces) {
        std::vector<std::string> result;
        if (ces->GetAidsForService(GetCurrentUserId(), elementName, type, result) == NFC_SUCCESS) {
            return result;
        }
    }
    return std::vector<std::string>();
}
bool CardEmulation::RegisterRemoteObject(int userId,
                                         const OHOS::AppExecFwk::ElementName& elementName,
                                         const OHOS::sptr<OHOS::IRemoteObject>& apduChannel)
{
    OHOS::sptr<ICardEmulationAgent> ces = ces_.promote();
    if (ces) {
        return ces->RegisterRemoteObject(GetCurrentUserId(), elementName, apduChannel) == NFC_SUCCESS;
    }
    return false;
}
bool CardEmulation::AddAidsForService(const OHOS::AppExecFwk::ElementName& elementName,
                                      std::vector<std::string> aids,
                                      std::string type)
{
    OHOS::sptr<ICardEmulationAgent> ces = ces_.promote();
    if (ces) {
        return ces->AddAidsForService(GetCurrentUserId(), elementName, aids, type) == NFC_SUCCESS;
    }
    return false;
}
bool CardEmulation::RemoveAidsForService(const OHOS::AppExecFwk::ElementName& elementName, std::string type)
{
    OHOS::sptr<ICardEmulationAgent> ces = ces_.promote();
    if (ces) {
        return ces->RemoveAidsForService(GetCurrentUserId(), elementName, type) == NFC_SUCCESS;
    }
    return false;
}
bool CardEmulation::RegisterOffHostService(const OHOS::AppExecFwk::ElementName& elementName,
                                           std::string offHostSecureElement)
{
    OHOS::sptr<ICardEmulationAgent> ces = ces_.promote();
    if (ces) {
        return ces->RegisterOffHostService(GetCurrentUserId(), elementName, offHostSecureElement) == NFC_SUCCESS;
    }
    return false;
}
bool CardEmulation::UnregisterOffHostService(const OHOS::AppExecFwk::ElementName& elementName)
{
    OHOS::sptr<ICardEmulationAgent> ces = ces_.promote();
    if (ces) {
        return ces->UnregisterOffHostService(GetCurrentUserId(), elementName) == NFC_SUCCESS;
    }
    return false;
}
bool CardEmulation::IsSupportedAidPrefixMode()
{
    OHOS::sptr<ICardEmulationAgent> ces = ces_.promote();
    if (ces) {
        bool result;
        if (ces->IsSupportedAidPrefixMode(result) == NFC_SUCCESS) {
            return result;
        }
    }
    return false;
}
std::string CardEmulation::GetDescriptionForPreferredService()
{
    OHOS::sptr<ICardEmulationAgent> ces = ces_.promote();
    if (ces) {
        std::string result;
        if (ces->GetDescriptionForPreferredService(GetCurrentUserId(), TYPE_SECURE, result) == NFC_SUCCESS) {
            return result;
        }
    }
    return std::string();
}
bool CardEmulation::SetPreferredForegroundService(const OHOS::AppExecFwk::ElementName& elementName)
{
    OHOS::sptr<ICardEmulationAgent> ces = ces_.promote();
    if (ces) {
        return ces->SetPreferredForegroundService(GetCurrentUserId(), elementName) == NFC_SUCCESS;
    }
    return false;
}
bool CardEmulation::UnsetPreferredForegroundService()
{
    OHOS::sptr<ICardEmulationAgent> ces = ces_.promote();
    if (ces) {
        return ces->UnsetPreferredForegroundService(GetCurrentUserId()) == NFC_SUCCESS;
    }
    return false;
}
std::string CardEmulation::GetPrimaryServiceForType(std::string type)
{
    OHOS::sptr<ICardEmulationAgent> ces = ces_.promote();
    if (ces) {
        OHOS::AppExecFwk::ElementName elementName;
        ces->GetPrimaryServiceForType(GetCurrentUserId(), type, elementName);
        return elementName.GetAbilityName();
    }

    return std::string();
}
bool CardEmulation::SetPrimaryServiceForType(const OHOS::AppExecFwk::ElementName& elementName, std::string type)
{
    OHOS::sptr<ICardEmulationAgent> ces = ces_.promote();
    if (ces) {
        return ces->SetPrimaryServiceForType(GetCurrentUserId(), elementName, type) == NFC_SUCCESS;
    }
    return false;
}
CardEmulation::CardEmulation(const OHOS::wptr<ICardEmulationAgent>& ces) : ces_(ces)
{
}
int CardEmulation::GetCurrentUserId() const
{
    return OHOS::IPCSkeleton::GetCallingUid();
}
}  // namespace cardemulation
}  // namespace OHOS::nfc::sdk
