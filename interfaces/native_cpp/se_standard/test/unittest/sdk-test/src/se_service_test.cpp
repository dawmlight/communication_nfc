#include "se_service.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>
#include <thread>

#include "connected_listener_mock.h"
#include "if_system_ability_manager.h"
#include "iremote_object.h"
#include "isecure_element_service_mock.h"
#include "iservice_registry.h"
#include "loghelper.h"
#include "system_ability_definition.h"

using namespace testing;
namespace OHOS::se {
namespace omapi {
class SEServiceTest : public ::testing::Test {
public:
    virtual void SetUp() override
    {
        DebugLog("set up in");
        sem = new ISecureElementServiceMock();
        systemAbilityMgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
        if ((systemAbilityMgr->AddSystemAbility(NFC_MANAGER_SYS_ABILITY_ID + 10, sem)) == 0) {
            DebugLog("AddSystemAbility suc");
            std::u16string testAbilityId = reinterpret_cast<const char16_t*>("test_se_service");
            systemAbilityMgr->AddOnDemandSystemAbilityInfo(NFC_MANAGER_SYS_ABILITY_ID + 10, testAbilityId);
            listener = std::make_shared<ConnectedListenerMock>();
            seService = std::make_shared<SEService>(listener);

            EXPECT_CALL(*listener, CBConnected()).Times(1);
            seService->InitSEService();
            std::this_thread::sleep_for(std::chrono::milliseconds(2000));
        } else {
            EXPECT_TRUE(false);
        }
    }

    virtual void TearDown() override
    {
        listener = std::shared_ptr<ConnectedListenerMock>();
        seService = std::shared_ptr<SEService>();
        systemAbilityMgr->RemoveSystemAbility(NFC_MANAGER_SYS_ABILITY_ID + 10);
    }

public:
    OHOS::sptr<ISecureElementServiceMock> sem;
    std::shared_ptr<SEService> seService;
    sptr<ISystemAbilityManager> systemAbilityMgr;
    std::shared_ptr<ConnectedListenerMock> listener;
};

TEST_F(SEServiceTest, IsConnected_Test)
{
    seService->IsConnected();
    seService->Shutdown();
    seService->GetVersion();
    seService->GetListener();
}

TEST_F(SEServiceTest, GetReaders_Test)
{
    std::vector<std::shared_ptr<Reader>> readerList = seService->GetReaders();
    EXPECT_TRUE(readerList.size() == 0);
}

TEST_F(SEServiceTest, GetUiccReader_Test)
{
    std::shared_ptr<Reader> uiccReader = seService->GetUiccReader(0);
    EXPECT_FALSE(uiccReader);
    uiccReader = seService->GetUiccReader(2);
    EXPECT_FALSE(uiccReader);
}

TEST_F(SEServiceTest, Shutdown_Test)
{
    seService->Shutdown();
}

// TEST_F(SEServiceTest, OnConnectedSystemAbility_Test)
// {
//     sptr<IRemoteObject> connectCallback = new IPCObjectStub(u"se service listener unit test");
//     seService->GetConnectCallback()->OnConnectedSystemAbility(connectCallback);
// }

// TEST_F(SEServiceTest, OnDisConnectedSystemAbility_Test)
// {
//     int systemAbilityId = 10;
//     seService->GetConnectCallback()->OnDisConnectedSystemAbility(systemAbilityId);
// }

TEST_F(SEServiceTest, SEService_OK)
{
    // sptr<IRemoteObject> connectionCallback = new IPCObjectStub(u"se service listener unit test");

    // std::shared_ptr<ISecureElementServiceMock> sem = std::make_shared<ISecureElementServiceMock>();

    // EXPECT_CALL(*sem, AsObject()).WillRepeatedly(testing::Return(connectionCallback));

    std::vector<std::string> readers;
    readers.push_back("ESE1");
    readers.push_back("SM1");
    // seService->GetConnectCallback()->OnConnectedSystemAbility(sem->AsObject());
    EXPECT_CALL(*sem, GetReaders()).WillRepeatedly(testing::Return(readers));

    std::vector<std::shared_ptr<Reader>> readerList = seService->GetReaders();
    std::shared_ptr<Reader> uiccReader = seService->GetUiccReader(0);
    EXPECT_FALSE(uiccReader);
    uiccReader = seService->GetUiccReader(2);

    EXPECT_TRUE(seService->IsConnected());

    seService->Shutdown();
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
}
}  // namespace omapi
}  // namespace OHOS::se