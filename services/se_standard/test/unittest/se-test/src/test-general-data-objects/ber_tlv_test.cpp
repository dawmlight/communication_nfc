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
#include "general-data-objects/ber_tlv.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>
#include <string>
#include <vector>

#include "../access_rule_data_common.h"

using namespace OHOS::se::security;

namespace OHOS::se::test {
class BerTlvTest : public ::testing::Test {
public:
    virtual void SetUp() override
    {
        std::cout << " BerTlvTest set up." << std::endl;
    }

    virtual void TearDown() override
    {
        std::cout << " BerTlvTest tear down." << std::endl;
    }

public:
};

/**
 * @tc.number    : BER_TLV_TEST_001
 * @tc.name      : BerTlv_Test
 * @tc.desc      : Generate ber-tlv
 */
TEST_F(BerTlvTest, BerTlv_Test)
{
    try {
        int tag = 0x01;
        int length = 0x1FFFF;
        std::string value = {};
        BerTlv berTlv = BerTlv(tag, length, value);
        EXPECT_EQ(0x83, berTlv.GetData()[1]);
        length = 0x88;
        berTlv = BerTlv(tag, length, value);
        EXPECT_EQ(0x81, berTlv.GetData()[1]);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : BER_TLV_TEST_002
 * @tc.name      : Empty_Data_Test
 * @tc.desc      : Generate ber-tlv with empty data
 */
TEST_F(BerTlvTest, Empty_Data_Test)
{
    try {
        std::string value = {};
        std::shared_ptr<BerTlv> berTlv = BerTlv::StrToBerTlv(value);
        EXPECT_TRUE(false);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(true);
    }
}

/**
 * @tc.number    : BER_TLV_TEST_003
 * @tc.name      : Data_Only_Tag_Test
 * @tc.desc      : Generate ber-tlv with tag-only data
 */
TEST_F(BerTlvTest, Data_Only_Tag_Test)
{
    try {
        std::string value = {0x01};
        std::shared_ptr<BerTlv> berTlv = BerTlv::StrToBerTlv(value);
        EXPECT_TRUE(false);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(true);
    }
}

/**
 * @tc.number    : BER_TLV_TEST_004
 * @tc.name      : Data_Only_Tag_2_Test
 * @tc.desc      : Generate ber-tlv with tag-only data
 */
TEST_F(BerTlvTest, Data_Only_Tag_2_Test)
{
    try {
        std::string value = {0x1F};
        std::shared_ptr<BerTlv> berTlv = BerTlv::StrToBerTlv(value);
        EXPECT_TRUE(false);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(true);
    }
}

/**
 * @tc.number    : BER_TLV_TEST_005
 * @tc.name      : Data_Length_1_Value_Not_Enough_Test
 * @tc.desc      : Generate ber-tlv with length-1 length but not enough value
 */
TEST_F(BerTlvTest, Data_Length_1_Value_Not_Enough_Test)
{
    try {
        std::string value = {0x01, (char)0x81, 0x01};
        std::shared_ptr<BerTlv> berTlv = BerTlv::StrToBerTlv(value);
        EXPECT_TRUE(false);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(true);
    }
}

/**
 * @tc.number    : BER_TLV_TEST_006
 * @tc.name      : Data_Length_2_Length_Not_Enough_Test
 * @tc.desc      : Generate ber-tlv with length-2 length but not enough length
 */
TEST_F(BerTlvTest, Data_Length_2_Length_Not_Enough_Test)
{
    try {
        std::string value = {0x01, (char)0x82, 0x01};
        std::shared_ptr<BerTlv> berTlv = BerTlv::StrToBerTlv(value);
        EXPECT_TRUE(false);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(true);
    }
}

/**
 * @tc.number    : BER_TLV_TEST_007
 * @tc.name      : Data_Length_2_Value_Not_Enough_Test
 * @tc.desc      : Generate ber-tlv with length-2 length but not enough value
 */
TEST_F(BerTlvTest, Data_Length_2_Value_Not_Enough_Test)
{
    try {
        std::string value = {0x01, (char)0x82, 0x01, 0x00};
        std::shared_ptr<BerTlv> berTlv = BerTlv::StrToBerTlv(value);
        EXPECT_TRUE(true);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : BER_TLV_TEST_008
 * @tc.name      : Data_Length_3_Length_Not_Enough_Test
 * @tc.desc      : Generate ber-tlv with length-3 length but not enough length
 */
TEST_F(BerTlvTest, Data_Length_3_Length_Not_Enough_Test)
{
    try {
        std::string value = {0x01, (char)0x83, 0x01, 0x00};
        std::shared_ptr<BerTlv> berTlv = BerTlv::StrToBerTlv(value);
        EXPECT_TRUE(false);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(true);
    }
}

/**
 * @tc.number    : BER_TLV_TEST_009
 * @tc.name      : Data_Length_3_Value_Not_Enough_Test
 * @tc.desc      : Generate ber-tlv with length-3 length but not enough value
 */
TEST_F(BerTlvTest, Data_Length_3_Value_Not_Enough_Test)
{
    try {
        std::string value = {0x01, (char)0x83, 0x01, 0x00, 0x00};
        std::shared_ptr<BerTlv> berTlv = BerTlv::StrToBerTlv(value);
        EXPECT_TRUE(true);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : BER_TLV_TEST_010
 * @tc.name      : Data_Length_Over_flow_Test
 * @tc.desc      : Generate ber-tlv with length-n length, n over flow
 */
TEST_F(BerTlvTest, Data_Length_Over_flow_Test)
{
    try {
        std::string value = {0x01, (char)0x84, 0x01, 0x00, 0x00};
        std::shared_ptr<BerTlv> berTlv = BerTlv::StrToBerTlv(value);
        EXPECT_TRUE(false);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(true);
    }
}
}  // namespace OHOS::se::test