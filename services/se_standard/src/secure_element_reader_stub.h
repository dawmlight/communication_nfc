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
#ifndef SECURE_ELEMENT_READER_STUB_H
#define SECURE_ELEMENT_READER_STUB_H

#include "iremote_stub.h"
#include "isecure_element_reader.h"

namespace OHOS::se::omapi {
class SecureElemntReaderStub : public OHOS::IRemoteStub<ISecureElementReader> {
public:
    int OnRemoteRequest(uint32_t code,                         /* [in] */
                        OHOS::MessageParcel& data,             /* [in] */
                        OHOS::MessageParcel& reply,            /* [out] */
                        OHOS::MessageOption& option) override; /* [in] */

    virtual ~SecureElemntReaderStub() {}

private:
    int HandleOpenSession(OHOS::MessageParcel& data, OHOS::MessageParcel& reply);
    int HandleCloseSessions(OHOS::MessageParcel& data, OHOS::MessageParcel& reply);
    int HandleReset(OHOS::MessageParcel& data, OHOS::MessageParcel& reply);
    int HandleIsSecureElementPresent(OHOS::MessageParcel& data, OHOS::MessageParcel& reply);

private:
    static constexpr int COMMAND_READER_START_ID = 200;
    static constexpr int COMMAND_READER_OPEN_SESSION = COMMAND_READER_START_ID + 0;
    static constexpr int COMMAND_READER_CLOSE_SESSIONS = COMMAND_READER_START_ID + 1;
    static constexpr int COMMAND_READER_RESET = COMMAND_READER_START_ID + 2;
    static constexpr int COMMAND_READER_IS_SE_PRESENT = COMMAND_READER_START_ID + 3;
};
}  // namespace OHOS::se::omapi
#endif  // !SECURE_ELEMENT_READER_STUB_H