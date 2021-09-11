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

#include "aid_routing_planner.h"

#include <assert.h>

#include <algorithm>
#include <sstream>

#include "aid_routing_adapter.h"
#include "aid_routing_filter.h"
#include "aid_routing_table.h"
#include "aid_set.h"
#include "aid_string.h"
#include "card_emulation_def.h"
#include "card_emulation_error.h"
#include "card_emulation_service_info.h"
#include "element_name.h"
#include "iaid_routing_policy.h"
#include "loghelper.h"
#ifdef USE_HILOG
DEFINE_NFC_LOG_LABEL("AidRoutingPlanner");
#endif
#ifdef MOCK_FOR_TESTING
#include "aid_routing_common_event.h"
#endif

namespace OHOS::nfc::cardemulation {
class AidBatchAdder {
public:
    AidBatchAdder(std::shared_ptr<AidRoutingAdapter> routingController, int reserved)
        : routingController_(routingController),
        reserved_(reserved){};
    void AddAidRoutingEntry(std::vector<unsigned char>&& aid, const std::string& target, int aidType);

    int AddAidsWhenEnoughSpace();

private:
    std::shared_ptr<AidRoutingAdapter> routingController_;
    int reserved_;  

    std::vector<RoutingEntry> entries_{};
};

AidRoutingPlanner::AidRoutingPlanner(std::unique_ptr<IAidRoutingPolicy> routingStrategy,
                                     const std::shared_ptr<AidRoutingAdapter>& routingController)
    : routingPolicy_(std::move(routingStrategy)),
    routingController_(routingController),
    aidTable_(),
    mu_()
{
}
int AidRoutingPlanner::Init()
{
    return 0;
}

int AidRoutingPlanner::OnCeServiceChanged(const std::vector<std::shared_ptr<CardEmulationServiceInfo>>& infos,
                                          const std::shared_ptr<CardEmulationServiceInfo>& primary,
                                          const std::shared_ptr<CardEmulationServiceInfo>& preferred)
{
    if (!routingPolicy_ || !routingController_) {
        return ERR_NULL_POINTER;
    }

    auto routingTable = routingPolicy_->PlanRoutingTable(infos, primary, preferred);
    // - AID_HEAD_LENGTH for default routing entry
    routingController_->ClearRoutingTable();
    int remain = routingController_->GetRemainRoutingTableSize() - AID_HEAD_LENGTH;
    if (remain < 1) {
        return ERR_ROUTING_TABLE_NOT_ENOUGH_CAPACITY;
    }

    for (auto it = std::begin(routingTable); it != std::end(routingTable); ++it) {
        auto& aidset = *it;
        auto location = aidset->GetExecutionEnvironment();
        if (routingController_->IsDefaultRoute(location)) {
            DebugLog("env: %s is the same env with default: 0x%02X",
                     location.c_str(),
                     routingController_->GetDefaultRoute());
            continue;
        }
        AidBatchAdder adder(routingController_, remain);
        aidset->Visit([&adder, location](const std::string&, const AidString& aidstring) {
            std::vector<unsigned char> bytes;
            aidstring.ToBytes(bytes);
            assert(!bytes.empty());
            assert(aidstring.GetType() != AidType::INVALID);
            adder.AddAidRoutingEntry(std::move(bytes), location, AidTypeToInt(aidstring.GetType()));
        });
        // batch add aids of AidSet.
        // cannot add in part.
        remain = adder.AddAidsWhenEnoughSpace();
        if (remain < AID_HEAD_LENGTH) {
            // remain is insufficient.
            // host is default route. so erase all of not in host
            if (routingController_->IsDefaultRoute(NFC_EE_HOST)) {
                routingTable.erase(std::remove_if(it, routingTable.end(), [this](decltype(it)::reference r) {
                    return r->GetExecutionEnvironment() != NFC_EE_HOST;
                }));
            } else {
                routingTable.erase(it, routingTable.end());
            }
            break;
        }
    }

    int addState = AddDefaultRouting(primary, routingTable);

    {
        std::lock_guard<std::mutex> lk(mu_);
        aidTable_ = std::move(routingTable);
#ifdef USE_HILOG

        DebugLog("cardemulation service count : %{public}zu, routing table capacity: %{public}zu bytes",
                 infos.size(),
                 remain);
#else
        // TODO:
        std::stringstream ss;
        ss << "cardemulation service count:" << infos.size() << ", routing table remain size: " << remain << "\n";
        ss << "uncommit routing entry size: " << aidTable_.size() + (IS_OK(addState) ? 1u : 0u) << "\n";

        std::for_each(aidTable_.begin(), aidTable_.end(), [&ss](auto r) {
            std::vector<std::string> aids = r->GetAllAidRawString(true);
            ss << "EE: " << r->GetExecutionEnvironment();
            ss << ", aid: [";
            for (auto& s : aids) {
                ss << s << ", ";
            }
            ss << "]\n";
        });
        if (IS_OK(addState)) {
            ss << "EE: 0x" << std::hex << routingController_->GetDefaultRoute() << ", aid: []\n";
        }
        printf("%s", ss.str().c_str());
#endif
#ifdef MOCK_FOR_TESTING
        AidRoutingCommonEvent::PublishAidRoutingTable(aidTable_);
#endif
    }
    return routingController_->CommitAidRouting();
}

std::vector<ServiceInfoTypePair> AidRoutingPlanner::GetCardEmulationServicesByAid(const std::string& aid)
{
    std::vector<ServiceInfoTypePair> rv;
    if (aid.empty()) {
        return rv;
    }
    AidString aidStr(aid);
    if (!aidStr.IsValid()) {
        return rv;
    }
    DebugLog("aid: %s", aidStr.ToString().c_str());
    std::lock_guard<std::mutex> lk(mu_);
    std::for_each(aidTable_.cbegin(), aidTable_.cend(), [&rv, &aidStr](decltype(aidTable_)::const_reference r) {
        if (r) {
            if (r->HasAidString(aidStr)) {
                //
                rv.emplace_back(std::make_pair(r->GetOwner(), r->GetType()));
            }
        }
    });

    return rv;
}

int AidRoutingPlanner::AddDefaultRouting(const std::shared_ptr<CardEmulationServiceInfo>& primary,
                                         std::vector<std::shared_ptr<nfc::cardemulation::AidSet>>& routingTable)
{
    int route = routingController_->GetDefaultRoute();
    DebugLog("default route: 0x%02X", route);
    return routingController_->AddAidRoutingEntry({}, route, AidTypeToInt(AidType::PREFIX));
}

void AidBatchAdder::AddAidRoutingEntry(std::vector<unsigned char>&& aid, const std::string& target, int aidType)
{
    auto re = RoutingEntry();
    re.aid_ = aid;
    re.route_ = routingController_->LocationStringToInt(target);
    re.aidPattern_ = aidType;
    entries_.emplace_back(re);
    reserved_ -= aid.size() + AID_HEAD_LENGTH;
}

int AidBatchAdder::AddAidsWhenEnoughSpace()
{
    if (reserved_ >= 0) {
        std::for_each(std::begin(entries_), std::end(entries_), [this](decltype(entries_)::reference r) {
            routingController_->AddAidRoutingEntry(r.aid_, r.route_, r.aidPattern_);
        });
    }
    return reserved_;
}
}  // namespace OHOS::nfc::cardemulation
