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
#include "tag.h"

#include <cstdio>
#include <iostream>

// #include "pac_map.h"
#include "loghelper.h"
#include "nfc_map.h"
#include "nfc_sdk_common.h"
#include "parcel.h"
#include "refbase.h"
#include "tag_session_proxy.h"

using OHOS::nfc::reader::ITagSession;
using OHOS::nfc::reader::TagSessionProxy;
using namespace std;
namespace OHOS {
namespace nfc {
namespace sdk {
Tag::Tag(std::string& id,
         std::vector<int> technologyList,
         std::weak_ptr<NfcMap> tagExternData,
         int serviceHandle,
         OHOS::sptr<ITagSession> tagService)
    : mServiceHandle_(serviceHandle),
      mConnectTagTech_(NFC_INVALID_TECH),
      mId_(id),
      mTechnologyList_(std::move(technologyList)),
      mTagService_(tagService),
      mTagExternData_(tagExternData.lock())
{
}

Tag::~Tag()
{
    mId_.clear();
    mTechnologyList_.clear();
    mConnectTagTech_ = NFC_INVALID_TECH;
    mServiceHandle_ = 0;
}

bool Tag::IsSupportTech(int technology)
{
    for (int n : mTechnologyList_) {
        if (n == technology) {
            return true;
        }
    }
    return false;
}

OHOS::sptr<ITagSession> Tag::GetTagService() const
{
    return mTagService_;
}

std::weak_ptr<NfcMap> Tag::GetTagExternData() const
{
    return mTagExternData_;
}

std::vector<int> Tag::GetTagTechnologyList() const
{
    return std::move(mTechnologyList_);
}

std::weak_ptr<NfcMap> Tag::GetTechExtras(int tech)
{
    if (!mTagExternData_) {
        return std::shared_ptr<NfcMap>();
    }

    for (int i = 0; i < int(mTechnologyList_.size()); i++) {
        if (tech == mTechnologyList_[i]) {
            return mTagExternData_->GetNfcMap("Tech_Extra_Data_" + to_string(i));
        }
    }
    return std::shared_ptr<NfcMap>();
}

string Tag::GetStringExternData(std::weak_ptr<NfcMap> extraData, const string& externName)
{
    string value = "";
    if (extraData.expired() || externName.empty()) {
        return value;
    }
    std::weak_ptr<std::string> res = extraData.lock()->GetCharArray(externName);
    if (!res.expired()) {
        value = (*res.lock());
    }
    return value;
}

int Tag::GetIntExternData(std::weak_ptr<NfcMap> extraData, const string& externName)
{
    if (extraData.expired()) {
        return NfcErrorCode::NFC_SDK_ERROR_INVALID_PARAM;
    }
    std::weak_ptr<long> exter = extraData.lock()->GetLong(externName);
    if (!exter.expired()) {
        return int(*(exter.lock().get()));
    }
    return NfcErrorCode::NFC_SDK_ERROR_UNKOWN;
}

void Tag::SetConnectTagTech(int connectTech)
{
    mConnectTagTech_ = connectTech;
}

int Tag::GetConnectTagTech() const
{
    return mConnectTagTech_;
}

string Tag::GetTagId() const
{
    return mId_;
}

int Tag::GetServiceHandle() const
{
    return mServiceHandle_;
}

bool Tag::Marshalling(Parcel& parcel) const
{
    DebugLog("Tag::Marshalling");
    if (mTagService_) {
        DebugLog("Tag::mTagService_ is exist.");
    } else {
        DebugLog("Tag::mTagService_ is unexist.");
    }
    parcel.WriteInt32(mServiceHandle_);
    parcel.WriteString(mId_);
    parcel.WriteInt32Vector(mTechnologyList_);

    parcel.WriteObject<IRemoteObject>(mTagService_->AsObject());
    parcel.WriteParcelable(mTagExternData_.get());
    return true;
}

Tag* Tag::Unmarshalling(Parcel& parcel)
{
    DebugLog("Tag::Unmarshalling in");
    int serviceHandle = parcel.ReadInt32();
    std::string id = parcel.ReadString();
    std::vector<int> technologyList;
    parcel.ReadInt32Vector(&technologyList);
    sptr<IRemoteObject> tagService = parcel.ReadObject<IRemoteObject>();
    OHOS::sptr<ITagSession> tagSession = new TagSessionProxy(tagService);
    std::shared_ptr<NfcMap> tagExternData(parcel.ReadParcelable<NfcMap>());

    Tag* tag = new Tag(id, technologyList, tagExternData, serviceHandle, tagSession);
    return tag;
}
}  // namespace sdk
}  // namespace nfc
}  // namespace OHOS