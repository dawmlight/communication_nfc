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

#ifndef OHOS_I_MANAGER_MANAGER_H
#define OHOS_I_MANAGER_MANAGER_H

#include <memory>
#include <cstring>
#include "iremote_broker.h"
#include "ohos/aafwk/content/intent.h"

namespace OHOS {
namespace Nfc {
class INfcManager : public OHOS::IRemoteBroker {
public:
    enum {
        /* NFC open or close operations */
        SET_NFC_ENABLED = 1,
        GET_NFC_STATE = 2,
        HAS_CAPABILITY = 3,

        /* NFC Tag operations */
        CONNECT_TAG = 4,
        RECONNECT_TAG = 5,
        IS_TAG_CONNECTED = 6,
        SET_SEND_DATA_TIMEOUT = 7,
        GET_SEND_DATA_TIMEOUT = 8,
        SEND_DATA = 9,
        GET_MAX_SEND_LENGTH = 10,
        RESET_TIMEOUT = 11,

        /* Secure Element operations */
        GET_NFC_SERVICE = 12,
        BIND_SE_SERVICE = GET_NFC_SERVICE + 1,
        IS_SE_SERVICE_CONNECTED = GET_NFC_SERVICE + 2,
        GET_SECURE_ELEMENTS = GET_NFC_SERVICE + 3,
        IS_SECURE_ELEMENT_PRESENT = GET_NFC_SERVICE + 4,
        OPEN_CHANNEL = GET_NFC_SERVICE + 5,
        TRANSMIT_APDU = GET_NFC_SERVICE + 6,
        CLOSE_CHANNEL = GET_NFC_SERVICE + 7,

        /* Card Emulation operations */
        SET_LISTEN_MODE = 20,
        IS_LISTEN_MODE_ENABLED = SET_LISTEN_MODE + 1,
        GET_NFC_INFO = SET_LISTEN_MODE + 2,
        SET_RF_CONFIG = SET_LISTEN_MODE + 3,
        GET_INFO_FROM_CONFIG = SET_LISTEN_MODE + 4,
        GET_SELECT_RESPONSE = SET_LISTEN_MODE + 5,
        OPEN_SESSION = SET_LISTEN_MODE + 6,
        CLOSE_SE_SESSIONS = SET_LISTEN_MODE + 7,
        CLOSE_SESSION_CHANNELS = SET_LISTEN_MODE + 8,
        GET_ATR = SET_LISTEN_MODE + 9,
        IS_CHANNEL_CLOSED = SET_LISTEN_MODE + 10,

        /* NFC Tag operations 2.0 */
        IS_NDEF = 31,
        NDEF_READ = IS_NDEF + 1,
        NDEF_WRITE = IS_NDEF + 2,
        NDEF_SET_READONLY = IS_NDEF + 3,
        CAN_SET_READONLY = IS_NDEF + 4,

        /* Card Emulation operations 2.0 */
        SET_FOREGROUND_SERVICE = 36,
        UNSET_FOREGROUND_SERVICE = SET_FOREGROUND_SERVICE + 1,
        IS_DEFAULT_FOR_AID = SET_FOREGROUND_SERVICE + 2,
        REGISTER_AIDS = SET_FOREGROUND_SERVICE + 3,
        REMOVE_AIDS = SET_FOREGROUND_SERVICE + 4,
        GET_AIDS = SET_FOREGROUND_SERVICE + 5,
        IS_DEFAULT_FOR_TYPE = SET_FOREGROUND_SERVICE + 6,

        /* NFC Controller operations 2.0.1 */
        REGISTER_FOREGROUND_DISPATCH = 43,
        SET_READER_MODE = 44
    };

    /* NFC open or close operations */
    virtual int32_t SetNfcEnabled(MessageParcel& data, MessageParcel& reply) = 0;
    virtual int32_t GetNfcState(MessageParcel& data, MessageParcel& reply) = 0;
    virtual bool IsNfcAvailable(MessageParcel& data, MessageParcel& reply) = 0;

    /* NFC Tag operations */
    virtual bool ConnectTag(MessageParcel& data, MessageParcel& reply) = 0;
    virtual bool IsTagConnected(MessageParcel& data, MessageParcel& reply) = 0;
    virtual bool SetSendDataTimeout(MessageParcel& data, MessageParcel& reply) = 0;
    virtual int32_t GetSendDataTimeout(MessageParcel& data, MessageParcel& reply) = 0;
    virtual int32_t SendData(MessageParcel& data, MessageParcel& reply) = 0;
    virtual int32_t GetMaxSendLength(MessageParcel& data, MessageParcel& reply) = 0;
    virtual bool ReconnectTag(MessageParcel& data, MessageParcel& reply) = 0;
    virtual void ResetTimeouts(MessageParcel& data, MessageParcel& reply) = 0;

    /* SecureElement operation */
    virtual sptr<IRemoteObject> GetRemoteNfcService() = 0;
    virtual int32_t BindSeService(MessageParcel& data, MessageParcel& reply) = 0;
    virtual int32_t IsSeServiceConnected(MessageParcel& data, MessageParcel& reply) = 0;
    virtual int32_t GetSecureElements(MessageParcel& data, MessageParcel& reply) = 0;
    virtual sptr<IRemoteObject> GetSecureElement(std::u16string seName) = 0;
    virtual int32_t IsSecureElementPresent(MessageParcel& data, MessageParcel& reply) = 0;
    virtual int32_t OpenSession(MessageParcel& data, MessageParcel& reply) = 0;
    virtual int32_t GetAtr(MessageParcel& data, MessageParcel& reply) = 0;
    virtual int32_t OpenChannel(MessageParcel& data, MessageParcel& reply) = 0;
    virtual int32_t TransmitApdu(MessageParcel& data, MessageParcel& reply) = 0;
    virtual int32_t CloseChannel(MessageParcel& data, MessageParcel& reply) = 0;
    virtual int32_t IsChannelClosed(MessageParcel& data, MessageParcel& reply) = 0;
    virtual int32_t GetSelectResponse(MessageParcel& data, MessageParcel& reply) = 0;
    virtual int32_t CloseSeSessions(MessageParcel& data, MessageParcel& reply) = 0;
    virtual int32_t CloseSessionChannels(MessageParcel& data, MessageParcel& reply) = 0;

    /* Card Emulation operations */
    virtual int32_t SetListenMode(MessageParcel& data, MessageParcel& reply) = 0;
    virtual int32_t IsListenModeEnabled(MessageParcel& data, MessageParcel& reply) = 0;
    virtual int32_t GetNfcInfo(MessageParcel& data, MessageParcel& reply) = 0;
    virtual int32_t SetRfConfig(MessageParcel& data, MessageParcel& reply) = 0;
    virtual int32_t GetInfoFromConfig(MessageParcel& data, MessageParcel& reply) = 0;
    virtual int32_t SetForegroundPreferred(MessageParcel& data, MessageParcel& reply) = 0;
    virtual int32_t UnsetForegroundPreferred(MessageParcel& data, MessageParcel& reply) = 0;
    virtual int32_t IsDefaultForAid(MessageParcel& data, MessageParcel& reply) = 0;
    virtual int32_t RegisterAids(MessageParcel& data, MessageParcel& reply) = 0;
    virtual int32_t RemoveAids(MessageParcel& data, MessageParcel& reply) = 0;
    virtual int32_t GetAids(MessageParcel& data, MessageParcel& reply) = 0;
    virtual int32_t IsDefaultForType(MessageParcel& data, MessageParcel& reply) = 0;

    /* NFC Tag operations 2.0 */
    virtual int32_t IsNdefTag(MessageParcel& data, MessageParcel& reply) = 0;
    virtual int32_t NdefRead(MessageParcel& data, MessageParcel& reply) = 0;
    virtual int32_t NdefWrite(MessageParcel& data, MessageParcel& reply) = 0;
    virtual int32_t NdefSetReadOnly(MessageParcel& data, MessageParcel& reply) = 0;
    virtual int32_t CanSetReadOnly(MessageParcel& data, MessageParcel& reply) = 0;

    /* NFC Controller operations 2.0.1 */
    virtual void RegisterForegroundDispatch(MessageParcel& data, MessageParcel& reply) = 0;
    virtual bool SetReaderMode(MessageParcel& data, MessageParcel& reply) = 0;

public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.ipc.NfcManager");
};
} // namespace Nfc
} // namespace OHOS
#endif // OHOS_I_MANAGER_MANAGER_H