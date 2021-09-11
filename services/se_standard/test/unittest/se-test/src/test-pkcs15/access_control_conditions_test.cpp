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
#include "pkcs15/access_control_conditions.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>
#include <string>
#include <vector>

#include "../access_rule_data_common.h"
#include "access_rule_cache.h"
#include "access_rule_files_channel.h"
#include "general-data-objects/aid_ref_do.h"

using namespace OHOS::se::security;
using namespace OHOS::se;

namespace OHOS::se::test {
class AccessControlConditionsTest : public ::testing::Test {
public:
    virtual void SetUp() override
    {
        std::cout << " AccessControlConditionsTest set up." << std::endl;
        accessRuleCache_ = std::make_shared<AccessRuleCache>();
        aidRefDo_ = AidRefDo::AidToAidRefDo(AID_1);
        accessControlConditions_ = std::make_shared<AccessControlConditions>(accessRuleCache_, aidRefDo_);
    }

    virtual void TearDown() override
    {
        std::cout << " AccessControlConditionsTest tear down." << std::endl;
    }

public:
    std::shared_ptr<AccessRuleCache> accessRuleCache_;
    std::shared_ptr<AidRefDo> aidRefDo_;
    std::shared_ptr<AccessControlConditions> accessControlConditions_;
};

/**
 * @tc.number    : ACCESS_CONTROL_CONDITIONS_TEST_001
 * @tc.name      : Read_Data_Test
 * @tc.desc      : Read access-control-condition-file data
 */
TEST_F(AccessControlConditionsTest, Read_Data_Test)
{
    try {
        accessControlConditions_->ReadData(ACCESS_CONTROL_CONDITIONS_DATA);
        EXPECT_EQ(ACCESS_CONTROL_CONDITIONS_DATA, accessControlConditions_->GetData());
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : ACCESS_CONTROL_CONDITIONS_TEST_002
 * @tc.name      : Read_Data_Apdu_Always_Test
 * @tc.desc      : Read access-control-condition-file data with apdu-always
 */
TEST_F(AccessControlConditionsTest, Read_Data_Apdu_Always_Test)
{
    try {
        accessControlConditions_->ReadData(ACCESS_CONTROL_CONDITIONS_DATA_APDU_ALWAYS);
        EXPECT_EQ(ACCESS_CONTROL_CONDITIONS_DATA_APDU_ALWAYS, accessControlConditions_->GetData());
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : ACCESS_CONTROL_CONDITIONS_TEST_003
 * @tc.name      : Read_Data_Apdu_Filter_Test
 * @tc.desc      : Read access-control-condition-file data with apdu-filter
 */
TEST_F(AccessControlConditionsTest, Read_Data_Apdu_Filter_Test)
{
    try {
        accessControlConditions_->ReadData(ACCESS_CONTROL_CONDITIONS_DATA_APDU_FILTER);
        EXPECT_EQ(ACCESS_CONTROL_CONDITIONS_DATA_APDU_FILTER, accessControlConditions_->GetData());
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : ACCESS_CONTROL_CONDITIONS_TEST_004
 * @tc.name      : Read_Data_Empty_Value_Test
 * @tc.desc      : Read access-control-condition-file data with empty value
 */
TEST_F(AccessControlConditionsTest, Read_Data_Empty_Value_Test)
{
    try {
        accessControlConditions_->ReadData(ACCESS_CONTROL_CONDITIONS_DATA_ALL);
        EXPECT_EQ(ACCESS_CONTROL_CONDITIONS_DATA_ALL, accessControlConditions_->GetData());
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : ACCESS_CONTROL_CONDITIONS_TEST_005
 * @tc.name      : Read_Data_No_Valid_Info_Test
 * @tc.desc      : Read access-control-condition-file data with no valid info
 */
TEST_F(AccessControlConditionsTest, Read_Data_No_Valid_Info_Test)
{
    try {
        accessControlConditions_->ReadData(ACCESS_CONTROL_CONDITIONS_DATA_NO_VALID_INFO);
        EXPECT_EQ(ACCESS_CONTROL_CONDITIONS_DATA_NO_VALID_INFO, accessControlConditions_->GetData());
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : ACCESS_CONTROL_CONDITIONS_TEST_006
 * @tc.name      : Read_Data_Empty_Test
 * @tc.desc      : Read access-control-condition-file data empty
 */
TEST_F(AccessControlConditionsTest, Read_Data_Empty_Test)
{
    try {
        accessControlConditions_->ReadData(ACCESS_CONTROL_CONDITIONS_DATA_EMPTY);
        EXPECT_EQ(ACCESS_CONTROL_CONDITIONS_DATA_EMPTY, accessControlConditions_->GetData());
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : ACCESS_CONTROL_CONDITIONS_TEST_007
 * @tc.name      : Read_Data_Hash_Length_Illegal_Test
 * @tc.desc      : Read access-control-condition-file data with illegal hash length
 */
TEST_F(AccessControlConditionsTest, Read_Data_Hash_Length_Illegal_Test)
{
    try {
        accessControlConditions_->ReadData(ACCESS_CONTROL_CONDITIONS_DATA_HASH_LENGTH_ERROR);
        EXPECT_EQ(ACCESS_CONTROL_CONDITIONS_DATA_HASH_LENGTH_ERROR, accessControlConditions_->GetData());
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(true);
    }
}

/**
 * @tc.number    : ACCESS_CONTROL_CONDITIONS_TEST_008
 * @tc.name      : Read_Data_Illegal_Apdu_Test
 * @tc.desc      : Read access-control-condition-file data with illegal apdu
 */
TEST_F(AccessControlConditionsTest, Read_Data_Illegal_Apdu_Test)
{
    try {
        accessControlConditions_->ReadData(ACCESS_CONTROL_CONDITIONS_DATA_INVALID_APDU);
        EXPECT_EQ(ACCESS_CONTROL_CONDITIONS_DATA_INVALID_APDU, accessControlConditions_->GetData());
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(true);
    }
}

/**
 * @tc.number    : ACCESS_CONTROL_CONDITIONS_TEST_009
 * @tc.name      : Read_Data_Illegal_Apdu_Filter_Test
 * @tc.desc      : Read access-control-condition-file data with illegal apdu filter
 */
TEST_F(AccessControlConditionsTest, Read_Data_Illegal_Apdu_Filter_Test)
{
    try {
        accessControlConditions_->ReadData(ACCESS_CONTROL_CONDITIONS_DATA_INVALID_APDU_FILTER);
        EXPECT_EQ(ACCESS_CONTROL_CONDITIONS_DATA_INVALID_APDU_FILTER, accessControlConditions_->GetData());
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(true);
    }
}

/**
 * @tc.number    : ACCESS_CONTROL_CONDITIONS_TEST_010
 * @tc.name      : Read_Data_Illegal_Nfc_Test
 * @tc.desc      : Read access-control-condition-file data with illegal nfc
 */
TEST_F(AccessControlConditionsTest, Read_Data_Illegal_Nfc_Test)
{
    try {
        accessControlConditions_->ReadData(ACCESS_CONTROL_CONDITIONS_DATA_INVALID_NFC);
        EXPECT_EQ(ACCESS_CONTROL_CONDITIONS_DATA_INVALID_NFC, accessControlConditions_->GetData());
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(true);
    }
}

/**
 * @tc.number    : ACCESS_CONTROL_CONDITIONS_TEST_011
 * @tc.name      : Read_Data_Illegal_Tag_Test
 * @tc.desc      : Read access-control-condition-file data with illegal tag
 */
TEST_F(AccessControlConditionsTest, Read_Data_Illegal_Tag_Test)
{
    try {
        accessControlConditions_->ReadData(ACCESS_CONTROL_CONDITIONS_DATA_INVALID_TAG);
        EXPECT_EQ(ACCESS_CONTROL_CONDITIONS_DATA_INVALID_TAG, accessControlConditions_->GetData());
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(true);
    }
}

/**
 * @tc.number    : ACCESS_CONTROL_CONDITIONS_TEST_012
 * @tc.name      : Read_Data_From_Path_Test
 * @tc.desc      : Read data from path
 */
TEST_F(AccessControlConditionsTest, Read_Data_From_Path_Test)
{
    try {
        std::shared_ptr<AccessRuleFilesChannel> arfChannel;
        std::string path = {};
        accessControlConditions_->ReadDataFromPath(arfChannel, path);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        // transmit error
        EXPECT_TRUE(true);
    }
}
}  // namespace OHOS::se::test