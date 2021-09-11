#include "nfc_test_env.h"

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

NfcTestEnv::~NfcTestEnv()
{
    mContext_ = std::shared_ptr<osal::Context>();
    CommonEventManager::UnSubscribeCommonEvent(mReceiver_);

    mReceiver_ = std::shared_ptr<NfcStateChangeReceiver>();
}

void NfcTestEnv::Initialize()
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

void NfcTestEnv::SendSystemBoardcast(std::string action)
{
    // shared_ptr<Intent> intent = make_shared<Intent>(action);
    // mContext_->SendBroadcast(intent);
}

void NfcTestEnv::SetScreenOn(bool state)
{
    std::weak_ptr<osal::PowerManager> powerManager = mContext_->GetPowerManagerService();
    if (powerManager.expired()) {
        return;
    }
    powerManager.lock()->SetScreenOn(state);
}

void NfcTestEnv::SetKeyguardLocked(bool status)
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