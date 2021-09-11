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
#include "channel_access_rule.h"

namespace OHOS::se::security {
ChannelAccessRule::ChannelAccessRule()
    : accessRule_(UNKNOWN),
      reason_("no access by default"),
      apduAccessRule_(UNKNOWN),
      hasApduFilter_(false),
      apduFilters_({}),
      nfcAccessRule_(UNKNOWN),
      bundleName_(""),
      callingPid_(0),
      privilegeAccessRule_(UNKNOWN)
{
}

ChannelAccessRule::~ChannelAccessRule() {}

ChannelAccessRule::ACCESSRULE ChannelAccessRule::GetAccessRule()
{
    return accessRule_;
}

void ChannelAccessRule::SetAccessRule(ACCESSRULE apduAccessRule, const std::string& reason)
{
    accessRule_ = apduAccessRule;
    reason_ = reason;
}

std::string ChannelAccessRule::GetReason()
{
    return reason_;
}

ChannelAccessRule::ACCESSRULE ChannelAccessRule::GetApduAccessRule()
{
    return apduAccessRule_;
}

void ChannelAccessRule::SetApduAccessRule(ACCESSRULE apduAccessRule)
{
    apduAccessRule_ = apduAccessRule;
}

bool ChannelAccessRule::HasApduFilter()
{
    return hasApduFilter_;
}

void ChannelAccessRule::SetHasApduFilter(bool useApduFilter)
{
    hasApduFilter_ = useApduFilter;
}

std::vector<std::string> ChannelAccessRule::GetApduFilters()
{
    return apduFilters_;
}

void ChannelAccessRule::SetApduFilters(std::vector<std::string> apduFilters)
{
    apduFilters_ = apduFilters;
}

ChannelAccessRule::ACCESSRULE ChannelAccessRule::GetNFCEventAccessRule()
{
    return nfcAccessRule_;
}

void ChannelAccessRule::SetNFCEventAccessRule(ACCESSRULE nfcAccessRule)
{
    nfcAccessRule_ = nfcAccessRule;
}

std::string ChannelAccessRule::GetBundleName()
{
    return bundleName_;
}

void ChannelAccessRule::SetBundleName(const std::string& bundleName)
{
    bundleName_ = bundleName;
}

int ChannelAccessRule::GetCallingPid()
{
    return callingPid_;
}

void ChannelAccessRule::SetCallingPid(int callingPid)
{
    callingPid_ = callingPid;
}

std::shared_ptr<ChannelAccessRule> ChannelAccessRule::GetPrivilegeAccessRule(const std::string& bundleName, int callingPid)
{
    std::shared_ptr<ChannelAccessRule> channelAccessRule = std::make_shared<ChannelAccessRule>();
    channelAccessRule->SetBundleName(bundleName);
    channelAccessRule->SetCallingPid(callingPid);
    channelAccessRule->SetAccessRule(ALWAYS, "privilege application");
    channelAccessRule->SetApduAccessRule(ALWAYS);
    channelAccessRule->SetNFCEventAccessRule(ALWAYS);
    channelAccessRule->SetPrivilegeAccessRule(ALWAYS);
    return channelAccessRule;
}

std::shared_ptr<ChannelAccessRule> ChannelAccessRule::GetCarrierPrivilegeAccessRule(const std::string& bundleName, int callingPid)
{
    std::shared_ptr<ChannelAccessRule> channelAccessRule = std::make_shared<ChannelAccessRule>();
    channelAccessRule->SetBundleName(bundleName);
    channelAccessRule->SetCallingPid(callingPid);
    channelAccessRule->SetAccessRule(ALWAYS, "Carrier-Privilege");
    channelAccessRule->SetApduAccessRule(ALWAYS);
    channelAccessRule->SetPrivilegeAccessRule(ALWAYS);
    return channelAccessRule;
}

ChannelAccessRule::ACCESSRULE ChannelAccessRule::GetPrivilegeAccessRule()
{
    return privilegeAccessRule_;
}

void ChannelAccessRule::SetPrivilegeAccessRule(ACCESSRULE privilegeAccessRule)
{
    privilegeAccessRule_ = privilegeAccessRule;
}

void ChannelAccessRule::SetCarrierPrivilegeAccessRule(const std::string& bundleName, int pid)
{
    bundleName_ = bundleName;
    callingPid_ = pid;
    accessRule_ = ALWAYS;
    apduAccessRule_ = ALWAYS;
    privilegeAccessRule_ = ALWAYS;
    reason_ = "Carrier-Privilege";
}
}  // namespace OHOS::se::security