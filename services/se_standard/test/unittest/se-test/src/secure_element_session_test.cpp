/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#define private public
#include "secure_element_session.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>

#include "base_test.h"
#include "bundle_manager.h"
#include "context.h"
#include "ipc_object_stub.h"
#include "ipc_skeleton.h"
#include "isecure_element_mock.h"
#include "loghelper.h"
#include "se_common_exception.h"
#include "secure_element_channel.h"
#include "secure_element_reader.h"
#include "secure_element_service.h"
#include "terminal.h"
#undef private

using namespace testing;
using namespace OHOS::se::omapi;
using namespace osal;
using OHOS::hardware::se::v1_0::ISecureElementCallback;
using OHOS::hardware::se::v1_0::ISecureElementMock;
namespace OHOS {
namespace se {
namespace test {
class SecureElementSessionTest : public ::testing::Test {
public:
    virtual void SetUp() override
    {
        context_ = std::make_shared<Context>();
        seMock_ = BaseTest::LoadSecureElement(true);
        terminalTable_ = BaseTest::LoadTerminalTable(context_, seMock_);
        InfoLog("==SetUp==");
        seServiceManager_ = std::make_shared<SeEndService>();
        seServiceManager_->Init(context_);
        seService_ = new SecureElementService(seServiceManager_, terminalTable_);
        reader_ = new SecureElementReader(seServiceManager_, terminalTable_[0].second);
        seSession_ = (SecureElementSession*)(reader_->OpenSession().GetRefPtr());
        EXPECT_TRUE(seSession_ != nullptr);
        EXPECT_FALSE(seSession_->IsClosed());
    }

    virtual void TearDown() override
    {
        InfoLog("==TearDown==");
        context_ = std::shared_ptr<Context>();
        seService_ = nullptr;
        seServiceManager_ = nullptr;
        seMock_ = nullptr;
        terminalTable_.clear();
        reader_ = nullptr;
    }

public:
    sptr<SecureElementSession> seSession_;
    sptr<SecureElementReader> reader_;
    sptr<SecureElementService> seService_;
    std::shared_ptr<SeEndService> seServiceManager_;
    sptr<ISecureElementMock> seMock_;
    std::shared_ptr<Context> context_;
    TerminalTable terminalTable_;
};

TEST_F(SecureElementSessionTest, GetATR_Test)
{
    std::string response{0x00, 0x00, (char)0x90, 0x00};
    EXPECT_CALL(*seMock_, GetAtr()).WillRepeatedly(Return(response));
    try {
        std::string atr = seSession_->GetATR();
        EXPECT_TRUE(0 == atr.compare(response));
    } catch (const std::exception&) {
        EXPECT_TRUE(false);
    }
}

TEST_F(SecureElementSessionTest, Close_Test)
{
    sptr<ISESession> session = reader_->OpenSession();
    EXPECT_TRUE(session != nullptr);
    EXPECT_FALSE(session->IsClosed());
    // close session
    session->Close();
    EXPECT_TRUE(session->IsClosed());
}

/**
 * @brief Check about OpenBasicChannel exception 1
 * 1. Checking the parameter p2 isn't 0x00/0x04/0x08/0x0C.
 * 2. Checking Open Basic Channel when the session is closed.
 */
TEST_F(SecureElementSessionTest, OpenBasicChannel_Test_1)
{
    std::string aid{};
    sptr<ISEChannel> seChannel = nullptr;
    sptr<ISESession> session = reader_->OpenSession();
    EXPECT_TRUE(session != nullptr);
    EXPECT_FALSE(session->IsClosed());

    int p2 = 20;
    try {
        seChannel = session->OpenBasicChannel(aid, p2);
        EXPECT_FALSE(true);
    } catch (const UnsupportedOperationError& e) {
        EXPECT_TRUE(true);
    } catch (const std::exception& e) {
        EXPECT_FALSE(true);
    }

    // close session
    session->Close();
    EXPECT_TRUE(session->IsClosed());
    try {
        session->OpenBasicChannel(aid, 0);
        EXPECT_FALSE(true);
    } catch (const IllegalStateError& e) {
        EXPECT_TRUE(true);
    } catch (const std::exception& e) {
        EXPECT_FALSE(true);
    }
}

/**
 * @brief Check about OpenBasicChannel exception 2
 * 1. Checking the related bundle name of the calling uid is null.
 * 2. Checking the parameter aid length is less then 5 and bigger then 16.
 * 3. Checking the response data of  OpenBasicChannel from HAL Service.
 */
TEST_F(SecureElementSessionTest, OpenBasicChannel_Test_2)
{
    std::string aid{};
    sptr<ISEChannel> seChannel = nullptr;
    sptr<ISESession> session = reader_->OpenSession();
    EXPECT_TRUE(session != nullptr);
    EXPECT_FALSE(session->IsClosed());

    std::shared_ptr<osal::BundleManager> pkgMng = context_->GetBundleManager().lock();
    pkgMng->RemoveBundlesForUid(OHOS::IPCSkeleton::GetCallingUid());
    try {
        seChannel = session->OpenBasicChannel(aid, 4);
        EXPECT_FALSE(true);
    } catch (const AccessControlError& e) {
        EXPECT_TRUE(true);
    } catch (const std::exception& e) {
        EXPECT_FALSE(true);
    }

    std::string pkgName{"1234567890"};
    BaseTest::SetUpBundleInfo(pkgName, context_);
    aid = "1234";
    try {
        seChannel = session->OpenBasicChannel(aid, 4);
        EXPECT_FALSE(true);
    } catch (const IllegalArgumentError& e) {
        EXPECT_TRUE(true);
    } catch (const std::exception& e) {
        EXPECT_FALSE(true);
    }

    aid = "1234567890abcdef1";
    try {
        seChannel = session->OpenBasicChannel(aid, 8);
        EXPECT_FALSE(true);
    } catch (const IllegalArgumentError& e) {
        EXPECT_TRUE(true);
    } catch (const std::exception& e) {
        EXPECT_FALSE(true);
    }

    BaseTest::SetUpOpenLogicalChannel(seMock_);
    std::unique_ptr<SEBasicRespData> respData{};
    EXPECT_CALL(*seMock_, OpenBasicChannel(_, _)).WillRepeatedly(Return(ByMove(std::move(respData))));
    aid = "12345";
    try {
        seChannel = session->OpenBasicChannel(aid, 0x0c);
        EXPECT_FALSE(true);
    } catch (const ServiceSpecificError& e) {
        EXPECT_TRUE(true);
    } catch (const std::exception& e) {
        EXPECT_FALSE(true);
    }

    BaseTest::SetUpOpenLogicalChannel(seMock_);
    respData = std::make_unique<SEBasicRespData>();
    respData->status = SEStatus::CHANNEL_NOT_AVAILABLE;
    EXPECT_CALL(*seMock_, OpenBasicChannel(_, _)).WillRepeatedly(Return(ByMove(std::move(respData))));
    aid = "12345";
    try {
        seChannel = session->OpenBasicChannel(aid, 0x0c);
        EXPECT_TRUE(!seChannel);
    } catch (const std::exception& e) {
        EXPECT_FALSE(true);
    }

    BaseTest::SetUpOpenLogicalChannel(seMock_);
    respData = std::make_unique<SEBasicRespData>();
    respData->status = SEStatus::UNSUPPORTED_OPERATION;
    EXPECT_CALL(*seMock_, OpenBasicChannel(_, _)).WillRepeatedly(Return(ByMove(std::move(respData))));
    aid = "12345";
    try {
        seChannel = session->OpenBasicChannel(aid, 0x0c);
        EXPECT_FALSE(true);
    } catch (const UnsupportedOperationError& e) {
        EXPECT_TRUE(true);
    } catch (const std::exception& e) {
        EXPECT_FALSE(true);
    }

    BaseTest::SetUpOpenLogicalChannel(seMock_);
    respData = std::make_unique<SEBasicRespData>();
    respData->status = SEStatus::IO_ERROR;
    EXPECT_CALL(*seMock_, OpenBasicChannel(_, _)).WillRepeatedly(Return(ByMove(std::move(respData))));
    aid = "12345";
    try {
        seChannel = session->OpenBasicChannel(aid, 0x0c);
        EXPECT_FALSE(true);
    } catch (const ServiceSpecificError& e) {
        EXPECT_TRUE(true);
    } catch (const std::exception& e) {
        EXPECT_FALSE(true);
    }

    BaseTest::SetUpOpenLogicalChannel(seMock_);
    respData = std::make_unique<SEBasicRespData>();
    respData->status = SEStatus::NO_SUCH_SE_ERROR;
    EXPECT_CALL(*seMock_, OpenBasicChannel(_, _)).WillRepeatedly(Return(ByMove(std::move(respData))));
    aid = "12345";
    try {
        seChannel = session->OpenBasicChannel(aid, 0x0c);
        EXPECT_FALSE(true);
    } catch (const ServiceSpecificError& e) {
        EXPECT_TRUE(true);
    } catch (const std::exception& e) {
        EXPECT_FALSE(true);
    }
}

/**
 * @brief Check about OpenBasicChannel 3
 * 1. Checking Open Basic Channel.
 * 2. Checking Repeat opening Basic Channel.
 */
TEST_F(SecureElementSessionTest, OpenBasicChannel_Test_3)
{
    std::string aid{};
    sptr<ISEChannel> seChannel = nullptr;
    sptr<ISESession> session = reader_->OpenSession();
    EXPECT_TRUE(session != nullptr);
    EXPECT_FALSE(session->IsClosed());

    //  the related the bundle name
    std::string pkgName{"1234567890"};
    BaseTest::SetUpBundleInfo(pkgName, context_);
    // the response data of OpenBasicChannel
    BaseTest::SetUpOpenLogicalChannel(seMock_);
    std::unique_ptr<SEBasicRespData> respData = std::make_unique<SEBasicRespData>();
    respData->status = SEStatus::SUCCESS;
    EXPECT_CALL(*seMock_, OpenBasicChannel(_, _)).WillRepeatedly(Return(ByMove(std::move(respData))));
    aid = "12345";
    try {
        seChannel = session->OpenBasicChannel(aid, 0x00);
        EXPECT_TRUE(seChannel != nullptr);
    } catch (const std::exception& e) {
        EXPECT_FALSE(true);
    }

    // repeat opening basic channel
    BaseTest::SetUpOpenLogicalChannel(seMock_);
    sptr<ISEChannel> repeatChannel;
    aid = "";
    try {
        repeatChannel = session->OpenBasicChannel(aid, 0x04);
        EXPECT_TRUE(!repeatChannel);
    } catch (const std::exception& e) {
        EXPECT_FALSE(true);
    }

    session = reader_->OpenSession();
    EXPECT_TRUE(session != nullptr);
    EXPECT_FALSE(session->IsClosed());

    std::string response{(char)0x90, 0x00};
    EXPECT_CALL(*seMock_, Transmit(_)).WillRepeatedly(Return(response));
    EXPECT_CALL(*seMock_, CloseChannel(_)).WillRepeatedly(Return(SEStatus::SUCCESS));
    seChannel->Close();
    EXPECT_TRUE(seChannel->IsClosed());

    BaseTest::SetUpOpenLogicalChannel(seMock_);
    aid = "";
    try {
        repeatChannel = session->OpenBasicChannel(aid, 0x04);
        EXPECT_TRUE(!repeatChannel);
    } catch (const std::exception& e) {
        EXPECT_FALSE(true);
    }

    BaseTest::SetUpOpenLogicalChannel(seMock_);
    respData = std::make_unique<SEBasicRespData>();
    respData->status = SEStatus::SUCCESS;
    EXPECT_CALL(*seMock_, OpenBasicChannel(_, _)).WillRepeatedly(Return(ByMove(std::move(respData))));
    aid = "12345";
    try {
        repeatChannel = session->OpenBasicChannel(aid, 0x04);
        EXPECT_TRUE(repeatChannel != nullptr);
    } catch (const std::exception& e) {
        EXPECT_FALSE(true);
    }
}

/**
 * @brief Check about OpenLogicalChannel exception 1
 * 1. Checking AID is empty when the reader is UICC.
 * 2. Checking the parameter p2 isn't 0x00/0x04/0x08/0x0C.
 * 3. Checking Open Logical Channel when the session is closed.
 */
TEST_F(SecureElementSessionTest, OpenLogicalChannel_Test_1)
{
    sptr<SecureElementReader> reader = new SecureElementReader(seServiceManager_, terminalTable_[1].second);

    std::string aid{};
    sptr<ISEChannel> seChannel = nullptr;
    sptr<ISESession> session = reader->OpenSession();
    EXPECT_TRUE(session != nullptr);
    EXPECT_FALSE(session->IsClosed());

    // Aid cann't be empty when the reader is UICC
    try {
        seChannel = session->OpenLogicalChannel(aid, 0x04);
        EXPECT_TRUE(!seChannel);
    } catch (const std::exception& e) {
        EXPECT_FALSE(true);
    }

    int p2 = 20;
    aid = "12345";
    try {
        seChannel = session->OpenLogicalChannel(aid, p2);
        EXPECT_FALSE(true);
    } catch (const UnsupportedOperationError& e) {
        EXPECT_TRUE(true);
    } catch (const std::exception& e) {
        EXPECT_FALSE(true);
    }

    // close session
    session->Close();
    EXPECT_TRUE(session->IsClosed());
    try {
        session->OpenLogicalChannel(aid, 0);
        EXPECT_FALSE(true);
    } catch (const IllegalStateError& e) {
        EXPECT_TRUE(true);
    } catch (const std::exception& e) {
        EXPECT_FALSE(true);
    }
}

/**
 * @brief Check about OpenLogicalChannel exception 2
 * 1. Checking the related bundle name of the calling uid is null.
 * 2. Checking the parameter aid length is less then 5 and bigger then 16.
 * 3. Checking the response data of  OpenLogicalChannel from HAL Service.
 */
TEST_F(SecureElementSessionTest, OpenLogicalChannel_Test_2)
{
    std::string aid{};
    sptr<ISEChannel> seChannel = nullptr;
    sptr<ISESession> session = reader_->OpenSession();
    EXPECT_TRUE(session != nullptr);
    EXPECT_FALSE(session->IsClosed());

    std::vector<std::string> pkgNames{};
    std::shared_ptr<osal::BundleManager> pkgMng = context_->GetBundleManager().lock();
    pkgMng->SetBundlesForUid(OHOS::IPCSkeleton::GetCallingUid(), pkgNames);
    // the related bundle(name) is emtpy with the calling uid
    try {
        seChannel = session->OpenLogicalChannel(aid, 4);
        EXPECT_FALSE(true);
    } catch (const AccessControlError& e) {
        EXPECT_TRUE(true);
    } catch (const std::exception& e) {
        EXPECT_FALSE(true);
    }

    std::string pkgName{"1234567890"};
    BaseTest::SetUpBundleInfo(pkgName, context_);

    // aid length is less then 5
    aid = "1234";
    try {
        seChannel = session->OpenLogicalChannel(aid, 8);
        EXPECT_FALSE(true);
    } catch (const IllegalArgumentError& e) {
        EXPECT_TRUE(true);
    } catch (const std::exception& e) {
        EXPECT_FALSE(true);
    }

    // aid length is bigger 16
    aid = "1234567890abcdef1";
    try {
        seChannel = session->OpenLogicalChannel(aid, 0x0C);
        EXPECT_FALSE(true);
    } catch (const IllegalArgumentError& e) {
        EXPECT_TRUE(true);
    } catch (const std::exception& e) {
        EXPECT_FALSE(true);
    }

    // response data is empty
    std::unique_ptr<SELogicalRespData> respData{};
    BaseTest::SetUpOpenLogicalChannelWithPreData(seMock_, respData);
    aid = "12345";
    try {
        seChannel = session->OpenLogicalChannel(aid, 0x00);
        EXPECT_FALSE(true);
    } catch (const ServiceSpecificError& e) {
        EXPECT_TRUE(true);
    } catch (const std::exception& e) {
        EXPECT_FALSE(true);
    }

    // response data status is CHANNEL_NOT_AVAILABLE
    respData = std::make_unique<SELogicalRespData>();
    respData->status = SEStatus::CHANNEL_NOT_AVAILABLE;
    BaseTest::SetUpOpenLogicalChannelWithPreData(seMock_, respData);
    aid = "12345";
    try {
        seChannel = session->OpenLogicalChannel(aid, 0x04);
        EXPECT_TRUE(!seChannel);
    } catch (const std::exception& e) {
        EXPECT_FALSE(true);
    }
    // response data status is UNSUPPORTED_OPERATION
    respData = std::make_unique<SELogicalRespData>();
    respData->status = SEStatus::UNSUPPORTED_OPERATION;
    BaseTest::SetUpOpenLogicalChannelWithPreData(seMock_, respData);
    aid = "12345";
    try {
        seChannel = session->OpenLogicalChannel(aid, 0x08);
        EXPECT_FALSE(true);
    } catch (const UnsupportedOperationError& e) {
        EXPECT_TRUE(true);
    } catch (const std::exception& e) {
        EXPECT_FALSE(true);
    }
    // response data status is IO_ERROR
    respData = std::make_unique<SELogicalRespData>();
    respData->status = SEStatus::IO_ERROR;
    BaseTest::SetUpOpenLogicalChannelWithPreData(seMock_, respData);
    aid = "12345";
    try {
        seChannel = session->OpenLogicalChannel(aid, 0x0c);
        EXPECT_FALSE(true);
    } catch (const ServiceSpecificError& e) {
        EXPECT_TRUE(true);
    } catch (const std::exception& e) {
        EXPECT_FALSE(true);
    }
    // response data status is NO_SUCH_SE_ERROR
    respData = std::make_unique<SELogicalRespData>();
    respData->status = SEStatus::NO_SUCH_SE_ERROR;
    BaseTest::SetUpOpenLogicalChannelWithPreData(seMock_, respData);
    aid = "12345";
    try {
        seChannel = session->OpenLogicalChannel(aid, 0x00);
        EXPECT_FALSE(true);
    } catch (const ServiceSpecificError& e) {
        EXPECT_TRUE(true);
    } catch (const std::exception& e) {
        EXPECT_FALSE(true);
    }
    // response data status isn't SUCCESS
    respData = std::make_unique<SELogicalRespData>();
    respData->status = SEStatus::FAILED;
    respData->channelNumber = 1;
    BaseTest::SetUpOpenLogicalChannelWithPreData(seMock_, respData);
    aid = "12345";
    try {
        seChannel = session->OpenLogicalChannel(aid, 0x00);
        EXPECT_TRUE(!seChannel);
    } catch (const std::exception& e) {
        EXPECT_FALSE(true);
    }
    // response data channel number is <= 0
    respData = std::make_unique<SELogicalRespData>();
    respData->status = SEStatus::SUCCESS;
    respData->channelNumber = 0;
    BaseTest::SetUpOpenLogicalChannelWithPreData(seMock_, respData);
    aid = "12345";
    try {
        seChannel = session->OpenLogicalChannel(aid, 0x00);
        EXPECT_TRUE(!seChannel);
    } catch (const std::exception& e) {
        EXPECT_FALSE(true);
    }
}

/**
 * @brief Check about OpenLogicalChannel 3
 * 1. Checking Open Logical Channel.
 * 2. Checking Repeat opening Logical Channel.
 */
TEST_F(SecureElementSessionTest, OpenLogicalChannel_Test_3)
{
    std::string aid{};
    sptr<ISEChannel> seChannel = nullptr;
    sptr<ISESession> session = reader_->OpenSession();
    EXPECT_TRUE(session != nullptr);
    EXPECT_FALSE(session->IsClosed());

    //  the related the bundle name
    std::string pkgName{"1234567890"};
    BaseTest::SetUpBundleInfo(pkgName, context_);
    // the response data of OpenBasicChannel
    std::unique_ptr<SELogicalRespData> respData = std::make_unique<SELogicalRespData>();
    respData->status = SEStatus::SUCCESS;
    respData->channelNumber = 1;
    BaseTest::SetUpOpenLogicalChannelWithPreData(seMock_, respData);
    aid = "12345";
    try {
        seChannel = session->OpenLogicalChannel(aid, 0x00);
        EXPECT_TRUE(seChannel != nullptr);
    } catch (const std::exception& e) {
        EXPECT_FALSE(true);
    }

    sptr<ISEChannel> repeatChannel{};
    respData = std::make_unique<SELogicalRespData>();
    respData->status = SEStatus::SUCCESS;
    respData->channelNumber = 2;
    BaseTest::SetUpOpenLogicalChannelWithPreData(seMock_, respData);
    aid = "12345";
    try {
        repeatChannel = session->OpenLogicalChannel(aid, 0x0C);
        EXPECT_TRUE(repeatChannel != nullptr);
    } catch (const std::exception& e) {
        EXPECT_FALSE(true);
    }
}

TEST_F(SecureElementSessionTest, CloseChannels_Test)
{
    std::string aid{};
    sptr<ISEChannel> seChannel = nullptr;
    sptr<ISESession> session = reader_->OpenSession();
    EXPECT_TRUE(session != nullptr);
    EXPECT_FALSE(session->IsClosed());

    //  the related the bundle name
    std::string pkgName{"1234567890"};
    BaseTest::SetUpBundleInfo(pkgName, context_);
    // the response data of OpenLogicalChannel
    std::unique_ptr<SELogicalRespData> respData = std::make_unique<SELogicalRespData>();
    respData->status = SEStatus::SUCCESS;
    respData->channelNumber = 1;
    BaseTest::SetUpOpenLogicalChannelWithPreData(seMock_, respData);
    aid = "12345";
    try {
        seChannel = session->OpenLogicalChannel(aid, 0x00);
        EXPECT_TRUE(seChannel != nullptr);
    } catch (const std::exception& e) {
        EXPECT_FALSE(true);
    }

    sptr<ISEChannel> repeatChannel{};
    respData = std::make_unique<SELogicalRespData>();
    respData->status = SEStatus::SUCCESS;
    respData->channelNumber = 2;
    BaseTest::SetUpOpenLogicalChannelWithPreData(seMock_, respData);
    aid = "12345";
    try {
        repeatChannel = session->OpenLogicalChannel(aid, 0x0C);
        EXPECT_TRUE(repeatChannel != nullptr);
    } catch (const std::exception& e) {
        EXPECT_FALSE(true);
    }

    // the response data of OpenBasicChannel
    BaseTest::SetUpOpenLogicalChannel(seMock_);
    std::unique_ptr<SEBasicRespData> respBasicData = std::make_unique<SEBasicRespData>();
    respBasicData->status = SEStatus::SUCCESS;
    EXPECT_CALL(*seMock_, OpenBasicChannel(_, _)).WillRepeatedly(Return(ByMove(std::move(respBasicData))));
    aid = "12345";
    try {
        seChannel = session->OpenBasicChannel(aid, 0x00);
        EXPECT_TRUE(seChannel != nullptr);
    } catch (const std::exception& e) {
        InfoLog("exception: %s", e.what());
        EXPECT_FALSE(true);
    }

    // This is need by closing the basic channel
    std::string response{(char)0x90, 0x00};
    EXPECT_CALL(*seMock_, Transmit(_)).WillRepeatedly(Return(response));
    // closing the channels
    EXPECT_CALL(*seMock_, CloseChannel(_)).Times(3).WillRepeatedly(Return(SEStatus::SUCCESS));
    session->CloseChannels();
}
}  // namespace test
}  // namespace se
}  // namespace OHOS