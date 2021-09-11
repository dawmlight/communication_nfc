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
#include "tag_session_stub.h"

#include "context.h"
#include "loghelper.h"
#include "nfc_permissions.h"
#include "nfc_sdk_common.h"

using namespace std;
using OHOS::nfc::reader::ResResult;

namespace OHOS {
namespace nfc {
using sdk::NfcErrorCode;

namespace reader {
int TagSessionStub::OnRemoteRequest(uint32_t code,         /* [in] */
                                    MessageParcel& data,   /* [in] */
                                    MessageParcel& reply,  /* [out] */
                                    MessageOption& option) /* [in] */
{
    DebugLog("OnRemoteRequest occur, code is %d", code);
    switch (code) {
        case COMMAND_CONNECT:
            return HandleConnect(data, reply);
        case COMMAND_RECONNECT:
            return HandleReconnect(data, reply);
        case COMMAND_DISCONNECT:
            return HandleDisconnect(data, reply);
        case COMMAND_GET_TECHLIST:
            return HandleGetTechList(data, reply);
        case COMMAND_IS_PRESENT:
            return HandleIsPresent(data, reply);
        case COMMAND_IS_NDEF:
            return HandleIsNdef(data, reply);
        case COMMAND_SEND_RAW_FRAME:
            return HandleSendRawFrame(data, reply);
        case COMMAND_NDEF_READ:
            return HandleNdefRead(data, reply);
        case COMMAND_NDEF_WRITE:
            return HandleNdefWrite(data, reply);
        case COMMAND_NDEF_MAKE_READ_ONLY:
            return HandleCanMakeReadOnly(data, reply);
        case COMMAND_FORMAT_NDEF:
            return HandleFormatNdef(data, reply);
        case COMMAND_CAN_MAKE_READ_ONLY:
            return HandleCanMakeReadOnly(data, reply);
        case COMMAND_GET_MAX_TRANSCEIVE_LENGTH:
            return HandleGetMaxTransceiveLength(data, reply);
        case COMMAND_IS_SUPPORTED_APDUS_EXTENDED:
            return HandleIsSupportedApdusExtended(data, reply);
        default:
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
}
int TagSessionStub::HandleConnect(MessageParcel& data, MessageParcel& reply)
{
    if (!NfcPermissions::EnforceUserPermissions(mContext_)) {
        return NfcErrorCode::NFC_SDK_ERROR_PERMISSION;
    }

    int nativeHandle = data.ReadInt32();
    int technology = data.ReadInt32();
    int res = Connect(nativeHandle, technology);

    reply.WriteInt32(res);
    return ERR_NONE;
}
int TagSessionStub::HandleReconnect(MessageParcel& data, MessageParcel& reply)
{
    if (!NfcPermissions::EnforceUserPermissions(mContext_)) {
        return NfcErrorCode::NFC_SDK_ERROR_PERMISSION;
    }

    int nativeHandle = data.ReadInt32();
    int ec = Reconnect(nativeHandle);

    reply.WriteInt32(ec);
    return ERR_NONE;
}
int TagSessionStub::HandleDisconnect(MessageParcel& data, MessageParcel& reply)
{
    if (!NfcPermissions::EnforceUserPermissions(mContext_)) {
        return NfcErrorCode::NFC_SDK_ERROR_PERMISSION;
    }

    int nativeHandle = data.ReadInt32();
    Disconnect(nativeHandle);
    return ERR_NONE;
}
int TagSessionStub::HandleGetTechList(MessageParcel& data, MessageParcel& reply)
{
    if (!NfcPermissions::EnforceUserPermissions(mContext_)) {
        return NfcErrorCode::NFC_SDK_ERROR_PERMISSION;
    }

    int nativeHandle = data.ReadInt32();
    std::vector<int32_t> techList = GetTechList(nativeHandle);

    reply.WriteInt32Vector(techList);
    return ERR_NONE;
}
int TagSessionStub::HandleIsPresent(MessageParcel& data, MessageParcel& reply)
{
    int nativeHandle = data.ReadInt32();
    bool res = IsNdef(nativeHandle);

    reply.WriteBool(res);
    return ERR_NONE;
}
int TagSessionStub::HandleIsNdef(MessageParcel& data, MessageParcel& reply)
{
    if (!NfcPermissions::EnforceUserPermissions(mContext_)) {
        return NfcErrorCode::NFC_SDK_ERROR_PERMISSION;
    }

    int nativeHandle = data.ReadInt32();
    bool res = IsNdef(nativeHandle);

    reply.WriteBool(res);
    return ERR_NONE;
}
int TagSessionStub::HandleSendRawFrame(MessageParcel& data, MessageParcel& reply)
{
    if (!NfcPermissions::EnforceUserPermissions(mContext_)) {
        return NfcErrorCode::NFC_SDK_ERROR_PERMISSION;
    }

    int nativeHandle = data.ReadInt32();
    std::string commandData = data.ReadString();
    bool raw = data.ReadBool();
    std::unique_ptr<ResResult> res = SendRawFrame(nativeHandle, commandData, raw);
    reply.WriteParcelable(res.get());
    reply.WriteInt32(ERR_NONE);
    return ERR_NONE;
}
int TagSessionStub::HandleNdefRead(MessageParcel& data, MessageParcel& reply)
{
    if (!NfcPermissions::EnforceUserPermissions(mContext_)) {
        return NfcErrorCode::NFC_SDK_ERROR_PERMISSION;
    }

    int nativeHandle = data.ReadInt32();
    std::string readData = NdefRead(nativeHandle);
    reply.WriteString(readData);
    return ERR_NONE;
}
int TagSessionStub::HandleNdefWrite(MessageParcel& data, MessageParcel& reply)
{
    if (!NfcPermissions::EnforceUserPermissions(mContext_)) {
        return NfcErrorCode::NFC_SDK_ERROR_PERMISSION;
    }

    int nativeHandle = data.ReadInt32();
    std::string msg = data.ReadString();
    int ec = NdefWrite(nativeHandle, msg);

    reply.WriteInt32(ec);
    return ERR_NONE;
}
int TagSessionStub::HandleNdefMakeReadOnly(MessageParcel& data, MessageParcel& reply)
{
    if (!NfcPermissions::EnforceUserPermissions(mContext_)) {
        return NfcErrorCode::NFC_SDK_ERROR_PERMISSION;
    }

    int nativeHandle = data.ReadInt32();
    reply.WriteInt32(NdefMakeReadOnly(nativeHandle));
    return ERR_NONE;
}
int TagSessionStub::HandleFormatNdef(MessageParcel& data, MessageParcel& reply)
{
    if (!NfcPermissions::EnforceUserPermissions(mContext_)) {
        return NfcErrorCode::NFC_SDK_ERROR_PERMISSION;
    }

    int nativeHandle = data.ReadInt32();
    std::string key = data.ReadString();
    reply.WriteBool(FormatNdef(nativeHandle, key));
    return ERR_NONE;
}
int TagSessionStub::HandleCanMakeReadOnly(MessageParcel& data, MessageParcel& reply)
{
    int technology = data.ReadInt32();
    reply.WriteBool(CanMakeReadOnly(technology));
    return ERR_NONE;
}
int TagSessionStub::HandleGetMaxTransceiveLength(MessageParcel& data, MessageParcel& reply)
{
    int technology = data.ReadInt32();
    reply.WriteInt32(GetMaxTransceiveLength(technology));
    return ERR_NONE;
}
int TagSessionStub::HandleIsSupportedApdusExtended(MessageParcel& data, MessageParcel& reply)
{
    reply.WriteBool(IsSupportedApdusExtended());
    return ERR_NONE;
}
}  // namespace reader
}  // namespace nfc
}  // namespace OHOS