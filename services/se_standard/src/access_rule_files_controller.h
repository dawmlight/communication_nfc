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
#ifndef ACCESS_RULE_FILES_CONTROLLER_H
#define ACCESS_RULE_FILES_CONTROLLER_H

#include <memory>
#include <string>

namespace OHOS::se {
class Terminal;
}  // namespace OHOS::se

namespace OHOS::se::security {
static const std::string GPAC_AID = {(char)0xA0, 0x00, 0x00, 0x00, 0x18, 0x47, 0x50, 0x41, 0x43, 0x2D, 0x31, 0x35};
static const std::string PKCS15_AID = {(char)0xA0, 0x00, 0x00, 0x00, 0x63, 0x50, 0x4B, 0x43, 0x53, 0x2D, 0x31, 0x35};

class AccessControlMain;
class AccessControlRules;
class AccessRuleCache;
class AccessRuleFilesChannel;
class AccessRuleFilesController {
public:
    AccessRuleFilesController(std::weak_ptr<AccessRuleCache> accessRuleCache,
                              std::weak_ptr<OHOS::se::Terminal> terminal);
    ~AccessRuleFilesController();
    void Initialize();

private:
    std::weak_ptr<AccessRuleCache> accessRuleCache_;
    std::weak_ptr<OHOS::se::Terminal> terminal_;
    std::shared_ptr<AccessRuleFilesChannel> arfChannel_;
    std::shared_ptr<AccessControlMain> accessControlMain_;
    std::shared_ptr<AccessControlRules> accessControlRule_;
    std::string accessControlMainPath_;
    std::string pkcs15Path_;
    bool acmfFound_;
    void GetAcmf();
    bool SelectAccessControlRules(std::string aid);
    bool UpdateAccessControlRules();
};
}  // namespace OHOS::se::security
#endif /* ACCESS_RULE_FILES_CONTROLLER_H */
