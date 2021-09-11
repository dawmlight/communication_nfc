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
#include "ndef_formatable_tag.h"

#include "itag_session.h"
#include "loghelper.h"
#include "mifare_classic_tag.h"
#include "ndef_message.h"
#include "nfc_sdk_common.h"
#include "tag.h"

using namespace std;
using OHOS::nfc::reader::ITagSession;

namespace OHOS {
namespace nfc {
namespace sdk {
NdefFormatableTag::NdefFormatableTag(std::weak_ptr<Tag> tag) : BasicTagSession(tag, Tag::NFC_NDEF_FORMATABLE_TECH) {}

std::shared_ptr<NdefFormatableTag> NdefFormatableTag::GetTag(std::weak_ptr<Tag> tag)
{
    if (tag.expired() || !tag.lock()->IsSupportTech(Tag::EmTagTechnology::NFC_NDEF_FORMATABLE_TECH)) {
        return std::shared_ptr<NdefFormatableTag>();
    }

    return std::make_shared<NdefFormatableTag>(tag);
}

int nfc::sdk::NdefFormatableTag::Format(std::weak_ptr<NdefMessage> firstMessage)
{
    return Format(firstMessage, false);
}

int nfc::sdk::NdefFormatableTag::FormatReadOnly(std::weak_ptr<NdefMessage> firstMessage)
{
    return Format(firstMessage, true);
}

int nfc::sdk::NdefFormatableTag::Format(std::weak_ptr<NdefMessage> firstMessage, bool bMakeReadOnly)
{
    OHOS::sptr<ITagSession> tagService = GetTagService();
    if (!tagService) {
        ErrorLog("[NdefTag::ReadNdef] tagService is null.");
        return NfcErrorCode::NFC_SDK_ERROR_TAG_INVALID;
    }

    int tagHandle = GetTagServiceHandle();
    string keyDefault(MifareClassicTag::MC_KEY_DEFAULT, MifareClassicTag::MC_KEY_LEN);
    int res = tagService->FormatNdef(tagHandle, keyDefault);
    if (res != NfcErrorCode::NFC_SUCCESS) {
        return res;
    }

    if (!tagService->IsNdef(tagHandle)) {
        return NfcErrorCode::NFC_SDK_ERROR_TAG_INVALID;
    }

    if (!firstMessage.expired()) {
        string ndefMessage = NdefMessage::MessageToString(firstMessage);
        if (ndefMessage.empty()) {
            return NfcErrorCode::NFC_SDK_ERROR_INVALID_PARAM;
        }
        res = tagService->NdefWrite(tagHandle, ndefMessage);
        if (res != NfcErrorCode::NFC_SUCCESS) {
            return res;
        }
    }

    if (bMakeReadOnly) {
        if (!tagService->CanMakeReadOnly(tagHandle)) {
            return NfcErrorCode::NFC_SDK_ERROR_DISABLE_MAKE_READONLY;
        }
        res = tagService->NdefMakeReadOnly(tagHandle);
        if (res != NfcErrorCode::NFC_SUCCESS) {
            return res;
        }
    }
    return NfcErrorCode::NFC_SUCCESS;
}
}  // namespace sdk
}  // namespace nfc
}  // namespace OHOS