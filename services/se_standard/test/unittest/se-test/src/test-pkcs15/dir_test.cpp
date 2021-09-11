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
#include "pkcs15/dir.h"

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
class DirTest : public ::testing::Test {
public:
    virtual void SetUp() override
    {
        std::cout << " DirTest set up." << std::endl;
        arfChannel_ = std::make_shared<AccessRuleFilesChannel>();
        dir_ = std::make_shared<DIR>(arfChannel_);
    }

    virtual void TearDown() override
    {
        std::cout << " DirTest tear down." << std::endl;
    }

public:
    std::shared_ptr<AccessRuleFilesChannel> arfChannel_;
    std::shared_ptr<DIR> dir_;
};

/**
 * @tc.number    : DIR_TEST_001
 * @tc.name      : Analyze_Data_Test
 * @tc.desc      : Analyze data
 */
TEST_F(DirTest, Analyze_Data_Test)
{
    try {
        EXPECT_EQ(ODF_PATH, dir_->AnalyzeData(DIR_DATA, DIR_AID));
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : DIR_TEST_002
 * @tc.name      : Analyze_Data_Another_Aid_Test
 * @tc.desc      : Analyze data with another aid
 */
TEST_F(DirTest, Analyze_Data_Another_Aid_Test)
{
    try {
        EXPECT_EQ("", dir_->AnalyzeData(DIR_DATA, ""));
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : DIR_TEST_003
 * @tc.name      : Analyze_Data_Illegal_Tag_Test
 * @tc.desc      : Analyze data with illegal tag
 */
TEST_F(DirTest, Analyze_Data_Illegal_Tag_Test)
{
    try {
        EXPECT_EQ(ODF_PATH, dir_->AnalyzeData(DIR_DATA_WRONG_TAG, DIR_AID));
        EXPECT_TRUE(false);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(true);
    }
}

/**
 * @tc.number    : DIR_TEST_004
 * @tc.name      : Read_Data_From_Aid_Test
 * @tc.desc      : Read data from aid
 */
TEST_F(DirTest, Read_Data_From_Aid_Test)
{
    try {
        std::string aid = {};
        dir_->ReadDataFromAid(aid);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        // transmit error
        EXPECT_TRUE(true);
    }
}
}  // namespace OHOS::se::test