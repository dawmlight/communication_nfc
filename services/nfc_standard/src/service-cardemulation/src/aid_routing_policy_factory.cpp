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

#include "aid_routing_policy_factory.h"

#include <memory>

#include "card_emulation_def.h"
#include "tag_priority_policy.h"

namespace OHOS::nfc::cardemulation {
std::unique_ptr<IAidRoutingPolicy> RoutingPolicyFactory::CreateRoutingPolicy(
    std::vector<std::string> supportedLocations,
    int aidRoutingMode,
    std::string priorTag)
{
    return std::make_unique<TagPriorityPolicy>(supportedLocations, aidRoutingMode, priorTag);
}
}  // namespace OHOS::nfc::cardemulation
