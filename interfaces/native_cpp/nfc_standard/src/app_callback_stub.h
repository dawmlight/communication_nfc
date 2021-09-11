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
#ifndef APP_CALLBACK_STUB_H
#define APP_CALLBACK_STUB_H

#include "iapp_callback.h"
#include "iremote_stub.h"

namespace OHOS {
namespace nfc {
namespace sdk {
class AppCallbackStub : public OHOS::IRemoteStub<IAppCallback> {
public:
    int OnRemoteRequest(uint32_t code,                         /* [in] */
                        OHOS::MessageParcel& data,             /* [in] */
                        OHOS::MessageParcel& reply,            /* [out] */
                        OHOS::MessageOption& option) override; /* [in] */

    virtual ~AppCallbackStub() {}

private:
    int HandleTagDiscovered(OHOS::MessageParcel& data, OHOS::MessageParcel& reply);

private:
    static constexpr int APP_CALLBACK_START_ID = 300;
    static constexpr int COMMAND_TAG_DISCOVERED = APP_CALLBACK_START_ID + 0;
};

class TagRemovedCallbackStub : public OHOS::IRemoteStub<ITagRemovedCallback> {
public:
    int OnRemoteRequest(uint32_t code,                         /* [in] */
                        OHOS::MessageParcel& data,             /* [in] */
                        OHOS::MessageParcel& reply,            /* [out] */
                        OHOS::MessageOption& option) override; /* [in] */

    virtual ~TagRemovedCallbackStub() {}

private:
    int HandleTagRemoved(OHOS::MessageParcel& data, OHOS::MessageParcel& reply);

private:
    static constexpr int TAG_REMOVED_CALLBACK_START_ID = 400;
    static constexpr int COMMAND_TAG_REMOVED = TAG_REMOVED_CALLBACK_START_ID + 0;
};
}  // namespace sdk
}  // namespace nfc
}  // namespace OHOS
#endif  // !APP_CALLBACK_STUB_H