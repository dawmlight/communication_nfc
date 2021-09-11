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

#include "host_card_emulation.h"

#include "icard_emulation_agent.h"
#include "ipc_skeleton.h"
#include "loghelper.h"
#include "nfc_sdk_common.h"
namespace OHOS::nfc::sdk::cardemulation {
HostCardEmulation::HostCardEmulation(const OHOS::AppExecFwk::ElementName& serviceName,
                                     const OHOS::wptr<ICardEmulationAgent>& ces)
    : IHostCardEmulationService(serviceName),
    mu_(),
    ces_(ces),
    name_(serviceName)
{
}
HostCardEmulation::~HostCardEmulation() = default;

bool HostCardEmulation::AddAidsForService(const std::vector<std::string>& aids, std::string cardemulationServiceType)
{
    auto ces = ces_.promote();
    if (!ces) {
        ErrorLog("errno: %d", NfcErrorCode::NFC_SDK_ERROR_CARDEMULATION_AGENT_NOT_INITIALIZED);
        return false;
    }
    int userId = OHOS::IPCSkeleton::GetCallingUid();
    RegisterRemoteObject(ces, userId);
    DebugLog("userId: %d, ces pointer: %p", userId, ces.GetRefPtr());

    auto rv = ces->AddAidsForService(userId, GetName(), aids, std::move(cardemulationServiceType));
    DebugLog("service return: %d", rv);
    return rv == NFC_SUCCESS;
}

bool HostCardEmulation::RemoveAidsForService(std::string cardemulationServiceType)
{
    auto ces = ces_.promote();
    if (!ces) {
        ErrorLog("errno: %d", NfcErrorCode::NFC_SDK_ERROR_CARDEMULATION_AGENT_NOT_INITIALIZED);
        return false;
    }
    int userId = OHOS::IPCSkeleton::GetCallingUid();
    RegisterRemoteObject(ces, userId);
    DebugLog("userId: %d, ces pointer: %p", userId, ces.GetRefPtr());
    auto rv = ces->RemoveAidsForService(userId, GetName(), std::move(cardemulationServiceType));

    DebugLog("service return: %d", rv);
    return rv == NFC_SUCCESS;
}

bool HostCardEmulation::SetApduCommandCallback(ApduCommandHandler handler)
{
    RegisterRemoteObject();
    std::lock_guard<std::mutex> lock(mu_);

    apduCommandHandler_ = std::move(handler);
    return true;
}

bool HostCardEmulation::SetDeactiveEventCallback(DeactiveEventHandler handler)
{
    RegisterRemoteObject();
    std::lock_guard<std::mutex> lock(mu_);
    deactiveEventHandler_ = std::move(handler);
    return true;
}

std::vector<unsigned char> HostCardEmulation::HandleApdu(const std::vector<unsigned char>& apduCommand)
{
    std::lock_guard<std::mutex> lock(mu_);
    if (apduCommandHandler_) {
        return apduCommandHandler_(apduCommand);
    }
    return {};
}

void HostCardEmulation::OnDeactivated(int reason)
{
    std::lock_guard<std::mutex> lock(mu_);
    DebugLog("OnDeactivated: %d, deactiveEventHandler_ is %s callable", reason, deactiveEventHandler_ ? "" : "not");

    if (deactiveEventHandler_) {
        DebugLog("OnDeactivated: %d", reason);
        deactiveEventHandler_(reason);
    }
}

OHOS::AppExecFwk::ElementName HostCardEmulation::GetName() const
{
    return name_;
}

bool HostCardEmulation::RegisterRemoteObject()
{
    if (channel_.GetRefPtr()) {
        return true;
    }
    auto ces = ces_.promote();
    if (!ces) {
        ErrorLog("errno: %d", NfcErrorCode::NFC_SDK_ERROR_CARDEMULATION_AGENT_NOT_INITIALIZED);
        return false;
    }

    int userId = OHOS::IPCSkeleton::GetCallingUid();

    return RegisterRemoteObject(ces, userId);
}

bool HostCardEmulation::RegisterRemoteObject(const OHOS::sptr<ICardEmulationAgent>& ces, int userId)
{
    if (channel_.GetRefPtr()) {
        return true;
    }
    auto ro = OnConnect(OHOS::AAFwk::Want());
    auto rv = ces->RegisterRemoteObject(userId, GetName(), ro);
    channel_ = ro;
    DebugLog("RegisterRemoteObject return: %d", rv);
    return rv == NFC_SUCCESS;
}
}  // namespace OHOS::nfc::sdk::cardemulation
