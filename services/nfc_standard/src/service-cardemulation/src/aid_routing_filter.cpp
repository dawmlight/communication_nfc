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

#include "aid_routing_filter.h"

#include "aid_string.h"
#include "card_emulation_def.h"
#ifdef DEBUG

#include <algorithm>
#include <cstdio>
#endif  // DEBUG

namespace OHOS::nfc::cardemulation {
LocationFilter::LocationFilter(std::vector<std::string> supportedLocation)
    : supportedLocation_(std::move(supportedLocation))
{
}

bool LocationFilter::Allow(const std::shared_ptr<AidSet>& aidset)
{
    bool allowed = false;
    auto location = aidset->GetExecutionEnvironment();
    allowed = std::any_of(
        supportedLocation_.begin(),
        supportedLocation_.end(),
        [&location](decltype(supportedLocation_)::reference r) { return StringStartsWith(r, location, true); });

    return allowed;
}

TagFilter::TagFilter(std::function<bool(const std::string&)> isTagAllowed) : isTagAllowed_(isTagAllowed)
{
}

bool TagFilter::Allow(const std::shared_ptr<AidSet>& aidset)
{
    bool allowed = false;
    aidset->Visit([&allowed, this](const std::string& tagName, const AidString&) {
        if (this->isTagAllowed_) {
            allowed = this->isTagAllowed_(tagName);
        }
    });

    return allowed;
}

AidFilter::AidFilter(std::function<bool(const std::shared_ptr<AidSet>&)> isAidAllowed) : isAidAllowed_(isAidAllowed)
{
}

bool AidFilter::Allow(const std::shared_ptr<AidSet>& aidset)
{
    return isAidAllowed_ && isAidAllowed_(aidset);
}

ModeFilter::ModeFilter(int mode) : mode_(mode)
{
}

bool ModeFilter::Allow(const std::shared_ptr<AidSet>& aidset)
{
    bool allowed = true;

    if (!aidset) {
        allowed = false;
    } else {
        aidset->Visit([&allowed, this](const std::string&, const AidString& aid) {
            if (aid.IsPrefix() && !SupportedPrefix()) {
                allowed = false;
            } else if (aid.IsSubset() && !SupportedSubset()) {
                allowed = false;
            }
        });
    }
    return allowed;
}

bool ModeFilter::SupportedPrefix() const
{
    return IsAidPrefixMode(mode_);
}

bool ModeFilter::SupportedSubset() const
{
    return IsAidSubsetMode(mode_);
}
}  // namespace OHOS::nfc::cardemulation
