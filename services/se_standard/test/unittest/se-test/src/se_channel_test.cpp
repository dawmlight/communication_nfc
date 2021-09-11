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
#include "se_channel.h"

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
#include "osal_bundle_info.h"
#include "se_common_exception.h"
#include "se_end_service.h"
#include "secure_element_reader.h"
#include "secure_element_service.h"
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
class SeChannelTest : public ::testing::Test {
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

    std::shared_ptr<Channel> GetSecureElementBasicChannel();
    std::shared_ptr<Channel> GetSecureElementLogicalChannel(int chNumber);

public:
    sptr<SecureElementReader> reader_;
    sptr<SecureElementService> seService_;
    std::shared_ptr<SeEndService> seServiceManager_;
    sptr<ISecureElementMock> seMock_;
    std::shared_ptr<Context> context_;
    TerminalTable terminalTable_;
};

std::shared_ptr<Channel> SeChannelTest::GetSecureElementBasicChannel()
{
    std::shared_ptr<Terminal> terminal = terminalTable_.at(0).second;

    std::string aid{};
    std::shared_ptr<Channel> seChannel{};

    std::string bundleName{"1234567890"};
    BaseTest::SetUpBundleInfo(bundleName, context_);
    int pid = OHOS::IPCSkeleton::GetCallingPid();

    std::unique_ptr<SEBasicRespData> respData{};
    respData = std::make_unique<SEBasicRespData>();
    respData->status = SEStatus::SUCCESS;
    EXPECT_CALL(*seMock_, OpenBasicChannel(_, _)).WillRepeatedly(Return(ByMove(std::move(respData))));
    BaseTest::SetUpOpenLogicalChannel(seMock_);
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

/**
 * @brief Get PID = 0 Channel
 *
 * @param chNumber the channel number
 * @return std::shared_ptr<Channel>
 */
std::shared_ptr<Channel> SeChannelTest::GetSecureElementLogicalChannel(int chNumber)
{
    std::shared_ptr<Terminal> terminal = terminalTable_.at(0).second;

    std::string aid{};
    std::shared_ptr<Channel> seChannel{};

    std::string bundleName{"1234567890"};
    BaseTest::SetUpBundleInfo(bundleName, context_);
    int pid = OHOS::IPCSkeleton::GetCallingPid();

    std::unique_ptr<SELogicalRespData> respData2{};
    respData2 = std::make_unique<SELogicalRespData>();
    respData2->status = SEStatus::SUCCESS;
    respData2->channelNumber = chNumber++;
    respData2->resp = {(char)0x90, 0x00};
    BaseTest::SetUpOpenLogicalChannelWithPreData(seMock_, respData2);
    aid = "12345";
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
TEST_F(SeChannelTest, Close_IsClosed_Test_1)
{
    std::shared_ptr<Channel> seChannel = GetSecureElementBasicChannel();
    EXPECT_TRUE(seChannel != nullptr);
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
TEST_F(SeChannelTest, Close_IsClosed_Test_2)
{
    std::shared_ptr<Channel> seChannel = GetSecureElementLogicalChannel(1);
    EXPECT_TRUE(seChannel != nullptr);
    EXPECT_FALSE(seChannel->IsClosed());
    EXPECT_FALSE(seChannel->IsBasicChannel());
    EXPECT_CALL(*seMock_, CloseChannel(_)).WillRepeatedly(Return(SEStatus::SUCCESS));
    seChannel->Close();
    EXPECT_TRUE(seChannel->IsClosed());
}

TEST_F(SeChannelTest, Transmit_Test_1)
{
    std::shared_ptr<Channel> seChannel = GetSecureElementLogicalChannel(2);

    // APDU command is empty.
    std::string cmd{};
    try {
        seChannel->Transmit(cmd);
        EXPECT_TRUE(false);
    } catch (const NullPointerError& e) {
        EXPECT_TRUE(true);
    } catch (const std::exception&) {
        EXPECT_TRUE(false);
    }
    std::shared_ptr<security::ChannelAccessRule> chAccecc = seChannel->GetChannelAccess().lock();
    // Channel Access is empty.
    cmd = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00};
    try {
        std::shared_ptr<security::ChannelAccessRule> tmpChannelAccecc{};
        seChannel->SetChannelAccess(tmpChannelAccecc);
        seChannel->Transmit(cmd);
        EXPECT_TRUE(false);
    } catch (const SecurityError& e) {
        EXPECT_TRUE(true);
    } catch (const std::exception&) {
        EXPECT_TRUE(false);
    }
    seChannel->SetChannelAccess(chAccecc);

    int pid = chAccecc->GetCallingPid();
    chAccecc->SetCallingPid(2);
    // Channel Access PID is disaccord with calling pid.
    cmd = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00};
    try {
        seChannel->Transmit(cmd);
        EXPECT_TRUE(false);
    } catch (const SecurityError& e) {
        EXPECT_TRUE(true);
    } catch (const std::exception& e) {
        InfoLog("exception: %s", e.what());
        EXPECT_TRUE(false);
    }
    chAccecc->SetCallingPid(pid);

    // channel is closed
    seChannel->Close();
    EXPECT_TRUE(seChannel->IsClosed());
    cmd = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00};
    try {
        seChannel->Transmit(cmd);
        EXPECT_TRUE(false);
    } catch (const IllegalStateError& e) {
        EXPECT_TRUE(true);
    } catch (const std::exception& e) {
        InfoLog("exception: %s", e.what());
        EXPECT_TRUE(false);
    }
}

TEST_F(SeChannelTest, Transmit_Test_2)
{
    std::shared_ptr<Channel> seChannel = GetSecureElementLogicalChannel(17);
    std::shared_ptr<security::ChannelAccessRule> chAccecc = seChannel->GetChannelAccess().lock();
    chAccecc->SetCallingPid(0);
    // APDU command is less then 4.
    std::string cmd{0x00, 0x00, 0x00};
    try {
        seChannel->Transmit(cmd);
        EXPECT_TRUE(false);
    } catch (const IllegalArgumentError& e) {
        EXPECT_TRUE(true);
    } catch (const std::exception& e) {
        InfoLog("exception: %s", e.what());
        EXPECT_TRUE(false);
    }

    // === specifies 01xx xxxx as further interindustry values. ===
    cmd = {0x60, 0x70, 0x00, 0x00, 0x00};
    try {
        seChannel->Transmit(cmd);
        EXPECT_TRUE(false);
    } catch (const SecurityError& e) {
        EXPECT_TRUE(true);
    } catch (const std::exception& e) {
        InfoLog("exception: %s", e.what());
        EXPECT_TRUE(false);
    }

    chAccecc->SetPrivilegeAccessRule(security::ChannelAccessRule::ACCESSRULE::NEVER);
    cmd = {0x60, (char)0xA4, 0x04, 0x00, 0x00};
    try {
        seChannel->Transmit(cmd);
        EXPECT_TRUE(false);
    } catch (const SecurityError& e) {
        EXPECT_TRUE(true);
    } catch (const std::exception& e) {
        InfoLog("exception: %s", e.what());
        EXPECT_TRUE(false);
    }

    std::string reason{};
    chAccecc->SetAccessRule(security::ChannelAccessRule::ACCESSRULE::NEVER, reason);
    // Channel Access is empty.
    cmd = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00};
    try {
        seChannel->Transmit(cmd);
        EXPECT_TRUE(false);
    } catch (const AccessControlError& e) {
        EXPECT_TRUE(true);
    } catch (const std::exception& e) {
        InfoLog("exception: %s", e.what());
        EXPECT_TRUE(false);
    }

    std::string response{(char)0x90, 0x00};
    EXPECT_CALL(*seMock_, Transmit(_)).WillRepeatedly(Return(response));
    chAccecc->SetAccessRule(security::ChannelAccessRule::ACCESSRULE::ALWAYS, reason);
    chAccecc->SetApduAccessRule(security::ChannelAccessRule::ACCESSRULE::ALWAYS);
    // Channel Access PID is disaccord with calling pid.
    cmd = {0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00};
    try {
        seChannel->Transmit(cmd);
        EXPECT_TRUE(true);
    } catch (const std::exception& e) {
        InfoLog("exception: %s", e.what());
        EXPECT_TRUE(false);
    }
}

TEST_F(SeChannelTest, Transmit_Test_3)
{
    try {
        std::shared_ptr<Terminal> terminal = terminalTable_.at(0).second;
        std::string aid{};
        std::shared_ptr<Channel> seChannel{};

        std::string bundleName{"1234567890"};
        BaseTest::SetUpBundleInfo(bundleName, context_);
        int pid = OHOS::IPCSkeleton::GetCallingPid();

        std::unique_ptr<SELogicalRespData> respData{};
        respData = std::make_unique<SELogicalRespData>();
        respData->status = SEStatus::SUCCESS;
        respData->channelNumber = 20;
        respData->resp = {(char)0x90, 0x00};
        BaseTest::SetUpOpenLogicalChannelWithPreData(seMock_, respData);
        aid = "12345";
        seChannel = terminal->OpenLogicalChannel(aid, 0x0C, bundleName, pid);

        std::shared_ptr<security::ChannelAccessRule> chAccecc = seChannel->GetChannelAccess().lock();
        chAccecc->SetCallingPid(0);

        std::string cmd{};
        std::string reason{};
        std::string response{(char)0x90, 0x00};
        EXPECT_CALL(*seMock_, Transmit(_)).WillRepeatedly(Return(response));
        chAccecc->SetAccessRule(security::ChannelAccessRule::ACCESSRULE::ALWAYS, reason);
        chAccecc->SetApduAccessRule(security::ChannelAccessRule::ACCESSRULE::ALWAYS);

        // SetChannelToClassByte
        cmd = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00};
        seChannel->Transmit(cmd);
        EXPECT_TRUE(false);
    } catch (const IllegalArgumentError& e) {
        EXPECT_TRUE(true);
    } catch (const std::exception& e) {
        InfoLog("exception: %s", e.what());
        EXPECT_TRUE(false);
    }
}

TEST(SeChannelTest1, GetSelectResponse_Test)
{
    std::shared_ptr<Terminal> terminal{};
    std::string selectResponse{};
    std::string aid{};
    std::shared_ptr<Channel> seChannel = std::make_shared<Channel>(terminal, 0, selectResponse, aid);
    EXPECT_TRUE(seChannel->IsBasicChannel());
    EXPECT_TRUE(seChannel->GetChannelNumber() == 0);
    EXPECT_TRUE(seChannel->GetSelectResponse().empty());
    EXPECT_FALSE(seChannel->HasSelectedAid());
    selectResponse = {(char)0x90, 0x00};
    aid = {0x60, (char)0x00, 0x00, 0x00, 0x00};
    seChannel = std::make_shared<Channel>(terminal, 1, selectResponse, aid);
    EXPECT_FALSE(seChannel->IsBasicChannel());
    EXPECT_TRUE(seChannel->GetChannelNumber() == 1);
    EXPECT_TRUE(0 == seChannel->GetSelectResponse().compare({(char)0x90, 0x00}));
    EXPECT_TRUE(seChannel->HasSelectedAid());
}
}  // namespace test
}  // namespace se
}  // namespace OHOS