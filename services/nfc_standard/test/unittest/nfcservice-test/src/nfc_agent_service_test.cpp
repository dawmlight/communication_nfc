#include "nfc_agent_service.h"

#include <gtest/gtest.h>

#include <chrono>

#include "context.h"
#include "key_guard_manager.h"
#include "shared_preferences.h"
// nfc service
#include "device_host.h"
#include "ipc_object_stub.h"
#include "nfc_map.h"
#include "nfc_service.h"
#include "nfc_service_define.h"
#include "nfc_service_handler.h"
#include "tag_dispatcher.h"
#include "tag_end_point.h"
// nfc sdk
#include "tag.h"
// mock
#include "test-ncibal/device_host_mock.h"
#include "test-reader/app_callback_mock.h"

using namespace testing;
using namespace osal;
using namespace OHOS::nfc;
using namespace OHOS::nfc::sdk;
using namespace OHOS::nfc::reader;
using namespace OHOS::nfc::ncibal;
using OHOS::IPCObjectStub;
using OHOS::IRemoteObject;
using OHOS::sptr;

using ::testing::Return;
namespace {
// The fixture for testing class NfcAgentService.
class NfcAgentServiceTest : public ::testing::Test {
public:
    virtual void SetUp() override
    {
        std::cout << " SetUpTestCase." << std::endl;

        ctx = std::make_shared<Context>();
        std::weak_ptr<osal::SharedPreferences> prefs = ctx->GetSharedPreferences("nfc");
        prefs.lock()->PutBool("nfc_on", false);
        deviceHost = std::make_unique<DeviceHostMock>();
        EXPECT_CALL(*deviceHost, FactoryReset()).Times(AnyNumber());
        EXPECT_CALL(*deviceHost, SetSecureNfc(_)).WillRepeatedly(Return(true));
        EXPECT_CALL(*deviceHost, CheckFirmware()).WillRepeatedly(Return(true));
        EXPECT_CALL(*deviceHost, SetDeviceHostListener(testing::_)).Times(1);
        EXPECT_CALL(*deviceHost, Initialize()).WillRepeatedly(Return(true));
        EXPECT_CALL(*deviceHost, Deinitialize()).WillRepeatedly(Return(true));
        EXPECT_CALL(*deviceHost, GetNciVersion()).WillRepeatedly(Return(0x02));
        EXPECT_CALL(*deviceHost, SetScreenStatus(_)).Times(AnyNumber()).WillRepeatedly(Return(true));
        EXPECT_CALL(*deviceHost, EnableDiscovery(_, _, _, _)).Times(AnyNumber());
        EXPECT_CALL(*deviceHost, DisableDiscovery()).Times(AnyNumber());
        EXPECT_CALL(*deviceHost, Abort()).Times(AnyNumber());
        EXPECT_CALL(*deviceHost, Shutdown()).Times(AnyNumber());
        nfcService = std::make_shared<NfcService>(ctx, std::move(deviceHost));
        EXPECT_TRUE(nfcService->Initialize());
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        // nfc agent service
        nfcAgentService = std::make_shared<NfcAgentService>(nfcService, ctx);
    }

    virtual void TearDown() override
    {
        std::cout << " TearDownTestCase." << std::endl;
        deviceHost = nullptr;
        nfcService = nullptr;
        ctx = nullptr;
    }

public:
    std::shared_ptr<Context> ctx;
    std::shared_ptr<NfcService> nfcService;
    std::unique_ptr<DeviceHostMock> deviceHost;
    // Test Class
    std::shared_ptr<NfcAgentService> nfcAgentService{};
};
}  // namespace

TEST_F(NfcAgentServiceTest, GetState_Test)
{
    // NFC State
    EXPECT_EQ(nfcAgentService->GetState(), OHOS::nfc::sdk::STATE_OFF);
}

TEST_F(NfcAgentServiceTest, DeviceSupportsNfcSecure_Test)
{
    // Device Supports Nfc Secure
    EXPECT_TRUE(nfcAgentService->DeviceSupportsNfcSecure());
}

TEST_F(NfcAgentServiceTest, GetNfcTagInterface_Test)
{
    // Get Tag Session
    EXPECT_NE(nfcAgentService->GetNfcTagInterface().lock(), nullptr);
}

TEST_F(NfcAgentServiceTest, SetNfcSecure_Test)
{
    EXPECT_TRUE(nfcAgentService->SetNfcSecure(true));
    auto keyguard_ = ctx->GetKeyguardManagerService();
    keyguard_.lock()->SetKeyguardLocked(true);
    // Set Nfc Secure true
    EXPECT_TRUE(nfcAgentService->SetNfcSecure(true));
    // Is Nfc Secure Enabled
    EXPECT_TRUE(nfcAgentService->IsNfcSecureEnabled());
    // Set Nfc Secure False
    EXPECT_FALSE(nfcAgentService->SetNfcSecure(false));

    keyguard_.lock()->SetKeyguardLocked(false);
    // Set Nfc Secure true
    EXPECT_TRUE(nfcAgentService->SetNfcSecure(true));
    // Set Nfc Secure True
    EXPECT_TRUE(nfcAgentService->SetNfcSecure(false));
}

TEST_F(NfcAgentServiceTest, TurnOn_Success_Test)
{
    EXPECT_TRUE(nfcAgentService->TurnOn());
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    EXPECT_TRUE(nfcAgentService->TurnOn());
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    EXPECT_TRUE(nfcAgentService->TurnOff(false));
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    EXPECT_TRUE(nfcAgentService->TurnOff(false));
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
}

TEST(NfcAgentService, TurnOn_Failed_Test)
{
    std::shared_ptr<Context> ctx = std::make_shared<Context>();
    std::unique_ptr<DeviceHostMock> deviceHost = std::make_unique<DeviceHostMock>();
    EXPECT_CALL(*deviceHost, FactoryReset()).Times(AnyNumber());
    EXPECT_CALL(*deviceHost, SetSecureNfc(_)).WillRepeatedly(Return(true));
    EXPECT_CALL(*deviceHost, CheckFirmware()).WillRepeatedly(Return(true));
    EXPECT_CALL(*deviceHost, SetDeviceHostListener(testing::_)).Times(1);
    EXPECT_CALL(*deviceHost, Initialize()).WillRepeatedly(Return(false));
    EXPECT_CALL(*deviceHost, Deinitialize()).WillRepeatedly(Return(true));
    // Nci Version Isnot 0x02
    EXPECT_CALL(*deviceHost, GetNciVersion()).WillRepeatedly(Return(0x02));
    EXPECT_CALL(*deviceHost, SetScreenStatus(_)).Times(AnyNumber()).WillRepeatedly(Return(true));
    EXPECT_CALL(*deviceHost, EnableDiscovery(_, _, _, _)).Times(AnyNumber());
    EXPECT_CALL(*deviceHost, DisableDiscovery()).Times(AnyNumber());
    EXPECT_CALL(*deviceHost, Abort()).Times(AnyNumber());
    std::shared_ptr<NfcService> nfcService = std::make_shared<NfcService>(ctx, std::move(deviceHost));
    EXPECT_TRUE(nfcService->Initialize());
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    // nfc agent service
    std::shared_ptr<NfcAgentService> nfcAgentService = std::make_shared<NfcAgentService>(nfcService, ctx);
    //
    EXPECT_TRUE(nfcAgentService->TurnOn());
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
}

TEST_F(NfcAgentServiceTest, PausePolling_Failed)
{
    // Print("Refusing to pause polling for 0 ms.")
    nfcAgentService->PausePolling(0);
    // Print("Refusing to pause polling for 4001 ms.")
    nfcAgentService->PausePolling(4001);
}

TEST_F(NfcAgentServiceTest, PausePolling_Test)
{
    nfcAgentService->PausePolling(100);
    // There is the async process of "MSG_EVENT_RESUME_POLLING" event and Process StartApplyDiscoveringTask
    std::this_thread::sleep_for(std::chrono::milliseconds(5000));
}

TEST_F(NfcAgentServiceTest, ResumePolling_Test)
{
    nfcAgentService->ResumePolling();
    // Process StartApplyDiscoveringTask
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    nfcAgentService->PausePolling(3000);
    nfcAgentService->ResumePolling();
    // Process StartApplyDiscoveringTask
    std::this_thread::sleep_for(std::chrono::milliseconds(5000));
}

#ifdef _NFC_SERVICE_READER_
TEST_F(NfcAgentServiceTest, SetRWMode_Test)
{
    sptr<IRemoteObject> readerObj = new IPCObjectStub(u"ohos.nfc.service.NfcAgentServiceTest");
    std::shared_ptr<IAppCallback> appCallback = std::make_shared<AppCallbackMock>();
    std::shared_ptr<NfcMap> extra = std::make_shared<NfcMap>();
    // active
    nfcAgentService->SetRWMode(readerObj, appCallback, 0x181, extra);
    // deactive
    nfcAgentService->SetRWMode(readerObj, appCallback, 0x0, extra);
}

TEST_F(NfcAgentServiceTest, SetIgnore_Test)
{
    std::shared_ptr<ITagRemovedCallback> appCallback = std::make_shared<TagRemovedCallbackMock>();
    nfcAgentService->SetIgnore(0, 0, appCallback);
}

TEST_F(NfcAgentServiceTest, SetFrontDispatch_Test)
{
    nfcAgentService->SetFrontDispatch();
}
#endif