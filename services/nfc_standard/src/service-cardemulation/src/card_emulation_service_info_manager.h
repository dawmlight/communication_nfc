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

#ifndef CARD_EMULATION_SERVICE_MANAGER_H
#define CARD_EMULATION_SERVICE_MANAGER_H
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

#include "card_emulation_service_info.h"
#include "installed_ceservice_getter.h"
#include "iremote_object.h"
#include "refbase.h"

namespace OHOS::AppExecFwk {
class ElementName;
}
namespace OHOS::nfc::cardemulation {
class IAidRoutingManager;
class CardEmulationServiceInfoManager final {
public:
    CardEmulationServiceInfoManager(std::shared_ptr<IAidRoutingManager> ob,
                                    std::shared_ptr<InstalledCeServiceGetter> ceserviceinfo_getter);
    ~CardEmulationServiceInfoManager();
    CardEmulationServiceInfoManager(const CardEmulationServiceInfoManager&) = delete;
    CardEmulationServiceInfoManager& operator=(const CardEmulationServiceInfoManager&) = delete;
    int Init();
    int Deinit();
    std::vector<std::unique_ptr<OHOS::nfc::sdk::cardemulation::CardEmulationServiceInfoLite>> GetServicesByType(
        int userId,
        const std::string& type);
    int RegisterRemoteObject(int userId,
                             const OHOS::AppExecFwk::ElementName& elementName,
                             const OHOS::sptr<OHOS::IRemoteObject>& apduChannel);
    int AddAidSet(int userId,
                  const OHOS::AppExecFwk::ElementName& elementName,
                  const std::string& type,
                  const std::vector<std::string>& aidSet);
    int RemoveAidSet(int userId, const OHOS::AppExecFwk::ElementName& elementName, const std::string& type);
    int MarkOffHostCEService(int userId,
                             const OHOS::AppExecFwk::ElementName& elementName,
                             const std::string& secureElementName);
    int UnmarkOffHostCEService(int userId, const OHOS::AppExecFwk::ElementName& elementName);

    int SetPreferredCEService(int userId, const OHOS::AppExecFwk::ElementName& elementName);
    int UnsetPreferredCEService(int userId);

    int OnUserSwitched(int userId);
    int OnServiceUpdated(int userId);

    std::shared_ptr<CardEmulationServiceInfo> GetUserPreferred(int userId);
    bool IsDefaultCEServiceForAid(int userId, const OHOS::AppExecFwk::ElementName& elementName, const std::string& aid);
    bool IsDefaultCEServiceForType(int userId,
                                   const OHOS::AppExecFwk::ElementName& elementName,
                                   const std::string& type);
    std::vector<std::string> GetAidsForCEService(int userId,
                                                 const OHOS::AppExecFwk::ElementName& elementName,
                                                 const std::string& type);
    OHOS::AppExecFwk::ElementName GetPrimaryServiceForType(int userId, const std::string& type);
    int SetDefaultCEServiceForType(int userId,
                                   const OHOS::AppExecFwk::ElementName& elementName,
                                   const std::string& type);

private:
    std::shared_ptr<CardEmulationServiceInfo> GetOrCreateAppInfo(int userId,
                                                                 const OHOS::AppExecFwk::ElementName& elementName);

    void NotifyCEServiceUpdated(int userId);
    using CEServices_cit_t = std::vector<std::shared_ptr<CardEmulationServiceInfo>>::const_iterator;
    bool IsCEServiceInstalled(int userId, const OHOS::AppExecFwk::ElementName& elementName);

    int ParseDefaultCEServiceForType(int userId);
    int StoreDefaultCEServiceForType(int userId,
                                     const OHOS::AppExecFwk::ElementName& elementName,
                                     const std::string& type);

    int ParseCEAbilityInfo(int userId);
    std::shared_ptr<CardEmulationServiceInfo> GetUserDefaultWithoutLock(int userId);
    std::shared_ptr<CardEmulationServiceInfo> GetUserPreferredWithoutLock(int userId);

    int GetDynamicCEServiceInfo(std::shared_ptr<CardEmulationServiceInfo>& info,
                                const std::vector<std::shared_ptr<CardEmulationServiceInfo>>& dynamicInfos);
    int StoreDynamicCEServiceInfo(int userId);
    int StoreDynamicCEServiceInfoWithoutLock(int userId);
    int GetCurrentUserId() const;
    int FetchServiceInfosByType(std::vector<std::shared_ptr<CardEmulationServiceInfo>>& result,
                                int userId,
                                const std::string& type);
    int FetchServiceInfoByName(std::shared_ptr<CardEmulationServiceInfo>& result,
                               int userId,
                               const OHOS::AppExecFwk::ElementName& elementName);
    bool ConfilictsWithDefault(int userId, const std::shared_ptr<CardEmulationServiceInfo>& abilityInfo);

private:
    /* data */
    using UserCEServices_t = std::map<int, std::vector<std::shared_ptr<CardEmulationServiceInfo>>>;
    UserCEServices_t userCEServices_;
    std::map<int, std::weak_ptr<CardEmulationServiceInfo>> userPreferredCEServices_;
    std::map<int, std::weak_ptr<CardEmulationServiceInfo>> userDefaultCEService_;
    std::mutex mu_;
    std::shared_ptr<IAidRoutingManager> routingManager_;
    std::shared_ptr<InstalledCeServiceGetter> serviceinfoGetter_;
    int lastUserId_;
};
}  // namespace OHOS::nfc::cardemulation
#endif  // CARD_EMULATION_SERVICE_MANAGER_H
