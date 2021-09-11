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
#include "isodep_tag.h"

#include "loghelper.h"
#include "nfc_map.h"
#include "nfc_sdk_common.h"
#include "tag.h"

using namespace std;

namespace OHOS {
namespace nfc {
namespace sdk {
IsoDepTag::IsoDepTag(std::weak_ptr<Tag> tag) : BasicTagSession(tag, Tag::NFC_ISO_DEP_TECH)
{
    DebugLog("IsoDepTag::IsoDepTag in");
    if (tag.expired() || tag.lock()->GetTechExtras(Tag::NFC_ISO_DEP_TECH).expired()) {
        DebugLog("IsoDepTag::IsoDepTag tag invalid ");
        return;
    }
    std::weak_ptr<NfcMap> extraData = tag.lock()->GetTechExtras(Tag::NFC_ISO_DEP_TECH);
    mHistoricalBytes_ = tag.lock()->GetStringExternData(extraData, Tag::HISTORICAL_BYTES);
    mHiLayerResponse_ = tag.lock()->GetStringExternData(extraData, Tag::HILAYER_RESPONSE);
    // iso 14443-3a
    mSak = tag.lock()->GetIntExternData(extraData, Tag::SAK);
    mAtqa_ = tag.lock()->GetStringExternData(extraData, Tag::ATQA);
    // iso 14443-3b
    mAppData_ = tag.lock()->GetStringExternData(extraData, Tag::APP_DATA);
    mProtocolInfo_ = tag.lock()->GetStringExternData(extraData, Tag::PROTOCOL_INFO);

    DebugLog(
        "IsoDepTag::IsoDepTag mHistoricalBytes_(%s) mHiLayerResponse_(%s) mSak(%d) mAtqa_(%s) "
        "mAppData_(%s) mProtocolInfo_(%s)",
        mHistoricalBytes_.c_str(),
        mHiLayerResponse_.c_str(),
        mSak,
        mAtqa_.c_str(),
        mAppData_.c_str(),
        mProtocolInfo_.c_str());
}

std::shared_ptr<IsoDepTag> IsoDepTag::GetTag(std::weak_ptr<Tag> tag)
{
    DebugLog("IsoDepTag::GetTag in");
    if (tag.expired() || !tag.lock()->IsSupportTech(Tag::EmTagTechnology::NFC_ISO_DEP_TECH)) {
        DebugLog("IsoDepTag::GetTag err");
        return std::shared_ptr<IsoDepTag>();
    }

    return std::make_shared<IsoDepTag>(tag);
}

string IsoDepTag::GetHistoricalBytes() const
{
    return mHistoricalBytes_;
}

string IsoDepTag::GetHiLayerResponse() const
{
    return mHiLayerResponse_;
}

int IsoDepTag::GetSak() const
{
    return mSak;
}

string IsoDepTag::GetAtqa() const
{
    return mAtqa_;
}

string IsoDepTag::GetAppData() const
{
    return mAppData_;
}

string IsoDepTag::GetProtocolInfo() const
{
    return mProtocolInfo_;
}
}  // namespace sdk
}  // namespace nfc
}  // namespace OHOS