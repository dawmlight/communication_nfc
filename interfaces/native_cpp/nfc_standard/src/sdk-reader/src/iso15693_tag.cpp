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
#include "iso15693_tag.h"

#include <cstring>

#include "itag_session.h"
#include "loghelper.h"
#include "nfc_map.h"
#include "nfc_sdk_common.h"

using namespace std;
using OHOS::nfc::reader::ResResult;

namespace OHOS {
namespace nfc {
namespace sdk {
Iso15693Tag::Iso15693Tag(std::weak_ptr<Tag> tag) : BasicTagSession(tag, Tag::EmTagTechnology::NFC_ISO_15693_TECH)
{
    if (tag.expired() || tag.lock()->GetTechExtras(Tag::NFC_ISO_15693_TECH).expired()) {
        return;
    }
    std::weak_ptr<NfcMap> extraData = tag.lock()->GetTechExtras(Tag::NFC_ISO_15693_TECH);
    mDsfId_ = char(tag.lock()->GetIntExternData(extraData, Tag::DSF_ID));
    mRespFlags_ = char(tag.lock()->GetIntExternData(extraData, Tag::RESPONSE_FLAGS));
}

Iso15693Tag::~Iso15693Tag()
{
    mDsfId_ = 0;
    mRespFlags_ = 0;
}

std::shared_ptr<Iso15693Tag> Iso15693Tag::GetTag(std::weak_ptr<Tag> tag)
{
    if (tag.expired() || !tag.lock()->IsSupportTech(Tag::EmTagTechnology::NFC_ISO_15693_TECH)) {
        return std::shared_ptr<Iso15693Tag>();
    }

    return std::make_shared<Iso15693Tag>(tag);
}

string Iso15693Tag::ReadSingleBlock(int flag, int blockIndex)
{
    InfoLog("Iso15693Tag::ReadSingleBlock in flag= %d blockIndex= %d", flag, blockIndex);
    if ((flag < 0 || flag >= ISO15693_MAX_FLAG_COUNT) || (blockIndex < 0 || blockIndex >= ISO15693_MAX_BLOCK_INDEX) ||
        !IsConnect()) {
        DebugLog("[Iso15693Tag::ReadSingleBlock] flag= %d blockIndex= %d err", flag, blockIndex);
        return "";
    }

    string tagId = GetTagId();
    char command[Tag::SEND_COMMAND_HEAD_LEN_2] = {char(flag & 0xFF), 0x20};
    string sendCommand(command, Tag::SEND_COMMAND_HEAD_LEN_2);
    sendCommand = sendCommand + tagId + char(blockIndex & 0xFF);

    int response = ResResult::ResponseResult::RESULT_FAILURE;
    return SendCommand(sendCommand, false, response);
}

int Iso15693Tag::WriteSingleBlock(int flag, int blockIndex, const string& data)
{
    InfoLog("Iso15693Tag::WriteSingleBlock in");
    if (!IsConnect()) {
        DebugLog("[Iso15693Tag::WriteSingleBlock] connect tag first!");
        return NfcErrorCode::NFC_SDK_ERROR_TAG_NOT_CONNECT;
    }
    if ((flag < 0 || flag >= ISO15693_MAX_FLAG_COUNT) || (blockIndex < 0 || blockIndex >= ISO15693_MAX_BLOCK_INDEX)) {
        DebugLog("[Iso15693Tag::WriteSingleBlock] flag= %d blockIndex= %d err", flag, blockIndex);
        return NfcErrorCode::NFC_SDK_ERROR_INVALID_PARAM;
    }

    string tagId = GetTagId();
    char command[Tag::SEND_COMMAND_HEAD_LEN_2] = {char(flag & 0xFF), 0x21};
    string sendCommand(command, Tag::SEND_COMMAND_HEAD_LEN_2);
    sendCommand = sendCommand + tagId + char(blockIndex & 0xFF) + data;

    int response = ResResult::ResponseResult::RESULT_FAILURE;
    string res = SendCommand(sendCommand, false, response);
    return response;
}

int Iso15693Tag::LockSingleBlock(int flag, int blockIndex)
{
    InfoLog("Iso15693Tag::LockSingleBlock in");
    if (!IsConnect()) {
        DebugLog("[Iso15693Tag::LockSingleBlock] connect tag first!");
        return NfcErrorCode::NFC_SDK_ERROR_TAG_NOT_CONNECT;
    }
    if ((flag < 0 || flag >= ISO15693_MAX_FLAG_COUNT) || (blockIndex < 0 || blockIndex >= ISO15693_MAX_BLOCK_INDEX)) {
        DebugLog("[Iso15693Tag::LockSingleBlock] flag= %d blockIndex= %d err", flag, blockIndex);
        return NfcErrorCode::NFC_SDK_ERROR_INVALID_PARAM;
    }

    string tagId = GetTagId();
    char command[Tag::SEND_COMMAND_HEAD_LEN_2] = {char(flag & 0xFF), 0x22};
    string sendCommand(command, Tag::SEND_COMMAND_HEAD_LEN_2);
    sendCommand = sendCommand + tagId + char(blockIndex & 0xFF);

    int response = ResResult::ResponseResult::RESULT_FAILURE;
    string res = SendCommand(sendCommand, false, response);
    return response;
}

string Iso15693Tag::ReadMultipleBlock(int flag, int blockIndex, int blockNum)
{
    InfoLog("Iso15693Tag::ReadMultipleBlock in flag= %d blockIndex= %d blockNum=%d", flag, blockIndex, blockNum);
    if ((flag < 0 || flag >= ISO15693_MAX_FLAG_COUNT) || (blockIndex < 0 || blockIndex >= ISO15693_MAX_BLOCK_INDEX) ||
        (blockNum < 0 || blockNum >= ISO15693_MAX_BLOCK_INDEX) || !IsConnect()) {
        DebugLog(
            "[Iso15693Tag::ReadMultipleBlock] flag= %d blockIndex= %d "
            "blockNum=%d err",
            flag,
            blockIndex,
            blockNum);
        return "";
    }

    string tagId = GetTagId();
    char command[Tag::SEND_COMMAND_HEAD_LEN_2] = {char(flag & 0xFF), 0x23};
    string sendCommand(command, Tag::SEND_COMMAND_HEAD_LEN_2);
    sendCommand = sendCommand + tagId + char(blockIndex & 0xFF) + char(blockNum & 0xFF);

    int response = ResResult::ResponseResult::RESULT_FAILURE;
    return SendCommand(sendCommand, false, response);
}

int Iso15693Tag::WriteMultipleBlock(int flag, int blockIndex, int blockNum, const string& data)
{
    InfoLog("Iso15693Tag::WriteMultipleBlock in");
    if (!IsConnect()) {
        DebugLog("[Iso15693Tag::WriteMultipleBlock] connect tag first!");
        return NfcErrorCode::NFC_SDK_ERROR_TAG_NOT_CONNECT;
    }
    if ((flag < 0 || flag >= ISO15693_MAX_FLAG_COUNT) || (blockIndex < 0 || blockIndex >= ISO15693_MAX_BLOCK_INDEX) ||
        (blockNum <= 0 || blockNum > ISO15693_MAX_BLOCK_INDEX)) {
        DebugLog("[Iso15693Tag::WriteMultipleBlock] flag=%d blockIndex= %d err", flag, blockIndex);
        return NfcErrorCode::NFC_SDK_ERROR_INVALID_PARAM;
    }

    string tagId = GetTagId();
    char command[Tag::SEND_COMMAND_HEAD_LEN_2] = {char(flag & 0xFF), 0x24};
    string sendCommand(command, Tag::SEND_COMMAND_HEAD_LEN_2);
    sendCommand = sendCommand + tagId + char(blockIndex & 0xFF) + char(blockNum & 0xFF) + data;

    int response = ResResult::ResponseResult::RESULT_FAILURE;
    string res = SendCommand(sendCommand, false, response);
    return response;
}

char Iso15693Tag::GetDsfId() const
{
    return mDsfId_;
}

char Iso15693Tag::GetRespFlags() const
{
    return mRespFlags_;
}
}  // namespace sdk
}  // namespace nfc
}  // namespace OHOS