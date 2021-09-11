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

#include "aid_set.h"

#include <algorithm>
#include <iterator>

#include "aid_string.h"
#include "card_emulation_def.h"
#include "card_emulation_service_info.h"
#include "element_name.h"
#include "loghelper.h"

#ifdef DEBUG
#include <sstream>
#endif  // DEBUG

#ifdef USE_HILOG
DEFINE_NFC_LOG_LABEL("AidSet");
#endif

namespace OHOS::nfc::cardemulation {
static const std::string INFO_KEY_AID_SET_DESCRIPTION = "description";
static const std::string INFO_KEY_AID_SET_TYPE = "type";
static const std::string INFO_KEY_AID_SET_AIDS = "aids";
static bool AssignFromRawString(AidSet* aidset, const std::vector<std::string>& aids)
{
    if (aidset == nullptr) {
        return false;
    }
    bool hasInvalid = false;
    std::for_each(aids.cbegin(), aids.cend(), [&aidset, &hasInvalid](const std::string& s) {
        auto aidStr = AidString(s);
        if (aidStr.IsValid()) {
            aidset->AddAidString(std::move(aidStr));
        } else {
            hasInvalid = true;
        }
    });
    if (hasInvalid) {
        aidset->Clear();
        return false;
    }
    return true;
}

static bool AssignFromAidString(AidSet* aidset, const std::vector<AidString>& aids)
{
    if (aidset == nullptr) {
        return false;
    }

    bool hasInvalid = false;
    std::for_each(aids.cbegin(), aids.cend(), [&aidset, &hasInvalid](const AidString& aidStr) {
        if (aidStr.IsValid()) {
            aidset->AddAidString(std::move(aidStr));
        } else {
            hasInvalid = true;
        }
    });
    if (hasInvalid) {
        aidset->Clear();
        return false;
    }
    return true;
}

void from_json(const nlohmann::json& j, std::shared_ptr<AidSet>& info)
{
    info = std::make_shared<AidSet>();
    info->FromJson(j);
}

void to_json(nlohmann::json& j, const std::shared_ptr<AidSet>& info)
{
    if (info) {
        info->ToJson(j);
    }
}

void from_json(const nlohmann::json& j, AidSet& aidset)
{
    aidset.FromJson(j);
}

void to_json(nlohmann::json& j, const AidSet& aidset)
{
    aidset.ToJson(j);
}

AidSet AidSet::From(const std::vector<std::string>& aids)
{
    AidSet aidset;
    AssignFromRawString(&aidset, aids);
    return aidset;
}

AidSet AidSet::From(const std::vector<AidString>& aids)
{
    AidSet aidset;
    AssignFromAidString(&aidset, aids);
    return aidset;
}

std::unique_ptr<AidSet> AidSet::FromRawString(const std::vector<std::string>& aids)
{
    std::unique_ptr<AidSet> aidset = std::make_unique<AidSet>();
    AssignFromRawString(aidset.get(), aids);
    return aidset;
}

std::unique_ptr<AidSet> AidSet::FromAidString(const std::vector<AidString>& aids)
{
    std::unique_ptr<AidSet> aidset = std::make_unique<AidSet>();
    AssignFromAidString(aidset.get(), aids);
    return aidset;
}

AidSet::AidSet() : AidSet("")
{
};

AidSet::AidSet(std::string tp) : type_(std::move(tp)), owner_(), aidset_()
{
}

AidSet::~AidSet() = default;

AidSet::AidSet(const AidSet& other) : AidSet("")
{
    Copy(other);
}

AidSet& AidSet::operator=(const AidSet& other)
{
    if (this != &other) {
        Copy(other);
    }

    return *this;
}

AidSet::AidSet(AidSet&& other) : AidSet("")
{
    Move(std::move(other));
}

AidSet& AidSet::operator=(AidSet&& other)
{
    if (this != &other) {
        Move(std::move(other));
    }

    return *this;
}

std::string AidSet::GetType() const
{
    return type_;
}

void AidSet::SetType(std::string tp)
{
    type_ = std::move(tp);
}

std::string AidSet::GetDescription() const
{
    return description_;
}

void AidSet::SetDescription(std::string description)
{
    description_ = description;
}

std::shared_ptr<OHOS::AppExecFwk::ElementName> AidSet::GetOwnerName() const
{
    auto owner = GetOwner().lock();
    if (owner) {
        return owner->GetName();
    }
    return std::shared_ptr<OHOS::AppExecFwk::ElementName>();
}

std::weak_ptr<CardEmulationServiceInfo> AidSet::GetOwner() const
{
    return owner_;
}

void AidSet::SetOwner(const std::shared_ptr<CardEmulationServiceInfo>& owner)
{
    owner_ = owner;
}

std::string AidSet::GetExecutionEnvironment() const
{
    auto o = GetOwner().lock();
    if (o) {
        return o->GetExecutionEnvironment();
    }
    return std::string();
}

bool AidSet::Empty() const
{
    return aidset_.empty();
}

size_t AidSet::Size() const
{
    return aidset_.size();
}

bool AidSet::HasAidString(const AidString& aid)
{
    return std::find_if(aidset_.cbegin(), aidset_.cend(), [&aid](std::shared_ptr<AidString> p) -> bool {
               return (p && p->EqualToWithoutPattern(aid));
           }) != aidset_.cend();
}

void AidSet::Visit(std::function<void(const std::string&, const AidString&)> visitor) const
{
    std::for_each(aidset_.cbegin(), aidset_.cend(), [&visitor, this](std::shared_ptr<AidString> p) {
        visitor(this->GetType(), *p);
    });
}

std::vector<std::shared_ptr<AidString>> AidSet::GetAll() const
{
    std::vector<std::shared_ptr<AidString>> ret;
    ret.reserve(aidset_.size());
    std::for_each(aidset_.cbegin(), aidset_.cend(), [&ret](std::shared_ptr<AidString> p) {
        ret.emplace_back(std::make_shared<AidString>(*p));
    });

    return ret;
}

std::vector<std::string> nfc::cardemulation::AidSet::GetAllAidRawString(bool fillSpace) const
{
    std::vector<std::string> ret;
    ret.reserve(aidset_.size());
    std::for_each(aidset_.cbegin(), aidset_.cend(), [&ret, fillSpace](std::shared_ptr<AidString> p) {
        ret.emplace_back(p->ToString(false, fillSpace));
    });

    return ret;
}
#ifdef DEBUG
std::string AidSet::ToString() const
{
    std::stringstream ss;
    ss << "type: " << type_ << " EE: " << GetExecutionEnvironment();
    ss << "\naids: [ ";
    std::for_each(aidset_.cbegin(), aidset_.cend(), [&ss](std::shared_ptr<AidString> p) {
        ss << (p->ToString()) << " ";
    });

    ss << " ]";

    return ss.str();
}
#endif  // DEBUG

bool AidSet::AddAidString(AidString aid)
{
    bool added = false;
    if (aid.IsValid()) {
        bool isExisted = false;
        for (auto&& a : aidset_) {
            if (a->ConflictWith(aid)) {
                isExisted = true;
                break;
            }
        }
        if (!isExisted) {
            aidset_.emplace(std::make_shared<AidString>(std::move(aid)));

            added = true;
        }
    }
    return added;
}

void AidSet::RemoveAidString(const AidString& aid)
{
    auto pos = Find(aid);
    if (pos != aidset_.cend()) {
        aidset_.erase(pos);
    }
}

void AidSet::Clear()
{
    type_.clear();
    aidset_.clear();
    owner_.reset();
    description_.clear();
}

bool AidSet::Equals(const AidSet& other) const
{
    if (this == &other) {
        return true;
    }
    if (type_ == other.type_ && AidsEqual(other.aidset_)) {
        return true;
    }
    return false;
}

bool AidSet::operator==(const AidSet& other) const
{
    return this->Equals(other);
}

bool AidSet::ConflictWith(const AidSet& other, bool nameEnabled) const
{
    if (this == &other) {
        return true;
    }
    if (nameEnabled && type_.compare(other.GetType()) == 0) {
        return true;
    }
    std::vector<std::shared_ptr<AidString>> aids;

    std::set_intersection(aidset_.begin(),
                          aidset_.end(),
                          other.aidset_.begin(),
                          other.aidset_.end(),
                          std::back_inserter(aids),
                          AidStringSPtrCompare());
    return !aids.empty();
}
void AidSet::swap(AidSet& other) noexcept
{
    using std::swap;
    swap(type_, other.type_);
    swap(owner_, other.owner_);
    swap(aidset_, other.aidset_);
    swap(description_, other.description_);
}
void AidSet::FromJson(const nlohmann::json& j)
{
    auto aids = j.at(INFO_KEY_AID_SET_AIDS).get<std::vector<std::string>>();
    auto aidsetTmp = AidSet::FromRawString(aids);

    aidsetTmp->SetDescription(j.at(INFO_KEY_AID_SET_DESCRIPTION).get<std::string>());

    auto type = j.at(INFO_KEY_AID_SET_TYPE).get<std::string>();
    if (type != CARDEMULATION_SERVICE_TYPE_SECURE && type != CARDEMULATION_SERVICE_TYPE_NORMAL) {
        type = CARDEMULATION_SERVICE_TYPE_NORMAL;
    }

    aidsetTmp->SetType(type);

    swap(*aidsetTmp);
}
void AidSet::ToJson(nlohmann::json& j) const
{
    std::vector<std::string> aids;
    for (auto&& aid : aidset_) {
        if (aid) {
            aids.emplace_back(aid->ToString());
        }
    }
    j = nlohmann::json{
        {INFO_KEY_AID_SET_TYPE, type_},
        {INFO_KEY_AID_SET_DESCRIPTION, description_},
        {INFO_KEY_AID_SET_AIDS, aids},
    };
}
aidset_t::const_iterator AidSet::Find(const AidString& aid) const
{
    return std::find_if(aidset_.cbegin(), aidset_.cend(), [&aid](std::shared_ptr<AidString> p) -> bool {
        return (p && p->ExactlyEqualTo(aid));
    });
}

bool AidSet::AidsEqual(const aidset_t& aids) const
{
    auto eq = [](const std::shared_ptr<AidString>& aid1, const std::shared_ptr<AidString>& aid2) {
        if (aid1 && aid2) {
            return *aid1 == *aid2;
        }
        return false;
    };
    return std::equal(aidset_.begin(), aidset_.end(), aids.begin(), aids.end(), eq);
}

void AidSet::Copy(const AidSet& other)
{
    if (this != &other) {
        type_ = other.type_;
        owner_ = other.owner_;

        description_ = other.description_;
        std::copy(other.aidset_.cbegin(), other.aidset_.cend(), std::inserter(aidset_, aidset_.cend()));
    }
}

void AidSet::Move(AidSet&& other)
{
    type_ = std::move(other.type_);
    owner_ = std::move(other.owner_);
    aidset_ = std::move(other.aidset_);
    description_ = std::move(other.description_);
}
}  // namespace OHOS::nfc::cardemulation
