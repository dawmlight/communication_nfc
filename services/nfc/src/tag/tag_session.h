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

#ifndef OHOS_TAG_SESSION_H
#define OHOS_TAG_SESSION_H

#include "iremote_object.h"
#include "nfc_adapter_utils.h"
#include "nfc_tag_adapter_utils.h"

namespace OHOS {
namespace Nfc {
namespace Tag {
class TagSession {
public:
    static TagSession& GetInstance();
    ~TagSession();
    sptr<IRemoteObject> GetNfcRemoteTagService();

    // tag read and write handle
    int32_t ConnectTag(MessageParcel& data, MessageParcel& reply);
    int32_t IsTagConnected(MessageParcel& data, MessageParcel& reply);
    int32_t SetSendDataTimeout(MessageParcel& data, MessageParcel& reply);
    int32_t GetSendDataTimeout(MessageParcel& data, MessageParcel& reply);
    int32_t GetMaxSendLength(MessageParcel& data, MessageParcel& reply);
    int32_t SendData(MessageParcel& data, MessageParcel& reply);
    int32_t ReconnectTag(MessageParcel& data, MessageParcel& reply);
    int32_t ResetTimeouts(MessageParcel& data, MessageParcel& reply);

    int32_t IsNdefTag(MessageParcel& data, MessageParcel& reply);
    int32_t NdefRead(MessageParcel& data, MessageParcel& reply);
    int32_t NdefWrite(MessageParcel& data, MessageParcel& reply);
    int32_t NdefSetReadOnly(MessageParcel& data, MessageParcel& reply);
    int32_t CanSetReadOnly(MessageParcel& data, MessageParcel& reply);

private:
    static TagSession g_tagSession;
    sptr<IRemoteObject> g_remoteTagService;
};
} // namespace Tag
} // namespace Nfc
} // namespace OHOS
#endif // OHOS_TAG_SESSION_H
