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

#include "secure_element_session_proxy.h"

#include "loghelper.h"
#include "message_option.h"
#include "message_parcel.h"
#include "secure_element_channel_proxy.h"

using namespace std;

namespace OHOS {
namespace se {
namespace omapi {
std::string SecureElemntSessionProxy::GetATR()
{
    MessageParcel data, reply;
    MessageOption option;

    int res = Remote()->SendRequest(COMMAND_SESSION_GETATR, data, reply, option);
    if (res != ERR_NONE) {
        InfoLog("[SecureElemntSessionProxy::GetATR] send request(%d) with Res(%d) fail.", COMMAND_SESSION_GETATR, res);
        return std::string();
    }
    InfoLog("[SecureElemntSessionProxy::GetATR] send request(%d) with Res(%d) success.", COMMAND_SESSION_GETATR, res);
    return reply.ReadString();
}

void SecureElemntSessionProxy::Close()
{
    MessageParcel data, reply;
    MessageOption option;
    int res = Remote()->SendRequest(COMMAND_SESSION_CLOSE, data, reply, option);
    if (res != ERR_NONE) {
        InfoLog("[SecureElemntSessionProxy::Close] send request(%d) with Res(%d) fail.", COMMAND_SESSION_CLOSE, res);
    }
}

bool SecureElemntSessionProxy::IsClosed()
{
    MessageParcel data, reply;
    MessageOption option;

    int res = Remote()->SendRequest(COMMAND_SESSION_ISCLOSED, data, reply, option);
    if (res != ERR_NONE) {
        InfoLog(
            "[SecureElemntSessionProxy::IsClosed] send request(%d) with Res(%d) fail.", COMMAND_SESSION_ISCLOSED, res);
        return true;
    }
    InfoLog(
        "[SecureElemntSessionProxy::IsClosed] send request(%d) with Res(%d) success.", COMMAND_SESSION_ISCLOSED, res);
    return reply.ReadBool();
}

void SecureElemntSessionProxy::CloseChannels()
{
    MessageParcel data, reply;
    MessageOption option;
    int res = Remote()->SendRequest(COMMAND_SESSION_CLOSE_CHANNELS, data, reply, option);
    if (res != ERR_NONE) {
        InfoLog("[SecureElemntSessionProxy::CloseChannels] send request(%d) with Res(%d) fail.",
                COMMAND_SESSION_CLOSE_CHANNELS,
                res);
    }
}

sptr<ISecureElementChannel> SecureElemntSessionProxy::OpenBasicChannel(const std::string& aid, int p2)
{
    MessageParcel data, reply;
    MessageOption option;

    data.WriteString(aid);
    data.WriteInt32(p2);
    int res = Remote()->SendRequest(COMMAND_SESSION_OPEN_BASIC_CHANNEL, data, reply, option);
    if (res != ERR_NONE) {
        InfoLog("[SecureElemntSessionProxy::OpenBasicChannel] send request(%d) with Res(%d) fail.",
                COMMAND_SESSION_OPEN_BASIC_CHANNEL,
                res);
        return nullptr;
    }
    sptr<OHOS::IRemoteObject> object = reply.ReadRemoteObject();
    if (object == nullptr) {
        InfoLog("[SecureElemntSessionProxy::OpenBasicChannel] send request(%d) with State(%d) Empty.",
                COMMAND_SESSION_OPEN_BASIC_CHANNEL,
                reply.ReadInt32());
        return nullptr;
    }
    return new SecureElemntChannelProxy(object);
}

sptr<ISecureElementChannel> SecureElemntSessionProxy::OpenLogicalChannel(const std::string& aid, int p2)
{
    MessageParcel data, reply;
    MessageOption option;

    data.WriteString(aid);
    data.WriteInt32(p2);
    int res = Remote()->SendRequest(COMMAND_SESSION_OPEN_LOGIC_CHANNEL, data, reply, option);
    if (res != ERR_NONE) {
        InfoLog("[SecureElemntSessionProxy::OpenLogicalChannel] send request(%d) with Res(%d) fail.",
                COMMAND_SESSION_OPEN_LOGIC_CHANNEL,
                res);
        return nullptr;
    }
    sptr<OHOS::IRemoteObject> object = reply.ReadRemoteObject();
    if (object == nullptr) {
        InfoLog("[SecureElemntSessionProxy::OpenLogicalChannel] send request(%d) with state(%d) Empty.",
                COMMAND_SESSION_OPEN_LOGIC_CHANNEL,
                reply.ReadInt32());
        return nullptr;
    }
    return new SecureElemntChannelProxy(object);
}
}  // namespace omapi
}  // namespace se
}  // namespace OHOS