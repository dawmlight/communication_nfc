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
#include "pkcs15/odf.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>
#include <string>
#include <vector>

#include "../access_rule_data_common.h"
#include "access_rule_files_channel.h"

using namespace OHOS::se::security;
using namespace OHOS::se;

namespace OHOS::se::test {
class OdfTest : public ::testing::Test {
public:
    virtual void SetUp() override
    {
        std::cout << " OdfTest set up." << std::endl;
        arfChannel_ = std::make_shared<AccessRuleFilesChannel>();
        odf_ = std::make_shared<ODF>(arfChannel_);
    }

    virtual void TearDown() override
    {
        std::cout << " OdfTest tear down." << std::endl;
    }

public:
    std::shared_ptr<AccessRuleFilesChannel> arfChannel_;
    std::shared_ptr<ODF> odf_;
};

/**
 * @tc.number    : ODF_TEST_001
 * @tc.name      : Analyze_Data_Test
 * @tc.desc      : Analyze data
 */
TEST_F(OdfTest, Analyze_Data_Test)
{
    try {
        EXPECT_EQ(DODF_PATH, odf_->AnalyzeData(ODF_DATA));
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : ODF_TEST_002
 * @tc.name      : Analyze_Data_Unknown_Test
 * @tc.desc      : Analyze data has no dodf path
 */
TEST_F(OdfTest, Analyze_Data_Unknown_Test)
{
    try {
        EXPECT_EQ("", odf_->AnalyzeData(ODF_DATA_UNKNOWN));
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : ODF_TEST_003
 * @tc.name      : Read_Data_From_Path_Test
 * @tc.desc      : Read data from path
 */
TEST_F(OdfTest, Read_Data_From_Path_Test)
{
    try {
        std::string path = {};
        odf_->ReadDataFromPath(path);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        // transmit error
        EXPECT_TRUE(true);
    }
}
}  // namespace OHOS::se::test