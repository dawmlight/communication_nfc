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
#include "secure_element_service_proxy.h"

#include <vector>

#include "loghelper.h"
#include "message_option.h"
#include "message_parcel.h"
#include "secure_element_reader_proxy.h"

namespace OHOS {
namespace se {
namespace omapi {
std::vector<std::string> SecureElementServiceProxy::GetReaders()
{
    MessageParcel data, reply;
    MessageOption option;

    int res = Remote()->SendRequest(COMMAND_SERVICE_GET_READERS, data, reply, option);
    if (res != ERR_NONE) {
        InfoLog("==[SecureElemntSessionProxy::GetReaders] send request(%d) with Res(%d) fail.",
                COMMAND_SERVICE_GET_READERS,
                res);
        return std::vector<std::string>();
    }
    std::vector<std::string> result{};
    if (!reply.ReadStringVector(&result)) {
        InfoLog("==Fail to ReadStringVector to result");
        return std::vector<std::string>();
    }
    InfoLog("[SecureElemntSessionProxy::GetReaders] send request(%d) with Res(%d) success.",
            COMMAND_SERVICE_GET_READERS,
            res);
    return result;
}

sptr<ISecureElementReader> SecureElementServiceProxy::GetReader(const std::string& readerName,
                                                                const sptr<IRemoteObject>& listener)
{
    MessageParcel data, reply;
    MessageOption option;
    data.WriteString(readerName);
    data.WriteRemoteObject(listener);
    int res = Remote()->SendRequest(COMMAND_SERVICE_GET_READER, data, reply, option);
    if (res != ERR_NONE) {
        InfoLog("[SecureElementServiceProxy::GetReader] send request(%d) with Res(%d) fail.",
                COMMAND_SERVICE_GET_READER,
                res);
        return nullptr;
    }

    sptr<OHOS::IRemoteObject> object = reply.ReadRemoteObject();
    if (object == nullptr) {
        InfoLog("[SecureElementServiceProxy::GetReader] send request(%d) with State(%d) success.",
                COMMAND_SERVICE_GET_READER,
                reply.ReadInt32());
        return nullptr;
    }
    return new SecureElemntReaderProxy(object);
}

std::vector<bool> SecureElementServiceProxy::IsNfcEventAllowed(const std::string& readerName,
                                                               const std::string& aid,
                                                               std::vector<std::string> packageNames)
{
    MessageParcel data, reply;
    MessageOption option;
    data.WriteString(readerName);
    data.WriteString(aid);
    data.WriteStringVector(packageNames);
    int res = Remote()->SendRequest(COMMAND_SERVICE_IS_NFCEVENT_ALLOWED, data, reply, option);
    if (res != ERR_NONE) {
        InfoLog("[SecureElementServiceProxy::IsNfcEventAllowed] send request(%d) with Res(%d) fail.",
                COMMAND_SERVICE_IS_NFCEVENT_ALLOWED,
                res);
        return std::vector<bool>();
    }

    std::vector<bool> eventList;
    if (!reply.ReadBoolVector(&eventList)) {
        InfoLog("==Fail to IsNfcEventAllowed to result");
        return std::vector<bool>();
    }
    InfoLog("[SecureElementServiceProxy::IsNfcEventAllowed] send request(%d) with Res(%d) success.",
            COMMAND_SERVICE_IS_NFCEVENT_ALLOWED,
            res);
    return eventList;
}
}  // namespace omapi
}  // namespace se
}  // namespace OHOS