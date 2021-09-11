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
#include "mifare_classic_tag.h"

#include <cstdio>
#include <cstring>

#include "itag_session.h"
#include "loghelper.h"
#include "nfc_map.h"
#include "nfc_sdk_common.h"
#include "tag.h"

using namespace std;
using OHOS::nfc::reader::ResResult;

namespace OHOS {
namespace nfc {
namespace sdk {
const char MifareClassicTag::MC_KEY_DEFAULT[MC_KEY_LEN] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
const char MifareClassicTag::MC_KEY_MIFARE_APPLICATION_DIRECTORY[MC_KEY_LEN] = {0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5};
const char MifareClassicTag::MC_KEY_NFC_FORUM[MC_KEY_LEN] = {0xD3, 0xF7, 0xD3, 0xF7, 0xD3, 0xF7};

MifareClassicTag::MifareClassicTag(std::weak_ptr<Tag> tag) : BasicTagSession(tag, Tag::NFC_MIFARE_CLASSIC_TECH)
{
    DebugLog("MifareClassicTag::MifareClassicTag in");
    if (tag.expired() || tag.lock()->GetTechExtras(Tag::NFC_MIFARE_CLASSIC_TECH).expired()) {
        DebugLog("MifareClassicTag::MifareClassicTag tag invalid");
        return;
    }
    std::weak_ptr<NfcMap> extraData = tag.lock()->GetTechExtras(Tag::NFC_MIFARE_CLASSIC_TECH);
    int sak = tag.lock()->GetIntExternData(extraData, Tag::SAK);
    string atqa = tag.lock()->GetStringExternData(extraData, Tag::ATQA);

    DebugLog("MifareClassicTag::MifareClassicTag sak.%d atqa.(%d)%s", sak, atqa.size(), atqa.c_str());
    for (size_t i = 0; i < atqa.size(); i++) {
        printf("%02x ", atqa.at(i));
    }
    printf("\n");

    mIsEmulated_ = false;
    mMifareTagType_ = EmMifareTagType::TYPE_CLASSIC;

    switch (sak) {
        case 0x01:
        case 0x08:
        case 0x88:
            mSize_ = MC_SIZE_1K;
            break;
        case 0x09:
            mSize_ = MC_SIZE_MINI;
            break;
        case 0x10:
            mSize_ = MC_SIZE_2K;
            mMifareTagType_ = EmMifareTagType::TYPE_PLUS;
            break;
        case 0x11:
            mSize_ = MC_SIZE_4K;
            mMifareTagType_ = EmMifareTagType::TYPE_PLUS;
            break;
        case 0x18:
            mSize_ = MC_SIZE_4K;
            break;
        case 0x28:
            mSize_ = MC_SIZE_1K;
            mIsEmulated_ = true;
            break;
        case 0x38:
            mSize_ = MC_SIZE_4K;
            mIsEmulated_ = true;
            break;
        case 0x98:
        case 0xB8:
            mSize_ = MC_SIZE_4K;
            mMifareTagType_ = EmMifareTagType::TYPE_PRO;
            break;
        default:
            break;
    }
}

std::shared_ptr<MifareClassicTag> MifareClassicTag::GetTag(std::weak_ptr<Tag> tag)
{
    if (tag.expired() || !tag.lock()->IsSupportTech(Tag::EmTagTechnology::NFC_MIFARE_CLASSIC_TECH)) {
        return std::shared_ptr<MifareClassicTag>();
    }

    return std::make_shared<MifareClassicTag>(tag);
}

bool MifareClassicTag::AuthenticateSector(int sectorIndex, const string& key, bool bIsKeyA)
{
    if ((sectorIndex < 0 || sectorIndex >= MC_MAX_SECTOR_COUNT) || !IsConnect() || key.empty()) {
        ErrorLog(
            "[MifareClassicTag::AuthenticateSector] param err! sectorIndex.%d "
            "keyLen.%d",
            sectorIndex,
            key.size());
        return false;
    }

    char command[Tag::SEND_COMMAND_MAX_LEN];
    int commandLen = 0;
    if (bIsKeyA) {
        command[commandLen++] = 0x60;
    } else {
        command[commandLen++] = 0x61;
    }

    command[commandLen++] = char(GetBlockIdexFromSector(sectorIndex));
    string sendCommand(command, commandLen);
    string tagId = GetTagId();
    static const int tagSubLen = 4;
    // Take the first 4 bytes of the tag as part of command
    sendCommand += tagId.substr(0, tagSubLen) + key;

    int response = ResResult::ResponseResult::RESULT_FAILURE;
    SendCommand(sendCommand, false, response);
    return (response == ResResult::ResponseResult::RESULT_SUCCESS);
}

string MifareClassicTag::ReadSingleBlock(int blockIndex)
{
    InfoLog("MifareClassicTag::ReadSingleBlock in");
    if ((blockIndex < 0 || blockIndex >= MC_MAX_BLOCK_INDEX) || !IsConnect()) {
        DebugLog("[MifareClassicTag::ReadSingleBlock] blockIndex= %d err", blockIndex);
        return "";
    }

    char command[Tag::SEND_COMMAND_HEAD_LEN_2] = {(char)0x30, char(blockIndex & 0xFF)};
    string sendCommand(command, Tag::SEND_COMMAND_HEAD_LEN_2);

    int response = ResResult::ResponseResult::RESULT_FAILURE;
    return SendCommand(sendCommand, false, response);
}

int MifareClassicTag::WriteSingleBlock(int blockIndex, const string& data)
{
    InfoLog("MifareClassicTag::WriteSingleBlock in");
    if (!IsConnect()) {
        DebugLog("[MifareClassicTag::WriteSingleBlock] connect tag first!");
        return NfcErrorCode::NFC_SDK_ERROR_TAG_NOT_CONNECT;
    }
    if ((blockIndex < 0 || blockIndex >= MC_MAX_BLOCK_INDEX) || (data.size() != MC_BLOCK_SIZE)) {
        DebugLog("[MifareClassicTag::WriteSingleBlock] blockIndex= %d dataLen= %d err", blockIndex, data.size());
        return NfcErrorCode::NFC_SDK_ERROR_INVALID_PARAM;
    }

    char command[Tag::SEND_COMMAND_HEAD_LEN_2] = {(char)0xA0, char(blockIndex & 0xFF)};
    string sendCommand(command, Tag::SEND_COMMAND_HEAD_LEN_2);
    sendCommand += data;

    int response = ResResult::ResponseResult::RESULT_FAILURE;
    SendCommand(sendCommand, false, response);
    return response;
}

int MifareClassicTag::IncrementBlock(int blockIndex, int value)
{
    InfoLog("MifareClassicTag::IncrementBlock in");
    if (!IsConnect()) {
        DebugLog("[MifareClassicTag::IncrementBlock] connect tag first!");
        return NfcErrorCode::NFC_SDK_ERROR_TAG_NOT_CONNECT;
    }
    if ((blockIndex < 0 || blockIndex >= MC_MAX_BLOCK_INDEX) || value < 0) {
        DebugLog("[MifareClassicTag::IncrementBlock] blockIndex= %d value=%d err", blockIndex, value);
        return NfcErrorCode::NFC_SDK_ERROR_INVALID_PARAM;
    }

    char command[Tag::SEND_COMMAND_HEAD_LEN_2] = {(char)0xC1, char(blockIndex & 0xFF)};
    string sendCommand(command, Tag::SEND_COMMAND_HEAD_LEN_2);
    sendCommand += NfcSdkCommon::IntToString(value, NfcSdkCommon::IsLittleEndian());

    int response = ResResult::ResponseResult::RESULT_FAILURE;
    SendCommand(sendCommand, false, response);
    return response;
}

int MifareClassicTag::DecrementBlock(int blockIndex, int value)
{
    InfoLog("MifareClassicTag::DecrementBlock in");
    if (!IsConnect()) {
        DebugLog("[MifareClassicTag::DecrementBlock] connect tag first!");
        return NfcErrorCode::NFC_SDK_ERROR_TAG_NOT_CONNECT;
    }
    if (blockIndex < 0 || blockIndex >= MC_MAX_BLOCK_INDEX || value < 0) {
        DebugLog("[MifareClassicTag::DecrementBlock] blockIndex= %d value=%d err", blockIndex, value);
        return NfcErrorCode::NFC_SDK_ERROR_INVALID_PARAM;
    }

    char command[Tag::SEND_COMMAND_HEAD_LEN_2] = {(char)0xC0, char(blockIndex & 0xFF)};
    string sendCommand(command, Tag::SEND_COMMAND_HEAD_LEN_2);
    sendCommand += NfcSdkCommon::IntToString(value, NfcSdkCommon::IsLittleEndian());

    int response = ResResult::ResponseResult::RESULT_FAILURE;
    SendCommand(sendCommand, false, response);
    return response;
}

int MifareClassicTag::TransferToBlock(int blockIndex)
{
    InfoLog("MifareClassicTag::TransferToBlock in");
    if (!IsConnect()) {
        DebugLog("[MifareClassicTag::TransferToBlock] connect tag first!");
        return NfcErrorCode::NFC_SDK_ERROR_TAG_NOT_CONNECT;
    }
    if (blockIndex < 0 || blockIndex >= MC_MAX_BLOCK_INDEX) {
        DebugLog("[MifareClassicTag::TransferToBlock] blockIndex= %d err", blockIndex);
        return NfcErrorCode::NFC_SDK_ERROR_INVALID_PARAM;
    }
    char command[Tag::SEND_COMMAND_HEAD_LEN_2] = {(char)0xB0, char(blockIndex & 0xFF)};
    string sendCommand(command, Tag::SEND_COMMAND_HEAD_LEN_2);

    int response = ResResult::ResponseResult::RESULT_FAILURE;
    SendCommand(sendCommand, false, response);
    return response;
}

int MifareClassicTag::RestoreFromBlock(int blockIndex)
{
    InfoLog("MifareClassicTag::RestoreFromBlock in");
    if (!IsConnect()) {
        DebugLog("[MifareClassicTag::TransferToBlock] connect tag first!");
        return NfcErrorCode::NFC_SDK_ERROR_TAG_NOT_CONNECT;
    }
    if (blockIndex < 0 || blockIndex >= MC_MAX_BLOCK_INDEX) {
        DebugLog("[MifareClassicTag::RestoreFromBlock] blockIndex= %d err", blockIndex);
        return NfcErrorCode::NFC_SDK_ERROR_INVALID_PARAM;
    }
    char command[Tag::SEND_COMMAND_HEAD_LEN_2] = {(char)0xC2, char(blockIndex & 0xFF)};
    string sendCommand(command, Tag::SEND_COMMAND_HEAD_LEN_2);

    int response = ResResult::ResponseResult::RESULT_FAILURE;
    SendCommand(sendCommand, false, response);
    return response;
}

int MifareClassicTag::GetSectorCount() const
{
    size_t count = 0;
    switch (mSize_) {
        case MC_SIZE_1K:
            count = MC_SECTOR_COUNT_OF_SIZE_1K;
            break;
        case MC_SIZE_2K:
            count = MC_SECTOR_COUNT_OF_SIZE_2K;
            break;
        case MC_SIZE_4K:
            count = MC_MAX_SECTOR_COUNT;
            break;
        case MC_SIZE_MINI:
            count = MC_SECTOR_COUNT_OF_SIZE_MINI;
            break;
        default:
            break;
    }
    return count;
}

int MifareClassicTag::GetBlockCountInSector(int sectorIndex) const
{
    if (sectorIndex >= 0 && sectorIndex < MC_SECTOR_COUNT_OF_SIZE_2K) {
        return MC_BLOCK_COUNT;
    } else if (sectorIndex >= MC_SECTOR_COUNT_OF_SIZE_2K && sectorIndex < MC_MAX_SECTOR_COUNT) {
        return MC_BLOCK_COUNT_OF_4K;
    }
    return NfcErrorCode::NFC_SDK_ERROR_UNKOWN;
}

size_t MifareClassicTag::GetMifareTagType() const
{
    return mMifareTagType_;
}

int MifareClassicTag::GetSize() const
{
    return mSize_;
}

bool MifareClassicTag::IsEmulated() const
{
    return mIsEmulated_;
}

int MifareClassicTag::GetBlockIdexFromSector(int sectorIndex) const
{
    if (sectorIndex >= 0 && sectorIndex < MC_SECTOR_COUNT_OF_SIZE_2K) {
        return sectorIndex * MC_BLOCK_COUNT;
    }
    if (sectorIndex >= MC_SECTOR_COUNT_OF_SIZE_2K && sectorIndex < MC_MAX_SECTOR_COUNT) {
        return MC_SECTOR_COUNT_OF_SIZE_2K * MC_BLOCK_COUNT +
               (sectorIndex - MC_SECTOR_COUNT_OF_SIZE_2K) * MC_BLOCK_COUNT_OF_4K;
    }
    return NfcErrorCode::NFC_SDK_ERROR_UNKOWN;
}
}  // namespace sdk
}  // namespace nfc
}  // namespace OHOS