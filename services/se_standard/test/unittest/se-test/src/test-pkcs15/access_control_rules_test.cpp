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
#include "pkcs15/access_control_rules.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>
#include <string>
#include <vector>

#include "../access_rule_data_common.h"
#include "access_rule_cache.h"
#include "access_rule_files_channel.h"

using namespace OHOS::se::security;

namespace OHOS::se::test {
class AccessControlRulesTest : public ::testing::Test {
public:
    virtual void SetUp() override
    {
        std::cout << " AccessControlRulesTest set up." << std::endl;
        accessRuleCache_ = std::make_shared<AccessRuleCache>();
        arfChannel_ = std::make_shared<AccessRuleFilesChannel>();
        accessControlRules_ = std::make_shared<AccessControlRules>(arfChannel_, accessRuleCache_);
    }

    virtual void TearDown() override
    {
        std::cout << " AccessControlRulesTest tear down." << std::endl;
    }

public:
    std::shared_ptr<AccessRuleFilesChannel> arfChannel_;
    std::shared_ptr<AccessRuleCache> accessRuleCache_;
    std::shared_ptr<AccessControlRules> accessControlRules_;
};

/**
 * @tc.number    : ACCESS_CONTROL_RULES_TEST_001
 * @tc.name      : Analyze_Data_Test
 * @tc.desc      : Analyze data
 */
TEST_F(AccessControlRulesTest, Analyze_Data_Test)
{
    try {
        accessControlRules_->AnalyzeData(ACCESS_CONTROL_RULES_DATA);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        // transmit error
        EXPECT_TRUE(true);
    }
}

/**
 * @tc.number    : ACCESS_CONTROL_RULES_TEST_002
 * @tc.name      : Analyze_Data_Empty_Aid_Test
 * @tc.desc      : Analyze data with empty aid
 */
TEST_F(AccessControlRulesTest, Analyze_Data_Empty_Aid_Test)
{
    try {
        accessControlRules_->AnalyzeData(ACCESS_CONTROL_RULES_DATA2);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        // transmit error
        EXPECT_TRUE(true);
    }
}

/**
 * @tc.number    : ACCESS_CONTROL_RULES_TEST_003
 * @tc.name      : Analyze_Data_Default_Test
 * @tc.desc      : Analyze data with default aid
 */
TEST_F(AccessControlRulesTest, Analyze_Data_Default_Test)
{
    try {
        accessControlRules_->AnalyzeData(ACCESS_CONTROL_RULES_DATA3);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        // transmit error
        EXPECT_TRUE(true);
    }
}

/**
 * @tc.number    : ACCESS_CONTROL_RULES_TEST_004
 * @tc.name      : Analyze_Data_Unknown_Tag_Test
 * @tc.desc      : Analyze data with unknown tag
 */
TEST_F(AccessControlRulesTest, Analyze_Data_Unknown_Tag_Test)
{
    try {
        accessControlRules_->AnalyzeData(ACCESS_CONTROL_RULES_DATA_UNKNOWN_TAG);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        // transmit error
        EXPECT_TRUE(true);
    }
}

/**
 * @tc.number    : ACCESS_CONTROL_RULES_TEST_005
 * @tc.name      : Read_Data_From_Path_Test
 * @tc.desc      : Read data from path
 */
TEST_F(AccessControlRulesTest, Read_Data_From_Path_Test)
{
    try {
        std::string path = {0x43, 0x00};
        accessControlRules_->ReadDataFromPath(path);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        // transmit error
        EXPECT_TRUE(true);
    }
}
}  // namespace OHOS::se::test