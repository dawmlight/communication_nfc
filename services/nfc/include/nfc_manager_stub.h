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

#ifndef OHOS_NFC_MANAGER_STUB_H
#define OHOS_NFC_MANAGER_STUB_H

#include <map>
#include "iremote_stub.h"
#include "i_nfc_manager.h"

namespace OHOS {
namespace Nfc {
class NfcManagerStub : public IRemoteStub<INfcManager> {
public:
    NfcManagerStub();
    ~NfcManagerStub();

    int32_t OnRemoteRequest(uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option) override;
private:
    using NfcManagerFunc = int32_t (NfcManagerStub::*)(MessageParcel& data, MessageParcel& reply);

    /* NFC open or close operations */
    int32_t SetNfcEnabledInner(MessageParcel& data, MessageParcel& reply);
    int32_t GetNfcStateInner(MessageParcel& data, MessageParcel& reply);
    int32_t IsNfcAvailableInner(MessageParcel& data, MessageParcel& reply);

    /* NFC Tag operations */
    int32_t ConnectTagInner(MessageParcel& data, MessageParcel& reply);
    int32_t IsTagConnectedInner(MessageParcel& data, MessageParcel& reply);
    int32_t SetSendDataTimeoutInner(MessageParcel& data, MessageParcel& reply);
    int32_t GetSendDataTimeoutInner(MessageParcel& data, MessageParcel& reply);
    int32_t GetMaxSendLengthInner(MessageParcel& data, MessageParcel& reply);
    int32_t ReconnectTagInner(MessageParcel& data, MessageParcel& reply);
    int32_t ResetTimeoutsInner(MessageParcel& data, MessageParcel& reply);
    int32_t SendDataInner(MessageParcel& data, MessageParcel& reply);

    /* NFC Secure Element operations */
    int32_t GetNfcServiceInner(MessageParcel& data, MessageParcel& reply);
    int32_t BindSeServiceInner(MessageParcel& data, MessageParcel& reply);
    int32_t IsSeServiceConnectedInner(MessageParcel& data, MessageParcel& reply);
    int32_t GetSecureElementsInner(MessageParcel& data, MessageParcel& reply);
    int32_t IsSecureElementPresentInner(MessageParcel& data, MessageParcel& reply);
    int32_t OpenChannelInner(MessageParcel& data, MessageParcel& reply);
    int32_t TransmitApduInner(MessageParcel& data, MessageParcel& reply);
    int32_t CloseChannelInner(MessageParcel& data, MessageParcel& reply);
    int32_t IsChannelClosedInner(MessageParcel& data, MessageParcel& reply);
    int32_t GetSelectResponseInner(MessageParcel& data, MessageParcel& reply);
    int32_t OpenSessionInner(MessageParcel& data, MessageParcel& reply);
    int32_t CloseSeSessionsInner(MessageParcel& data, MessageParcel& reply);
    int32_t CloseSessionChannelsInner(MessageParcel& data, MessageParcel& reply);
    int32_t GetAtrInner(MessageParcel& data, MessageParcel& reply);

    /* for listen mode */
    int32_t SetListenModeInner(MessageParcel& data, MessageParcel& reply);
    int32_t IsListenModeEnabledInner(MessageParcel& data, MessageParcel& reply);
    /* for get NFCC and eSE info */
    int32_t GetNfcInfoInner(MessageParcel& data, MessageParcel& reply);
    /* for set RF parameter */
    int32_t SetRfConfigInner(MessageParcel& data, MessageParcel& reply);
    int32_t GetInfoFromConfigInner(MessageParcel& data, MessageParcel& reply);
    /* for set foreground app */
    int32_t SetForegroundPreferredInner(MessageParcel& data, MessageParcel& reply);
    int32_t UnsetForegroundPreferredInner(MessageParcel& data, MessageParcel& reply);

    /* for register aids */
    int32_t IsDefaultForAidInner(MessageParcel& data, MessageParcel& reply);
    int32_t RegisterAidsInner(MessageParcel& data, MessageParcel& reply);
    int32_t RemoveAidsInner(MessageParcel& data, MessageParcel& reply);
    int32_t GetAidsInner(MessageParcel& data, MessageParcel& reply);
    int32_t IsDefaultForTypeInner(MessageParcel& data, MessageParcel& reply);

    /* NFC Tag operations 2.0 */
    int32_t IsNdefTagInner(MessageParcel& data, MessageParcel& reply);
    int32_t NdefReadInner(MessageParcel& data, MessageParcel& reply);
    int32_t NdefWriteInner(MessageParcel& data, MessageParcel& reply);
    int32_t NdefSetReadOnlyInner(MessageParcel& data, MessageParcel& reply);
    int32_t CanSetReadOnlyInner(MessageParcel& data, MessageParcel& reply);

    /* NFC Controller operations 2.0.1 */
    int32_t RegisterForegroundDispatchInner(MessageParcel& data, MessageParcel& reply);
    int32_t SetReaderModeInner(MessageParcel& data, MessageParcel& reply);

    std::map<uint32_t, NfcManagerFunc> memberFuncMap_;
};
} // namespace Nfc
} // namespace OHOS
#endif // OHOS_NFC_MANAGER_STUB_H
