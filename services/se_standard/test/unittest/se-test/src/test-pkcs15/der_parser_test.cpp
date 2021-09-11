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
#include "pkcs15/der_parser.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>
#include <string>
#include <vector>

#include "../access_rule_data_common.h"

using namespace OHOS::se::security;
using namespace OHOS::se;

namespace OHOS::se::test {
class DerParserTest : public ::testing::Test {
public:
    virtual void SetUp() override
    {
        std::cout << " DerParser set up." << std::endl;
    }

    virtual void TearDown() override
    {
        std::cout << " DerParser tear down." << std::endl;
    }

public:
};

/**
 * @tc.number    : DER_PARSER_TEST_001
 * @tc.name      : Create_Illegal_Format_Test
 * @tc.desc      : Create der parser with illegal format
 */
TEST_F(DerParserTest, Create_Illegal_Format_Test)
{
    try {
        std::string data = {(char)0xFF, 0x01};
        DerParser der = DerParser(data);
        EXPECT_TRUE(false);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(true);
    }
}

/**
 * @tc.number    : DER_PARSER_TEST_002
 * @tc.name      : Parse_Tlv_Unexpected_Type_Test
 * @tc.desc      : Parse tlv with unexpected type
 */
TEST_F(DerParserTest, Parse_Tlv_Unexpected_Type_Test)
{
    try {
        std::string data = {0x01, 0x01, 0x01};
        DerParser der = DerParser(data);
        der.ParseTlv(0x02);
        EXPECT_TRUE(false);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(true);
    }
}

/**
 * @tc.number    : DER_PARSER_TEST_003
 * @tc.name      : Restore_Snapshot_Illegal_Size_Test
 * @tc.desc      : Restore snapshot with illegal size
 */
TEST_F(DerParserTest, Restore_Snapshot_Illegal_Size_Test)
{
    try {
        std::string data = {0x01, 0x01, 0x01};
        DerParser der = DerParser(data);
        std::vector<int> snapshot;
        der.RestoreSnapshot(snapshot);
        EXPECT_TRUE(false);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(true);
    }
}

/**
 * @tc.number    : DER_PARSER_TEST_004
 * @tc.name      : Restore_Snapshot_Out_Of_Index_Test
 * @tc.desc      : Restore snapshot with value out of index
 */
TEST_F(DerParserTest, Restore_Snapshot_Out_Of_Index_Test)
{
    try {
        std::string data = {0x01, 0x01, 0x01};
        DerParser der = DerParser(data);
        std::vector<int> snapshot;
        snapshot.push_back(100);
        snapshot.push_back(1);
        der.RestoreSnapshot(snapshot);
        EXPECT_TRUE(false);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(true);
    }
}

/**
 * @tc.number    : DER_PARSER_TEST_005
 * @tc.name      : Parse_Oid_Test
 * @tc.desc      : Parse oid
 */
TEST_F(DerParserTest, Parse_Oid_Test)
{
    try {
        std::string data = {
            0x06, 0x0A, (char)0x88, (char)0x86, 0x48, (char)0x86, (char)0xFC, 0x6B, (char)0x81, 0x48, 0x01, 0x01};
        DerParser der = DerParser(data);
        EXPECT_EQ("2.131832.114283.200.1.1", der.ParseOid());
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(true);
    }
}

/**
 * @tc.number    : DER_PARSER_TEST_006
 * @tc.name      : Parse_Oid_Illegal_Length_Test
 * @tc.desc      : Parse oid with illegal length
 */
TEST_F(DerParserTest, Parse_Oid_Illegal_Length_Test)
{
    try {
        std::string data = {0x06, 0x00, 0x01};
        DerParser der = DerParser(data);
        der.ParseOid();
        EXPECT_TRUE(false);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(true);
    }
}

/**
 * @tc.number    : DER_PARSER_TEST_007
 * @tc.name      : Get_Tlv_Type_Is_End_Test
 * @tc.desc      : Get tlv type while is end
 */
TEST_F(DerParserTest, Get_Tlv_Type_Is_End_Test)
{
    try {
        std::string data = {0x01, 0x01, 0x01};
        DerParser der = DerParser(data);
        der.ParseTlv();
        der.SkipTlvData();
        der.GetTlvType();
        EXPECT_TRUE(false);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(true);
    }
}

/**
 * @tc.number    : DER_PARSER_TEST_008
 * @tc.name      : Get_Tlv_Size_Is_End_Test
 * @tc.desc      : Get tlv size while is end
 */
TEST_F(DerParserTest, Get_Tlv_Size_Is_End_Test)
{
    try {
        std::string data = {0x01, 0x01, 0x01};
        DerParser der = DerParser(data);
        der.ParseTlv();
        der.SkipTlvData();
        der.GetTlvSize();
        EXPECT_TRUE(false);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(true);
    }
}
}  // namespace OHOS::se::test