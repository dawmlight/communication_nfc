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
#ifndef _SETTING_BROADCAST_H_
#define _SETTING_BROADCAST_H_

#include "common_event_manager.h"
#include "element_name.h"
#include "want.h"

namespace OHOS::nfc::test {
class SettingBroadcast final {
public:
    static void NotifyPrimaryServiceChanged(const OHOS::AppExecFwk::ElementName& name)
    {
        using namespace OHOS::AAFwk;
        using namespace OHOS::EventFwk;

        Want want;
        want.SetAction(ACTION_SETTING_PRIMARY_SERVICE_CHANGED);
        want.SetParam(KEY_DEVICE_ID, name.GetDeviceID());
        want.SetParam(KEY_BUNDLE_NAME, name.GetBundleName());
        want.SetParam(KEY_ABILITY_NAME, name.GetAbilityName());
        CommonEventData data;
        data.SetWant(want);
        bool rv = CommonEventManager::PublishCommonEvent(data);
        printf("publish primary service changed event. result: %s\n", rv ? "succeeded" : "failed");
    }

private:
    static const std::string ACTION_SETTING_PRIMARY_SERVICE_CHANGED;
    static const std::string KEY_DEVICE_ID;
    static const std::string KEY_BUNDLE_NAME;
    static const std::string KEY_ABILITY_NAME;
};
const std::string SettingBroadcast::ACTION_SETTING_PRIMARY_SERVICE_CHANGED = "ACTION_SETTING_PRIMARY_SERVICE_CHANGED";

const std::string SettingBroadcast::KEY_DEVICE_ID = "KEY_DEVICE_ID";
const std::string SettingBroadcast::KEY_BUNDLE_NAME = "KEY_BUNDLE_NAME";
const std::string SettingBroadcast::KEY_ABILITY_NAME = "KEY_ABILITY_NAME";
}  // namespace OHOS::nfc::test
#endif  //_SETTING_BROADCAST_H_
