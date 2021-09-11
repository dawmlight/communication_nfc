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
#include "secure_element_reader_stub.h"

#include "isecure_element_session.h"
#include "loghelper.h"
#include "message_parcel.h"
#include "se_common_exception.h"

using namespace std;

namespace OHOS::se::omapi {
int SecureElemntReaderStub::OnRemoteRequest(uint32_t code,         /* [in] */
                                            MessageParcel& data,   /* [in] */
                                            MessageParcel& reply,  /* [out] */
                                            MessageOption& option) /* [in] */
{
    DebugLog("SecureElemntReaderStub::OnRemoteRequest occur, code is %d", code);
    switch (code) {
        case COMMAND_READER_OPEN_SESSION:
            return HandleOpenSession(data, reply);
        case COMMAND_READER_CLOSE_SESSIONS:
            return HandleCloseSessions(data, reply);
        case COMMAND_READER_RESET:
            return HandleReset(data, reply);
        case COMMAND_READER_IS_SE_PRESENT:
            return HandleIsSecureElementPresent(data, reply);
        default:
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
}
int SecureElemntReaderStub::HandleOpenSession(OHOS::MessageParcel& data, OHOS::MessageParcel& reply)
{
    int32_t state = ERR_NONE;
    sptr<IRemoteObject> iSession = nullptr;
    try {
        sptr<ISecureElementSession> response = OpenSession();
        if (response) {
            DebugLog("SecureElemntReaderStub::HandleOpenSession Result: Success.");
            iSession = response->AsObject();
        } else {
            DebugLog("SecureElemntReaderStub::HandleOpenSession Result: Empty.");
            state = SE_SER_ERR_NULL_POINTER;
        }
    } catch (const ServiceSpecificError& e) {
        InfoLog("OpenSession is failed: %s", e.what());
        state = SE_SER_ERR_SECURITY;
    }
    reply.WriteRemoteObject(iSession);
    reply.WriteInt32(state);
    return ERR_NONE;
}

int SecureElemntReaderStub::HandleCloseSessions(OHOS::MessageParcel& data, OHOS::MessageParcel& reply)
{
    DebugLog("SecureElemntReaderStub::HandleCloseSessions.");
    CloseSessions();
    return ERR_NONE;
}

int SecureElemntReaderStub::HandleReset(OHOS::MessageParcel& data, OHOS::MessageParcel& reply)
{
    bool res = Reset();
    DebugLog("SecureElemntReaderStub::HandleReset Result: %d.", res);
    reply.WriteBool(res);
    return ERR_NONE;
}
int SecureElemntReaderStub::HandleIsSecureElementPresent(OHOS::MessageParcel& data, OHOS::MessageParcel& reply)
{
    bool res = IsSecureElementPresent();
    DebugLog("SecureElemntReaderStub::HandleIsSecureElementPresent Result: %d.", res);
    reply.WriteBool(res);
    return ERR_NONE;
}
}  // namespace OHOS::se::omapi