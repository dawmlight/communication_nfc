#include "watch_dog.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "context.h"
#include "nfc_service.h"
#include "test-ncibal/device_host_mock.h"

using namespace osal;
using namespace OHOS::nfc;
using namespace OHOS::nfc::ncibal;
using namespace testing;

TEST(WatchDog, Run_Test)
{
    std::shared_ptr<DeviceHostMock> deviceHost = std::make_shared<DeviceHostMock>();
    EXPECT_CALL(*deviceHost, Abort()).Times(1);
    WatchDog watchDog("enableInternal", 100, deviceHost);
    watchDog.Run();
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
}

TEST(WatchDog, Canel_Test)
{
    std::shared_ptr<DeviceHostMock> deviceHost = std::make_shared<DeviceHostMock>();
    EXPECT_CALL(*deviceHost, Abort()).Times(0);
    WatchDog watchDog("enableInternal", 100, deviceHost);
    watchDog.Run();
    watchDog.Cancel();
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
}