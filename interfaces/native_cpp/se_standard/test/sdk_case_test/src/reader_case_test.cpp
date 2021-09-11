#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>
#include <thread>

#include "cb_connect_mock.h"
#include "isecure_element_reader.h"
#include "isecure_element_session.h"
#include "loghelper.h"
#include "reader.h"
#include "se_service.h"

using namespace testing;

namespace OHOS::se {
namespace omapi {
namespace test {
class ReaderTest : public ::testing::Test {
public:
    virtual void SetUp() override
    {
        InfoLog("==ReaderTest::SetUp==");
        listener_ = std::make_shared<CBConnectMock>();
        seService_ = std::make_shared<SEService>();
    }

    virtual void TearDown() override
    {
        InfoLog("==ReaderTest::TearDown==");
        seService_ = std::shared_ptr<SEService>();
        listener_ = std::shared_ptr<CBConnectMock>();
    }

public:
    std::shared_ptr<SEService> seService_;
    std::shared_ptr<CBConnectMock> listener_;
};

TEST_F(ReaderTest, A001_001)
{
    EXPECT_CALL(*listener_, CBConnected()).Times(1);
    EXPECT_TRUE(seService_->InitSEService(listener_));
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    EXPECT_TRUE(seService_->IsConnected());

    std::vector<std::shared_ptr<Reader>> readers = seService_->GetReaders();
    EXPECT_TRUE(readers.size() > 0);
}

/**
 * @brief Do not start the st service, but start/restart the se backend service
 */
TEST_F(ReaderTest, A001_002)
{
    EXPECT_CALL(*listener_, CBConnected()).Times(1);
    EXPECT_TRUE(seService_->InitSEService(listener_));
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    EXPECT_TRUE(seService_->IsConnected());

    std::vector<std::shared_ptr<Reader>> readers = seService_->GetReaders();
    EXPECT_TRUE(readers.size() == 0);
}

TEST_F(ReaderTest, A001_003)
{
    EXPECT_CALL(*listener_, CBConnected()).Times(1);
    EXPECT_TRUE(seService_->InitSEService(listener_));
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    EXPECT_TRUE(seService_->IsConnected());

    std::vector<std::shared_ptr<Reader>> readers = seService_->GetReaders();
    EXPECT_TRUE(readers.size() > 0);
    std::vector<std::shared_ptr<Reader>> readers2 = seService_->GetReaders();
    EXPECT_TRUE(readers2.size() > 0);
    EXPECT_TRUE(readers.size() == readers2.size());
    EXPECT_TRUE(readers.at(0) == readers2.at(0));
}
/**
 * @brief Do not start se backend service
 */
TEST_F(ReaderTest, A001_004)
{
    EXPECT_FALSE(seService_->InitSEService(listener_));
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    EXPECT_FALSE(seService_->IsConnected());

    std::vector<std::shared_ptr<Reader>> readers = seService_->GetReaders();
    EXPECT_TRUE(readers.size() == 0);
}
}  // namespace test
}  // namespace omapi
}  // namespace OHOS::se