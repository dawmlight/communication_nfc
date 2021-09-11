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

#ifndef IAID_ROUTING_POLICY_H
#define IAID_ROUTING_POLICY_H

#include <memory>
#include <vector>

namespace OHOS::nfc::cardemulation {
class AidSet;
class CardEmulationServiceInfo;
class IAidRoutingPolicy {
public:
    virtual ~IAidRoutingPolicy() = default;

    virtual std::vector<std::shared_ptr<AidSet>> PlanRoutingTable(
        std::vector<std::shared_ptr<CardEmulationServiceInfo>> const& apps,
        const std::shared_ptr<CardEmulationServiceInfo>& primary,
        const std::shared_ptr<CardEmulationServiceInfo>& preferred) = 0;
};
}  // namespace OHOS::nfc::cardemulation
#endif  // IAID_ROUTING_POLICY_H
