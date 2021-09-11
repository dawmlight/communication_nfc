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

#include "offhost_card_emulation.h"

#include "icard_emulation_agent.h"
#include "ipc_skeleton.h"
#include "loghelper.h"
#include "nfc_sdk_common.h"

namespace OHOS::nfc::sdk::cardemulation {
OffHostCardEmulation::OffHostCardEmulation(const OHOS::AppExecFwk::ElementName& serviceName,
                                           const OHOS::wptr<ICardEmulationAgent>& ces)
    : mu_(),
    ces_(ces),
    name_(serviceName)
{
}
bool OffHostCardEmulation::AddAidsForService(const std::vector<std::string>& aids, std::string cardemulationServiceType)
{
    auto ces = ces_.promote();
    if (!ces) {
        ErrorLog("errno: %d", NfcErrorCode::NFC_SDK_ERROR_CARDEMULATION_AGENT_NOT_INITIALIZED);
        return false;

        return NfcErrorCode::NFC_SDK_ERROR_CARDEMULATION_AGENT_NOT_INITIALIZED;
    }
    int userId = OHOS::IPCSkeleton::GetCallingUid();
    auto rv = ces->AddAidsForService(userId, GetName(), aids, std::move(cardemulationServiceType));
    DebugLog("service return: %d", rv);
    return rv == NFC_SUCCESS;
}
bool OffHostCardEmulation::RemoveAidsForService(std::string cardemulationServiceType)
{
    auto ces = ces_.promote();
    if (!ces) {
        ErrorLog("errno: %d", NfcErrorCode::NFC_SDK_ERROR_CARDEMULATION_AGENT_NOT_INITIALIZED);
        return false;

        return NfcErrorCode::NFC_SDK_ERROR_CARDEMULATION_AGENT_NOT_INITIALIZED;
    }
    int userId = OHOS::IPCSkeleton::GetCallingUid();
    auto rv = ces->RemoveAidsForService(userId, GetName(), std::move(cardemulationServiceType));
    DebugLog("service return: %d", rv);
    return rv == NFC_SUCCESS;
}
bool OffHostCardEmulation::RegisterOffHostService(std::string offHostSecureElement)
{
    auto ces = ces_.promote();
    if (!ces) {
        ErrorLog("errno: %d", NfcErrorCode::NFC_SDK_ERROR_CARDEMULATION_AGENT_NOT_INITIALIZED);
        return false;
    }
    int userId = OHOS::IPCSkeleton::GetCallingUid();
    auto rv = ces->RegisterOffHostService(userId, GetName(), offHostSecureElement);
    DebugLog("service return: %d", rv);
    return rv == NFC_SUCCESS;
}
bool OffHostCardEmulation::UnregisterOffHostService()
{
    auto ces = ces_.promote();
    if (!ces) {
        ErrorLog("errno: %d", NfcErrorCode::NFC_SDK_ERROR_CARDEMULATION_AGENT_NOT_INITIALIZED);
        return false;
    }
    int userId = OHOS::IPCSkeleton::GetCallingUid();
    auto rv = ces->UnregisterOffHostService(userId, GetName());
    DebugLog("service return: %d", rv);
    return rv == NFC_SUCCESS;
}

OHOS::AppExecFwk::ElementName OffHostCardEmulation::GetName() const
{
    return name_;
}
}  // namespace OHOS::nfc::sdk::cardemulation
