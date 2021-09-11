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
#ifndef SECURE_ELEMENT_SERVICE_STUB_H
#define SECURE_ELEMENT_SERVICE_STUB_H

#include "iremote_stub.h"
#include "isecure_element_service.h"

namespace OHOS::se::omapi {
class SecureElementServiceStub : public OHOS::IRemoteStub<ISecureElementService> {
public:
    int OnRemoteRequest(uint32_t code,                         /* [in] */
                        OHOS::MessageParcel& data,             /* [in] */
                        OHOS::MessageParcel& reply,            /* [out] */
                        OHOS::MessageOption& option) override; /* [in] */

    virtual ~SecureElementServiceStub() {}

private:
    int HandleGetReaders(OHOS::MessageParcel& data, OHOS::MessageParcel& reply);
    int HandleGetReader(OHOS::MessageParcel& data, OHOS::MessageParcel& reply);
    int HandleIsNfcEventAllowed(OHOS::MessageParcel& data, OHOS::MessageParcel& reply);

private:
    static constexpr int COMMAND_SERVICE_START_ID = 100;
    static constexpr int COMMAND_SERVICE_GET_READERS = COMMAND_SERVICE_START_ID + 0;
    static constexpr int COMMAND_SERVICE_GET_READER = COMMAND_SERVICE_START_ID + 1;
    static constexpr int COMMAND_SERVICE_IS_NFCEVENT_ALLOWED = COMMAND_SERVICE_START_ID + 2;
};
}  // namespace OHOS::se::omapi
#endif  // !SECURE_ELEMENT_SERVICE_STUB_H