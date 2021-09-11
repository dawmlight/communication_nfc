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
#include "st_se_service_stub.h"

#include "loghelper.h"

namespace OHOS {
namespace hardware {
namespace se {
namespace v1_0 {
int StSeServiceStub::OnRemoteRequest(uint32_t code,         /* [in] */
                                     MessageParcel& data,   /* [in] */
                                     MessageParcel& reply,  /* [out] */
                                     MessageOption& option) /* [in] */
{
    DebugLog("OnRemoteRequest occur, code is %d", code);
    switch (code) {
        case COMMAND_INITIALIZE:
            return HandleInitialize(data, reply);
        case COMMAND_GET_ATR:
            return HandleGetAtr(data, reply);
        case COMMAND_IS_CARD_PRESENT:
            return HandleIsCardPresent(data, reply);
        case COMMAND_TRANSMIT:
            return HandleTransmit(data, reply);
        case COMMAND_OPEN_BASIC_CHANNEL:
            return HandleOpenBasicChannel(data, reply);
        case COMMAND_OPEN_LOGICAL_CHANNEL:
            return HandleOpenLogicalChannel(data, reply);
        case COMMAND_CLOSE_CHANNEL:
            return HandleCloseChannel(data, reply);
        case COMMAND_RESET:
            return HandleReset(data, reply);
        default:
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
}

int StSeServiceStub::HandleInitialize(OHOS::MessageParcel& data, OHOS::MessageParcel& reply)
{
    sptr<IRemoteObject> obj = data.ReadRemoteObject();
    if (obj == nullptr) {
        InfoLog("ISecureElementCallback can't be empty.");
        return ERR_INVALID_DATA;
    }
    DebugLog("ISecureElementCallback IRemoteObject is ProxyObject (%d)", obj->IsProxyObject());
    Initialize(obj);
    return ERR_NONE;
}

int StSeServiceStub::HandleGetAtr(OHOS::MessageParcel& data, OHOS::MessageParcel& reply)
{
    std::string atr = GetAtr();
    reply.WriteString(atr);
    return ERR_NONE;
}
int StSeServiceStub::HandleIsCardPresent(OHOS::MessageParcel& data, OHOS::MessageParcel& reply)
{
    bool result = IsCardPresent();
    reply.WriteBool(result);
    return ERR_NONE;
}
int StSeServiceStub::HandleTransmit(OHOS::MessageParcel& data, OHOS::MessageParcel& reply)
{
    std::string cmd = data.ReadString();
    std::string rsp = Transmit(cmd);
    reply.WriteString(rsp);
    return ERR_NONE;
}
int StSeServiceStub::HandleOpenBasicChannel(OHOS::MessageParcel& data, OHOS::MessageParcel& reply)
{
    std::string aid = data.ReadString();
    char p2 = data.ReadInt8();
    std::unique_ptr<SEBasicRespData> respData = OpenBasicChannel(aid, p2);
    reply.WriteParcelable(respData.get());
    reply.WriteInt32(ERR_NONE);
    return ERR_NONE;
}
int StSeServiceStub::HandleOpenLogicalChannel(OHOS::MessageParcel& data, OHOS::MessageParcel& reply)
{
    std::string aid = data.ReadString();
    char p2 = data.ReadInt8();
    std::unique_ptr<SELogicalRespData> respData = OpenLogicalChannel(aid, p2);
    reply.WriteParcelable(respData.get());
    reply.WriteInt32(ERR_NONE);
    return ERR_NONE;
}
int StSeServiceStub::HandleCloseChannel(OHOS::MessageParcel& data, OHOS::MessageParcel& reply)
{
    uint8_t channelNumber = data.ReadUint8();
    int32_t status = static_cast<int32_t>(CloseChannel(channelNumber));
    reply.WriteInt32(status);
    return ERR_NONE;
}
int StSeServiceStub::HandleReset(OHOS::MessageParcel& data, OHOS::MessageParcel& reply)
{
    int32_t result = static_cast<int32_t>(Reset());
    reply.WriteInt32(result);
    return ERR_NONE;
}
}  // namespace v1_0
}  // namespace se
}  // namespace hardware
}  // namespace OHOS