#include "session.h"

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

using namespace testing;

namespace OHOS::se {
namespace omapi {
class SessionTest : public ::testing::Test {
public:
    virtual void SetUp() override
    {
        std::shared_ptr<ConnectedListenerMock> listener = std::make_shared<ConnectedListenerMock>();
        seService = std::make_shared<SEService>(listener);
        seService->InitSEService();

        std::string name = "channel_test";
        std::shared_ptr<ISecureElementReaderMock> iReader = std::make_shared<ISecureElementReaderMock>();
        reader = std::make_shared<Reader>(seService, name, iReader);

        iSession = std::make_shared<ISecureElementSessionMock>();
        session = std::make_shared<Session>(seService, iSession, reader);
    }

    virtual void TearDown() override
    {
        session = std::shared_ptr<Session>();
    }

public:
    std::shared_ptr<SEService> seService;
    std::shared_ptr<Reader> reader;
    std::shared_ptr<Session> session;
    std::shared_ptr<ISecureElementSessionMock> iSession;
};

TEST_F(SessionTest, GetReader_Test)
{
    session->GetReader();
}

TEST_F(SessionTest, GetATR_Test)
{
    std::string atr = session->GetATR();
    EXPECT_TRUE(atr.empty());
}

TEST_F(SessionTest, Close_Test)
{
    session->Close();
}

TEST_F(SessionTest, IsClosed_Test)
{
    EXPECT_FALSE(session->IsClosed());
}

TEST_F(SessionTest, CloseChannels_Test)
{
    session->CloseChannels();
}

TEST_F(SessionTest, OpenBasicChannel_Test)
{
    std::string aid = "12333444545";
    int p2 = 0x02;
    EXPECT_FALSE(session->OpenBasicChannel(aid, p2));
    EXPECT_FALSE(session->OpenBasicChannel(aid));
}

TEST_F(SessionTest, OpenLogicalChannel_Test)
{
    std::string aid = "12333444545";
    int p2 = 0x02;
    EXPECT_FALSE(session->OpenLogicalChannel(aid, p2));
    EXPECT_FALSE(session->OpenLogicalChannel(aid));
}

TEST_F(SessionTest, SessionTest_OK)
{
    // sptr<IRemoteObject> connectionCallback = new IPCObjectStub(u"se service listener unit test");

    // std::shared_ptr<ISecureElementServiceMock> sem = std::make_shared<ISecureElementServiceMock>();
    // EXPECT_CALL(*sem, AsObject()).WillRepeatedly(testing::Return(connectionCallback));
    // seService->GetConnectCallback()->OnConnectedSystemAbility(sem->AsObject());

    std::string rsp = "succuss";
    std::shared_ptr<ISecureElementChannelMock> tC = std::make_shared<ISecureElementChannelMock>();
    EXPECT_CALL(*iSession, GetATR()).WillRepeatedly(testing::Return(rsp));
    EXPECT_CALL(*iSession, OpenBasicChannel(_, _, _)).WillRepeatedly(testing::Return(tC));
    EXPECT_CALL(*iSession, OpenLogicalChannel(_, _, _)).WillRepeatedly(testing::Return(tC));

    std::string atr = session->GetATR();
    EXPECT_FALSE(atr.empty());

    std::string aid = "12333444545";
    int p2 = 0x02;
    EXPECT_TRUE(session->OpenBasicChannel(aid, p2));
    EXPECT_TRUE(session->OpenBasicChannel(aid));
    EXPECT_TRUE(session->OpenLogicalChannel(aid, p2));
    EXPECT_TRUE(session->OpenLogicalChannel(aid));

    session->IsClosed();
    session->CloseChannels();
    session->Close();

    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
}
}  // namespace omapi
}  // namespace OHOS::se