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
#include "st_se_service_proxy.h"

#include "hilog/log.h"
#include "message_option.h"
#include "message_parcel.h"

namespace OHOS {
namespace hardware {
namespace se {
namespace v1_0 {
void StSeServiceProxy::Initialize(const sptr<IRemoteObject>& callback)
{
    MessageParcel data, reply;
    MessageOption option(MessageOption::TF_ASYNC);
    data.WriteRemoteObject(callback);
    int res = Remote()->SendRequest(COMMAND_INITIALIZE, data, reply, option);
    if (res != ERR_NONE) {
        HILOG_INFO(
            LOG_CORE, "[StSeServiceProxy::Initialize] send request(%d) with Res(%d) fail.", COMMAND_INITIALIZE, res);
    }
}

std::string StSeServiceProxy::GetAtr()
{
    MessageParcel data, reply;
    MessageOption option(MessageOption::TF_SYNC);
    int res = Remote()->SendRequest(COMMAND_GET_ATR, data, reply, option);
    if (res != ERR_NONE) {
        HILOG_INFO(LOG_CORE, "[StSeServiceProxy::GetAtr] send request(%d) with Res(%d) fail.", COMMAND_GET_ATR, res);
        return std::string();
    }
    return reply.ReadString();
}

bool StSeServiceProxy::IsCardPresent()
{
    MessageParcel data, reply;
    MessageOption option(MessageOption::TF_SYNC);
    int res = Remote()->SendRequest(COMMAND_IS_CARD_PRESENT, data, reply, option);
    if (res != ERR_NONE) {
        HILOG_INFO(LOG_CORE, "[StSeServiceProxy::GetAtr] send request(%d) with Res(%d) fail.", COMMAND_GET_ATR, res);
        return false;
    }
    return reply.ReadBool();
}

std::string StSeServiceProxy::Transmit(const std::string& cmd)
{
    MessageParcel data, reply;
    MessageOption option(MessageOption::TF_SYNC);
    data.WriteString(cmd);
    int res = Remote()->SendRequest(COMMAND_TRANSMIT, data, reply, option);
    if (res != ERR_NONE) {
        HILOG_INFO(LOG_CORE, "[StSeServiceProxy::GetAtr] send request(%d) with Res(%d) fail.", COMMAND_GET_ATR, res);
        return std::string();
    }
    return reply.ReadString();
}

std::unique_ptr<SEBasicRespData> StSeServiceProxy::OpenBasicChannel(const std::string& aid, char p2)
{
    MessageParcel data, reply;
    MessageOption option(MessageOption::TF_SYNC);
    data.WriteString(aid);
    data.WriteInt8(p2);
    int res = Remote()->SendRequest(COMMAND_OPEN_BASIC_CHANNEL, data, reply, option);
    if (res != ERR_NONE) {
        HILOG_INFO(LOG_CORE,
                   "[StSeServiceProxy::OpenBasicChannel] send request(%d) with Res(%d) fail.",
                   COMMAND_OPEN_BASIC_CHANNEL,
                   res);
        return std::unique_ptr<SEBasicRespData>();
    }
    SEBasicRespData* respData = reply.ReadParcelable<SEBasicRespData>();
    return std::unique_ptr<SEBasicRespData>(respData);
}
std::unique_ptr<SELogicalRespData> StSeServiceProxy::OpenLogicalChannel(const std::string& aid, char p2)
{
    MessageParcel data, reply;
    MessageOption option(MessageOption::TF_SYNC);
    data.WriteString(aid);
    data.WriteInt8(p2);
    int res = Remote()->SendRequest(COMMAND_OPEN_LOGICAL_CHANNEL, data, reply, option);
    if (res != ERR_NONE) {
        HILOG_INFO(LOG_CORE,
                   "[StSeServiceProxy::OpenLogicalChannel] send request(%d) with Res(%d) fail.",
                   COMMAND_OPEN_LOGICAL_CHANNEL,
                   res);
        return std::unique_ptr<SELogicalRespData>();
    }
    SELogicalRespData* respData = reply.ReadParcelable<SELogicalRespData>();
    return std::unique_ptr<SELogicalRespData>(respData);
}
SecureElementStatus StSeServiceProxy::CloseChannel(uint8_t channelNumber)
{
    MessageParcel data, reply;
    MessageOption option(MessageOption::TF_SYNC);
    data.WriteUint8(channelNumber);
    int res = Remote()->SendRequest(COMMAND_CLOSE_CHANNEL, data, reply, option);
    if (res != ERR_NONE) {
        HILOG_INFO(LOG_CORE,
                   "[StSeServiceProxy::OpenLogicalChannel] send request(%d) with Res(%d) fail.",
                   COMMAND_CLOSE_CHANNEL,
                   res);
        return SecureElementStatus::FAILED;
    }
    return static_cast<SecureElementStatus>(reply.ReadInt32());
}
SecureElementStatus StSeServiceProxy::Reset()
{
    MessageParcel data, reply;
    MessageOption option(MessageOption::TF_SYNC);
    int res = Remote()->SendRequest(COMMAND_RESET, data, reply, option);
    if (res != ERR_NONE) {
        HILOG_INFO(LOG_CORE, "[StSeServiceProxy::Reset] send request(%d) with Res(%d) fail.", COMMAND_RESET, res);
        return SecureElementStatus::FAILED;
    }
    return static_cast<SecureElementStatus>(reply.ReadInt32());
}
}  // namespace v1_0
}  // namespace se
}  // namespace hardware
}  // namespace OHOS