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
#include "general-data-objects/nfc_ar_do.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>
#include <string>
#include <vector>

#include "../access_rule_data_common.h"

using namespace OHOS::se::security;

namespace OHOS::se::test {
class NfcArDoTest : public ::testing::Test {
public:
    virtual void SetUp() override
    {
        std::cout << " NfcArDoTest set up." << std::endl;
    }

    virtual void TearDown() override
    {
        std::cout << " NfcArDoTest tear down." << std::endl;
    }

public:
};

/**
 * @tc.number    : NFC_AR_DO_TEST_001
 * @tc.name      : Nfc_Test
 * @tc.desc      : Generate nfc-ar-do
 */
TEST_F(NfcArDoTest, Nfc_Test)
{
    try {
        std::string data = {(char)0xD1, 0x01, 0x01};
        std::shared_ptr<BerTlv> bt = BerTlv::StrToBerTlv(data);
        std::shared_ptr<NfcArDo> nfcArDo = NfcArDo::BerTlvToNfcArDo(bt);
        EXPECT_TRUE(nfcArDo->IsNfcAllowed());
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : NFC_AR_DO_TEST_002
 * @tc.name      : Illegal_Tag_Test
 * @tc.desc      : Generate nfc-ar-do with illegal tag
 */
TEST_F(NfcArDoTest, Illegal_Tag_Test)
{
    try {
        std::string data = {(char)0x00, 0x01, 0x01};
        std::shared_ptr<BerTlv> bt = BerTlv::StrToBerTlv(data);
        std::shared_ptr<NfcArDo> nfcArDo = NfcArDo::BerTlvToNfcArDo(bt);
        EXPECT_FALSE(nfcArDo);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : NFC_AR_DO_TEST_003
 * @tc.name      : Data_Not_Enough_Test
 * @tc.desc      : Generate nfc-ar-do with not enough value
 */
TEST_F(NfcArDoTest, Data_Not_Enough_Test)
{
    try {
        std::string data = {(char)0xD1, 0x01};
        std::shared_ptr<BerTlv> bt = BerTlv::StrToBerTlv(data);
        std::shared_ptr<NfcArDo> nfcArDo = NfcArDo::BerTlvToNfcArDo(bt);
        EXPECT_TRUE(false);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(true);
    }
}

/**
 * @tc.number    : NFC_AR_DO_TEST_004
 * @tc.name      : Illegal_Length_Test
 * @tc.desc      : Generate nfc-ar-do with illegal length
 */
TEST_F(NfcArDoTest, Illegal_Length_Test)
{
    try {
        std::string data = {(char)0xD1, 0x02, 0x01, 0x01};
        std::shared_ptr<BerTlv> bt = BerTlv::StrToBerTlv(data);
        std::shared_ptr<NfcArDo> nfcArDo = NfcArDo::BerTlvToNfcArDo(bt);
        EXPECT_TRUE(false);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(true);
    }
}
}  // namespace OHOS::se::test