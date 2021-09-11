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
#include "ndef_tag.h"

#include "itag_session.h"
#include "loghelper.h"
#include "ndef_message.h"
#include "nfc_map.h"
#include "nfc_sdk_common.h"
#include "tag.h"

using OHOS::nfc::reader::ITagSession;
using namespace std;
namespace OHOS {
namespace nfc {
namespace sdk {
NdefTag::NdefTag(std::weak_ptr<Tag> tag) : BasicTagSession(tag, Tag::NFC_NDEF_TECH)
{
    DebugLog("NdefTag::NdefTag in");
    if (tag.expired()) {
        DebugLog("NdefTag::NdefTag tag invalid");
        return;
    }
    std::weak_ptr<NfcMap> extraData = tag.lock()->GetTechExtras(Tag::NFC_NDEF_TECH);
    if (extraData.expired()) {
        DebugLog("NdefTag::NdefTag extra data invalid");
        return;
    }

    mNfcForumType_ = (EmNfcForumType)tag.lock()->GetIntExternData(extraData, Tag::NDEF_FORUM_TYPE);
    mNdefTagMode_ = (EmNdefTagMode)tag.lock()->GetIntExternData(extraData, Tag::NDEF_TAG_MODE);
    mNdefMsg_ = tag.lock()->GetStringExternData(extraData, Tag::NDEF_MSG);

    DebugLog("NdefTag::NdefTag mNfcForumType_(%d) mNdefTagMode_(%d) mNdefMsg_(%s)",
             mNfcForumType_,
             mNdefTagMode_,
             mNdefMsg_.c_str());
}

std::shared_ptr<NdefTag> NdefTag::GetTag(std::weak_ptr<Tag> tag)
{
    DebugLog("NdefTag::GetTag in");
    if (tag.expired() || !tag.lock()->IsSupportTech(Tag::EmTagTechnology::NFC_NDEF_TECH)) {
        DebugLog("NdefTag::GetTag tech len.%d", tag.lock()->GetTagTechnologyList().size());
        for (size_t i = 0; i < tag.lock()->GetTagTechnologyList().size(); i++) {
            DebugLog("NdefTag::GetTag index.%d(tech.%d)", i, tag.lock()->GetTagTechnologyList().at(i));
        }

        return std::shared_ptr<NdefTag>();
    }

    return std::make_shared<NdefTag>(tag);
}

NdefTag::EmNfcForumType NdefTag::GetNdefTagType() const
{
    return mNfcForumType_;
}

NdefTag::EmNdefTagMode NdefTag::GetNdefTagMode() const
{
    return mNdefTagMode_;
}

std::shared_ptr<NdefMessage> NdefTag::GetCachedNdefMsg() const
{
    return NdefMessage::GetNdefMessage(mNdefMsg_);
}

string NdefTag::GetNdefTagTypeString(EmNfcForumType emNfcForumType)
{
    InfoLog("NdefTag::GetNdefTagTypeString in");
    string typeString;
    switch (emNfcForumType) {
        case NFC_FORUM_TYPE_1:
            typeString = STRING_NFC_FORUM_TYPE_1;
            break;
        case NFC_FORUM_TYPE_2:
            typeString = STRING_NFC_FORUM_TYPE_2;
            break;
        case NFC_FORUM_TYPE_3:
            typeString = STRING_NFC_FORUM_TYPE_3;
            break;
        case NFC_FORUM_TYPE_4:
            typeString = STRING_NFC_FORUM_TYPE_4;
            break;
        case MIFARE_CLASSIC:
            typeString = STRING_MIFARE_CLASSIC;
            break;
        case ICODE_SLI:
            typeString = STRING_ICODE_SLI;
            break;
        default:
            break;
    }
    DebugLog("[NdefTag::GetNdefTagTypeString] typeString=%d.", emNfcForumType);
    return typeString;
}

bool NdefTag::IsNdefWritable() const
{
    return (mNdefTagMode_ == EmNdefTagMode::MODE_READ_WRITE);
}

std::shared_ptr<NdefMessage> NdefTag::ReadNdef()
{
    DebugLog("NdefTag::ReadNdef in.");
    OHOS::sptr<ITagSession> tagService = GetTagService();
    if (!tagService) {
        DebugLog("[NdefTag::ReadNdef] tagService is null.");
        return std::shared_ptr<NdefMessage>();
    }

    if (tagService->IsNdef(GetTagServiceHandle())) {
        string MessageData = tagService->NdefRead(GetTagServiceHandle());
        if (MessageData.empty() && !tagService->IsPresent(GetTagServiceHandle())) {
            DebugLog(
                "[NdefTag::ReadNdef] read ndef message is null and tag is not "
                "present.");
            return std::shared_ptr<NdefMessage>();
        }

        return NdefMessage::GetNdefMessage(MessageData);
    } else {
        if (!tagService->IsPresent(GetTagServiceHandle())) {
            DebugLog("[NdefTag::ReadNdef] tag is not present.");
            return std::shared_ptr<NdefMessage>();
        }
    }
    return std::shared_ptr<NdefMessage>();
}

int NdefTag::WriteNdef(std::shared_ptr<NdefMessage> msg)
{
    InfoLog("NdefTag::WriteNdef in.");
    if (!IsConnect()) {
        DebugLog("[NdefTag::WriteNdef] connect tag first!");
        return NfcErrorCode::NFC_SDK_ERROR_TAG_NOT_CONNECT;
    }

    OHOS::sptr<ITagSession> tagService = GetTagService();
    if (!tagService) {
        DebugLog("[NdefTag::WriteNdef] tagService is null.");
        return NfcErrorCode::NFC_SDK_ERROR_TAG_NOT_CONNECT;
    }

    if (tagService->IsNdef(GetTagServiceHandle())) {
        string ndefMessage = NdefMessage::MessageToString(msg);
        for (size_t i = 0; i < ndefMessage.size(); i++) {
            printf("%02x ", ndefMessage.at(i));
        }
        printf("\n");
        return tagService->NdefWrite(GetTagServiceHandle(), ndefMessage);
    } else {
        DebugLog("[NdefTag::WriteNdef] is not ndef tag!");
        return NfcErrorCode::NFC_SDK_ERROR_NOT_NDEF_TAG;
    }
}

bool NdefTag::IsEnableReadOnly()
{
    OHOS::sptr<ITagSession> tagService = GetTagService();
    if (!tagService) {
        DebugLog("[NdefTag::IsEnableReadOnly] tagService is null.");
        return 0;
    }
    return tagService->CanMakeReadOnly(mNfcForumType_);
}

int NdefTag::EnableReadOnly()
{
    if (!IsConnect()) {
        DebugLog("[NdefTag::EnableReadOnly] connect tag first!");
        return NfcErrorCode::NFC_SDK_ERROR_TAG_NOT_CONNECT;
    }
    OHOS::sptr<ITagSession> tagService = GetTagService();
    if (!tagService) {
        DebugLog("[NdefTag::EnableReadOnly] tagService is null.");
        return NfcErrorCode::NFC_SDK_ERROR_TAG_NOT_CONNECT;
    }
    return tagService->NdefMakeReadOnly(GetTagServiceHandle());
}
}  // namespace sdk
}  // namespace nfc
}  // namespace OHOS