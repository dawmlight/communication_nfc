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
#include "secure_element_channel_stub.h"

#include "loghelper.h"
#include "message_parcel.h"
#include "se_common_exception.h"

using namespace std;

namespace OHOS::se::omapi {
int SecureElemntChannelStub::OnRemoteRequest(uint32_t code,         /* [in] */
                                             MessageParcel& data,   /* [in] */
                                             MessageParcel& reply,  /* [out] */
                                             MessageOption& option) /* [in] */
{
    DebugLog("SecureElemntChannelStub::OnRemoteRequest occur, code is %d", code);
    switch (code) {
        case COMMAND_CHANNEL_CLOSE:
            return HandleClose(data, reply);
        case COMMAND_CHANNEL_ISCLOSED:
            return HandleIsClosed(data, reply);
        case COMMAND_CHANNEL_ISBASICCHANNEL:
            return HandleIsBasicChannel(data, reply);
        case COMMAND_CHANNEL_GET_SELECT_RESPONSE:
            return HandleGetSelectResponse(data, reply);
        case COMMAND_CHANNEL_TRANSMIT:
            return HandleTransmit(data, reply);
        case COMMAND_CHANNEL_SELECT_NEXT:
            return HandleSelectNext(data, reply);
        default:
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
}
int SecureElemntChannelStub::HandleClose(OHOS::MessageParcel& data, OHOS::MessageParcel& reply)
{
    Close();
    return ERR_NONE;
}

int SecureElemntChannelStub::HandleIsClosed(OHOS::MessageParcel& data, OHOS::MessageParcel& reply)
{
    bool res = IsClosed();
    reply.WriteBool(res);
    return ERR_NONE;
}

int SecureElemntChannelStub::HandleIsBasicChannel(OHOS::MessageParcel& data, OHOS::MessageParcel& reply)
{
    bool res = IsBasicChannel();
    reply.WriteBool(res);
    return ERR_NONE;
}

int SecureElemntChannelStub::HandleGetSelectResponse(OHOS::MessageParcel& data, OHOS::MessageParcel& reply)
{
    std::string response = GetSelectResponse();
    reply.WriteString(response);
    return ERR_NONE;
}

int SecureElemntChannelStub::HandleTransmit(OHOS::MessageParcel& data, OHOS::MessageParcel& reply)
{
    std::string command = data.ReadString();
    std::string response;
    int32_t state = ERR_NONE;
    try {
        response = Transmit(command);
    } catch (const SecurityError& e) {
        InfoLog("Transmit is failed: %s", e.what());
        state = SE_SER_ERR_SECURITY;
    } catch (const IllegalStateError& e) {
        InfoLog("Transmit is failed: %s", e.what());
        state = SE_SER_ERR_ILLEGAL_STATE;
    } catch (const IllegalArgumentError& e) {
        InfoLog("Transmit is failed: %s", e.what());
        state = SE_SER_ERR_ILLEGAL_PARAM;
    } catch (const AccessControlError& e) {
        InfoLog("Transmit is failed: %s", e.what());
        state = SE_SER_ERR_ACCESS_CONTROL;
    }
    reply.WriteString(response);
    return ERR_NONE;
}

int SecureElemntChannelStub::HandleSelectNext(OHOS::MessageParcel& data, OHOS::MessageParcel& reply)
{
    bool res = SelectNext();
    reply.WriteBool(res);
    return ERR_NONE;
}
}  // namespace OHOS::se::omapi