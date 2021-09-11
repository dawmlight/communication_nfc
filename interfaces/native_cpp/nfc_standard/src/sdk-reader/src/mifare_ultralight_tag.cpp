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
#include "mifare_ultralight_tag.h"

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
MifareUltralightTag::MifareUltralightTag(std::weak_ptr<Tag> tag) : BasicTagSession(tag, Tag::NFC_MIFARE_ULTRALIGHT_TECH)
{
    InfoLog("MifareUltralightTag::MifareUltralightTag in");
    if (tag.expired() || tag.lock()->GetTechExtras(Tag::NFC_MIFARE_ULTRALIGHT_TECH).expired()) {
        InfoLog("MifareUltralightTag::MifareUltralightTag tag invalid ");
        return;
    }
    std::weak_ptr<NfcMap> extraData = tag.lock()->GetTechExtras(Tag::NFC_MIFARE_ULTRALIGHT_TECH);

    InfoLog("MifareUltralightTag::MifareUltralightTag sak.%d tagid.%d",
            tag.lock()->GetIntExternData(extraData, Tag::SAK),
            tag.lock()->GetTagId().at(0));
    if ((tag.lock()->GetIntExternData(extraData, Tag::SAK) == 0x00) &&
        tag.lock()->GetTagId().at(0) == NXP_MANUFACTURER_ID) {
        InfoLog("MifareUltralightTag::MifareUltralightTag Ctype.%d",
                tag.lock()->GetIntExternData(extraData, Tag::MIFARE_ULTRALIGHT_C_TYPE));
        if (tag.lock()->GetIntExternData(extraData, Tag::MIFARE_ULTRALIGHT_C_TYPE)) {
            mType_ = EmMifareUltralightType::TYPE_ULTRALIGHT_C;
        } else {
            mType_ = EmMifareUltralightType::TYPE_ULTRALIGHT;
        }
    }
}

MifareUltralightTag::~MifareUltralightTag() {}

std::shared_ptr<MifareUltralightTag> MifareUltralightTag::GetTag(std::weak_ptr<Tag> tag)
{
    InfoLog("MifareUltralightTag::GetTag in tech len.%d ", tag.lock()->GetTagTechnologyList().size());
    if (tag.expired() || !tag.lock()->IsSupportTech(Tag::EmTagTechnology::NFC_MIFARE_ULTRALIGHT_TECH)) {
        InfoLog("MifareUltralightTag::GetTag tag invalid");
        return std::shared_ptr<MifareUltralightTag>();
    }

    return std::make_shared<MifareUltralightTag>(tag);
}

string MifareUltralightTag::ReadMultiplePages(int pageIndex)
{
    InfoLog("MifareUltralightTag::ReadMultiplePages in.");
    if ((pageIndex > 0 && pageIndex < MU_MAX_PAGE_COUNT) && IsConnect()) {
        char command[Tag::SEND_COMMAND_HEAD_LEN_2] = {(char)0x30, char(pageIndex & 0xFF)};
        string sendCommand(command, Tag::SEND_COMMAND_HEAD_LEN_2);
        DebugLog("%02X  %02X   ", command[0], command[1]);

        int response = ResResult::ResponseResult::RESULT_FAILURE;
        return SendCommand(sendCommand, false, response);
    } else {
        DebugLog("[MifareUltralightTag::ReadMultiplePages] pageindex.%d err!", pageIndex)
    }
    return "";
}

int MifareUltralightTag::WriteSinglePages(int pageIndex, const string& data)
{
    InfoLog("MifareUltralightTag::WriteSinglePages in.");
    if (!IsConnect()) {
        DebugLog("[MifareUltralightTag::WriteSinglePages] connect tag first!");
        return NfcErrorCode::NFC_SDK_ERROR_TAG_NOT_CONNECT;
    }
    if ((pageIndex > 0 && pageIndex < MU_MAX_PAGE_COUNT) && (data.size() == MU_PAGE_SIZE)) {
        char command[Tag::SEND_COMMAND_HEAD_LEN_2] = {(char)0xA2, char(pageIndex & 0xFF)};
        string sendCommand(command, Tag::SEND_COMMAND_HEAD_LEN_2);
        sendCommand += data;

        int response = ResResult::ResponseResult::RESULT_FAILURE;
        SendCommand(sendCommand, false, response);
        return response;
    }

    InfoLog("MifareUltralightTag::WriteSinglePages param error!");
    return NfcErrorCode::NFC_SDK_ERROR_INVALID_PARAM;
}

MifareUltralightTag::EmMifareUltralightType MifareUltralightTag::GetType() const
{
    return mType_;
}
}  // namespace sdk
}  // namespace nfc
}  // namespace OHOS