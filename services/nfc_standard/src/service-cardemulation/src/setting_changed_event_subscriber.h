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

#ifndef NFC_SETTING_CHANGED_EVENT_SUBSCRIBER
#define NFC_SETTING_CHANGED_EVENT_SUBSCRIBER
#include <functional>
#include <memory>
#include <string>

#include "common_event_subscriber.h"

namespace OHOS {
namespace AppExecFwk {
class ElementName;
class CommonEventSubscribeInfo;
}  // namespace AppExecFwk
namespace nfc::cardemulation {
using SettingHandler_t = std::function<void(const OHOS::AppExecFwk::ElementName&)>;
class SettingChangedEventSubscriber final : public OHOS::EventFwk::CommonEventSubscriber {
public:
    static void Subscribe(SettingHandler_t handler);
    explicit SettingChangedEventSubscriber(const OHOS::EventFwk::CommonEventSubscribeInfo& info);
    ~SettingChangedEventSubscriber();
    void SetHandler(SettingHandler_t handler);
    void OnReceiveEvent(const OHOS::EventFwk::CommonEventData& data) override;

private:
    SettingHandler_t handler_;
    static const std::string ACTION_SETTING_PRIMARY_SERVICE_CHANGED;
    static const std::string KEY_DEVICE_ID;
    static const std::string KEY_BUNDLE_NAME;
    static const std::string KEY_ABILITY_NAME;
    static std::shared_ptr<SettingChangedEventSubscriber> subscriber_;
};
}  // namespace nfc::cardemulation
}  // namespace OHOS
#endif  // !NFC_SETTING_CHANGED_EVENT_SUBSCRIBER
