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

#include "secure_element.h"
#include "nfc_adapter_utils.h"
#include "nfc_error.h"
#include "nfc_log.h"
#include "nfc_manager_service.h"
#include "nfc_secureelement_adapter_utils.h"
#include "nfc_utils.h"
#include "se_service_callback_skeleton.h"

namespace OHOS {
namespace Nfc {
namespace Oma {
const std::u16string NFC_TAG_TOKEN = u"ohos.nfc.ITagInfo";
const std::string PERM_NFC_TAG = "ohos.permission.NFC_TAG";
const std::string PKG_SE_SERVICE = "ohos.se.omapi.ISecureElementService";
const std::string PKG_SE_READER = "ohos.se.omapi.ISecureElementReader";
const std::string PKG_SE_SESSION = "ohos.se.omapi.ISecureElementSession";
const std::string PKG_SE_CHANNEL = "ohos.se.omapi.ISecureElementChannel";
const std::u16string SECURE_ELEMENT_TOKEN = u"ohos.nfc.oma.ISecureElement";

/* Channel Types */
const int BASIC_CHANNEL = 0;
const int LOGICAL_CHANNEL = 1;

SecureElement SecureElement::g_secureElement;

SecureElement::~SecureElement() = default;

SecureElement& SecureElement::GetInstance()
{
    return g_secureElement;
}

/* get secure element service */
sptr<IRemoteObject> SecureElement::GetRemoteSeService()
{
    if (g_remoteSeService != nullptr) {
        return g_remoteSeService;
    }
    sptr<IServiceRegistry> registry = ServiceRegistry::GetInstance();
    if (registry == nullptr) {
        return nullptr;
    }
    g_remoteSeService = registry->CheckService(u"secure_element");
    HILOGI("SecureElement::GetRemoteSeService");
    return g_remoteSeService;
}

bool SecureElement::CheckSeServiceOn()
{
    sptr<IServiceRegistry> registry = ServiceRegistry::GetInstance();
    if (registry == nullptr) {
        return false;
    }
    sptr<IRemoteObject> remoteSeService_ = registry->CheckService(u"secure_element");
    return remoteSeService_ != nullptr;
}

/* bind Secure Element Service in case it is not auto started */
int32_t SecureElement::BindSeService(MessageParcel& data, MessageParcel& reply)
{
    HILOGI("SecureElement::BindSeService start!");
    g_remoteNfcService = NfcManagerService::GetInstance()->GetRemoteNfcService();
    if (g_remoteNfcService == nullptr) {
        return Nfc::ERR_REMOTE_SERVICE_DISCONNECTED;
    }

    MessageOption option;
    HILOGI("SecureElement::BindSeService start transact");
    int32_t error = g_remoteNfcService->SendRequest(TRANSACTION_BIND_SE_SERVICE, data, reply, option);
    HILOGI("SecureElement::BindSeService finish transact");
    if (error != Nfc::ERR_NONE) {
        HILOGE("SecureElement::TRANSACTION_BIND_SE_SERVICE, fail. error: %{public}d", error);
        return Nfc::ERR_REMOTE_TRANSACT_FAILED;
    }

    /* Consume the exception code */
    int32_t exception = reply.ReadInt32();
    if (exception != Nfc::ERR_NONE) {
        HILOGE("SecureElement::TRANSACTION_BIND_SE_SERVICE. exception: %{public}d", exception);
        return Nfc::ERR_REMOTE_EXCEPTION_RETURN;
    }
    return Nfc::ERR_NONE;
}

int32_t SecureElement::IsSeServiceConnected(MessageParcel& data, MessageParcel& reply)
{
    if (!CheckSeServiceOn()) {
        HILOGW("NfcManagerStub::IsSeServiceConnectedInner  GetSeService is null");
        reply.WriteInt32(Nfc::ERR_REMOTE_SERVICE_DISCONNECTED);
        return Nfc::ERR_REMOTE_SERVICE_DISCONNECTED;
    }
    reply.WriteInt32(Nfc::ERR_NONE);
    return Nfc::ERR_NONE;
}


/* get all Secure Element names in string */
int32_t SecureElement::GetSecureElements(MessageParcel& data, MessageParcel& reply)
{
    if (GetRemoteSeService() == nullptr) {
        HILOGE("SecureElement::GetSeService == nullptr");
        return Nfc::ERR_REMOTE_SERVICE_DISCONNECTED;
    }

    MessageOption option;
    HILOGI("SecureElement::GetSecureElements start transact");
    int32_t error = GetRemoteSeService()->SendRequest(TRANSACTION_GET_READERS, data, reply, option);
    HILOGI("SecureElement::GetSecureElements finish transact");
    if (error != Nfc::ERR_NONE) {
        HILOGE("SecureElement::TRANSACTION_GET_READERS, fail. error: %{public}d", error);
        return Nfc::ERR_REMOTE_TRANSACT_FAILED;
    }

    /* Consume the exception code */
    int32_t exception = reply.ReadInt32();
    if (exception != Nfc::ERR_NONE) {
        HILOGE("SecureElement::TRANSACTION_GET_READERS. exception: %{public}d", exception);
        return Nfc::ERR_REMOTE_EXCEPTION_RETURN;
    }
    return Nfc::ERR_NONE;
}

/* get SE reader */
sptr<IRemoteObject> SecureElement::GetSecureElement(std::u16string seName)
{
    if (GetRemoteSeService() == nullptr) {
        HILOGI("SecureElement::GetSeService == nullptr");
        return nullptr;
    }

    // Send TRANSACTION
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    WRITE_INTF_TOKEN_NULL(StringReplace(PKG_SE_SERVICE), data);
    data.WriteString16(seName);
    HILOGI("SecureElement::GetSecureElement Transact Start");
    int32_t error = GetRemoteSeService()->SendRequest(TRANSACTION_GET_READER, data, reply, option);
    if (error != Nfc::ERR_NONE) {
        HILOGE("SecureElement::NfcService_->Transact, fail. error: %{public}d", error);
        return nullptr;
    }
    HILOGI("SecureElement::GetSecureElement Transact Succeed");

    /* Consume the exception code */
    int errorCode = reply.ReadInt32();
    if (errorCode != Nfc::ERR_NONE) {
        HILOGE("SecureElement::TRANSACTION_GET_READER exception code = %d", errorCode);
        return nullptr;
    }
    /* Return the exec result (reader in nxp service) */
    sptr<IRemoteObject> secureElement = reply.ReadObject<IRemoteObject>();
    return secureElement;
}

/* verify if a card present in specific Secure element */
int32_t SecureElement::IsSecureElementPresent(MessageParcel& data, MessageParcel& reply)
{
    std::string remoteDesc = Str16ToStr8(data.ReadString16());
    std::string validDesc = Str16ToStr8(SECURE_ELEMENT_TOKEN);
    CHECK_REMOTE_DESC(remoteDesc, validDesc);

    HILOGI("SecureElement::IsSecureElementPresent start");
    std::u16string seName = data.ReadString16();
    HILOGI("SecureElement::IsSecureElementPresent input = %{public}s", Str16ToStr8(seName).c_str());
    sptr<IRemoteObject> secureElement = GetSecureElement(seName);
    if (secureElement == nullptr) {
        HILOGE("SecureElement::IsSecureElementPresen:GetSecureElement() == nullptr");
        return Nfc::ERR_REMOTE_SERVICE_DISCONNECTED;
    }

    // Send TRANSACTION
    MessageParcel tmpData;
    MessageOption option;
    WRITE_INTF_TOKEN(StringReplace(PKG_SE_READER), tmpData);
    HILOGI("SecureElement::IsSecureElementPresent Transact start!");
    int32_t error = secureElement->SendRequest(TRANSACTION_IS_SE_PRESENT, tmpData, reply, option);
    HILOGI("SecureElement::IsSecureElementPresent Transact end!");
    if (error != Nfc::ERR_NONE) {
        HILOGE("SecureElement::TRANSACTION_IS_SE_PRESENT, fail. error: %{public}d", error);
        return Nfc::ERR_REMOTE_TRANSACT_FAILED;
    }

    /* Consume the exception code */
    int errorCode = reply.ReadInt32();
    if (errorCode != Nfc::ERR_NONE) {
        HILOGE("SecureElement::TRANSACTION_GET_READER exception code = %d", errorCode);
        return Nfc::ERR_REMOTE_EXCEPTION_RETURN;
    }
    return Nfc::ERR_NONE;
}

/* Reader Get Session */
int32_t SecureElement::OpenSession(MessageParcel& data, MessageParcel& reply)
{
    std::string remoteDesc = Str16ToStr8(data.ReadString16());
    std::string validDesc = Str16ToStr8(SECURE_ELEMENT_TOKEN);
    CHECK_REMOTE_DESC(remoteDesc, validDesc);

    HILOGI("SecureElement::OpenSession Start!");
    std::u16string seName = data.ReadString16();
    sptr<IRemoteObject> secureElement = GetSecureElement(seName);
    if (secureElement == nullptr) {
        HILOGE("SecureElement::OpenSession GetSecureElement return null.");
        return Nfc::ERR_REMOTE_SERVICE_DISCONNECTED;
    }

    // Send TRANSACTION
    MessageParcel newData;
    MessageParcel newReply;
    MessageOption option;
    WRITE_INTF_TOKEN(StringReplace(PKG_SE_READER), newData);
    HILOGI("SecureElement::OpenSession Transact Start!");
    int32_t error = secureElement->SendRequest(TRANSACTION_OPEN_SESSION, newData, newReply, option);
    if (error != Nfc::ERR_NONE) {
        HILOGE("SecureElement::OpenSession Transact, call fail. error: %{public}d", error);
        return Nfc::ERR_REMOTE_TRANSACT_FAILED;
    } else {
        HILOGI("SecureElement::OpenSession Transact Succeed");
    }

    /* Consume the exception code */
    int errorCode = newReply.ReadInt32();
    if (errorCode != Nfc::ERR_NONE) {
        HILOGE("SecureElement::OpenSession Transact exception code = %d", errorCode);
        return Nfc::ERR_REMOTE_EXCEPTION_RETURN;
    }

    /* return remote object to Kits */
    sptr<IRemoteObject> session = newReply.ReadRemoteObject();
    if (session == nullptr) {
        HILOGE("SecureElement::OpenSession Get session Fail!!");
        return Nfc::ERR_REMOTE_SERVICE_DISCONNECTED;
    }
    reply.WriteInt32(Nfc::ERR_NONE);
    reply.WriteRemoteObject(session);
    return Nfc::ERR_NONE;
}

int32_t SecureElement::GetAtr(MessageParcel& data, MessageParcel& reply)
{
    std::string remoteDesc = Str16ToStr8(data.ReadString16());
    std::string validDesc = Str16ToStr8(SECURE_ELEMENT_TOKEN);
    CHECK_REMOTE_DESC(remoteDesc, validDesc);

    // check if the service is on
    HILOGI("SecureElement::GetAtr start");
    if (GetRemoteSeService() == nullptr) {
        HILOGE("SecureElement::GetAtr == nullptr");
        return Nfc::ERR_REMOTE_SERVICE_DISCONNECTED;
    }
    sptr<IRemoteObject> session = data.ReadRemoteObject();
    if (session == nullptr) {
        HILOGE("SecureElement::GetAtr Get Session Fail!!");
        return Nfc::ERR_REMOTE_SERVICE_DISCONNECTED;
    }
    HILOGI("SecureElement::GetAtr Session get RSP Transact start!");

    // do get Response from channel of nxp service.
    MessageParcel newData;
    MessageOption option;
    WRITE_INTF_TOKEN(StringReplace(PKG_SE_SESSION), newData);
    int32_t error = GetRemoteSeService()->SendRequest(TRANSACTION_GET_ATR, newData, reply, option);
    HILOGI("SecureElement::GetAtr Session get RSP Transact end!");
    if (error != Nfc::ERR_NONE) {
        HILOGE("SecureElement::GetAtr NfcChannel_->Transact, fail Call SellectRSP. error: %d", error);
        return Nfc::ERR_REMOTE_TRANSACT_FAILED;
    }

    /* Consume the exception code */
    HILOGI("SecureElement::GetAtr NfcChannel_->Transact, Succeed!");
    int errorCode = reply.ReadInt32();
    if (errorCode != Nfc::ERR_NONE) {
        HILOGE("SecureElement::GetAtr Get Response from Transact Fail. error: %{public}d", errorCode);
        return Nfc::ERR_REMOTE_EXCEPTION_RETURN;
    }
    return Nfc::ERR_NONE;
}

/* Reader Close Remote Sessions */
int32_t SecureElement::CloseSeSessions(MessageParcel& data, MessageParcel& reply)
{
    std::string remoteDesc = Str16ToStr8(data.ReadString16());
    std::string validDesc = Str16ToStr8(SECURE_ELEMENT_TOKEN);
    CHECK_REMOTE_DESC(remoteDesc, validDesc);

    HILOGI("SecureElement::CloseSeSessions start");
    std::u16string seName = data.ReadString16();
    sptr<IRemoteObject> secureElement = GetSecureElement(seName);
    if (secureElement == nullptr) {
        HILOGE("SecureElement::CloseSeSessions GetSecureElement return null.");
        return Nfc::ERR_REMOTE_SERVICE_DISCONNECTED;
    }
    // Send TRANSACTION
    MessageParcel newData;
    MessageOption option;
    WRITE_INTF_TOKEN(StringReplace(PKG_SE_READER), newData);
    HILOGI("SecureElement::CloseSeSessions Transact Start!");
    int32_t error = secureElement->SendRequest(TRANSACTION_CLOSE_SESSIONS, newData, reply, option);
    if (error != Nfc::ERR_NONE) {
        HILOGE("SecureElement::CloseSeSessions, call CloseSessions fail. error: %{public}d", error);
        return Nfc::ERR_REMOTE_TRANSACT_FAILED;
    } else {
        HILOGI("SecureElement::CloseSeSessions Transact Succeed");
    }
    /* Consume the exception code */
    int errorCode = reply.ReadInt32();
    if (errorCode != Nfc::ERR_NONE) {
        HILOGE("SecureElement::CloseSeSessions exception code = %d", errorCode);
        return Nfc::ERR_REMOTE_EXCEPTION_RETURN;
    }
    return Nfc::ERR_NONE;
}

// Get Remote Channel
sptr<IRemoteObject> SecureElement::GetRemoteChannel(sptr<IRemoteObject>& session,
    int type, MessageParcel& data, MessageParcel& reply)
{
    // use session to open channel, return the channel interface.
    MessageOption option;
    HILOGI("SecureElement::OpenChannel:GetRemoteChannel Type %{public}d Channel Transact start!", type);
    int error = Nfc::ERR_NONE;
    if (type == BASIC_CHANNEL) {
        error = session->SendRequest(TRANSACTION_OPEN_BASICHANNEL, data, reply, option);
    } else if (type == LOGICAL_CHANNEL) {
        error = session->SendRequest(TRANSACTION_OPEN_LOGICALCHANNEL, data, reply, option);
    } else {
        HILOGE("SecureElement::OpenChannel:GetRemoteChannel Channel Type Input Wrong!");
        return nullptr;
    }
    HILOGI("SecureElement::OpenChannel:GetRemoteChannel: Type %{public}d Channel Transact end!", type);

    // Check Transact result
    if (error != Nfc::ERR_NONE) {
        HILOGE("SecureElement::OpenChannel:GetRemoteChannel: Get Type %d Cha Fail.error: %{public}d", type, error);
        return nullptr;
    }

    // error code of open channel success
    int errorCode = reply.ReadInt32();
    if (errorCode != Nfc::ERR_NONE) {
        HILOGE("SecureElement::OpenChannel:GetRemoteChannel G T %d Ch Tra F. error: %{public}d", type, errorCode);
        return nullptr;
    }

    sptr<IRemoteObject> channel = reply.ReadObject<IRemoteObject>();
    if (channel == nullptr) {
        HILOGE("SecureElement::OpenChannel:GetRemoteChannel Get Type %{public}d Channel Fail!!", type);
        return nullptr;
    }
    return channel;
}

/* open basic or logical channel */
int32_t SecureElement::OpenChannel(MessageParcel& data, MessageParcel& reply)
{
    HILOGI("SecureElement::OpenChannel start");
    std::string remoteDesc = Str16ToStr8(data.ReadString16());
    std::string validDesc = Str16ToStr8(SECURE_ELEMENT_TOKEN);
    CHECK_REMOTE_DESC(remoteDesc, validDesc);

    // get remote session
    sptr<IRemoteObject> session = data.ReadRemoteObject();
    if (session == nullptr) {
        HILOGE("SecureElement::OpenChannel Cannot Get Session!!");
        return Nfc::ERR_REMOTE_SERVICE_DISCONNECTED;
    }
    int32_t type = data.ReadInt32();
    HILOGI("SecureElement::OpenChannel request channel type = %{public}d", type);

    // read info from data and write it into newData
    MessageParcel newData;
    MessageParcel newReply;
    WRITE_INTF_TOKEN(StringReplace(PKG_SE_SESSION), newData);

    // copy the aid into newData
    std::vector<uint8_t> aid;
    data.ReadUInt8Vector(&aid);
    newData.WriteUInt8Vector(aid);

    // copy the p2 into newData
    uint8_t p2 = 0xff;
    data.ReadUint8(p2);
    newData.WriteUint8(p2);
    HILOGI("SecureElement::OpenChannel Read from original data: Channel p2 = %{public}d", p2);

    // insert the callback (com.nxp.nfc.ISecureElementCallback)
    auto callback = std::make_unique<SecureElementCallbackStub>();
    newData.WriteRemoteObject(callback->AsObject());
    sptr<IRemoteObject> channel = GetRemoteChannel(session, type, newData, newReply);
    if (channel == nullptr) {
        HILOGE("SecureElement::OpenChannel Get Type %{public}d Channel Fail!!", type);
        return Nfc::ERR_REMOTE_SERVICE_DISCONNECTED;
    }
    reply.WriteInt32(Nfc::ERR_NONE); // Get Channel Id Successfully
    reply.WriteRemoteObject(channel);
    return Nfc::ERR_NONE;
}

int32_t SecureElement::CloseSessionChannels(MessageParcel& data, MessageParcel& reply)
{
    std::string remoteDesc = Str16ToStr8(data.ReadString16());
    std::string validDesc = Str16ToStr8(SECURE_ELEMENT_TOKEN);
    CHECK_REMOTE_DESC(remoteDesc, validDesc);
    HILOGI("SecureElement::CloseSessionChannels start");

    // get remote session
    sptr<IRemoteObject> session = data.ReadRemoteObject();
    if (session == nullptr) {
        HILOGE("SecureElement::CloseSessionChannels Cannot Get Session!!");
        return Nfc::ERR_REMOTE_SERVICE_DISCONNECTED;
    }

    // read info from data and write it into newData
    MessageParcel newData;
    MessageOption option;
    WRITE_INTF_TOKEN(StringReplace(PKG_SE_SESSION), newData);
    int error = session->SendRequest(TRANSACTION_CLOSE_CHANNELS, newData, reply, option);
    HILOGI("SecureElement::CloseSessionChannels Channels Close Transact end!");
    if (error != Nfc::ERR_NONE) {
        HILOGE("SecureElement::CloseSessionChannels NfcChannel_->Transact fail. error: %{public}d", error);
        return Nfc::ERR_REMOTE_TRANSACT_FAILED;
    }

    /* Consume the exception code */
    HILOGI("SecureElement::CloseSessionChannels NfcChannel_->Transact, Succeed!");
    int errorCode = reply.ReadInt32();
    if (errorCode != Nfc::ERR_NONE) {
        HILOGE("SecureElement::CloseSessionChannels Get Response from Transact Fail. error: %{public}d", errorCode);
        return Nfc::ERR_REMOTE_EXCEPTION_RETURN;
    }
    return Nfc::ERR_NONE;
}

int32_t SecureElement::GetSelectResponse(MessageParcel& data, MessageParcel& reply)
{
    std::string remoteDesc = Str16ToStr8(data.ReadString16());
    std::string validDesc = Str16ToStr8(SECURE_ELEMENT_TOKEN);
    CHECK_REMOTE_DESC(remoteDesc, validDesc);

    // check if the service is on
    if (!CheckSeServiceOn()) {
        HILOGE("SecureElement::GetSelectResponse == nullptr");
        return Nfc::ERR_REMOTE_SERVICE_DISCONNECTED;
    }
    HILOGI("SecureElement::GetSelectResponse start");
    sptr<IRemoteObject> channel = data.ReadRemoteObject();
    if (channel == nullptr) {
        HILOGE("SecureElement::GetSelectResponse Get Channel Fail!!");
        return Nfc::ERR_REMOTE_SERVICE_DISCONNECTED;
    }
    HILOGI("SecureElement::GetSelectResponse Channel get RSP Transact start!");

    // do get Response from channel of nxp service.
    MessageParcel newData;
    MessageOption option;
    WRITE_INTF_TOKEN(StringReplace(PKG_SE_CHANNEL), newData);
    int32_t error = channel->SendRequest(TRANSACTION_SELECT_RESPONSE, newData, reply, option);
    HILOGI("SecureElement::GetSelectResponse Channel get RSP Transact end!");
    if (error != Nfc::ERR_NONE) {
        HILOGE("SecureElement::GetSelectResponse NfcChannel_->Transact, fail Call SellectRSP. error: %d", error);
        return Nfc::ERR_REMOTE_TRANSACT_FAILED;
    }

    /* Consume the exception code */
    HILOGI("SecureElement::GetSelectResponse NfcChannel_->Transact, Succeed!");
    int errorCode = reply.ReadInt32();
    if (errorCode != Nfc::ERR_NONE) {
        HILOGE("SecureElement::GetSelectResponse Get Response from Transact Fail. error: %{public}d", errorCode);
        return Nfc::ERR_REMOTE_EXCEPTION_RETURN;
    }
    return Nfc::ERR_NONE;
}

int32_t SecureElement::TransmitApdu(MessageParcel& data, MessageParcel& reply)
{
    std::string remoteDesc = Str16ToStr8(data.ReadString16());
    std::string validDesc = Str16ToStr8(SECURE_ELEMENT_TOKEN);
    CHECK_REMOTE_DESC(remoteDesc, validDesc);

    // check if the service is on
    if (!CheckSeServiceOn()) {
        HILOGE("SecureElement::GetSelectResponse == nullptr");
        return Nfc::ERR_REMOTE_SERVICE_DISCONNECTED;
    }
    HILOGI("SecureElement::TransmitApdu start");
    sptr<IRemoteObject> channel = data.ReadRemoteObject();
    if (channel == nullptr) {
        HILOGE("SecureElement::TransmitApdu Get Channel Fail!!");
        return Nfc::ERR_REMOTE_SERVICE_DISCONNECTED;
    }

    // init the new data, copy the command into newData
    MessageParcel newData;
    MessageOption option;
    WRITE_INTF_TOKEN(StringReplace(PKG_SE_CHANNEL), newData);
    std::vector<uint8_t> cmd;
    data.ReadUInt8Vector(&cmd);
    newData.WriteUInt8Vector(cmd);

    // transmit apdu
    HILOGI("SecureElement::TransmitApdu Channel Transact start!");
    int32_t error = channel->SendRequest(TRANSACTION_TRANSMIT, newData, reply, option); // session open basic
    HILOGI("SecureElement::TransmitApdu Channel Transact end!");
    if (error != Nfc::ERR_NONE) {
        HILOGE("SecureElement::TransmitApdu NfcChannel_->Transact, fail to send Apdu. error: %{public}d", error);
        return Nfc::ERR_REMOTE_TRANSACT_FAILED;
    }
    HILOGI("SecureElement::TransmitApdu NfcChannel_->Transact, Succeed!");
    int errorCode = reply.ReadInt32();
    if (errorCode != Nfc::ERR_NONE) {
        HILOGE("SecureElement::TransmitApdu Get Response from Transact Fail. error: %{public}d", errorCode);
        return Nfc::ERR_REMOTE_EXCEPTION_RETURN;
    }
    return Nfc::ERR_NONE;
}

int32_t SecureElement::CloseChannel(MessageParcel& data, MessageParcel& reply)
{
    std::string remoteDesc = Str16ToStr8(data.ReadString16());
    std::string validDesc = Str16ToStr8(SECURE_ELEMENT_TOKEN);
    CHECK_REMOTE_DESC(remoteDesc, validDesc);

    // check if the service is on
    if (!CheckSeServiceOn()) {
        HILOGE("SecureElement::GetSelectResponse == nullptr");
        return Nfc::ERR_REMOTE_SERVICE_DISCONNECTED;
    }
    HILOGI("SecureElement::CloseChannel start");
    sptr<IRemoteObject> channel = data.ReadRemoteObject();
    if (channel == nullptr) {
        HILOGE("SecureElement::CloseChannel Get Channel Fail!!");
        return Nfc::ERR_REMOTE_SERVICE_DISCONNECTED;
    }
    HILOGI("SecureElement::CloseChannel Channel Close Transact start!");

    // do close channel to nxp service.
    MessageParcel newData;
    MessageOption option;
    WRITE_INTF_TOKEN(StringReplace(PKG_SE_CHANNEL), newData);
    int32_t error = channel->SendRequest(TRANSACTION_CLOSE, newData, reply, option);
    HILOGI("SecureElement::CloseChannel Channel Close Transact end!");
    if (error != Nfc::ERR_NONE) {
        HILOGE("SecureElement::CloseChannel NfcChannel_->Transact, fail. error: %d", error);
        return Nfc::ERR_REMOTE_TRANSACT_FAILED;
    }

    /* Consume the exception code */
    HILOGI("SecureElement::CloseChannel NfcChannel_->Transact, Succeed!");
    int errorCode = reply.ReadInt32();
    if (errorCode != Nfc::ERR_NONE) {
        HILOGE("SecureElement::CloseChannel Get Response from Transact Fail. error: %{public}d", errorCode);
        return Nfc::ERR_REMOTE_EXCEPTION_RETURN;
    }
    return Nfc::ERR_NONE;
}

int32_t SecureElement::IsChannelClosed(MessageParcel& data, MessageParcel& reply)
{
    std::string remoteDesc = Str16ToStr8(data.ReadString16());
    std::string validDesc = Str16ToStr8(SECURE_ELEMENT_TOKEN);
    CHECK_REMOTE_DESC(remoteDesc, validDesc);

    // check if the service is on
    if (!CheckSeServiceOn()) {
        HILOGE("SecureElement::GetSelectResponse == nullptr");
        return Nfc::ERR_REMOTE_SERVICE_DISCONNECTED;
    }
    HILOGI("SecureElement::IsChannelClosed start");
    sptr<IRemoteObject> channel = data.ReadRemoteObject();
    if (channel == nullptr) {
        HILOGE("SecureElement::IsChannelClosed Get Channel Fail!!");
        return Nfc::ERR_REMOTE_SERVICE_DISCONNECTED;
    }
    HILOGI("SecureElement::IsChannelClosed Channel State Check Transact start!");

    // do close channel to nxp service.
    MessageParcel newData;
    MessageOption option;
    WRITE_INTF_TOKEN(StringReplace(PKG_SE_CHANNEL), newData);
    int32_t error = channel->SendRequest(TRANSACTION_IS_CLOSED, newData, reply, option);
    HILOGI("SecureElement::IsChannelClosed Channel State Check Transact end!");
    if (error != Nfc::ERR_NONE) {
        HILOGE("SecureElement::IsChannelClosed NfcChannel_->Transact, fail. error: %d", error);
        return Nfc::ERR_REMOTE_TRANSACT_FAILED;
    }

    /* Consume the exception code */
    HILOGI("SecureElement::IsChannelClosed NfcChannel_->Transact, Succeed!");
    int errorCode = reply.ReadInt32();
    if (errorCode != Nfc::ERR_NONE) {
        HILOGE("SecureElement::IsChannelClosed Get Response from Transact Fail. error: %{public}d", errorCode);
        return Nfc::ERR_REMOTE_EXCEPTION_RETURN;
    }
    return Nfc::ERR_NONE;
}
} // namespace Tag
} // namespace Nfc
} // namespace OHOS

