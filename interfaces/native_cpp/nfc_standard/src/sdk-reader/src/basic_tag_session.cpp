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
#include "basic_tag_session.h"

#include "itag_session.h"
#include "loghelper.h"
#include "nfc_sdk_common.h"
using namespace std;
using OHOS::nfc::reader::ITagSession;
using OHOS::nfc::reader::ResResult;

namespace OHOS {
namespace nfc {
namespace sdk {
BasicTagSession::BasicTagSession(std::weak_ptr<Tag> tag, int technology)
    : mTag_(tag), mTagTechnology_(technology), mIsConnect_(false)
{
}

int BasicTagSession::Connect()
{
    if (mTag_.expired()) {
        return NfcErrorCode::NFC_SDK_ERROR_TAG_INVALID;
    }
    OHOS::sptr<ITagSession> tagService = mTag_.lock()->GetTagService();
    if (!tagService) {
        return NfcErrorCode::NFC_SDK_ERROR_NOT_INITIALIZED;
    }
    int tagServiceHandle = GetTagServiceHandle();
    int res = tagService->Connect(tagServiceHandle, mTagTechnology_);
    DebugLog("connect tag.%d tech.%d res.%d. ", tagServiceHandle, mTagTechnology_, res);
    if (res == NfcErrorCode::NFC_SUCCESS) {
        mIsConnect_ = true;
        SetConnectTagTech(mTagTechnology_);
    }
    return res;
}

bool BasicTagSession::IsConnect() const
{
    int connectTech = GetConnectTagTech();
    if ((connectTech != mTagTechnology_) || (connectTech == Tag::NFC_INVALID_TECH)) {
        return false;
    }
    return true;
}

int nfc::sdk::BasicTagSession::Close()
{
    /* Note that we don't want to physically disconnect the tag,
     * but just reconnect to it to reset its state
     */
    OHOS::sptr<ITagSession> tagService = GetTagService();
    if (mTag_.expired() || !tagService) {
        return NfcErrorCode::NFC_SDK_ERROR_NOT_INITIALIZED;
    }

    if (tagService->Reconnect(GetTagServiceHandle()) != NfcErrorCode::NFC_SUCCESS) {
        DebugLog("[BasicTagSession::Close] Reconnect fail!");
    }
    mIsConnect_ = false;
    mTag_.lock()->SetConnectTagTech(Tag::NFC_INVALID_TECH);
    return NfcErrorCode::NFC_SUCCESS;
}

string BasicTagSession::GetTagId()
{
    if (mTag_.expired()) {
        return "";
    }
    return mTag_.lock()->GetTagId();
}

string BasicTagSession::SendCommand(string& data, bool raw, int& response)
{
    DebugLog("BasicTagSession::SendCommand in");
    string result = "";
    OHOS::sptr<ITagSession> tagService = GetTagService();
    if (!tagService) {
        DebugLog("BasicTagSession::SendCommand tagService invalid");
        return result;
    }

    std::unique_ptr<ResResult> res = tagService->SendRawFrame(GetTagServiceHandle(), data, raw);

    if (res) {
        response = res->GetResult();
        if (res->GetResult() == ResResult::ResponseResult::RESULT_SUCCESS) {
            result = res->GetResData();
        }
        DebugLog("[BasicTagSession::SendCommand] result.%d", response);
    }
    return result;
}

int BasicTagSession::GetMaxSendCommandLength() const
{
    if (mTag_.expired() || (mTagTechnology_ == Tag::NFC_INVALID_TECH)) {
        return NfcErrorCode::NFC_SDK_ERROR_INVALID_PARAM;
    }
    OHOS::sptr<ITagSession> tagService = GetTagService();
    if (!tagService) {
        return NfcErrorCode::NFC_SDK_ERROR_NOT_INITIALIZED;
    }
    return tagService->GetMaxTransceiveLength(mTagTechnology_);
}

bool BasicTagSession::IsSupportApduExtended() const
{
    if (mTag_.expired()) {
        return false;
    }
    OHOS::sptr<ITagSession> tagService = GetTagService();
    if (!tagService) {
        return false;
    }
    return tagService->IsSupportedApdusExtended();
}

OHOS::sptr<ITagSession> BasicTagSession::GetTagService() const
{
    DebugLog("BasicTagSession::GetTagService in.");
    if (!IsConnect() || mTag_.expired()) {
        DebugLog("[BasicTagSession::GetTagService] tag is null.");
        return OHOS::sptr<ITagSession>();
    }

    return mTag_.lock()->GetTagService();
}

int BasicTagSession::GetTagServiceHandle() const
{
    if (mTag_.expired()) {
        DebugLog("[BasicTagSession::GetTagServiceHandle] tag is null.");
        return NfcErrorCode::NFC_SDK_ERROR_TAG_INVALID;
    }
    return mTag_.lock()->GetServiceHandle();
}

void nfc::sdk::BasicTagSession::SetConnectTagTech(int tech) const
{
    if (mTag_.expired()) {
        DebugLog("[BasicTagSession::SetConnectTagTech] tag is null.");
        return;
    }
    mTag_.lock()->SetConnectTagTech(tech);
}

int nfc::sdk::BasicTagSession::GetConnectTagTech() const
{
    if (mTag_.expired()) {
        DebugLog("[BasicTagSession::GetConnectTagTech] tag is null.");
        return NfcErrorCode::NFC_SDK_ERROR_TAG_INVALID;
    }

    return mTag_.lock()->GetConnectTagTech();
}
}  // namespace sdk
}  // namespace nfc
}  // namespace OHOS