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
#include "channel_access_rule.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>
#include <string>
#include <vector>

using namespace OHOS::se::security;

namespace OHOS::se::test {
class ChannelAccessRuleTest : public ::testing::Test {
public:
    virtual void SetUp() override
    {
        std::cout << " ChannelAccessTest set up." << std::endl;
    }

    virtual void TearDown() override
    {
        std::cout << " ChannelAccessTest tear down." << std::endl;
    }

public:
};

/**
 * @tc.number    : CHANNEL_ACCESS_RULE_001
 * @tc.name      : BundleName_Test
 * @tc.desc      : Get bundle name
 */
TEST_F(ChannelAccessRuleTest, BundleName_Test)
{
    try {
        ChannelAccessRule channelAccessRule = ChannelAccessRule();
        std::string bundleName = "BundleName_Test";
        EXPECT_NE(bundleName, channelAccessRule.GetBundleName());
        channelAccessRule.SetBundleName(bundleName);
        EXPECT_EQ(bundleName, channelAccessRule.GetBundleName());
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : CHANNEL_ACCESS_RULE_002
 * @tc.name      : CallingPid_Test
 * @tc.desc      : Get calling pid
 */
TEST_F(ChannelAccessRuleTest, CallingPid_Test)
{
    try {
        ChannelAccessRule channelAccessRule = ChannelAccessRule();
        int pid = 65501;
        EXPECT_NE(pid, channelAccessRule.GetCallingPid());
        channelAccessRule.SetCallingPid(pid);
        EXPECT_EQ(pid, channelAccessRule.GetCallingPid());
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : CHANNEL_ACCESS_RULE_003
 * @tc.name      : PrivilegeAccess_Test
 * @tc.desc      : Privilege access
 */
TEST_F(ChannelAccessRuleTest, PrivilegeAccess_Test)
{
    try {
        ChannelAccessRule channelAccessRule = ChannelAccessRule();
        EXPECT_EQ(ChannelAccessRule::ACCESSRULE::UNKNOWN, channelAccessRule.GetPrivilegeAccessRule());
        channelAccessRule.SetPrivilegeAccessRule(ChannelAccessRule::ACCESSRULE::ALWAYS);
        EXPECT_EQ(ChannelAccessRule::ACCESSRULE::ALWAYS, channelAccessRule.GetPrivilegeAccessRule());
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : CHANNEL_ACCESS_RULE_004
 * @tc.name      : GetPrivilegeAccess_Test
 * @tc.desc      : Get privilege access
 */
TEST_F(ChannelAccessRuleTest, GetPrivilegeAccess_Test)
{
    try {
        std::string bundleName = "GetPrivilegeAccess_Test";
        int pid = 65502;
        std::shared_ptr<ChannelAccessRule> channelAccessRule = ChannelAccessRule::GetPrivilegeAccessRule(bundleName, pid);
        EXPECT_EQ(bundleName, channelAccessRule->GetBundleName());
        EXPECT_EQ(pid, channelAccessRule->GetCallingPid());
        EXPECT_EQ(ChannelAccessRule::ACCESSRULE::ALWAYS, channelAccessRule->GetAccessRule());
        EXPECT_EQ(ChannelAccessRule::ACCESSRULE::ALWAYS, channelAccessRule->GetApduAccessRule());
        EXPECT_EQ(ChannelAccessRule::ACCESSRULE::ALWAYS, channelAccessRule->GetNFCEventAccessRule());
        EXPECT_EQ(ChannelAccessRule::ACCESSRULE::ALWAYS, channelAccessRule->GetPrivilegeAccessRule());
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : CHANNEL_ACCESS_RULE_005
 * @tc.name      : GetCarrierPrivilegeAccess_Test
 * @tc.desc      : Get carrier privilege access
 */
TEST_F(ChannelAccessRuleTest, GetCarrierPrivilegeAccess_Test)
{
    try {
        std::string bundleName = "GetCarrierPrivilegeAccess_Test";
        int pid = 65503;
        std::shared_ptr<ChannelAccessRule> channelAccessRule = ChannelAccessRule::GetCarrierPrivilegeAccessRule(bundleName, pid);
        EXPECT_EQ(bundleName, channelAccessRule->GetBundleName());
        EXPECT_EQ(pid, channelAccessRule->GetCallingPid());
        EXPECT_EQ(ChannelAccessRule::ACCESSRULE::ALWAYS, channelAccessRule->GetAccessRule());
        EXPECT_EQ(ChannelAccessRule::ACCESSRULE::ALWAYS, channelAccessRule->GetApduAccessRule());
        EXPECT_EQ(ChannelAccessRule::ACCESSRULE::ALWAYS, channelAccessRule->GetPrivilegeAccessRule());
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : CHANNEL_ACCESS_RULE_006
 * @tc.name      : SetCarrierPrivilegeAccess_Test
 * @tc.desc      : Set carrier privilege access
 */
TEST_F(ChannelAccessRuleTest, SetCarrierPrivilegeAccess_Test)
{
    try {
        ChannelAccessRule channelAccessRule = ChannelAccessRule();
        std::string bundleName = "SetCarrierPrivilegeAccess_Test";
        int pid = 65504;
        channelAccessRule.SetCarrierPrivilegeAccessRule(bundleName, pid);
        EXPECT_EQ(bundleName, channelAccessRule.GetBundleName());
        EXPECT_EQ(pid, channelAccessRule.GetCallingPid());
        EXPECT_EQ(ChannelAccessRule::ACCESSRULE::ALWAYS, channelAccessRule.GetAccessRule());
        EXPECT_EQ(ChannelAccessRule::ACCESSRULE::ALWAYS, channelAccessRule.GetApduAccessRule());
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}
}  // namespace OHOS::se::test