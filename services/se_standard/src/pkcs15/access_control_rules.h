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
#ifndef ACCESS_CONTROL_RULES_H
#define ACCESS_CONTROL_RULES_H

#include <map>
#include <memory>
#include <string>

namespace OHOS::se::security {
class AccessRuleCache;
class AccessRuleFilesChannel;
/*
 * There shall be only one ACRF file per Secure Element. If a Secure Element contains several ACRF files,
 * then the security shall be considered compromised and the Access Control enforcer shall forbid access to all
 * the Secure Element applications for, and only for, this specific Secure Element.
 * Each Rule object explicitly or implicitly identifies a set of Secure Element applications, and refers to the
 * Access Control Conditions File that describes how these applications can be accessed.
 */
class AccessControlRules {
public:
    AccessControlRules(std::weak_ptr<AccessRuleFilesChannel> arfChannel,
                       std::weak_ptr<AccessRuleCache> accessRuleCache);
    ~AccessControlRules();
    void ReadDataFromPath(std::string path);
    void AnalyzeData(std::string data);

private:
    std::weak_ptr<AccessRuleFilesChannel> arfChannel_;
    std::weak_ptr<AccessRuleCache> accessRuleCache_;
    std::map<std::string, std::string> accfCache_;
};
}  // namespace OHOS::se::security
#endif  // !ACCESS_CONTROL_RULES_H
