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

#ifndef TAG_PRIORITY_STRATEGY_H
#define TAG_PRIORITY_STRATEGY_H

#include <string>
#include <vector>

#include "iaid_routing_policy.h"
namespace OHOS::nfc::cardemulation {
class TagPriorityPolicy final : public IAidRoutingPolicy {
public:
    TagPriorityPolicy(std::vector<std::string> supportedLocations, int aidRoutingMode, std::string priorType);
    virtual ~TagPriorityPolicy();
    std::vector<std::shared_ptr<nfc::cardemulation::AidSet>> PlanRoutingTable(
        std::vector<std::shared_ptr<CardEmulationServiceInfo>> const& apps,
        const std::shared_ptr<CardEmulationServiceInfo>& primary,
        const std::shared_ptr<CardEmulationServiceInfo>& preferred) override;

private:
    std::string GetPriorType() const;
    bool ConflictingTolerant(const std::string& location) const;

private:
    std::string priorType_;
    std::vector<std::string> supportedLocations_;
    int aidRoutingMode_;
};
}  // namespace OHOS::nfc::cardemulation
#endif  // TAG_PRIORITY_STRATEGY_H
