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

#ifndef CARD_EMULATION_SERVICE_H
#define CARD_EMULATION_SERVICE_H

#include <atomic>
#include <memory>
#include <string>
#include <vector>

#include "icard_emulation_service.h"

namespace OHOS {
namespace nfc {
namespace cardemulation {
class AidRoutingAdapter;
class AidRoutingPlanner;
class CardEmulationServiceInfoManager;
class HostCardEmulation;
class CardEmulationEventHandler;
class ICardEmulationDeviceHost;

class CardEmulationService : public ICardEmulationService {
public:
    CardEmulationService(CardEmulationService const&) = delete;
    CardEmulationService& operator=(CardEmulationService const&) = delete;
    explicit CardEmulationService(std::shared_ptr<ICardEmulationDeviceHost> nci);
    virtual ~CardEmulationService();

    int Init() override;
    int Deinit() override;
    int OnUserSwitched(int userId) override;
    int RegisterRemoteObject(int userId,
                             const OHOS::AppExecFwk::ElementName& serviceName,
                             const OHOS::sptr<OHOS::IRemoteObject>& apduChannel) override;
    int AddAidsForService(int userId,
                          const OHOS::AppExecFwk::ElementName& elementName,
                          const std::string& type,
                          const std::vector<std::string>& aidSet) override;

    int RemoveAidsForService(int userId,
                             const OHOS::AppExecFwk::ElementName& elementName,
                             const std::string& type) override;
    int MarkOffHostForService(int userId,
                              const OHOS::AppExecFwk::ElementName& elementName,
                              const std::string& secureElementName) override;
    int UnmarkOffHostForService(int userId, const OHOS::AppExecFwk::ElementName& elementName) override;
    bool SupportsAidPrefix(void) const override;
    int SetPreferredService(int userId, const OHOS::AppExecFwk::ElementName& elementName) override;
    int UnsetPreferredService(int userId) override;
    int OnHCEActivated() override;
    int OnHCEData(const unsigned char* data, size_t len) override;
    int OnHCEDeactivated() override;
    int OnOffHostTransaction(void) override;
    bool IsDefaultServiceForAid(int userId,
                                const OHOS::AppExecFwk::ElementName& elementName,
                                const std::string& aid) const override;
    bool IsDefaultServiceForType(int userId,
                                 const OHOS::AppExecFwk::ElementName& elementName,
                                 const std::string& type) const override;
    std::vector<std::string> GetAidsForService(int userId,
                                               const OHOS::AppExecFwk::ElementName& elementName,
                                               const std::string& type) const override;
    OHOS::AppExecFwk::ElementName GetPrimaryServiceForType(int userId, const std::string& type) override;
    std::string GetDescriptionForPreferredService(int userId, std::string type) override;
    int SetPrimaryServiceForType(int userId,
                                 const OHOS::AppExecFwk::ElementName& elementName,
                                 const std::string& type) override;
    void OnNfcEnabled() override;
    void OnNfcDisabled() override;
    void OnSecureNfcToggled() override;

    std::vector<std::unique_ptr<OHOS::nfc::sdk::cardemulation::CardEmulationServiceInfoLite>> GetServicesByType(
        int userId,
        const std::string& type) override;

protected:
    bool IsInited() const;

private:
    std::shared_ptr<AidRoutingAdapter> aidRoutingAdapter_;
    std::shared_ptr<AidRoutingPlanner> aidRoutingPlanner_{};
    std::shared_ptr<CardEmulationServiceInfoManager> serviceInfoManager_{};
    std::shared_ptr<CardEmulationEventHandler> eventHandler_{};
    std::atomic_bool inited_{false};
    std::shared_ptr<ICardEmulationDeviceHost> ceDeviceHost_;
};
}  // namespace cardemulation
}  // namespace nfc
}  // namespace OHOS
#endif  // CARD_EMULATION_SERVICE_H
