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
#ifndef _BUNDLE_CHANGED_BROADCAST_H_
#define _BUNDLE_CHANGED_BROADCAST_H_

#include <memory>
#include <string>

#include "common_event_manager.h"
#include "element_name.h"
#include "want.h"

namespace OHOS::nfc::test {
class BundleChangedBroadcast {
private:
    static const std::string ACTION_CARDEMULATION_BUNDLE_CHANGED;
    static const std::string KEY_CHANGED_TYPE;
    static const std::string KEY_RESULT_CODE;
    static const std::string KEY_RESULT_MSG;
    static const std::string KEY_BUNDLE_NAME;

public:
    static void NotifyPrimaryServiceChanged(bool installing, const std::string& bundleName)
    {
        using namespace OHOS::AAFwk;
        using namespace OHOS::EventFwk;

        Want want;
        want.SetAction(ACTION_CARDEMULATION_BUNDLE_CHANGED);
        want.SetParam(KEY_CHANGED_TYPE, installing ? 1 : 0);
        want.SetParam(KEY_RESULT_CODE, 0);
        want.SetParam(KEY_RESULT_MSG, std::string(""));
        want.SetParam(KEY_BUNDLE_NAME, bundleName);
        CommonEventData data;
        data.SetWant(want);
        bool rv = CommonEventManager::PublishCommonEvent(data);
        printf("publish bundle changed event. result: %s\n", rv ? "succeeded" : "failed");
    }
};

const std::string BundleChangedBroadcast::ACTION_CARDEMULATION_BUNDLE_CHANGED = "ACTION_CARDEMULATION_BUNDLE_CHANGED";
const std::string BundleChangedBroadcast::KEY_CHANGED_TYPE = "KEY_CHANGED_TYPE";
const std::string BundleChangedBroadcast::KEY_RESULT_CODE = "KEY_RESULT_CODE";
const std::string BundleChangedBroadcast::KEY_RESULT_MSG = "KEY_RESULT_MSG";
const std::string BundleChangedBroadcast::KEY_BUNDLE_NAME = "KEY_BUNDLE_NAME";
}  // namespace OHOS::nfc::test
#endif  //_BUNDLE_CHANGED_BROADCAST_H_
