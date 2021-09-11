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
#include "nfc_service_stub.h"

#include "app_callback_proxy.h"
#include "context.h"
#include "itag_session.h"
#include "loghelper.h"
#include "nfc_map.h"
#include "nfc_permissions.h"
#include "nfc_sdk_common.h"

#ifdef _NFC_SERVICE_HCE_
#include "apdu_channel.h"
#include "icard_emulation_agent.h"
using namespace OHOS::nfc::sdk::cardemulation;
#endif

namespace OHOS {
namespace nfc {
using sdk::NfcErrorCode;

int NfcServiceStub::OnRemoteRequest(uint32_t code,         /* [in] */
                                    MessageParcel& data,   /* [in] */
                                    MessageParcel& reply,  /* [out] */
                                    MessageOption& option) /* [in] */
{
    DebugLog("OnRemoteRequest occur, code is %d", code);
    switch (code) {
        case COMMAND_GET_STATE:
            return HandleGetState(data, reply);
        case COMMAND_TURN_ON:
            return HandleTurnOn(data, reply);
        case COMMAND_TURN_OFF:
            return HandleTurnOff(data, reply);
        case COMMAND_PAUSE_POLLING:
            return HandlePausePolling(data, reply);
        case COMMAND_RESUME_POLLING:
            return HandleResumePolling(data, reply);
        case COMMAND_SET_NFC_SECURE:
            return HandleSetNfcSecure(data, reply);
        case COMMAND_DEVICE_SUPPORT_SECURE:
            return HandleDeviceSupportsNfcSecure(data, reply);
        case COMMAND_IS_SECURE_ENABLED:
            return HandleIsNfcSecureEnabled(data, reply);
        case COMMAND_SET_RW_MODE:
            return HandleSetRWMode(data, reply);
        case COMMAND_SET_IGNORE:
            return HandleSetIgnore(data, reply);
        case COMMAND_SET_FRONT_DISPATCH:
            return HandleSetFrontDispatch(data, reply);
        case COMMAND_GET_TAG_INTERFACE:
            return HandleGetNfcTagInterface(data, reply);
        case COMMAND_GET_CARDEMULATION_INTERFACE:
            return HandleGetCardEmulationInterface(data, reply);
        default:
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
}

int NfcServiceStub::HandleGetState(MessageParcel& data, MessageParcel& reply)
{
    int state = GetState();

    reply.WriteInt32(state);
    return ERR_NONE;
}

int NfcServiceStub::HandleTurnOn(MessageParcel& data, MessageParcel& reply)
{
    if (!NfcPermissions::EnforceAdminPermissions(mContext_)) {
        return NfcErrorCode::NFC_SDK_ERROR_PERMISSION;
    }

    bool result = TurnOn();
    reply.WriteInt32(result);
    return ERR_NONE;
}
int NfcServiceStub::HandleTurnOff(MessageParcel& data, MessageParcel& reply)
{
    if (!NfcPermissions::EnforceAdminPermissions(mContext_)) {
        return NfcErrorCode::NFC_SDK_ERROR_PERMISSION;
    }

    bool saveState = data.ReadBool();

    saveState = TurnOff(saveState);
    reply.WriteInt32(saveState);
    return ERR_NONE;
}
int NfcServiceStub::HandlePausePolling(MessageParcel& data, MessageParcel& reply)
{
    if (!NfcPermissions::EnforceAdminPermissions(mContext_)) {
        return NfcErrorCode::NFC_SDK_ERROR_PERMISSION;
    }

    int timeOutMS = data.ReadInt64();
    PausePolling(timeOutMS);

    return ERR_NONE;
}
int NfcServiceStub::HandleResumePolling(MessageParcel& data, MessageParcel& reply)
{
    if (!NfcPermissions::EnforceAdminPermissions(mContext_)) {
        return NfcErrorCode::NFC_SDK_ERROR_PERMISSION;
    }

    ResumePolling();

    return ERR_NONE;
}
int NfcServiceStub::HandleSetNfcSecure(MessageParcel& data, MessageParcel& reply)
{
    if (!NfcPermissions::EnforceAdminPermissions(mContext_)) {
        return NfcErrorCode::NFC_SDK_ERROR_PERMISSION;
    }

    bool result = data.ReadBool();
    result = SetNfcSecure(result);

    reply.WriteBool(result);
    return ERR_NONE;
}
int NfcServiceStub::HandleDeviceSupportsNfcSecure(MessageParcel& data, MessageParcel& reply)
{
    bool result = DeviceSupportsNfcSecure();

    reply.WriteInt32(result);
    return ERR_NONE;
}
int NfcServiceStub::HandleIsNfcSecureEnabled(MessageParcel& data, MessageParcel& reply)
{
    bool result = IsNfcSecureEnabled();

    reply.WriteInt32(result);
    return ERR_NONE;
}
int NfcServiceStub::HandleSetRWMode(MessageParcel& data, MessageParcel& reply)
{
    sptr<OHOS::IRemoteObject> readerObj = data.ReadRemoteObject();
    sptr<OHOS::IRemoteObject> callback = data.ReadRemoteObject();
    DebugLog("NfcServiceStub::HandleSetRWMode callback.%p ", (void*)callback.GetRefPtr());
    std::shared_ptr<nfc::sdk::IAppCallback> appCallback = std::make_shared<sdk::AppCallbackProxy>(callback);
    int flags = data.ReadInt32();
    std::shared_ptr<sdk::NfcMap> readerExtras{};
    if (flags != 0) {
        readerExtras = std::shared_ptr<sdk::NfcMap>(data.ReadParcelable<sdk::NfcMap>());
    } else {
        readerExtras = std::shared_ptr<sdk::NfcMap>();
    }
    SetRWMode(readerObj, appCallback, flags, readerExtras);
    return ERR_NONE;
}

int NfcServiceStub::HandleSetIgnore(MessageParcel& data, MessageParcel& reply)
{
    if (!NfcPermissions::EnforceUserPermissions(mContext_)) {
        return NfcErrorCode::NFC_SDK_ERROR_PERMISSION;
    }

    int handle = data.ReadInt32();
    int debounceMs = data.ReadInt32();
    sptr<IRemoteObject> callback = data.ReadRemoteObject();
    std::shared_ptr<nfc::sdk::ITagRemovedCallback> removedCallback =
        std::make_shared<nfc::sdk::TagRemovedCallbackProxy>(callback);
    bool res = SetIgnore(handle, debounceMs, removedCallback);
    reply.WriteBool(res);
    return ERR_NONE;
}
int NfcServiceStub::HandleSetFrontDispatch(MessageParcel& data, MessageParcel& reply)
{
    if (!NfcPermissions::EnforceUserPermissions(mContext_)) {
        return NfcErrorCode::NFC_SDK_ERROR_PERMISSION;
    }

    SetFrontDispatch();
    return ERR_NONE;
}
int NfcServiceStub::HandleGetNfcTagInterface(MessageParcel& data, MessageParcel& reply)
{
    OHOS::sptr<nfc::reader::ITagSession> tagSession = GetNfcTagInterface();
    if (!tagSession) {
        DebugLog("tagSession err!");
    }

    reply.WriteRemoteObject(tagSession->AsObject());
    reply.WriteInt32(ERR_NONE);
    return ERR_NONE;
}
int NfcServiceStub::HandleGetCardEmulationInterface(OHOS::MessageParcel& data, OHOS::MessageParcel& reply)
{
#ifdef _NFC_SERVICE_HCE_
    using namespace OHOS::nfc::sdk::cardemulation;
    auto svs = GetNfcCardEmulationInterface();
    DebugLog("svs: %p", svs.GetRefPtr());
    if (svs) {
        bool b = reply.WriteRemoteObject(RemoteObjectPool::GetRemoteObject(svs->AsObject()));
        DebugLog("WriteRemoteObject return %s\n", b ? "true" : "false");
        return ERR_NONE;
    }
#endif

    return IPC_SKELETON_NULL_OBJECT_ERR + 1;
}
}  // namespace nfc
}  // namespace OHOS