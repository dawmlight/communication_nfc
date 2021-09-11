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

#include "tag_priority_policy.h"

#include <assert.h>

#include <algorithm>

#include "aid_routing_filter.h"
#include "aid_routing_table.h"
#include "aid_set.h"
#include "card_emulation_def.h"
#include "card_emulation_service_info.h"
#include "element_name.h"
#include "loghelper.h"
#ifdef USE_HILOG
DEFINE_NFC_LOG_LABEL("TagPriorityPolicy");
#endif

namespace OHOS::nfc::cardemulation {
TagPriorityPolicy::TagPriorityPolicy(std::vector<std::string> supportedLocations,
                                     int aidMatchMode,
                                     std::string priorType)
    : priorType_(std::move(priorType)),
    supportedLocations_(std::move(supportedLocations)),
    aidRoutingMode_(aidMatchMode)
{
}

TagPriorityPolicy::~TagPriorityPolicy() = default;

std::vector<std::shared_ptr<nfc::cardemulation::AidSet>> TagPriorityPolicy::PlanRoutingTable(
    std::vector<std::shared_ptr<CardEmulationServiceInfo>> const& apps,
    const std::shared_ptr<CardEmulationServiceInfo>& primary,
    const std::shared_ptr<CardEmulationServiceInfo>& preferred)
{
#ifdef DEBUG
    printf("info size: %zu\n", apps.size());
    printf("matching mode: %d\n", aidRoutingMode_);

    printf("supported EE: ");
    for (auto& s : supportedLocations_) {
        printf("%s ", s.c_str());
    }
    printf(" \n");

    if (primary) {
        printf("primary name: %s\n", primary->GetName()->GetAbilityName().c_str());
        printf("aids: %s\n", primary->PrintAllAids().c_str());
    }
    if (preferred) {
        printf("preferred name: %s\n", preferred->GetName()->GetAbilityName().c_str());
        printf("aids: %s\n", preferred->PrintAllAids().c_str());
    }
#endif  // DEBUG

    std::vector<std::shared_ptr<AidSet>> result;

    auto isTagAllowed = [&result, this](const std::string& type) {
        auto found = std::find_if(result.begin(), result.end(), [this, type](decltype(result)::reference r) {
            return (StringEqual(r->GetType(), this->GetPriorType()) && StringEqual(type, r->GetType()));
        });

        return (found == result.end());
    };
    auto cmp = CardEmulationServiceInfo::ALessB;

    auto belongsToPrimary = [&primary](const std::shared_ptr<AidSet>& aidset) {
        if (!primary || !aidset) {
            return false;
        }
        auto owner = aidset->GetOwner().lock();
        if (!owner) {
            return false;
        }
        return owner == primary;
    };
    auto isAidAllowed = [&result, this, &belongsToPrimary](const std::shared_ptr<AidSet>& aidset) {
        bool allowed = true;
        for (auto& r : result) {
            if (r.get() == aidset.get()) {
                allowed = false;
                break;
            }
            // aid set conflicting
            if (aidset->ConflictWith(*r, false))
                // r belongs to primary
                if (belongsToPrimary(r) || StringEqual(aidset->GetType(), GetPriorType(), true) ||
                    !(this->ConflictingTolerant(aidset->GetExecutionEnvironment()))) {
                    allowed = false;
                    break;
                }
        }

        return allowed;
    };
    LocationFilter lf(supportedLocations_);
    ModeFilter mf(aidRoutingMode_);
    TagFilter tf(isTagAllowed);
    AidFilter af(isAidAllowed);

    std::vector<IRoutingFilter*> filters{&lf, &mf, &tf, &af};
#ifdef DEBUG
    if (primary) {
        DebugLog("primary service ability name: %s", primary->GetName()->GetAbilityName().c_str());
    }
    if (preferred) {
        DebugLog("preferred service ability name: %s", preferred->GetName()->GetAbilityName().c_str());
    }

#endif  // DEBUG

    auto handler = [&result, &filters, &cmp](const std::shared_ptr<CardEmulationServiceInfo>& app) {
#ifdef DEBUG
        std::vector<std::string> filterNames{"env filter", "pattern filter", "type filter", "aid filter"};
#endif  // DEBUG
        auto aidsets = app->SortAidsets(cmp);

        for (auto& aidset : aidsets) {
            bool allowed = true;
#ifdef DEBUG
            int i = 0;
#endif  // DEBUG

            for (auto& f : filters) {
#ifdef DEBUG
                ++i;
#endif  // DEBUG
                if (!f->Allow(aidset)) {
#ifdef DEBUG
                    std::string ownerName;
                    auto o = aidset->GetOwner().lock();
                    if (o) {
                        ownerName = o->GetName()->GetAbilityName();
                    }
                    printf("owner: %s, aidset: {%s}\n", ownerName.c_str(), aidset->ToString().c_str());
                    printf("%s denied\n", filterNames[i - 1].c_str());
#endif  // DEBUG
                    allowed = false;
                    break;
                }
            }
            if (allowed) {
                result.push_back(aidset);
            }
        }
    };
    if (primary) {
        handler(primary);
    }

    if (preferred) {
        handler(preferred);
    }

    for (auto& app : apps) {
        if (app != preferred) {
            handler(app);
        }
    }
    return result;
}

std::string TagPriorityPolicy::GetPriorType() const
{
    return priorType_;
}

bool TagPriorityPolicy::ConflictingTolerant(const std::string& location) const
{
    return StringEqual(location, NFC_EE_HOST, true);
}
}  // namespace OHOS::nfc::cardemulation
