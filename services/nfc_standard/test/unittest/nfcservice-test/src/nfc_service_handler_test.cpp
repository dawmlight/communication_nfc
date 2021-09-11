#include "nfc_service_handler.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>

#include "context.h"
#include "device_host.h"
#include "key_guard_manager.h"
#include "nfc_service.h"
#include "nfc_service_define.h"
#include "nfc_service_handler.h"
#include "shared_preferences.h"
#include "tag_dispatcher.h"
#include "test-ncibal/device_host_mock.h"

using namespace testing;
using namespace osal;
using namespace OHOS::nfc;
using namespace OHOS::nfc::reader;

using ::testing::Return;

// The fixture for testing class NfcServiceHandler.
class NfcServiceHandlerTest : public ::testing::Test {
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
        // NFC Service Handler
        std::shared_ptr<INfcService> service = nfcService;
        mHandler_ = service->GetMessageHandler();
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
    std::weak_ptr<OHOS::AppExecFwk::EventHandler> mHandler_{};
};

TEST_F(NfcServiceHandlerTest, HandleMessage_Test_1)
{
    // MSG_TOAST_DEBOUNCE_EVENT
    EXPECT_EQ(mHandler_.lock()->SendEvent(MSG_TOAST_DEBOUNCE_EVENT), true);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
}

TEST_F(NfcServiceHandlerTest, HandleMessage_Test_2)
{
    // MSG_APPLY_SCREEN_STATE
    EXPECT_EQ(mHandler_.lock()->SendEvent(MSG_APPLY_SCREEN_STATE, 1, (int64_t)0), true);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
}

TEST_F(NfcServiceHandlerTest, HandleMessage_Test_3)
{
    // MSG_TAG_DEBOUNCE
    EXPECT_EQ(mHandler_.lock()->SendEvent(MSG_TAG_DEBOUNCE), true);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
}

TEST_F(NfcServiceHandlerTest, HandleMessage_Test_4)
{
    // MSG_NDEF_TAG
    EXPECT_EQ(mHandler_.lock()->SendEvent(MSG_NDEF_TAG), true);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
}

TEST_F(NfcServiceHandlerTest, HandleMessage_Test_5)
{
    // MSG_EVENT_RESUME_POLLING
    EXPECT_EQ(mHandler_.lock()->SendEvent(MSG_EVENT_RESUME_POLLING), true);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
}
