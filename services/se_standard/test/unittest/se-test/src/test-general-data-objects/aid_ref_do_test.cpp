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
#include "general-data-objects/aid_ref_do.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>
#include <string>
#include <vector>

#include "../access_rule_data_common.h"

using namespace OHOS::se::security;

namespace OHOS::se::test {
class AidRefDoTest : public ::testing::Test {
public:
    virtual void SetUp() override
    {
        std::cout << " AidRefDoTest set up." << std::endl;
    }

    virtual void TearDown() override
    {
        std::cout << " AidRefDoTest tear down." << std::endl;
    }

public:
};

/**
 * @tc.number    : AID_REF_DO_TEST_001
 * @tc.name      : Create_By_Aid_Test
 * @tc.desc      : Generate aid-ref-do with normal aid 
 */
TEST_F(AidRefDoTest, Create_By_Aid_Test)
{
    try {
        std::shared_ptr<AidRefDo> aidRefDo = AidRefDo::AidToAidRefDo(AID_1);
        EXPECT_EQ(AID_1, aidRefDo->GetAid());
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : AID_REF_DO_TEST_002
 * @tc.name      : Create_By_Empty_Aid_Test
 * @tc.desc      : Generate aid-ref-do with empty aid 
 */
TEST_F(AidRefDoTest, Create_By_Empty_Aid_Test)
{
    try {
        std::shared_ptr<AidRefDo> aidRefDo = AidRefDo::AidToAidRefDo(AID_AID_ALL);
        EXPECT_EQ(AID_AID_ALL, aidRefDo->GetAid());
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : AID_REF_DO_TEST_003
 * @tc.name      : Create_By_Default_Aid_Test
 * @tc.desc      : Generate aid-ref-do with default aid 
 */
TEST_F(AidRefDoTest, Create_By_Default_Aid_Test)
{
    try {
        std::shared_ptr<AidRefDo> aidRefDo = AidRefDo::AidToAidRefDo(AID_3);
        EXPECT_EQ(AID_3, aidRefDo->GetAid());
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : AID_REF_DO_TEST_004
 * @tc.name      : Create_By_Data_Test
 * @tc.desc      : Generate aid-ref-do with data 
 */
TEST_F(AidRefDoTest, Create_By_Data_Test)
{
    try {
        std::string data = AID_DO_1;
        std::shared_ptr<BerTlv> bt = BerTlv::StrToBerTlv(data);
        std::shared_ptr<AidRefDo> aidRefDo = AidRefDo::BerTlvToAidRefDo(bt);
        EXPECT_EQ(AID_1, aidRefDo->GetAid());
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : AID_REF_DO_TEST_005
 * @tc.name      : Create_By_Data_Empty_Aid_Test
 * @tc.desc      : Generate aid-ref-do with empty aid data 
 */
TEST_F(AidRefDoTest, Create_By_Data_Empty_Aid_Test)
{
    try {
        std::string data = AID_DO_AID_ALL;
        std::shared_ptr<BerTlv> bt = BerTlv::StrToBerTlv(data);
        std::shared_ptr<AidRefDo> aidRefDo = AidRefDo::BerTlvToAidRefDo(bt);
        EXPECT_EQ(AID_AID_ALL, aidRefDo->GetAid());
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : AID_REF_DO_TEST_006
 * @tc.name      : Illegal_Tag_Test
 * @tc.desc      : Generate aid-ref-do with illegal tag 
 */
TEST_F(AidRefDoTest, Illegal_Tag_Test)
{
    try {
        std::string data = AID_DO_1;
        data[0] = 0x00;
        std::shared_ptr<BerTlv> bt = BerTlv::StrToBerTlv(data);
        std::shared_ptr<AidRefDo> aidRefDo = AidRefDo::BerTlvToAidRefDo(bt);
        EXPECT_TRUE(false);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(true);
    }
}

/**
 * @tc.number    : AID_REF_DO_TEST_007
 * @tc.name      : Illegal_Default_length_Test
 * @tc.desc      : Generate aid-ref-do with default tag data but illeagal length
 */
TEST_F(AidRefDoTest, Illegal_Default_length_Test)
{
    try {
        std::string data = AID_DO_1;
        data[0] = AidRefDo::AID_REF_DO_TAG_DEFAULT_APPLICATION;
        std::shared_ptr<BerTlv> bt = BerTlv::StrToBerTlv(data);
        std::shared_ptr<AidRefDo> aidRefDo = AidRefDo::BerTlvToAidRefDo(bt);
        EXPECT_TRUE(false);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(true);
    }
}

/**
 * @tc.number    : AID_REF_DO_TEST_008
 * @tc.name      : Illegal_length_Test
 * @tc.desc      : Generate aid-ref-do with data but illeagal aid-length
 */
TEST_F(AidRefDoTest, Illegal_length_Test)
{
    try {
        std::string aid = {0x01};
        int length = aid.length();
        std::string data = STRING_AID_REF_DO_TAG + (char)length + aid;
        std::shared_ptr<BerTlv> bt = BerTlv::StrToBerTlv(data);
        std::shared_ptr<AidRefDo> aidRefDo = AidRefDo::BerTlvToAidRefDo(bt);
        EXPECT_TRUE(false);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(true);
    }
}

/**
 * @tc.number    : AID_REF_DO_TEST_009
 * @tc.name      : Data_Not_enough_Test
 * @tc.desc      : Generate aid-ref-do with data but not enough value
 */
TEST_F(AidRefDoTest, Data_Not_enough_Test)
{
    try {
        int length = AID_1.length();
        std::string data = STRING_AID_REF_DO_TAG + (char)(length + 1) + AID_1;
        std::shared_ptr<BerTlv> bt = BerTlv::StrToBerTlv(data);
        std::shared_ptr<AidRefDo> aidRefDo = AidRefDo::BerTlvToAidRefDo(bt);
        EXPECT_TRUE(false);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(true);
    }
}
}  // namespace OHOS::se::test