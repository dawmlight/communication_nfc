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

#include "nfc_manager_stub.h"

#include <cstring>

#include "ipc_types.h"
#include "nfc_log.h"
#include "string_ex.h"

namespace OHOS {
namespace Nfc {
static std::vector<sptr<Parcelable>> holders_;

NfcManagerStub::NfcManagerStub()
{
    /* NFC open or close operations */
    memberFuncMap_[SET_NFC_ENABLED] = &NfcManagerStub::SetNfcEnabledInner;
    memberFuncMap_[HAS_CAPABILITY] = &NfcManagerStub::IsNfcAvailableInner;
    memberFuncMap_[GET_NFC_STATE] = &NfcManagerStub::GetNfcStateInner;

    /* NFC Tag operations */
    memberFuncMap_[CONNECT_TAG] = &NfcManagerStub::ConnectTagInner;
    memberFuncMap_[IS_TAG_CONNECTED] = &NfcManagerStub::IsTagConnectedInner;
    memberFuncMap_[SET_SEND_DATA_TIMEOUT] = &NfcManagerStub::SetSendDataTimeoutInner;
    memberFuncMap_[GET_SEND_DATA_TIMEOUT] = &NfcManagerStub::GetSendDataTimeoutInner;
    memberFuncMap_[GET_MAX_SEND_LENGTH] = &NfcManagerStub::GetMaxSendLengthInner;
    memberFuncMap_[SEND_DATA] = &NfcManagerStub::SendDataInner;
    memberFuncMap_[RECONNECT_TAG] = &NfcManagerStub::ReconnectTagInner;
    memberFuncMap_[RESET_TIMEOUT] = &NfcManagerStub::ResetTimeoutsInner;
    memberFuncMap_[GET_NFC_SERVICE] = &NfcManagerStub::GetNfcServiceInner;
    memberFuncMap_[BIND_SE_SERVICE] = &NfcManagerStub::BindSeServiceInner;
    memberFuncMap_[IS_SE_SERVICE_CONNECTED] = &NfcManagerStub::IsSeServiceConnectedInner;
    memberFuncMap_[GET_SECURE_ELEMENTS] = &NfcManagerStub::GetSecureElementsInner;

    /* NFC OMA operation */
    memberFuncMap_[IS_SECURE_ELEMENT_PRESENT] = &NfcManagerStub::IsSecureElementPresentInner;
    memberFuncMap_[OPEN_CHANNEL] = &NfcManagerStub::OpenChannelInner;
    memberFuncMap_[TRANSMIT_APDU] = &NfcManagerStub::TransmitApduInner;
    memberFuncMap_[CLOSE_CHANNEL] = &NfcManagerStub::CloseChannelInner;
    memberFuncMap_[IS_CHANNEL_CLOSED] = &NfcManagerStub::IsChannelClosedInner;
    memberFuncMap_[GET_SELECT_RESPONSE] = &NfcManagerStub::GetSelectResponseInner;
    memberFuncMap_[CLOSE_SE_SESSIONS] = &NfcManagerStub::CloseSeSessionsInner;
    memberFuncMap_[OPEN_SESSION] = &NfcManagerStub::OpenSessionInner;
    memberFuncMap_[GET_ATR] = &NfcManagerStub::GetAtrInner;
    memberFuncMap_[CLOSE_SESSION_CHANNELS] = &NfcManagerStub::CloseSessionChannelsInner;

    /* NFC card emulation operation */
    memberFuncMap_[SET_LISTEN_MODE] = &NfcManagerStub::SetListenModeInner;
    memberFuncMap_[IS_LISTEN_MODE_ENABLED] = &NfcManagerStub::IsListenModeEnabledInner;
    memberFuncMap_[GET_NFC_INFO] = &NfcManagerStub::GetNfcInfoInner;
    memberFuncMap_[SET_RF_CONFIG] = &NfcManagerStub::SetRfConfigInner;
    memberFuncMap_[GET_INFO_FROM_CONFIG] = &NfcManagerStub::GetInfoFromConfigInner;
    memberFuncMap_[SET_FOREGROUND_SERVICE] = &NfcManagerStub::SetForegroundPreferredInner;
    memberFuncMap_[UNSET_FOREGROUND_SERVICE] = &NfcManagerStub::UnsetForegroundPreferredInner;
    memberFuncMap_[IS_DEFAULT_FOR_AID] = &NfcManagerStub::IsDefaultForAidInner;
    memberFuncMap_[REGISTER_AIDS] = &NfcManagerStub::RegisterAidsInner;
    memberFuncMap_[REMOVE_AIDS] = &NfcManagerStub::RemoveAidsInner;
    memberFuncMap_[GET_AIDS] = &NfcManagerStub::GetAidsInner;
    memberFuncMap_[IS_DEFAULT_FOR_TYPE] = &NfcManagerStub::IsDefaultForTypeInner;

    /* NFC Tag operations 2.0 */
    memberFuncMap_[IS_NDEF] = &NfcManagerStub::IsNdefTagInner;
    memberFuncMap_[NDEF_READ] = &NfcManagerStub::NdefReadInner;
    memberFuncMap_[NDEF_WRITE] = &NfcManagerStub::NdefWriteInner;
    memberFuncMap_[CAN_SET_READONLY] = &NfcManagerStub::CanSetReadOnlyInner;

    /* NFC Controller operations 2.0.1 */
    memberFuncMap_[REGISTER_FOREGROUND_DISPATCH] = &NfcManagerStub::RegisterForegroundDispatchInner;
    memberFuncMap_[SET_READER_MODE] = &NfcManagerStub::SetReaderModeInner;
}

NfcManagerStub::~NfcManagerStub()
{
    memberFuncMap_.clear();
}

int32_t NfcManagerStub::OnRemoteRequest(uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    auto itFunc = memberFuncMap_.find(code);
    if (itFunc != memberFuncMap_.end()) {
        auto memberFunc = itFunc->second;
        if (memberFunc != nullptr) {
            return (this->*memberFunc)(data, reply);
        }
    }
    HILOGI("NfcManagerStub::OnRemoteRequest, unknown code = %{public}d", code);
    return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
}

int32_t NfcManagerStub::SetNfcEnabledInner(MessageParcel& data, MessageParcel& reply)
{
    int32_t result = SetNfcEnabled(data, reply);
    HILOGW("NfcManagerStub: setNfcEnabledInner reply to client data = %{public}d", result);
    if (!reply.WriteInt32(result)) {
        HILOGW("NfcManagerStub: SetNfcEnabledInner write reply failed.");
    }
    return result;
}

int32_t NfcManagerStub::GetNfcStateInner(MessageParcel& data, MessageParcel& reply)
{
    int32_t result = GetNfcState(data, reply);
    HILOGW("NfcManagerStub: getNfcStateInner reply to client data = %{public}d", result);
    if (!reply.WriteInt32(result)) {
        HILOGW("NfcManagerStub: GetNfcStateInner write reply failed.");
    }
    return 0;
}

int32_t NfcManagerStub::IsNfcAvailableInner(MessageParcel& data, MessageParcel& reply)
{
    bool result = IsNfcAvailable(data, reply);
    HILOGW("NfcManagerStub: IsNfcAvailableInner reply to client data = %{public}d", result);
    if (!reply.WriteBool(result)) {
        HILOGW("NfcManagerStub: IsNfcAvailableInner write reply failed.");
    }
    return 0;
}

int32_t NfcManagerStub::RegisterForegroundDispatchInner(MessageParcel& data, MessageParcel& reply)
{
    HILOGI("NfcManagerStub: RegisterForegroundDispatchInner in");
    RegisterForegroundDispatch(data, reply);
    HILOGI("NfcManagerStub: RegisterForegroundDispatchInner reply to client");
    return 0;
}

int32_t NfcManagerStub::SetReaderModeInner(MessageParcel& data, MessageParcel& reply)
{
    HILOGI("NfcManagerStub: SetReaderModeInner in");
    int result = SetReaderMode(data, reply);
    HILOGI("NfcManagerStub: SetReaderModeInner reply to client");
    if (result != 0) {
        HILOGW("NfcManagerStub: SetReaderModeInner write reply failed.");
    }
    return result;
}

int32_t NfcManagerStub::ConnectTagInner(MessageParcel& data, MessageParcel& reply)
{
    bool result = ConnectTag(data, reply);
    HILOGW("NfcManagerStub: ConnectTagInner reply to client data = %{public}d", result);
    if (!reply.WriteBool(result)) {
        HILOGW("NfcManagerStub: ConnectTagInner write reply failed.");
    }
    return 0;
}

int32_t NfcManagerStub::ReconnectTagInner(MessageParcel& data, MessageParcel& reply)
{
    bool result = ReconnectTag(data, reply);
    HILOGW("NfcManagerStub: ReconnectTagInner reply to client data = %{public}d", result);
    if (!reply.WriteBool(result)) {
        HILOGW("NfcManagerStub: ReconnectTagInner write reply failed.");
    }
    return 0;
}

int32_t NfcManagerStub::ResetTimeoutsInner(MessageParcel& data, MessageParcel& reply)
{
    ResetTimeouts(data, reply);
    HILOGW("NfcManagerStub: ResetTimeoutsInner reply to client");
    return 0;
}

int32_t NfcManagerStub::IsTagConnectedInner(MessageParcel& data, MessageParcel& reply)
{
    bool result = IsTagConnected(data, reply);
    HILOGW("NfcManagerStub: IsTagConnectedInner reply to client data = %{public}d", result);
    if (!reply.WriteBool(result)) {
        HILOGW("NfcManagerStub: IsTagConnectedInner write reply failed.");
    }
    return 0;
}

int32_t NfcManagerStub::SetSendDataTimeoutInner(MessageParcel& data, MessageParcel& reply)
{
    bool result = SetSendDataTimeout(data, reply);
    HILOGW("NfcManagerStub: SetSendDataTimeoutInner reply to client data = %{public}d", result);
    if (!reply.WriteBool(result)) {
        HILOGW("NfcManagerStub: SetSendDataTimeoutInner write reply failed.");
    }
    return 0;
}

int32_t NfcManagerStub::GetSendDataTimeoutInner(MessageParcel& data, MessageParcel& reply)
{
    int32_t result = GetSendDataTimeout(data, reply);
    HILOGW("NfcManagerStub: GetSendDataTimeoutInner reply to client data = %{public}d", result);
    if (!reply.WriteInt32(result)) {
        HILOGW("NfcManagerStub: GetSendDataTimeoutInner write reply failed.");
    }
    return 0;
}

int32_t NfcManagerStub::GetMaxSendLengthInner(MessageParcel& data, MessageParcel& reply)
{
    int32_t result = GetMaxSendLength(data, reply);
    HILOGW("NfcManagerStub: GetMaxSendLengthInner reply to client data = %{public}d", result);
    if (!reply.WriteInt32(result)) {
        HILOGW("NfcManagerStub: GetMaxSendLengthInner write reply failed.");
    }
    return 0;
}

int32_t NfcManagerStub::SendDataInner(MessageParcel& data, MessageParcel& reply)
{
    HILOGW("NfcManagerStub: SendDataInner start");
    int result = SendData(data, reply);
    HILOGI("NfcManagerStub: SendData reply to client");
    // 0: NfcServiceName::ERR_NONE
    if (result != 0) {
        HILOGW("NfcManagerStub: SendDataInner write reply failed.");
    }
    return result;
}

int32_t NfcManagerStub::GetNfcServiceInner(MessageParcel& data, MessageParcel& reply)
{
    HILOGW("NfcManagerStub::getNfcServiceInner entry");
    sptr<IRemoteObject> nfcService = GetRemoteNfcService();
    if (nfcService == nullptr) {
        HILOGW("NfcManagerStub::getNfcServiceInner  GetNfcService is null");
        return -1;
    }

    bool ret = false;
    holders_.push_back(nfcService);
    ret = reply.WriteObject<IRemoteObject>(nfcService);
    if (!ret) {
        HILOGW("NfcManagerStub::getNfcServiceInner write reply failed");
        return -1;
    }

    HILOGI("NfcManagerStub::getNfcServiceInner ret %{public}d", ret);
    return 0;
}

int32_t NfcManagerStub::BindSeServiceInner(MessageParcel& data, MessageParcel& reply)
{
    HILOGI("NfcManagerStub: BindSeServiceInner start");
    int result = BindSeService(data, reply);
    HILOGI("NfcManagerStub: BindSeServiceInner reply to client");
    if (result != 0) {
        HILOGW("NfcManagerStub: BindSeServiceInner write reply failed.");
    }
    return result;
}

int32_t NfcManagerStub::IsSeServiceConnectedInner(MessageParcel& data, MessageParcel& reply)
{
    HILOGW("NfcManagerStub::IsSeServiceConnectedInner entry");
    int result = IsSeServiceConnected(data, reply);
    if (result != 0) {
        HILOGW("NfcManagerStub::IsSeServiceConnectedInner  GetSeService is null");
    }
    return result;
}

int32_t NfcManagerStub::GetSecureElementsInner(MessageParcel& data, MessageParcel& reply)
{
    HILOGI("NfcManagerStub: GetSecureElements start");
    int result = GetSecureElements(data, reply);
    HILOGI("NfcManagerStub: GetSecureElements reply to client");
    if (result != 0) {
        HILOGW("NfcManagerStub: GetSecureElements write reply failed.");
    }
    return result;
}

int32_t NfcManagerStub::IsSecureElementPresentInner(MessageParcel& data, MessageParcel& reply)
{
    int result = IsSecureElementPresent(data, reply);
    HILOGI("NfcManagerStub: IsSecureElementPresentInner reply to client");
    if (result != 0) {
        HILOGW("NfcManagerStub: IsSecureElementPresentInner write reply failed.");
    }
    return result;
}

int32_t NfcManagerStub::OpenChannelInner(MessageParcel& data, MessageParcel& reply)
{
    int result = OpenChannel(data, reply);
    HILOGI("NfcManagerStub: OpenChannelInner reply to client");
    if (result != 0) {
        HILOGW("NfcManagerStub: OpenChannelInner write reply failed.");
    }
    return result;
}

int32_t NfcManagerStub::TransmitApduInner(MessageParcel& data, MessageParcel& reply)
{
    int result = TransmitApdu(data, reply);
    HILOGI("NfcManagerStub: TransmitApduInner reply to client");
    if (result != 0) {
        HILOGW("NfcManagerStub: TransmitApduInner write reply failed.");
    }
    return result;
}

int32_t NfcManagerStub::CloseChannelInner(MessageParcel& data, MessageParcel& reply)
{
    int result = CloseChannel(data, reply);
    HILOGI("NfcManagerStub: CloseChannelInner reply to client");
    if (result != 0) {
        HILOGW("NfcManagerStub: CloseChannelInner write reply failed.");
    }
    return result;
}

int32_t NfcManagerStub::GetSelectResponseInner(MessageParcel& data, MessageParcel& reply)
{
    int result = GetSelectResponse(data, reply);
    HILOGI("NfcManagerStub: GetSelectResponseInner reply to client");
    if (result != 0) {
        HILOGW("NfcManagerStub: GetSelectResponseInner write reply failed.");
    }
    return result;
}

int32_t NfcManagerStub::CloseSeSessionsInner(MessageParcel& data, MessageParcel& reply)
{
    int result = CloseSeSessions(data, reply);
    HILOGI("NfcManagerStub: CloseSeSessionsInner reply to client");
    if (result != 0) {
        HILOGW("NfcManagerStub: CloseSeSessionsInner write reply failed.");
    }
    return result;
}

int32_t NfcManagerStub::SetListenModeInner(MessageParcel& data, MessageParcel& reply)
{
    int result = SetListenMode(data, reply);
    HILOGI("NfcManagerStub: SetListenModeInner reply to client");
    if (result != 0) {
        HILOGW("NfcManagerStub: SetListenModeInner write reply failed.");
    }
    return result;
}

int32_t NfcManagerStub::IsListenModeEnabledInner(MessageParcel& data, MessageParcel& reply)
{
    int result = IsListenModeEnabled(data, reply);
    HILOGI("NfcManagerStub: IsListenModeEnabled reply to client");
    if (result != 0) {
        HILOGW("NfcManagerStub: IsListenModeEnabledInner write reply failed.");
    }
    return result;
}

int32_t NfcManagerStub::GetNfcInfoInner(MessageParcel& data, MessageParcel& reply)
{
    int result = GetNfcInfo(data, reply);
    HILOGI("NfcManagerStub: GetNfcInfoInner reply to client");
    if (result != 0) {
        HILOGW("NfcManagerStub: GetNfcInfoInner write reply failed.");
    }
    return result;
}

int32_t NfcManagerStub::SetRfConfigInner(MessageParcel& data, MessageParcel& reply)
{
    int result = SetRfConfig(data, reply);
    HILOGI("NfcManagerStub: SetRfConfigInner reply to client");
    if (result != 0) {
        HILOGW("NfcManagerStub: SetRfConfigInner write reply failed.");
    }
    return result;
}

int32_t NfcManagerStub::OpenSessionInner(MessageParcel& data, MessageParcel& reply)
{
    int result = OpenSession(data, reply);
    HILOGI("NfcManagerStub: OpenSessionInner reply to client");
    if (result != 0) {
        HILOGW("NfcManagerStub: OpenSessionInner write reply failed.");
    }
    return result;
}

int32_t NfcManagerStub::GetInfoFromConfigInner(MessageParcel& data, MessageParcel& reply)
{
    int result = GetInfoFromConfig(data, reply);
    HILOGI("NfcManagerStub: GetInfoFromConfigInner reply to client");
    if (result != 0) {
        HILOGW("NfcManagerStub: GetInfoFromConfigInner write reply failed.");
    }
    return result;
}

int32_t NfcManagerStub::SetForegroundPreferredInner(MessageParcel& data, MessageParcel& reply)
{
    int result = SetForegroundPreferred(data, reply);
    HILOGI("NfcManagerStub: SetForegroundPreferredInner reply to client");
    if (result != 0) {
        HILOGW("NfcManagerStub: SetForegroundPreferredInner write reply failed.");
    }
    return result;
}

int32_t NfcManagerStub::UnsetForegroundPreferredInner(MessageParcel& data, MessageParcel& reply)
{
    int result = UnsetForegroundPreferred(data, reply);
    HILOGI("NfcManagerStub: UnsetForegroundPreferredInner reply to client");
    if (result != 0) {
        HILOGW("NfcManagerStub: UnsetForegroundPreferredInner write reply failed.");
    }
    return result;
}

int32_t NfcManagerStub::IsDefaultForAidInner(MessageParcel& data, MessageParcel& reply)
{
    int result = IsDefaultForAid(data, reply);
    HILOGI("NfcManagerStub: IsDefaultForAidInner reply to client");
    if (result != 0) {
        HILOGW("NfcManagerStub: IsDefaultForAidInner write reply failed.");
    }
    return result;
}

int32_t NfcManagerStub::RegisterAidsInner(MessageParcel& data, MessageParcel& reply)
{
    int result = RegisterAids(data, reply);
    HILOGI("NfcManagerStub: RegisterAidsInner reply to client");
    if (result != 0) {
        HILOGW("NfcManagerStub: RegisterAidsInner write reply failed.");
    }
    return result;
}

int32_t NfcManagerStub::RemoveAidsInner(MessageParcel& data, MessageParcel& reply)
{
    int result = RemoveAids(data, reply);
    HILOGI("NfcManagerStub: RemoveAidsInner reply to client");
    if (result != 0) {
        HILOGW("NfcManagerStub: RemoveAidsInner write reply failed.");
    }
    return result;
}

int32_t NfcManagerStub::GetAidsInner(MessageParcel& data, MessageParcel& reply)
{
    int result = GetAids(data, reply);
    HILOGI("NfcManagerStub: GetAidsInner reply to client");
    if (result != 0) {
        HILOGW("NfcManagerStub: GetAidsInner write reply failed.");
    }
    return result;
}

int32_t NfcManagerStub::IsDefaultForTypeInner(MessageParcel& data, MessageParcel& reply)
{
    int result = IsDefaultForType(data, reply);
    HILOGI("NfcManagerStub: IsDefaultForTypeInner reply to client");
    if (result != 0) {
        HILOGW("NfcManagerStub: IsDefaultForTypeInner write reply failed.");
    }
    return result;
}

int32_t NfcManagerStub::CloseSessionChannelsInner(MessageParcel& data, MessageParcel& reply)
{
    int result = CloseSessionChannels(data, reply);
    HILOGI("NfcManagerStub: CloseSessionChannelsInner reply to client");
    if (result != 0) {
        HILOGW("NfcManagerStub: CloseSessionChannelsInner write reply failed.");
    }
    return result;
}

int32_t NfcManagerStub::IsChannelClosedInner(MessageParcel& data, MessageParcel& reply)
{
    int result = IsChannelClosed(data, reply);
    HILOGI("NfcManagerStub: IsChannelClosedInner reply to client");
    if (result != 0) {
        HILOGW("NfcManagerStub: IsChannelClosedInner write reply failed.");
    }
    return result;
}

int32_t NfcManagerStub::GetAtrInner(MessageParcel& data, MessageParcel& reply)
{
    int result = GetAtr(data, reply);
    HILOGI("NfcManagerStub: GetAtrInner reply to client");
    if (result != 0) {
        HILOGW("NfcManagerStub: GetAtrInner write reply failed.");
    }
    return result;
}

/* NFC Tag operations 2.0 */
int32_t NfcManagerStub::IsNdefTagInner(MessageParcel& data, MessageParcel& reply)
{
    int result = IsNdefTag(data, reply);
    HILOGI("NfcManagerStub: IsNdefTagInner reply to client");
    if (result != 0) {
        HILOGW("NfcManagerStub: IsNdefTagInner write reply failed.");
    }
    return result;
}

int32_t NfcManagerStub::NdefReadInner(MessageParcel& data, MessageParcel& reply)
{
    int result = NdefRead(data, reply);
    HILOGI("NfcManagerStub: NdefReadInner reply to client");
    if (result != 0) {
        HILOGW("NfcManagerStub: NdefReadInner write reply failed.");
    }
    return result;
}

int32_t NfcManagerStub::NdefWriteInner(MessageParcel& data, MessageParcel& reply)
{
    int result = NdefWrite(data, reply);
    HILOGI("NfcManagerStub: NdefWriteInner reply to client");
    if (result != 0) {
        HILOGW("NfcManagerStub: NdefWriteInner write reply failed.");
    }
    return result;
}

int32_t NfcManagerStub::NdefSetReadOnlyInner(MessageParcel& data, MessageParcel& reply)
{
    int result = NdefSetReadOnly(data, reply);
    HILOGI("NfcManagerStub: NdefSetReadOnlyInner reply to client");
    if (result != 0) {
        HILOGW("NfcManagerStub: NdefSetReadOnlyInner write reply failed.");
    }
    return result;
}

int32_t NfcManagerStub::CanSetReadOnlyInner(MessageParcel& data, MessageParcel& reply)
{
    int result = CanSetReadOnly(data, reply);
    HILOGI("NfcManagerStub: CanSetReadOnlyInner reply to client");
    if (result != 0) {
        HILOGW("NfcManagerStub: CanSetReadOnlyInner write reply failed.");
    }
    return result;
}
} // namespace Nfc
} // namespace OHOS