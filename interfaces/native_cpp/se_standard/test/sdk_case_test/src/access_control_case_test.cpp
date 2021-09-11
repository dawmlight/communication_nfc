#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>
#include <thread>
#include <vector>

#include "cb_connect_mock.h"
#include "channel.h"
#include "reader.h"
#include "se_service.h"
#include "session.h"

using namespace testing;

namespace OHOS::se {
namespace omapi {
namespace test {
const std::string AID1 = {
    (char)0xA0, 0x00, 0x00, 0x03, 0x33, 0x00, 0x53, 0x44, 0x01, 0x21, 0x56, 0x11, 0x26, 0x24, 0x10, 0x00};
const std::string AID2 = {(char)0xA0, 0x00, 0x00, 0x01, 0x51, 0x43, 0x52, 0x53, 0x00};
const std::string AID3 = {(char)0xA0, 0x00, 0x00, 0x01, 0x51, 0x00, 0x00, 0x00};
const std::string AID4 = {
    (char)0xA0, 0x00, 0x00, 0x03, 0x33, 0x01, 0x01, 0x02, 0x00, 0x63, 0x48, 0x57, 0x50, 0x41, 0x59, 0x05};
const std::string AID5 = {(char)0xA0, 0x00, 0x00, 0x01, 0x51, 0x41, 0x43, 0x4C, 0x08};
const std::string GET_CHALLENGE = {0x00, (char)0x84, 0x00, 0x00, 0x04};
const std::string APDU_1 = {(char)0x80, (char)0xF2, 0x00, 0x00, 0x00};
const std::string APDU_2 = {(char)0x80, (char)0xCA, 0x00, 0x00, 0x00};
const std::string APDU_3 = {(char)0x80, (char)0xF2, 0x00, 0x02, 0x00};

class AccessControlTest : public ::testing::Test {
public:
    virtual void SetUp() override
    {
        // [Command-Delete-AR-DO] 80E2100002F100
        std::shared_ptr<CBConnectMock> listener = std::make_shared<CBConnectMock>();
        EXPECT_CALL(*listener, CBConnected()).Times(1);
        seService_ = std::make_shared<SEService>();
        EXPECT_TRUE(seService_->InitSEService(listener));
        std::this_thread::sleep_for(std::chrono::milliseconds(50));

        std::vector<std::shared_ptr<Reader>> readerList = seService_->GetReaders();
        ASSERT_TRUE(readerList.size() >= 1);
        reader_ = readerList.at(0);
        session_ = reader_->OpenSession();
        ASSERT_TRUE(session_);
    }

    virtual void TearDown() override
    {
        reader_->CloseSessions();
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        seService_ = std::shared_ptr<SEService>();
        reader_ = std::shared_ptr<Reader>();
        session_ = std::shared_ptr<Session>();
    }

public:
    std::shared_ptr<SEService> seService_;
    std::shared_ptr<Reader> reader_;
    std::shared_ptr<Session> session_;
};

TEST_F(AccessControlTest, APDU_ALWAYS)
{
    // Allowed any apdu command
    // eg. [Command-Store-AR-DO]
    // 80E2100036F034E232E1284F10A0000003330053440121561126241000C114A642A77ABD7D4F51BF9226CEAF891FCBB5B299B8E306D00101D10101
    std::shared_ptr<Channel> channel = session_->OpenBasicChannel(AID1);
    ASSERT_TRUE(channel);
    EXPECT_FALSE(channel->IsClosed());

    std::string res = channel->Transmit(GET_CHALLENGE);
    EXPECT_FALSE(res.empty());
}

TEST_F(AccessControlTest, APDU_NEVER)
{
    // Deny all apdu command
    // eg. [Command-Store-AR-DO]
    // 80E2100036F034E232E1284F10A0000003330053440121561126241000C114A642A77ABD7D4F51BF9226CEAF891FCBB5B299B8E306D00100D10100
    std::shared_ptr<Channel> channel = session_->OpenBasicChannel(AID1);
    ASSERT_TRUE(channel);
    EXPECT_FALSE(channel->IsClosed());

    std::string res = channel->Transmit(GET_CHALLENGE);
    EXPECT_TRUE(res.empty());
}

TEST_F(AccessControlTest, APDU_FILTER_1)
{
    // Only allowed apdu1
    // eg. [Command-Store-AR-DO]
    // 80E2100036F034E232E1214F09A00000015143525300C114A642A77ABD7D4F51BF9226CEAF891FCBB5B299B8E30DD00880F20000FFFFFFFFD10101
    std::shared_ptr<Channel> channel = session_->OpenBasicChannel(AID2);
    ASSERT_TRUE(channel);
    EXPECT_FALSE(channel->IsClosed());

    std::string res1 = channel->Transmit(APDU_1);
    EXPECT_FALSE(res1.empty());
    std::string res2 = channel->Transmit(APDU_2);
    EXPECT_TRUE(res2.empty());
    std::string res3 = channel->Transmit(APDU_3);
    EXPECT_TRUE(res3.empty());
}

TEST_F(AccessControlTest, APDU_FILTER_2)
{
    // Allowed apdu2 and apdu3, but not allowed apdu1
    // eg. [Command-Store-AR-DO]
    // 80E210003EF03CE23AE1214F09A00000015143525300C114A642A77ABD7D4F51BF9226CEAF891FCBB5B299B8E315D01080F20002FFFFFFFF80CA0000FFFF0000D10101
    std::shared_ptr<Channel> channel = session_->OpenBasicChannel(AID2);
    ASSERT_TRUE(channel);
    EXPECT_FALSE(channel->IsClosed());

    std::string res1 = channel->Transmit(APDU_1);
    EXPECT_TRUE(res1.empty());
    std::string res2 = channel->Transmit(APDU_2);
    EXPECT_FALSE(res2.empty());
    std::string res3 = channel->Transmit(APDU_3);
    EXPECT_FALSE(res3.empty());
}
}  // namespace test
}  // namespace omapi
}  // namespace OHOS::se