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
#include "general-data-objects/ref_do.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>
#include <string>
#include <vector>

#include "../access_rule_data_common.h"
#include "general-data-objects/aid_ref_do.h"
#include "general-data-objects/hash_ref_do.h"

using namespace OHOS::se::security;

namespace OHOS::se::test {
class RefDoTest : public ::testing::Test {
public:
    virtual void SetUp() override
    {
        std::cout << " RefDoTest set up." << std::endl;
    }

    virtual void TearDown() override
    {
        std::cout << " RefDoTest tear down." << std::endl;
    }

public:
};

/**
 * @tc.number    : REF_DO_TEST_001
 * @tc.name      : Ref_Test
 * @tc.desc      : Generate ref-do
 */
TEST_F(RefDoTest, Ref_Test)
{
    try {
        std::string data = REF_DO_A1_H1;
        std::shared_ptr<BerTlv> bt = BerTlv::StrToBerTlv(data);
        std::shared_ptr<RefDo> refDo = RefDo::BerTlvToRefDo(bt);
        std::shared_ptr<AidRefDo> ard = refDo->GetAidRefDo();
        std::shared_ptr<HashRefDo> hrd = refDo->GetHashRefDo();
        EXPECT_EQ(AID_1, ard->GetAid());
        EXPECT_EQ(HASH_1, hrd->GetHash());
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : REF_DO_TEST_002
 * @tc.name      : Illegal_Tag_Test
 * @tc.desc      : Generate ref-do with illegal tag
 */
TEST_F(RefDoTest, Illegal_Tag_Test)
{
    try {
        std::string data = REF_DO_A1_H1;
        data[0] = 0x00;
        std::shared_ptr<BerTlv> bt = BerTlv::StrToBerTlv(data);
        std::shared_ptr<RefDo> refArDo = RefDo::BerTlvToRefDo(bt);
        EXPECT_TRUE(false);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(true);
    }
}

/**
 * @tc.number    : REF_DO_TEST_003
 * @tc.name      : Data_Not_Enough_Test
 * @tc.desc      : Generate ref-do with not enough value
 */
TEST_F(RefDoTest, Data_Not_Enough_Test)
{
    try {
        std::string data = REF_DO_A1_H1;
        data = data.substr(0, data.length() - 1);
        std::shared_ptr<BerTlv> bt = BerTlv::StrToBerTlv(data);
        std::shared_ptr<RefDo> refArDo = RefDo::BerTlvToRefDo(bt);
        EXPECT_TRUE(false);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(true);
    }
}
}  // namespace OHOS::se::test