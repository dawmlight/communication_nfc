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

#ifndef IAID_ROUTING_MANAGER_H
#define IAID_ROUTING_MANAGER_H

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "icard_emulation_service_info_observer.h"

namespace OHOS::nfc::cardemulation {
class CardEmulationServiceInfo;
using ServiceInfoTypePair = std::pair<std::weak_ptr<CardEmulationServiceInfo>, std::string>;
class IAidRoutingManager : public ICardEmulationServiceInfoObserver {
public:
    virtual ~IAidRoutingManager() = default;

    /**
     * brief: get service infomation and service type pair by aid
     * parameter: 
     *   aid -- included aid by card emulation service
     * return: card emulation service infomations and service types
     */
    virtual std::vector<ServiceInfoTypePair> GetCardEmulationServicesByAid(const std::string& aid) = 0;
private:
};
}  // namespace OHOS::nfc::cardemulation

#endif  // !IAID_ROUTING_MANAGER_H
