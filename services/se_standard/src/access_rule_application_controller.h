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
#ifndef ACCESS_RULE_APPLICATION_CONTROLLER_H
#define ACCESS_RULE_APPLICATION_CONTROLLER_H

#include <memory>
#include <string>
#include <vector>

namespace OHOS::se {
class SeChannel;
class Terminal;
}  // namespace OHOS::se

namespace OHOS::se::security {
static const std::string ARA_M_AID = {(char)0xA0, 0x00, 0x00, 0x01, 0x51, 0x41, 0x43, 0x4C, 0x00};
// CLA INS P1 P2 LE
static const std::string GET_DATA_REFRESH_TAG = {(char)0x80, (char)0xCA, (char)0xDF, (char)0x20, (char)0x00};
static const std::string GET_DATA_ALL = {(char)0x80, (char)0xCA, (char)0xFF, (char)0x40, (char)0x00};
static const std::string GET_DATA_NEXT = {(char)0x80, (char)0xCA, (char)0xFF, (char)0x60, (char)0x00};
class AccessRuleCache;
class RefArDo;
class AccessRuleApplicationController {
public:
    AccessRuleApplicationController(std::weak_ptr<AccessRuleCache> accessRuleCache,
                                    std::weak_ptr<OHOS::se::Terminal> terminal);
    ~AccessRuleApplicationController();
    void Initialize();
    std::string GetAraMAid();
    std::string GetAccessControlAid();
    std::vector<RefArDo> GetAllAccessRules(std::shared_ptr<OHOS::se::SeChannel> channel);
    std::string GetRefreshTag(std::shared_ptr<OHOS::se::SeChannel> channel);

private:
    std::weak_ptr<AccessRuleCache> accessRuleCache_;
    std::weak_ptr<OHOS::se::Terminal> terminal_;
    std::string accessControlAid_;
    std::string GetDataRefreshTag(std::shared_ptr<OHOS::se::SeChannel> channel);
    std::string GetDataAll(std::shared_ptr<OHOS::se::SeChannel> channel);
    std::string GetDataNext(std::shared_ptr<OHOS::se::SeChannel> channel);
    bool ResponseIsSuccess(std::string response);
};
}  // namespace OHOS::se::security
#endif /* ACCESS_RULE_APPLICATION_CONTROLLER_H */
