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
#include "secure_element_channel.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>

#include "access_rule_data_common.h"
#include "base_test.h"
#include "bundle_manager.h"
#include "channel_access_rule.h"
#include "context.h"
#include "ipc_object_stub.h"
#include "ipc_skeleton.h"
#include "isecure_element_mock.h"
#include "loghelper.h"
#include "se_channel.h"
#include "se_common_exception.h"
#include "secure_element_reader.h"
#include "secure_element_service.h"
#include "secure_element_session.h"
#include "terminal.h"
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
class SecureElementChannelTest : public ::testing::Test {
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

    std::shared_ptr<Channel> GetSecureElementBasicChannel();
    std::shared_ptr<Channel> GetSecureElementLogicalChannel(int chNumber, std::string aid = {"12345"});

public:
    sptr<SecureElementSession> seSession_;
    sptr<SecureElementReader> reader_;
    sptr<SecureElementService> seService_;
    std::shared_ptr<SeEndService> seServiceManager_;
    sptr<ISecureElementMock> seMock_;
    std::shared_ptr<Context> context_;
    TerminalTable terminalTable_;
};

std::shared_ptr<Channel> SecureElementChannelTest::GetSecureElementBasicChannel()
{
    std::shared_ptr<Terminal> terminal = terminalTable_.at(0).second;

    std::string aid{};
    std::shared_ptr<Channel> seChannel{};

    std::string bundleName{"1234567890"};
    BaseTest::SetUpBundleInfo(bundleName, context_);
    int pid = OHOS::IPCSkeleton::GetCallingPid();

    BaseTest::SetUpOpenLogicalChannel(seMock_);
    std::unique_ptr<SEBasicRespData> respData{};
    respData = std::make_unique<SEBasicRespData>();
    respData->status = SEStatus::SUCCESS;
    EXPECT_CALL(*seMock_, OpenBasicChannel(_, _)).WillRepeatedly(Return(ByMove(std::move(respData))));
    EXPECT_CALL(*seMock_, Transmit(_)).WillOnce(Return(RESPONSE_REFERESH_TAG_D0));
    aid = "12345";
    try {
        seChannel = terminal->OpenBasicChannel(aid, 0x0C, bundleName, pid);
        EXPECT_TRUE(true);
    } catch (const std::exception& e) {
        InfoLog("exception: %s", e.what());
        EXPECT_FALSE(true);
    }
    return seChannel;
}

std::shared_ptr<Channel> SecureElementChannelTest::GetSecureElementLogicalChannel(int chNumber, std::string aid)
{
    std::shared_ptr<Terminal> terminal = terminalTable_.at(0).second;

    std::shared_ptr<Channel> seChannel{};

    std::string bundleName{"1234567890"};
    BaseTest::SetUpBundleInfo(bundleName, context_);
    int pid = OHOS::IPCSkeleton::GetCallingPid();

    std::unique_ptr<SELogicalRespData> respData{};
    respData = std::make_unique<SELogicalRespData>();
    respData->status = SEStatus::SUCCESS;
    respData->channelNumber = chNumber;
    respData->resp = {(char)0x90, 0x00};
    BaseTest::SetUpOpenLogicalChannelWithPreData(seMock_, respData);
    try {
        seChannel = terminal->OpenLogicalChannel(aid, 0x0C, bundleName, pid);
        EXPECT_TRUE(true);
    } catch (const std::exception& e) {
        InfoLog("exception: %s", e.what());
        EXPECT_FALSE(true);
    }
    return seChannel;
}

/**
 * @brief BasicChannel is closed.
 */
TEST_F(SecureElementChannelTest, Close_IsClosed_Test_1)
{
    std::shared_ptr<Channel> channel = GetSecureElementBasicChannel();
    sptr<SecureElementChannel> seChannel = new SecureElementChannel(channel);
    EXPECT_TRUE(seChannel);
    EXPECT_FALSE(seChannel->IsClosed());
    EXPECT_TRUE(seChannel->IsBasicChannel());
    // for basic channel for open default basic channel
    std::string response{(char)0x90, 0x00};
    EXPECT_CALL(*seMock_, Transmit(_)).WillRepeatedly(Return(response));
    // ===
    EXPECT_CALL(*seMock_, CloseChannel(_)).WillRepeatedly(Return(SEStatus::SUCCESS));
    seChannel->Close();
    EXPECT_TRUE(seChannel->IsClosed());
    EXPECT_TRUE(seChannel->IsBasicChannel());
}

/**
 * @brief LogicalChannel is closed.
 */
TEST_F(SecureElementChannelTest, Close_IsClosed_Test_2)
{
    std::shared_ptr<Channel> channel = GetSecureElementLogicalChannel(2);
    sptr<SecureElementChannel> seChannel = new SecureElementChannel(channel);
    EXPECT_TRUE(seChannel);
    EXPECT_FALSE(seChannel->IsClosed());
    EXPECT_FALSE(seChannel->IsBasicChannel());
    EXPECT_CALL(*seMock_, CloseChannel(_)).WillRepeatedly(Return(SEStatus::SUCCESS));
    seChannel->Close();
    EXPECT_TRUE(seChannel->IsClosed());
}

TEST_F(SecureElementChannelTest, Transmit_Test)
{
    std::shared_ptr<Channel> channel = GetSecureElementLogicalChannel(16);
    sptr<SecureElementChannel> seChannel = new SecureElementChannel(channel);

    // APDU command is empty.
    std::string cmd{};
    std::string reason{};
    std::shared_ptr<security::ChannelAccessRule> chAccecc = channel->GetChannelAccess().lock();
    std::string response{};
    EXPECT_CALL(*seMock_, Transmit(_)).WillRepeatedly(Return(response));
    chAccecc->SetAccessRule(security::ChannelAccessRule::ACCESSRULE::ALWAYS, reason);
    chAccecc->SetApduAccessRule(security::ChannelAccessRule::ACCESSRULE::ALWAYS);
    // Channel Access PID is disaccord with calling pid.
    cmd = {0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00};
    try {
        seChannel->Transmit(cmd);
        EXPECT_TRUE(false);
    } catch (const ServiceSpecificError& e) {
        InfoLog("exception: %s", e.what());
        EXPECT_TRUE(true);
    } catch (const std::exception& e) {
        InfoLog("exception: %s", e.what());
        EXPECT_TRUE(false);
    }

    response = {(char)0x90, 0x00};
    EXPECT_CALL(*seMock_, Transmit(_)).WillRepeatedly(Return(response));
    try {
        seChannel->Transmit(cmd);
        EXPECT_TRUE(true);
    } catch (const std::exception& e) {
        InfoLog("exception: %s", e.what());
        EXPECT_TRUE(false);
    }
}

TEST_F(SecureElementChannelTest, SelectNext_Test_1)
{
    std::shared_ptr<Channel> seChannel = GetSecureElementLogicalChannel(16, {});
    sptr<SecureElementChannel> channel = new SecureElementChannel(seChannel);

    std::shared_ptr<security::ChannelAccessRule> chAccecc = seChannel->GetChannelAccess().lock();
    // Channel Access is empty.
    try {
        std::shared_ptr<security::ChannelAccessRule> tmpChannelAccecc{};
        seChannel->SetChannelAccess(tmpChannelAccecc);
        channel->SelectNext();
        EXPECT_TRUE(false);
    } catch (const IllegalStateError& e) {
        EXPECT_TRUE(true);
    } catch (const std::exception&) {
        EXPECT_TRUE(false);
    }
    seChannel->SetChannelAccess(chAccecc);

    int pid = chAccecc->GetCallingPid();
    chAccecc->SetCallingPid(2);
    // Channel Access PID is disaccord with calling pid.
    try {
        channel->SelectNext();
        EXPECT_TRUE(false);
    } catch (const SecurityError& e) {
        EXPECT_TRUE(true);
    } catch (const std::exception& e) {
        InfoLog("exception: %s", e.what());
        EXPECT_TRUE(false);
    }
    chAccecc->SetCallingPid(pid);

    // AID is empty
    try {
        channel->SelectNext();
        EXPECT_TRUE(false);
    } catch (const UnsupportedOperationError& e) {
        EXPECT_TRUE(true);
    } catch (const std::exception& e) {
        InfoLog("exception: %s", e.what());
        EXPECT_TRUE(false);
    }

    // channel is closed
    seChannel->Close();
    EXPECT_TRUE(seChannel->IsClosed());
    try {
        channel->SelectNext();
        EXPECT_TRUE(false);
    } catch (const IllegalStateError& e) {
        EXPECT_TRUE(true);
    } catch (const std::exception& e) {
        InfoLog("exception: %s", e.what());
        EXPECT_TRUE(false);
    }
}

TEST_F(SecureElementChannelTest, SelectNext_Test_2)
{
    std::shared_ptr<Channel> seChannel = GetSecureElementLogicalChannel(1);
    sptr<SecureElementChannel> channel = new SecureElementChannel(seChannel);

    std::shared_ptr<security::ChannelAccessRule> chAccecc = seChannel->GetChannelAccess().lock();

    std::string reason{};
    std::string response{(char)0x90, 0x00};
    EXPECT_CALL(*seMock_, Transmit(_)).WillRepeatedly(Return(response));
    chAccecc->SetAccessRule(security::ChannelAccessRule::ACCESSRULE::ALWAYS, reason);
    chAccecc->SetApduAccessRule(security::ChannelAccessRule::ACCESSRULE::ALWAYS);
    try {
        EXPECT_TRUE(channel->SelectNext());
        EXPECT_TRUE(0 == channel->GetSelectResponse().compare({(char)0x90, 0x00}));
    } catch (const std::exception& e) {
        InfoLog("exception: %s", e.what());
        EXPECT_TRUE(false);
    }

    response = {0x62, 0x00};
    EXPECT_CALL(*seMock_, Transmit(_)).WillRepeatedly(Return(response));
    try {
        EXPECT_TRUE(channel->SelectNext());
        EXPECT_TRUE(0 == channel->GetSelectResponse().compare({0x62, 0x00}));
    } catch (const std::exception& e) {
        InfoLog("exception: %s", e.what());
        EXPECT_TRUE(false);
    }

    response = {0x63, 0x00};
    EXPECT_CALL(*seMock_, Transmit(_)).WillRepeatedly(Return(response));
    try {
        EXPECT_TRUE(channel->SelectNext());
        EXPECT_TRUE(0 == channel->GetSelectResponse().compare({0x63, 0x00}));
    } catch (const std::exception& e) {
        InfoLog("exception: %s", e.what());
        EXPECT_TRUE(false);
    }

    response = {0x6A, 0x00};
    EXPECT_CALL(*seMock_, Transmit(_)).WillRepeatedly(Return(response));
    try {
        EXPECT_FALSE(channel->SelectNext());
    } catch (const std::exception& e) {
        InfoLog("exception: %s", e.what());
        EXPECT_TRUE(false);
    }

    response = {0x00, 0x00};
    EXPECT_CALL(*seMock_, Transmit(_)).WillRepeatedly(Return(response));
    try {
        channel->SelectNext();
        EXPECT_TRUE(false);
    } catch (const UnsupportedOperationError& e) {
        InfoLog("exception: %s", e.what());
        EXPECT_TRUE(true);
    } catch (const std::exception& e) {
        InfoLog("exception: %s", e.what());
        EXPECT_TRUE(false);
    }

    response = {};
    EXPECT_CALL(*seMock_, Transmit(_)).WillRepeatedly(Return(response));
    try {
        channel->SelectNext();
        EXPECT_TRUE(false);
    } catch (const ServiceSpecificError& e) {
        InfoLog("exception: %s", e.what());
        EXPECT_TRUE(true);
    } catch (const std::exception& e) {
        InfoLog("exception: %s", e.what());
        EXPECT_TRUE(false);
    }
}
}  // namespace test
}  // namespace se
}  // namespace OHOS