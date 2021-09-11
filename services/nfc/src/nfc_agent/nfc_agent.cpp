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

#include "nfc_agent.h"

#include "app_callback_skeleton.h"
#include "common_event_manager.h"
#include "nfc_adapter_utils.h"
#include "nfc_agent_adapter_utils.h"
#include "nfc_error.h"
#include "nfc_log.h"
#include "nfc_manager_service.h"
#include "nfc_utils.h"
#include "profile_parcel.h"

namespace OHOS {
namespace Nfc {
const std::u16string NFC_CONTROLLER_TOKEN = u"ohos.nfc.INfcController";
const std::string WRITE_SECURE_SETTINGS = "ohos.permission.MANAGE_SECURE_SETTINGS";
const std::string PKG_ADAPTER = "ohos.nfc.INfcAdapter";

NfcAgent NfcAgent::g_nfcAgent;

NfcAgent::~NfcAgent() = default;

NfcAgent& NfcAgent::GetInstance()
{
    return g_nfcAgent;
}

int32_t NfcAgent::SetNfcEnabled(MessageParcel& data, MessageParcel& reply)
{
    HILOGI("NfcAgent::SetNfcEnabled Start");
    std::string remoteDesc = Str16ToStr8(data.ReadString16());
    std::string validDesc = Str16ToStr8(NFC_CONTROLLER_TOKEN);
    CHECK_REMOTE_DESC(remoteDesc, validDesc);
    CHECK_PERMISSON(WRITE_SECURE_SETTINGS, IPCSkeleton::GetCallingUid());

    int enabled = data.ReadInt32();
    HILOGI("NfcAgent::setNfcEnabled enabled = %{public}d", enabled);

    g_remoteNfcService = NfcManagerService::GetInstance()->GetRemoteNfcService();
    if (g_remoteNfcService == nullptr) {
        return Nfc::ERR_REMOTE_SERVICE_DISCONNECTED;
    }
    MessageParcel dataToNfcService;
    MessageParcel replyFromNfcService;
    MessageOption option;
    WRITE_INTF_TOKEN(StringReplace(PKG_ADAPTER), dataToNfcService);
    HILOGI("NfcAgent::TRANSACTION_ENABLE/DISABLE start!");
    int32_t error;
    if (enabled) {
        error = g_remoteNfcService->SendRequest(TRANSACTION_ENABLE, dataToNfcService, replyFromNfcService, option);
    } else {
        error = g_remoteNfcService->SendRequest(TRANSACTION_DISABLE, dataToNfcService, replyFromNfcService, option);
    }
    HILOGI("NfcAgent::TRANSACTION_ENABLE/DISABLE end!");
    if (error != Nfc::ERR_NONE) {
        HILOGE("NfcAgent::TRANSACTION_ENABLE/DISABLE, fail. error: %{public}d", error);
        return Nfc::ERR_REMOTE_TRANSACT_FAILED;
    }

    /* Consume the exception code */
    int32_t exception = replyFromNfcService.ReadInt32();
    if (exception != Nfc::ERR_NONE) {
        HILOGE("NfcAgent::TRANSACTION_ENABLE, fail. exception: %{public}d", exception);
        return Nfc::ERR_REMOTE_EXCEPTION_RETURN;
    }

    /* get the return value from NFC service API. 1 means enable/disable success. */
    int32_t ret = replyFromNfcService.ReadInt32();
    HILOGI("NfcAgent::TRANSACTION_ENABLE/DISABLE ret: %{public}d", ret);
    return ret;
}


int32_t NfcAgent::GetNfcState(MessageParcel& data, MessageParcel& reply)
{
    HILOGI("NfcAgent::GetNfcState function start!");
    std::string remoteDesc = Str16ToStr8(data.ReadString16());
    std::string validDesc = Str16ToStr8(NFC_CONTROLLER_TOKEN);
    CHECK_REMOTE_DESC(remoteDesc, validDesc);
    g_remoteNfcService = NfcManagerService::GetInstance()->GetRemoteNfcService();
    if (g_remoteNfcService == nullptr) {
        return Nfc::ERR_REMOTE_SERVICE_DISCONNECTED;
    }

    MessageParcel dataToNfcService;
    MessageParcel replyFromNfcService;
    MessageOption option;
    WRITE_INTF_TOKEN(StringReplace(PKG_ADAPTER), dataToNfcService);

    HILOGI("NfcAgent::Transact == GET_NFC_STATE start!");
    int32_t error = g_remoteNfcService->SendRequest(TRANSACTION_GET_STATE,
        dataToNfcService, replyFromNfcService, option);
    HILOGI("NfcAgent::Transact == GET_NFC_STATE end!");
    if (error != Nfc::ERR_NONE) {
        HILOGE("NfcAgent::TRANSACTION_GET_STATE. error: %{public}d", error);
        return Nfc::ERR_REMOTE_TRANSACT_FAILED;
    }

    /* Consume the exception code */
    int32_t exception = replyFromNfcService.ReadInt32();
    if (exception != Nfc::ERR_NONE) {
        HILOGE("NfcAgent::TRANSACTION_GET_STATE. exception: %{public}d", exception);
        return Nfc::ERR_REMOTE_EXCEPTION_RETURN;
    }

    /* get the return value from NFC service API. the NFC state value. */
    int32_t ret = replyFromNfcService.ReadInt32();
    HILOGI("NfcAgent::TRANSACTION_GET_STATE, ret: %{public}d", ret);
    return ret;
}

int32_t NfcAgent::IsNfcAvailable(MessageParcel& data, MessageParcel& reply)
{
    HILOGI("NfcAgent::IsNfcAvailable start");
    std::string remoteDesc = Str16ToStr8(data.ReadString16());
    std::string validDesc = Str16ToStr8(NFC_CONTROLLER_TOKEN);
    CHECK_REMOTE_DESC(remoteDesc, validDesc);
    g_remoteNfcService = NfcManagerService::GetInstance()->GetRemoteNfcService();
    if (g_remoteNfcService == nullptr) {
        return Nfc::ERR_REMOTE_SERVICE_DISCONNECTED;
    }
    return Nfc::ERR_NONE;
}

void WriteSkills(MessageParcel& data, MessageParcel& reply)
{
    int size = reply.ReadInt32();
    for (int i = 0; i < size; ++i) {
        std::string actionName = reply.ReadString();
        std::string categoryName = reply.ReadString();
        sptr<AAFwk::Skills> skill = new AAFwk::Skills();
        if (skill == nullptr) {
            HILOGE("Failed to malloc memory for Skills.");
            return;
        }
        skill->AddAction(actionName);
        skill->AddEntity(categoryName);
        data.WriteParcelable(skill);
    }
}

int32_t SetupProfileParcel(const MessageParcel& data, const sptr<ProfileParcel>& result)
{
    return 0;
}

int32_t NfcAgent::RegisterForegroundDispatch(MessageParcel& data, MessageParcel& reply)
{
    HILOGI("NfcAgent::RegisterForegroundDispatch function start!");
    std::string remoteDesc = Str16ToStr8(data.ReadString16());
    std::string validDesc = Str16ToStr8(NFC_CONTROLLER_TOKEN);
    CHECK_REMOTE_DESC(remoteDesc, validDesc);
    g_remoteNfcService = NfcManagerService::GetInstance()->GetRemoteNfcService();
    if (g_remoteNfcService == nullptr) {
        return Nfc::ERR_REMOTE_SERVICE_DISCONNECTED;
    }

    MessageParcel replyFromNfcService;
    MessageOption option;
    WriteSkills(data, reply);
    sptr<ProfileParcel> profileParcel;
    int32_t err = SetupProfileParcel(reply, profileParcel);

    // 0 means success
    if (err == 0) {
        data.WriteParcelable(profileParcel);
    }
    HILOGI("NfcAgent::Transact == SET_FOREGROUND_DISPATCH start!");
    int32_t error = g_remoteNfcService->SendRequest(TRANSACTION_SET_FOREGROUND_DISPATCH,
        data, replyFromNfcService, option);
    HILOGI("NfcAgent::Transact == SET_FOREGROUND_DISPATCH end!");
    if (error != Nfc::ERR_NONE) {
        HILOGE("NfcAgent::TRANSACTION_SET_FOREGROUND_DISPATCH. error: %{public}d", error);
        return Nfc::ERR_REMOTE_TRANSACT_FAILED;
    }
    /* Consume the exception code */
    int32_t exception = replyFromNfcService.ReadInt32();
    if (exception != Nfc::ERR_NONE) {
        HILOGE("NfcAgent::TRANSACTION_SET_FOREGROUND_DISPATCH. exception: %{public}d", exception);
        return Nfc::ERR_REMOTE_EXCEPTION_RETURN;
    }
    return Nfc::ERR_NONE;
}

int32_t NfcAgent::SetReaderMode(MessageParcel& data, MessageParcel& reply)
{
    HILOGI("NfcAgent::SetReaderMode function start!");
    std::string remoteDesc = Str16ToStr8(data.ReadString16());
    std::string validDesc = Str16ToStr8(NFC_CONTROLLER_TOKEN);
    CHECK_REMOTE_DESC(remoteDesc, validDesc);
    g_remoteNfcService = NfcManagerService::GetInstance()->GetRemoteNfcService();
    if (g_remoteNfcService == nullptr) {
        return Nfc::ERR_REMOTE_SERVICE_DISCONNECTED;
    }

    MessageOption option;
    WRITE_INTF_TOKEN(StringReplace(PKG_ADAPTER), data);

    sptr<IRemoteObject> token = data.ReadRemoteObject();
    data.WriteRemoteObject(token);

    // insert the callback
    auto callback = std::make_unique<AppCallbackStub>();
    data.WriteRemoteObject(callback->AsObject());

    HILOGI("NfcAgent::Transact == SET_READER_MODE start!");
    int32_t error = g_remoteNfcService->SendRequest(TRANSACTION_SET_READER_MODE,
        data, reply, option);
    HILOGI("NfcAgent::Transact == SET_READER_MODE end!");
    if (error != Nfc::ERR_NONE) {
        HILOGE("NfcAgent::TRANSACTION_SET_READER_MODE. error: %{public}d", error);
        return Nfc::ERR_REMOTE_TRANSACT_FAILED;
    }

    /* Consume the exception code */
    int32_t exception = reply.ReadInt32();
    if (exception != Nfc::ERR_NONE) {
        HILOGE("NfcAgent::TRANSACTION_SET_READER_MODE. exception: %{public}d", exception);
        return Nfc::ERR_REMOTE_EXCEPTION_RETURN;
    }

    /* get the return value */
    bool ret = reply.ReadBool();
    HILOGI("NfcAgent::TRANSACTION_SET_READER_MODE, ret: %{public}d", ret);
    return ret;
}
} // namespace Nfc
} // namespace OHOS
