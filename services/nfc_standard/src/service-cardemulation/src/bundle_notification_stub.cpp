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

#include "bundle_notification_stub.h"

#include <algorithm>

#include "bundle_mgr_proxy.h"
#include "card_emulation_def.h"
#include "card_emulation_service_info.h"
#include "common_event_manager.h"
#include "element_name.h"
#include "iservice_registry.h"
#include "loghelper.h"
#include "refbase.h"
#include "system_ability_definition.h"
#include "want.h"

namespace OHOS::nfc::cardemulation {
BundleNotificationStub::BundleNotificationStub(BundleChangedHandler_t handler) : handler_(std::move(handler))
{
}

BundleNotificationStub::~BundleNotificationStub()
{
}

void BundleNotificationStub::OnBundleStateChanged(const uint8_t installType,
                                                  const int32_t resultCode,
                                                  const std::string& resultMsg,
                                                  const std::string& bundleName)
{
    using namespace OHOS::AppExecFwk;
    using namespace OHOS;
    if (handler_) {
        handler_(installType == static_cast<uint8_t>(InstallType::INSTALL_CALLBACK), bundleName);
    }
}

void BundleNotificationStub::Register()
{
#ifdef MOCK_FOR_TESTING
    BundleChangedEventSubscriber::Subscribe(shared_from_this());
#endif
}

void BundleNotificationStub::Unregister()
{
#ifdef MOCK_FOR_TESTING
    BundleChangedEventSubscriber::Subscribe(nullptr);
#endif
}

#ifdef MOCK_FOR_TESTING
std::shared_ptr<BundleChangedEventSubscriber> BundleChangedEventSubscriber::subscriber_;
const std::string BundleChangedEventSubscriber::ACTION_CARDEMULATION_BUNDLE_CHANGED =
    "ACTION_CARDEMULATION_BUNDLE_CHANGED";
const std::string BundleChangedEventSubscriber::KEY_CHANGED_TYPE = "KEY_CHANGED_TYPE";
const std::string BundleChangedEventSubscriber::KEY_RESULT_CODE = "KEY_RESULT_CODE";
const std::string BundleChangedEventSubscriber::KEY_RESULT_MSG = "KEY_RESULT_MSG";
const std::string BundleChangedEventSubscriber::KEY_BUNDLE_NAME = "KEY_BUNDLE_NAME";

void BundleChangedEventSubscriber::Subscribe(const std::shared_ptr<BundleNotificationStub>& handler)
{
    if (!subscriber_) {
        OHOS::EventFwk::MatchingSkills matchSkill;
        matchSkill.AddEvent(ACTION_CARDEMULATION_BUNDLE_CHANGED);
        OHOS::EventFwk::CommonEventSubscribeInfo info(matchSkill);
        subscriber_ = std::make_unique<BundleChangedEventSubscriber>(info);
        bool rv = OHOS::EventFwk::CommonEventManager::SubscribeCommonEvent(subscriber_);
        DebugLog("subscribe bundle change event result: %s\n", rv ? "succeeded" : "failed");
    }
    subscriber_->SetHandler(handler);
}

BundleChangedEventSubscriber::BundleChangedEventSubscriber(const OHOS::EventFwk::CommonEventSubscribeInfo& info)
    : OHOS::EventFwk::CommonEventSubscriber(info)
{
}

BundleChangedEventSubscriber::~BundleChangedEventSubscriber()
{
    OHOS::EventFwk::CommonEventManager::UnSubscribeCommonEvent(subscriber_);
}
void BundleChangedEventSubscriber::OnReceiveEvent(const OHOS::EventFwk::CommonEventData& data)
{
    if (handler_) {
        using namespace OHOS::AppExecFwk;
        using namespace OHOS::AAFwk;
        Want want = data.GetWant();

        auto installType = static_cast<uint8_t>(want.GetIntParam(KEY_CHANGED_TYPE, 0));
        auto resultCode = static_cast<int32_t>(want.GetIntParam(KEY_RESULT_CODE, 0));
        auto resultMsg = want.GetStringParam(KEY_RESULT_MSG);
        auto bundleName = want.GetStringParam(KEY_BUNDLE_NAME);
        handler_->OnBundleStateChanged(installType, resultCode, resultMsg, bundleName);
    }
}
void BundleChangedEventSubscriber::SetHandler(const std::shared_ptr<BundleNotificationStub>& handler)
{
    handler_ = handler;
}
#endif
}  // namespace OHOS::nfc::cardemulation
