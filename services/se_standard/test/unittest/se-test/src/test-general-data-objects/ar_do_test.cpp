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
#include "general-data-objects/ar_do.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>
#include <string>
#include <vector>

#include "../access_rule_data_common.h"
#include "general-data-objects/apdu_ar_do.h"
#include "general-data-objects/nfc_ar_do.h"

using namespace OHOS::se::security;

namespace OHOS::se::test {
class ArDoTest : public ::testing::Test {
public:
    virtual void SetUp() override
    {
        std::cout << " ArDoTest set up." << std::endl;
    }

    virtual void TearDown() override
    {
        std::cout << " ArDoTest tear down." << std::endl;
    }

public:
};

/**
 * @tc.number    : AR_DO_TEST_001
 * @tc.name      : Ar_Test
 * @tc.desc      : Generate ar-do
 */
TEST_F(ArDoTest, Ar_Test)
{
    try {
        std::string data = AR_DO_DATA_11;
        std::shared_ptr<BerTlv> bt = BerTlv::StrToBerTlv(data);
        std::shared_ptr<ArDo> arDo = ArDo::BerTlvToArDo(bt);
        std::shared_ptr<ApduArDo> aad = arDo->GetApduArDo();
        std::shared_ptr<NfcArDo> nad = arDo->GetNfcArDo();
        EXPECT_TRUE(aad->IsApduAllowed());
        EXPECT_TRUE(aad->GetApduFilters().empty());
        EXPECT_TRUE(nad->IsNfcAllowed());
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : AR_DO_TEST_002
 * @tc.name      : Illegal_Tag_Test
 * @tc.desc      : Generate ar-do with illegal tag
 */
TEST_F(ArDoTest, Illegal_Tag_Test)
{
    try {
        std::string data = AR_DO_DATA_11;
        data[0] = 0x00;
        std::shared_ptr<BerTlv> bt = BerTlv::StrToBerTlv(data);
        std::shared_ptr<ArDo> arDo = ArDo::BerTlvToArDo(bt);
        EXPECT_TRUE(false);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(true);
    }
}

/**
 * @tc.number    : AR_DO_TEST_003
 * @tc.name      : Illegal_Length_Test
 * @tc.desc      : Generate ar-do with illegal length
 */
TEST_F(ArDoTest, Illegal_Length_Test)
{
    try {
        std::string data = AR_DO_DATA_11;
        data = data.substr(0, data.length() - 1);
        std::shared_ptr<BerTlv> bt = BerTlv::StrToBerTlv(data);
        std::shared_ptr<ArDo> arDo = ArDo::BerTlvToArDo(bt);
        EXPECT_TRUE(false);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(true);
    }
}

/**
 * @tc.number    : AR_DO_TEST_004
 * @tc.name      : Empty_Value_Test
 * @tc.desc      : Generate ar-do with empty value
 */
TEST_F(ArDoTest, Empty_Value_Test)
{
    try {
        std::string data = {(char)0xE3, 0x02, (char)0xDD, 0x00};
        std::shared_ptr<BerTlv> bt = BerTlv::StrToBerTlv(data);
        std::shared_ptr<ArDo> arDo = ArDo::BerTlvToArDo(bt);
        EXPECT_TRUE(false);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(true);
    }
}
}  // namespace OHOS::se::test