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

#include "card_emulation_service_info.h"

#include <algorithm>
#include <cstdio>
#include <iostream>
#include <sstream>

#include "aid_set.h"
#include "apdu_channel.h"
#include "card_emulation_def.h"
#include "card_emulation_error.h"
#include "card_emulation_service_info_lite.h"
#include "element_name.h"
#include "loghelper.h"

#ifdef USE_HILOG
DEFINE_NFC_LOG_LABEL("CardEmulationServiceInfo");
#endif

namespace OHOS::nfc::cardemulation {
static const std::string INFO_KEY_NAME = "name";
static const std::string INFO_KEY_ABILITY_NAME = "ability_name";
static const std::string INFO_KEY_ABILITY_TYPE = "ability_type";
static const std::string INFO_KEY_MATCHING_SKILLS = "matching_skills";
static const std::string INFO_KEY_PERMISSION = "permission";
static const std::string INFO_KEY_META_DATA = "extended";

static const std::string INFO_ABILITY_TYPE_HOST_CARD_EMULATION = "host_card_emulation";
static const std::string INFO_ABILITY_TYPE_OFFHOST_CARD_EMULATION = "offhost_card_emulation";
static const std::string INFO_KEY_AIDSETS = "aid_sets";
static const std::string INFO_KEY_LABEL = "label";
static const std::string INFO_KEY_ICON = "icon";
static const std::string INFO_KEY_UNLOCK_DEVICE = "unlock_device";
static const std::string INFO_KEY_ENVIRONMENT = "environment";

bool CardEmulationServiceInfo::ALessB(const std::shared_ptr<AidSet>& a, const std::shared_ptr<AidSet>& b)
{
    if (!a || !b) {
        return false;
    }
    if (a == b) {
        return false;
    }
    // a<b means a has higher priority than b
    if (a->GetType() == CARDEMULATION_SERVICE_TYPE_SECURE) {
        return true;
    }
    if (b->GetType() == CARDEMULATION_SERVICE_TYPE_SECURE) {
        return false;
    }
    return a->GetType() < b->GetType();
}

std::unique_ptr<OHOS::nfc::sdk::cardemulation::CardEmulationServiceInfoLite> CardEmulationServiceInfo::GetLiteInfo(
    const CardEmulationServiceInfo& info)
{
    auto name = info.GetName();
    return std::make_unique<OHOS::nfc::sdk::cardemulation::CardEmulationServiceInfoLite>(
        name ? *name : OHOS::AppExecFwk::ElementName(),
        info.GetLabel(),
        info.GetIcon(),
        info.GetExecutionEnvironment());
}

void from_json(const nlohmann::json& jsonObject, CardEmulationServiceInfo& info)
{
    info.FromJson(jsonObject);
}

void to_json(nlohmann::json& j, const CardEmulationServiceInfo& info)
{
    info.ToJson(j);
}
void from_json(const nlohmann::json& jsonObject, std::shared_ptr<CardEmulationServiceInfo>& info)
{
    info = std::make_shared<CardEmulationServiceInfo>("");
    info->FromJson(jsonObject);
}
void to_json(nlohmann::json& j, const std::shared_ptr<CardEmulationServiceInfo>& info)
{
    if (info) {
        info->ToJson(j);
    }
}
CardEmulationServiceInfo::CardEmulationServiceInfo(std::string location)
    : CardEmulationServiceInfo(std::move(location), OHOS::AppExecFwk::ElementName())
{
}

nfc::cardemulation::CardEmulationServiceInfo::CardEmulationServiceInfo(std::string location,
                                                                       const OHOS::AppExecFwk::ElementName& name)
    : CardEmulationServiceInfo(std::move(location), name, true)
{
}

CardEmulationServiceInfo::CardEmulationServiceInfo(std::string location,
                                                   const OHOS::AppExecFwk::ElementName& name,
                                                   bool mustUnlock)
    : staticAidsets_(),
    dynamicAidsets_(),
    mu_(),
    execEnv_(std::move(location)),
    mustUnlock_(mustUnlock)
{
    SetName(name);
}

CardEmulationServiceInfo::~CardEmulationServiceInfo()
{
}
bool CardEmulationServiceInfo::AddAidsetToStatic(std::unique_ptr<AidSet> aidset)
{
    if (aidset && !aidset->Empty() && !aidset->GetType().empty()) {
        auto p = std::shared_ptr<AidSet>(std::move(aidset));
        p->SetOwner(shared_from_this());
        staticAidsets_[p->GetType()] = p;
        return true;
    }
    return false;
}

bool CardEmulationServiceInfo::AddAidset(std::unique_ptr<AidSet> aidset)
{
    if (aidset && !aidset->Empty() && !aidset->GetType().empty()) {
        auto p = std::shared_ptr<AidSet>(std::move(aidset));
        p->SetOwner(shared_from_this());
        dynamicAidsets_[p->GetType()] = p;
        return true;
    }
    return false;
}

bool CardEmulationServiceInfo::RemoveAidset(const std::string& type)
{
    auto pos = dynamicAidsets_.find(type);
    if (pos != std::end(dynamicAidsets_)) {
        if (pos->second) {
            pos->second->SetOwner(std::shared_ptr<CardEmulationServiceInfo>());
        }
        dynamicAidsets_.erase(pos);
        return true;
    }
    return false;
}

void CardEmulationServiceInfo::SetName(const OHOS::AppExecFwk::ElementName& name)
{
    elementName_ = std::make_shared<OHOS::AppExecFwk::ElementName>(name);
}

int CardEmulationServiceInfo::SetExecutionEnvironment(std::string location)
{
    DebugLog("l: %s, env: %s, rawEnv: %s", location.c_str(), execEnv_.c_str(), rawExecEnv_.c_str());
    if (IsOnHost(execEnv_)) {
        return ERR_HOST_NO_MODIFICATION_ALLOWED;
    }
    if (IsOnHost(location)) {
        return ERR_HOST_NO_MODIFICATION_ALLOWED;
    }
    execEnv_ = (location.empty()) ? rawExecEnv_ : std::move(location);
    return ERR_OK;
}

std::string CardEmulationServiceInfo::GetExecutionEnvironment() const
{
    if (!execEnv_.empty()) {
        return execEnv_;
    }
    return rawExecEnv_.empty() ? NFC_EE_HOST : rawExecEnv_;
}

std::string CardEmulationServiceInfo::GetLabel() const
{
    return label_;
}

void CardEmulationServiceInfo::SetLabel(std::string label)
{
    label_ = std::move(label);
}

std::string CardEmulationServiceInfo::GetIcon() const
{
    return icon_;
}

void CardEmulationServiceInfo::SetIcon(std::string icon)
{
    icon_ = std::move(icon);
}

std::shared_ptr<OHOS::AppExecFwk::ElementName> CardEmulationServiceInfo::GetName() const
{
    return elementName_;
}

bool CardEmulationServiceInfo::MustUnlock() const
{
    return mustUnlock_;
}

void CardEmulationServiceInfo::SetMustUnlock(bool must)
{
    mustUnlock_ = must;
}

bool CardEmulationServiceInfo::HasType(const std::string& type)
{
    auto aidsets = Merge();
    return aidsets.find(type) != aidsets.end();
}

void CardEmulationServiceInfo::SetApduChannel(const OHOS::sptr<OHOS::IRemoteObject>& ro)
{
    using namespace OHOS::nfc::sdk::cardemulation;
    if (ro) {
        apduChannel_ = OHOS::sptr<OHOS::nfc::sdk::cardemulation::ApduChannelProxy>(
            new OHOS::nfc::sdk::cardemulation::ApduChannelProxy(ro));
        RemoteObjectPool::AddRemoteDeathRecipient(ro, shared_from_this());
    } else {
        apduChannel_ = nullptr;
    }
}

OHOS::sptr<OHOS::nfc::sdk::cardemulation::ApduChannelProxy> CardEmulationServiceInfo::GetApduChannel() const
{
    return apduChannel_;
}

std::vector<std::shared_ptr<AidSet>> CardEmulationServiceInfo::SortAidsets(
    std::function<bool(std::shared_ptr<AidSet>, std::shared_ptr<AidSet>)> less)
{
    std::vector<std::shared_ptr<AidSet>> rv = MergeToVector();
    if (less) {
        std::sort(rv.begin(), rv.end(), less);
    }
    return rv;
}

std::shared_ptr<AidSet> CardEmulationServiceInfo::GetAidsetByType(const std::string& type)
{
    auto pos = dynamicAidsets_.find(type);
    if (pos != std::end(dynamicAidsets_)) {
        return pos->second;
    }
    auto posStatic = staticAidsets_.find(type);
    if (posStatic != std::end(staticAidsets_)) {
        return posStatic->second;
    }

    return std::shared_ptr<AidSet>();
}

void CardEmulationServiceInfo::FromJson(const nlohmann::json& jsonObject)
{
    if (!jsonObject.is_object()) {
        return;
    }
    OHOS::AppExecFwk::ElementName name;
    try {
        name.SetAbilityName(jsonObject.at(INFO_KEY_ABILITY_NAME).get<std::string>());
        SetName(name);
        std::string type;
        jsonObject.at(INFO_KEY_ABILITY_TYPE).get_to(type);
        SetLabel(jsonObject.at(INFO_KEY_LABEL).get<std::string>());
        SetIcon(jsonObject.at(INFO_KEY_ICON).get<std::string>());
        if (jsonObject.contains(INFO_KEY_ENVIRONMENT)) {
            rawExecEnv_ = (jsonObject.at(INFO_KEY_ENVIRONMENT).get<std::string>());
        }
        if (rawExecEnv_.empty()) {
            rawExecEnv_ = NFC_EE_HOST;
        }
        // override execEnv_ by constructor
        execEnv_ = rawExecEnv_;
        if (jsonObject.contains(INFO_KEY_UNLOCK_DEVICE)) {
            SetMustUnlock(jsonObject.at(INFO_KEY_UNLOCK_DEVICE).get<bool>());
        }

        auto aidsets = jsonObject.at(INFO_KEY_AIDSETS).get<std::vector<std::shared_ptr<AidSet>>>();
        std::for_each(aidsets.begin(), aidsets.end(), [this](decltype(aidsets)::reference r) {
            if (r) {
                r->SetOwner(shared_from_this());
                staticAidsets_[r->GetType()] = r;
            }
        });
    } catch (nlohmann::json::out_of_range& err) {
        ErrorLog("catch exception: %s", err.what());
    } catch (nlohmann::json::type_error& err) {
        ErrorLog("catch exception: %s", err.what());
    }
}

static std::string HostOrOffhost(const std::string& env, const std::string& rawEnv)
{
    if (env.empty() && rawEnv.empty()) {
        return INFO_ABILITY_TYPE_HOST_CARD_EMULATION;
    }
    return IsOnHost(env) ? INFO_ABILITY_TYPE_HOST_CARD_EMULATION : INFO_ABILITY_TYPE_OFFHOST_CARD_EMULATION;
}
void CardEmulationServiceInfo::ToJson(nlohmann::json& j) const
{
    std::vector<nlohmann::json> aidsets;
    std::for_each(dynamicAidsets_.cbegin(),
                  dynamicAidsets_.cend(),
                  [&aidsets](CardEmulationServiceInfo::Aidsets_t::const_reference r) {
                      if (r.second) {
                          nlohmann::json jo;
                          if (r.second) {
                              r.second->ToJson(jo);
                              aidsets.emplace_back(std::move(jo));
                          }
                      }
                  });

    j = {{INFO_KEY_ABILITY_NAME, elementName_ ? elementName_->GetAbilityName() : ""},
         {INFO_KEY_ABILITY_TYPE, HostOrOffhost(execEnv_, rawExecEnv_)},
         {INFO_KEY_ENVIRONMENT, execEnv_},
         {INFO_KEY_LABEL, label_},
         {INFO_KEY_ICON, icon_},
         {INFO_KEY_UNLOCK_DEVICE, mustUnlock_},
         {INFO_KEY_AIDSETS, aidsets}};
}
std::string CardEmulationServiceInfo::PrintAllAids()
{
    std::stringstream ss;
    auto merged = Merge();
    ss << "ee: " << execEnv_ << " \n";
    std::for_each(merged.cbegin(), merged.cend(), [&ss](CardEmulationServiceInfo::Aidsets_t::const_reference r) {
        ss << "type: " << r.first << "  aids: [ ";
        if (r.second) {
            auto aids = r.second->GetAllAidRawString();
            std::for_each(aids.begin(), aids.end(), [&ss](std::vector<std::string>::reference aid) {
                ss << aid << " ";
            });
        }
        ss << "]\n";
    });

    return ss.str();
}

std::map<std::string, std::shared_ptr<AidSet>> CardEmulationServiceInfo::Merge() const
{
    Aidsets_t rv = dynamicAidsets_;
    rv.insert(staticAidsets_.begin(), staticAidsets_.end());
    return rv;
}
std::vector<std::shared_ptr<AidSet>> CardEmulationServiceInfo::MergeToVector() const
{
    std::vector<std::shared_ptr<AidSet>> rv;

    Aidsets_t tmp = Merge();
    std::for_each(tmp.begin(), tmp.end(), [&rv](Aidsets_t::reference r) { rv.emplace_back(r.second); });
    return rv;
}
void CardEmulationServiceInfo::Visit(std::function<void(const std::string&, std::shared_ptr<AidSet>)> visitor) const
{
    auto merged = Merge();
    std::for_each(merged.cbegin(), merged.cend(), [&visitor](CardEmulationServiceInfo::Aidsets_t::const_reference r) {
        visitor(r.first, r.second);
    });
}

void CardEmulationServiceInfo::OnRemoteDied(const OHOS::wptr<IRemoteObject>& object)
{
    DebugLog("OnRemoteDied()");
    apduChannel_ = nullptr;
}
}  // namespace OHOS::nfc::cardemulation
