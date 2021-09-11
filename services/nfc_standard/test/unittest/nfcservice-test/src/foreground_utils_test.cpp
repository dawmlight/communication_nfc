#include "utils/foreground_utils.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "context.h"
#include "key_guard_manager.h"
#include "power_manager.h"

using namespace testing;
using namespace OHOS::nfc;
using namespace osal;

using ::testing::Return;

namespace {
class CallbackImp : public ForegroundUtils::Callback {
public:
    void OnUidToBackground(int uid)
    {
        uid = 0;
    }
};
}  // namespace

TEST(ForegroundUtils, GetInstance_Test)
{
    EXPECT_TRUE(ForegroundUtils::GetInstance());
}

TEST(ForegroundUtils, RegisterUidToBackgroundCallback_Test)
{
    ForegroundUtils* utils = ForegroundUtils::GetInstance();
    std::shared_ptr<ForegroundUtils::Callback> callback;
    EXPECT_TRUE(utils->RegisterUidToBackgroundCallback(callback, 0));
    EXPECT_FALSE(utils->RegisterUidToBackgroundCallback(callback, 0));
    EXPECT_TRUE(utils->RegisterUidToBackgroundCallback(callback, 2));
}

TEST(ForegroundUtils, OnForegroundActivitiesChanged_Test)
{
    ForegroundUtils* utils = ForegroundUtils::GetInstance();
    std::shared_ptr<Context> ctx = std::make_shared<Context>();
    std::shared_ptr<ForegroundUtils::Callback> callback = std::make_shared<CallbackImp>();
    EXPECT_TRUE(utils->RegisterUidToBackgroundCallback(callback, -1));
    EXPECT_EQ(utils->IsInForegroundForTest(0), false);
    EXPECT_EQ(utils->IsInForegroundForTest(-1), false);
    utils->OnForegroundActivitiesChanged(-1, 0, true);
    EXPECT_EQ(utils->IsInForegroundForTest(0), true);
    EXPECT_EQ(utils->IsInForegroundForTest(-1), false);
    utils->OnForegroundActivitiesChanged(1, -1, true);
    EXPECT_EQ(utils->IsInForegroundForTest(0), true);
    EXPECT_EQ(utils->IsInForegroundForTest(-1), true);
    utils->OnForegroundActivitiesChanged(1, 1, true);
    utils->OnForegroundActivitiesChanged(1, 0, true);
    utils->OnProcessDied(-1, 0);
    EXPECT_EQ(utils->IsInForegroundForTest(0), true);
    utils->OnProcessDied(1, 0);
    EXPECT_EQ(utils->IsInForegroundForTest(0), false);
    EXPECT_EQ(utils->IsInForegroundForTest(-1), true);
    utils->OnProcessDied(1, -1);
    EXPECT_EQ(utils->IsInForegroundForTest(0), false);
    EXPECT_EQ(utils->IsInForegroundForTest(-1), false);
}
