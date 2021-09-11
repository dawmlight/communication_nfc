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

#ifndef AID_ROUTING_PLANNER_H
#define AID_ROUTING_PLANNER_H

#include <memory>
#include <mutex>
#include <vector>

#include "iaid_routing_manager.h"

namespace OHOS::nfc::cardemulation {
class AidRoutingAdapter;
class CardEmulationServiceInfo;
class AidSet;
class IAidRoutingPolicy;
class AidRoutingPlanner final : public IAidRoutingManager {
public:
    AidRoutingPlanner(std::unique_ptr<IAidRoutingPolicy> routingStrategy,
                      const std::shared_ptr<AidRoutingAdapter>& routingController);
    int Init();
    int OnCeServiceChanged(const std::vector<std::shared_ptr<CardEmulationServiceInfo>>& infos,
                           const std::shared_ptr<CardEmulationServiceInfo>& primary,
                           const std::shared_ptr<CardEmulationServiceInfo>& preferred) override;

    std::vector<ServiceInfoTypePair> GetCardEmulationServicesByAid(const std::string& aid) override;

private:
    int AddDefaultRouting(const std::shared_ptr<CardEmulationServiceInfo>& primary,
                          std::vector<std::shared_ptr<nfc::cardemulation::AidSet>>& routingTable);

private:
    std::unique_ptr<IAidRoutingPolicy> routingPolicy_;
    std::shared_ptr<AidRoutingAdapter> routingController_;
    using aid_table_t = std::vector<std::shared_ptr<AidSet>>;
    aid_table_t aidTable_;
    std::mutex mu_;
};
}  // namespace OHOS::nfc::cardemulation
#endif  // AID_ROUTING_PLANNER_H
