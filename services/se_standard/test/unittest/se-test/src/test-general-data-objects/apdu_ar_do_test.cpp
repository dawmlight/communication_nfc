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
#include "general-data-objects/apdu_ar_do.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>
#include <string>
#include <vector>

#include "../access_rule_data_common.h"

using namespace OHOS::se::security;

namespace OHOS::se::test {
class ApduArDoTest : public ::testing::Test {
public:
    virtual void SetUp() override
    {
        std::cout << " ApduArDoTest set up." << std::endl;
    }

    virtual void TearDown() override
    {
        std::cout << " ApduArDoTest tear down." << std::endl;
    }

public:
};

/**
 * @tc.number    : APDU_AR_DO_TEST_001
 * @tc.name      : Apdu_Test
 * @tc.desc      : Generate apdu-ar-do
 */
TEST_F(ApduArDoTest, Apdu_Test)
{
    try {
        std::string data = {(char)0xD0, 0x01, 0x01};
        std::shared_ptr<BerTlv> bt = BerTlv::StrToBerTlv(data);
        std::shared_ptr<ApduArDo> apduArDo = ApduArDo::BerTlvToApduArDo(bt);
        EXPECT_TRUE(apduArDo->IsApduAllowed());
        EXPECT_TRUE(apduArDo->GetApduFilters().empty());
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : APDU_AR_DO_TEST_002
 * @tc.name      : Data_Not_Enough_Test
 * @tc.desc      : Generate apdu-ar-do with not enough value
 */
TEST_F(ApduArDoTest, Data_Not_Enough_Test)
{
    try {
        std::string data = {(char)0xD0, 0x02, 0x01};
        std::shared_ptr<BerTlv> bt = BerTlv::StrToBerTlv(data);
        std::shared_ptr<ApduArDo> apduArDo = ApduArDo::BerTlvToApduArDo(bt);
        EXPECT_TRUE(false);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(true);
    }
}

/**
 * @tc.number    : APDU_AR_DO_TEST_003
 * @tc.name      : Error_Length_Test
 * @tc.desc      : Generate apdu-ar-do with illegal length
 */
TEST_F(ApduArDoTest, Illegal_Length_Test)
{
    try {
        std::string data = {(char)0xD0, 0x02, 0x00, 0x00};
        std::shared_ptr<BerTlv> bt = BerTlv::StrToBerTlv(data);
        std::shared_ptr<ApduArDo> apduArDo = ApduArDo::BerTlvToApduArDo(bt);
        EXPECT_TRUE(false);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(true);
    }
}

/**
 * @tc.number    : APDU_AR_DO_TEST_004
 * @tc.name      : Error_Value_Test
 * @tc.desc      : Generate apdu-ar-do with illegal value
 */
TEST_F(ApduArDoTest, Illegal_Value_Test)
{
    try {
        std::string data = {(char)0xD0, 0x01, 0x02};
        std::shared_ptr<BerTlv> bt = BerTlv::StrToBerTlv(data);
        std::shared_ptr<ApduArDo> apduArDo = ApduArDo::BerTlvToApduArDo(bt);
        EXPECT_TRUE(false);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(true);
    }
}
}  // namespace OHOS::se::test