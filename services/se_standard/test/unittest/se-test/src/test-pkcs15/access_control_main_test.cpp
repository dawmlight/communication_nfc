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
#include "pkcs15/access_control_main.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>
#include <string>
#include <vector>

#include "../access_rule_data_common.h"
#include "access_rule_cache.h"
#include "access_rule_files_channel.h"
#include "se_channel.h"

using namespace OHOS::se::security;

namespace OHOS::se::test {
class AccessControlMainTest : public ::testing::Test {
public:
    virtual void SetUp() override
    {
        std::cout << " AccessControlMainTest set up." << std::endl;
        path_ = {0x43, 0x00};
        accessRuleCache_ = std::make_shared<AccessRuleCache>();
        arfChannel_ = std::make_shared<AccessRuleFilesChannel>();
        accessControlMain_ = std::make_shared<AccessControlMain>(arfChannel_, accessRuleCache_, path_);
    }

    virtual void TearDown() override
    {
        std::cout << " AccessControlMainTest tear down." << std::endl;
    }

public:
    std::shared_ptr<AccessRuleFilesChannel> arfChannel_;
    std::shared_ptr<AccessRuleCache> accessRuleCache_;
    std::string path_;
    std::shared_ptr<AccessControlMain> accessControlMain_;
};

/**
 * @tc.number    : ACCESS_CONTROL_MAIN_TEST_001
 * @tc.name      : Analyze_Data_Test
 * @tc.desc      : Analyze data
 */
TEST_F(AccessControlMainTest, Analyze_Data_Test)
{
    try {
        EXPECT_EQ(ACCESS_CONTROL_RULES_PATH, accessControlMain_->AnalyzeData(ACCESS_CONTROL_MAIN_DATA));
        EXPECT_TRUE((accessControlMain_->AnalyzeData(ACCESS_CONTROL_MAIN_DATA)).empty());
        EXPECT_EQ(ACCESS_CONTROL_RULES_PATH, accessControlMain_->AnalyzeData(ACCESS_CONTROL_MAIN_DATA2));
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : ACCESS_CONTROL_MAIN_TEST_002
 * @tc.name      : Analyze_Data_Refresh_Tag_Illegal_Length_Test
 * @tc.desc      : Analyze data, refresh tag has illegal length
 */
TEST_F(AccessControlMainTest, Analyze_Data_Refresh_Tag_Illegal_Length_Test)
{
    try {
        EXPECT_EQ(ACCESS_CONTROL_RULES_PATH,
                  accessControlMain_->AnalyzeData(ACCESS_CONTROL_MAIN_DATA_REFRESH_TAG_LENGTH_ERROR));
        EXPECT_TRUE(false);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(true);
    }
}

/**
 * @tc.number    : ACCESS_CONTROL_MAIN_TEST_003
 * @tc.name      : Analyze_Data_Cache_Expired_Test
 * @tc.desc      : Analyze data, access rule cache has expired
 */
TEST_F(AccessControlMainTest, Analyze_Data_Cache_Expired_Test)
{
    try {
        accessRuleCache_ = std::shared_ptr<AccessRuleCache>();
        accessControlMain_ = std::make_shared<AccessControlMain>(arfChannel_, accessRuleCache_, path_);
        EXPECT_EQ(ACCESS_CONTROL_RULES_PATH, accessControlMain_->AnalyzeData(ACCESS_CONTROL_MAIN_DATA));
        EXPECT_TRUE(false);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(true);
    }
}

/**
 * @tc.number    : ACCESS_CONTROL_MAIN_TEST_004
 * @tc.name      : Read_Data_From_Path_Test
 * @tc.desc      : Read data from path
 */
TEST_F(AccessControlMainTest, Read_Data_From_Path_Test)
{
    try {
        accessControlMain_->ReadDataFromPath();
        EXPECT_TRUE(false);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        // transmit error
        EXPECT_TRUE(true);
    }
}
}  // namespace OHOS::se::test