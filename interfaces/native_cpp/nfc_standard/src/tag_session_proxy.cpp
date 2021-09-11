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

#include "tag_session_proxy.h"

#include "loghelper.h"
#include "message_option.h"
#include "message_parcel.h"

using namespace std;

namespace OHOS {
namespace nfc {
namespace reader {
int TagSessionProxy::Connect(int nativeHandle, int technology)
{
    int result = 0;
    MessageParcel data;
    data.WriteInt32(nativeHandle);
    data.WriteInt32(technology);
    MessageOption option(MessageOption::TF_SYNC);
    ProcessIntRes(COMMAND_CONNECT, data, option, result);
    return result;
}

int TagSessionProxy::Reconnect(int nativeHandle)
{
    int result = 0;
    MessageParcel data;
    data.WriteInt32(nativeHandle);
    MessageOption option(MessageOption::TF_SYNC);
    ProcessIntRes(COMMAND_RECONNECT, data, option, result);
    return result;
}

void TagSessionProxy::Disconnect(int nativeHandle)
{
    MessageParcel data;
    data.WriteInt32(nativeHandle);
    MessageOption option(MessageOption::TF_ASYNC);
    ProcessCommand(COMMAND_DISCONNECT, data, option);
    return;
}

std::vector<int> TagSessionProxy::GetTechList(int nativeHandle)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    data.WriteInt32(nativeHandle);
    int res = Remote()->SendRequest(COMMAND_GET_TECHLIST, data, reply, option);
    if (res != ERR_NONE) {
        InfoLog("It is failed To Get Tech List with Res(%d).", res);
        return std::vector<int>();
    }
    std::vector<int32_t> result{};
    reply.ReadInt32Vector(&result);
    return result;
}

bool TagSessionProxy::IsPresent(int nativeHandle)
{
    bool result = false;
    MessageParcel data;
    data.WriteInt32(nativeHandle);
    MessageOption option(MessageOption::TF_SYNC);
    ProcessBoolRes(COMMAND_IS_PRESENT, data, option, result);
    return result;
}

bool TagSessionProxy::IsNdef(int nativeHandle)
{
    bool result = false;
    MessageParcel data;
    data.WriteInt32(nativeHandle);
    MessageOption option(MessageOption::TF_SYNC);
    ProcessBoolRes(COMMAND_IS_NDEF, data, option, result);
    return result;
}

std::unique_ptr<ResResult> TagSessionProxy::SendRawFrame(int nativeHandle, std::string msg, bool raw)
{
    MessageParcel data, reply;
    MessageOption option;
    data.WriteInt32(nativeHandle);
    data.WriteString(msg);
    data.WriteBool(raw);
    int res = Remote()->SendRequest(COMMAND_SEND_RAW_FRAME, data, reply, option);
    if (res != ERR_NONE) {
        InfoLog("It is failed To Send Raw Frame with Res(%d).", res);
        // return std::unique_ptr<ResResult>();
    }
    sptr<ResResult> result = reply.ReadStrongParcelable<ResResult>();
    int res1 = reply.ReadInt32();
    if (res1 != ERR_NONE) {
        InfoLog("It is failed To Send Raw Frame with Res1(%d).", res1);
        return std::unique_ptr<ResResult>();
    }
    std::unique_ptr<ResResult> resResult = std::make_unique<ResResult>();
    resResult->SetResult(result->GetResult());
    resResult->SetResData(result->GetResData());
    DebugLog("TagSessionProxy::SendRawFrame result.%d", result->GetResult()) return resResult;
}

std::string TagSessionProxy::NdefRead(int nativeHandle)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    data.WriteInt32(nativeHandle);
    int res = Remote()->SendRequest(COMMAND_NDEF_READ, data, reply, option);
    if (res != ERR_NONE) {
        InfoLog("It is failed To Ndef Read with Res(%d).", res);
        return std::string();
    }
    return reply.ReadString();
}

int TagSessionProxy::NdefWrite(int nativeHandle, std::string msg)
{
    int result = 0;
    MessageParcel data;
    data.WriteInt32(nativeHandle);
    data.WriteString(msg);
    MessageOption option(MessageOption::TF_SYNC);
    ProcessIntRes(COMMAND_NDEF_WRITE, data, option, result);
    return result;
}

int TagSessionProxy::NdefMakeReadOnly(int nativeHandle)
{
    int result = 0;
    MessageParcel data;
    data.WriteInt32(nativeHandle);
    MessageOption option(MessageOption::TF_SYNC);
    ProcessIntRes(COMMAND_NDEF_MAKE_READ_ONLY, data, option, result);
    return result;
}

int TagSessionProxy::FormatNdef(int nativeHandle, const std::string& key)
{
    int result = 0;
    MessageParcel data;
    data.WriteInt32(nativeHandle);
    data.WriteString(key);
    MessageOption option(MessageOption::TF_SYNC);
    ProcessIntRes(COMMAND_FORMAT_NDEF, data, option, result);
    return result;
}

bool TagSessionProxy::CanMakeReadOnly(int technology)
{
    bool result = false;
    MessageParcel data;
    data.WriteInt32(technology);
    MessageOption option(MessageOption::TF_SYNC);
    ProcessBoolRes(COMMAND_CAN_MAKE_READ_ONLY, data, option, result);
    return result;
}

int TagSessionProxy::GetMaxTransceiveLength(int technology)
{
    int result = 0;
    MessageParcel data;
    data.WriteInt32(technology);
    MessageOption option(MessageOption::TF_SYNC);
    ProcessIntRes(COMMAND_GET_MAX_TRANSCEIVE_LENGTH, data, option, result);
    return result;
}

bool TagSessionProxy::IsSupportedApdusExtended()
{
    bool result = false;
    MessageParcel data;
    MessageOption option(MessageOption::TF_SYNC);
    ProcessBoolRes(COMMAND_IS_SUPPORTED_APDUS_EXTENDED, data, option, result);
    return result;
}
}  // namespace reader
}  // namespace nfc
}  // namespace OHOS
