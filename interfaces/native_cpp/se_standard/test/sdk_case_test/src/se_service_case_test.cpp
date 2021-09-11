#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <chrono>
#include <memory>
#include <thread>

#include "bundle_manager.h"
#include "cb_connect_mock.h"
#include "context.h"
#include "ipc_skeleton.h"
#include "iremote_object.h"
#include "isecure_element_service.h"
#include "loghelper.h"
#include "osal_bundle_info.h"
#include "se_service.h"

using osal::BundleInfo;
using osal::BundleManager;
using osal::Context;
using namespace testing;
namespace OHOS::se {
namespace omapi {
namespace test {
class SEServiceTest : public ::testing::Test {
public:
    virtual void SetUp() override
    {
        context_ = std::make_shared<Context>();
        bundleManager_ = context_->GetBundleManager().lock();
        listener_ = std::make_shared<CBConnectMock>();
        EXPECT_CALL(*listener_, CBConnected()).Times(1);
        seService_ = std::make_shared<SEService>();
    }

    virtual void TearDown() override
    {
        seService_ = std::shared_ptr<SEService>();
        bundleManager_ = std::shared_ptr<BundleManager>();
        context_ = std::shared_ptr<Context>();
    }

public:
    std::shared_ptr<SEService> seService_;
    std::shared_ptr<Context> context_;
    std::shared_ptr<BundleManager> bundleManager_;
    std::shared_ptr<CBConnectMock> listener_;
};

TEST_F(SEServiceTest, A001_003)
{
    EXPECT_TRUE(seService_->InitSEService(listener_));
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
}

TEST_F(SEServiceTest, A001_005)
{
    EXPECT_TRUE(seService_->InitSEService(listener_));
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    std::shared_ptr<CBConnectMock> listener = std::make_shared<CBConnectMock>();
    EXPECT_CALL(*listener, CBConnected()).Times(1);
    std::shared_ptr<SEService> seService = std::make_shared<SEService>();
    EXPECT_TRUE(seService->InitSEService(listener));
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
}
}  // namespace test
}  // namespace omapi
}  // namespace OHOS::se