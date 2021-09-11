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

#include "aid_routing_common_event.h"

#ifdef MOCK_FOR_TESTING
#include <algorithm>
#include <cstdio>

#include "aid_set.h"
#include "common_event_manager.h"
#include "want.h"

namespace OHOS::nfc::cardemulation {
static const std::string ACTION_AID_ROUTING_CHANGED = "ACTION_AID_ROUTING_CHANGED";
static const std::string KEY_AID_ROUTING_TABLE = "KEY_AID_ROUTING_TABLE";
void AidRoutingCommonEvent::PublishAidRoutingTable(const std::vector<std::shared_ptr<AidSet>>& table)
{
    using namespace OHOS::AAFwk;
    using namespace OHOS::EventFwk;

    std::vector<std::string> aids;

    std::for_each(table.cbegin(), table.cend(), [&aids](std::vector<std::shared_ptr<AidSet>>::const_reference r) {
        auto t = r->GetAllAidRawString();
        aids.insert(aids.end(), t.begin(), t.end());
    });

    Want want;
    want.SetAction(ACTION_AID_ROUTING_CHANGED);
    want.SetParam(KEY_AID_ROUTING_TABLE, aids);
    CommonEventData data;
    data.SetWant(want);
    bool rv = CommonEventManager::PublishCommonEvent(data);
    printf("publish routing changed event. result: %s\n", rv ? "succeeded" : "failed");
}
}  // namespace OHOS::nfc::cardemulation
#endif
