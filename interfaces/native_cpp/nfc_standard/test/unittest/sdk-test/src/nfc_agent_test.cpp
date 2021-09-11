#include "nfc_agent.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "ability.h"
#include "if_system_ability_manager.h"
#include "infc_agent_service_mock.h"
#include "iservice_registry.h"
#include "itag_session.h"
#include "loghelper.h"
#include "nfc_ability_manager.h"
#include "ohos_application.h"
#include "refbase.h"
#include "system_ability_definition.h"
#include "tag_callback_mock.h"
#include "tag_data.h"

using namespace OHOS::nfc::sdk;
using namespace OHOS::nfc::reader;
using namespace testing;
using namespace OHOS;
using namespace OHOS::AppExecFwk;

class NfcAgentTest : public ::testing::Test {
public:
    virtual void SetUp() override
    {
        std::cout << " SetUpTestCase NfcAgentTest." << std::endl;
    }

    virtual void TearDown() override
    {
        std::cout << " TearDownTestCase NfcAgentTest." << std::endl;
    }
};

TEST_F(NfcAgentTest, GetNFCAgent_Ok)
{
    std::shared_ptr<INfcAgentServiceMock> ns_ = std::make_shared<INfcAgentServiceMock>();
    std::shared_ptr<OHOSApplication> app_ = std::make_shared<OHOSApplication>();
    std::shared_ptr<ITagSessionMock> tsm_ = std::make_shared<ITagSessionMock>();  // addservice

    sptr<ISystemAbilityManager> systemAbilityMgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (systemAbilityMgr &&
        (systemAbilityMgr->AddSystemAbility(NFC_MANAGER_SYS_ABILITY_ID + 10, ns_->AsObject()) == 0)) {
    } else {
        InfoLog("NfcServiceManager::Init Add System Ability failed!");
        return;
    }

    auto nae = NfcAgent::GetNFCAgent(app_);
    EXPECT_FALSE(nae);
    EXPECT_CALL(*ns_, GetNfcTagInterface()).WillRepeatedly(testing::Return(tsm_));
    EXPECT_CALL(*ns_, TurnOn()).WillRepeatedly(testing::Return(true));
    EXPECT_CALL(*ns_, TurnOff(_)).WillRepeatedly(testing::Return(true));

    std::shared_ptr<NfcAgent> na_ = /*std::make_shared<NfcAgent>()*/ NfcAgent::GetNFCAgent(app_);
    EXPECT_TRUE(na_->ActiveNfc());
    EXPECT_TRUE(na_->DeactiveNfc());

    // auto ability = std::make_shared<Ability>(app_, "test_valid_ability");
    auto ability = std::make_shared<Ability>();
    auto cb = std::make_shared<TagCallbackMock>();
    // auto nam = na_->GetNfcAbilityManager()/* std::make_shared<NfcAbilityManager>()*/;
    EXPECT_EQ(na_->ActiveNfcReaderMode(ability, cb, data_valid_connect_tech, data_null_extra), 0);
    EXPECT_EQ(na_->DeactiveNfcReaderMode(ability), 0);

    // std::shared_ptr<osal::Bundle> b;
    // nam->OnActivityCreated(ability, b);
    // nam->OnActivityStarted(ability);
    // nam->OnActivityStopped(ability);
    // nam->OnActivityResumed(ability);
    // nam->OnActivityPaused(ability);
    // nam->OnActivityDestroyed(ability);

    auto t = TagData::GetValidTag();
    EXPECT_FALSE(na_->IgnoreTag(t, cb, 100));

    std::string id = data_valid_id;
    auto extra = std::make_shared<NfcMap>();
    auto itsm = std::make_shared<ITagSessionMock>();
    auto ut = std::make_unique<Tag>(id, data_techlist_all, extra, data_valid_service_handle, itsm);
    // nam->OnTagDiscovered(std::move(ut));

    systemAbilityMgr->RemoveSystemAbility(NFC_MANAGER_SYS_ABILITY_ID + 10);
}

TEST_F(NfcAgentTest, GetNFCAgent_Failed)
{
    auto app = std::make_shared<OHOSApplication>();
    auto agent = NfcAgent::GetNFCAgent(app);
    EXPECT_FALSE(agent);

    app = std::shared_ptr<OHOSApplication>();
    agent = NfcAgent::GetNFCAgent(app);
    EXPECT_FALSE(agent);
}

TEST_F(NfcAgentTest, HasNfcCapability_Test)
{
    auto na = std::make_shared<NfcAgent>();
    na->HasNfcCapability();
}

TEST_F(NfcAgentTest, HasNfcHceCapability_Test)
{
    auto na = std::make_shared<NfcAgent>();
    na->HasNfcHceCapability();
}

TEST_F(NfcAgentTest, GetNfcCardEmulationService_Test)
{
    auto na = std::make_shared<NfcAgent>();
    na->GetNfcCardEmulationService();
}

TEST_F(NfcAgentTest, ActiveNfc_Failed)
{
    auto na = std::make_shared<NfcAgent>();
    EXPECT_NE(na->ActiveNfc(), 0);
}

TEST_F(NfcAgentTest, DeactiveNfc_Failed)
{
    auto na = std::make_shared<NfcAgent>();
    EXPECT_NE(na->DeactiveNfc(), 0);
}

TEST_F(NfcAgentTest, GetNfcState_Failed)
{
    auto na = std::make_shared<NfcAgent>();
    EXPECT_NE(na->GetNfcState(), 0);
}

TEST_F(NfcAgentTest, ActiveNfcSecure_Failed)
{
    auto na = std::make_shared<NfcAgent>();
    EXPECT_NE(na->ActiveNfcSecure(), 0);
}

TEST_F(NfcAgentTest, DeactiveNfcSecure_Failed)
{
    auto na = std::make_shared<NfcAgent>();
    EXPECT_NE(na->DeactiveNfcSecure(), 0);
}

TEST_F(NfcAgentTest, GetNfcSecureState_Failed)
{
    auto na = std::make_shared<NfcAgent>();
    EXPECT_NE(na->GetNfcSecureState(), 0);
}

TEST_F(NfcAgentTest, HasSecureNfcCapability_Failed)
{
    auto na = std::make_shared<NfcAgent>();
    na->HasSecureNfcCapability();
}

TEST_F(NfcAgentTest, PausePolling_Failed)
{
    auto na = std::make_shared<NfcAgent>();
    na->PausePolling(100);
}

TEST_F(NfcAgentTest, ResumePolling_Failed)
{
    auto na = std::make_shared<NfcAgent>();
    na->ResumePolling();
}

TEST_F(NfcAgentTest, ActiveNfcReaderMode_Failed)
{
    auto na = std::make_shared<NfcAgent>();

    auto app = std::make_shared<OHOSApplication>();
    // auto ability = std::make_shared<Ability>(app, "testability");
    auto ability = std::make_shared<Ability>();
    auto cb = std::make_shared<TagCallbackMock>();

    auto iapp = std::shared_ptr<OHOSApplication>();
    // auto iability = std::make_shared<Ability>(iapp, "testability");
    auto iability = std::make_shared<Ability>();
    auto iability1 = std::shared_ptr<Ability>();
    auto icb = std::shared_ptr<TagCallbackMock>();

    EXPECT_EQ(na->ActiveNfcReaderMode(iability, cb, data_valid_connect_tech, data_null_extra), 0);
    EXPECT_NE(na->ActiveNfcReaderMode(iability1, cb, data_valid_connect_tech, data_null_extra), 0);
    EXPECT_NE(na->ActiveNfcReaderMode(ability, icb, data_valid_connect_tech, data_null_extra), 0);
    EXPECT_EQ(na->ActiveNfcReaderMode(ability, cb, data_valid_connect_tech, data_null_extra), 0);
}

TEST_F(NfcAgentTest, DeactiveNfcReaderMode_Failed)
{
    auto app = std::make_shared<OHOSApplication>();
    auto app1 = std::shared_ptr<OHOSApplication>();
    auto na = std::make_shared<NfcAgent>();
    // auto ability = std::make_shared<Ability>(app, "testability");
    auto ability = std::make_shared<Ability>();
    auto ability1 = std::shared_ptr<Ability>();

    EXPECT_EQ(na->DeactiveNfcReaderMode(ability), 0);
    EXPECT_NE(na->DeactiveNfcReaderMode(ability1), 0);
}

TEST_F(NfcAgentTest, IgnoreTag_Failed)
{
    auto na = std::make_shared<NfcAgent>();
    auto t = TagData::GetValidTag();
    auto cb = std::make_shared<TagCallbackMock>();
    EXPECT_FALSE(na->IgnoreTag(t, cb, 100));
}

TEST_F(NfcAgentTest, GetNfcAbilityManager_Failed)
{
    // auto na = std::make_shared<NfcAgent>();
    // na->GetNfcAbilityManager();
}

TEST_F(NfcAgentTest, OnTagRemoved_Failed)
{
    auto na = std::make_shared<NfcAgent>();
    auto cb = std::make_shared<TagCallbackMock>();
    auto rcb = std::make_shared<NfcAgent::TagRemovedCallback>(cb);
    rcb->OnTagRemoved();
}
