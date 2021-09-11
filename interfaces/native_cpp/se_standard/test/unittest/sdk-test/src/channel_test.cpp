#include "channel.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>
#include <thread>

#include "connected_listener_mock.h"
#include "isecure_element_channel_mock.h"
#include "isecure_element_reader_mock.h"
#include "isecure_element_service_mock.h"
#include "isecure_element_session_mock.h"
#include "reader.h"
#include "se_service.h"
#include "session.h"

using namespace testing;

namespace OHOS::se {
namespace omapi {
class ChannelTest : public ::testing::Test {
public:
    virtual void SetUp() override
    {
        std::shared_ptr<ConnectedListenerMock> listener = std::make_shared<ConnectedListenerMock>();
        seService = std::make_shared<SEService>(listener);
        seService->InitSEService();

        std::string name = "channel_test";
        std::shared_ptr<ISecureElementReader> iReader = std::make_shared<ISecureElementReaderMock>();
        reader = std::make_shared<Reader>(seService, name, iReader);

        std::shared_ptr<ISecureElementSession> iSession = std::make_shared<ISecureElementSessionMock>();
        session = std::make_shared<Session>(seService, iSession, reader);

        iChannel = std::make_shared<ISecureElementChannelMock>();
        channel = std::make_shared<Channel>(seService, session, iChannel);
    }

    virtual void TearDown() override
    {
        channel = std::shared_ptr<Channel>();
    }

public:
    std::shared_ptr<SEService> seService;
    std::shared_ptr<Reader> reader;
    std::shared_ptr<Session> session;
    std::shared_ptr<Channel> channel;
    std::shared_ptr<ISecureElementChannelMock> iChannel;
};

TEST_F(ChannelTest, Close_Test)
{
    channel->Close();
    channel->GetSession();
}

TEST_F(ChannelTest, IsClosed_Test)
{
    EXPECT_FALSE(channel->IsClosed());
}

TEST_F(ChannelTest, IsBasicChannel_Test)
{
    EXPECT_FALSE(channel->IsBasicChannel());
}

TEST_F(ChannelTest, GetSelectResponse_Test)
{
    std::string res = channel->GetSelectResponse();
    EXPECT_TRUE(res.empty());
}

TEST_F(ChannelTest, Transmit_Test)
{
    std::string command = "1929";
    std::string res = channel->Transmit(command);
    EXPECT_TRUE(res.empty());
}

TEST_F(ChannelTest, SelectNext_Test)
{
    EXPECT_FALSE(channel->SelectNext());
}

TEST_F(ChannelTest, ChannelTest_OK)
{
    // sptr<IRemoteObject> connectionCallback = new IPCObjectStub(u"se service listener unit test");

    // std::shared_ptr<ISecureElementServiceMock> sem = std::make_shared<ISecureElementServiceMock>();
    // EXPECT_CALL(*sem, AsObject()).WillRepeatedly(testing::Return(connectionCallback));
    // seService->GetConnectCallback()->OnConnectedSystemAbility(sem->AsObject());
    EXPECT_TRUE(seService->IsConnected());

    std::shared_ptr<Channel> tC = std::make_shared<Channel>(seService, session, iChannel);
    std::string rsp = "success";

    EXPECT_CALL(*iChannel, IsBasicChannel()).WillRepeatedly(testing::Return(true));
    EXPECT_TRUE(tC->IsBasicChannel());

    EXPECT_CALL(*iChannel, IsClosed()).WillRepeatedly(testing::Return(false));
    EXPECT_TRUE(tC->IsClosed());

    EXPECT_CALL(*iChannel, GetSelectResponse()).WillRepeatedly(testing::Return(rsp));
    EXPECT_CALL(*iChannel, SelectNext()).WillRepeatedly(testing::Return(true));
    EXPECT_CALL(*iChannel, Transmit(_)).WillRepeatedly(testing::Return(rsp));
    tC->GetSelectResponse();
    tC->Close();
    tC->SelectNext();
    std::string command = "testok";
    tC->Transmit(command);

    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
}
}  // namespace omapi
}  // namespace OHOS::se