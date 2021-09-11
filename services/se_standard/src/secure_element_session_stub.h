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
#ifndef SECURE_ELEMENT_SESSION_STUB_H
#define SECURE_ELEMENT_SESSION_STUB_H

#include "iremote_stub.h"
#include "isecure_element_session.h"

namespace OHOS::se::omapi {
class SecureElemntSessionStub : public OHOS::IRemoteStub<ISecureElementSession> {
public:
    int OnRemoteRequest(uint32_t code,                         /* [in] */
                        OHOS::MessageParcel& data,             /* [in] */
                        OHOS::MessageParcel& reply,            /* [out] */
                        OHOS::MessageOption& option) override; /* [in] */

    virtual ~SecureElemntSessionStub() {}

private:
    int HandleClose(OHOS::MessageParcel& data, OHOS::MessageParcel& reply);
    int HandleIsClosed(OHOS::MessageParcel& data, OHOS::MessageParcel& reply);
    int HandleCloseChannels(OHOS::MessageParcel& data, OHOS::MessageParcel& reply);
    int HandleGetATR(OHOS::MessageParcel& data, OHOS::MessageParcel& reply);
    int HandleOpenBasicChannel(OHOS::MessageParcel& data, OHOS::MessageParcel& reply);
    int HandleOpenLogicalChannel(OHOS::MessageParcel& data, OHOS::MessageParcel& reply);

private:
    static constexpr int COMMAND_SESSION_START_ID = 300;
    static constexpr int COMMAND_SESSION_CLOSE = COMMAND_SESSION_START_ID + 0;
    static constexpr int COMMAND_SESSION_ISCLOSED = COMMAND_SESSION_START_ID + 1;
    static constexpr int COMMAND_SESSION_GETATR = COMMAND_SESSION_START_ID + 2;
    static constexpr int COMMAND_SESSION_CLOSE_CHANNELS = COMMAND_SESSION_START_ID + 3;
    static constexpr int COMMAND_SESSION_OPEN_BASIC_CHANNEL = COMMAND_SESSION_START_ID + 4;
    static constexpr int COMMAND_SESSION_OPEN_LOGIC_CHANNEL = COMMAND_SESSION_START_ID + 5;
};
}  // namespace OHOS::se::omapi
#endif  // !SECURE_ELEMENT_SESSION_STUB_H