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
#include "secure_element_service_stub.h"

#include "isecure_element_reader.h"
#include "loghelper.h"
#include "message_parcel.h"
#include "se_common_exception.h"

using namespace std;

namespace OHOS::se::omapi {
int SecureElementServiceStub::OnRemoteRequest(uint32_t code,         /* [in] */
                                              MessageParcel& data,   /* [in] */
                                              MessageParcel& reply,  /* [out] */
                                              MessageOption& option) /* [in] */
{
    DebugLog("SecureElementServiceStub::OnRemoteRequest occur, code is %d", code);
    switch (code) {
        case COMMAND_SERVICE_GET_READERS:
            return HandleGetReaders(data, reply);
        case COMMAND_SERVICE_GET_READER:
            return HandleGetReader(data, reply);
        case COMMAND_SERVICE_IS_NFCEVENT_ALLOWED:
            return HandleIsNfcEventAllowed(data, reply);
        default:
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
}
int SecureElementServiceStub::HandleGetReaders(OHOS::MessageParcel& data, OHOS::MessageParcel& reply)
{
    std::vector<std::string> readerList = GetReaders();
    InfoLog("=Stub Reader Name List Size: %d", readerList.size());
    reply.WriteStringVector(readerList);
    return ERR_NONE;
}

int SecureElementServiceStub::HandleGetReader(OHOS::MessageParcel& data, OHOS::MessageParcel& reply)
{
    std::string readerName = data.ReadString();
    sptr<OHOS::IRemoteObject> listener = data.ReadRemoteObject();
    DebugLog("SecureElementServiceStub::HandleGetReader: %p .", (void*)listener.GetRefPtr());

    sptr<IRemoteObject> iReader = nullptr;
    int32_t state = ERR_NONE;
    try {
        sptr<ISecureElementReader> reader = GetReader(readerName, listener);
        if (reader) {
            InfoLog("Front Asobject, ISEReader sptr count %d", reader->GetSptrRefCount());
            iReader = reader->AsObject();
            InfoLog("Behind Asobject, ISEReader sptr count %d", reader->GetSptrRefCount());
        }
    } catch (const IllegalArgumentError& e) {
        InfoLog("GetReader is failed: %s.", e.what());
        state = SE_SER_ERR_ILLEGAL_PARAM;
    }
    reply.WriteRemoteObject(iReader);
    reply.WriteInt32(state);
    return ERR_NONE;
}

int SecureElementServiceStub::HandleIsNfcEventAllowed(OHOS::MessageParcel& data, OHOS::MessageParcel& reply)
{
    std::string readerName = data.ReadString();
    std::string aid = data.ReadString();
    std::vector<std::string> packageNames{};
    data.ReadStringVector(&packageNames);
    std::vector<bool> res{};
    int32_t state = ERR_NONE;
    try {
        res = IsNfcEventAllowed(readerName, aid, packageNames);
    } catch (const IllegalArgumentError& e) {
        InfoLog("CheckNfcEventAllowed is failed: %s.", e.what());
        state = SE_SER_ERR_ILLEGAL_PARAM;
    }
    reply.WriteBoolVector(res);
    reply.WriteInt32(state);
    return ERR_NONE;
}
}  // namespace OHOS::se::omapi