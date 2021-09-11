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
#ifndef ACCESS_RULE_CACHE_H
#define ACCESS_RULE_CACHE_H

#include <map>
#include <memory>
#include <string>
#include <vector>

namespace OHOS::se::security {
class ChannelAccessRule;
class ArDo;
class RefDo;
static const std::string EMPTY_AID = "";
static const std::string CARRIER_PRIVILEGE_AID = {
    (char)0xFF, (char)0xFF, (char)0xFF, (char)0xFF, (char)0xFF, (char)0xFF};
class AccessRuleCache {
public:
    AccessRuleCache();
    ~AccessRuleCache();
    bool CompareRefreshTag(const std::string& refreshTag);
    void SetRefreshTag(const std::string& refreshTag);
    void ClearAccessRules();
    void AddAccessRule(std::shared_ptr<RefDo> refDo, ChannelAccessRule channelAccess);
    void AddAccessRule(std::shared_ptr<RefDo> refDo, std::shared_ptr<ArDo> arDo);
    ChannelAccessRule GetAccessRule(const std::string& aid, std::vector<std::string> hashes);
    bool CheckCarrierPrivilege(std::string bundleName, std::vector<std::string> hashes);

private:
    ChannelAccessRule GetAccessRuleWithUnknown(const std::string& aid, std::vector<std::string> hashes);
    std::string refreshTag_;
    std::map<RefDo, ChannelAccessRule> accessRuleMap_;
    std::vector<RefDo> carrierPrivilegeCache_;
};
}  // namespace OHOS::se::security
#endif /* ACCESS_RULE_CACHE_H */
