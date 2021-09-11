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
#ifndef SECURE_ELEMENT_CALLBACK_STUB_H
#define SECURE_ELEMENT_CALLBACK_STUB_H

#include "iremote_stub.h"
#include "isecure_element.h"
#include "message_parcel.h"

namespace OHOS {
namespace hardware {
namespace se {
namespace v1_0 {
class SeCallbackStub : public OHOS::IRemoteStub<ISecureElementCallback> {
public:
    int OnRemoteRequest(uint32_t code,                /* [in] */
                        OHOS::MessageParcel& data,    /* [in] */
                        OHOS::MessageParcel& reply,   /* [out] */
                        OHOS::MessageOption& option); /* [in] */

    virtual ~SeCallbackStub() {}

private:
    int HandleOnStateChange(OHOS::MessageParcel& data, OHOS::MessageParcel& reply);

private:
    static constexpr int COMMAND_START_ID = 50;
    static constexpr int COMMAND_STATE_CHANGE = COMMAND_START_ID + 0;
};
}  // namespace v1_0
}  // namespace se
}  // namespace hardware
}  // namespace OHOS
#endif  // !SECURE_ELEMENT_CALLBACK_STUB_H