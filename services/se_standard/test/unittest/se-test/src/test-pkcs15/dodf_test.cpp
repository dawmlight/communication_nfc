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
#include "pkcs15/dodf.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>
#include <string>
#include <vector>

#include "../access_rule_data_common.h"
#include "access_rule_files_channel.h"
#include "se_channel.h"

using namespace OHOS::se::security;
using namespace OHOS::se;

namespace OHOS::se::test {
class DodfTest : public ::testing::Test {
public:
    virtual void SetUp() override
    {
        std::cout << " DodfTest set up." << std::endl;
        arfChannel_ = std::make_shared<AccessRuleFilesChannel>();
        dodf_ = std::make_shared<DODF>(arfChannel_);
    }

    virtual void TearDown() override
    {
        std::cout << " DodfTest tear down." << std::endl;
    }

public:
    std::shared_ptr<AccessRuleFilesChannel> arfChannel_;
    std::shared_ptr<DODF> dodf_;
};

/**
 * @tc.number    : DODF_TEST_001
 * @tc.name      : Analyze_Data_Test
 * @tc.desc      : Analyze data
 */
TEST_F(DodfTest, Analyze_Data_Test)
{
    try {
        EXPECT_EQ(ACCESS_CONTROL_MAIN_PATH, dodf_->AnalyzeData(DODF_DATA));
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : DODF_TEST_002
 * @tc.name      : Analyze_Data_Diff_Oid_Test
 * @tc.desc      : Analyze data with diff oid
 */
TEST_F(DodfTest, Analyze_Data_Diff_Oid_Test)
{
    try {
        EXPECT_EQ("", dodf_->AnalyzeData(DODF_DATA_DIFF_OID));
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : DODF_TEST_003
 * @tc.name      : Analyze_Data_No_Oid_Tag_Test
 * @tc.desc      : Analyze data without oid tag
 */
TEST_F(DodfTest, Analyze_Data_No_Oid_Tag_Test)
{
    try {
        EXPECT_EQ(ACCESS_CONTROL_MAIN_PATH, dodf_->AnalyzeData(DODF_DATA_NO_OID_TAG));
        EXPECT_TRUE(false);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(true);
    }
}

/**
 * @tc.number    : DODF_TEST_004
 * @tc.name      : Read_Data_From_Path_Test
 * @tc.desc      : Read data from path
 */
TEST_F(DodfTest, Read_Data_From_Path_Test)
{
    try {
        std::string path = {};
        dodf_->ReadDataFromPath(path);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        // transmit error
        EXPECT_TRUE(true);
    }
}
}  // namespace OHOS::se::test