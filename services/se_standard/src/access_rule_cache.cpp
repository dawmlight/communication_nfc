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
#include "access_rule_cache.h"

#include "channel_access_rule.h"
#include "general-data-objects/aid_ref_do.h"
#include "general-data-objects/apdu_ar_do.h"
#include "general-data-objects/ar_do.h"
#include "general-data-objects/hash_ref_do.h"
#include "general-data-objects/nfc_ar_do.h"
#include "general-data-objects/pkg_ref_do.h"
#include "general-data-objects/ref_do.h"
#include "loghelper.h"

namespace OHOS::se::security {
AccessRuleCache::AccessRuleCache() : refreshTag_(""), accessRuleMap_({}), carrierPrivilegeCache_({}) {}

AccessRuleCache::~AccessRuleCache()
{
    accessRuleMap_.clear();
    carrierPrivilegeCache_.clear();
}

bool AccessRuleCache::CompareRefreshTag(const std::string& refreshTag)
{
    DebugLog("AccessRuleCache::CompareRefreshTag");
    if (refreshTag_.empty() || refreshTag.empty()) {
        return false;
    }
    return (refreshTag_ == refreshTag);
}

void AccessRuleCache::SetRefreshTag(const std::string& refreshTag)
{
    DebugLog("AccessRuleCache::SetRefreshTag");
    refreshTag_ = refreshTag;
}

void AccessRuleCache::ClearAccessRules()
{
    DebugLog("AccessRuleCache::ClearAccessRules");
    accessRuleMap_.clear();
    carrierPrivilegeCache_.clear();
}

void AccessRuleCache::AddAccessRule(std::shared_ptr<RefDo> refDo, ChannelAccessRule channelAccessRule)
{
    DebugLog("AccessRuleCache::AddAccessRule");
    if (CARRIER_PRIVILEGE_AID == refDo->GetAidRefDo()->GetAid()) {
        carrierPrivilegeCache_.push_back(*refDo);
        return;
    }
    std::map<RefDo, ChannelAccessRule>::iterator it = accessRuleMap_.find(*refDo);
    if (it != accessRuleMap_.end()) {
        DebugLog("Already have, compare two rules");
        ChannelAccessRule arNew = ChannelAccessRule();
        ChannelAccessRule arOri = it->second;

        if (arOri.GetAccessRule() == ChannelAccessRule::ACCESSRULE::NEVER) {
            arNew.SetAccessRule(ChannelAccessRule::ACCESSRULE::NEVER, arOri.GetReason());
        } else if (channelAccessRule.GetAccessRule() == ChannelAccessRule::ACCESSRULE::NEVER) {
            arNew.SetAccessRule(ChannelAccessRule::ACCESSRULE::NEVER, channelAccessRule.GetReason());
        } else if (arOri.GetAccessRule() == ChannelAccessRule::ACCESSRULE::ALWAYS ||
                   channelAccessRule.GetAccessRule() == ChannelAccessRule::ACCESSRULE::ALWAYS) {
            arNew.SetAccessRule(ChannelAccessRule::ACCESSRULE::ALWAYS, "");
        }
        // NEVER (APDU) > APDU filter > ALWAYS (APDU)
        if (channelAccessRule.GetApduAccessRule() == ChannelAccessRule::ACCESSRULE::NEVER ||
            arOri.GetApduAccessRule() == ChannelAccessRule::ACCESSRULE::NEVER) {
            arNew.SetApduAccessRule(ChannelAccessRule::ACCESSRULE::NEVER);
        } else if (channelAccessRule.HasApduFilter() || arOri.HasApduFilter()) {
            // If aggregated access rules contain APDU filters, then these shall be combined per OR operation.
            arNew.SetHasApduFilter(true);
            std::vector<std::string> apduFiltersNew = channelAccessRule.GetApduFilters();
            for (std::string apduFilter : arOri.GetApduFilters()) {
                apduFiltersNew.push_back(apduFilter);
            }
            arNew.SetApduFilters(apduFiltersNew);
        } else if (channelAccessRule.GetApduAccessRule() == ChannelAccessRule::ACCESSRULE::ALWAYS ||
                   arOri.GetApduAccessRule() == ChannelAccessRule::ACCESSRULE::ALWAYS) {
            arNew.SetApduAccessRule(ChannelAccessRule::ACCESSRULE::ALWAYS);
            arNew.SetHasApduFilter(false);
            arNew.SetApduFilters({});
        }
        // NEVER (NFC) > ALWAYS (NFC)
        if (channelAccessRule.GetNFCEventAccessRule() == ChannelAccessRule::ACCESSRULE::NEVER ||
            arOri.GetNFCEventAccessRule() == ChannelAccessRule::ACCESSRULE::NEVER) {
            arNew.SetNFCEventAccessRule(ChannelAccessRule::ACCESSRULE::NEVER);
        } else if (channelAccessRule.GetNFCEventAccessRule() == ChannelAccessRule::ACCESSRULE::ALWAYS ||
                   arOri.GetNFCEventAccessRule() == ChannelAccessRule::ACCESSRULE::ALWAYS) {
            arNew.SetNFCEventAccessRule(ChannelAccessRule::ACCESSRULE::ALWAYS);
        }
        accessRuleMap_.erase(it);
        accessRuleMap_.insert(std::pair<RefDo, ChannelAccessRule>(*refDo, arNew));
    } else {
        DebugLog("Add a new one");
        accessRuleMap_.insert(std::pair<RefDo, ChannelAccessRule>(*refDo, channelAccessRule));
    }
}

void AccessRuleCache::AddAccessRule(std::shared_ptr<RefDo> refDo, std::shared_ptr<ArDo> arDo)
{
    DebugLog("AccessRuleCache::AddAccessRule");
    if (CARRIER_PRIVILEGE_AID == refDo->GetAidRefDo()->GetAid()) {
        carrierPrivilegeCache_.push_back(*refDo);
        return;
    }
    ChannelAccessRule channelAccessRule = ChannelAccessRule();
    std::shared_ptr<ApduArDo> apduArDo = arDo->GetApduArDo();
    if (!apduArDo) {
        channelAccessRule.SetApduAccessRule(ChannelAccessRule::ACCESSRULE::UNKNOWN);
    } else if (apduArDo->IsApduAllowed()) {
        channelAccessRule.SetAccessRule(ChannelAccessRule::ACCESSRULE::ALWAYS, "");
        if (apduArDo->GetApduFilters().size() > 0) {
            channelAccessRule.SetHasApduFilter(true);
            channelAccessRule.SetApduFilters(apduArDo->GetApduFilters());
        } else {
            channelAccessRule.SetApduAccessRule(ChannelAccessRule::ACCESSRULE::ALWAYS);
        }
    } else {
        channelAccessRule.SetAccessRule(ChannelAccessRule::ACCESSRULE::NEVER, "");
        channelAccessRule.SetApduAccessRule(ChannelAccessRule::ACCESSRULE::NEVER);
    }
    std::shared_ptr<NfcArDo> nfcArDo = arDo->GetNfcArDo();
    if (!nfcArDo) {
        channelAccessRule.SetNFCEventAccessRule(ChannelAccessRule::ACCESSRULE::UNKNOWN);
    } else if (nfcArDo->IsNfcAllowed()) {
        channelAccessRule.SetNFCEventAccessRule(ChannelAccessRule::ACCESSRULE::ALWAYS);
    } else {
        channelAccessRule.SetNFCEventAccessRule(ChannelAccessRule::ACCESSRULE::NEVER);
    }

    this->AddAccessRule(refDo, channelAccessRule);
}

ChannelAccessRule AccessRuleCache::GetAccessRule(const std::string& aid, std::vector<std::string> hashes)
{
    DebugLog("AccessRuleCache::GetAccessRule");
    ChannelAccessRule channelAccessRule = this->GetAccessRuleWithUnknown(aid, hashes);
    if (channelAccessRule.GetApduAccessRule() == ChannelAccessRule::ACCESSRULE::UNKNOWN &&
        !channelAccessRule.HasApduFilter()) {
        DebugLog("Missing apdu rules");
        channelAccessRule.SetAccessRule(ChannelAccessRule::ACCESSRULE::NEVER, "");
        channelAccessRule.SetApduAccessRule(ChannelAccessRule::ACCESSRULE::NEVER);
    }
    // For NFC transaction events, if no rule explicitly specifies NFC permissions, permission shall be granted
    // based on APDU channel rules. A device application authorized to establish an APDU channel with a secure
    // element application is implicitly authorized to receive NFC events from this application.
    if (channelAccessRule.GetNFCEventAccessRule() == ChannelAccessRule::ACCESSRULE::UNKNOWN) {
        DebugLog("Missing nfc rules");
        if (channelAccessRule.HasApduFilter()) {
            channelAccessRule.SetNFCEventAccessRule(ChannelAccessRule::ACCESSRULE::ALWAYS);
        } else {
            channelAccessRule.SetNFCEventAccessRule(channelAccessRule.GetApduAccessRule());
        }
    }
    return channelAccessRule;
}

bool AccessRuleCache::CheckCarrierPrivilege(std::string bundleName, std::vector<std::string> hashes)
{
    if (bundleName.empty() || hashes.empty()) {
        return false;
    }
    for (RefDo refDo : carrierPrivilegeCache_) {
        for (std::string hash : hashes) {
            if ((hash == refDo.GetHashRefDo()->GetHash()) && (bundleName == refDo.GetPkgRefDo()->GetName())) {
                return true;
            }
        }
    }
    return false;
}

ChannelAccessRule AccessRuleCache::GetAccessRuleWithUnknown(const std::string& aid, std::vector<std::string> hashes)
{
    DebugLog("AccessRuleCache::GetAccessRule");
    // Secure Element Access Control – Public Release v1.0, 4.2.3 Algorithm for Applying Rules
    std::shared_ptr<AidRefDo> aidRefDo;
    std::shared_ptr<HashRefDo> hashRefDo;
    std::shared_ptr<RefDo> refDo;
    std::map<RefDo, ChannelAccessRule>::iterator iter;
    if (hashes.size() == 0) {
        DebugLog("Application certificates do not exist.");
        return ChannelAccessRule();
    }
    for (std::string hash : hashes) {
        aidRefDo = AidRefDo::AidToAidRefDo(aid);
        hashRefDo = std::make_shared<HashRefDo>(hash);
        refDo = std::make_shared<RefDo>(aidRefDo, hashRefDo);
        iter = accessRuleMap_.find(*refDo);
        if (iter != accessRuleMap_.end()) {
            DebugLog("Find the access rule by the given aid and hash");
            return iter->second;
        }
    }
    DebugLog("No rule found by the given aid and the given hash");

    for (iter = accessRuleMap_.begin(); iter != accessRuleMap_.end(); iter++) {
        RefDo arRefDo = iter->first;
        std::string arAid = arRefDo.GetAidRefDo()->GetAid();
        std::string arHash = arRefDo.GetHashRefDo()->GetHash();
        if (aid == arAid && arHash.length() > 0) {
            DebugLog("Aid has a specific rule with different hash, denied access");
            ChannelAccessRule channelAccessRule = ChannelAccessRule();
            channelAccessRule.SetAccessRule(ChannelAccessRule::ACCESSRULE::NEVER, "Specific rules have priority.");
            channelAccessRule.SetApduAccessRule(ChannelAccessRule::ACCESSRULE::NEVER);
            channelAccessRule.SetNFCEventAccessRule(ChannelAccessRule::ACCESSRULE::NEVER);
            return channelAccessRule;
        }
    }

    {
        aidRefDo = AidRefDo::AidToAidRefDo(aid);
        hashRefDo = std::make_shared<HashRefDo>("");
        refDo = std::make_shared<RefDo>(aidRefDo, hashRefDo);
        iter = accessRuleMap_.find(*refDo);
        if (iter != accessRuleMap_.end()) {
            DebugLog("Find the access rule by the given aid and the empty hash");
            return iter->second;
        }
    }
    DebugLog("No rule found by the given aid");

    for (std::string hash : hashes) {
        aidRefDo = std::make_shared<AidRefDo>(AidRefDo::AID_REF_DO_TAG, EMPTY_AID);
        hashRefDo = std::make_shared<HashRefDo>(hash);
        refDo = std::make_shared<RefDo>(aidRefDo, hashRefDo);
        iter = accessRuleMap_.find(*refDo);
        if (iter != accessRuleMap_.end()) {
            DebugLog("Find the access rule by the empty aid and the given hash");
            return iter->second;
        }
    }
    DebugLog("No rule found by the given hash");

    for (iter = accessRuleMap_.begin(); iter != accessRuleMap_.end(); iter++) {
        RefDo arRefDo = iter->first;
        std::string arAid = arRefDo.GetAidRefDo()->GetAid();
        std::string arHash = arRefDo.GetHashRefDo()->GetHash();
        if (arAid.empty() && arHash.length() > 0) {
            DebugLog("Empty aid has a specific rule with different hash, denied access");
            ChannelAccessRule channelAccessRule = ChannelAccessRule();
            channelAccessRule.SetAccessRule(ChannelAccessRule::ACCESSRULE::NEVER, "Specific rules have priority.");
            channelAccessRule.SetApduAccessRule(ChannelAccessRule::ACCESSRULE::NEVER);
            channelAccessRule.SetNFCEventAccessRule(ChannelAccessRule::ACCESSRULE::NEVER);
            return channelAccessRule;
        }
    }

    {
        aidRefDo = std::make_shared<AidRefDo>(AidRefDo::AID_REF_DO_TAG, EMPTY_AID);
        hashRefDo = std::make_shared<HashRefDo>("");
        refDo = std::make_shared<RefDo>(aidRefDo, hashRefDo);
        iter = accessRuleMap_.find(*refDo);
        if (iter != accessRuleMap_.end()) {
            DebugLog("Find the access rule by the empty aid and the empty hash");
            return iter->second;
        }
    }
    DebugLog("No rule found");
    return ChannelAccessRule();
}
}  // namespace OHOS::se::security