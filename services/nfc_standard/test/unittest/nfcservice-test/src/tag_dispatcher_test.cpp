#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <chrono>
#include <vector>

#include "context.h"
#include "key_guard_manager.h"
#include "loghelper.h"
#include "nfc_map.h"
#include "resources.h"
#include "shared_preferences.h"
// nfc service
#include "device_host.h"
#include "event_handler.h"
#include "ipc_object_stub.h"
#include "nfc_service.h"
#include "nfc_service_define.h"
#include "nfc_service_handler.h"
#include "tag_dispatcher.h"
#include "tag_end_point.h"
// nfc sdk
#include "ndef_message.h"
#include "tag.h"
// mock
#include "test-ncibal/device_host_mock.h"
#include "test-ncibal/tag_end_point_mock.h"
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
using OHOS::AppExecFwk::EventHandler;

using ::testing::Return;
namespace {
// The fixture for testing class TagDispatcher.
class TagDispatcherTest : public ::testing::Test {
public:
    virtual void SetUp() override
    {
        std::cout << " SetUpTestCase." << std::endl;

        ctx = std::make_shared<Context>();
        std::weak_ptr<osal::SharedPreferences> prefs = ctx->GetSharedPreferences("nfc");
        prefs.lock()->PutBool("nfc_on", true);
        std::weak_ptr<Resources> resources = ctx->GetResources();
        resources.lock()->PutBool(R::enable_nfc_blocking_alert, true);
        resources.lock()->PutInt(R::nfc_blocking_count, -1);
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
}  // namespace

// Ndef Data
std::vector<unsigned char> data1 = {0xd1, 0x01, 0x12, 0x55, 0x03, 0x61, 0x73, 0x63, 0x69, 0x69, 0x2e,
                                    0x39, 0x31, 0x31, 0x63, 0x68, 0x61, 0x2e, 0x63, 0x6f, 0x6d, 0x2f};

std::shared_ptr<TagEndPointMock> GetTagEndPointMock(int handler, std::string uid, std::string ndefMsg)
{
    // dispatch the tag of uid='123' and handler=1 and ndef message
    std::shared_ptr<TagEndPointMock> tag = std::make_shared<TagEndPointMock>();
    std::vector<int> techList = {1, 2};
    std::shared_ptr<NfcMap> techExtras;
    EXPECT_CALL(*tag, GetUid()).WillRepeatedly(Return(uid));
    EXPECT_CALL(*tag, GetTechList()).WillRepeatedly(Return(techList));
    EXPECT_CALL(*tag, GetTechExtras()).WillRepeatedly(Return(techExtras));
    EXPECT_CALL(*tag, GetHandle()).WillRepeatedly(Return(handler));
    EXPECT_CALL(*tag, IsPresent()).WillRepeatedly(Return(true));
    EXPECT_CALL(*tag, ReadNdef()).WillRepeatedly(Return(ndefMsg));
    EXPECT_CALL(*tag, Reconnect()).WillRepeatedly(Return(true));
    EXPECT_CALL(*tag, Disconnect()).WillRepeatedly(Return(true));
    EXPECT_CALL(*tag, StartPresenceChecking(_, _)).Times(AnyNumber());
    EXPECT_CALL(*tag, StopPresenceChecking()).Times(AnyNumber());
    return tag;
}

TEST(TagDispatcher, InitializeReaderParams_Test_1)
{
    std::shared_ptr<Context> context = std::make_shared<Context>();
    std::weak_ptr<Resources> resources = context->GetResources();
    resources.lock()->PutInt(R::toast_debounce_time_ms, 10001);
    resources.lock()->PutBool(R::enable_nfc_blocking_alert, true);
    resources.lock()->PutInt(R::nfc_blocking_count, 1);

    std::shared_ptr<NfcService> nfcService = std::make_shared<NfcService>(context);
    std::shared_ptr<TagDispatcher> tagDispatcher = std::make_shared<TagDispatcher>(context, nfcService);
    tagDispatcher->InitializeReaderParams();
}

TEST(TagDispatcher, InitializeReaderParams_Test_2)
{
    std::shared_ptr<Context> context = std::make_shared<Context>();
    std::weak_ptr<Resources> resources = context->GetResources();
    resources.lock()->PutInt(R::toast_debounce_time_ms, 1000);
    resources.lock()->PutBool(R::enable_nfc_blocking_alert, false);

    std::shared_ptr<NfcService> nfcService = std::make_shared<NfcService>(context);
    std::shared_ptr<TagDispatcher> tagDispatcher = std::make_shared<TagDispatcher>(context, nfcService);
    tagDispatcher->InitializeReaderParams();
}

// Tag is unexist
TEST(TagDispatcher, DispatchTag_Test)
{
    std::shared_ptr<Context> context = std::make_shared<Context>();
    std::shared_ptr<NfcService> nfcService = std::make_shared<NfcService>(context);
    std::shared_ptr<TagDispatcher> tagDispatcher = std::make_shared<TagDispatcher>(context, nfcService);
    std::shared_ptr<ITagEndPoint> tag;
    EXPECT_EQ(tagDispatcher->DispatchTag(tag), 0);
}

// No Reader Mode, Tag isnot the ndef tag or ndefmessage is null
// and To Reconnect Tag is failed
TEST_F(TagDispatcherTest, DispatchTag_Failed_1)
{
    std::shared_ptr<TagEndPointMock> tag = std::make_shared<TagEndPointMock>();
    EXPECT_CALL(*tag, ReadNdef()).WillRepeatedly(Return(""));
    EXPECT_CALL(*tag, Reconnect()).WillRepeatedly(Return(false));
    EXPECT_CALL(*tag, Disconnect()).WillRepeatedly(Return(true));

    // Test Class (TagDispatcher)
    std::shared_ptr<TagDispatcher> tagDispatcher = std::make_shared<TagDispatcher>(ctx, nfcService);
    tagDispatcher->DebounceMsgProcess();
    EXPECT_EQ(tagDispatcher->DispatchTag(tag), 0);
}

// No Reader Mode, Tag isnot the ndef tag or ndefmessage is null
// and To Reconnect Tag is success and The Ignore Tag is empty
TEST_F(TagDispatcherTest, DispatchTag_Failed_2)
{
    // dispatch the tag of uid='123' and handler=1 and ndef message is empty
    std::shared_ptr<TagEndPointMock> tag = GetTagEndPointMock(1, "123", "");
    // The parameters is to Dispatch is Failed.
    std::weak_ptr<Resources> resources = ctx->GetResources();
    resources.lock()->PutInt(R::unknown_tag_polling_delay, 1000);
    resources.lock()->PutBool(R::enable_notify_dispatch_failed, true);
    // nfc blocking alert
    resources.lock()->PutInt(R::toast_debounce_time_ms, 1000);
    resources.lock()->PutBool(R::enable_nfc_blocking_alert, true);
    resources.lock()->PutInt(R::nfc_blocking_count, -1);
    // Get Tag Dispatcher
    std::shared_ptr<INfcService> service = nfcService;
    if (!nfcService) {
        InfoLog("DispatchTag_Failed_2 : nfcService is empty.");
        return;
    }
    std::shared_ptr<TagDispatcher> dispatcher = service->GetTagDispatcher().lock();
    if (!dispatcher) {
        InfoLog("DispatchTag_Failed_2 : dispatcher is empty.");
        return;
    }
    // On Background Dispatcher
    dispatcher->DebounceMsgProcess();
    EXPECT_EQ(dispatcher->DispatchTag(tag), 2);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
}

// Reader Mode and front dispatch the tag
TEST_F(TagDispatcherTest, DispatchTag_Test_1)
{
    sptr<IRemoteObject> readerObj = new IPCObjectStub(u"ohos.nfc.service.TagDispatcherTest");
    std::shared_ptr<AppCallbackMock> appCallback = std::make_shared<AppCallbackMock>();
    EXPECT_CALL(*appCallback, OnTagDiscovered(_)).Times(1);
    std::shared_ptr<NfcMap> extra;
    // Get Tag Dispatcher
    std::shared_ptr<INfcService> service = nfcService;
    std::shared_ptr<TagDispatcher> dispatcher = service->GetTagDispatcher().lock();
    // active and FLAG_READER_SKIP_NDEF_CHECK and Paly Sound
    dispatcher->SetRWMode(readerObj, appCallback, 0x08F, extra);
    // there is the apply discovery
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    // dispatch the tag of uid='123' and handler=1 and ndef message
    std::shared_ptr<TagEndPointMock> tag = GetTagEndPointMock(1, "123", "");
    dispatcher->DebounceMsgProcess();
    EXPECT_EQ(dispatcher->DispatchTag(tag), 1);
}

// It set the ignore tag after It front dispatch the tag
// Then it dispath the ignore tag
TEST_F(TagDispatcherTest, DispatchTag_Test_2)
{
    sptr<IRemoteObject> readerObj = new IPCObjectStub(u"ohos.nfc.service.TagDispatcherTest");
    std::shared_ptr<AppCallbackMock> appCallback = std::make_shared<AppCallbackMock>();
    EXPECT_CALL(*appCallback, OnTagDiscovered(_)).Times(1);
    std::shared_ptr<NfcMap> extra = std::make_shared<NfcMap>();
    // Get Tag Dispatcher
    std::shared_ptr<INfcService> service = nfcService;
    std::shared_ptr<TagDispatcher> dispatcher = service->GetTagDispatcher().lock();
    // active and FLAG_READER_SKIP_NDEF_CHECK and Paly Sound
    dispatcher->SetRWMode(readerObj, appCallback, 0x0F, extra);
    // there is the apply discovery
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    // dispatch the tag of uid='123' and handler=1 and ndef message
    std::string ndef(data1.begin(), data1.end());
    std::shared_ptr<TagEndPointMock> tag = GetTagEndPointMock(1, "123", ndef);

    dispatcher->DebounceMsgProcess();
    EXPECT_EQ(dispatcher->DispatchTag(tag), 1);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    // RemovedCallbackMock
    std::shared_ptr<TagRemovedCallbackMock> removeCallback = std::make_shared<TagRemovedCallbackMock>();
    EXPECT_CALL(*removeCallback, OnTagRemoved()).Times(AnyNumber());
    dispatcher->Ignore(1, 3000, removeCallback);

    tag = GetTagEndPointMock(1, "123", ndef);
    dispatcher->DebounceMsgProcess();
    EXPECT_EQ(dispatcher->DispatchTag(tag), 0);
    std::this_thread::sleep_for(std::chrono::milliseconds(5000));
}

// It set the ignore tag after It front dispatch the tag
// Then it dispath the ignore tag
TEST_F(TagDispatcherTest, DispatchTag_Test_3)
{
    sptr<IRemoteObject> readerObj = new IPCObjectStub(u"ohos.nfc.service.TagDispatcherTest");
    std::shared_ptr<AppCallbackMock> appCallback = std::make_shared<AppCallbackMock>();
    EXPECT_CALL(*appCallback, OnTagDiscovered(_)).Times(AnyNumber());
    std::shared_ptr<NfcMap> extra = std::make_shared<NfcMap>();
    // Get Tag Dispatcher
    std::shared_ptr<INfcService> service = nfcService;
    std::shared_ptr<TagDispatcher> dispatcher = service->GetTagDispatcher().lock();
    // active and FLAG_READER_SKIP_NDEF_CHECK and Paly Sound
    dispatcher->SetRWMode(readerObj, appCallback, 0x0F, extra);
    // there is the apply discovery
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    // dispatch the tag of uid='123' and handler=1 and ndef message
    std::shared_ptr<TagEndPointMock> tag = GetTagEndPointMock(1, "123", "");
    dispatcher->DebounceMsgProcess();
    EXPECT_EQ(dispatcher->DispatchTag(tag), 1);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    // RemovedCallbackMock
    std::shared_ptr<TagRemovedCallbackMock> removeCallback = std::make_shared<TagRemovedCallbackMock>();
    EXPECT_CALL(*removeCallback, OnTagRemoved()).Times(AnyNumber());
    dispatcher->Ignore(1, 3000, removeCallback);

    tag = GetTagEndPointMock(2, "234", "");
    dispatcher->DebounceMsgProcess();
    EXPECT_EQ(dispatcher->DispatchTag(tag), 1);
    std::this_thread::sleep_for(std::chrono::milliseconds(5000));
}

TEST(TagDispatcher, ResetIgnoreTag_Test)
{
    std::shared_ptr<Context> context = std::make_shared<Context>();
    std::shared_ptr<NfcService> nfcService = std::make_shared<NfcService>(context);
    std::shared_ptr<TagDispatcher> tagDispatcher = std::make_shared<TagDispatcher>(context, nfcService);
    tagDispatcher->ResetIgnoreTag();
}

TEST(TagDispatcher, DebounceMsgProcess_Test)
{
    std::shared_ptr<Context> context = std::make_shared<Context>();
    std::shared_ptr<NfcService> nfcService = std::make_shared<NfcService>(context);
    std::shared_ptr<TagDispatcher> tagDispatcher = std::make_shared<TagDispatcher>(context, nfcService);
    tagDispatcher->DebounceMsgProcess();
}

TEST_F(TagDispatcherTest, MaybeDisconnectTarget_Test)
{
    sptr<IRemoteObject> readerObj = new IPCObjectStub(u"ohos.nfc.service.TagDispatcherTest");
    std::shared_ptr<AppCallbackMock> appCallback = std::make_shared<AppCallbackMock>();
    EXPECT_CALL(*appCallback, OnTagDiscovered(_)).Times(1);
    std::shared_ptr<NfcMap> extra = std::make_shared<NfcMap>();
    // Get Tag Dispatcher
    std::shared_ptr<INfcService> service = nfcService;
    std::shared_ptr<TagDispatcher> dispatcher = service->GetTagDispatcher().lock();
    // active and FLAG_READER_SKIP_NDEF_CHECK and Paly Sound
    dispatcher->SetRWMode(readerObj, appCallback, 0x08F, extra);
    // there is the apply discovery
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    // dispatch the tag of uid='123' and handler=1 and ndef message
    std::shared_ptr<TagEndPointMock> tag = GetTagEndPointMock(1, "123", "");
    EXPECT_EQ(dispatcher->DispatchTag(tag), 1);
    dispatcher->MaybeDisconnectTarget();
}

TEST_F(TagDispatcherTest, Ignore_Test)
{
    sptr<IRemoteObject> readerObj = new IPCObjectStub(u"ohos.nfc.service.TagDispatcherTest");
    std::shared_ptr<AppCallbackMock> appCallback = std::make_shared<AppCallbackMock>();
    EXPECT_CALL(*appCallback, OnTagDiscovered(_)).Times(AnyNumber());
    std::shared_ptr<NfcMap> extra = std::make_shared<NfcMap>();
    // Get Tag Dispatcher
    std::shared_ptr<INfcService> service = nfcService;
    std::shared_ptr<TagDispatcher> dispatcher = service->GetTagDispatcher().lock();
    // active and FLAG_READER_SKIP_NDEF_CHECK and Paly Sound
    dispatcher->SetRWMode(readerObj, appCallback, 0x01, extra);
    // there is the apply discovery
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    // dispatch the tag of uid='123' and handler=1 and ndef message
    std::shared_ptr<TagEndPointMock> tag = GetTagEndPointMock(1, "123", "");
    EXPECT_EQ(dispatcher->DispatchTag(tag), 1);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    // RemovedCallbackMock
    std::shared_ptr<TagRemovedCallbackMock> removeCallback = std::make_shared<TagRemovedCallbackMock>();
    EXPECT_CALL(*removeCallback, OnTagRemoved()).Times(AnyNumber());
    EXPECT_TRUE(dispatcher->Ignore(1, 3000, removeCallback));
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    // disable the Ignore Tag
    EXPECT_TRUE(dispatcher->Ignore(1, 0, removeCallback));
    std::this_thread::sleep_for(std::chrono::milliseconds(4000));
}

TEST_F(TagDispatcherTest, SetRWMode_Failed_Test)
{
    sptr<IRemoteObject> readerObj = nullptr;
    std::shared_ptr<IAppCallback> appCallback;
    int flags{0};
    std::shared_ptr<NfcMap> extras;
    // Get Tag Dispatcher
    std::weak_ptr<INfcService> service = nfcService;
    std::weak_ptr<TagDispatcher> dispatcher = service.lock()->GetTagDispatcher();
    dispatcher.lock()->SetRWMode(readerObj, appCallback, flags, extras);
}

TEST_F(TagDispatcherTest, SetRWMode_Test)
{
    sptr<IRemoteObject> readerObj = new IPCObjectStub(u"ohos.nfc.service.dispatcher");
    std::shared_ptr<AppCallbackMock> appCallback = std::make_shared<AppCallbackMock>();
    EXPECT_CALL(*appCallback, OnTagDiscovered(_)).Times(1);
    std::shared_ptr<NfcMap> extras;
    // Get Tag Dispatcher
    std::shared_ptr<INfcService> service = nfcService;
    std::shared_ptr<TagDispatcher> dispatcher = service->GetTagDispatcher().lock();
    //  extras is empty
    dispatcher->SetRWMode(readerObj, appCallback, 1, extras);
    // extras is unempty
    extras = std::make_shared<NfcMap>();
    extras->PutLong("presence", 100);
    dispatcher->SetRWMode(readerObj, appCallback, 0x081, extras);
    // there is the apply discovery
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    // dispatch the tag of uid='123' and handler=1 and ndef message
    std::shared_ptr<TagEndPointMock> tag = GetTagEndPointMock(1, "123", "");

    EXPECT_EQ(dispatcher->DispatchTag(tag), 1);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    // deactive reader mode
    dispatcher->SetRWMode(readerObj, appCallback, 0, extras);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
}

TEST_F(TagDispatcherTest, SetFrontDispatchss_Test)
{
    std::weak_ptr<INfcService> service = nfcService;
    std::weak_ptr<TagDispatcher> dispatcher = service.lock()->GetTagDispatcher();

    std::shared_ptr<void> obj;
    dispatcher.lock()->SetFrontDispatch(obj);
}

TEST_F(TagDispatcherTest, BinderDied_Test)
{
    std::weak_ptr<INfcService> service = nfcService;
    std::weak_ptr<TagDispatcher> dispatcher = service.lock()->GetTagDispatcher();
    dispatcher.lock()->BinderDied();
}

TEST_F(TagDispatcherTest, ResumeAppSwitches_Test)
{
    std::weak_ptr<INfcService> service = nfcService;
    std::weak_ptr<TagDispatcher> dispatcher = service.lock()->GetTagDispatcher();
    dispatcher.lock()->ResumeAppSwitches();
}
