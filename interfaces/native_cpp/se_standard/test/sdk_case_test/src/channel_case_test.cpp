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
// AID1 apdu-always
const std::string AID1 = {
    (char)0xA0, 0x00, 0x00, 0x03, 0x33, 0x00, 0x53, 0x44, 0x01, 0x21, 0x56, 0x11, 0x26, 0x24, 0x10, 0x00};
// AID2 apdu-filter, allowed apdu1
const std::string AID2 = {(char)0xA0, 0x00, 0x00, 0x01, 0x51, 0x43, 0x52, 0x53, 0x00};
const std::string AID3 = {(char)0xA0, 0x00, 0x00, 0x01, 0x51, 0x00, 0x00, 0x00};
const std::string AID4 = {
    (char)0xA0, 0x00, 0x00, 0x03, 0x33, 0x01, 0x01, 0x02, 0x00, 0x63, 0x48, 0x57, 0x50, 0x41, 0x59, 0x05};
const std::string AID5 = {(char)0xA0, 0x00, 0x00, 0x01, 0x51, 0x41, 0x43, 0x4C, 0x08};
const std::string AID_WRONG = {0xA0, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
const std::string AID_EMPTY = {};
const std::string GET_CHALLENGE = {0x00, (char)0x84, 0x00, 0x00, 0x04};
const std::string SELECT_CMD = {0x00, (char)0xA4, 0x04, 0x00};
const std::string MANAGE_CHANNEL_CMD = {0x00, 0x70, 0x00, 0x00, 0x01};
const std::string APDU1 = {(char)0x80, (char)0xF2, 0x00, 0x00, 0x00};

class ChannelTest : public ::testing::Test {
public:
    virtual void SetUp() override
    {
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

TEST_F(ChannelTest, A001_001)
{
    std::shared_ptr<Channel> channel = session_->OpenBasicChannel(AID1, 0x00);
    ASSERT_TRUE(channel);
    EXPECT_FALSE(channel->IsClosed());
    std::string res = channel->Transmit(GET_CHALLENGE);
    EXPECT_FALSE(res.empty());
}

TEST_F(ChannelTest, A001_002)
{
    std::shared_ptr<Channel> channel = session_->OpenBasicChannel(AID_EMPTY, 0x00);
    ASSERT_TRUE(channel);
    EXPECT_FALSE(channel->IsClosed());
}

TEST_F(ChannelTest, A001_003)
{
    std::shared_ptr<Channel> channel = session_->OpenBasicChannel(AID1);
    ASSERT_TRUE(channel);
    EXPECT_FALSE(channel->IsClosed());
    std::string res = channel->Transmit(GET_CHALLENGE);
    EXPECT_FALSE(res.empty());
}

TEST_F(ChannelTest, A001_004)
{
    // Need at least two readers
    // EXPECT_TRUE(readerList.size() >= 2);
    // std::shared_ptr<Channel> channel = session_->OpenBasicChannel(AID1);
    // EXPECT_TRUE(channel);
    // EXPECT_FALSE(channel->IsClosed());
    // std::string res = channel->Transmit(GET_CHALLENGE);
    // EXPECT_FALSE(res.empty());

    // std::shared_ptr<Reader> reader2 = readerList.at(1);
    // std::shared_ptr<Session> session2 = reader2->OpenSession();
    // EXPECT_TRUE(session2 != nullptr);
    // std::shared_ptr<Channel> channel2 = session2->OpenBasicChannel(AID1);
    // EXPECT_TRUE(channel2);
    // EXPECT_FALSE(channel2->IsClosed());
    // std::string res2 = channel2->2Transmit(GET_CHALLENGE);
    // EXPECT_FALSE(res2.empty());
    // session2->CloseClannels();
    // std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

TEST_F(ChannelTest, B002_001)
{
    std::shared_ptr<Channel> channel = session_->OpenBasicChannel(AID_WRONG, 0x00);
    EXPECT_FALSE(channel);
}

TEST_F(ChannelTest, B002_002)
{
    seService_->Shutdown();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    std::shared_ptr<Channel> channel = session_->OpenBasicChannel(AID1, 0x00);
    EXPECT_FALSE(channel);
}

TEST_F(ChannelTest, B002_003)
{
    std::shared_ptr<Channel> channel = session_->OpenBasicChannel(AID1, 0x00);
    ASSERT_TRUE(channel);
    EXPECT_FALSE(channel->IsClosed());
    std::string res = channel->Transmit(GET_CHALLENGE);
    EXPECT_FALSE(res.empty());

    std::shared_ptr<Channel> channel2 = session_->OpenBasicChannel(AID1, 0x00);
    EXPECT_FALSE(channel2);
}

TEST_F(ChannelTest, B002_004)
{
    std::shared_ptr<Channel> channel = session_->OpenBasicChannel(AID1, 0x00);
    ASSERT_TRUE(channel);
    EXPECT_FALSE(channel->IsClosed());
    std::string res = channel->Transmit(GET_CHALLENGE);
    EXPECT_FALSE(res.empty());

    std::shared_ptr<Channel> channel2 = session_->OpenBasicChannel(AID2, 0x00);
    EXPECT_FALSE(channel2);
}

TEST_F(ChannelTest, B002_005)
{
    std::shared_ptr<Channel> channel = session_->OpenBasicChannel(AID1, 0x00);
    ASSERT_TRUE(channel);
    EXPECT_FALSE(channel->IsClosed());
    std::string res = channel->Transmit(GET_CHALLENGE);
    EXPECT_FALSE(res.empty());

    std::shared_ptr<Channel> channel2 = session_->OpenBasicChannel(AID2, 0x00);
    EXPECT_FALSE(channel2);

    EXPECT_FALSE(channel->IsClosed());
    std::string res2 = channel->Transmit(GET_CHALLENGE);
    EXPECT_FALSE(res.empty());
}

TEST_F(ChannelTest, C003_001)
{
    std::shared_ptr<Channel> channel = session_->OpenBasicChannel(AID1, 0x00);
    ASSERT_TRUE(channel);
    EXPECT_FALSE(channel->IsClosed());
    std::string res = channel->Transmit(GET_CHALLENGE);
    EXPECT_FALSE(res.empty());

    channel->Close();
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    EXPECT_TRUE(channel->IsClosed());
}

TEST_F(ChannelTest, C003_002)
{
    std::shared_ptr<Channel> channel = session_->OpenBasicChannel(AID1, 0x00);
    ASSERT_TRUE(channel);
    EXPECT_FALSE(channel->IsClosed());
    std::string res = channel->Transmit(GET_CHALLENGE);
    EXPECT_FALSE(res.empty());

    channel->Close();
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    EXPECT_TRUE(channel->IsClosed());
    channel->Close();
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    EXPECT_TRUE(channel->IsClosed());
}

TEST_F(ChannelTest, C003_003)
{
    std::shared_ptr<Channel> channel = session_->OpenBasicChannel(AID1, 0x00);
    ASSERT_TRUE(channel);
    EXPECT_FALSE(channel->IsClosed());
    std::string res = channel->Transmit(GET_CHALLENGE);
    EXPECT_FALSE(res.empty());

    channel->Close();
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    EXPECT_TRUE(channel->IsClosed());

    std::shared_ptr<Channel> channel2 = session_->OpenBasicChannel(AID1, 0x00);
    ASSERT_TRUE(channel2);
    EXPECT_FALSE(channel2->IsClosed());
}

TEST_F(ChannelTest, D004_001)
{
    std::shared_ptr<Channel> channel = session_->OpenLogicalChannel(AID1, 0x00);
    ASSERT_TRUE(channel);
    EXPECT_FALSE(channel->IsClosed());
    std::string res = channel->Transmit(GET_CHALLENGE);
    EXPECT_FALSE(res.empty());
}

TEST_F(ChannelTest, D004_002)
{
    std::shared_ptr<Channel> channel = session_->OpenLogicalChannel(AID1, 0x00);
    ASSERT_TRUE(channel);
    EXPECT_FALSE(channel->IsClosed());
    std::string res = channel->Transmit(GET_CHALLENGE);
    EXPECT_FALSE(res.empty());

    std::shared_ptr<Channel> channel2 = session_->OpenLogicalChannel(AID2, 0x00);
    ASSERT_TRUE(channel2);
    EXPECT_FALSE(channel2->IsClosed());
    std::string res2 = channel2->Transmit(APDU1);
    EXPECT_FALSE(res2.empty());
}

TEST_F(ChannelTest, D004_003)
{
    std::shared_ptr<Channel> channel = session_->OpenLogicalChannel(AID1, 0x00);
    ASSERT_TRUE(channel);
    EXPECT_FALSE(channel->IsClosed());
    std::string res = channel->Transmit(GET_CHALLENGE);
    EXPECT_FALSE(res.empty());

    std::shared_ptr<Channel> channel2 = session_->OpenLogicalChannel(AID1, 0x00);
    ASSERT_TRUE(channel2);
    EXPECT_FALSE(channel2->IsClosed());
    std::string res2 = channel2->Transmit(GET_CHALLENGE);
    EXPECT_FALSE(res2.empty());
}

TEST_F(ChannelTest, D004_004)
{
    std::shared_ptr<Channel> channel = session_->OpenLogicalChannel(AID1, 0x00);
    ASSERT_TRUE(channel);
    EXPECT_FALSE(channel->IsClosed());
    std::string res = channel->Transmit(GET_CHALLENGE);
    EXPECT_FALSE(res.empty());

    std::shared_ptr<Session> session2 = reader_->OpenSession();
    ASSERT_TRUE(session2);
    std::shared_ptr<Channel> channel2 = session2->OpenLogicalChannel(AID1, 0x00);
    ASSERT_TRUE(channel2);
    EXPECT_FALSE(channel2->IsClosed());
    std::string res2 = channel2->Transmit(GET_CHALLENGE);
    EXPECT_FALSE(res2.empty());
}

TEST_F(ChannelTest, D004_005)
{
    std::shared_ptr<Channel> channel = session_->OpenLogicalChannel(AID_EMPTY, 0x00);
    ASSERT_TRUE(channel);
    EXPECT_FALSE(channel->IsClosed());
}

TEST_F(ChannelTest, D004_006)
{
    std::shared_ptr<Channel> channel = session_->OpenBasicChannel(AID1, 0x00);
    ASSERT_TRUE(channel);
    EXPECT_FALSE(channel->IsClosed());
    std::string res = channel->Transmit(GET_CHALLENGE);
    EXPECT_FALSE(res.empty());

    std::shared_ptr<Channel> channel2 = session_->OpenLogicalChannel(AID1, 0x00);
    ASSERT_TRUE(channel2);
    EXPECT_FALSE(channel2->IsClosed());
    std::string res2 = channel2->Transmit(GET_CHALLENGE);
    EXPECT_FALSE(res2.empty());
}

TEST_F(ChannelTest, D004_007)
{
    std::shared_ptr<Channel> channel = session_->OpenBasicChannel(AID1, 0x00);
    ASSERT_TRUE(channel);
    EXPECT_FALSE(channel->IsClosed());
    std::string res = channel->Transmit(GET_CHALLENGE);
    EXPECT_FALSE(res.empty());

    std::shared_ptr<Channel> channel2 = session_->OpenLogicalChannel(AID2, 0x00);
    ASSERT_TRUE(channel2);
    EXPECT_FALSE(channel2->IsClosed());
    std::string res2 = channel2->Transmit(APDU1);
    EXPECT_FALSE(res2.empty());
}

TEST_F(ChannelTest, D004_008)
{
    std::shared_ptr<Channel> channel = session_->OpenLogicalChannel(AID1, 0x00);
    ASSERT_TRUE(channel);
    EXPECT_FALSE(channel->IsClosed());
    std::string res = channel->Transmit(GET_CHALLENGE);
    EXPECT_FALSE(res.empty());

    std::shared_ptr<Channel> channel2 = session_->OpenBasicChannel(AID2, 0x00);
    ASSERT_TRUE(channel2);
    EXPECT_FALSE(channel2->IsClosed());
    std::string res2 = channel2->Transmit(APDU1);
    EXPECT_FALSE(res2.empty());
}

TEST_F(ChannelTest, E005_001)
{
    std::shared_ptr<Channel> channel = session_->OpenLogicalChannel(AID_WRONG, 0x00);
    EXPECT_FALSE(channel);

    std::shared_ptr<Channel> channel2 = session_->OpenLogicalChannel(AID1, 0x00);
    ASSERT_TRUE(channel2);
    EXPECT_FALSE(channel2->IsClosed());
    std::string res = channel2->Transmit(GET_CHALLENGE);
    EXPECT_FALSE(res.empty());
}

TEST_F(ChannelTest, E005_002)
{
    std::shared_ptr<Channel> channel = session_->OpenLogicalChannel(AID1, 0x00);
    ASSERT_TRUE(channel);
    EXPECT_FALSE(channel->IsClosed());
    std::string res = channel->Transmit(GET_CHALLENGE);
    EXPECT_FALSE(res.empty());

    seService_->Shutdown();
    std::shared_ptr<Channel> channel2 = session_->OpenLogicalChannel(AID1, 0x00);
    EXPECT_FALSE(channel2);
}

TEST_F(ChannelTest, E005_003)
{
    std::shared_ptr<Channel> channel;
    while (1) {
        // open logical channel until limited
        std::shared_ptr<Channel> channelTemp = session_->OpenLogicalChannel(AID1, 0x00);
        if (channelTemp) {
            channel = channelTemp;
        } else {
            break;
        }
    }
    ASSERT_TRUE(channel);
    channel->Close();
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    std::shared_ptr<Channel> channel2 = session_->OpenLogicalChannel(AID1, 0x00);
    ASSERT_TRUE(channel2);
    EXPECT_FALSE(channel2->IsClosed());
}

TEST_F(ChannelTest, E005_004)
{
    // Reader should be UICC
    // std::shared_ptr<Channel> channel = session_->OpenLogicalChannel(AID_EMPTY, 0x00);
    // EXPECT_FALSE(channel);
}

TEST_F(ChannelTest, F006_001)
{
    std::shared_ptr<Channel> channel = session_->OpenLogicalChannel(AID1, 0x00);
    ASSERT_TRUE(channel);
    EXPECT_FALSE(channel->IsClosed());
    std::string res = channel->Transmit(GET_CHALLENGE);
    EXPECT_FALSE(res.empty());

    channel->Close();
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    EXPECT_TRUE(channel->IsClosed());
}

TEST_F(ChannelTest, F006_002)
{
    std::shared_ptr<Channel> channel = session_->OpenLogicalChannel(AID1, 0x00);
    ASSERT_TRUE(channel);
    EXPECT_FALSE(channel->IsClosed());
    std::string res = channel->Transmit(GET_CHALLENGE);
    EXPECT_FALSE(res.empty());

    channel->Close();
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    EXPECT_TRUE(channel->IsClosed());

    channel->Close();
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    EXPECT_TRUE(channel->IsClosed());
}

TEST_F(ChannelTest, F006_003)
{
    std::shared_ptr<Channel> channel = session_->OpenLogicalChannel(AID1, 0x00);
    ASSERT_TRUE(channel);
    EXPECT_FALSE(channel->IsClosed());
    std::string res = channel->Transmit(GET_CHALLENGE);
    EXPECT_FALSE(res.empty());

    std::shared_ptr<Channel> channel2 = session_->OpenLogicalChannel(AID1, 0x00);
    ASSERT_TRUE(channel2);
    EXPECT_FALSE(channel2->IsClosed());
    std::string res2 = channel2->Transmit(GET_CHALLENGE);
    EXPECT_FALSE(res2.empty());

    std::shared_ptr<Channel> channel3 = session_->OpenLogicalChannel(AID1, 0x00);
    ASSERT_TRUE(channel3);
    EXPECT_FALSE(channel3->IsClosed());
    std::string res3 = channel3->Transmit(GET_CHALLENGE);
    EXPECT_FALSE(res3.empty());

    channel->Close();
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    EXPECT_TRUE(channel->IsClosed());

    channel2->Close();
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    EXPECT_TRUE(channel2->IsClosed());

    channel3->Close();
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    EXPECT_TRUE(channel3->IsClosed());
}

TEST_F(ChannelTest, F006_004)
{
    std::shared_ptr<Channel> channel = session_->OpenLogicalChannel(AID1, 0x00);
    ASSERT_TRUE(channel);
    EXPECT_FALSE(channel->IsClosed());
    std::string res = channel->Transmit(GET_CHALLENGE);
    EXPECT_FALSE(res.empty());

    channel->Close();
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    EXPECT_TRUE(channel->IsClosed());

    std::shared_ptr<Channel> channel2 = session_->OpenLogicalChannel(AID1, 0x00);
    ASSERT_TRUE(channel2);
    EXPECT_FALSE(channel2->IsClosed());
    std::string res2 = channel2->Transmit(GET_CHALLENGE);
    EXPECT_FALSE(res2.empty());
}

TEST_F(ChannelTest, G007_001)
{
    std::shared_ptr<Channel> channel = session_->OpenBasicChannel(AID1, 0x00);
    ASSERT_TRUE(channel);
    EXPECT_FALSE(channel->IsClosed());

    std::shared_ptr<Channel> channel2 = session_->OpenLogicalChannel(AID1, 0x00);
    ASSERT_TRUE(channel2);
    EXPECT_FALSE(channel2->IsClosed());

    session_->CloseChannels();
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    EXPECT_TRUE(channel->IsClosed());
    EXPECT_TRUE(channel2->IsClosed());
}

TEST_F(ChannelTest, G007_002)
{
    std::shared_ptr<Channel> channel = session_->OpenBasicChannel(AID1, 0x00);
    ASSERT_TRUE(channel);
    EXPECT_FALSE(channel->IsClosed());

    std::shared_ptr<Channel> channel2 = session_->OpenLogicalChannel(AID1, 0x00);
    ASSERT_TRUE(channel2);
    EXPECT_FALSE(channel2->IsClosed());

    std::shared_ptr<Channel> channel3 = session_->OpenLogicalChannel(AID1, 0x00);
    ASSERT_TRUE(channel3);
    EXPECT_FALSE(channel3->IsClosed());

    std::shared_ptr<Channel> channel4 = session_->OpenLogicalChannel(AID1, 0x00);
    ASSERT_TRUE(channel4);
    EXPECT_FALSE(channel4->IsClosed());

    std::shared_ptr<Channel> channel5 = session_->OpenLogicalChannel(AID1, 0x00);
    EXPECT_FALSE(channel5);

    session_->CloseChannels();
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    std::shared_ptr<Channel> channel6 = session_->OpenLogicalChannel(AID1, 0x00);
    ASSERT_TRUE(channel6);
    EXPECT_FALSE(channel6->IsClosed());
}

TEST_F(ChannelTest, G007_003)
{
    std::shared_ptr<Channel> channel = session_->OpenBasicChannel(AID1, 0x00);
    ASSERT_TRUE(channel);
    EXPECT_FALSE(channel->IsClosed());

    std::shared_ptr<Channel> channel2 = session_->OpenLogicalChannel(AID1, 0x00);
    ASSERT_TRUE(channel2);
    EXPECT_FALSE(channel2->IsClosed());

    session_->CloseChannels();
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    EXPECT_TRUE(channel->IsClosed());
    EXPECT_TRUE(channel2->IsClosed());

    session_->CloseChannels();
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    EXPECT_TRUE(channel->IsClosed());
    EXPECT_TRUE(channel2->IsClosed());
}

TEST_F(ChannelTest, H008_001)
{
    std::shared_ptr<Channel> channel = session_->OpenBasicChannel(AID1, 0x00);
    ASSERT_TRUE(channel);
    EXPECT_FALSE(channel->IsClosed());
    std::string res = channel->Transmit(GET_CHALLENGE);
    EXPECT_FALSE(res.empty());
}

TEST_F(ChannelTest, H008_002)
{
    std::shared_ptr<Channel> channel = session_->OpenBasicChannel(AID2, 0x00);
    ASSERT_TRUE(channel);
    EXPECT_FALSE(channel->IsClosed());
    std::string res = channel->Transmit(APDU1);
    EXPECT_FALSE(res.empty());

    EXPECT_TRUE(channel->SelectNext());
    std::string res2 = channel->GetSelectResponse();
    EXPECT_FALSE(res2.empty());
    std::string res3 = channel->Transmit(APDU1);
    EXPECT_FALSE(res3.empty());
}

TEST_F(ChannelTest, H008_003)
{
    std::shared_ptr<Channel> channel = session_->OpenBasicChannel(AID1, 0x00);
    ASSERT_TRUE(channel);
    EXPECT_FALSE(channel->IsClosed());
    std::string res = channel->Transmit(GET_CHALLENGE);
    EXPECT_FALSE(res.empty());

    EXPECT_FALSE(channel->SelectNext());
    std::string res2 = channel->Transmit(GET_CHALLENGE);
    EXPECT_FALSE(res2.empty());
}

TEST_F(ChannelTest, H008_004)
{
    std::shared_ptr<Channel> channel = session_->OpenBasicChannel(AID_WRONG, 0x00);
    EXPECT_FALSE(channel);
}

TEST_F(ChannelTest, H008_005)
{
    std::shared_ptr<Channel> channel = session_->OpenBasicChannel(AID1, 0x01);
    EXPECT_FALSE(channel);
}

TEST_F(ChannelTest, I009_001)
{
    std::shared_ptr<Channel> channel = session_->OpenLogicalChannel(AID1, 0x00);
    ASSERT_TRUE(channel);
    EXPECT_FALSE(channel->IsClosed());
    std::string res = channel->Transmit(GET_CHALLENGE);
    EXPECT_FALSE(res.empty());
}

TEST_F(ChannelTest, I009_002)
{
    std::shared_ptr<Channel> channel = session_->OpenLogicalChannel(AID2, 0x00);
    ASSERT_TRUE(channel);
    EXPECT_FALSE(channel->IsClosed());
    std::string res = channel->Transmit(APDU1);
    EXPECT_FALSE(res.empty());

    EXPECT_TRUE(channel->SelectNext());
    std::string res2 = channel->GetSelectResponse();
    EXPECT_FALSE(res2.empty());
    std::string res3 = channel->Transmit(APDU1);
    EXPECT_FALSE(res3.empty());
}

TEST_F(ChannelTest, I009_003)
{
    std::shared_ptr<Channel> channel = session_->OpenLogicalChannel(AID1, 0x00);
    ASSERT_TRUE(channel);
    EXPECT_FALSE(channel->IsClosed());
    std::string res = channel->Transmit(GET_CHALLENGE);
    EXPECT_FALSE(res.empty());

    EXPECT_FALSE(channel->SelectNext());
    std::string res2 = channel->Transmit(GET_CHALLENGE);
    EXPECT_FALSE(res2.empty());
}

TEST_F(ChannelTest, I009_004)
{
    std::shared_ptr<Channel> channel = session_->OpenLogicalChannel(AID_WRONG, 0x00);
    EXPECT_FALSE(channel);
}

TEST_F(ChannelTest, I009_005)
{
    std::shared_ptr<Channel> channel = session_->OpenLogicalChannel(AID1, 0x01);
    EXPECT_FALSE(channel);
}

TEST_F(ChannelTest, I009_006)
{
    std::shared_ptr<Channel> channel = session_->OpenBasicChannel(AID1, 0x00);
    ASSERT_TRUE(channel);
    EXPECT_FALSE(channel->SelectNext());
    std::string res = channel->GetSelectResponse();
    EXPECT_FALSE(res.empty());

    std::shared_ptr<Channel> channel2 = session_->OpenLogicalChannel(AID1, 0x00);
    ASSERT_TRUE(channel2);
    EXPECT_FALSE(channel2->SelectNext());
    std::string res2 = channel2->GetSelectResponse();
    EXPECT_FALSE(res2.empty());
}

TEST_F(ChannelTest, J010_001)
{
    std::shared_ptr<Channel> channel = session_->OpenBasicChannel(AID1, 0x00);
    ASSERT_TRUE(channel);
    std::string res1 = channel->GetSelectResponse();
    EXPECT_FALSE(res1.empty());
    std::string res2 = channel->Transmit(GET_CHALLENGE);
    EXPECT_FALSE(res2.empty());
}

TEST_F(ChannelTest, J010_002)
{
    std::shared_ptr<Channel> channel = session_->OpenBasicChannel(AID1, 0x00);
    ASSERT_TRUE(channel);
    std::string res1 = channel->Transmit(GET_CHALLENGE);
    EXPECT_FALSE(res1.empty());
    channel->Close();
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    EXPECT_TRUE(channel->IsClosed());
    std::string res2 = channel->GetSelectResponse();
    EXPECT_TRUE(res2.empty());
}

TEST_F(ChannelTest, J010_003)
{
    std::shared_ptr<Channel> channel = session_->OpenBasicChannel(AID_EMPTY, 0x00);
    ASSERT_TRUE(channel);
    EXPECT_FALSE(channel->IsClosed());
    std::string res = channel->GetSelectResponse();
    EXPECT_TRUE(res.empty());
}

TEST_F(ChannelTest, J010_004)
{
    std::shared_ptr<Channel> channel = session_->OpenLogicalChannel(AID1, 0x00);
    ASSERT_TRUE(channel);
    std::string res1 = channel->Transmit(GET_CHALLENGE);
    EXPECT_FALSE(res1.empty());
    std::string res2 = channel->GetSelectResponse();
    EXPECT_FALSE(res2.empty());
}

TEST_F(ChannelTest, J010_005)
{
    std::shared_ptr<Channel> channel = session_->OpenLogicalChannel(AID1, 0x00);
    ASSERT_TRUE(channel);
    std::string res1 = channel->Transmit(GET_CHALLENGE);
    EXPECT_FALSE(res1.empty());
    channel->Close();
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    EXPECT_TRUE(channel->IsClosed());
    std::string res2 = channel->GetSelectResponse();
    EXPECT_TRUE(res2.empty());
}

TEST_F(ChannelTest, K011_001)
{
    std::string cmd = SELECT_CMD;
    cmd.append(1, (char)AID1.length());
    cmd.append(AID1);

    std::shared_ptr<Channel> channel = session_->OpenBasicChannel(AID1, 0x00);
    ASSERT_TRUE(channel);
    EXPECT_FALSE(channel->IsClosed());
    std::string res = channel->Transmit(cmd);
    EXPECT_TRUE(res.empty());

    std::shared_ptr<Channel> channel2 = session_->OpenLogicalChannel(AID1, 0x00);
    ASSERT_TRUE(channel2);
    EXPECT_FALSE(channel2->IsClosed());
    std::string res2 = channel->Transmit(cmd);
    EXPECT_TRUE(res2.empty());
}

TEST_F(ChannelTest, K011_002)
{
    std::shared_ptr<Channel> channel = session_->OpenBasicChannel(AID1, 0x00);
    ASSERT_TRUE(channel);
    EXPECT_FALSE(channel->IsClosed());
    std::string res = channel->Transmit(MANAGE_CHANNEL_CMD);
    EXPECT_TRUE(res.empty());

    std::shared_ptr<Channel> channel2 = session_->OpenLogicalChannel(AID1, 0x00);
    ASSERT_TRUE(channel2);
    EXPECT_FALSE(channel2->IsClosed());
    std::string res2 = channel->Transmit(MANAGE_CHANNEL_CMD);
    EXPECT_TRUE(res2.empty());
}

TEST_F(ChannelTest, L012_001)
{
    std::shared_ptr<Channel> channel = session_->OpenLogicalChannel(AID1, 0x00);
    ASSERT_TRUE(channel);
    std::string res = channel->Transmit(GET_CHALLENGE);
    EXPECT_FALSE(res.empty());

    std::shared_ptr<CBConnectMock> listener = std::make_shared<CBConnectMock>();
    EXPECT_CALL(*listener, CBConnected()).Times(1);
    std::shared_ptr<SEService> seService = std::make_shared<SEService>();
    EXPECT_TRUE(seService->InitSEService(listener));
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    std::vector<std::shared_ptr<Reader>> readerList = seService->GetReaders();
    EXPECT_TRUE(readerList.size() >= 1);
    std::shared_ptr<Session> session = readerList.at(0)->OpenSession();
    EXPECT_TRUE(session != nullptr);
    std::shared_ptr<Channel> channel2 = session_->OpenLogicalChannel(AID1, 0x00);
    ASSERT_TRUE(channel2);
    res = channel->Transmit(GET_CHALLENGE);
    EXPECT_FALSE(res.empty());
}

TEST_F(ChannelTest, L012_002)
{
    // Need at least two readers
    // std::shared_ptr<Channel> channel = session_->OpenLogicalChannel(AID1, 0x00);
    // ASSERT_TRUE(channel);
    // std::string res = channel->Transmit(GET_CHALLENGE);
    // EXPECT_FALSE(res.empty());

    // std::shared_ptr<CBConnectMock> listener = std::make_shared<CBConnectMock>();
    // EXPECT_CALL(*listener, CBConnected()).Times(1);
    // std::shared_ptr<SEService> seService = std::make_shared<SEService>();
    // EXPECT_TRUE(seService->InitSEService(listener));
    // std::this_thread::sleep_for(std::chrono::milliseconds(50));
    // std::vector<std::shared_ptr<Reader>> readerList = seService->GetReaders();
    // EXPECT_TRUE(readerList.size() >= 1);
    // std::shared_ptr<Session> session = readerList.at(1)->OpenSession();
    // EXPECT_TRUE(session != nullptr);
    // std::shared_ptr<Channel> channel2 = session_->OpenLogicalChannel(AID1, 0x00);
    // ASSERT_TRUE(channel2);
    // res = channel->Transmit(GET_CHALLENGE);
    // EXPECT_FALSE(res.empty());
}
}  // namespace test
}  // namespace omapi
}  // namespace OHOS::se