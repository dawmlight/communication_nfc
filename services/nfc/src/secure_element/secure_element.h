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

#ifndef OHOS_SECURE_ELEMENT_H
#define OHOS_SECURE_ELEMENT_H

#include "iremote_object.h"
#include "nfc_adapter_utils.h"
#include "nfc_secureelement_adapter_utils.h"

namespace OHOS {
namespace Nfc {
namespace Oma {
class SecureElement {
public:
    static SecureElement& GetInstance();
    ~SecureElement();
    sptr<IRemoteObject> GetRemoteSeService();

    sptr<IRemoteObject> GetSecureElement(std::u16string seName);
    sptr<IRemoteObject> GetRemoteChannel(sptr<IRemoteObject>& session,
        int type, MessageParcel& data, MessageParcel& reply);
    int32_t BindSeService(MessageParcel& data, MessageParcel& reply);
    int32_t IsSeServiceConnected(MessageParcel& data, MessageParcel& reply);
    int32_t GetSecureElements(MessageParcel& data, MessageParcel& reply);
    int32_t IsSecureElementPresent(MessageParcel& data, MessageParcel& reply);
    int32_t OpenSession(MessageParcel& data, MessageParcel& reply);
    int32_t GetAtr(MessageParcel& data, MessageParcel& reply);
    int32_t OpenChannel(MessageParcel& data, MessageParcel& reply);
    int32_t TransmitApdu(MessageParcel& data, MessageParcel& reply);
    int32_t CloseChannel(MessageParcel& data, MessageParcel& reply);
    int32_t IsChannelClosed(MessageParcel& data, MessageParcel& reply);
    int32_t GetSelectResponse(MessageParcel& data, MessageParcel& reply);
    int32_t CloseSeSessions(MessageParcel& data, MessageParcel& reply);
    int32_t CloseSessionChannels(MessageParcel& data, MessageParcel& reply);
private:
    bool CheckSeServiceOn();
    static SecureElement g_secureElement;
    sptr<IRemoteObject> g_remoteNxpService;
    sptr<IRemoteObject> g_remoteNfcService;
    sptr<IRemoteObject> g_remoteSeService;
};
} // namespace Oma
} // namespace Nfc
} // namespace OHOS
#endif // OHOS_SECURE_ELEMENT_H
