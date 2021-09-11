#include "tag_session.h"

#include <gtest/gtest.h>

#include <chrono>
#include <vector>

#include "context.h"
#include "key_guard_manager.h"
#include "nfc_agent_service.h"
#include "nfc_map.h"
#include "shared_preferences.h"
// nfc service
#include "device_host.h"
#include "ipc_object_stub.h"
#include "nfc_service.h"
#include "nfc_service_define.h"
#include "tag_dispatcher.h"
// nfc sdk
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
using OHOS::nfc::sdk::NfcErrorCode;

using namespace OHOS::nfc::sdk;
using ::testing::Return;
namespace {
class TagSessionTest : public ::testing::Test {
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
        // Tag Session
        EXPECT_CALL(*deviceHost, CanMakeReadOnly(_)).WillRepeatedly(Return(true));
        EXPECT_CALL(*deviceHost, GetIsoDepMaxTransceiveLength()).WillRepeatedly(Return(256));
        EXPECT_CALL(*deviceHost, GetExtendedLengthApdusSupported()).WillRepeatedly(Return(true));
        nfcService = std::make_shared<NfcService>(ctx, std::move(deviceHost));
        EXPECT_TRUE(nfcService->Initialize());
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        // nfc agent service
        nfcAgentService = std::make_shared<NfcAgentService>(nfcService, ctx);
        // tag session
        ts = std::make_shared<TagSession>(nfcService, ctx);
    }

    virtual void TearDown() override
    {
        std::cout << " TearDownTestCase." << std::endl;
        ts = nullptr;
        deviceHost = nullptr;
        nfcService = nullptr;
        ctx = nullptr;
    }

public:
    std::shared_ptr<Context> ctx;
    std::shared_ptr<NfcService> nfcService;
    std::unique_ptr<DeviceHostMock> deviceHost;
    std::shared_ptr<NfcAgentService> nfcAgentService;
    std::shared_ptr<TagEndPointMock> tagEndPoint;
    // Test Class
    std::shared_ptr<TagSession> ts;
};
}  // namespace

std::shared_ptr<TagEndPointMock> GetTagEndPointMock(int handle, bool isPre, bool isCon, bool isRe, int res)
{
    // dispatch the tag of uid='123' and handler=1 and ndef message
    std::shared_ptr<TagEndPointMock> tag = std::make_shared<TagEndPointMock>();
    std::vector<int> techList = {1, 2};
    std::shared_ptr<NfcMap> techExtras;
    EXPECT_CALL(*tag, GetUid()).WillRepeatedly(Return("123"));
    EXPECT_CALL(*tag, GetTechList()).WillRepeatedly(Return(techList));
    EXPECT_CALL(*tag, GetTechExtras()).WillRepeatedly(Return(techExtras));
    EXPECT_CALL(*tag, GetHandle()).WillRepeatedly(Return(handle));
    EXPECT_CALL(*tag, Connect(_)).WillRepeatedly(Return(isCon));
    EXPECT_CALL(*tag, Reconnect()).WillRepeatedly(Return(isRe));
    EXPECT_CALL(*tag, Disconnect()).WillRepeatedly(Return(true));
    EXPECT_CALL(*tag, IsPresent()).WillRepeatedly(Return(isPre));
    EXPECT_CALL(*tag, CheckNdef(_)).WillRepeatedly(Return(isPre));
    EXPECT_CALL(*tag, Transceive(_, _)).WillRepeatedly(Return(res));
    EXPECT_CALL(*tag, GetConnectedTechnology()).WillRepeatedly(Return(1));
    EXPECT_CALL(*tag, ReadNdef()).WillRepeatedly(Return(""));
    EXPECT_CALL(*tag, WriteNdef(_)).WillRepeatedly(Return(false));
    EXPECT_CALL(*tag, MakeReadOnly()).WillRepeatedly(Return(false));
    EXPECT_CALL(*tag, FormatNdef(_)).WillRepeatedly(Return(false));
    EXPECT_CALL(*tag, StartPresenceChecking(_, _)).Times(AnyNumber());
    EXPECT_CALL(*tag, StopPresenceChecking()).Times(AnyNumber());
    return tag;
}

std::shared_ptr<TagEndPointMock> GetNdefTagEndPointMock(
    int handle, std::string ndef, bool isWrite, bool isRead, bool isFormat)
{
    // dispatch the tag of uid='123' and handler=1 and ndef message
    std::shared_ptr<TagEndPointMock> tag = std::make_shared<TagEndPointMock>();
    std::vector<int> techList = {1, 2};
    std::shared_ptr<NfcMap> techExtras;
    EXPECT_CALL(*tag, GetUid()).WillRepeatedly(Return("123"));
    EXPECT_CALL(*tag, GetTechList()).WillRepeatedly(Return(techList));
    EXPECT_CALL(*tag, GetTechExtras()).WillRepeatedly(Return(techExtras));
    EXPECT_CALL(*tag, GetHandle()).WillRepeatedly(Return(handle));
    EXPECT_CALL(*tag, Connect(_)).WillRepeatedly(Return(true));
    EXPECT_CALL(*tag, Reconnect()).WillRepeatedly(Return(true));
    EXPECT_CALL(*tag, Disconnect()).WillRepeatedly(Return(true));
    EXPECT_CALL(*tag, IsPresent()).WillRepeatedly(Return(true));
    EXPECT_CALL(*tag, CheckNdef(_)).WillRepeatedly(Return(true));
    EXPECT_CALL(*tag, Transceive(_, _)).WillRepeatedly(Return(1));
    EXPECT_CALL(*tag, GetConnectedTechnology()).WillRepeatedly(Return(1));
    EXPECT_CALL(*tag, ReadNdef()).WillRepeatedly(Return(ndef));
    EXPECT_CALL(*tag, WriteNdef(_)).WillRepeatedly(Return(isWrite));
    EXPECT_CALL(*tag, MakeReadOnly()).WillRepeatedly(Return(isRead));
    EXPECT_CALL(*tag, FormatNdef(_)).WillRepeatedly(Return(isFormat));
    EXPECT_CALL(*tag, StartPresenceChecking(_, _)).Times(AnyNumber());
    EXPECT_CALL(*tag, StopPresenceChecking()).Times(AnyNumber());
    return tag;
}

void MockOnTagDiscovered(std::shared_ptr<NfcService> nfcService, std::shared_ptr<TagEndPointMock> tag)
{
    sptr<IRemoteObject> readerObj = new IPCObjectStub(u"ohos.nfc.service.TagSessionTest");
    std::shared_ptr<AppCallbackMock> appCallback = std::make_shared<AppCallbackMock>();
    EXPECT_CALL(*appCallback, OnTagDiscovered(_)).Times(1);
    std::shared_ptr<NfcMap> extras;

    // Get Tag Dispatcher
    std::shared_ptr<INfcService> service = nfcService;
    std::shared_ptr<TagDispatcher> dispatcher = service->GetTagDispatcher().lock();
    // active and FLAG_READER_SKIP_NDEF_CHECK and Paly Sound
    dispatcher->SetRWMode(readerObj, appCallback, 0x081, extras);
    // there is the apply discovery
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    EXPECT_EQ(dispatcher->DispatchTag(tag), 1);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
}

TEST_F(TagSessionTest, Connect_Test)
{
    // NFC_SER_ERROR_NOT_INITIALIZED
    EXPECT_EQ(ts->Connect(0, 0), NfcErrorCode::NFC_SER_ERROR_NOT_INITIALIZED);
    nfcAgentService->TurnOn();
    std::this_thread::sleep_for(std::chrono::milliseconds(300));
    // tag.expired
    EXPECT_EQ(ts->Connect(0, 0), NfcErrorCode::NFC_SER_ERROR_DISCONNECT);
    // mock No Present Tag
    std::shared_ptr<TagEndPointMock> tag = GetTagEndPointMock(0, false, true, true, 1);
    MockOnTagDiscovered(nfcService, tag);
    // NFC_SUCCESS
    EXPECT_EQ(ts->Connect(0, 0), NfcErrorCode::NFC_SER_ERROR_DISCONNECT);
    // mock Present Tag / To connect the tag is failed
    tag = GetTagEndPointMock(1, true, false, true, 1);
    MockOnTagDiscovered(nfcService, tag);
    // NFC_SUCCESS
    EXPECT_EQ(ts->Connect(1, 0), NfcErrorCode::NFC_SER_ERROR_DISCONNECT);
    // mock Present Tag / To connect the tag is success
    tag = GetTagEndPointMock(2, true, true, true, 1);
    MockOnTagDiscovered(nfcService, tag);
    // NFC_SUCCESS
    EXPECT_EQ(ts->Connect(2, 0), NfcErrorCode::NFC_SUCCESS);
}

TEST_F(TagSessionTest, Reconnect_Test)
{
    EXPECT_EQ(ts->Reconnect(0), NfcErrorCode::NFC_SER_ERROR_NOT_INITIALIZED);
    nfcAgentService->TurnOn();
    std::this_thread::sleep_for(std::chrono::milliseconds(300));
    // tag.expired
    EXPECT_EQ(ts->Reconnect(0), NfcErrorCode::NFC_SER_ERROR_DISCONNECT);
    // mock Present Tag / To reconnect the tag is failed
    std::shared_ptr<TagEndPointMock> tag = GetTagEndPointMock(0, true, true, false, 1);
    MockOnTagDiscovered(nfcService, tag);
    // NFC_SER_ERROR_DISCONNECT
    EXPECT_EQ(ts->Reconnect(0), NfcErrorCode::NFC_SER_ERROR_DISCONNECT);
    // mock Present Tag / To reconnect the tag is success
    tag = GetTagEndPointMock(1, true, true, true, 1);
    MockOnTagDiscovered(nfcService, tag);
    // NFC_SER_ERROR_DISCONNECT
    EXPECT_EQ(ts->Reconnect(1), NfcErrorCode::NFC_SUCCESS);
}

TEST_F(TagSessionTest, Disconnect_Test)
{
    ts->Disconnect(0);
    nfcAgentService->TurnOn();
    std::this_thread::sleep_for(std::chrono::milliseconds(300));
    // tag.expired
    ts->Disconnect(0);
    // mock Present Tag / To reconnect the tag is success
    std::shared_ptr<TagEndPointMock> tag = GetTagEndPointMock(0, true, true, true, 1);
    MockOnTagDiscovered(nfcService, tag);
    ts->Disconnect(0);
}

TEST_F(TagSessionTest, GetTechList_Test)
{
    unsigned int comp{0};
    EXPECT_EQ(ts->GetTechList(0).size(), comp);
    nfcAgentService->TurnOn();
    std::this_thread::sleep_for(std::chrono::milliseconds(300));
    // tag.expired
    EXPECT_EQ(ts->GetTechList(0).size(), comp);
    // mock Present Tag / To reconnect the tag is success
    std::shared_ptr<TagEndPointMock> tag = GetTagEndPointMock(0, true, true, true, 1);
    MockOnTagDiscovered(nfcService, tag);
    comp = 2;
    EXPECT_EQ(ts->GetTechList(0).size(), comp);
}

TEST_F(TagSessionTest, IsPresent_Test)
{
    EXPECT_EQ(ts->IsPresent(0), false);
    nfcAgentService->TurnOn();
    std::this_thread::sleep_for(std::chrono::milliseconds(300));
    // tag.expired
    EXPECT_EQ(ts->IsPresent(0), false);
    // mock Present Tag / To reconnect the tag is success
    std::shared_ptr<TagEndPointMock> tag = GetTagEndPointMock(0, true, true, true, 1);
    MockOnTagDiscovered(nfcService, tag);
    EXPECT_EQ(ts->IsPresent(0), true);
    // mock no Present Tag / To reconnect the tag is success
    tag = GetTagEndPointMock(1, false, true, true, 1);
    MockOnTagDiscovered(nfcService, tag);
    EXPECT_EQ(ts->IsPresent(1), false);
}

TEST_F(TagSessionTest, IsNdef_Test)
{
    EXPECT_EQ(ts->IsNdef(0), false);
    nfcAgentService->TurnOn();
    std::this_thread::sleep_for(std::chrono::milliseconds(300));
    // tag.expired
    EXPECT_EQ(ts->IsNdef(0), false);
    // mock Present Tag / To reconnect the tag is success
    std::shared_ptr<TagEndPointMock> tag = GetTagEndPointMock(0, true, true, true, 1);
    MockOnTagDiscovered(nfcService, tag);
    EXPECT_EQ(ts->IsNdef(0), true);
}

TEST_F(TagSessionTest, SendRawFrame_Failed)
{
    EXPECT_FALSE(ts->SendRawFrame(0, "", true));
    nfcAgentService->TurnOn();
    std::this_thread::sleep_for(std::chrono::milliseconds(300));
    // tag.expired
    EXPECT_FALSE(ts->SendRawFrame(0, "", true));
    // mock Present Tag / To reconnect the tag is success
    std::shared_ptr<TagEndPointMock> tag = GetTagEndPointMock(0, true, true, true, 1);
    MockOnTagDiscovered(nfcService, tag);
    // RESULT_EXCEEDED_LENGTH
    std::vector<unsigned char> temp(500);
    std::string data1(temp.begin(), temp.end());
    std::unique_ptr<ResResult> resResult = ts->SendRawFrame(0, data1, true);
    EXPECT_EQ(resResult->GetResult(), ResResult::RESULT_EXCEEDED_LENGTH);
    // RESULT_TAGLOST
    std::vector<unsigned char> temp2(123);
    std::string data2(temp2.begin(), temp2.end());
    resResult = ts->SendRawFrame(0, data2, true);
    EXPECT_EQ(resResult->GetResult(), ResResult::RESULT_TAGLOST);
    // RESULT_FAILURE
    // mock Present Tag / To reconnect the tag is success
    tag = GetTagEndPointMock(1, true, true, true, 2);
    MockOnTagDiscovered(nfcService, tag);
    resResult = ts->SendRawFrame(1, data2, true);
    EXPECT_EQ(resResult->GetResult(), ResResult::RESULT_FAILURE);
}

TEST_F(TagSessionTest, SendRawFrame_SUCCESS)
{
    nfcAgentService->TurnOn();
    std::this_thread::sleep_for(std::chrono::milliseconds(300));
    // RESULT_SUCCESS
    TagEndPointMock* tag = new TagEndPointMock();
    ON_CALL(*tag, Transceive(_, _)).WillByDefault([tag](std::string& request, std::string& response) {
        response = "123";
        return 0;
    });
    std::vector<int> techList = {1, 2};
    std::shared_ptr<NfcMap> techExtras;
    EXPECT_CALL(*tag, GetUid()).WillRepeatedly(Return("123"));
    EXPECT_CALL(*tag, GetTechList()).WillRepeatedly(Return(techList));
    EXPECT_CALL(*tag, GetTechExtras()).WillRepeatedly(Return(techExtras));
    EXPECT_CALL(*tag, GetHandle()).WillRepeatedly(Return(1));
    EXPECT_CALL(*tag, Connect(_)).WillRepeatedly(Return(true));
    EXPECT_CALL(*tag, Reconnect()).WillRepeatedly(Return(true));
    EXPECT_CALL(*tag, Disconnect()).WillRepeatedly(Return(true));
    EXPECT_CALL(*tag, IsPresent()).WillRepeatedly(Return(true));
    EXPECT_CALL(*tag, CheckNdef(_)).WillRepeatedly(Return(true));
    EXPECT_CALL(*tag, Transceive(_, _));
    EXPECT_CALL(*tag, GetConnectedTechnology()).WillRepeatedly(Return(1));
    EXPECT_CALL(*tag, ReadNdef()).WillRepeatedly(Return(""));
    EXPECT_CALL(*tag, WriteNdef(_)).WillRepeatedly(Return(false));
    EXPECT_CALL(*tag, MakeReadOnly()).WillRepeatedly(Return(false));
    EXPECT_CALL(*tag, FormatNdef(_)).WillRepeatedly(Return(false));
    EXPECT_CALL(*tag, StartPresenceChecking(_, _)).Times(AnyNumber());
    EXPECT_CALL(*tag, StopPresenceChecking()).Times(AnyNumber());
    MockOnTagDiscovered(nfcService, std::shared_ptr<TagEndPointMock>(tag));

    std::vector<unsigned char> temp2(123);
    std::string data2(temp2.begin(), temp2.end());
    std::unique_ptr<ResResult> resResult = ts->SendRawFrame(1, data2, true);
    EXPECT_EQ(resResult->GetResult(), ResResult::RESULT_SUCCESS);
    std::weak_ptr<INfcService> service = nfcService;
    std::weak_ptr<TagDispatcher> dispatcher = service.lock()->GetTagDispatcher();
    dispatcher.lock()->MaybeDisconnectTarget();
    EXPECT_TRUE(Mock::VerifyAndClearExpectations(tag));
}

// Nfdef Data
std::vector<unsigned char> data = {0xd1, 0x01, 0x12, 0x55, 0x03, 0x61, 0x73, 0x63, 0x69, 0x69, 0x2e,
                                   0x39, 0x31, 0x31, 0x63, 0x68, 0x61, 0x2e, 0x63, 0x6f, 0x6d, 0x2f};
TEST_F(TagSessionTest, NdefRead_Test)
{
    EXPECT_STREQ(ts->NdefRead(0).c_str(), "");
    nfcAgentService->TurnOn();
    std::this_thread::sleep_for(std::chrono::milliseconds(300));
    // tag.expired
    EXPECT_STREQ(ts->NdefRead(0).c_str(), "");
    // mock Present Tag / To reconnect the tag is success
    std::string data1(data.begin(), data.end());
    std::shared_ptr<TagEndPointMock> tag = GetNdefTagEndPointMock(0, data1, true, true, 1);
    MockOnTagDiscovered(nfcService, tag);
    EXPECT_STREQ(ts->NdefRead(0).c_str(), data1.c_str());
}

TEST_F(TagSessionTest, NdefWrite_Test)
{
    EXPECT_EQ(ts->NdefWrite(0, ""), NfcErrorCode::NFC_SER_ERROR_NOT_INITIALIZED);
    nfcAgentService->TurnOn();
    std::this_thread::sleep_for(std::chrono::milliseconds(300));
    // tag.expired
    EXPECT_EQ(ts->NdefWrite(0, ""), NfcErrorCode::NFC_SER_ERROR_IO);
    // mock Present Tag / To reconnect the tag is success
    std::string data1(data.begin(), data.end());
    std::shared_ptr<TagEndPointMock> tag = GetNdefTagEndPointMock(0, data1, true, true, 1);
    MockOnTagDiscovered(nfcService, tag);
    std::string msg;
    EXPECT_EQ(ts->NdefWrite(0, msg), NfcErrorCode::NFC_SER_ERROR_INVALID_PARAM);
    // msg is unempty
    msg = "123";
    EXPECT_EQ(ts->NdefWrite(0, msg), NFC_SUCCESS);
    //
    tag = GetNdefTagEndPointMock(1, data1, false, true, 1);
    MockOnTagDiscovered(nfcService, tag);
    EXPECT_EQ(ts->NdefWrite(1, msg), NfcErrorCode::NFC_SER_ERROR_IO);
}

TEST_F(TagSessionTest, NdefMakeReadOnly_Test)
{
    EXPECT_EQ(ts->NdefMakeReadOnly(0), NfcErrorCode::NFC_SER_ERROR_NOT_INITIALIZED);
    nfcAgentService->TurnOn();
    std::this_thread::sleep_for(std::chrono::milliseconds(300));
    // tag.expired
    EXPECT_EQ(ts->NdefMakeReadOnly(0), NfcErrorCode::NFC_SER_ERROR_IO);
    // NFC_SUCCESS
    std::string data1(data.begin(), data.end());
    std::shared_ptr<TagEndPointMock> tag = GetNdefTagEndPointMock(0, data1, true, true, 1);
    MockOnTagDiscovered(nfcService, tag);
    EXPECT_EQ(ts->NdefMakeReadOnly(0), NFC_SUCCESS);
    // NFC_SER_ERROR_IO
    tag = GetNdefTagEndPointMock(1, data1, true, false, 1);
    MockOnTagDiscovered(nfcService, tag);
    EXPECT_EQ(ts->NdefMakeReadOnly(1), NfcErrorCode::NFC_SER_ERROR_IO);
}

TEST_F(TagSessionTest, FormatNdef_Test)
{
    EXPECT_EQ(ts->FormatNdef(0, ""), NfcErrorCode::NFC_SER_ERROR_NOT_INITIALIZED);
    nfcAgentService->TurnOn();
    std::this_thread::sleep_for(std::chrono::milliseconds(300));
    // tag.expired
    EXPECT_EQ(ts->FormatNdef(0, ""), NfcErrorCode::NFC_SER_ERROR_IO);
    // NFC_SUCCESS
    std::string data1(data.begin(), data.end());
    std::shared_ptr<TagEndPointMock> tag = GetNdefTagEndPointMock(0, data1, true, true, true);
    MockOnTagDiscovered(nfcService, tag);
    EXPECT_EQ(ts->FormatNdef(0, ""), NFC_SUCCESS);
    // NFC_SER_ERROR_IO
    tag = GetNdefTagEndPointMock(1, data1, true, true, false);
    MockOnTagDiscovered(nfcService, tag);
    EXPECT_EQ(ts->FormatNdef(1, ""), NfcErrorCode::NFC_SER_ERROR_IO);
}

TEST_F(TagSessionTest, CanMakeReadOnly_Test)
{
    EXPECT_TRUE(ts->CanMakeReadOnly(0));
}

TEST_F(TagSessionTest, GetMaxTransceiveLength_Test)
{
    EXPECT_EQ(ts->GetMaxTransceiveLength(0), 256);
}

TEST_F(TagSessionTest, IsSupportedApdusExtended_Test)
{
    EXPECT_TRUE(ts->IsSupportedApdusExtended());
}