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
#include "secure_element_reader_proxy.h"

#include "isecure_element_session.h"
#include "loghelper.h"
#include "message_option.h"
#include "message_parcel.h"
#include "secure_element_session_proxy.h"

namespace OHOS {
namespace se {
namespace omapi {
bool SecureElemntReaderProxy::IsSecureElementPresent()
{
    MessageParcel data, reply;
    MessageOption option;

    int res = Remote()->SendRequest(COMMAND_READER_IS_SE_PRESENT, data, reply, option);
    if (res != ERR_NONE) {
        InfoLog("[SecureElemntReaderProxy::IsSecureElementPresent] send request(%d) with Res(%d) fail.",
                COMMAND_READER_IS_SE_PRESENT,
                res);
        return false;
    }
    InfoLog("[SecureElemntReaderProxy::IsSecureElementPresent] send request(%d) with Res(%d) success.",
            COMMAND_READER_IS_SE_PRESENT,
            res);
    return reply.ReadBool();
}

sptr<ISecureElementSession> SecureElemntReaderProxy::OpenSession()
{
    MessageParcel data, reply;
    MessageOption option;

    std::string result;
    int res = Remote()->SendRequest(COMMAND_READER_OPEN_SESSION, data, reply, option);
    if (res != ERR_NONE) {
        InfoLog("[SecureElemntReaderProxy::OpenSession] send request(%d) with Res(%d) fail.",
                COMMAND_READER_OPEN_SESSION,
                res);
        return nullptr;
    }

    sptr<OHOS::IRemoteObject> object = reply.ReadRemoteObject();
    if (object == nullptr) {
        InfoLog("[SecureElemntReaderProxy::OpenSession] send request(%d) with State(%d) fail. IRemoteObject is empty.",
                COMMAND_READER_OPEN_SESSION,
                reply.ReadInt32());
        return nullptr;
    }
    InfoLog("[SecureElemntReaderProxy::OpenSession] send request(%d) with Res(%d) success.",
            COMMAND_READER_OPEN_SESSION,
            res);
    return new SecureElemntSessionProxy(object);
}

void SecureElemntReaderProxy::CloseSessions()
{
    MessageParcel data, reply;
    MessageOption option;
    int res = Remote()->SendRequest(COMMAND_READER_CLOSE_SESSIONS, data, reply, option);
    if (res != ERR_NONE) {
        InfoLog("[SecureElemntReaderProxy::CloseSessions] send request(%d) with Res(%d) fail.",
                COMMAND_READER_CLOSE_SESSIONS,
                res);
    }
}

bool SecureElemntReaderProxy::Reset()
{
    MessageParcel data, reply;
    MessageOption option;

    int res = Remote()->SendRequest(COMMAND_READER_RESET, data, reply, option);
    if (res != ERR_NONE) {
        InfoLog("[SecureElemntReaderProxy::Reset] send request(%d) with Res(%d) fail.", COMMAND_READER_RESET, res);
        return false;
    }
    InfoLog("[SecureElemntReaderProxy::Reset] send request(%d) with Res(%d) success.", COMMAND_READER_RESET, res);
    return reply.ReadBool();
}
}  // namespace omapi
}  // namespace se
}  // namespace OHOS