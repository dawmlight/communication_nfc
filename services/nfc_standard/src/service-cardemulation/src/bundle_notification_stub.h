/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 *
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software

 * * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied.
 * See the License for the specific language governing permissions and
 * limitations
 * under the License.
 */

#ifndef CARD_EMULATION_BUNDLE_NOTIFICATION_STUB_H
#define CARD_EMULATION_BUNDLE_NOTIFICATION_STUB_H

#include <functional>
#include <memory>

#include "bundle_status_callback_interface.h"
#include "common_event_subscriber.h"
#include "iremote_stub.h"

namespace OHOS::nfc::cardemulation {
using BundleChangedHandler_t = std::function<void(bool installing, const std::string& bundleName)>;

class BundleNotificationStub : public OHOS::IRemoteStub<::OHOS::AppExecFwk::IBundleStatusCallback>,
                               public std::enable_shared_from_this<BundleNotificationStub> {
public:
    explicit BundleNotificationStub(BundleChangedHandler_t handler);
    ~BundleNotificationStub() override;
    void OnBundleStateChanged(const uint8_t installType,
                              const int32_t resultCode,
                              const std::string& resultMsg,
                              const std::string& bundleName) override;

    void Register();
    void Unregister();

private:
    BundleChangedHandler_t handler_;
};

#ifdef MOCK_FOR_TESTING

class BundleChangedEventSubscriber final : public OHOS::EventFwk::CommonEventSubscriber {
public:
    static void Subscribe(const std::shared_ptr<BundleNotificationStub>& handler);
    BundleChangedEventSubscriber(const OHOS::EventFwk::CommonEventSubscribeInfo& info);
    ~BundleChangedEventSubscriber() override;

    void OnReceiveEvent(const OHOS::EventFwk::CommonEventData& data) override;
    void SetHandler(const std::shared_ptr<BundleNotificationStub>& handler);

private:
    std::shared_ptr<BundleNotificationStub> handler_;
    static std::shared_ptr<BundleChangedEventSubscriber> subscriber_;
    static const std::string ACTION_CARDEMULATION_BUNDLE_CHANGED;
    static const std::string KEY_CHANGED_TYPE;
    static const std::string KEY_RESULT_CODE;
    static const std::string KEY_RESULT_MSG;
    static const std::string KEY_BUNDLE_NAME;
};
#endif
}  // namespace OHOS::nfc::cardemulation
#endif  // !CARD_EMULATION_BUNDLE_NOTIFICATION_STUB_H
