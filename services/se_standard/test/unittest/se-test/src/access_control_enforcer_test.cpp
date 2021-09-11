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
#include "access_control_enforcer.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>
#include <string>
#include <vector>

#define private public
#include "access_rule_application_controller.h"
#include "access_rule_data_common.h"
#include "base_test.h"
#include "bundle_manager.h"
#include "channel_access_rule.h"
#include "context.h"
#include "isecure_element_mock.h"
#include "osal_bundle_info.h"
#include "se_channel.h"
#include "secure_element_session.h"
#include "terminal.h"
#undef private

using namespace OHOS::se::security;
using namespace OHOS::se;
using namespace osal;
using namespace testing;
using namespace OHOS::hardware::se::v1_0;

namespace OHOS::se::test {
class AccessControlEnforcerTest : public ::testing::Test {
public:
    virtual void SetUp() override
    {
        std::cout << " AccessControlEnforcerTest set up." << std::endl;
        secureElementMock_ = BaseTest::LoadSecureElement(true);
        EXPECT_CALL(*secureElementMock_, Initialize(_)).Times(AnyNumber());
        terminal_ = std::make_shared<Terminal>("AccessControlEnforcerTest", context_);
        terminal_->Initialize(secureElementMock_);
        accessControlEnforcer_ = std::make_shared<AccessControlEnforcer>(terminal_);
    }

    virtual void TearDown() override
    {
        std::cout << " AccessControlEnforcerTest tear down." << std::endl;
    }

public:
    std::shared_ptr<Context> context_;
    sptr<ISecureElementMock> secureElementMock_;
    std::shared_ptr<Terminal> terminal_;
    std::shared_ptr<AccessControlEnforcer> accessControlEnforcer_;
};

/**
 * @tc.number    : ACCESS_CONTROL_ENFORCER_TEST_001
 * @tc.name      : Initialize_Ara_Test
 * @tc.desc      : Initialize ara
 */
TEST_F(AccessControlEnforcerTest, Initialize_Ara_Test)
{
    try {
        std::string apduSuccess = {(char)0x90, 0x00};
        std::unique_ptr<OHOS::hardware::se::v1_0::ISecureElement::LogicalRespData> resData(
            std::make_unique<OHOS::hardware::se::v1_0::ISecureElement::LogicalRespData>());
        resData->status = SEStatus::SUCCESS;
        resData->channelNumber = 1;
        resData->resp = apduSuccess;
        EXPECT_CALL(*secureElementMock_, OpenLogicalChannel(_, _)).WillOnce(Return(ByMove(std::move(resData))));
        EXPECT_CALL(*secureElementMock_, Transmit(_))
            .WillOnce(Return(RESPONSE_REFERESH_TAG_D0))
            .WillOnce(Return(RESPONSE_ALL));
        accessControlEnforcer_->Initialize();
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : ACCESS_CONTROL_ENFORCER_TEST_002
 * @tc.name      : Initialize_No_Ara_No_Arf_Test
 * @tc.desc      : Initialize neither ara nor arf
 */
TEST_F(AccessControlEnforcerTest, Initialize_No_Ara_No_Arf_Test)
{
    try {
        accessControlEnforcer_->Initialize();
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : ACCESS_CONTROL_ENFORCER_TEST_003
 * @tc.name      : GetDefaultAccessControlAid_Test
 * @tc.desc      : Get default access control aid
 */
TEST_F(AccessControlEnforcerTest, GetDefaultAccessControlAid_Test)
{
    try {
        EXPECT_EQ(ARA_M_AID, accessControlEnforcer_->GetDefaultAccessControlAid());
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : ACCESS_CONTROL_ENFORCER_TEST_004
 * @tc.name      : BundleManager_Test
 * @tc.desc      : Get bundle manager
 */
TEST_F(AccessControlEnforcerTest, BundleManager_Test)
{
    try {
        std::shared_ptr<osal::BundleManager> bundleManager1 = std::make_shared<osal::BundleManager>();
        accessControlEnforcer_->SetBundleManager(bundleManager1);
        std::shared_ptr<osal::BundleManager> bundleManager2 = accessControlEnforcer_->GetBundleManager().lock();
        EXPECT_TRUE(bundleManager2);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : ACCESS_CONTROL_ENFORCER_TEST_005
 * @tc.name      : GetTerminal_Test
 * @tc.desc      : Get terminal
 */
TEST_F(AccessControlEnforcerTest, GetTerminal_Test)
{
    try {
        std::shared_ptr<Terminal> terminal = accessControlEnforcer_->GetTerminal().lock();
        EXPECT_TRUE(terminal);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : ACCESS_CONTROL_ENFORCER_TEST_006
 * @tc.name      : GetAccessRuleCache_Test
 * @tc.desc      : Get access rule cache
 */
TEST_F(AccessControlEnforcerTest, GetAccessRuleCache_Test)
{
    try {
        std::shared_ptr<AccessRuleCache> accessRuleCache = accessControlEnforcer_->GetAccessRuleCache().lock();
        EXPECT_TRUE(accessRuleCache);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : ACCESS_CONTROL_ENFORCER_TEST_007
 * @tc.name      : Reset_Test
 * @tc.desc      : Reset
 */
TEST_F(AccessControlEnforcerTest, Reset_Test)
{
    try {
        accessControlEnforcer_->Reset();
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : ACCESS_CONTROL_ENFORCER_TEST_008
 * @tc.name      : IsNoRuleFound_Test
 * @tc.desc      : No rule found
 */
TEST_F(AccessControlEnforcerTest, IsNoRuleFound_Test)
{
    try {
        EXPECT_FALSE(accessControlEnforcer_->IsNoRuleFound());
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : ACCESS_CONTROL_ENFORCER_TEST_009
 * @tc.name      : CheckCommand_Always_Test
 * @tc.desc      : Check command always
 */
TEST_F(AccessControlEnforcerTest, CheckCommand_Always_Test)
{
    try {
        std::shared_ptr<Terminal> terminal = std::shared_ptr<Terminal>();
        int channelNumber = 1;
        std::string selectResponse = "";
        std::string aid = "";
        std::shared_ptr<SeChannel> channel = std::make_shared<SeChannel>(terminal, channelNumber, selectResponse, aid);

        std::shared_ptr<ChannelAccessRule> channelAccessRule = std::make_shared<ChannelAccessRule>();
        channelAccessRule->SetAccessRule(ChannelAccessRule::ACCESSRULE::ALWAYS, "CheckCommand_Test");
        channelAccessRule->SetApduAccessRule(ChannelAccessRule::ACCESSRULE::ALWAYS);
        channelAccessRule->SetNFCEventAccessRule(ChannelAccessRule::ACCESSRULE::ALWAYS);
        channel->SetChannelAccess(channelAccessRule);

        std::string command = {};
        accessControlEnforcer_->CheckCommand(channel, command);
        EXPECT_TRUE(true);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : ACCESS_CONTROL_ENFORCER_TEST_010
 * @tc.name      : CheckCommand_NoChannelAccessRule_Test
 * @tc.desc      : Check command with no channel access rule
 */
TEST_F(AccessControlEnforcerTest, CheckCommand_NoChannelAccessRule_Test)
{
    try {
        std::shared_ptr<Terminal> terminal = std::shared_ptr<Terminal>();
        int channelNumber = 1;
        std::string selectResponse = "";
        std::string aid = "";
        std::shared_ptr<SeChannel> channel = std::make_shared<SeChannel>(terminal, channelNumber, selectResponse, aid);

        std::string command = {};
        accessControlEnforcer_->CheckCommand(channel, command);
        EXPECT_TRUE(false);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(true);
    }
}

/**
 * @tc.number    : ACCESS_CONTROL_ENFORCER_TEST_011
 * @tc.name      : CheckCommand_Never_Test
 * @tc.desc      : Check command never
 */
TEST_F(AccessControlEnforcerTest, CheckCommand_Never_Test)
{
    try {
        std::shared_ptr<Terminal> terminal = std::shared_ptr<Terminal>();
        int channelNumber = 1;
        std::string selectResponse = "";
        std::string aid = "";
        std::shared_ptr<SeChannel> channel = std::make_shared<SeChannel>(terminal, channelNumber, selectResponse, aid);

        std::shared_ptr<ChannelAccessRule> channelAccessRule = std::make_shared<ChannelAccessRule>();
        channelAccessRule->SetAccessRule(ChannelAccessRule::ACCESSRULE::NEVER, "CheckCommand_Test");
        channel->SetChannelAccess(channelAccessRule);

        std::string command = {};
        accessControlEnforcer_->CheckCommand(channel, command);
        EXPECT_TRUE(false);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(true);
    }
}

/**
 * @tc.number    : ACCESS_CONTROL_ENFORCER_TEST_012
 * @tc.name      : CheckCommand_ApduNever_Test
 * @tc.desc      : Check command apdu-never
 */
TEST_F(AccessControlEnforcerTest, CheckCommand_ApduNever_Test)
{
    try {
        std::shared_ptr<Terminal> terminal = std::shared_ptr<Terminal>();
        int channelNumber = 1;
        std::string selectResponse = "";
        std::string aid = "";
        std::shared_ptr<SeChannel> channel = std::make_shared<SeChannel>(terminal, channelNumber, selectResponse, aid);

        std::shared_ptr<ChannelAccessRule> channelAccessRule = std::make_shared<ChannelAccessRule>();
        channelAccessRule->SetAccessRule(ChannelAccessRule::ACCESSRULE::ALWAYS, "CheckCommand_Test");
        channelAccessRule->SetApduAccessRule(ChannelAccessRule::ACCESSRULE::NEVER);
        channel->SetChannelAccess(channelAccessRule);

        std::string command = {};
        accessControlEnforcer_->CheckCommand(channel, command);
        EXPECT_TRUE(false);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(true);
    }
}

/**
 * @tc.number    : ACCESS_CONTROL_ENFORCER_TEST_013
 * @tc.name      : CheckCommand_ApduFilter_Test
 * @tc.desc      : Check command apdu-filter
 */
TEST_F(AccessControlEnforcerTest, CheckCommand_ApduFilter_Test)
{
    try {
        std::shared_ptr<Terminal> terminal = std::shared_ptr<Terminal>();
        int channelNumber = 1;
        std::string selectResponse = "";
        std::string aid = "";
        std::shared_ptr<SeChannel> channel = std::make_shared<SeChannel>(terminal, channelNumber, selectResponse, aid);

        std::shared_ptr<ChannelAccessRule> channelAccessRule = std::make_shared<ChannelAccessRule>();
        channelAccessRule->SetAccessRule(ChannelAccessRule::ACCESSRULE::ALWAYS, "CheckCommand_Test");
        channelAccessRule->SetHasApduFilter(true);
        std::string apduFilter1 = {(char)0x80, (char)0xCA, 0x00, 0x00, (char)0xFF, (char)0xFF, (char)0x00, (char)0x00};
        std::vector<std::string> apduFilters;
        apduFilters.push_back(apduFilter1);
        channelAccessRule->SetApduFilters(apduFilters);
        channel->SetChannelAccess(channelAccessRule);

        std::string command = {(char)0x80, (char)0xCA, 0x00, 0x00, (char)0x11, (char)0x22, (char)0x33, (char)0x44};
        accessControlEnforcer_->CheckCommand(channel, command);
        EXPECT_TRUE(true);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : ACCESS_CONTROL_ENFORCER_TEST_014
 * @tc.name      : CheckCommand_ApduFilter_Null_Test
 * @tc.desc      : Check command with null apdu-filter
 */
TEST_F(AccessControlEnforcerTest, CheckCommand_ApduFilter_Null_Test)
{
    try {
        std::shared_ptr<Terminal> terminal = std::shared_ptr<Terminal>();
        int channelNumber = 1;
        std::string selectResponse = "";
        std::string aid = "";
        std::shared_ptr<SeChannel> channel = std::make_shared<SeChannel>(terminal, channelNumber, selectResponse, aid);

        std::shared_ptr<ChannelAccessRule> channelAccessRule = std::make_shared<ChannelAccessRule>();
        channelAccessRule->SetAccessRule(ChannelAccessRule::ACCESSRULE::ALWAYS, "CheckCommand_Test");
        channelAccessRule->SetHasApduFilter(true);
        std::vector<std::string> apduFilters;
        channelAccessRule->SetApduFilters(apduFilters);
        channel->SetChannelAccess(channelAccessRule);

        std::string command = {(char)0x88, (char)0xCA, 0x00, 0x00, (char)0x11, (char)0x22, (char)0x33, (char)0x44};
        accessControlEnforcer_->CheckCommand(channel, command);
        EXPECT_TRUE(false);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(true);
    }
}

/**
 * @tc.number    : ACCESS_CONTROL_ENFORCER_TEST_015
 * @tc.name      : CheckCommand_Channel_Expired_Test
 * @tc.desc      : Check command when channel expired
 */
TEST_F(AccessControlEnforcerTest, CheckCommand_Channel_Expired_Test)
{
    try {
        std::shared_ptr<SeChannel> channel = std::shared_ptr<SeChannel>();

        std::string command = {(char)0x88, (char)0xCA, 0x00, 0x00, (char)0x11, (char)0x22, (char)0x33, (char)0x44};
        accessControlEnforcer_->CheckCommand(channel, command);
        EXPECT_TRUE(false);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(true);
    }
}

/**
 * @tc.number    : ACCESS_CONTROL_ENFORCER_TEST_016
 * @tc.name      : EstablishChannelAccessRuleForBundle_Test
 * @tc.desc      : Establish chanel access rule for bundle
 */
TEST_F(AccessControlEnforcerTest, EstablishChannelAccessRuleForBundle_Test)
{
    try {
        std::string apduSuccess = {(char)0x90, 0x00};
        std::unique_ptr<OHOS::hardware::se::v1_0::ISecureElement::LogicalRespData> resData(
            std::make_unique<OHOS::hardware::se::v1_0::ISecureElement::LogicalRespData>());
        resData->status = SEStatus::SUCCESS;
        resData->channelNumber = 1;
        resData->resp = apduSuccess;
        EXPECT_CALL(*secureElementMock_, OpenLogicalChannel(_, _)).WillOnce(Return(ByMove(std::move(resData))));
        EXPECT_CALL(*secureElementMock_, Transmit(_))
            .WillOnce(Return(RESPONSE_REFERESH_TAG_D0))
            .WillOnce(Return(RESPONSE_ALL2));
        accessControlEnforcer_->Initialize();

        std::string aid = {
            (char)0xA0, 0x00, 0x00, 0x03, 0x33, 0x01, 0x01, 0x02, 0x00, 0x63, 0x48, 0x57, 0x50, 0x41, 0x59, 0x05};
        std::string bundleName = "EstablishChannelAccessRuleForBundle_Test";
        std::string signature = "1234567890";
        std::shared_ptr<osal::BundleManager> bundleManager = std::make_shared<osal::BundleManager>();
        std::shared_ptr<osal::BundleInfo> bundleInfo = std::make_shared<osal::BundleInfo>();
        bundleInfo->mBundleName_ = bundleName;
        bundleInfo->signatures_.push_back(signature);
        bundleManager->SetBundleInfo(bundleName, bundleInfo);
        accessControlEnforcer_->SetBundleManager(bundleManager);
        bool checkRefreshTag = false;
        std::shared_ptr<ChannelAccessRule> ca =
            accessControlEnforcer_->EstablishChannelAccessRuleForBundle(aid, bundleName, checkRefreshTag);
        EXPECT_EQ(ChannelAccessRule::ACCESSRULE::ALWAYS, ca->GetAccessRule());
        EXPECT_EQ(ChannelAccessRule::ACCESSRULE::ALWAYS, ca->GetApduAccessRule());
        EXPECT_FALSE(ca->HasApduFilter());
        EXPECT_EQ(ChannelAccessRule::ACCESSRULE::ALWAYS, ca->GetNFCEventAccessRule());
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : ACCESS_CONTROL_ENFORCER_TEST_017
 * @tc.name      : EstablishChannelAccessRuleForBundle_NoRulesRead_Test
 * @tc.desc      : Establish chanel access rule for bundle with no rules read
 */
TEST_F(AccessControlEnforcerTest, EstablishChannelAccessRuleForBundle_NoRulesRead_Test)
{
    try {
        accessControlEnforcer_->Initialize();

        std::string aid = "123456";
        std::string bundleName = "EstablishChannelAccessRuleForBundle_Test";
        bool checkRefreshTag = true;
        std::shared_ptr<ChannelAccessRule> ca =
            accessControlEnforcer_->EstablishChannelAccessRuleForBundle(aid, bundleName, checkRefreshTag);
        EXPECT_EQ(ChannelAccessRule::ACCESSRULE::NEVER, ca->GetAccessRule());
        EXPECT_EQ(ChannelAccessRule::ACCESSRULE::NEVER, ca->GetApduAccessRule());
        EXPECT_FALSE(ca->HasApduFilter());
        EXPECT_EQ(ChannelAccessRule::ACCESSRULE::NEVER, ca->GetNFCEventAccessRule());
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : ACCESS_CONTROL_ENFORCER_TEST_018
 * @tc.name      : EstablishChannelAccessRuleForBundle_EmptyBundleName_Test
 * @tc.desc      : Establish chanel access rule for bundle with empty bundle name
 */
TEST_F(AccessControlEnforcerTest, EstablishChannelAccessRuleForBundle_EmptyBundleName_Test)
{
    try {
        std::string aid = "123456";
        std::string bundleName = "";
        bool checkRefreshTag = true;
        accessControlEnforcer_->EstablishChannelAccessRuleForBundle(aid, bundleName, checkRefreshTag);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(true);
    }
}

/**
 * @tc.number    : ACCESS_CONTROL_ENFORCER_TEST_019
 * @tc.name      : GetAccessRule_Test
 * @tc.desc      : Get access rule
 */
TEST_F(AccessControlEnforcerTest, GetAccessRule_Test)
{
    try {
        std::string apduSuccess = {(char)0x90, 0x00};
        std::unique_ptr<OHOS::hardware::se::v1_0::ISecureElement::LogicalRespData> resData(
            std::make_unique<OHOS::hardware::se::v1_0::ISecureElement::LogicalRespData>());
        resData->status = SEStatus::SUCCESS;
        resData->channelNumber = 1;
        resData->resp = apduSuccess;
        EXPECT_CALL(*secureElementMock_, OpenLogicalChannel(_, _)).WillOnce(Return(ByMove(std::move(resData))));
        EXPECT_CALL(*secureElementMock_, Transmit(_))
            .WillOnce(Return(RESPONSE_REFERESH_TAG_D0))
            .WillOnce(Return(RESPONSE_ALL));
        accessControlEnforcer_->Initialize();

        std::string aid = {
            (char)0xA0, 0x00, 0x00, 0x03, 0x33, 0x01, 0x01, 0x02, 0x00, 0x63, 0x48, 0x57, 0x50, 0x41, 0x59, 0x05};
        std::vector<std::string> hashes;
        std::string hash = {0x00};
        hashes.push_back(hash);
        std::shared_ptr<ChannelAccessRule> channelAccessRule = accessControlEnforcer_->GetAccessRule(aid, hashes);
        EXPECT_EQ(ChannelAccessRule::ACCESSRULE::ALWAYS, channelAccessRule->GetAccessRule());
        EXPECT_EQ(ChannelAccessRule::ACCESSRULE::ALWAYS, channelAccessRule->GetApduAccessRule());
        EXPECT_FALSE(channelAccessRule->HasApduFilter());
        EXPECT_EQ(ChannelAccessRule::ACCESSRULE::ALWAYS, channelAccessRule->GetNFCEventAccessRule());
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : ACCESS_CONTROL_ENFORCER_TEST_020
 * @tc.name      : GetAccessRule_NoRulesRead_Test
 * @tc.desc      : Get access rule when no rules read
 */
TEST_F(AccessControlEnforcerTest, GetAccessRule_NoRulesRead_Test)
{
    try {
        std::string aid = {};
        std::vector<std::string> hashes;
        std::shared_ptr<ChannelAccessRule> channelAccessRule = accessControlEnforcer_->GetAccessRule(aid, hashes);
        EXPECT_EQ(ChannelAccessRule::ACCESSRULE::NEVER, channelAccessRule->GetAccessRule());
        EXPECT_EQ(ChannelAccessRule::ACCESSRULE::NEVER, channelAccessRule->GetApduAccessRule());
        EXPECT_FALSE(channelAccessRule->HasApduFilter());
        EXPECT_EQ(ChannelAccessRule::ACCESSRULE::NEVER, channelAccessRule->GetNFCEventAccessRule());
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : ACCESS_CONTROL_ENFORCER_TEST_021
 * @tc.name      : IsNfcEventAllowed_Test
 * @tc.desc      : Is nfc event allowed
 */
TEST_F(AccessControlEnforcerTest, IsNfcEventAllowed_Test)
{
    try {
        std::string apduSuccess = {(char)0x90, 0x00};
        std::unique_ptr<OHOS::hardware::se::v1_0::ISecureElement::LogicalRespData> resData(
            std::make_unique<OHOS::hardware::se::v1_0::ISecureElement::LogicalRespData>());
        resData->status = SEStatus::SUCCESS;
        resData->channelNumber = 1;
        resData->resp = apduSuccess;
        EXPECT_CALL(*secureElementMock_, OpenLogicalChannel(_, _)).WillOnce(Return(ByMove(std::move(resData))));
        EXPECT_CALL(*secureElementMock_, Transmit(_))
            .WillOnce(Return(RESPONSE_REFERESH_TAG_D0))
            .WillOnce(Return(RESPONSE_ALL2));
        accessControlEnforcer_->Initialize();

        std::string bundleName = "IsNfcEventAllowed_Test";
        std::shared_ptr<osal::BundleManager> bundleManager = std::make_shared<osal::BundleManager>();
        std::shared_ptr<osal::BundleInfo> bundleInfo = std::make_shared<osal::BundleInfo>();
        bundleInfo->mBundleName_ = bundleName;
        std::string signature = "1234567890";
        bundleInfo->signatures_.push_back(signature);
        bundleManager->SetBundleInfo(bundleName, bundleInfo);
        accessControlEnforcer_->SetBundleManager(bundleManager);
        std::string aid = {
            (char)0xA0, 0x00, 0x00, 0x03, 0x33, 0x01, 0x01, 0x02, 0x00, 0x63, 0x48, 0x57, 0x50, 0x41, 0x59, 0x05};
        std::vector<std::string> bundleNames;
        bundleNames.push_back(bundleName);
        std::vector<bool> nfcAlloweds = accessControlEnforcer_->IsNfcEventAllowed(aid, bundleNames);
        EXPECT_TRUE(nfcAlloweds[0]);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : ACCESS_CONTROL_ENFORCER_TEST_022
 * @tc.name      : IsNfcEventAllowed_NoRulesRead_Test
 * @tc.desc      : Is nfc event allowed when no rules read
 */
TEST_F(AccessControlEnforcerTest, IsNfcEventAllowed_NoRulesRead_Test)
{
    try {
        accessControlEnforcer_->Initialize();
        std::string aid = {};
        std::vector<std::string> bundleNames;
        bundleNames.push_back("IsNfcEventAllowed_Test");
        std::vector<bool> nfcAlloweds = accessControlEnforcer_->IsNfcEventAllowed(aid, bundleNames);
        EXPECT_FALSE(nfcAlloweds[0]);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : ACCESS_CONTROL_ENFORCER_TEST_023
 * @tc.name      : CheckCarrierPrivilege_Test
 * @tc.desc      : Check carrier privilege
 */
TEST_F(AccessControlEnforcerTest, CheckCarrierPrivilege_Test)
{
    try {
        std::shared_ptr<osal::BundleInfo> bInfo = std::make_shared<osal::BundleInfo>();
        bInfo->mBundleName_ = "CheckCarrierPrivilege_Test";
        bool checkRefreshTag = true;
        EXPECT_FALSE(accessControlEnforcer_->CheckCarrierPrivilege(bInfo, checkRefreshTag));
    } catch (std::exception& e) {
        // No bundle manager
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(true);
    }
}

/**
 * @tc.number    : ACCESS_CONTROL_ENFORCER_TEST_024
 * @tc.name      : CheckCarrierPrivilege_NoRule_Test
 * @tc.desc      : Check carrier privilege with no rules
 */
TEST_F(AccessControlEnforcerTest, CheckCarrierPrivilege_NoRule_Test)
{
    try {
        accessControlEnforcer_->Initialize();
        std::shared_ptr<osal::BundleInfo> bInfo = std::make_shared<osal::BundleInfo>();
        bInfo->mBundleName_ = "CheckCarrierPrivilege_Test";
        bool checkRefreshTag = false;
        EXPECT_FALSE(accessControlEnforcer_->CheckCarrierPrivilege(bInfo, checkRefreshTag));
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : ACCESS_CONTROL_ENFORCER_TEST_025
 * @tc.name      : CheckCarrierPrivilege_NoBundleName_Test
 * @tc.desc      : Check carrier privilege with no bundle name
 */
TEST_F(AccessControlEnforcerTest, CheckCarrierPrivilege_NoBundleName_Test)
{
    try {
        std::shared_ptr<osal::BundleInfo> bInfo = std::make_shared<osal::BundleInfo>();
        bInfo->mBundleName_ = "";
        bool checkRefreshTag = false;
        EXPECT_FALSE(accessControlEnforcer_->CheckCarrierPrivilege(bInfo, checkRefreshTag));
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(true);
    }
}
}  // namespace OHOS::se::test