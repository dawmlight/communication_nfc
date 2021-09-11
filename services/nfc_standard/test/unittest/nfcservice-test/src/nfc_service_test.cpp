#include "nfc_service.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <chrono>

#include "context.h"
#include "key_guard_manager.h"
#include "power_manager.h"
#include "shared_preferences.h"
// nfc service
#include "common_event_manager.h"
#include "device_host.h"
#include "nfc_service.h"
#include "nfc_service_define.h"
#include "nfc_service_handler.h"
#include "tag_dispatcher.h"
#include "tag_end_point.h"
#include "want.h"
// nfc sdk
#include "tag.h"
// mock
#include "test-ncibal/device_host_mock.h"
#include "test-reader/app_callback_mock.h"

using namespace testing;
using namespace osal;
using namespace OHOS::nfc;
using namespace OHOS::nfc::reader;
using namespace OHOS::nfc::ncibal;

namespace OHOS {
using AAFwk::Want;
using EventFwk::CommonEventData;
using EventFwk::CommonEventManager;
using EventFwk::CommonEventPublishInfo;
using EventFwk::CommonEventSubscribeInfo;
using EventFwk::CommonEventSubscriber;
using EventFwk::MatchingSkills;

// Screen Common Event
const std::string ACTION_SCREEN_ON = "ohos.intent.action.SCREEN_ON";
const std::string ACTION_SCREEN_OFF = "ohos.intent.action.SCREEN_OFF";
const std::string ACTION_USER_PRESENT = "ohos.intent.action.USER_PRESENT";
const std::string ACTION_USER_SWITCHED = "ohos.intent.action.USER_SWITCHED";
// Package Common Event
const std::string ACTION_PACKAGE_REMOVED = "ohos.intent.action.PACKAGE_REMOVED";
const std::string ACTION_PACKAGE_ADDED = "ohos.intent.action.PACKAGE_ADDED";
const std::string ACTION_EXTERNAL_APPLICATIONS_AVAILABLE = "ohos.intent.action.EXTERNAL_APPLICATIONS_AVAILABLE";
const std::string ACTION_EXTERNAL_APPLICATIONS_UNAVAILABLE = "ohos.intent.action.EXTERNAL_APPLICATIONS_UNAVAILABLE";
const std::string ACTION_SHUTDOWN = "ohos.intent.action.ACTION_SHUTDOWN";
// Package Common Event Permission
const std::string ACTION_PACKAGE_PERMISSION = "package";

using ::testing::Return;

void ProcessScreenOnAndKeyLocked(bool screenOn, bool keyLocked, std::shared_ptr<Context> context)
{
    std::weak_ptr<PowerManager> pm = context->GetPowerManagerService();
    std::weak_ptr<KeyguardManager> keyguard = context->GetKeyguardManagerService();
    pm.lock()->SetScreenOn(screenOn);
    // SCREEN_STATE_ON_LOCKED
    keyguard.lock()->SetKeyguardLocked(keyLocked);
}

// The fixture for testing class NfcAgentService.
class NfcServiceTest : public ::testing::Test {
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
    }

    virtual void TearDown() override
    {
        ProcessScreenOnAndKeyLocked(true, false, ctx);
        std::cout << " TearDownTestCase." << std::endl;
        deviceHost = nullptr;
        nfcService = nullptr;
        ctx = nullptr;
    }

public:
    std::shared_ptr<Context> ctx;
    std::shared_ptr<NfcService> nfcService;
    std::unique_ptr<DeviceHostMock> deviceHost;
};

TEST(NfcService, NfcService_Structure_Test)
{
    // Nfc Structure Success
    std::shared_ptr<Context> ctx = std::make_shared<Context>();
    std::shared_ptr<NfcService> nfcService = std::make_shared<NfcService>(ctx);
}

TEST(NfcService, NfcService_Structure_Failed)
{
    // Nfc Structure Failed
    try {
        std::shared_ptr<Context> ctx;
        std::shared_ptr<NfcService> nfcService = std::make_shared<NfcService>(ctx);
    } catch (const std::exception& e) {
        EXPECT_STREQ(e.what(), "Context Expired");
    }
}

/**
 * @brief Check NCI CheckFirmware When NCI Isnot used.
 */
// TEST(NfcService, Initialize_Original)
//{
//    auto ctx = std::make_shared<Context>();
//    std::weak_ptr<osal::SharedPreferences> prefs =  ctx->GetSharedPreferences("nfc");
//    // Check NCI CheckFirmware
//    prefs.lock()->PutBool("nfc_on", false);
//    auto nfcService = std::make_shared<NfcService>(ctx);
//    EXPECT_TRUE(nfcService->Initialize());
//}

TEST_F(NfcServiceTest, GetInstance_Test)
{
    // Get Instance
    EXPECT_EQ(nfcService->GetInstance().lock(), nfcService);
}

TEST_F(NfcServiceTest, IDeviceHostListener_Test)
{
    nfcService->OnRemoteFieldActivated();
    nfcService->OnRemoteFieldDeactivated();
    nfcService->OnHostCardEmulationActivated(1);
    nfcService->OnHostCardEmulationDeactivated(1);
    std::string data("123");
    nfcService->OnHostCardEmulationDataReceived(1, data);
    std::string aid("123");
    std::string tdata("abc");
    std::string seName("678");
    nfcService->OnOffHostTransactionEvent(aid, tdata, seName);
    nfcService->OnEeUpdate();
    nfcService->OnTagDiscovered(nullptr);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
}

TEST_F(NfcServiceTest, ACTION_SCREEN_ON_Test)
{
    std::shared_ptr<Context> context = std::make_shared<Context>();
    ProcessScreenOnAndKeyLocked(true, true, context);

    Want want;
    want.SetAction(ACTION_SCREEN_ON);
    CommonEventData data;
    data.SetWant(want);
    CommonEventManager::PublishCommonEvent(data);

    // broadcast delayed
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
}

TEST_F(NfcServiceTest, ACTION_SCREEN_ON_2_Test)
{
    std::shared_ptr<Context> context = std::make_shared<Context>();
    ProcessScreenOnAndKeyLocked(true, false, context);
    Want want;
    want.SetAction(ACTION_SCREEN_ON);
    CommonEventData data;
    data.SetWant(want);
    CommonEventManager::PublishCommonEvent(data);
    // broadcast delayed
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
}

TEST_F(NfcServiceTest, ACTION_USER_PRESENT_Test)
{
    Want want;
    want.SetAction(ACTION_USER_PRESENT);
    CommonEventData data;
    data.SetWant(want);
    CommonEventManager::PublishCommonEvent(data);
    // broadcast delayed
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
}

TEST(NfcService, GetNciVersion_Test)
{
    std::shared_ptr<Context> ctx = std::make_shared<Context>();
    std::weak_ptr<osal::SharedPreferences> prefs = ctx->GetSharedPreferences("nfc");
    prefs.lock()->PutBool("nfc_on", true);
    std::unique_ptr<DeviceHostMock> deviceHost = std::make_unique<DeviceHostMock>();
    EXPECT_CALL(*deviceHost, FactoryReset()).Times(AnyNumber());
    EXPECT_CALL(*deviceHost, SetSecureNfc(_)).WillRepeatedly(Return(true));
    EXPECT_CALL(*deviceHost, CheckFirmware()).WillRepeatedly(Return(true));
    EXPECT_CALL(*deviceHost, SetDeviceHostListener(testing::_)).Times(1);
    EXPECT_CALL(*deviceHost, Initialize()).WillRepeatedly(Return(true));
    EXPECT_CALL(*deviceHost, Deinitialize()).WillRepeatedly(Return(true));
    // Nci Version Isnot 0x02
    EXPECT_CALL(*deviceHost, GetNciVersion()).WillRepeatedly(Return(0x01));
    EXPECT_CALL(*deviceHost, SetScreenStatus(_)).Times(AnyNumber()).WillRepeatedly(Return(true));
    EXPECT_CALL(*deviceHost, EnableDiscovery(_, _, _, _)).Times(AnyNumber());
    EXPECT_CALL(*deviceHost, DisableDiscovery()).Times(AnyNumber());
    EXPECT_CALL(*deviceHost, Abort()).Times(AnyNumber());
    std::shared_ptr<NfcService> nfcService = std::make_shared<NfcService>(ctx, std::move(deviceHost));
    EXPECT_TRUE(nfcService->Initialize());
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    Want want;
    want.SetAction(ACTION_USER_PRESENT);
    CommonEventData data;
    data.SetWant(want);
    CommonEventManager::PublishCommonEvent(data);
    // broadcast delayed
    std::this_thread::sleep_for(std::chrono::milliseconds(3000));
}

TEST_F(NfcServiceTest, ACTION_SCREEN_OFF_Test)
{
    std::shared_ptr<Context> context = std::make_shared<Context>();
    ProcessScreenOnAndKeyLocked(false, true, context);
    Want want;
    want.SetAction(ACTION_SCREEN_OFF);
    CommonEventData data;
    data.SetWant(want);
    CommonEventManager::PublishCommonEvent(data);
    // broadcast delayed
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    ProcessScreenOnAndKeyLocked(false, false, context);
    CommonEventManager::PublishCommonEvent(data);
    // broadcast delayed
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
}

TEST_F(NfcServiceTest, ACTION_USER_SWITCHED_Test)
{
    std::shared_ptr<Context> context = std::make_shared<Context>();
    ProcessScreenOnAndKeyLocked(false, true, context);
    Want want;
    want.SetAction(ACTION_USER_SWITCHED);
    CommonEventData data;
    data.SetWant(want);
    CommonEventManager::PublishCommonEvent(data);
    // broadcast delayed
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
}

TEST_F(NfcServiceTest, ACTION_PACKAGE_REMOVED_Test)
{
    Want want;
    want.SetAction(ACTION_PACKAGE_REMOVED);
    CommonEventData data;
    data.SetWant(want);
    CommonEventManager::PublishCommonEvent(data);
    // broadcast delayed
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
}

TEST_F(NfcServiceTest, ACTION_PACKAGE_ADDED_Test)
{
    Want want;
    want.SetAction(ACTION_PACKAGE_ADDED);
    CommonEventData data;
    data.SetWant(want);
    CommonEventManager::PublishCommonEvent(data);
    // broadcast delayed
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
}

TEST_F(NfcServiceTest, ACTION_EXTERNAL_APPLICATIONS_AVAILABLE_Test)
{
    Want want;
    want.SetAction(ACTION_EXTERNAL_APPLICATIONS_AVAILABLE);
    CommonEventData data;
    data.SetWant(want);
    CommonEventManager::PublishCommonEvent(data);
    // broadcast delayed
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
}

TEST_F(NfcServiceTest, ACTION_EXTERNAL_APPLICATIONS_UNAVAILABLE_Test)
{
    Want want;
    want.SetAction(ACTION_EXTERNAL_APPLICATIONS_UNAVAILABLE);
    CommonEventData data;
    data.SetWant(want);
    CommonEventManager::PublishCommonEvent(data);
    // broadcast delayed
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
}

TEST(NfcService, ACTION_SHUTDOWN_Test)
{
    std::shared_ptr<Context> ctx = std::make_shared<Context>();
    std::weak_ptr<osal::SharedPreferences> prefs = ctx->GetSharedPreferences("nfc");
    prefs.lock()->PutBool("nfc_on", true);
    std::unique_ptr<DeviceHostMock> deviceHost = std::make_unique<DeviceHostMock>();
    EXPECT_CALL(*deviceHost, FactoryReset()).Times(AnyNumber());
    EXPECT_CALL(*deviceHost, SetSecureNfc(_)).WillRepeatedly(Return(true));
    EXPECT_CALL(*deviceHost, CheckFirmware()).WillRepeatedly(Return(true));
    EXPECT_CALL(*deviceHost, SetDeviceHostListener(testing::_)).Times(1);
    EXPECT_CALL(*deviceHost, Initialize()).WillRepeatedly(Return(true));
    EXPECT_CALL(*deviceHost, Deinitialize()).WillRepeatedly(Return(true));
    // Nci Version Isnot 0x02
    EXPECT_CALL(*deviceHost, GetNciVersion()).WillRepeatedly(Return(0x01));
    EXPECT_CALL(*deviceHost, SetScreenStatus(_)).Times(AnyNumber()).WillRepeatedly(Return(true));
    EXPECT_CALL(*deviceHost, EnableDiscovery(_, _, _, _)).Times(AnyNumber());
    EXPECT_CALL(*deviceHost, DisableDiscovery()).Times(AnyNumber());
    EXPECT_CALL(*deviceHost, Abort()).Times(AnyNumber());
    EXPECT_CALL(*deviceHost, Shutdown()).Times(AnyNumber());
    std::shared_ptr<NfcService> nfcService = std::make_shared<NfcService>(ctx, std::move(deviceHost));
    EXPECT_TRUE(nfcService->Initialize());
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    Want want;
    want.SetAction(ACTION_SHUTDOWN);
    CommonEventData data;
    data.SetWant(want);
    CommonEventManager::PublishCommonEvent(data);
    // broadcast delayed
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
}
}  // namespace OHOS