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
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>
#include <string>
#include <vector>

#include "access_rule_application_controller.h"
#include "access_rule_cache.h"
#include "access_rule_data_common.h"
#include "base_test.h"
#include "channel_access_rule.h"
#include "context.h"
#include "general-data-objects/ref_ar_do.h"
#include "isecure_element_mock.h"
#include "se_channel.h"
#include "terminal.h"
#undef private

using namespace OHOS::se::security;
using namespace OHOS::se;
using namespace OHOS::hardware::se::v1_0;
using namespace osal;
using namespace testing;

namespace OHOS::se::test {
class AraTest : public ::testing::Test {
public:
    virtual void SetUp() override
    {
        std::cout << " AraTest set up." << std::endl;
        accessRuleCache_ = std::make_shared<AccessRuleCache>();
        context_ = std::make_shared<Context>();
        secureElementMock_ = BaseTest::LoadSecureElement(true);
        EXPECT_CALL(*secureElementMock_, Initialize(_)).Times(AnyNumber());
        terminal_ = std::make_shared<Terminal>("AraTest", context_);
        terminal_->Initialize(secureElementMock_);
        accessRuleApplicationController_ =
            std::make_shared<AccessRuleApplicationController>(accessRuleCache_, terminal_);
    }

    virtual void TearDown() override
    {
        std::cout << " AraTest tear down." << std::endl;
    }

public:
    std::shared_ptr<AccessRuleCache> accessRuleCache_;
    std::shared_ptr<Context> context_;
    sptr<ISecureElementMock> secureElementMock_;
    std::shared_ptr<Terminal> terminal_;
    std::shared_ptr<AccessRuleApplicationController> accessRuleApplicationController_;
};

/**
 * @tc.number    : ARA_TEST_001
 * @tc.name      : Initialize_OpenLogicalChannel_Fail_Test
 * @tc.desc      : Intialize when open logical channel fail
 */
TEST_F(AraTest, Initialize_OpenLogicalChannel_Fail_Test)
{
    try {
        accessRuleApplicationController_->Initialize();
        EXPECT_TRUE(false);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        // OpenLogicalChannel fail
        EXPECT_TRUE(true);
    }
}

/**
 * @tc.number    : ARA_TEST_002
 * @tc.name      : Initialize_TerminalExpired_Test
 * @tc.desc      : Intialize when terminal expired
 */
TEST_F(AraTest, Initialize_TerminalExpired_Test)
{
    try {
        terminal_ = std::shared_ptr<Terminal>();
        accessRuleApplicationController_->Initialize();
        EXPECT_TRUE(false);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(true);
    }
}

/**
 * @tc.number    : ARA_TEST_003
 * @tc.name      : Initialize_AccessRuleCacheExpired_Test
 * @tc.desc      : Intialize when access rule cache expired
 */
TEST_F(AraTest, Initialize_AccessRuleCacheExpired_Test)
{
    try {
        accessRuleCache_ = std::shared_ptr<AccessRuleCache>();
        accessRuleApplicationController_->Initialize();
        EXPECT_TRUE(false);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(true);
    }
}

/**
 * @tc.number    : ARA_TEST_004
 * @tc.name      : GetAccessControlAid_Test
 * @tc.desc      : Get access control aid
 */
TEST_F(AraTest, GetAccessControlAid_Test)
{
    try {
        EXPECT_EQ(ARA_M_AID, accessRuleApplicationController_->GetAccessControlAid());
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : ARA_TEST_005
 * @tc.name      : GetRefreshTag_Test
 * @tc.desc      : Get refresh tag
 */
TEST_F(AraTest, GetRefreshTag_Test)
{
    try {
        EXPECT_CALL(*secureElementMock_, Transmit(_)).Times(AnyNumber()).WillOnce(Return(RESPONSE_REFERESH_TAG_D0));

        std::shared_ptr<OHOS::se::SeChannel> channel = std::make_shared<OHOS::se::SeChannel>(terminal_, 1, "", "");
        std::shared_ptr<ChannelAccessRule> channelAccess = std::make_shared<ChannelAccessRule>();
        channelAccess->SetAccessRule(ChannelAccessRule::ACCESSRULE::ALWAYS, "");
        channelAccess->SetApduAccessRule(ChannelAccessRule::ACCESSRULE::ALWAYS);
        channelAccess->SetNFCEventAccessRule(ChannelAccessRule::ACCESSRULE::ALWAYS);
        channelAccess->SetPrivilegeAccessRule(ChannelAccessRule::ACCESSRULE::ALWAYS);
        channel->SetChannelAccess(channelAccess);

        EXPECT_EQ(REFERESH_TAG, accessRuleApplicationController_->GetRefreshTag(channel));
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : ARA_TEST_006
 * @tc.name      : GetRefreshTag_DiffTag_Test
 * @tc.desc      : Get refresh tag with different tag
 */
TEST_F(AraTest, GetRefreshTag_DiffTag_Test)
{
    try {
        EXPECT_CALL(*secureElementMock_, Transmit(_)).Times(AnyNumber()).WillOnce(Return(NOT_RESPONSE_REFERESH_TAG_D0));

        std::shared_ptr<OHOS::se::SeChannel> channel = std::make_shared<OHOS::se::SeChannel>(terminal_, 1, "", "");
        std::shared_ptr<ChannelAccessRule> channelAccess = std::make_shared<ChannelAccessRule>();
        channelAccess->SetAccessRule(ChannelAccessRule::ACCESSRULE::ALWAYS, "");
        channelAccess->SetApduAccessRule(ChannelAccessRule::ACCESSRULE::ALWAYS);
        channelAccess->SetNFCEventAccessRule(ChannelAccessRule::ACCESSRULE::ALWAYS);
        channelAccess->SetPrivilegeAccessRule(ChannelAccessRule::ACCESSRULE::ALWAYS);
        channel->SetChannelAccess(channelAccess);

        EXPECT_EQ(REFERESH_TAG, accessRuleApplicationController_->GetRefreshTag(channel));
        EXPECT_TRUE(false);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(true);
    }
}

/**
 * @tc.number    : ARA_TEST_007
 * @tc.name      : GetRefreshTag_ErrorResponse_Test
 * @tc.desc      : Get refresh tag with error response
 */
TEST_F(AraTest, GetRefreshTag_ErrorResponse_Test)
{
    try {
        EXPECT_CALL(*secureElementMock_, Transmit(_)).Times(AnyNumber()).WillOnce(Return(APDU_ERROR_RESPONSE));

        std::shared_ptr<OHOS::se::SeChannel> channel = std::make_shared<OHOS::se::SeChannel>(terminal_, 1, "", "");
        std::shared_ptr<ChannelAccessRule> channelAccess = std::make_shared<ChannelAccessRule>();
        channelAccess->SetAccessRule(ChannelAccessRule::ACCESSRULE::ALWAYS, "");
        channelAccess->SetApduAccessRule(ChannelAccessRule::ACCESSRULE::ALWAYS);
        channelAccess->SetNFCEventAccessRule(ChannelAccessRule::ACCESSRULE::ALWAYS);
        channelAccess->SetPrivilegeAccessRule(ChannelAccessRule::ACCESSRULE::ALWAYS);
        channel->SetChannelAccess(channelAccess);

        EXPECT_EQ(REFERESH_TAG, accessRuleApplicationController_->GetRefreshTag(channel));
        EXPECT_TRUE(false);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(true);
    }
}

/**
 * @tc.number    : ARA_TEST_008
 * @tc.name      : GetAllAccessRules_All_Test
 * @tc.desc      : Get all access rules with GET DATA [All]
 */
TEST_F(AraTest, GetAllAccessRules_All_Test)
{
    try {
        EXPECT_CALL(*secureElementMock_, Transmit(_)).Times(AnyNumber()).WillOnce(Return(RESPONSE_ALL));

        std::shared_ptr<OHOS::se::SeChannel> channel = std::make_shared<OHOS::se::SeChannel>(terminal_, 1, "", "");
        std::shared_ptr<ChannelAccessRule> channelAccess = std::make_shared<ChannelAccessRule>();
        channelAccess->SetAccessRule(ChannelAccessRule::ACCESSRULE::ALWAYS, "");
        channelAccess->SetApduAccessRule(ChannelAccessRule::ACCESSRULE::ALWAYS);
        channelAccess->SetNFCEventAccessRule(ChannelAccessRule::ACCESSRULE::ALWAYS);
        channelAccess->SetPrivilegeAccessRule(ChannelAccessRule::ACCESSRULE::ALWAYS);
        channel->SetChannelAccess(channelAccess);

        std::vector<RefArDo> radList = accessRuleApplicationController_->GetAllAccessRules(channel);
        EXPECT_EQ(1U, radList.size());
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : ARA_TEST_009
 * @tc.name      : GetAllAccessRules_Next_Test
 * @tc.desc      : Get all access rules with GET DATA [Next]
 */
TEST_F(AraTest, GetAllAccessRules_Next_Test)
{
    try {
        EXPECT_CALL(*secureElementMock_, Transmit(_))
            .Times(AnyNumber())
            .WillOnce(Return(RESPONSE_ALL_FOR_NEXT))
            .WillOnce(Return(RESPONSE_NEXT));

        std::shared_ptr<OHOS::se::SeChannel> channel = std::make_shared<OHOS::se::SeChannel>(terminal_, 1, "", "");
        std::shared_ptr<ChannelAccessRule> channelAccess = std::make_shared<ChannelAccessRule>();
        channelAccess->SetAccessRule(ChannelAccessRule::ACCESSRULE::ALWAYS, "");
        channelAccess->SetApduAccessRule(ChannelAccessRule::ACCESSRULE::ALWAYS);
        channelAccess->SetNFCEventAccessRule(ChannelAccessRule::ACCESSRULE::ALWAYS);
        channelAccess->SetPrivilegeAccessRule(ChannelAccessRule::ACCESSRULE::ALWAYS);
        channel->SetChannelAccess(channelAccess);
        
        std::vector<RefArDo> radList = accessRuleApplicationController_->GetAllAccessRules(channel);
        EXPECT_EQ(6U, radList.size());
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : ARA_TEST_010
 * @tc.name      : GetAllAccessRules_AllErrorResponse_Test
 * @tc.desc      : Get all access rules with error response when GET DATA [All]
 */
TEST_F(AraTest, GetAllAccessRules_AllErrorResponse_Test)
{
    try {
        EXPECT_CALL(*secureElementMock_, Transmit(_)).Times(AnyNumber()).WillOnce(Return(APDU_ERROR_RESPONSE));

        std::shared_ptr<OHOS::se::SeChannel> channel = std::make_shared<OHOS::se::SeChannel>(terminal_, 1, "", "");
        std::shared_ptr<ChannelAccessRule> channelAccess = std::make_shared<ChannelAccessRule>();
        channelAccess->SetAccessRule(ChannelAccessRule::ACCESSRULE::ALWAYS, "");
        channelAccess->SetApduAccessRule(ChannelAccessRule::ACCESSRULE::ALWAYS);
        channelAccess->SetNFCEventAccessRule(ChannelAccessRule::ACCESSRULE::ALWAYS);
        channelAccess->SetPrivilegeAccessRule(ChannelAccessRule::ACCESSRULE::ALWAYS);
        channel->SetChannelAccess(channelAccess);

        std::vector<RefArDo> radList = accessRuleApplicationController_->GetAllAccessRules(channel);
        EXPECT_TRUE(false);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(true);
    }
}

/**
 * @tc.number    : ARA_TEST_011
 * @tc.name      : GetRefreshTag_NextErrorResponse_Test
 * @tc.desc      : Get all access rules with error response when GET DATA [Next]
 */
TEST_F(AraTest, GetRefreshTag_NextErrorResponse_Test)
{
    try {
        EXPECT_CALL(*secureElementMock_, Transmit(_))
            .Times(AnyNumber())
            .WillOnce(Return(RESPONSE_ALL_FOR_NEXT))
            .WillOnce(Return(APDU_ERROR_RESPONSE));

        std::shared_ptr<OHOS::se::SeChannel> channel = std::make_shared<OHOS::se::SeChannel>(terminal_, 1, "", "");
        std::shared_ptr<ChannelAccessRule> channelAccess = std::make_shared<ChannelAccessRule>();
        channelAccess->SetAccessRule(ChannelAccessRule::ACCESSRULE::ALWAYS, "");
        channelAccess->SetApduAccessRule(ChannelAccessRule::ACCESSRULE::ALWAYS);
        channelAccess->SetNFCEventAccessRule(ChannelAccessRule::ACCESSRULE::ALWAYS);
        channelAccess->SetPrivilegeAccessRule(ChannelAccessRule::ACCESSRULE::ALWAYS);
        channel->SetChannelAccess(channelAccess);

        std::vector<RefArDo> radList = accessRuleApplicationController_->GetAllAccessRules(channel);
        EXPECT_TRUE(false);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(true);
    }
}
}  // namespace OHOS::se::test