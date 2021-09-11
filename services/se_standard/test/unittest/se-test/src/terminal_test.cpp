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
#include "terminal.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <chrono>
#include <memory>
#include <thread>

#include "access_rule_data_common.h"
#include "base_test.h"
#include "bundle_manager.h"
#include "context.h"
#include "ipc_object_stub.h"
#include "ipc_skeleton.h"
#include "isecure_element_mock.h"
#include "loghelper.h"
#include "osal_bundle_info.h"
#include "se_channel.h"
#include "se_common_exception.h"
#include "secure_element_channel.h"
#include "secure_element_reader.h"
#include "secure_element_service.h"
#undef private

using namespace testing;
using namespace osal;
using OHOS::hardware::se::v1_0::ISecureElementCallback;
using OHOS::hardware::se::v1_0::ISecureElementMock;
using Channel = OHOS::se::SeChannel;
namespace OHOS {
namespace se {
namespace test {
const std::string ESE_READER{"eSE1"};
const std::string SIM_READER{"SIM1"};
class TerminalTest : public ::testing::Test {
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
    sptr<SecureElementReader> reader_;
    sptr<SecureElementService> seService_;
    std::shared_ptr<SeEndService> seServiceManager_;
    sptr<ISecureElementMock> seMock_;
    std::shared_ptr<Context> context_;
    TerminalTable terminalTable_;
};

TEST(TerminalTest1, Initialize_Test_1)
{
    TerminalTable terminalTable;
    std::shared_ptr<Terminal> terminal{};
    std::shared_ptr<Context> context = std::make_shared<Context>();

    sptr<ISecureElementMock> seMock{};
    // add eSE1==
    terminal = std::make_shared<Terminal>(ESE_READER, context);
    // HAL Service is unexist.
    try {
        terminal->Initialize(seMock);
        EXPECT_TRUE(false);
    } catch (const NoSuchElementError& e) {
        EXPECT_TRUE(true);
    } catch (const std::exception& e) {
        InfoLog("%s", e.what());
        EXPECT_TRUE(false);
    }
}

TEST(TerminalTest1, Initialize_Test_2)
{
    TerminalTable terminalTable;
    std::shared_ptr<Terminal> terminal{};
    std::shared_ptr<Context> context = std::make_shared<Context>();

    sptr<ISecureElementMock> seMock{};
    seMock = BaseTest::LoadSecureElement(false);
    // add eSE1==
    terminal = std::make_shared<Terminal>(ESE_READER, context);
    terminal->Initialize(seMock);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    EXPECT_TRUE(terminal->GetAccessControlEnforcer().expired());

    BaseTest::SetSeInitializeCall(true, seMock);
    std::unique_ptr<SELogicalRespData> respData2 = std::make_unique<SELogicalRespData>();
    respData2->status = SEStatus::SUCCESS;
    respData2->channelNumber = 1;
    respData2->resp = {0x09, 0x00};
    BaseTest::SetUpOpenLogicalChannelWithPreData(seMock, respData2);
    terminal->Initialize(seMock);

    BaseTest::SetSeInitializeCall(false, seMock);
    terminal->Initialize(seMock);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    EXPECT_FALSE(terminal->GetAccessControlEnforcer().expired());

    terminal->CloseChannels();
    terminal->Close();
}

TEST_F(TerminalTest, GetName_Test)
{
    EXPECT_FALSE(terminalTable_.empty());
    TerminalTable::const_iterator iter = terminalTable_.cbegin();
    for (; iter != terminalTable_.cend(); iter++) {
        EXPECT_FALSE(iter->second->GetName().empty());
    }
}

TEST_F(TerminalTest, OpenBasicChannel_Test_1)
{
    std::shared_ptr<Terminal> terminal = terminalTable_.at(0).second;

    std::string aid{};
    std::shared_ptr<Channel> seChannel{};

    std::string bundleName{"1234567890"};
    BaseTest::SetUpBundleInfo(bundleName, context_);
    int pid = OHOS::IPCSkeleton::GetCallingPid();

    // aid length is less then 5
    aid = "1234";
    try {
        seChannel = terminal->OpenBasicChannel(aid, 4, bundleName, pid);
        EXPECT_FALSE(true);
    } catch (const IllegalArgumentError& e) {
        EXPECT_TRUE(true);
    } catch (const std::exception& e) {
        InfoLog("%s", e.what());
        EXPECT_FALSE(true);
    }

    // aid length is bigger then 16
    aid = "1234567890abcdef1";
    try {
        seChannel = terminal->OpenBasicChannel(aid, 8, bundleName, pid);
        EXPECT_FALSE(true);
    } catch (const IllegalArgumentError& e) {
        EXPECT_TRUE(true);
    } catch (const std::exception& e) {
        InfoLog("%s", e.what());
        EXPECT_FALSE(true);
    }

    // respData is empty 1
    BaseTest::SetUpOpenLogicalChannel(seMock_);
    std::unique_ptr<SEBasicRespData> respData{};
    EXPECT_CALL(*seMock_, OpenBasicChannel(_, _)).WillRepeatedly(Return(ByMove(std::move(respData))));
    aid = "12345";
    try {
        seChannel = terminal->OpenBasicChannel(aid, 0x0C, bundleName, pid);
        EXPECT_FALSE(true);
    } catch (const IOError& e) {
        EXPECT_TRUE(true);
    } catch (const std::exception& e) {
        InfoLog("%s", e.what());
        EXPECT_FALSE(true);
    }

    // respData is empty 2
    respData = std::unique_ptr<SEBasicRespData>();
    EXPECT_CALL(*seMock_, OpenBasicChannel(_, _)).WillRepeatedly(Return(ByMove(std::move(respData))));
    try {
        seChannel = terminal->OpenBasicChannel(aid, 0x0C, std::string(""), pid);
        EXPECT_FALSE(true);
    } catch (const IOError& e) {
        EXPECT_TRUE(true);
    } catch (const std::exception& e) {
        InfoLog("%s", e.what());
        EXPECT_FALSE(true);
    }
    // respData is CHANNEL_NOT_AVAILABLE
    BaseTest::SetUpOpenLogicalChannel(seMock_);
    respData = std::make_unique<SEBasicRespData>();
    respData->status = SEStatus::CHANNEL_NOT_AVAILABLE;
    EXPECT_CALL(*seMock_, OpenBasicChannel(_, _)).WillRepeatedly(Return(ByMove(std::move(respData))));
    aid = "12345";
    try {
        seChannel = terminal->OpenBasicChannel(aid, 0x0C, bundleName, pid);
        EXPECT_TRUE(!seChannel);
    } catch (const std::exception& e) {
        InfoLog("%s", e.what());
        EXPECT_FALSE(true);
    }

    // respData is UNSUPPORTED_OPERATION
    BaseTest::SetUpOpenLogicalChannel(seMock_);
    respData = std::make_unique<SEBasicRespData>();
    respData->status = SEStatus::UNSUPPORTED_OPERATION;
    EXPECT_CALL(*seMock_, OpenBasicChannel(_, _)).WillRepeatedly(Return(ByMove(std::move(respData))));
    aid = "12345";
    try {
        seChannel = terminal->OpenBasicChannel(aid, 0x0C, bundleName, pid);
        EXPECT_FALSE(true);
    } catch (const UnsupportedOperationError& e) {
        EXPECT_TRUE(true);
    } catch (const std::exception& e) {
        InfoLog("%s", e.what());
        EXPECT_FALSE(true);
    }

    // respData is IO_ERROR
    BaseTest::SetUpOpenLogicalChannel(seMock_);
    respData = std::make_unique<SEBasicRespData>();
    respData->status = SEStatus::IO_ERROR;
    EXPECT_CALL(*seMock_, OpenBasicChannel(_, _)).WillRepeatedly(Return(ByMove(std::move(respData))));
    aid = "12345";
    try {
        seChannel = terminal->OpenBasicChannel(aid, 0x0C, bundleName, pid);
        EXPECT_FALSE(true);
    } catch (const IOError& e) {
        EXPECT_TRUE(true);
    } catch (const std::exception& e) {
        InfoLog("%s", e.what());
        EXPECT_FALSE(true);
    }
    // respData is NO_SUCH_SE_ERROR
    BaseTest::SetUpOpenLogicalChannel(seMock_);
    respData = std::make_unique<SEBasicRespData>();
    respData->status = SEStatus::NO_SUCH_SE_ERROR;
    EXPECT_CALL(*seMock_, OpenBasicChannel(_, _)).WillRepeatedly(Return(ByMove(std::move(respData))));
    aid = "12345";
    try {
        seChannel = terminal->OpenBasicChannel(aid, 0x0C, bundleName, pid);
        EXPECT_FALSE(true);
    } catch (const NoSuchElementError& e) {
        EXPECT_TRUE(true);
    } catch (const std::exception& e) {
        InfoLog("%s", e.what());
        EXPECT_FALSE(true);
    }

    BaseTest::SetSeInitializeCall(false, seMock_);
    terminal->Initialize(seMock_);
    aid = "12345";
    try {
        seChannel = terminal->OpenBasicChannel(aid, 0x0C, bundleName, pid);
        EXPECT_FALSE(true);
    } catch (const IOError& e) {
        EXPECT_TRUE(true);
    } catch (const std::exception& e) {
        InfoLog("%s", e.what());
        EXPECT_FALSE(true);
    }
}

/**
 * @brief Check about OpenBasicChannel 2
 * 1. Checking Open Basic Channel.
 * 2. Checking Repeat opening Basic Channel.
 */
TEST_F(TerminalTest, OpenBasicChannel_Test_2)
{
    std::string aid{};
    std::shared_ptr<Channel> seChannel{};
    int pid = OHOS::IPCSkeleton::GetCallingPid();
    std::shared_ptr<Terminal> terminal = terminalTable_.at(1).second;

    //  the related the bundle name
    std::string bundleName{"1234567890"};
    BaseTest::SetUpBundleInfo(bundleName, context_);
    std::unique_ptr<SELogicalRespData> respData2 = std::make_unique<SELogicalRespData>();
    respData2->status = SEStatus::SUCCESS;
    respData2->channelNumber = 1;
    respData2->resp = {0x09, 0x00};
    BaseTest::SetUpOpenLogicalChannelWithPreData(seMock_, respData2);
    // the response data of OpenBasicChannel
    std::unique_ptr<SEBasicRespData> respData = std::make_unique<SEBasicRespData>();
    respData->status = SEStatus::SUCCESS;
    EXPECT_CALL(*seMock_, OpenBasicChannel(_, _)).WillRepeatedly(Return(ByMove(std::move(respData))));
    aid = "12345";
    try {
        seChannel = terminal->OpenBasicChannel(aid, 0x00, bundleName, pid);
        EXPECT_TRUE(seChannel != nullptr);
    } catch (const std::exception& e) {
        InfoLog("exception: %s", e.what());
        EXPECT_FALSE(true);
    }

    // repeat opening basic channel
    respData2 = std::make_unique<SELogicalRespData>();
    respData2->status = SEStatus::SUCCESS;
    respData2->channelNumber = 1;
    respData2->resp = {0x09, 0x00};
    BaseTest::SetUpOpenLogicalChannelWithPreData(seMock_, respData2);
    respData = std::make_unique<SEBasicRespData>();
    respData->status = SEStatus::SUCCESS;
    EXPECT_CALL(*seMock_, OpenBasicChannel(_, _)).WillRepeatedly(Return(ByMove(std::move(respData))));
    std::shared_ptr<Channel> repeatChannel;
    aid = "";
    try {
        repeatChannel = terminal->OpenBasicChannel(aid, 0x04, bundleName, pid);
        EXPECT_TRUE(!repeatChannel);
    } catch (const std::exception& e) {
        InfoLog("exception: %s", e.what());
        EXPECT_FALSE(true);
    }

    std::string response{(char)0x90, 0x00};
    EXPECT_CALL(*seMock_, Transmit(_)).WillRepeatedly(Return(response));
    EXPECT_CALL(*seMock_, CloseChannel(_)).WillRepeatedly(Return(SEStatus::SUCCESS));
    seChannel->Close();
    EXPECT_TRUE(seChannel->IsClosed());

    respData2 = std::make_unique<SELogicalRespData>();
    respData2->status = SEStatus::SUCCESS;
    respData2->channelNumber = 1;
    respData2->resp = {0x09, 0x00};
    BaseTest::SetUpOpenLogicalChannelWithPreData(seMock_, respData2);
    respData = std::make_unique<SEBasicRespData>();
    respData->status = SEStatus::SUCCESS;
    EXPECT_CALL(*seMock_, OpenBasicChannel(_, _)).WillRepeatedly(Return(ByMove(std::move(respData))));
    aid = "";
    try {
        repeatChannel = terminal->OpenBasicChannel(aid, 0x04, bundleName, pid);
        EXPECT_TRUE(!repeatChannel);
    } catch (const std::exception& e) {
        InfoLog("exception: %s", e.what());
        EXPECT_FALSE(true);
    }

    respData2 = std::make_unique<SELogicalRespData>();
    respData2->status = SEStatus::SUCCESS;
    respData2->channelNumber = 1;
    respData2->resp = {0x09, 0x00};
    BaseTest::SetUpOpenLogicalChannelWithPreData(seMock_, respData2);
    respData = std::make_unique<SEBasicRespData>();
    respData->status = SEStatus::SUCCESS;
    EXPECT_CALL(*seMock_, OpenBasicChannel(_, _)).WillRepeatedly(Return(ByMove(std::move(respData))));
    aid = "12345";
    try {
        repeatChannel = terminal->OpenBasicChannel(aid, 0x08, bundleName, pid);
        EXPECT_TRUE(repeatChannel != nullptr);
    } catch (const std::exception& e) {
        InfoLog("exception: %s", e.what());
        EXPECT_FALSE(true);
    }
}

TEST_F(TerminalTest, OpenLogicalChannel_Test_1)
{
    std::string aid{};
    std::shared_ptr<Channel> seChannel{};
    int pid = OHOS::IPCSkeleton::GetCallingPid();
    std::shared_ptr<Terminal> terminal = terminalTable_.at(0).second;
    std::unique_ptr<SELogicalRespData> respData{};

    std::string bundleName{"1234567890"};
    std::vector<std::string> bundleNames{};
    std::shared_ptr<osal::BundleManager> bundleMng = context_->GetBundleManager().lock();
    //  the related the bundle name
    bundleNames.push_back(bundleName);
    bundleMng->SetBundlesForUid(OHOS::IPCSkeleton::GetCallingUid(), bundleNames);

    respData = std::make_unique<SELogicalRespData>();
    respData->status = SEStatus::SUCCESS;
    respData->channelNumber = 2;
    EXPECT_CALL(*seMock_, OpenLogicalChannel(_, _)).WillRepeatedly(Return(ByMove(std::move(respData))));
    // aid length is less then 5
    aid = "1234";
    try {
        seChannel = terminal->OpenLogicalChannel(aid, 0x00, bundleName, pid);
        EXPECT_FALSE(true);
    } catch (const IllegalArgumentError& e) {
        EXPECT_TRUE(true);
    } catch (const std::exception& e) {
        InfoLog("%s", e.what());
        EXPECT_FALSE(true);
    }

    respData = std::make_unique<SELogicalRespData>();
    respData->status = SEStatus::SUCCESS;
    respData->channelNumber = 3;
    EXPECT_CALL(*seMock_, OpenLogicalChannel(_, _)).WillRepeatedly(Return(ByMove(std::move(respData))));
    // aid length is bigger 16
    aid = "1234567890abcdef1";
    try {
        seChannel = terminal->OpenLogicalChannel(aid, 0x0C, bundleName, pid);
        EXPECT_FALSE(true);
    } catch (const IllegalArgumentError& e) {
        EXPECT_TRUE(true);
    } catch (const std::exception& e) {
        InfoLog("%s", e.what());
        EXPECT_FALSE(true);
    }

    // response data is empty
    BaseTest::SetUpBundleInfo(bundleName, context_);
    respData = std::unique_ptr<SELogicalRespData>();
    BaseTest::SetUpOpenLogicalChannelWithPreData(seMock_, respData);
    aid = "12345";
    try {
        seChannel = terminal->OpenLogicalChannel(aid, 0x08, bundleName, pid);
        EXPECT_FALSE(true);
    } catch (const IOError& e) {
        EXPECT_TRUE(true);
    } catch (const std::exception& e) {
        InfoLog("%s", e.what());
        EXPECT_FALSE(true);
    }

    // response data status is CHANNEL_NOT_AVAILABLE
    respData = std::make_unique<SELogicalRespData>();
    respData->status = SEStatus::CHANNEL_NOT_AVAILABLE;
    BaseTest::SetUpOpenLogicalChannelWithPreData(seMock_, respData);
    aid = "12345";
    try {
        seChannel = terminal->OpenLogicalChannel(aid, 0x04, bundleName, pid);
        EXPECT_TRUE(!seChannel);
    } catch (const std::exception& e) {
        InfoLog("%s", e.what());
        EXPECT_FALSE(true);
    }
    // response data status is UNSUPPORTED_OPERATION
    respData = std::make_unique<SELogicalRespData>();
    respData->status = SEStatus::UNSUPPORTED_OPERATION;
    BaseTest::SetUpOpenLogicalChannelWithPreData(seMock_, respData);
    aid = "12345";
    try {
        seChannel = terminal->OpenLogicalChannel(aid, 0x00, bundleName, pid);
        EXPECT_FALSE(true);
    } catch (const UnsupportedOperationError& e) {
        EXPECT_TRUE(true);
    } catch (const std::exception& e) {
        InfoLog("%s", e.what());
        EXPECT_FALSE(true);
    }
    // response data status is IO_ERROR
    respData = std::make_unique<SELogicalRespData>();
    respData->status = SEStatus::IO_ERROR;
    BaseTest::SetUpOpenLogicalChannelWithPreData(seMock_, respData);
    aid = "12345";
    try {
        seChannel = terminal->OpenLogicalChannel(aid, 0x0C, bundleName, pid);
        EXPECT_FALSE(true);
    } catch (const IOError& e) {
        EXPECT_TRUE(true);
    } catch (const std::exception& e) {
        InfoLog("%s", e.what());
        EXPECT_FALSE(true);
    }
    // response data status is NO_SUCH_SE_ERROR
    respData = std::make_unique<SELogicalRespData>();
    respData->status = SEStatus::NO_SUCH_SE_ERROR;
    BaseTest::SetUpOpenLogicalChannelWithPreData(seMock_, respData);
    aid = "12345";
    try {
        seChannel = terminal->OpenLogicalChannel(aid, 0x08, bundleName, pid);
        EXPECT_FALSE(true);
    } catch (const NoSuchElementError& e) {
        EXPECT_TRUE(true);
    } catch (const std::exception& e) {
        InfoLog("%s", e.what());
        EXPECT_FALSE(true);
    }
    // response data status isn't SUCCESS
    respData = std::make_unique<SELogicalRespData>();
    respData->status = SEStatus::FAILED;
    respData->channelNumber = 1;
    BaseTest::SetUpOpenLogicalChannelWithPreData(seMock_, respData);
    aid = "12345";
    try {
        seChannel = terminal->OpenLogicalChannel(aid, 0x04, bundleName, pid);
        EXPECT_TRUE(!seChannel);
    } catch (const std::exception& e) {
        InfoLog("%s", e.what());
        EXPECT_FALSE(true);
    }
    // response data channel number is <= 0
    respData = std::make_unique<SELogicalRespData>();
    respData->status = SEStatus::SUCCESS;
    respData->channelNumber = 0;
    BaseTest::SetUpOpenLogicalChannelWithPreData(seMock_, respData);
    aid = "12345";
    try {
        seChannel = terminal->OpenLogicalChannel(aid, 0x00, bundleName, pid);
        EXPECT_TRUE(!seChannel);
    } catch (const std::exception& e) {
        InfoLog("%s", e.what());
        EXPECT_FALSE(true);
    }
}

/**
 * @brief Check about OpenLogicalChannel 2
 * 1. Checking Open Logical Channel.
 * 2. Checking Repeat opening Logical Channel.
 */
TEST_F(TerminalTest, OpenLogicalChannel_Test_2)
{
    std::string aid{};
    std::shared_ptr<Channel> seChannel{};
    int pid = OHOS::IPCSkeleton::GetCallingPid();
    std::shared_ptr<Terminal> terminal = terminalTable_.at(0).second;

    //  the related the bundle name
    std::string bundleName{"1234567890"};
    BaseTest::SetUpBundleInfo(bundleName, context_);
    // the response data of OpenBasicChannel
    std::unique_ptr<SELogicalRespData> respData = std::make_unique<SELogicalRespData>();
    respData->status = SEStatus::SUCCESS;
    respData->channelNumber = 1;
    BaseTest::SetUpOpenLogicalChannelWithPreData(seMock_, respData);
    aid = "12345";
    try {
        seChannel = terminal->OpenLogicalChannel(aid, 0x00, bundleName, pid);
        EXPECT_TRUE(seChannel != nullptr);
    } catch (const std::exception& e) {
        EXPECT_FALSE(true);
    }

    std::shared_ptr<Channel> repeatChannel{};
    respData = std::make_unique<SELogicalRespData>();
    respData->status = SEStatus::SUCCESS;
    respData->channelNumber = 2;
    BaseTest::SetUpOpenLogicalChannelWithPreData(seMock_, respData);
    aid = "12345";
    try {
        repeatChannel = terminal->OpenLogicalChannel(aid, 0x00, bundleName, pid);
        EXPECT_TRUE(repeatChannel != nullptr);
    } catch (const std::exception& e) {
        EXPECT_FALSE(true);
    }
}

TEST(TerminalTest1, Transmit_Test)
{
    std::shared_ptr<Terminal> terminal{};
    std::shared_ptr<Context> context = std::make_shared<Context>();

    sptr<ISecureElementMock> seMock{};
    seMock = BaseTest::LoadSecureElement();
    // add eSE1==
    terminal = std::make_shared<Terminal>(ESE_READER, context);
    terminal->Initialize(seMock);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // Terminal isn't Connected
    std::string apduCmd{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00};
    try {
        terminal->Transmit(apduCmd);
        EXPECT_TRUE(false);
    } catch (const IOError& e) {
        EXPECT_TRUE(true);
    } catch (const std::exception&) {
        EXPECT_TRUE(false);
    }

    std::unique_ptr<SELogicalRespData> respData = std::make_unique<SELogicalRespData>();
    respData->status = SEStatus::SUCCESS;
    respData->channelNumber = 1;
    EXPECT_CALL(*seMock, OpenLogicalChannel(_, _)).WillOnce(Return(ByMove(std::move(respData))));

    BaseTest::SetSeInitializeCall(true, seMock);
    // add eSE1==
    terminal->Initialize(seMock);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    std::string response{};
    EXPECT_CALL(*seMock, Transmit(_)).WillRepeatedly(Return(response));
    apduCmd = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00};
    try {
        terminal->Transmit(apduCmd);
        EXPECT_TRUE(false);
    } catch (const IOError& e) {
        EXPECT_TRUE(true);
    } catch (const std::exception&) {
        EXPECT_TRUE(false);
    }

    response = {0x00};
    EXPECT_CALL(*seMock, Transmit(_)).WillRepeatedly(Return(response));
    apduCmd = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00};
    try {
        terminal->Transmit(apduCmd);
        EXPECT_TRUE(false);
    } catch (const UnsupportedOperationError& e) {
        EXPECT_TRUE(true);
    } catch (const std::exception& e) {
        InfoLog("exception: %s", e.what());
        EXPECT_TRUE(false);
    }

    std::string resData;
    // repeat command
    response = {0x6C, 0x00};
    std::string response2 = {(char)0x90, 0x00};
    EXPECT_CALL(*seMock, Transmit(_)).WillOnce(Return(response)).WillOnce(Return(response)).WillOnce(Return(response2));
    apduCmd = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00};
    try {
        resData = terminal->Transmit(apduCmd);
        EXPECT_TRUE(0 == resData.compare(response2));
    } catch (const std::exception& e) {
        InfoLog("%s", e.what());
        EXPECT_TRUE(false);
    }

    std::string expire{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, (char)0x90, 0x00};
    // response long data
    response = {0x00, 0x00, 0x61, 0x00};
    response2 = {0x00, 0x00, 0x00, (char)0x90, 0x00};
    EXPECT_CALL(*seMock, Transmit(_)).WillOnce(Return(response)).WillOnce(Return(response)).WillOnce(Return(response2));
    apduCmd = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00};
    try {
        resData = terminal->Transmit(apduCmd);
        EXPECT_TRUE(0 == resData.compare(expire));
    } catch (const std::exception& e) {
        InfoLog("%s", e.what());
        EXPECT_TRUE(false);
    }
}

TEST(TerminalTest1, IsSecureElementPresent_Test)
{
    std::shared_ptr<Terminal> terminal{};
    std::shared_ptr<Context> context = std::make_shared<Context>();

    sptr<ISecureElementMock> seMock = BaseTest::LoadSecureElement(true);
    std::unique_ptr<SELogicalRespData> respData = std::make_unique<SELogicalRespData>();
    respData->status = SEStatus::SUCCESS;
    respData->channelNumber = 1;
    EXPECT_CALL(*seMock, OpenLogicalChannel(_, _)).WillOnce(Return(ByMove(std::move(respData))));
    terminal = std::make_shared<Terminal>(SIM_READER, context);
    terminal->Initialize(seMock);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    EXPECT_CALL(*seMock, IsCardPresent()).Times(1).WillRepeatedly(Return(true));
    EXPECT_TRUE(terminal->IsSecureElementPresent());
    EXPECT_CALL(*seMock, IsCardPresent()).Times(1).WillRepeatedly(Return(false));
    EXPECT_FALSE(terminal->IsSecureElementPresent());
}

TEST(TerminalTest1, GetAtr_Test)
{
    std::shared_ptr<Terminal> terminal{};
    std::shared_ptr<Context> context = std::make_shared<Context>();

    sptr<ISecureElementMock> seMock = BaseTest::LoadSecureElement();
    // add eSE1==
    terminal = std::make_shared<Terminal>(ESE_READER, context);
    terminal->Initialize(seMock);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    std::string atr{};
    // Terminal isn't Connected
    try {
        atr = terminal->GetAtr();
        EXPECT_TRUE(atr.empty());
    } catch (const std::exception&) {
        EXPECT_TRUE(false);
    }

    std::unique_ptr<SELogicalRespData> respData = std::make_unique<SELogicalRespData>();
    respData->status = SEStatus::SUCCESS;
    respData->channelNumber = 1;
    EXPECT_CALL(*seMock, OpenLogicalChannel(_, _)).WillOnce(Return(ByMove(std::move(respData))));
    BaseTest::SetSeInitializeCall(true, seMock);
    // add eSE1==
    terminal->Initialize(seMock);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    std::string response{0x00, 0x00, (char)0x90, 0x00};
    EXPECT_CALL(*seMock, GetAtr()).WillRepeatedly(Return(response));
    try {
        atr = terminal->GetAtr();
        EXPECT_TRUE(0 == atr.compare(response));
    } catch (const std::exception&) {
        EXPECT_TRUE(false);
    }
}

TEST(TerminalTest1, IsAidSelectable_Test)
{
    std::shared_ptr<Terminal> terminal{};
    std::shared_ptr<Context> context = std::make_shared<Context>();

    sptr<ISecureElementMock> seMock = BaseTest::LoadSecureElement();
    // add eSE1==
    terminal = std::make_shared<Terminal>(ESE_READER, context);
    terminal->Initialize(seMock);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    std::string aid{};
    // aid is empty
    try {
        terminal->IsAidSelectable(aid);
        EXPECT_TRUE(false);
    } catch (const NullPointerError& e) {
        EXPECT_TRUE(true);
    } catch (const std::exception&) {
        EXPECT_TRUE(false);
    }

    // Terminal isn't Connected
    aid = {0x00, 0x01, 0x02, 0x03, 0x04};
    try {
        EXPECT_FALSE(terminal->IsAidSelectable(aid));
    } catch (const std::exception&) {
        EXPECT_TRUE(false);
    }

    std::unique_ptr<SELogicalRespData> respData = std::make_unique<SELogicalRespData>();
    respData->status = SEStatus::SUCCESS;
    respData->channelNumber = 1;
    respData->resp = {0x09, 0x00};

    std::unique_ptr<SELogicalRespData> respData2 = std::make_unique<SELogicalRespData>();
    respData2->status = SEStatus::SUCCESS;
    respData2->channelNumber = 2;
    respData2->resp = {0x09, 0x00};
    EXPECT_CALL(*seMock, OpenLogicalChannel(_, _))
        .WillOnce(Return(ByMove(std::move(respData))))
        .WillOnce(Return(ByMove(std::move(respData2))));
    BaseTest::SetSeInitializeCall(true, seMock);
    // add eSE1==
    terminal->Initialize(seMock);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    try {
        EXPECT_TRUE(terminal->IsAidSelectable(aid));
    } catch (const std::exception&) {
        EXPECT_TRUE(false);
    }

    respData = std::make_unique<SELogicalRespData>();
    respData->status = SEStatus::FAILED;
    EXPECT_CALL(*seMock, OpenLogicalChannel(_, _)).WillRepeatedly(Return(ByMove(std::move(respData))));
    try {
        EXPECT_FALSE(terminal->IsAidSelectable(aid));
    } catch (const std::exception&) {
        EXPECT_TRUE(false);
    }
}

TEST(TerminalTest1, SelectDefaultApplication_Test)
{
    std::shared_ptr<Terminal> terminal{};
    std::shared_ptr<Context> context = std::make_shared<Context>();

    sptr<ISecureElementMock> seMock = BaseTest::LoadSecureElement();
    // add eSE1==
    terminal = std::make_shared<Terminal>(ESE_READER, context);
    terminal->Initialize(seMock);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // Terminal isn't Connected, Only Print "ignore exception: "
    try {
        terminal->SelectDefaultApplication();
        EXPECT_TRUE(true);
    } catch (const std::exception& e) {
        InfoLog("exception: %s", e.what());
        EXPECT_TRUE(false);
    }

    std::unique_ptr<SELogicalRespData> respData = std::make_unique<SELogicalRespData>();
    respData->status = SEStatus::SUCCESS;
    respData->channelNumber = 1;
    EXPECT_CALL(*seMock, OpenLogicalChannel(_, _)).WillOnce(Return(ByMove(std::move(respData))));
    BaseTest::SetSeInitializeCall(true, seMock);
    // add eSE1==
    terminal->Initialize(seMock);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    std::string response{};
    EXPECT_CALL(*seMock, Transmit(_)).WillRepeatedly(Return(response));
    try {
        terminal->SelectDefaultApplication();
        EXPECT_TRUE(true);
    } catch (const std::exception& e) {
        InfoLog("exception: %s", e.what());
        EXPECT_TRUE(false);
    }
    // response data length is less then 2 bytes(0x00)
    response = {0x00};
    EXPECT_CALL(*seMock, Transmit(_)).WillRepeatedly(Return(response));
    try {
        terminal->SelectDefaultApplication();
        EXPECT_TRUE(true);
    } catch (const std::exception& e) {
        InfoLog("exception: %s", e.what());
        EXPECT_TRUE(false);
    }

    // response data status is error (0x3001)
    response = {0x30, 0x01};
    EXPECT_CALL(*seMock, Transmit(_)).WillRepeatedly(Return(response));
    try {
        terminal->SelectDefaultApplication();
        EXPECT_TRUE(true);
    } catch (const std::exception& e) {
        InfoLog("exception: %s", e.what());
        EXPECT_TRUE(false);
    }

    // // response data status is error (0x9000)
    response = {(char)0x90, 0x00};
    EXPECT_CALL(*seMock, Transmit(_)).WillRepeatedly(Return(response));
    try {
        terminal->SelectDefaultApplication();
        EXPECT_TRUE(true);
    } catch (const std::exception& e) {
        InfoLog("exception: %s", e.what());
        EXPECT_TRUE(false);
    }
}

TEST(TerminalTest1, CheckCarrierPrivilegeRules_Test)
{
    std::shared_ptr<Terminal> terminal{};
    std::shared_ptr<Context> context = std::make_shared<Context>();

    sptr<ISecureElementMock> seMock = BaseTest::LoadSecureElement();
    // add eSE1==
    terminal = std::make_shared<Terminal>(ESE_READER, context);
    terminal->Initialize(seMock);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    std::shared_ptr<BundleInfo> bundleInfo{};
    // Terminal isn't Connected, Only Print "ignore exception: "
    try {
        EXPECT_TRUE(terminal->GetAccessControlEnforcer().expired());
        EXPECT_FALSE(terminal->CheckCarrierPrivilegeRules(bundleInfo));
        EXPECT_FALSE(terminal->GetAccessControlEnforcer().expired());
    } catch (const std::exception& e) {
        InfoLog("exception: %s", e.what());
        EXPECT_TRUE(false);
    }

    std::unique_ptr<SELogicalRespData> respData = std::make_unique<SELogicalRespData>();
    respData->status = SEStatus::SUCCESS;
    respData->channelNumber = 16;
    respData->resp = {0x09, 0x00};
    EXPECT_CALL(*seMock, OpenLogicalChannel(_, _)).WillRepeatedly(Return(ByMove(std::move(respData))));
    BaseTest::SetSeInitializeCall(true, seMock);
    // add eSE1==
    terminal->Initialize(seMock);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    respData = std::make_unique<SELogicalRespData>();
    respData->status = SEStatus::SUCCESS;
    respData->channelNumber = 17;
    respData->resp = {0x09, 0x00};
    EXPECT_CALL(*seMock, OpenLogicalChannel(_, _)).WillRepeatedly(Return(ByMove(std::move(respData))));
    try {
        EXPECT_FALSE(terminal->GetAccessControlEnforcer().expired());
        EXPECT_FALSE(terminal->CheckCarrierPrivilegeRules(bundleInfo));
    } catch (const std::exception& e) {
        InfoLog("exception: %s", e.what());
        EXPECT_TRUE(false);
    }

    respData = std::make_unique<SELogicalRespData>();
    respData->status = SEStatus::SUCCESS;
    respData->channelNumber = 1;
    respData->resp = {0x09, 0x00};
    EXPECT_CALL(*seMock, OpenLogicalChannel(_, _)).WillRepeatedly(Return(ByMove(std::move(respData))));

    std::shared_ptr<BundleManager> pm = context->GetBundleManager().lock();
    pm->AddPermission("0987654321", "1234567890");
    bundleInfo = pm->GetBundleInfo("1234567890", 0);
    bundleInfo->signatures_.push_back("1234567890abcdef");
    // response data status is error (0x9000)
    EXPECT_CALL(*seMock, Transmit(_))
        .Times(AnyNumber())
        .WillOnce(Return(RESPONSE_REFERESH_TAG_D0))
        .WillOnce(Return(RESPONSE_ALL));
    try {
        EXPECT_FALSE(terminal->CheckCarrierPrivilegeRules(bundleInfo));
    } catch (const std::exception& e) {
        InfoLog("exception: %s", e.what());
        EXPECT_TRUE(false);
    }
}

TEST(TerminalTest1, IsNfcEventAllowed_Test)
{
    std::shared_ptr<Terminal> terminal{};
    std::shared_ptr<Context> context = std::make_shared<Context>();

    sptr<ISecureElementMock> seMock = BaseTest::LoadSecureElement();
    // add eSE1==
    terminal = std::make_shared<Terminal>(ESE_READER, context);
    terminal->Initialize(seMock);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    std::shared_ptr<BundleManager> pm = context->GetBundleManager().lock();
    pm->AddPermission("0987654321", "1234567890");

    std::string aid{};
    std::vector<std::string> bundleNames{};
    std::shared_ptr<BundleInfo> bundleInfo{};
    std::vector<bool> nfcEventAllowed;
    // Terminal isn't Connected, Only Print "ignore exception: "
    try {
        EXPECT_TRUE(terminal->GetAccessControlEnforcer().expired());
        nfcEventAllowed = terminal->IsNfcEventAllowed(pm, aid, bundleNames);
        EXPECT_TRUE(nfcEventAllowed.empty());
        EXPECT_FALSE(terminal->GetAccessControlEnforcer().expired());
    } catch (const std::exception& e) {
        InfoLog("exception: %s", e.what());
        EXPECT_TRUE(false);
    }
    bundleInfo = pm->GetBundleInfo("1234567890", 0);
    bundleInfo->signatures_.push_back("1234567890abcdef");
    bundleNames.push_back("1234567890");

    std::unique_ptr<SELogicalRespData> respData = std::make_unique<SELogicalRespData>();
    respData->status = SEStatus::SUCCESS;
    respData->channelNumber = 1;
    respData->resp = {0x09, 0x00};
    EXPECT_CALL(*seMock, OpenLogicalChannel(_, _)).WillRepeatedly(Return(ByMove(std::move(respData))));
    BaseTest::SetSeInitializeCall(true, seMock);
    // add eSE1==
    terminal->Initialize(seMock);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    std::string response{};
    // // response data status is error (0x9000)
    response = {(char)0x90, 0x00};
    EXPECT_CALL(*seMock, Transmit(_)).WillRepeatedly(Return(response));
    try {
        EXPECT_FALSE(terminal->GetAccessControlEnforcer().expired());
        nfcEventAllowed = terminal->IsNfcEventAllowed(pm, aid, bundleNames);
        EXPECT_TRUE(!nfcEventAllowed.empty());
    } catch (const std::exception& e) {
        InfoLog("exception: %s", e.what());
        EXPECT_TRUE(false);
    }
}

TEST(TerminalTest1, CloseChannel_Test)
{
    std::shared_ptr<Terminal> terminal{};
    std::shared_ptr<Context> context = std::make_shared<Context>();

    sptr<ISecureElementMock> seMock = BaseTest::LoadSecureElement();
    EXPECT_CALL(*seMock, IsCardPresent()).WillRepeatedly(Return(true));
    // add eSE1==
    terminal = std::make_shared<Terminal>(ESE_READER, context);
    terminal->Initialize(seMock);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    std::shared_ptr<SeEndService> seServiceManager = std::make_shared<SeEndService>();
    seServiceManager->Init(context);
    sptr<SecureElementReader> reader = new SecureElementReader(seServiceManager, terminal);

    // Channel is empty.
    std::shared_ptr<SeChannel> channel{};
    try {
        terminal->CloseChannel(channel);
    } catch (const std::exception& e) {
        InfoLog("exception: %s", e.what());
        EXPECT_TRUE(false);
    }

    std::string selectResponse{(char)0x90, 0x00};
    std::string aid{};
    channel = std::shared_ptr<SeChannel>(new SeChannel(terminal, 0, selectResponse, aid));
    try {
        terminal->CloseChannel(channel);
    } catch (const std::exception& e) {
        InfoLog("exception: %s", e.what());
        EXPECT_TRUE(false);
    }

    std::unique_ptr<SELogicalRespData> respData = std::make_unique<SELogicalRespData>();
    respData->status = SEStatus::SUCCESS;
    respData->channelNumber = 1;
    respData->resp = {0x09, 0x00};
    EXPECT_CALL(*seMock, OpenLogicalChannel(_, _)).WillRepeatedly(Return(ByMove(std::move(respData))));
    BaseTest::SetSeInitializeCall(true, seMock);
    terminal->Initialize(seMock);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    EXPECT_CALL(*seMock, CloseChannel(_)).WillRepeatedly(Return(SEStatus::FAILED));
    channel = std::shared_ptr<SeChannel>(new SeChannel(terminal, 0, selectResponse, aid));
    try {
        terminal->CloseChannel(channel);
    } catch (const std::exception& e) {
        InfoLog("exception: %s", e.what());
        EXPECT_TRUE(false);
    }

    EXPECT_CALL(*seMock, CloseChannel(_)).WillRepeatedly(Return(SEStatus::SUCCESS));
    try {
        terminal->CloseChannel(channel);
    } catch (const std::exception& e) {
        InfoLog("exception: %s", e.what());
        EXPECT_TRUE(false);
    }

    respData = std::make_unique<SELogicalRespData>();
    respData->status = SEStatus::SUCCESS;
    respData->channelNumber = 1;
    respData->resp = {0x09, 0x00};
    EXPECT_CALL(*seMock, OpenLogicalChannel(_, _)).WillRepeatedly(Return(ByMove(std::move(respData))));
    std::string bundleName{};
    int pid = OHOS::IPCSkeleton::GetCallingPid();
    try {
        channel = terminal->OpenLogicalChannel(aid, 0x00, bundleName, pid);
        terminal->CloseChannel(channel);
    } catch (const std::exception& e) {
        InfoLog("exception: %s", e.what());
        EXPECT_TRUE(false);
    }
}

TEST(TerminalTest1, CloseChannels_Test)
{
    std::shared_ptr<Terminal> terminal{};
    std::shared_ptr<Context> context = std::make_shared<Context>();

    sptr<ISecureElementMock> seMock = BaseTest::LoadSecureElement();
    EXPECT_CALL(*seMock, IsCardPresent()).WillRepeatedly(Return(true));
    // add eSE1==
    terminal = std::make_shared<Terminal>(ESE_READER, context);
    terminal->Initialize(seMock);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    EXPECT_TRUE(!terminal->GetContext().expired());

    std::shared_ptr<SeEndService> seServiceManager = std::make_shared<SeEndService>();
    seServiceManager->Init(context);
    sptr<SecureElementReader> reader = new SecureElementReader(seServiceManager, terminal);

    try {
        terminal->CloseChannels();
    } catch (const std::exception& e) {
        InfoLog("exception: %s", e.what());
        EXPECT_TRUE(false);
    }

    std::shared_ptr<SeChannel> channel{};
    std::string selectResponse{(char)0x90, 0x00};
    std::string aid{};
    try {
        terminal->CloseChannel(channel);
    } catch (const std::exception& e) {
        InfoLog("exception: %s", e.what());
        EXPECT_TRUE(false);
    }

    std::unique_ptr<SELogicalRespData> respData = std::make_unique<SELogicalRespData>();
    respData->status = SEStatus::SUCCESS;
    respData->channelNumber = 1;
    respData->resp = {0x09, 0x00};
    EXPECT_CALL(*seMock, OpenLogicalChannel(_, _)).WillRepeatedly(Return(ByMove(std::move(respData))));
    BaseTest::SetSeInitializeCall(true, seMock);
    terminal->Initialize(seMock);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    EXPECT_CALL(*seMock, CloseChannel(_)).WillRepeatedly(Return(SEStatus::FAILED));

    respData = std::make_unique<SELogicalRespData>();
    respData->status = SEStatus::SUCCESS;
    respData->channelNumber = 2;
    respData->resp = {0x09, 0x00};
    EXPECT_CALL(*seMock, OpenLogicalChannel(_, _)).WillRepeatedly(Return(ByMove(std::move(respData))));
    std::string bundleName{};
    int pid = OHOS::IPCSkeleton::GetCallingPid();
    try {
        channel = terminal->OpenLogicalChannel(aid, 0x00, bundleName, pid);
        respData = std::make_unique<SELogicalRespData>();
        respData->status = SEStatus::SUCCESS;
        respData->channelNumber = 3;
        respData->resp = {0x09, 0x00};
        EXPECT_CALL(*seMock, OpenLogicalChannel(_, _)).WillRepeatedly(Return(ByMove(std::move(respData))));
        channel = terminal->OpenLogicalChannel(aid, 0x00, bundleName, pid);

        std::unique_ptr<SEBasicRespData> respBasicData = std::make_unique<SEBasicRespData>();
        respBasicData->status = SEStatus::SUCCESS;
        respBasicData->resp = {0x09, 0x00};
        EXPECT_CALL(*seMock, OpenBasicChannel(_, _)).WillRepeatedly(Return(ByMove(std::move(respBasicData))));
        channel = terminal->OpenBasicChannel(aid, 0x00, bundleName, pid);

        terminal->CloseChannels();
    } catch (const std::exception& e) {
        InfoLog("exception: %s", e.what());
        EXPECT_TRUE(false);
    }
}
}  // namespace test
}  // namespace se
}  // namespace OHOS