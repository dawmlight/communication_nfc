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
#include "nfc_adapter_utils.h"
#include "nfc_cardemulation_adapter_utils.h"
#include "nfc_error.h"
#include "nfc_log.h"
#include "nfc_manager_service.h"
#include "nfc_utils.h"

namespace OHOS {
namespace Nfc {
namespace Card {
const std::string PKG_ADAPTER = "ohos.nfc.INfcAdapter";

const std::string PERM_CARD_EMULATION = "ohos.permission.NFC_CARD_EMULATION";
const std::string PERM_TRANSACTION_EVENT = "ohos.permission.RCV_NFC_TRANSACTION_EVENT";
const std::u16string CARD_EMULATION_TOKEN = u"ohos.nfc.cardemulation.ICardEmulation";

CardEmulation CardEmulation::g_cardEmulation;

CardEmulation::~CardEmulation() = default;

CardEmulation& CardEmulation::GetInstance()
{
    return g_cardEmulation;
}

/* get NFC card emulation service */
sptr<IRemoteObject> CardEmulation::GetCardEmulationService()
{
    if (g_remoteCEService != nullptr) {
        return g_remoteCEService;
    }

    sptr<IRemoteObject> remoteNfcService_ = NfcManagerService::GetInstance()->GetRemoteNfcService();
    if (remoteNfcService_ == nullptr) {
        return nullptr;
    }

    // Send TRANSACTION
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    WRITE_INTF_TOKEN_NULL(StringReplace(PKG_ADAPTER), data);
    HILOGI("CardEmulation::Transact == TRANSACTION_getNfcCardEmulationInterface start!");
    int32_t error = remoteNfcService_->SendRequest(TRANS_GET_CARD_EMU_INTERFACE,
        data, reply, option);
    HILOGI("CardEmulation::Transact == TRANSACTION_getNfcCardEmulationInterface end!");
    if (error != Nfc::ERR_NONE) {
        HILOGE("CardEmulation::NfcService_->Transact, fail. error: %{public}d", error);
        return nullptr;
    }

    // Consume the exception code
    int32_t exception = reply.ReadInt32();
    if (exception != Nfc::ERR_NONE) {
        HILOGE("CardEmulation::GET_NFCCARDEMULATIONINTERFACE. exception: %{public}d", exception);
        return nullptr;
    }

    // Return the exec result
    g_remoteCEService = reply.ReadObject<IRemoteObject>();
    return g_remoteCEService;
}

int32_t CardEmulation::SetListenMode(MessageParcel& data, MessageParcel& reply)
{
    HILOGI("CardEmulation::SetListenMode begin");
    CHECK_PERMISSON(PERM_CARD_EMULATION, IPCSkeleton::GetCallingUid());
    g_remoteNxpService = NfcManagerService::GetInstance()->GetRemoteNfcService();
    if (g_remoteNxpService == nullptr) {
        return Nfc::ERR_REMOTE_SERVICE_DISCONNECTED;
    }

    // SendRequest start
    MessageOption option;
    HILOGI("CardEmulation::SetListenMode SendRequest start!");
    int32_t error = g_remoteNxpService->SendRequest(SET_LISTENTECHMASK, data, reply, option);
    HILOGI("CardEmulation::SetListenMode SendRequest end!");
    if (error != Nfc::ERR_NONE) {
        HILOGE("CardEmulation::SetListenMode NxpService_->SendRequest, call SetListenMode fail. error: %d", error);
        return Nfc::ERR_REMOTE_TRANSACT_FAILED;
    }
    HILOGI("CardEmulation::SetListenMode NxpService_->SendRequest, Succeed!");

    /* Consume the exception code */
    int errorCode = reply.ReadInt32();
    if (errorCode != Nfc::ERR_NONE) {
        HILOGE("CardEmulation::SetListenMode Get Response from SendRequest Fail. error: %{public}d", errorCode);
        return Nfc::ERR_REMOTE_EXCEPTION_RETURN;
    }
    return Nfc::ERR_NONE;
}

int32_t CardEmulation::IsListenModeEnabled(MessageParcel& data, MessageParcel& reply)
{
    HILOGI("CardEmulation::IsListenModeEnabled begin");
    CHECK_PERMISSON(PERM_CARD_EMULATION, IPCSkeleton::GetCallingUid());
    g_remoteNxpService = NfcManagerService::GetInstance()->GetRemoteNfcService();
    if (g_remoteNxpService == nullptr) {
        return Nfc::ERR_REMOTE_SERVICE_DISCONNECTED;
    }

    // SendRequest start
    MessageOption option;
    HILOGI("CardEmulation::IsListenModeEnabled SendRequest start!");
    int32_t error = g_remoteNxpService->SendRequest(IS_LISTENTRCHMASK, data, reply, option);
    HILOGI("CardEmulation::IsListenModeEnabled SendRequest end!");
    if (error != Nfc::ERR_NONE) {
        HILOGE("CardEmulation::IsListenModeEnabled NxpService_->SendRequest, call IsLM fail. error: %d", error);
        return Nfc::ERR_REMOTE_TRANSACT_FAILED;
    }
    HILOGI("CardEmulation::IsListenModeEnabled NxpService_->SendRequest, Succeed!");

    // Consume the exception code
    int errorCode = reply.ReadInt32();
    if (errorCode != Nfc::ERR_NONE) {
        HILOGE("CardEmulation::IsListenModeEnabled Get Response from SendRequest Fail. error: %d", errorCode);
        return Nfc::ERR_REMOTE_EXCEPTION_RETURN;
    }
    return Nfc::ERR_NONE;
}

int32_t CardEmulation::GetNfcInfo(MessageParcel& data, MessageParcel& reply)
{
    HILOGI("CardEmulation::GetNfcInfo begin");
    CHECK_PERMISSON(PERM_CARD_EMULATION, IPCSkeleton::GetCallingUid());
    g_remoteNxpService = NfcManagerService::GetInstance()->GetRemoteNfcService();
    if (g_remoteNxpService == nullptr) {
        return Nfc::ERR_REMOTE_SERVICE_DISCONNECTED;
    }

    // SendRequest start
    MessageOption option;
    HILOGI("CardEmulation::GetNfcInfo SendRequest start!");
    int32_t error = g_remoteNxpService->SendRequest(GET_NFC_INFO, data, reply, option);
    HILOGI("CardEmulation::GetNfcInfo SendRequest end!");
    if (error != Nfc::ERR_NONE) {
        HILOGE("CardEmulation::GetNfcInfo NxpService_->SendRequest, call GetNfcInfo. error: %{public}d", error);
        return Nfc::ERR_REMOTE_TRANSACT_FAILED;
    }
    HILOGI("CardEmulation::GetNfcInfo NxpService_->SendRequest, Succeed!");

    // Consume the exception code
    int errorCode = reply.ReadInt32();
    if (errorCode != Nfc::ERR_NONE) {
        HILOGE("CardEmulation::GetNfcInfo Get Response from SendRequest Fail. error: %{public}d", errorCode);
        return Nfc::ERR_REMOTE_EXCEPTION_RETURN;
    }
    return Nfc::ERR_NONE;
}

int32_t CardEmulation::SetRfConfig(MessageParcel& data, MessageParcel& reply)
{
    HILOGI("CardEmulation::SetRfConfig begin");
    CHECK_PERMISSON(PERM_CARD_EMULATION, IPCSkeleton::GetCallingUid());
    g_remoteNxpService = NfcManagerService::GetInstance()->GetRemoteNfcService();
    if (g_remoteNxpService == nullptr) {
        return Nfc::ERR_REMOTE_SERVICE_DISCONNECTED;
    }

    // SendRequest start
    MessageOption option;
    HILOGI("CardEmulation::SetRfConfig SendRequest start!");
    int32_t error = g_remoteNxpService->SendRequest(SET_CONFIG, data, reply, option);
    HILOGI("CardEmulation::SetRfConfig SendRequest end!");
    if (error != Nfc::ERR_NONE) {
        HILOGE("CardEmulation::SetRfConfig NxpService_->SendRequest, call SetRfConfig. error: %{public}d", error);
        return Nfc::ERR_REMOTE_TRANSACT_FAILED;
    }
    HILOGI("CardEmulation::SetRfConfig NxpService_->SendRequest, Succeed!");

    // Consume the exception code
    int errorCode = reply.ReadInt32();
    if (errorCode != Nfc::ERR_NONE) {
        HILOGE("CardEmulation::SetRfConfig Get Response from SendRequest Fail. error: %{public}d", errorCode);
        return Nfc::ERR_REMOTE_EXCEPTION_RETURN;
    }
    return Nfc::ERR_NONE;
}

/* get information from config file */
int32_t CardEmulation::GetInfoFromConfig(MessageParcel& data, MessageParcel& reply)
{
    HILOGI("CardEmulation::GetInfoFromConfig start!");
    CHECK_PERMISSON(PERM_CARD_EMULATION, IPCSkeleton::GetCallingUid());
    g_remoteNxpService = NfcManagerService::GetInstance()->GetRemoteNfcService();
    if (g_remoteNxpService == nullptr) {
        return Nfc::ERR_REMOTE_SERVICE_DISCONNECTED;
    }

    MessageOption option;
    HILOGI("CardEmulation::GetInfoFromConfig start transact");
    int32_t error = g_remoteNxpService->SendRequest(GET_INFO_FROM_CONFIG, data, reply, option);
    HILOGI("CardEmulation::GetInfoFromConfig finish transact");
    if (error != Nfc::ERR_NONE) {
        HILOGI("CardEmulation::GET_INFO_FROM_CONFIG, fail. error: %{public}d", error);
        return Nfc::ERR_REMOTE_TRANSACT_FAILED;
    }

    // Consume the exception code
    int32_t exception = reply.ReadInt32();
    if (exception != Nfc::ERR_NONE) {
        HILOGI("CardEmulation::GET_INFO_FROM_CONFIG. exception: %{public}d", exception);
        return Nfc::ERR_REMOTE_EXCEPTION_RETURN;
    }
    return Nfc::ERR_NONE;
}

/* set foreground preferred */
int32_t CardEmulation::SetForegroundPreferred(MessageParcel& data, MessageParcel& reply)
{
    HILOGI("CardEmulation::SetForegroundPreferred start!");
    CHECK_PERMISSON(PERM_CARD_EMULATION, IPCSkeleton::GetCallingUid());
    g_remoteCEService = GetCardEmulationService();
    if (g_remoteCEService == nullptr) {
        HILOGI("CardEmulation::SetForegroundPreferred GetCardEmulationService == nullptr");
        return Nfc::ERR_REMOTE_SERVICE_DISCONNECTED;
    }

    MessageOption option;
    HILOGI("CardEmulation::SetForegroundPreferred start transact");
    int32_t error = g_remoteCEService->SendRequest(SET_FOREGROUND_SERVICE, data, reply, option);
    HILOGI("CardEmulation::SetForegroundPreferred finish transact");
    if (error != Nfc::ERR_NONE) {
        HILOGI("CardEmulation::SET_FOREGROUND_SERVICE, fail. error: %{public}d", error);
        return Nfc::ERR_REMOTE_TRANSACT_FAILED;
    }

    // Consume the exception code
    int32_t exception = reply.ReadInt32();
    if (exception != Nfc::ERR_NONE) {
        HILOGI("CardEmulation::SET_FOREGROUND_SERVICE. exception: %{public}d", exception);
        return Nfc::ERR_REMOTE_EXCEPTION_RETURN;
    }
    return Nfc::ERR_NONE;
}

/* unset foreground preferred */
int32_t CardEmulation::UnsetForegroundPreferred(MessageParcel& data, MessageParcel& reply)
{
    HILOGI("CardEmulation::UnsetForegroundPreferred start!");
    CHECK_PERMISSON(PERM_CARD_EMULATION, IPCSkeleton::GetCallingUid());
    g_remoteCEService = GetCardEmulationService();
    if (g_remoteCEService == nullptr) {
        HILOGI("CardEmulation::SetForegroundPreferred GetCardEmulationService == nullptr");
        return Nfc::ERR_REMOTE_SERVICE_DISCONNECTED;
    }

    MessageOption option;
    HILOGI("CardEmulation::UnsetForegroundPreferred start transact");
    int32_t error = g_remoteCEService->SendRequest(UNSET_FOREGROUND_SERVICE, data, reply, option);
    HILOGI("CardEmulation::UnsetForegroundPreferred finish transact");
    if (error != Nfc::ERR_NONE) {
        HILOGI("CardEmulation::UNSET_FOREGROUND_SERVICE, fail. error: %{public}d", error);
        return Nfc::ERR_REMOTE_TRANSACT_FAILED;
    }

    // Consume the exception code
    int32_t exception = reply.ReadInt32();
    if (exception != Nfc::ERR_NONE) {
        HILOGI("CardEmulation::UNSET_FOREGROUND_SERVICE. exception: %{public}d", exception);
        return Nfc::ERR_REMOTE_EXCEPTION_RETURN;
    }
    return Nfc::ERR_NONE;
}

int32_t CardEmulation::IsDefaultForType(MessageParcel& data, MessageParcel& reply)
{
    HILOGI("CardEmulation::IsDefaultForType start!");
    CHECK_PERMISSON(PERM_CARD_EMULATION, IPCSkeleton::GetCallingUid());
    g_remoteNfcService = NfcManagerService::GetInstance()->GetRemoteNfcService();
    if (g_remoteNfcService == nullptr) {
        HILOGI("CardEmulation::IsDefaultForType GetNfcService == nullptr");
        return Nfc::ERR_REMOTE_SERVICE_DISCONNECTED;
    }

    MessageOption option;
    HILOGI("CardEmulation::IsDefaultForType start transact");
    int32_t error = g_remoteNfcService->SendRequest(IS_DEFAULT_FOR_TYPE, data, reply, option);
    HILOGI("CardEmulation::IsDefaultForType finish transact");
    if (error != Nfc::ERR_NONE) {
        HILOGI("CardEmulation::IS_DEFAULT_FOR_TYPE. exception: %{public}d", error);
        return Nfc::ERR_REMOTE_TRANSACT_FAILED;
    }

    // Consume the exception code
    int32_t exception = reply.ReadInt32();
    if (exception != Nfc::ERR_NONE) {
        HILOGI("CardEmulation::IS_DEFAULT_FOR_TYPE. exception: %{public}d", exception);
        return Nfc::ERR_REMOTE_EXCEPTION_RETURN;
    }
    return Nfc::ERR_NONE;
}

int32_t CardEmulation::IsDefaultForAid(MessageParcel& data, MessageParcel& reply)
{
    HILOGI("CardEmulation::IsDefaultForAid start!");
    CHECK_PERMISSON(PERM_CARD_EMULATION, IPCSkeleton::GetCallingUid());
    g_remoteCEService = GetCardEmulationService();
    if (g_remoteCEService == nullptr) {
        HILOGI("CardEmulation::SetForegroundPreferred GetCardEmulationService == nullptr");
        return Nfc::ERR_REMOTE_SERVICE_DISCONNECTED;
    }

    MessageOption option;
    HILOGI("CardEmulation::IsDefaultForAid start transact");
    int32_t error = g_remoteCEService->SendRequest(IS_DEFAULT_FOR_AID, data, reply, option);
    HILOGI("CardEmulation::IsDefaultForAid finish transact");
    if (error != Nfc::ERR_NONE) {
        HILOGI("CardEmulation::IS_DEFAULT_FOR_AID. exception: %{public}d", error);
        return Nfc::ERR_REMOTE_TRANSACT_FAILED;
    }

    // Consume the exception code
    int32_t exception = reply.ReadInt32();
    if (exception != Nfc::ERR_NONE) {
        HILOGI("CardEmulation::IS_DEFAULT_FOR_AID. exception: %{public}d", exception);
        return Nfc::ERR_REMOTE_EXCEPTION_RETURN;
    }
    return Nfc::ERR_NONE;
}

int32_t CardEmulation::RegisterAids(MessageParcel& data, MessageParcel& reply)
{
    HILOGI("CardEmulation::RegisterAids start!");
    CHECK_PERMISSON(PERM_CARD_EMULATION, IPCSkeleton::GetCallingUid());
    g_remoteNfcService = NfcManagerService::GetInstance()->GetRemoteNfcService();
    if (g_remoteNfcService == nullptr) {
        return Nfc::ERR_REMOTE_SERVICE_DISCONNECTED;
    }

    MessageOption option;
    HILOGI("CardEmulation::RegisterAids start transact");
    int32_t error = g_remoteNfcService->SendRequest(REGISTER_AIDS, data, reply, option);
    HILOGI("CardEmulation::RegisterAids finish transact");
    if (error != Nfc::ERR_NONE) {
        HILOGI("CardEmulation::REGISTER_AIDS. exception: %{public}d", error);
        return Nfc::ERR_REMOTE_TRANSACT_FAILED;
    }

    // Consume the exception code
    int32_t exception = reply.ReadInt32();
    if (exception != Nfc::ERR_NONE) {
        HILOGI("CardEmulation::REGISTER_AIDS. exception: %{public}d", exception);
        return Nfc::ERR_REMOTE_EXCEPTION_RETURN;
    }
    return Nfc::ERR_NONE;
}

int32_t CardEmulation::RemoveAids(MessageParcel& data, MessageParcel& reply)
{
    HILOGI("CardEmulation::RemoveAids start!");
    CHECK_PERMISSON(PERM_CARD_EMULATION, IPCSkeleton::GetCallingUid());
    g_remoteCEService = GetCardEmulationService();
    if (g_remoteCEService == nullptr) {
        HILOGI("CardEmulation::SetForegroundPreferred GetCardEmulationService == nullptr");
        return Nfc::ERR_REMOTE_SERVICE_DISCONNECTED;
    }

    MessageOption option;
    HILOGI("CardEmulation::RemoveAids start transact");
    int32_t error = g_remoteCEService->SendRequest(REMOVE_AIDS, data, reply, option);
    HILOGI("CardEmulation::RemoveAids finish transact");
    if (error != Nfc::ERR_NONE) {
        HILOGI("CardEmulation::REMOVE_AIDS. exception: %{public}d", error);
        return Nfc::ERR_REMOTE_TRANSACT_FAILED;
    }

    // Consume the exception code
    int32_t exception = reply.ReadInt32();
    if (exception != Nfc::ERR_NONE) {
        HILOGI("CardEmulation::REMOVE_AIDS. exception: %{public}d", exception);
        return Nfc::ERR_REMOTE_EXCEPTION_RETURN;
    }
    return Nfc::ERR_NONE;
}

int32_t CardEmulation::GetAids(MessageParcel& data, MessageParcel& reply)
{
    HILOGI("CardEmulation::GetAids start!");
    CHECK_PERMISSON(PERM_CARD_EMULATION, IPCSkeleton::GetCallingUid());
    g_remoteCEService = GetCardEmulationService();
    if (g_remoteCEService == nullptr) {
        HILOGI("CardEmulation::GetAids GetCardEmulationService == nullptr");
        return Nfc::ERR_REMOTE_SERVICE_DISCONNECTED;
    }

    MessageOption option;
    HILOGI("CardEmulation::GetAids start transact");
    int32_t error = g_remoteCEService->SendRequest(GET_AIDS, data, reply, option);
    HILOGI("CardEmulation::GetAids finish transact");
    if (error != Nfc::ERR_NONE) {
        HILOGI("CardEmulation::GET_AIDS. exception: %{public}d", error);
        return Nfc::ERR_REMOTE_TRANSACT_FAILED;
    }

    // Consume the exception code
    int32_t exception = reply.ReadInt32();
    if (exception != Nfc::ERR_NONE) {
        HILOGI("CardEmulation::GET_AIDS. exception: %{public}d", exception);
        return Nfc::ERR_REMOTE_EXCEPTION_RETURN;
    }
    return Nfc::ERR_NONE;
}
} // namespace Card
} // namespace Nfc
} // namespace OHOS

