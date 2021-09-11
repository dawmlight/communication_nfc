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
#ifndef ACCESS_CONTROL_ENFORCER_H
#define ACCESS_CONTROL_ENFORCER_H

#include <memory>
#include <mutex>
#include <string>
#include <vector>

namespace osal {
class BundleInfo;
class BundleManager;
}  // namespace osal

namespace OHOS {
namespace se {
class SeChannel;
class Terminal;
namespace security {
class AccessRuleCache;
class ChannelAccessRule;
class AccessRuleApplicationController;
class AccessRuleFilesController;
/*
 * Software that is part of the Secure Element access API, it obtains access
 * rules from the Secure Element and applies those rules to restrict device
 * application access to the various Secure Element applications.
 */
class AccessControlEnforcer {
public:
    AccessControlEnforcer(std::weak_ptr<Terminal> terminal);
    ~AccessControlEnforcer();
    std::string GetDefaultAccessControlAid();
    std::weak_ptr<osal::BundleManager> GetBundleManager();
    void SetBundleManager(std::weak_ptr<osal::BundleManager> bundleManager);
    std::weak_ptr<Terminal> GetTerminal();
    std::weak_ptr<AccessRuleCache> GetAccessRuleCache();
    void Reset();
    void Initialize();
    bool IsNoRuleFound();
    void CheckCommand(std::weak_ptr<OHOS::se::SeChannel> channel, const std::string& command);
    std::shared_ptr<ChannelAccessRule> EstablishChannelAccessRuleForBundle(const std::string& aid,
                                                                           const std::string& bundleName,
                                                                           bool checkRefreshTag);
    std::shared_ptr<ChannelAccessRule> GetAccessRule(const std::string& aid, std::vector<std::string> appCertHashes);
    std::vector<bool> IsNfcEventAllowed(const std::string& aid, const std::vector<std::string>& bundleNames);
    bool CheckCarrierPrivilege(std::weak_ptr<osal::BundleInfo> pInfo, bool checkRefreshTag);

private:
    std::mutex mutex_;
    std::weak_ptr<Terminal> terminal_;
    std::weak_ptr<osal::BundleManager> bundleManager_;
    std::shared_ptr<AccessRuleApplicationController> accessRuleApplicationController_;
    std::shared_ptr<AccessRuleFilesController> accessRuleFilesController_;
    std::shared_ptr<AccessRuleCache> accessRuleCache_;
    bool noRuleFound_;
    bool hasAra_;
    bool hasArf_;
    bool rulesRead_;
    std::vector<std::string> GetHashesFromBundle(const std::string& bundleName);
    void CheckRefreshTag();
    static std::string UnsignedCharArrayToString(const unsigned char* charArray, int length);
};
}  // namespace security
}  // namespace se
}  // namespace OHOS
#endif  // !ACCESS_CONTROL_ENFORCER_H
