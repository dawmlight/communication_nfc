#include "utils/screen_state_helper.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "context.h"
#include "key_guard_manager.h"
#include "power_manager.h"

using namespace testing;
using namespace OHOS::nfc;
using namespace osal;

using ::testing::Return;

TEST(ScreenStateHelper, CheckScreenState_Test)
{
    std::shared_ptr<Context> ctx = std::make_shared<Context>();
    std::shared_ptr<ScreenStateHelper> helper = std::make_shared<ScreenStateHelper>(ctx);
    std::weak_ptr<PowerManager> pm = ctx->GetPowerManagerService();
    std::weak_ptr<KeyguardManager> km = ctx->GetKeyguardManagerService();
    // SCREEN_STATE_ON_LOCKED
    pm.lock()->SetScreenOn(false);
    km.lock()->SetKeyguardLocked(false);
    EXPECT_EQ(helper->CheckScreenState(), SCREEN_STATE_OFF_UNLOCKED);
    km.lock()->SetKeyguardLocked(true);
    EXPECT_EQ(helper->CheckScreenState(), SCREEN_STATE_OFF_LOCKED);
    pm.lock()->SetScreenOn(true);
    // SCREEN_STATE_ON_LOCKED
    km.lock()->SetKeyguardLocked(true);
    EXPECT_EQ(helper->CheckScreenState(), SCREEN_STATE_ON_LOCKED);
    // SCREEN_STATE_ON_UNLOCKED
    km.lock()->SetKeyguardLocked(false);
    EXPECT_EQ(helper->CheckScreenState(), SCREEN_STATE_ON_UNLOCKED);
}