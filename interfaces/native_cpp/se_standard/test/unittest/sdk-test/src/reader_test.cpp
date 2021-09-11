#include "reader.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>
#include <thread>

#include "connected_listener_mock.h"
#include "isecure_element_reader_mock.h"
#include "isecure_element_service_mock.h"
#include "isecure_element_session_mock.h"

using namespace testing;

namespace OHOS::se {
namespace omapi {
class ReaderTest : public ::testing::Test {
public:
    virtual void SetUp() override
    {
        std::shared_ptr<ConnectedListenerMock> listener = std::make_shared<ConnectedListenerMock>();
        seService = std::make_shared<SEService>(listener);
        seService->InitSEService();
        std::string name = "reader_test";
        std::shared_ptr<ISecureElementReader> iReader = std::make_shared<ISecureElementReaderMock>();

        reader = std::make_shared<Reader>(seService, name, iReader);
    }

    virtual void TearDown() override
    {
        reader = std::shared_ptr<Reader>();
    }

public:
    std::shared_ptr<SEService> seService;
    std::shared_ptr<Reader> reader;
};

TEST_F(ReaderTest, GetName_Test)
{
    reader->GetName();
    reader->GetSEService();
}

TEST_F(ReaderTest, OpenSession_Test)
{
    EXPECT_FALSE(reader->OpenSession());
}

TEST_F(ReaderTest, IsSecureElementPresent_Test)
{
    EXPECT_FALSE(reader->IsSecureElementPresent());
}

TEST_F(ReaderTest, CloseSessions_Test)
{
    reader->CloseSessions();
}

TEST_F(ReaderTest, Reset_Test)
{
    EXPECT_FALSE(reader->Reset());
}

TEST_F(ReaderTest, ReaderTest_OK)
{
    // sptr<IRemoteObject> connectionCallback = new IPCObjectStub(u"se service listener unit test");

    // std::shared_ptr<ISecureElementServiceMock> sem = std::make_shared<ISecureElementServiceMock>();
    // EXPECT_CALL(*sem, AsObject()).WillRepeatedly(testing::Return(connectionCallback));
    // seService->GetConnectCallback()->OnConnectedSystemAbility(sem->AsObject());

    std::string rName = "testReader";
    std::shared_ptr<ISecureElementSessionMock> iSs = std::make_shared<ISecureElementSessionMock>();
    std::shared_ptr<ISecureElementReaderMock> uR = std::make_shared<ISecureElementReaderMock>();
    EXPECT_CALL(*uR, OpenSession()).WillRepeatedly(testing::Return(iSs));

    std::shared_ptr<Reader> testReader = std::make_shared<Reader>(seService, rName, uR);
    std::shared_ptr<Session> testSession = testReader->OpenSession();
    EXPECT_TRUE(testSession);

    EXPECT_CALL(*uR, IsSecureElementPresent()).WillRepeatedly(testing::Return(true));
    EXPECT_TRUE(testReader->IsSecureElementPresent());

    testReader->CloseSessions();
    EXPECT_CALL(*uR, Reset()).WillRepeatedly(testing::Return(true));
    EXPECT_TRUE(testReader->Reset());

    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
}
}  // namespace omapi
}  // namespace OHOS::se