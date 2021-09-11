#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>
#include <thread>
#include <vector>

#include "cb_connect_mock.h"
#include "hilog/log.h"
#include "reader.h"
#include "session.h"

using namespace testing;

namespace OHOS::se {
namespace omapi {
namespace test {
const std::string AID_1 = "A0000003330101020063485750415905";
const std::string AID_2 = "A000000151000000";
const std::string AID_3 = "A00000015143525300";
const std::string AID_4 = "A0000003330053440121561126241000";
const std::string AID_5 = "A00000015141434C08";

class SessionTest : public ::testing::Test {
public:
    virtual void SetUp() override
    {
        std::shared_ptr<CBConnectMock> listener = std::make_shared<CBConnectMock>();
        EXPECT_CALL(*listener, CBConnected()).Times(1);
        seService_ = std::make_shared<SEService>();
        EXPECT_TRUE(seService_->InitSEService(listener));
        std::this_thread::sleep_for(std::chrono::milliseconds(50));

        std::string name = "channel_test";
        std::vector<std::shared_ptr<Reader>> readerList = seService_->GetReaders();
        EXPECT_TRUE(readerList.size() >= 1);
        reader_ = readerList.at(0);
    }

    virtual void TearDown() override
    {
        seService_ = std::shared_ptr<SEService>();
        reader_ = std::shared_ptr<Reader>();
    }

public:
    std::shared_ptr<SEService> seService_;
    std::shared_ptr<Reader> reader_;
};

TEST_F(SessionTest, A001_001)
{
    EXPECT_TRUE(reader_->IsSecureElementPresent());
    std::shared_ptr<Session> session = reader_->OpenSession();
    EXPECT_TRUE(session != nullptr);
    EXPECT_FALSE(session->IsClosed());
}

TEST_F(SessionTest, A001_002)
{
    EXPECT_TRUE(reader_->IsSecureElementPresent());
    std::shared_ptr<Session> session = reader_->OpenSession();
    EXPECT_TRUE(session != nullptr);
    EXPECT_FALSE(session->IsClosed());
    std::shared_ptr<Session> session2 = reader_->OpenSession();
    EXPECT_TRUE(session2 != nullptr);
    EXPECT_FALSE(session2->IsClosed());
    // Two instances
    EXPECT_TRUE(session != session2);
}

TEST_F(SessionTest, B002_001)
{
    EXPECT_TRUE(reader_->IsSecureElementPresent());
    seService_->Shutdown();
    EXPECT_FALSE(reader_->IsSecureElementPresent());
    EXPECT_TRUE(!reader_->GetName().empty());
    std::shared_ptr<Session> session = reader_->OpenSession();
    EXPECT_FALSE(session);
}

TEST_F(SessionTest, B002_002)
{
    EXPECT_TRUE(reader_->IsSecureElementPresent());
    std::shared_ptr<Session> session = reader_->OpenSession();
    EXPECT_TRUE(session != nullptr);
    reader_->CloseSessions();
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    EXPECT_TRUE(session->IsClosed());
}

TEST_F(SessionTest, B002_003)
{
    EXPECT_TRUE(reader_->IsSecureElementPresent());
    std::shared_ptr<Session> session = reader_->OpenSession();
    EXPECT_TRUE(session != nullptr);
    reader_->CloseSessions();
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    std::shared_ptr<Session> session2 = reader_->OpenSession();
    EXPECT_TRUE(session2 != nullptr);
    EXPECT_FALSE(session2->IsClosed());
}

TEST_F(SessionTest, B002_004)
{
    EXPECT_TRUE(reader_->IsSecureElementPresent());
    EXPECT_FALSE(reader_->GetName().empty());
    std::shared_ptr<Session> session = reader_->OpenSession();
    EXPECT_TRUE(session != nullptr);
    EXPECT_FALSE(session->IsClosed());
    reader_->CloseSessions();
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    EXPECT_TRUE(session->IsClosed());
    session->CloseChannels();
}
}  // namespace test
}  // namespace omapi
}  // namespace OHOS::se