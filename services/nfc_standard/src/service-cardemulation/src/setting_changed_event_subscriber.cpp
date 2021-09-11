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

#include "setting_changed_event_subscriber.h"

#include "common_event_manager.h"
#include "element_name.h"
#include "loghelper.h"
#include "want.h"

namespace OHOS::nfc::cardemulation {
const std::string SettingChangedEventSubscriber::ACTION_SETTING_PRIMARY_SERVICE_CHANGED =
    "ACTION_SETTING_PRIMARY_SERVICE_CHANGED";

const std::string SettingChangedEventSubscriber::KEY_DEVICE_ID = "KEY_DEVICE_ID";
const std::string SettingChangedEventSubscriber::KEY_BUNDLE_NAME = "KEY_BUNDLE_NAME";
const std::string SettingChangedEventSubscriber::KEY_ABILITY_NAME = "KEY_ABILITY_NAME";
std::shared_ptr<SettingChangedEventSubscriber> SettingChangedEventSubscriber::subscriber_;
void SettingChangedEventSubscriber::Subscribe(SettingHandler_t handler)
{
    if (!subscriber_) {
        OHOS::EventFwk::MatchingSkills matchSkill;
        matchSkill.AddEvent(ACTION_SETTING_PRIMARY_SERVICE_CHANGED);
        OHOS::EventFwk::CommonEventSubscribeInfo info(matchSkill);
        subscriber_ = std::shared_ptr<SettingChangedEventSubscriber>(new SettingChangedEventSubscriber(info));
        bool rv = OHOS::EventFwk::CommonEventManager::SubscribeCommonEvent(subscriber_);
        DebugLog("subscribe setting change event result: %s\n", rv ? "succeeded" : "failed");
    }

    subscriber_->SetHandler(std::move(handler));
}
void SettingChangedEventSubscriber::SetHandler(SettingHandler_t handler)
{
    handler_ = std::move(handler);
}
void SettingChangedEventSubscriber::OnReceiveEvent(const OHOS::EventFwk::CommonEventData& data)
{
    using namespace OHOS::AppExecFwk;
    using namespace OHOS::AAFwk;
    Want want = data.GetWant();
    ElementName name(want.GetStringParam(KEY_DEVICE_ID),
                     want.GetStringParam(KEY_BUNDLE_NAME),
                     want.GetStringParam(KEY_ABILITY_NAME));
    DebugLog("SettingChangedEventSubscriber::OnReceiveEvent: %s.%s.%s",
             name.GetAbilityName().c_str(),
             name.GetBundleName().c_str(),
             name.GetDeviceID().c_str());
    if (handler_) {
        handler_(name);
    }
}
SettingChangedEventSubscriber::SettingChangedEventSubscriber(const OHOS::EventFwk::CommonEventSubscribeInfo& info)
    : OHOS::EventFwk::CommonEventSubscriber(info)
{
}
SettingChangedEventSubscriber::~SettingChangedEventSubscriber()
{
    OHOS::EventFwk::CommonEventManager::UnSubscribeCommonEvent(subscriber_);
}
}  // namespace OHOS::nfc::cardemulation
