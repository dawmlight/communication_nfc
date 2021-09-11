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
#define private public
#include "pkcs15/ef.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>
#include <string>
#include <vector>

#include "../access_rule_data_common.h"
#include "access_rule_files_channel.h"
#include "se_channel.h"
#undef private

using namespace OHOS::se::security;
using namespace OHOS::se;

namespace OHOS::se::test {
class EfTest : public ::testing::Test {
public:
    virtual void SetUp() override
    {
        std::cout << " EfTest set up." << std::endl;
        arfChannel_ = std::make_shared<AccessRuleFilesChannel>();
        ef_ = std::make_shared<EF>(arfChannel_);
    }

    virtual void TearDown() override
    {
        std::cout << " EfTest tear down." << std::endl;
    }

public:
    std::shared_ptr<AccessRuleFilesChannel> arfChannel_;
    std::shared_ptr<EF> ef_;
};

/**
 * @tc.number    : EF_TEST_001
 * @tc.name      : Get_File_Id_Test
 * @tc.desc      : Get file id
 */
TEST_F(EfTest, Get_File_Id_Test)
{
    try {
        EXPECT_EQ(0xFF, ef_->GetFileId());
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : EF_TEST_002
 * @tc.name      : Sim_Ef_Transparent_Test
 * @tc.desc      : Sim ef transparent
 */
TEST_F(EfTest, Sim_Ef_Transparent_Test)
{
    try {
        ef_->ParseFile(EF_SIM_EF_TRANSPARENT);
        EXPECT_EQ(0, ef_->GetFileNbRecords());
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : EF_TEST_003
 * @tc.name      : Sim_Ef_Linear_Fixed_Test
 * @tc.desc      : Sim ef linear fixed
 */
TEST_F(EfTest, Sim_Ef_Linear_Fixed_Test)
{
    try {
        ef_->ParseFile(EF_SIM_EF_LINEAR_FIXED);
        EXPECT_EQ(2, ef_->GetFileNbRecords());
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : EF_TEST_004
 * @tc.name      : Sim_Ef_Unknown_Test
 * @tc.desc      : Sim ef unknown
 */
TEST_F(EfTest, Sim_Ef_Unknown_Test)
{
    try {
        ef_->ParseFile(EF_SIM_UNKNOWN_UNKNOWN);
        EXPECT_EQ(0, ef_->GetFileNbRecords());
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : EF_TEST_005
 * @tc.name      : Usim_Ef_Transparent_Test
 * @tc.desc      : Usim ef transparent
 */
TEST_F(EfTest, Usim_Ef_Transparent_Test)
{
    try {
        ef_->ParseFile(EF_USIM_EF_TRANSPARENT);
        EXPECT_EQ(1, ef_->GetFileNbRecords());
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : EF_TEST_006
 * @tc.name      : Usim_Ef_Linear_Fixed_Test
 * @tc.desc      : Usim ef linear fixed
 */
TEST_F(EfTest, Usim_Ef_Linear_Fixed_Test)
{
    try {
        ef_->ParseFile(EF_USIM_EF_LINEAR_FIXED);
        EXPECT_EQ(1, ef_->GetFileNbRecords());
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : EF_TEST_007
 * @tc.name      : Usim_Ef_Unknown_Test
 * @tc.desc      : Usim ef unknown
 */
TEST_F(EfTest, Usim_Ef_Unknown_Test)
{
    try {
        ef_->ParseFile(EF_USIM_UNKNOWN_UNKNOWN);
        EXPECT_EQ(1, ef_->GetFileNbRecords());
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : EF_TEST_008
 * @tc.name      : Parse_Empty_Data_Test
 * @tc.desc      : Parse empty data
 */
TEST_F(EfTest, Parse_Empty_Data_Test)
{
    try {
        ef_->ParseFile("");
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : EF_TEST_009
 * @tc.name      : Parse_Sim_Data_Not_Enough_Test
 * @tc.desc      : Parse sim data but value not enough
 */
TEST_F(EfTest, Parse_Sim_Data_Not_Enough_Test)
{
    try {
        std::string data = {0x00, 0x01};
        ef_->ParseFile(data);
        EXPECT_TRUE(false);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(true);
    }
}

/**
 * @tc.number    : EF_TEST_010
 * @tc.name      : Read_Record_Test
 * @tc.desc      : Read record
 */
TEST_F(EfTest, Read_Record_Test)
{
    try {
        std::shared_ptr<Terminal> terminal;
        int channelNumber;
        std::string selectResponse;
        std::string aid;
        std::shared_ptr<SeChannel> channel = std::make_shared<SeChannel>(terminal, channelNumber, selectResponse, aid);
        arfChannel_->SetChannel(channel);
        ef_ = std::make_shared<EF>(arfChannel_);

        ef_->ParseFile(EF_SIM_EF_LINEAR_FIXED);
        ef_->ReadRecord(0);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        // tansmit error
        EXPECT_TRUE(true);
    }
}

/**
 * @tc.number    : EF_TEST_011
 * @tc.name      : Read_Record_Illegal_File_Type_Test
 * @tc.desc      : Read record with illegal file type
 */
TEST_F(EfTest, Read_Record_Illegal_File_Type_Test)
{
    try {
        ef_->ParseFile(EF_SIM_UNKNOWN_UNKNOWN);
        ef_->ReadRecord(0);
        EXPECT_TRUE(false);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(true);
    }
}

/**
 * @tc.number    : EF_TEST_012
 * @tc.name      : Read_Record_Illegal_File_Structure_Test
 * @tc.desc      : Read record with illegal file structure
 */
TEST_F(EfTest, Read_Record_Illegal_File_Structure_Test)
{
    try {
        ef_->ParseFile(EF_SIM_EF_TRANSPARENT);
        ef_->ReadRecord(0);
        EXPECT_TRUE(false);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(true);
    }
}

/**
 * @tc.number    : EF_TEST_013
 * @tc.name      : Read_Record_Illegal_Record_Num_Test
 * @tc.desc      : Read record with record num
 */
TEST_F(EfTest, Read_Record_Illegal_Record_Num_Test)
{
    try {
        ef_->ParseFile(EF_SIM_EF_LINEAR_FIXED);
        ef_->ReadRecord(100);
        EXPECT_TRUE(false);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(true);
    }
}

/**
 * @tc.number    : EF_TEST_014
 * @tc.name      : Read_Data_Test
 * @tc.desc      : Read data
 */
TEST_F(EfTest, Read_Data_Test)
{
    try {
        std::shared_ptr<Terminal> terminal;
        int channelNumber;
        std::string selectResponse;
        std::string aid;
        std::shared_ptr<SeChannel> channel = std::make_shared<SeChannel>(terminal, channelNumber, selectResponse, aid);
        arfChannel_->SetChannel(channel);
        ef_ = std::make_shared<EF>(arfChannel_);

        ef_->ParseFile(EF_SIM_EF_TRANSPARENT);
        ef_->ReadData(0, 16);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        // tansmit error
        EXPECT_TRUE(true);
    }
}

/**
 * @tc.number    : EF_TEST_015
 * @tc.name      : Read_Data_Len_Limit_Test
 * @tc.desc      : Read data with limit length
 */
TEST_F(EfTest, Read_Data_Len_Limit_Test)
{
    try {
        std::shared_ptr<Terminal> terminal;
        int channelNumber;
        std::string selectResponse;
        std::string aid;
        std::shared_ptr<SeChannel> channel = std::make_shared<SeChannel>(terminal, channelNumber, selectResponse, aid);
        arfChannel_->SetChannel(channel);
        ef_ = std::make_shared<EF>(arfChannel_);

        ef_->ParseFile(EF_SIM_EF_TRANSPARENT);
        ef_->ReadData(0, 253);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        // tansmit error
        EXPECT_TRUE(true);
    }
}

/**
 * @tc.number    : EF_TEST_016
 * @tc.name      : Read_Data_Illegal_File_Type_Test
 * @tc.desc      : Read data with illegal file type
 */
TEST_F(EfTest, Read_Data_Illegal_File_Type_Test)
{
    try {
        ef_->ParseFile(EF_SIM_UNKNOWN_UNKNOWN);
        ef_->ReadData(0, -1);
        EXPECT_TRUE(false);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(true);
    }
}

/**
 * @tc.number    : EF_TEST_017
 * @tc.name      : Read_Data_Illegal_File_Structure_Test
 * @tc.desc      : Read data with illegal file structure
 */
TEST_F(EfTest, Read_Data_Illegal_File_Structure_Test)
{
    try {
        ef_->ParseFile(EF_SIM_EF_LINEAR_FIXED);
        ef_->ReadData(0, -1);
        EXPECT_TRUE(false);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(true);
    }
}
}  // namespace OHOS::se::test