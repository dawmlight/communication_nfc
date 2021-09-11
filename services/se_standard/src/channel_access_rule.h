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
#ifndef CHANNEL_ACCESS_H
#define CHANNEL_ACCESS_H

#include <memory>
#include <string>
#include <vector>

namespace OHOS::se::security {
class ChannelAccessRule {
public:
    enum ACCESSRULE { ALWAYS, NEVER, UNKNOWN };
    ChannelAccessRule();
    ~ChannelAccessRule();
    ChannelAccessRule::ACCESSRULE GetAccessRule();
    void SetAccessRule(ACCESSRULE accessRule, const std::string& reason);
    std::string GetReason();
    ChannelAccessRule::ACCESSRULE GetApduAccessRule();
    void SetApduAccessRule(ACCESSRULE apduAccess);
    bool HasApduFilter();
    void SetHasApduFilter(bool hasApduFilter);
    std::vector<std::string> GetApduFilters();
    void SetApduFilters(std::vector<std::string> apduFilters);
    ChannelAccessRule::ACCESSRULE GetNFCEventAccessRule();
    void SetNFCEventAccessRule(ACCESSRULE nfcAccessRule);
    std::string GetBundleName();
    void SetBundleName(const std::string& bundleName);
    int GetCallingPid();
    void SetCallingPid(int callingPid);
    static std::shared_ptr<ChannelAccessRule> GetPrivilegeAccessRule(const std::string& bundleName, int callingPid);
    static std::shared_ptr<ChannelAccessRule> GetCarrierPrivilegeAccessRule(const std::string& bundleName, int callingPid);
    ChannelAccessRule::ACCESSRULE GetPrivilegeAccessRule();
    void SetPrivilegeAccessRule(ACCESSRULE privilegeAccessRule);
    void SetCarrierPrivilegeAccessRule(const std::string& bundleName, int pid);

private:
    ACCESSRULE accessRule_;
    std::string reason_;
    ACCESSRULE apduAccessRule_;
    bool hasApduFilter_;
    std::vector<std::string> apduFilters_;
    ACCESSRULE nfcAccessRule_;
    std::string bundleName_;
    int callingPid_;
    ACCESSRULE privilegeAccessRule_;
};
}  // namespace OHOS::se::security
#endif /* CHANNEL_ACCESS_H */