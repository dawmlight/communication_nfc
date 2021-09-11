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
#ifndef SECURE_ELEMENT_CHANNEL_STUB_H
#define SECURE_ELEMENT_CHANNEL_STUB_H

#include "iremote_stub.h"
#include "isecure_element_channel.h"

namespace OHOS::se::omapi {
class SecureElemntChannelStub : public OHOS::IRemoteStub<ISecureElementChannel> {
public:
    int OnRemoteRequest(uint32_t code,                         /* [in] */
                        OHOS::MessageParcel& data,             /* [in] */
                        OHOS::MessageParcel& reply,            /* [out] */
                        OHOS::MessageOption& option) override; /* [in] */

    virtual ~SecureElemntChannelStub() {}

private:
    int HandleClose(OHOS::MessageParcel& data, OHOS::MessageParcel& reply);
    int HandleIsClosed(OHOS::MessageParcel& data, OHOS::MessageParcel& reply);
    int HandleIsBasicChannel(OHOS::MessageParcel& data, OHOS::MessageParcel& reply);
    int HandleGetSelectResponse(OHOS::MessageParcel& data, OHOS::MessageParcel& reply);
    int HandleTransmit(OHOS::MessageParcel& data, OHOS::MessageParcel& reply);
    int HandleSelectNext(OHOS::MessageParcel& data, OHOS::MessageParcel& reply);

private:
    static constexpr int COMMAND_CHANNEL_START_ID = 400;
    static constexpr int COMMAND_CHANNEL_CLOSE = COMMAND_CHANNEL_START_ID + 0;
    static constexpr int COMMAND_CHANNEL_ISCLOSED = COMMAND_CHANNEL_START_ID + 1;
    static constexpr int COMMAND_CHANNEL_ISBASICCHANNEL = COMMAND_CHANNEL_START_ID + 2;
    static constexpr int COMMAND_CHANNEL_GET_SELECT_RESPONSE = COMMAND_CHANNEL_START_ID + 3;
    static constexpr int COMMAND_CHANNEL_TRANSMIT = COMMAND_CHANNEL_START_ID + 4;
    static constexpr int COMMAND_CHANNEL_SELECT_NEXT = COMMAND_CHANNEL_START_ID + 5;
};
}  // namespace OHOS::se::omapi
#endif  // !SECURE_ELEMENT_CHANNEL_STUB_H