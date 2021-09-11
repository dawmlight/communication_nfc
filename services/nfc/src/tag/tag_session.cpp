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

#include "tag_session.h"
#include "nfc_adapter_utils.h"
#include "nfc_error.h"
#include "nfc_log.h"
#include "nfc_manager_service.h"
#include "nfc_tag_adapter_utils.h"
#include "nfc_utils.h"

namespace OHOS {
namespace Nfc {
namespace Tag {
const std::u16string NFC_TAG_TOKEN = u"ohos.nfc.ITagInfo";
const std::string PERM_NFC_TAG = "ohos.permission.NFC_TAG";
const std::string PKG_TAG = "ohos.nfc.INfcTag";
const std::string PKG_ADAPTER = "ohos.nfc.INfcAdapter";

TagSession TagSession::g_tagSession;

TagSession::~TagSession() = default;

TagSession& TagSession::GetInstance()
{
    return g_tagSession;
}

/* get NFC tag service(nfctag) */
sptr<IRemoteObject> TagSession::GetNfcRemoteTagService()
{
    if (g_remoteTagService != nullptr) {
        return g_remoteTagService;
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
    HILOGI("TagSession::GetRemoteNfcService start!");
    int32_t error = remoteNfcService_->SendRequest(TRANSACTION_GET_NFCTAGINTERFACE, data, reply, option);
    HILOGI("TagSession::GetRemoteNfcService end!");
    if (error != Nfc::ERR_NONE) {
        HILOGE("TagSession::GetRemoteNfcService, fail. error: %{public}d", error);
        return nullptr;
    }

    /* Consume the exception code */
    int32_t exception = reply.ReadInt32();
    if (exception != Nfc::ERR_NONE) {
        HILOGE("TagSession::GetRemoteNfcService. exception: %{public}d", exception);
        return nullptr;
    }
    /* Return the exec result */
    g_remoteTagService = reply.ReadObject<IRemoteObject>();
    return g_remoteTagService;
}


int32_t TagSession::ConnectTag(MessageParcel& data, MessageParcel& reply)
{
    std::string remoteDesc = Str16ToStr8(data.ReadString16());
    std::string validDesc = Str16ToStr8(NFC_TAG_TOKEN);
    CHECK_REMOTE_DESC(remoteDesc, validDesc);
    CHECK_PERMISSON(PERM_NFC_TAG, IPCSkeleton::GetCallingUid());

    int32_t handleInt = data.ReadInt32();
    int32_t techInt = data.ReadInt32();
    HILOGI("TagSession::ConnectTag function start! TechInt = %d", techInt);

    if (GetNfcRemoteTagService() == nullptr) {
        return Nfc::ERR_REMOTE_SERVICE_DISCONNECTED;
    }

    // Send TRANSACTION
    MessageParcel dataToNfcService;
    MessageParcel replyFromNfcService;
    MessageOption option;
    WRITE_INTF_TOKEN(StringReplace(PKG_TAG), dataToNfcService);
    dataToNfcService.WriteInt32(handleInt);
    dataToNfcService.WriteInt32(techInt);
    HILOGI("TagSession::Transact == TRANSACTION_connect start!");
    int32_t error = GetNfcRemoteTagService()->SendRequest(TRANSACTION_CONNECT,
        dataToNfcService, replyFromNfcService, option);
    HILOGI("TagSession::Transact == TRANSACTION_connect end!");
    if (error != Nfc::ERR_NONE) {
        HILOGE("TagSession::TRANSACTION_CONNECT, fail. error: %{public}d", error);
        return Nfc::ERR_REMOTE_TRANSACT_FAILED;
    }

    /* Consume the exception code */
    int32_t exception = replyFromNfcService.ReadInt32();
    if (exception != Nfc::ERR_NONE) {
        HILOGE("TagSession::TRANSACTION_CONNECT. exception: %{public}d", exception);
        return Nfc::ERR_REMOTE_EXCEPTION_RETURN;
    }

    /* Return the exec result, 0 means connect success in NFC service. */
    if (replyFromNfcService.ReadInt32() == 0) {
        return Nfc::ERR_NONE;
    }
    return Nfc::ERR_REMOTE_RETURN_ERR;
}

int32_t TagSession::ReconnectTag(MessageParcel& data, MessageParcel& reply)
{
    std::string remoteDesc = Str16ToStr8(data.ReadString16());
    std::string validDesc = Str16ToStr8(NFC_TAG_TOKEN);
    CHECK_REMOTE_DESC(remoteDesc, validDesc);
    CHECK_PERMISSON(PERM_NFC_TAG, IPCSkeleton::GetCallingUid());
    int32_t nativeHandle = data.ReadInt32();
    HILOGI("TagSession::ReconnectTag function start!");
    if (GetNfcRemoteTagService() == nullptr) {
        return Nfc::ERR_REMOTE_SERVICE_DISCONNECTED;
    }

    // Send TRANSACTION
    MessageParcel dataToNfcService;
    MessageParcel replyFromNfcService;
    MessageOption option;
    WRITE_INTF_TOKEN(StringReplace(PKG_TAG), dataToNfcService);
    dataToNfcService.WriteInt32(nativeHandle);
    HILOGI("TagSession::Transact == TRANSACTION_reconnect start!");
    int32_t error = GetNfcRemoteTagService()->SendRequest(TRANSACTION_RECONNECT,
        dataToNfcService, replyFromNfcService, option);
    HILOGI("TagSession::Transact == TRANSACTION_reconnect end!");
    if (error != Nfc::ERR_NONE) {
        HILOGE("TagSession::NfcService_->Transact, fail. error: %{public}d", error);
        return Nfc::ERR_REMOTE_TRANSACT_FAILED;
    }

    /* Consume the exception code */
    int32_t exception = replyFromNfcService.ReadInt32();
    if (exception != Nfc::ERR_NONE) {
        HILOGE("TagSession::TRANSACTION_RECONNECT. exception: %{public}d", exception);
        return Nfc::ERR_REMOTE_EXCEPTION_RETURN;
    }

    /* Return the exec result, 0 means connect success. */
    if (replyFromNfcService.ReadInt32() == 0) {
        return Nfc::ERR_NONE;
    }
    return Nfc::ERR_REMOTE_RETURN_ERR;
}

int32_t TagSession::ResetTimeouts(MessageParcel& data, MessageParcel& reply)
{
    std::string remoteDesc = Str16ToStr8(data.ReadString16());
    std::string validDesc = Str16ToStr8(NFC_TAG_TOKEN);
    CHECK_REMOTE_DESC(remoteDesc, validDesc);
    CHECK_PERMISSON(PERM_NFC_TAG, IPCSkeleton::GetCallingUid());
    HILOGI("TagSession::ResetTimeouts function start!");
    if (GetNfcRemoteTagService() == nullptr) {
        return Nfc::ERR_REMOTE_SERVICE_DISCONNECTED;
    }

    // Send TRANSACTION
    MessageParcel dataToNfcService;
    MessageParcel replyFromNfcService;
    MessageOption option;
    WRITE_INTF_TOKEN(StringReplace(PKG_TAG), dataToNfcService);
    HILOGI("TagSession::Transact == TRANSACTION_resetTimeouts start!");
    int32_t error = GetNfcRemoteTagService()->SendRequest(TRANSACTION_RESETTIMEOUTS,
        dataToNfcService, replyFromNfcService, option);
    HILOGI("TagSession::Transact == TRANSACTION_resetTimeouts end!");
    if (error != Nfc::ERR_NONE) {
        HILOGE("TagSession::NfcService_->Transact, fail. error: %{public}d", error);
        return Nfc::ERR_REMOTE_TRANSACT_FAILED;
    }
    /* Consume the exception code */
    int32_t exception = replyFromNfcService.ReadInt32();
    if (exception != Nfc::ERR_NONE) {
        HILOGE("TagSession::TRANSACTION_RESETTIMEOUTS. exception: %{public}d", exception);
        return Nfc::ERR_REMOTE_EXCEPTION_RETURN;
    }
    return Nfc::ERR_NONE;
}

int32_t TagSession::IsTagConnected(MessageParcel& data, MessageParcel& reply)
{
    std::string remoteDesc = Str16ToStr8(data.ReadString16());
    std::string validDesc = Str16ToStr8(NFC_TAG_TOKEN);
    CHECK_REMOTE_DESC(remoteDesc, validDesc);
    CHECK_PERMISSON(PERM_NFC_TAG, IPCSkeleton::GetCallingUid());
    int32_t nativeHandle = data.ReadInt32();
    HILOGI("TagSession::IsTagConnected function start!");
    if (GetNfcRemoteTagService() == nullptr) {
        return Nfc::ERR_REMOTE_SERVICE_DISCONNECTED;
    }

    // Send TRANSACTION
    MessageParcel dataToNfcService;
    MessageParcel replyFromNfcService;
    MessageOption option;
    WRITE_INTF_TOKEN(StringReplace(PKG_TAG), dataToNfcService);
    dataToNfcService.WriteInt32(nativeHandle);
    HILOGI("TagSession::IsTagConnected start!");
    int32_t error = GetNfcRemoteTagService()->SendRequest(TRANSACTION_IS_PRESENT,
        dataToNfcService, replyFromNfcService, option);
    HILOGI("TagSession::IsTagConnected end!");
    if (error != Nfc::ERR_NONE) {
        HILOGE("TagSession::TRANSACTION_IS_PRESENT. error: %{public}d", error);
        return Nfc::ERR_REMOTE_TRANSACT_FAILED;
    }

    /* Consume the exception code */
    int32_t exception = replyFromNfcService.ReadInt32();
    if (exception != Nfc::ERR_NONE) {
        HILOGE("TagSession::TRANSACTION_IS_PRESENT. exception: %{public}d", exception);
        return Nfc::ERR_REMOTE_EXCEPTION_RETURN;
    }
    /* Return the exec result, 1 means tag connected. */
    if (replyFromNfcService.ReadInt32() == 1) {
        return Nfc::ERR_NONE;
    }
    return Nfc::ERR_REMOTE_RETURN_ERR;
}

int32_t TagSession::SetSendDataTimeout(MessageParcel& data, MessageParcel& reply)
{
    std::string remoteDesc = Str16ToStr8(data.ReadString16());
    std::string validDesc = Str16ToStr8(NFC_TAG_TOKEN);
    CHECK_REMOTE_DESC(remoteDesc, validDesc);
    CHECK_PERMISSON(PERM_NFC_TAG, IPCSkeleton::GetCallingUid());
    int32_t tech = data.ReadInt32();
    int32_t timeout = data.ReadInt32();
    HILOGI("TagSession::SetSendDataTimeout function start!");
    if (GetNfcRemoteTagService() == nullptr) {
        return Nfc::ERR_REMOTE_SERVICE_DISCONNECTED;
    }

    // Send TRANSACTION
    MessageParcel dataToNfcService;
    MessageParcel replyFromNfcService;
    MessageOption option;
    WRITE_INTF_TOKEN(StringReplace(PKG_TAG), dataToNfcService);
    dataToNfcService.WriteInt32(tech);
    dataToNfcService.WriteInt32(timeout);
    HILOGI("TagSession::Transact == TRANSACTION_setTimeout start!");
    int32_t error = GetNfcRemoteTagService()->SendRequest(TRANSACTION_SET_TIMEOUT,
        dataToNfcService, replyFromNfcService, option);
    HILOGI("TagSession::Transact == TRANSACTION_setTimeout end!");
    if (error != Nfc::ERR_NONE) {
        HILOGE("TagSession::TRANSACTION_SET_TIMEOUT. error: %{public}d", error);
        return Nfc::ERR_REMOTE_TRANSACT_FAILED;
    }

    /* Consume the exception code */
    int32_t exception = replyFromNfcService.ReadInt32();
    if (exception != Nfc::ERR_NONE) {
        HILOGE("TagSession::TRANSACTION_SET_TIMEOUT. exception: %{public}d", exception);
        return Nfc::ERR_REMOTE_EXCEPTION_RETURN;
    }
    /* Return the exec result, 0 means success from NFC service */
    if (replyFromNfcService.ReadInt32() == 0) {
        return Nfc::ERR_NONE;
    }
    return Nfc::ERR_REMOTE_RETURN_ERR;
}

int32_t TagSession::GetSendDataTimeout(MessageParcel& data, MessageParcel& reply)
{
    std::string remoteDesc = Str16ToStr8(data.ReadString16());
    std::string validDesc = Str16ToStr8(NFC_TAG_TOKEN);
    CHECK_REMOTE_DESC(remoteDesc, validDesc);
    CHECK_PERMISSON(PERM_NFC_TAG, IPCSkeleton::GetCallingUid());
    int32_t tech = data.ReadInt32();
    HILOGI("TagSession::GetSendDataTimeout function start!");
    if (GetNfcRemoteTagService() == nullptr) {
        return Nfc::ERR_REMOTE_SERVICE_DISCONNECTED;
    }

    // Send TRANSACTION
    MessageParcel dataToNfcService;
    MessageParcel replyFromNfcService;
    MessageOption option;
    WRITE_INTF_TOKEN(StringReplace(PKG_TAG), dataToNfcService);
    dataToNfcService.WriteInt32(tech);
    HILOGI("TagSession::Transact == TRANSACTION_getTimeout start!");
    int32_t error = GetNfcRemoteTagService()->SendRequest(TRANSACTION_GET_TIMEOUT,
        dataToNfcService, replyFromNfcService, option);
    HILOGI("TagSession::Transact == TRANSACTION_getTimeout end!");
    if (error != Nfc::ERR_NONE) {
        HILOGE("TagSession::TRANSACTION_GET_TIMEOUT fail. error: %{public}d", error);
        return Nfc::ERR_REMOTE_TRANSACT_FAILED;
    }

    /* Consume the exception code */
    int32_t exception = replyFromNfcService.ReadInt32();
    if (exception != Nfc::ERR_NONE) {
        HILOGE("TagSession::TRANSACTION_GET_TIMEOUT. exception: %{public}d", exception);
        return Nfc::ERR_REMOTE_EXCEPTION_RETURN;
    }
    /* Return the exec result, the time from NFC service */
    return replyFromNfcService.ReadInt32();
}

int32_t TagSession::GetMaxSendLength(MessageParcel& data, MessageParcel& reply)
{
    std::string remoteDesc = Str16ToStr8(data.ReadString16());
    std::string validDesc = Str16ToStr8(NFC_TAG_TOKEN);
    CHECK_REMOTE_DESC(remoteDesc, validDesc);
    CHECK_PERMISSON(PERM_NFC_TAG, IPCSkeleton::GetCallingUid());
    int32_t tech = data.ReadInt32();
    HILOGI("TagSession::GetMaxSendLength function start! DataInt = %d", tech);
    if (GetNfcRemoteTagService() == nullptr) {
        return Nfc::ERR_REMOTE_SERVICE_DISCONNECTED;
    }

    // Send TRANSACTION
    MessageParcel dataToNfcService;
    MessageParcel replyFromNfcService;
    MessageOption option;
    WRITE_INTF_TOKEN(StringReplace(PKG_TAG), dataToNfcService);
    dataToNfcService.WriteInt32(tech);
    HILOGI("TagSession::Transact == TRANSACTION_getMaxTransceiveLength start!");
    int32_t error = GetNfcRemoteTagService()->SendRequest(TRANSACTION_GET_MAX_TRANSCEIVE_LENGTH,
        dataToNfcService, replyFromNfcService, option);
    HILOGI("TagSession::Transact == TRANSACTION_getMaxTransceiveLength end!");
    if (error != Nfc::ERR_NONE) {
        HILOGE("TagSession::TRANSACTION_GET_MAX_TRANSCEIVE_LENGTH, fail. error: %{public}d", error);
        return Nfc::ERR_REMOTE_TRANSACT_FAILED;
    }

    /* Consume the exception code */
    int32_t exception = replyFromNfcService.ReadInt32();
    if (exception != Nfc::ERR_NONE) {
        HILOGE("TagSession::TRANSACTION_GET_MAX_TRANSCEIVE_LENGTH. exception: %{public}d", exception);
        return Nfc::ERR_REMOTE_EXCEPTION_RETURN;
    }
    /* Return the exec result */
    return replyFromNfcService.ReadInt32();
}

int32_t TagSession::SendData(MessageParcel& data, MessageParcel& reply)
{
    HILOGI("TagSession::SendData function start!");
    CHECK_PERMISSON(PERM_NFC_TAG, IPCSkeleton::GetCallingUid());
    if (GetNfcRemoteTagService() == nullptr) {
        return Nfc::ERR_REMOTE_SERVICE_DISCONNECTED;
    }

    // Send TRANSACTION
    MessageOption option;
    HILOGI("TagSession::Transact == TRANSACTION_SendRawApduData start!");
    int32_t error = GetNfcRemoteTagService()->SendRequest(TRANSACTION_TRANSCEIVE, data, reply, option);
    HILOGI("TagSession::Transact == TRANSACTION_SendRawApduData end!");
    if (error != Nfc::ERR_NONE) {
        HILOGE("TagSession::TRANSACTION_TRANSCEIVE, fail. SendData error: %{public}d", error);
        return Nfc::ERR_REMOTE_TRANSACT_FAILED;
    }
    /* Consume the exception code */
    int32_t exception = reply.ReadInt32();
    if (exception != Nfc::ERR_NONE) {
        HILOGE("TagSession::TRANSACTION_TRANSCEIVE. exception: %{public}d", exception);
        return Nfc::ERR_REMOTE_EXCEPTION_RETURN;
    }
    return Nfc::ERR_NONE;
}

/* NFC Tag operations 2.0 */
int32_t TagSession::IsNdefTag(MessageParcel& data, MessageParcel& reply)
{
    HILOGI("TagSession::IsNdefTag function start!");
    CHECK_PERMISSON(PERM_NFC_TAG, IPCSkeleton::GetCallingUid());
    if (GetNfcRemoteTagService() == nullptr) {
        return Nfc::ERR_REMOTE_SERVICE_DISCONNECTED;
    }

    // Send TRANSACTION
    MessageOption option;
    HILOGI("TagSession::Transact == TRANSACTION_IsNdefTag start!");
    int32_t error = GetNfcRemoteTagService()->SendRequest(TRANSACTION_ISNDEF, data, reply, option);
    HILOGI("TagSession::Transact == TRANSACTION_IsNdefTag end!");
    if (error != Nfc::ERR_NONE) {
        HILOGE("TagSession::TRANSACTION_ISNDEF, fail. ISNDEF error: %{public}d", error);
        return Nfc::ERR_REMOTE_TRANSACT_FAILED;
    }
    /* Consume the exception code */
    int32_t exception = reply.ReadInt32();
    if (exception != Nfc::ERR_NONE) {
        HILOGE("TagSession::TRANSACTION_ISNDEF. exception: %{public}d", exception);
        return Nfc::ERR_REMOTE_EXCEPTION_RETURN;
    }
    return Nfc::ERR_NONE;
}

int32_t TagSession::NdefRead(MessageParcel& data, MessageParcel& reply)
{
    HILOGI("TagSession::NdefRead function start!");
    CHECK_PERMISSON(PERM_NFC_TAG, IPCSkeleton::GetCallingUid());
    if (GetNfcRemoteTagService() == nullptr) {
        return Nfc::ERR_REMOTE_SERVICE_DISCONNECTED;
    }

    // Send TRANSACTION
    MessageOption option;
    HILOGI("TagSession::Transact == TRANSACTION_NdefRead start!");
    int32_t error = GetNfcRemoteTagService()->SendRequest(TRANSACTION_NDEFREAD, data, reply, option);
    HILOGI("TagSession::Transact == TRANSACTION_NdefRead end!");
    if (error != Nfc::ERR_NONE) {
        HILOGE("TagSession::TRANSACTION_NDEFREAD, fail. NdefRead error: %{public}d", error);
        return Nfc::ERR_REMOTE_TRANSACT_FAILED;
    }
    /* Consume the exception code */
    int32_t exception = reply.ReadInt32();
    if (exception != Nfc::ERR_NONE) {
        HILOGE("TagSession::TRANSACTION_NDEFREAD. exception: %{public}d", exception);
        return Nfc::ERR_REMOTE_EXCEPTION_RETURN;
    }
    return Nfc::ERR_NONE;
}

int32_t TagSession::NdefWrite(MessageParcel& data, MessageParcel& reply)
{
    HILOGI("TagSession::NdefWrite function start!");
    CHECK_PERMISSON(PERM_NFC_TAG, IPCSkeleton::GetCallingUid());
    if (GetNfcRemoteTagService() == nullptr) {
        return Nfc::ERR_REMOTE_SERVICE_DISCONNECTED;
    }

    // Send TRANSACTION
    MessageOption option;
    HILOGI("TagSession::Transact == TRANSACTION_NdefWrite start!");
    int32_t error = GetNfcRemoteTagService()->SendRequest(TRANSACTION_NDEFWRITE, data, reply, option);
    HILOGI("TagSession::Transact == TRANSACTION_NdefWrite end!");
    if (error != Nfc::ERR_NONE) {
        HILOGE("TagSession::TRANSACTION_NDEFWRITE, fail. NdefWrite error: %{public}d", error);
        return Nfc::ERR_REMOTE_TRANSACT_FAILED;
    }
    /* Consume the exception code */
    int32_t exception = reply.ReadInt32();
    if (exception != Nfc::ERR_NONE) {
        HILOGE("TagSession::TRANSACTION_NDEFWRITE. exception: %{public}d", exception);
        return Nfc::ERR_REMOTE_EXCEPTION_RETURN;
    }
    return Nfc::ERR_NONE;
}

int32_t TagSession::NdefSetReadOnly(MessageParcel& data, MessageParcel& reply)
{
    HILOGI("TagSession::NdefSetReadOnly function start!");
    CHECK_PERMISSON(PERM_NFC_TAG, IPCSkeleton::GetCallingUid());
    if (GetNfcRemoteTagService() == nullptr) {
        return Nfc::ERR_REMOTE_SERVICE_DISCONNECTED;
    }

    // Send TRANSACTION
    MessageOption option;
    HILOGI("TagSession::Transact == TRANSACTION_NdefSetReadOnly start!");
    int32_t error = GetNfcRemoteTagService()->SendRequest(TRANSACTION_NDEFSETREADONLY, data, reply, option);
    HILOGI("TagSession::Transact == TRANSACTION_NdefSetReadOnly end!");
    if (error != Nfc::ERR_NONE) {
        HILOGE("TagSession::TRANSACTION_NDEFSETREADONLY, fail. NdefSetReadOnly error: %{public}d", error);
        return Nfc::ERR_REMOTE_TRANSACT_FAILED;
    }
    /* Consume the exception code */
    int32_t exception = reply.ReadInt32();
    if (exception != Nfc::ERR_NONE) {
        HILOGE("TagSession::TRANSACTION_NDEFSETREADONLY. exception: %{public}d", exception);
        return Nfc::ERR_REMOTE_EXCEPTION_RETURN;
    }
    return Nfc::ERR_NONE;
}

int32_t TagSession::CanSetReadOnly(MessageParcel& data, MessageParcel& reply)
{
    HILOGI("TagSession::CanSetReadOnly function start!");
    CHECK_PERMISSON(PERM_NFC_TAG, IPCSkeleton::GetCallingUid());
    if (GetNfcRemoteTagService() == nullptr) {
        return Nfc::ERR_REMOTE_SERVICE_DISCONNECTED;
    }

    // Send TRANSACTION
    MessageOption option;
    HILOGI("TagSession::Transact == TRANSACTION_CanSetReadOnly start!");
    int32_t error = GetNfcRemoteTagService()->SendRequest(TRANSACTION_CANSETREADONLY, data, reply, option);
    HILOGI("TagSession::Transact == TRANSACTION_CanSetReadOnly end!");
    if (error != Nfc::ERR_NONE) {
        HILOGE("TagSession::TRANSACTION_CANSETREADONLY, fail. CanSetReadOnly error: %{public}d", error);
        return Nfc::ERR_REMOTE_TRANSACT_FAILED;
    }
    /* Consume the exception code */
    int32_t exception = reply.ReadInt32();
    if (exception != Nfc::ERR_NONE) {
        HILOGE("TagSession::TRANSACTION_CANSETREADONLY. exception: %{public}d", exception);
        return Nfc::ERR_REMOTE_EXCEPTION_RETURN;
    }
    return Nfc::ERR_NONE;
}
} // namespace Tag
} // namespace Nfc
} // namespace OHOS
