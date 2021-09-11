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
#include "general-data-objects/hash_ref_do.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>
#include <string>
#include <vector>

#include "../access_rule_data_common.h"

using namespace OHOS::se::security;

namespace OHOS::se::test {
class HashRefDoTest : public ::testing::Test {
public:
    virtual void SetUp() override
    {
        std::cout << " HashRefDoTest set up." << std::endl;
    }

    virtual void TearDown() override
    {
        std::cout << " HashRefDoTest tear down." << std::endl;
    }

public:
};

/**
 * @tc.number    : HASH_REF_DO_TEST_001
 * @tc.name      : Hash_Test
 * @tc.desc      : Generate hash-ref-do
 */
TEST_F(HashRefDoTest, Hash_Test)
{
    try {
        std::string data = HASH_DO_1;
        std::shared_ptr<BerTlv> bt = BerTlv::StrToBerTlv(data);
        std::shared_ptr<HashRefDo> hashRefDo = HashRefDo::BerTlvToHashRefDo(bt);
        EXPECT_EQ(HASH_1, hashRefDo->GetHash());
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : HASH_REF_DO_TEST_002
 * @tc.name      : Empty_Hash_Test
 * @tc.desc      : Generate hash-ref-do with empty data
 */
TEST_F(HashRefDoTest, Empty_Hash_Test)
{
    try {
        std::shared_ptr<HashRefDo> hashRefDo = std::make_shared<HashRefDo>();
        EXPECT_EQ("", hashRefDo->GetHash());
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : HASH_REF_DO_TEST_003
 * @tc.name      : Illegal_Tag_Test
 * @tc.desc      : Generate hash-ref-do with illegal tag
 */
TEST_F(HashRefDoTest, Illegal_Tag_Test)
{
    try {
        std::string data = HASH_1;
        data[0] = 0x00;
        std::shared_ptr<BerTlv> bt = BerTlv::StrToBerTlv(data);
        std::shared_ptr<HashRefDo> hashRefDo = HashRefDo::BerTlvToHashRefDo(bt);
        EXPECT_TRUE(false);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(true);
    }
}

/**
 * @tc.number    : HASH_REF_DO_TEST_004
 * @tc.name      : Data_Not_Enough_Test
 * @tc.desc      : Generate hash-ref-do with not enough value
 */
TEST_F(HashRefDoTest, Data_Not_Enough_Test)
{
    try {
        std::string data = HASH_1;
        data = data.substr(0, data.length() - 1);
        std::shared_ptr<BerTlv> bt = BerTlv::StrToBerTlv(data);
        std::shared_ptr<HashRefDo> hashRefDo = HashRefDo::BerTlvToHashRefDo(bt);
        EXPECT_TRUE(false);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(true);
    }
}

/**
 * @tc.number    : HASH_REF_DO_TEST_005
 * @tc.name      : Illegal_Length_Test
 * @tc.desc      : Generate hash-ref-do with illegal length
 */
TEST_F(HashRefDoTest, Illegal_Length_Test)
{
    try {
        std::string data = {(char)0xC1, 0x02, 0x00, 0x00};
        std::shared_ptr<BerTlv> bt = BerTlv::StrToBerTlv(data);
        std::shared_ptr<HashRefDo> hashRefDo = HashRefDo::BerTlvToHashRefDo(bt);
        EXPECT_TRUE(false);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(true);
    }
}
}  // namespace OHOS::se::test