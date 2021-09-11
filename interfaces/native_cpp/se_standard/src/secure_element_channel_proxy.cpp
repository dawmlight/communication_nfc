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
#include "secure_element_channel_proxy.h"

#include "loghelper.h"
#include "message_option.h"
#include "message_parcel.h"

namespace OHOS {
namespace se {
namespace omapi {
void SecureElemntChannelProxy::Close()
{
    MessageParcel data, reply;
    MessageOption option;
    int res = Remote()->SendRequest(COMMAND_CHANNEL_CLOSE, data, reply, option);
    if (res != ERR_NONE) {
        InfoLog("[SecureElemntChannelProxy::Close] send request(%d) with Res(%d) fail.", COMMAND_CHANNEL_CLOSE, res);
    }
}

bool SecureElemntChannelProxy::IsClosed()
{
    MessageParcel data, reply;
    MessageOption option;

    int res = Remote()->SendRequest(COMMAND_CHANNEL_ISCLOSED, data, reply, option);
    if (res != ERR_NONE) {
        InfoLog(
            "[SecureElemntChannelProxy::IsClosed] send request(%d) with Res(%d) fail.", COMMAND_CHANNEL_ISCLOSED, res);
        return true;
    }
    InfoLog(
        "[SecureElemntChannelProxy::IsClosed] send request(%d) with Res(%d) success.", COMMAND_CHANNEL_ISCLOSED, res);
    return reply.ReadBool();
}

bool SecureElemntChannelProxy::IsBasicChannel()
{
    MessageParcel data, reply;
    MessageOption option;

    int res = Remote()->SendRequest(COMMAND_CHANNEL_ISBASICCHANNEL, data, reply, option);
    if (res == ERR_NONE) {
        InfoLog("[SecureElemntChannelProxy::IsBasicChannel] send request(%d) with Res(%d) fail.",
                COMMAND_CHANNEL_ISBASICCHANNEL,
                res);
        return false;
    }
    InfoLog("[SecureElemntChannelProxy::IsBasicChannel] send request(%d) with Res(%d) success.",
            COMMAND_CHANNEL_ISBASICCHANNEL,
            res);
    return reply.ReadBool();
}
std::string SecureElemntChannelProxy::GetSelectResponse()
{
    MessageParcel data, reply;
    MessageOption option;

    int res = Remote()->SendRequest(COMMAND_CHANNEL_GET_SELECT_RESPONSE, data, reply, option);
    if (res != ERR_NONE) {
        InfoLog("[SecureElemntChannelProxy::GetSelectResponse] send request(%d) with Res(%d) fail.",
                COMMAND_CHANNEL_GET_SELECT_RESPONSE,
                res);
        return std::string();
    }
    InfoLog("[SecureElemntChannelProxy::GetSelectResponse] send request(%d) with Res(%d) success.",
            COMMAND_CHANNEL_GET_SELECT_RESPONSE,
            res);
    return reply.ReadString();
}
std::string SecureElemntChannelProxy::Transmit(const std::string& command)
{
    MessageParcel data, reply;
    MessageOption option;
    data.WriteString(command);

    int res = Remote()->SendRequest(COMMAND_CHANNEL_TRANSMIT, data, reply, option);
    if (res != ERR_NONE) {
        InfoLog(
            "[SecureElemntChannelProxy::Transmit] send request(%d) with Res(%d) fail.", COMMAND_CHANNEL_TRANSMIT, res);
        return std::string();
    }
    InfoLog(
        "[SecureElemntChannelProxy::Transmit] send request(%d) with Res(%d) success.", COMMAND_CHANNEL_TRANSMIT, res);
    return reply.ReadString();
}
bool SecureElemntChannelProxy::SelectNext()
{
    MessageParcel data, reply;
    MessageOption option;

    int res = Remote()->SendRequest(COMMAND_CHANNEL_SELECT_NEXT, data, reply, option);
    if (res != ERR_NONE) {
        InfoLog("[SecureElemntChannelProxy::SelectNext] send request(%d) with Res(%d) fail.",
                COMMAND_CHANNEL_SELECT_NEXT,
                res);
        return false;
    }
    InfoLog("[SecureElemntChannelProxy::SelectNext] send request(%d) with Res(%d) success.",
            COMMAND_CHANNEL_SELECT_NEXT,
            res);
    return reply.ReadBool();
}
}  // namespace omapi
}  // namespace se
}  // namespace OHOS