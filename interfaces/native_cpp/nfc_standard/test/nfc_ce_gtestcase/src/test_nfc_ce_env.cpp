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
#include "test_nfc_ce_env.h"

#include "common_event_manager.h"
#include "context.h"
#include "key_guard_manager.h"
#include "loghelper.h"
#include "power_manager.h"

using namespace osal;

namespace OHOS {
using EventFwk::CommonEventData;
using EventFwk::CommonEventManager;
using EventFwk::CommonEventSubscribeInfo;
using EventFwk::CommonEventSubscriber;
using EventFwk::MatchingSkills;

namespace nfc {
namespace test {
class NfcStateChangeReceiver : public CommonEventSubscriber {
public:
    explicit NfcStateChangeReceiver(const CommonEventSubscribeInfo& subscribeInfo);
    ~NfcStateChangeReceiver() {}
    void OnReceiveEvent(const CommonEventData& data) override;

private:
};

NfcStateChangeReceiver::NfcStateChangeReceiver(const CommonEventSubscribeInfo& subscribeInfo)
    : CommonEventSubscriber(subscribeInfo)
{
}

void NfcStateChangeReceiver::OnReceiveEvent(const CommonEventData& data)
{
    const std::string EXTRA_ADAPTER_STATE = "ohos.nfc.extra.ADAPTER_STATE";
    std::string action = data.GetWant().GetAction();
    int state = data.GetWant().GetIntParam(EXTRA_ADAPTER_STATE, -1);
    InfoLog("NfcStateChangeReceiver::OnReceiveEvent: Action (%s), newState (%d)", action.c_str(), state);
}

TestNfcCEEnv::~TestNfcCEEnv()
{
    mContext_ = std::shared_ptr<osal::Context>();
    CommonEventManager::UnSubscribeCommonEvent(mReceiver_);

    mReceiver_ = std::shared_ptr<NfcStateChangeReceiver>();
}

void TestNfcCEEnv::Initialize()
{
    const std::string ACTION_ADAPTER_STATE_CHANGED = "ohos.nfc.action.ADAPTER_STATE_CHANGED";

    mContext_ = std::make_shared<Context>();
    // Subscribe Common Event
    MatchingSkills matchingSkills;
    matchingSkills.AddEvent(ACTION_ADAPTER_STATE_CHANGED);
    CommonEventSubscribeInfo subscribeInfo(matchingSkills);
    mReceiver_ = std::make_shared<NfcStateChangeReceiver>(subscribeInfo);
    CommonEventManager::SubscribeCommonEvent(mReceiver_);
}

void TestNfcCEEnv::SendSystemBoardcast(std::string action)
{
    // shared_ptr<Intent> intent = make_shared<Intent>(action);
    // mContext_->SendBroadcast(intent);
}

void TestNfcCEEnv::SetScreenOn(bool state)
{
    std::weak_ptr<osal::PowerManager> powerManager = mContext_->GetPowerManagerService();
    if (powerManager.expired()) {
        return;
    }
    powerManager.lock()->SetScreenOn(state);
}

void TestNfcCEEnv::SetKeyguardLocked(bool status)
{
    std::weak_ptr<osal::KeyguardManager> keyguardManager = mContext_->GetKeyguardManagerService();
    if (keyguardManager.expired()) {
        return;
    }
    keyguardManager.lock()->SetKeyguardLocked(status);
}
}  // namespace test
}  // namespace nfc
}  // namespace OHOS