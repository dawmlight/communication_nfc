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
#include "general-data-objects/ref_ar_do.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>
#include <string>
#include <vector>

#include "../access_rule_data_common.h"
#include "general-data-objects/aid_ref_do.h"
#include "general-data-objects/apdu_ar_do.h"
#include "general-data-objects/ar_do.h"
#include "general-data-objects/hash_ref_do.h"
#include "general-data-objects/nfc_ar_do.h"
#include "general-data-objects/ref_do.h"

using namespace OHOS::se::security;

namespace OHOS::se::test {
class RefArDoTest : public ::testing::Test {
public:
    virtual void SetUp() override
    {
        std::cout << " RefArDoTest set up." << std::endl;
    }

    virtual void TearDown() override
    {
        std::cout << " RefArDoTest tear down." << std::endl;
    }

public:
};

/**
 * @tc.number    : REF_AR_DO_TEST_001
 * @tc.name      : RefAr_Test
 * @tc.desc      : Generate ref-ar-do
 */
TEST_F(RefArDoTest, RefAr_Test)
{
    try {
        std::string value = REF_DO_A1_H1 + AR_DO_DATA_11;
        int length = value.length();
        std::string data = STRING_REF_AR_DO_TAG + (char)length + value;
        std::shared_ptr<BerTlv> bt = BerTlv::StrToBerTlv(data);
        std::shared_ptr<RefArDo> refArDo = RefArDo::BerTlvToRefArDo(bt);

        std::shared_ptr<ArDo> ad = refArDo->GetArDo();
        std::shared_ptr<ApduArDo> aad = ad->GetApduArDo();
        std::shared_ptr<NfcArDo> nad = ad->GetNfcArDo();
        EXPECT_TRUE(aad->IsApduAllowed());
        EXPECT_TRUE(aad->GetApduFilters().empty());
        EXPECT_TRUE(nad->IsNfcAllowed());
        
        std::shared_ptr<RefDo> rd = refArDo->GetRefDo();
        std::shared_ptr<AidRefDo> ard = rd->GetAidRefDo();
        std::shared_ptr<HashRefDo> hrd = rd->GetHashRefDo();
        EXPECT_EQ(AID_1, ard->GetAid());
        EXPECT_EQ(HASH_1, hrd->GetHash());
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : REF_AR_DO_TEST_002
 * @tc.name      : Illegal_Tag_Test
 * @tc.desc      : Generate ref-ar-do with illegal tag
 */
TEST_F(RefArDoTest, Illegal_Tag_Test)
{
    try {
        std::string value = REF_DO_A1_H1 + AR_DO_DATA_11;
        int length = value.length();
        std::string data = STRING_REF_AR_DO_TAG + (char)length + value;
        data[0] = 0x00;
        std::shared_ptr<BerTlv> bt = BerTlv::StrToBerTlv(data);
        std::shared_ptr<RefArDo> refArDo = RefArDo::BerTlvToRefArDo(bt);
        EXPECT_TRUE(false);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(true);
    }
}

/**
 * @tc.number    : REF_AR_DO_TEST_003
 * @tc.name      : Data_Not_Enough_Test
 * @tc.desc      : Generate ref-ar-do with not ehough value
 */
TEST_F(RefArDoTest, Data_Not_Enough_Test)
{
    try {
        std::string value = REF_DO_A1_H1 + AR_DO_DATA_11;
        int length = value.length();
        std::string data = STRING_REF_AR_DO_TAG + (char)length + value;
        data = data.substr(0, data.length() - 1);
        std::shared_ptr<BerTlv> bt = BerTlv::StrToBerTlv(data);
        std::shared_ptr<RefArDo> refArDo = RefArDo::BerTlvToRefArDo(bt);
        EXPECT_TRUE(false);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(true);
    }
}
}  // namespace OHOS::se::test