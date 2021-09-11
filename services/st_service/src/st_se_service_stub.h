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
#ifndef ST_SE_SERVICE_STUB_H
#define ST_SE_SERVICE_STUB_H

#include "iremote_stub.h"
#include "isecure_element.h"
#include "message_parcel.h"

namespace OHOS {
namespace hardware {
namespace se {
namespace v1_0 {
class StSeServiceStub : public OHOS::IRemoteStub<ISecureElement> {
public:
    int OnRemoteRequest(uint32_t code,                /* [in] */
                        OHOS::MessageParcel& data,    /* [in] */
                        OHOS::MessageParcel& reply,   /* [out] */
                        OHOS::MessageOption& option); /* [in] */

    virtual ~StSeServiceStub() {}

private:
    int HandleInitialize(OHOS::MessageParcel& data, OHOS::MessageParcel& reply);
    int HandleGetAtr(OHOS::MessageParcel& data, OHOS::MessageParcel& reply);
    int HandleIsCardPresent(OHOS::MessageParcel& data, OHOS::MessageParcel& reply);
    int HandleTransmit(OHOS::MessageParcel& data, OHOS::MessageParcel& reply);
    int HandleOpenBasicChannel(OHOS::MessageParcel& data, OHOS::MessageParcel& reply);
    int HandleOpenLogicalChannel(OHOS::MessageParcel& data, OHOS::MessageParcel& reply);
    int HandleCloseChannel(OHOS::MessageParcel& data, OHOS::MessageParcel& reply);
    int HandleReset(OHOS::MessageParcel& data, OHOS::MessageParcel& reply);

private:
    static constexpr int COMMAND_START_ID = 100;
    static constexpr int COMMAND_INITIALIZE = COMMAND_START_ID + 0;
    static constexpr int COMMAND_GET_ATR = COMMAND_START_ID + 1;
    static constexpr int COMMAND_IS_CARD_PRESENT = COMMAND_START_ID + 2;
    static constexpr int COMMAND_TRANSMIT = COMMAND_START_ID + 3;
    static constexpr int COMMAND_OPEN_BASIC_CHANNEL = COMMAND_START_ID + 4;
    static constexpr int COMMAND_OPEN_LOGICAL_CHANNEL = COMMAND_START_ID + 5;
    static constexpr int COMMAND_CLOSE_CHANNEL = COMMAND_START_ID + 6;
    static constexpr int COMMAND_RESET = COMMAND_START_ID + 7;
};
}  // namespace v1_0
}  // namespace se
}  // namespace hardware
}  // namespace OHOS
#endif  // !ST_SE_SERVICE_STUB_H