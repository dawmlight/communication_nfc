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
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>
#include <string>
#include <vector>

#include "access_rule_cache.h"
#include "access_rule_data_common.h"
#include "channel_access_rule.h"
#include "general-data-objects/ar_do.h"
#include "general-data-objects/ref_do.h"

using namespace OHOS::se::security;

namespace OHOS::se::test {
class NfcTcSeTest : public ::testing::Test {
public:
    virtual void SetUp() override
    {
        std::cout << " NfcTcSeTest set up." << std::endl;
        accessRuleCache_ = std::make_shared<AccessRuleCache>();
    }

    virtual void TearDown() override
    {
        std::cout << " NfcTcSeTest tear down." << std::endl;
    }

    bool IsContainFilter(std::string apdu, std::vector<std::string> apduFilters)
    {
        for (std::string str : apduFilters) {
            if (apdu == str) {
                return true;
            }
        }
        return false;
    }

    enum AccessRuleResult {
        ApduAlwaysNfcAlways,
        ApduNeverNfcNever,
        ApduFilter1,
        ApduFilter23,
        ApduAlwaysNfcNever,
        ApduNeverNfcAlways,
        NfcAlways,
        NfcNever
    };

    void CheckChannelAccessRule(std::string aid, std::vector<std::string> hashes, AccessRuleResult result)
    {
        ChannelAccessRule channelAccessRule = accessRuleCache_->GetAccessRule(aid, hashes);
        switch (result) {
            case ApduAlwaysNfcAlways: {
                EXPECT_EQ(ChannelAccessRule::ACCESSRULE::ALWAYS, channelAccessRule.GetAccessRule());
                EXPECT_EQ(ChannelAccessRule::ACCESSRULE::ALWAYS, channelAccessRule.GetApduAccessRule());
                EXPECT_FALSE(channelAccessRule.HasApduFilter());
                EXPECT_EQ(ChannelAccessRule::ACCESSRULE::ALWAYS, channelAccessRule.GetNFCEventAccessRule());
            } break;

            case ApduNeverNfcNever: {
                EXPECT_EQ(ChannelAccessRule::ACCESSRULE::NEVER, channelAccessRule.GetAccessRule());
                EXPECT_EQ(ChannelAccessRule::ACCESSRULE::NEVER, channelAccessRule.GetApduAccessRule());
                EXPECT_FALSE(channelAccessRule.HasApduFilter());
                EXPECT_EQ(ChannelAccessRule::ACCESSRULE::NEVER, channelAccessRule.GetNFCEventAccessRule());
            } break;

            case ApduFilter1: {
                EXPECT_EQ(ChannelAccessRule::ACCESSRULE::ALWAYS, channelAccessRule.GetAccessRule());
                EXPECT_TRUE(channelAccessRule.HasApduFilter());
                EXPECT_EQ(1U, channelAccessRule.GetApduFilters().size());
                EXPECT_TRUE(IsContainFilter(APDU_1, channelAccessRule.GetApduFilters()));
                EXPECT_FALSE(IsContainFilter(APDU_2, channelAccessRule.GetApduFilters()));
                EXPECT_EQ(ChannelAccessRule::ACCESSRULE::ALWAYS, channelAccessRule.GetNFCEventAccessRule());
            } break;

            case ApduFilter23: {
                EXPECT_EQ(ChannelAccessRule::ACCESSRULE::ALWAYS, channelAccessRule.GetAccessRule());
                EXPECT_TRUE(channelAccessRule.HasApduFilter());
                EXPECT_EQ(2U, channelAccessRule.GetApduFilters().size());
                EXPECT_FALSE(IsContainFilter(APDU_1, channelAccessRule.GetApduFilters()));
                EXPECT_TRUE(IsContainFilter(APDU_2, channelAccessRule.GetApduFilters()));
                EXPECT_TRUE(IsContainFilter(APDU_3, channelAccessRule.GetApduFilters()));
                EXPECT_EQ(ChannelAccessRule::ACCESSRULE::ALWAYS, channelAccessRule.GetNFCEventAccessRule());
            } break;

            case ApduAlwaysNfcNever: {
                EXPECT_EQ(ChannelAccessRule::ACCESSRULE::ALWAYS, channelAccessRule.GetAccessRule());
                EXPECT_EQ(ChannelAccessRule::ACCESSRULE::ALWAYS, channelAccessRule.GetApduAccessRule());
                EXPECT_FALSE(channelAccessRule.HasApduFilter());
                EXPECT_EQ(ChannelAccessRule::ACCESSRULE::NEVER, channelAccessRule.GetNFCEventAccessRule());
            } break;

            case ApduNeverNfcAlways: {
                EXPECT_EQ(ChannelAccessRule::ACCESSRULE::NEVER, channelAccessRule.GetAccessRule());
                EXPECT_EQ(ChannelAccessRule::ACCESSRULE::NEVER, channelAccessRule.GetApduAccessRule());
                EXPECT_FALSE(channelAccessRule.HasApduFilter());
                EXPECT_EQ(ChannelAccessRule::ACCESSRULE::ALWAYS, channelAccessRule.GetNFCEventAccessRule());
            } break;

            case NfcAlways: {
                EXPECT_EQ(ChannelAccessRule::ACCESSRULE::ALWAYS, channelAccessRule.GetNFCEventAccessRule());
            } break;

            case NfcNever: {
                EXPECT_EQ(ChannelAccessRule::ACCESSRULE::NEVER, channelAccessRule.GetNFCEventAccessRule());
            } break;

            default: {
                EXPECT_TRUE(false);
            } break;
        }
    }

public:
    std::shared_ptr<AccessRuleCache> accessRuleCache_;
};

/**
 * @tc.number    : NFC_TC_SE_AccessControlTest_001_001
 * @tc.name      : AccessControlTest_001_001
 * @tc.desc      : Access control test
 */
TEST_F(NfcTcSeTest, AccessControlTest_001_001)
{
    try {
        // R0:AID.#1 (All-Ref)- AR-APDU-ALWAYS
        std::string refData = REF_DO_HASH_ALL;
        std::shared_ptr<BerTlv> refBt = BerTlv::StrToBerTlv(refData);
        std::shared_ptr<RefDo> refDo = RefDo::BerTlvToRefDo(refBt);
        std::string arData = AR_DO_DATA_ONLY_APDU_ALWAYS;
        std::shared_ptr<BerTlv> arBt = BerTlv::StrToBerTlv(arData);
        std::shared_ptr<ArDo> arDo = ArDo::BerTlvToArDo(arBt);
        accessRuleCache_->AddAccessRule(refDo, arDo);

        std::vector<std::string> hashes1;
        hashes1.push_back(HASH_1);
        CheckChannelAccessRule(AID_HASH_ALL, hashes1, ApduAlwaysNfcAlways);

        std::vector<std::string> hashes2;
        hashes2.push_back(HASH_2);
        CheckChannelAccessRule(AID_HASH_ALL, hashes2, ApduAlwaysNfcAlways);

        std::vector<std::string> hashes3;
        CheckChannelAccessRule(AID_HASH_ALL, hashes3, ApduNeverNfcNever);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : NFC_TC_SE_AccessControlTest_001_002
 * @tc.name      : AccessControlTest_001_002
 * @tc.desc      : Access control test
 */
TEST_F(NfcTcSeTest, AccessControlTest_001_002)
{
    try {
        // R0:AID.#1 (All-Ref)- AR-APDU-ALWAYS
        // R1:AID.#2 (All-Ref)- AR-APDU-ALWAYS
        std::string refData = REF_DO_HASH_ALL;
        std::shared_ptr<BerTlv> refBt = BerTlv::StrToBerTlv(refData);
        std::shared_ptr<RefDo> refDo = RefDo::BerTlvToRefDo(refBt);
        std::string refData2 = REF_DO_HASH_ALL_2;
        std::shared_ptr<BerTlv> refBt2 = BerTlv::StrToBerTlv(refData2);
        std::shared_ptr<RefDo> refDo2 = RefDo::BerTlvToRefDo(refBt2);
        std::string arData = AR_DO_DATA_ONLY_APDU_ALWAYS;
        std::shared_ptr<BerTlv> arBt = BerTlv::StrToBerTlv(arData);
        std::shared_ptr<ArDo> arDo = ArDo::BerTlvToArDo(arBt);
        accessRuleCache_->AddAccessRule(refDo, arDo);
        accessRuleCache_->AddAccessRule(refDo2, arDo);

        std::vector<std::string> hashes1;
        hashes1.push_back(HASH_1);
        CheckChannelAccessRule(AID_HASH_ALL, hashes1, ApduAlwaysNfcAlways);
        CheckChannelAccessRule(AID_HASH_ALL_2, hashes1, ApduAlwaysNfcAlways);
        CheckChannelAccessRule(AID_1, hashes1, ApduNeverNfcNever);

        std::vector<std::string> hashes2;
        CheckChannelAccessRule(AID_HASH_ALL, hashes2, ApduNeverNfcNever);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : NFC_TC_SE_AccessControlTest_001_003
 * @tc.name      : AccessControlTest_001_003
 * @tc.desc      : Access control test
 */
TEST_F(NfcTcSeTest, AccessControlTest_001_003)
{
    try {
        // AID.#All (All-Ref)- AR-APDU-ALWAYS
        std::string refData = REF_DO_AID_ALL_HASH_ALL;
        std::shared_ptr<BerTlv> refBt = BerTlv::StrToBerTlv(refData);
        std::shared_ptr<RefDo> refDo = RefDo::BerTlvToRefDo(refBt);
        std::string arData = AR_DO_DATA_ONLY_APDU_ALWAYS;
        std::shared_ptr<BerTlv> arBt = BerTlv::StrToBerTlv(arData);
        std::shared_ptr<ArDo> arDo = ArDo::BerTlvToArDo(arBt);
        accessRuleCache_->AddAccessRule(refDo, arDo);

        std::vector<std::string> hashes1;
        hashes1.push_back(HASH_1);
        CheckChannelAccessRule(AID_2, hashes1, ApduAlwaysNfcAlways);

        std::vector<std::string> hashes2;
        CheckChannelAccessRule(AID_3, hashes2, ApduNeverNfcNever);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : NFC_TC_SE_AccessControlTest_002_001
 * @tc.name      : AccessControlTest_002_001
 * @tc.desc      : Access control test
 */
TEST_F(NfcTcSeTest, AccessControlTest_002_001)
{
    try {
        // R0:AID.#1(All-Ref)- AR-APDU-NEVER
        std::string refData = REF_DO_HASH_ALL;
        std::shared_ptr<BerTlv> refBt = BerTlv::StrToBerTlv(refData);
        std::shared_ptr<RefDo> refDo = RefDo::BerTlvToRefDo(refBt);
        std::string arData = AR_DO_DATA_ONLY_APDU_NEVER;
        std::shared_ptr<BerTlv> arBt = BerTlv::StrToBerTlv(arData);
        std::shared_ptr<ArDo> arDo = ArDo::BerTlvToArDo(arBt);
        accessRuleCache_->AddAccessRule(refDo, arDo);

        std::vector<std::string> hashes1;
        hashes1.push_back(HASH_1);
        CheckChannelAccessRule(AID_HASH_ALL, hashes1, ApduNeverNfcNever);

        std::vector<std::string> hashes2;
        hashes2.push_back(HASH_2);
        CheckChannelAccessRule(AID_HASH_ALL, hashes2, ApduNeverNfcNever);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : NFC_TC_SE_AccessControlTest_002_002
 * @tc.name      : AccessControlTest_002_002
 * @tc.desc      : Access control test
 */
TEST_F(NfcTcSeTest, AccessControlTest_002_002)
{
    try {
        // R0:AID.#1(All-Ref)- AR-APDU-NEVER
        // R1:AID.#2(All-Ref)- AR-APDU-NEVER
        std::string refData = REF_DO_HASH_ALL;
        std::shared_ptr<BerTlv> refBt = BerTlv::StrToBerTlv(refData);
        std::shared_ptr<RefDo> refDo = RefDo::BerTlvToRefDo(refBt);
        std::string refData2 = REF_DO_HASH_ALL_2;
        std::shared_ptr<BerTlv> refBt2 = BerTlv::StrToBerTlv(refData2);
        std::shared_ptr<RefDo> refDo2 = RefDo::BerTlvToRefDo(refBt2);
        std::string arData = AR_DO_DATA_ONLY_APDU_NEVER;
        std::shared_ptr<BerTlv> arBt = BerTlv::StrToBerTlv(arData);
        std::shared_ptr<ArDo> arDo = ArDo::BerTlvToArDo(arBt);
        accessRuleCache_->AddAccessRule(refDo, arDo);
        accessRuleCache_->AddAccessRule(refDo2, arDo);

        std::vector<std::string> hashes1;
        hashes1.push_back(HASH_1);
        CheckChannelAccessRule(AID_HASH_ALL, hashes1, ApduNeverNfcNever);
        CheckChannelAccessRule(AID_HASH_ALL_2, hashes1, ApduNeverNfcNever);

        std::vector<std::string> hashes2;
        hashes2.push_back(HASH_2);
        CheckChannelAccessRule(AID_HASH_ALL, hashes2, ApduNeverNfcNever);
        CheckChannelAccessRule(AID_HASH_ALL_2, hashes2, ApduNeverNfcNever);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : NFC_TC_SE_AccessControlTest_002_003
 * @tc.name      : AccessControlTest_002_003
 * @tc.desc      : Access control test
 */
TEST_F(NfcTcSeTest, AccessControlTest_002_003)
{
    try {
        // R0:AID.#All (All-Ref)- AR-APDU-NEVER
        std::string refData = REF_DO_AID_ALL_HASH_ALL;
        std::shared_ptr<BerTlv> refBt = BerTlv::StrToBerTlv(refData);
        std::shared_ptr<RefDo> refDo = RefDo::BerTlvToRefDo(refBt);
        std::string arData = AR_DO_DATA_ONLY_APDU_NEVER;
        std::shared_ptr<BerTlv> arBt = BerTlv::StrToBerTlv(arData);
        std::shared_ptr<ArDo> arDo = ArDo::BerTlvToArDo(arBt);
        accessRuleCache_->AddAccessRule(refDo, arDo);

        std::vector<std::string> hashes1;
        hashes1.push_back(HASH_1);
        CheckChannelAccessRule(AID_1, hashes1, ApduNeverNfcNever);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : NFC_TC_SE_AccessControlTest_003_001
 * @tc.name      : AccessControlTest_003_001
 * @tc.desc      : Access control test
 */
TEST_F(NfcTcSeTest, AccessControlTest_003_001)
{
    try {
        // R0:AID.#1 (All-Ref)- AR-APDU-Filter(APDU1)
        std::string refData = REF_DO_HASH_ALL;
        std::shared_ptr<BerTlv> refBt = BerTlv::StrToBerTlv(refData);
        std::shared_ptr<RefDo> refDo = RefDo::BerTlvToRefDo(refBt);
        std::string arData = AR_DO_DATA_ONLY_APDU_FILTER;
        std::shared_ptr<BerTlv> arBt = BerTlv::StrToBerTlv(arData);
        std::shared_ptr<ArDo> arDo = ArDo::BerTlvToArDo(arBt);
        accessRuleCache_->AddAccessRule(refDo, arDo);

        std::vector<std::string> hashes1;
        hashes1.push_back(HASH_1);
        CheckChannelAccessRule(AID_HASH_ALL, hashes1, ApduFilter1);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : NFC_TC_SE_AccessControlTest_003_002
 * @tc.name      : AccessControlTest_003_002
 * @tc.desc      : Access control test
 */
TEST_F(NfcTcSeTest, AccessControlTest_003_002)
{
    try {
        // R0:AID.#1 (All-Ref)- AR-APDU-Filter (APDU1,APDU2)
        std::string refData = REF_DO_HASH_ALL;
        std::shared_ptr<BerTlv> refBt = BerTlv::StrToBerTlv(refData);
        std::shared_ptr<RefDo> refDo = RefDo::BerTlvToRefDo(refBt);
        std::string arData = AR_DO_DATA_ONLY_APDU_FILTER_2;
        std::shared_ptr<BerTlv> arBt = BerTlv::StrToBerTlv(arData);
        std::shared_ptr<ArDo> arDo = ArDo::BerTlvToArDo(arBt);
        accessRuleCache_->AddAccessRule(refDo, arDo);

        std::vector<std::string> hashes1;
        hashes1.push_back(HASH_1);
        CheckChannelAccessRule(AID_HASH_ALL, hashes1, ApduFilter23);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : NFC_TC_SE_AccessControlTest_003_003
 * @tc.name      : AccessControlTest_003_003
 * @tc.desc      : Access control test
 */
TEST_F(NfcTcSeTest, AccessControlTest_003_003)
{
    try {
        // R0:AID.#All (All-Ref)- AR-APDU-Filter(APDU1)
        std::string refData = REF_DO_AID_ALL_HASH_ALL;
        std::shared_ptr<BerTlv> refBt = BerTlv::StrToBerTlv(refData);
        std::shared_ptr<RefDo> refDo = RefDo::BerTlvToRefDo(refBt);
        std::string arData = AR_DO_DATA_ONLY_APDU_FILTER;
        std::shared_ptr<BerTlv> arBt = BerTlv::StrToBerTlv(arData);
        std::shared_ptr<ArDo> arDo = ArDo::BerTlvToArDo(arBt);
        accessRuleCache_->AddAccessRule(refDo, arDo);

        std::vector<std::string> hashes1;
        hashes1.push_back(HASH_1);
        CheckChannelAccessRule(AID_2, hashes1, ApduFilter1);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : NFC_TC_SE_AccessControlTest_003_004
 * @tc.name      : AccessControlTest_003_004
 * @tc.desc      : Access control test
 */
TEST_F(NfcTcSeTest, AccessControlTest_003_004)
{
    try {
        // R0:AID.#All (All-Ref)- AR-APDU-Filter(APDU1,APDU2)
        std::string refData = REF_DO_AID_ALL_HASH_ALL;
        std::shared_ptr<BerTlv> refBt = BerTlv::StrToBerTlv(refData);
        std::shared_ptr<RefDo> refDo = RefDo::BerTlvToRefDo(refBt);
        std::string arData = AR_DO_DATA_ONLY_APDU_FILTER_2;
        std::shared_ptr<BerTlv> arBt = BerTlv::StrToBerTlv(arData);
        std::shared_ptr<ArDo> arDo = ArDo::BerTlvToArDo(arBt);
        accessRuleCache_->AddAccessRule(refDo, arDo);

        std::vector<std::string> hashes1;
        hashes1.push_back(HASH_1);
        CheckChannelAccessRule(AID_2, hashes1, ApduFilter23);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : NFC_TC_SE_AccessControlTest_004_001
 * @tc.name      : AccessControlTest_004_001
 * @tc.desc      : Access control test
 */
TEST_F(NfcTcSeTest, AccessControlTest_004_001)
{
    try {
        // R0:AID.#1 (CER#1-Ref)- AR-APDU-ALWAYS
        std::string refData = REF_DO_A1_H1;
        std::shared_ptr<BerTlv> refBt = BerTlv::StrToBerTlv(refData);
        std::shared_ptr<RefDo> refDo = RefDo::BerTlvToRefDo(refBt);
        std::string arData = AR_DO_DATA_ONLY_APDU_ALWAYS;
        std::shared_ptr<BerTlv> arBt = BerTlv::StrToBerTlv(arData);
        std::shared_ptr<ArDo> arDo = ArDo::BerTlvToArDo(arBt);
        accessRuleCache_->AddAccessRule(refDo, arDo);

        std::vector<std::string> hashes1;
        hashes1.push_back(HASH_1);
        CheckChannelAccessRule(AID_1, hashes1, ApduAlwaysNfcAlways);

        std::vector<std::string> hashes2;
        CheckChannelAccessRule(AID_1, hashes2, ApduNeverNfcNever);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : NFC_TC_SE_AccessControlTest_004_002
 * @tc.name      : AccessControlTest_004_002
 * @tc.desc      : Access control test
 */
TEST_F(NfcTcSeTest, AccessControlTest_004_002)
{
    try {
        // R0:AID.#1 (CER#1-Ref)- AR-APDU-ALWAYS
        // R1:AID.#2 (CER#2-Ref)- AR-APDU-ALWAYS
        std::string refData = REF_DO_A1_H1;
        std::shared_ptr<BerTlv> refBt = BerTlv::StrToBerTlv(refData);
        std::shared_ptr<RefDo> refDo = RefDo::BerTlvToRefDo(refBt);
        std::string refData2 = REF_DO_A3_H3;
        std::shared_ptr<BerTlv> refBt2 = BerTlv::StrToBerTlv(refData2);
        std::shared_ptr<RefDo> refDo2 = RefDo::BerTlvToRefDo(refBt2);
        std::string arData = AR_DO_DATA_ONLY_APDU_ALWAYS;
        std::shared_ptr<BerTlv> arBt = BerTlv::StrToBerTlv(arData);
        std::shared_ptr<ArDo> arDo = ArDo::BerTlvToArDo(arBt);
        accessRuleCache_->AddAccessRule(refDo, arDo);
        accessRuleCache_->AddAccessRule(refDo2, arDo);

        std::vector<std::string> hashes1;
        hashes1.push_back(HASH_1);
        hashes1.push_back(HASH_3);
        CheckChannelAccessRule(AID_1, hashes1, ApduAlwaysNfcAlways);
        CheckChannelAccessRule(AID_3, hashes1, ApduAlwaysNfcAlways);

        std::vector<std::string> hashes2;
        hashes2.push_back(HASH_3);
        CheckChannelAccessRule(AID_3, hashes2, ApduAlwaysNfcAlways);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : NFC_TC_SE_AccessControlTest_004_003
 * @tc.name      : AccessControlTest_004_003
 * @tc.desc      : Access control test
 */
TEST_F(NfcTcSeTest, AccessControlTest_004_003)
{
    try {
        // R0:AID.#1 (CER#1-Ref)- AR-APDU-ALWAYS
        // R1:AID.#1 (CER#2-Ref)- AR-APDU-ALWAYS
        std::string refData = REF_DO_A1_H1;
        std::shared_ptr<BerTlv> refBt = BerTlv::StrToBerTlv(refData);
        std::shared_ptr<RefDo> refDo = RefDo::BerTlvToRefDo(refBt);
        std::string refData2 = REF_DO_A1_H2;
        std::shared_ptr<BerTlv> refBt2 = BerTlv::StrToBerTlv(refData2);
        std::shared_ptr<RefDo> refDo2 = RefDo::BerTlvToRefDo(refBt2);
        std::string arData = AR_DO_DATA_ONLY_APDU_ALWAYS;
        std::shared_ptr<BerTlv> arBt = BerTlv::StrToBerTlv(arData);
        std::shared_ptr<ArDo> arDo = ArDo::BerTlvToArDo(arBt);
        accessRuleCache_->AddAccessRule(refDo, arDo);
        accessRuleCache_->AddAccessRule(refDo2, arDo);

        std::vector<std::string> hashes1;
        hashes1.push_back(HASH_1);
        CheckChannelAccessRule(AID_1, hashes1, ApduAlwaysNfcAlways);

        std::vector<std::string> hashes2;
        hashes2.push_back(HASH_2);
        CheckChannelAccessRule(AID_1, hashes2, ApduAlwaysNfcAlways);

        std::vector<std::string> hashes3;
        CheckChannelAccessRule(AID_1, hashes3, ApduNeverNfcNever);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : NFC_TC_SE_AccessControlTest_004_004
 * @tc.name      : AccessControlTest_004_004
 * @tc.desc      : Access control test
 */
TEST_F(NfcTcSeTest, AccessControlTest_004_004)
{
    try {
        // R0:AID.#1 (CER#1-Ref)- AR-APDU-ALWAYS
        // R1:AID.#2 (CER#1-Ref)- AR-APDU-ALWAYS
        std::string refData = REF_DO_A1_H1;
        std::shared_ptr<BerTlv> refBt = BerTlv::StrToBerTlv(refData);
        std::shared_ptr<RefDo> refDo = RefDo::BerTlvToRefDo(refBt);
        std::string refData2 = REF_DO_A2_H1;
        std::shared_ptr<BerTlv> refBt2 = BerTlv::StrToBerTlv(refData2);
        std::shared_ptr<RefDo> refDo2 = RefDo::BerTlvToRefDo(refBt2);
        std::string arData = AR_DO_DATA_ONLY_APDU_ALWAYS;
        std::shared_ptr<BerTlv> arBt = BerTlv::StrToBerTlv(arData);
        std::shared_ptr<ArDo> arDo = ArDo::BerTlvToArDo(arBt);
        accessRuleCache_->AddAccessRule(refDo, arDo);
        accessRuleCache_->AddAccessRule(refDo2, arDo);

        std::vector<std::string> hashes1;
        hashes1.push_back(HASH_1);
        CheckChannelAccessRule(AID_1, hashes1, ApduAlwaysNfcAlways);
        CheckChannelAccessRule(AID_2, hashes1, ApduAlwaysNfcAlways);

        std::vector<std::string> hashes2;
        hashes2.push_back(HASH_1);
        CheckChannelAccessRule(AID_1, hashes2, ApduAlwaysNfcAlways);
        CheckChannelAccessRule(AID_2, hashes2, ApduAlwaysNfcAlways);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : NFC_TC_SE_AccessControlTest_005_001
 * @tc.name      : AccessControlTest_005_001
 * @tc.desc      : Access control test
 */
TEST_F(NfcTcSeTest, AccessControlTest_005_001)
{
    try {
        // R0:AID.#1 (CER#1-Ref)- AR-APDU-NEVER
        std::string refData = REF_DO_A1_H1;
        std::shared_ptr<BerTlv> refBt = BerTlv::StrToBerTlv(refData);
        std::shared_ptr<RefDo> refDo = RefDo::BerTlvToRefDo(refBt);
        std::string arData = AR_DO_DATA_ONLY_APDU_NEVER;
        std::shared_ptr<BerTlv> arBt = BerTlv::StrToBerTlv(arData);
        std::shared_ptr<ArDo> arDo = ArDo::BerTlvToArDo(arBt);
        accessRuleCache_->AddAccessRule(refDo, arDo);

        std::vector<std::string> hashes1;
        hashes1.push_back(HASH_1);
        CheckChannelAccessRule(AID_1, hashes1, ApduNeverNfcNever);

        std::vector<std::string> hashes2;
        CheckChannelAccessRule(AID_1, hashes2, ApduNeverNfcNever);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : NFC_TC_SE_AccessControlTest_005_002
 * @tc.name      : AccessControlTest_005_002
 * @tc.desc      : Access control test
 */
TEST_F(NfcTcSeTest, AccessControlTest_005_002)
{
    try {
        // R0:AID.#1 (CER#1-Ref)- AR-APDU-NEVER
        // R1:AID.#2 (CER#2-Ref)- AR-APDU-NEVER
        std::string refData = REF_DO_A1_H1;
        std::shared_ptr<BerTlv> refBt = BerTlv::StrToBerTlv(refData);
        std::shared_ptr<RefDo> refDo = RefDo::BerTlvToRefDo(refBt);
        std::string refData2 = REF_DO_A3_H3;
        std::shared_ptr<BerTlv> refBt2 = BerTlv::StrToBerTlv(refData2);
        std::shared_ptr<RefDo> refDo2 = RefDo::BerTlvToRefDo(refBt2);
        std::string arData = AR_DO_DATA_ONLY_APDU_NEVER;
        std::shared_ptr<BerTlv> arBt = BerTlv::StrToBerTlv(arData);
        std::shared_ptr<ArDo> arDo = ArDo::BerTlvToArDo(arBt);
        accessRuleCache_->AddAccessRule(refDo, arDo);
        accessRuleCache_->AddAccessRule(refDo2, arDo);

        std::vector<std::string> hashes1;
        hashes1.push_back(HASH_1);
        hashes1.push_back(HASH_3);
        CheckChannelAccessRule(AID_1, hashes1, ApduNeverNfcNever);
        CheckChannelAccessRule(AID_3, hashes1, ApduNeverNfcNever);

        std::vector<std::string> hashes2;
        hashes2.push_back(HASH_3);
        CheckChannelAccessRule(AID_3, hashes2, ApduNeverNfcNever);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : NFC_TC_SE_AccessControlTest_005_003
 * @tc.name      : AccessControlTest_005_003
 * @tc.desc      : Access control test
 */
TEST_F(NfcTcSeTest, AccessControlTest_005_003)
{
    try {
        // R0:AID.#1 (CER#1-Ref)- AR-APDU-NEVER
        // R1:AID.#1 (CER#2-Ref)- AR-APDU-NEVER
        std::string refData = REF_DO_A1_H1;
        std::shared_ptr<BerTlv> refBt = BerTlv::StrToBerTlv(refData);
        std::shared_ptr<RefDo> refDo = RefDo::BerTlvToRefDo(refBt);
        std::string refData2 = REF_DO_A1_H2;
        std::shared_ptr<BerTlv> refBt2 = BerTlv::StrToBerTlv(refData2);
        std::shared_ptr<RefDo> refDo2 = RefDo::BerTlvToRefDo(refBt2);
        std::string arData = AR_DO_DATA_ONLY_APDU_NEVER;
        std::shared_ptr<BerTlv> arBt = BerTlv::StrToBerTlv(arData);
        std::shared_ptr<ArDo> arDo = ArDo::BerTlvToArDo(arBt);
        accessRuleCache_->AddAccessRule(refDo, arDo);
        accessRuleCache_->AddAccessRule(refDo2, arDo);

        std::vector<std::string> hashes1;
        hashes1.push_back(HASH_1);
        CheckChannelAccessRule(AID_1, hashes1, ApduNeverNfcNever);

        std::vector<std::string> hashes2;
        hashes2.push_back(HASH_2);
        CheckChannelAccessRule(AID_1, hashes2, ApduNeverNfcNever);

        std::vector<std::string> hashes3;
        CheckChannelAccessRule(AID_1, hashes3, ApduNeverNfcNever);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : NFC_TC_SE_AccessControlTest_005_004
 * @tc.name      : AccessControlTest_005_004
 * @tc.desc      : Access control test
 */
TEST_F(NfcTcSeTest, AccessControlTest_005_004)
{
    try {
        // R0:AID.#1 (CER#1-Ref)- AR-APDU-NEVER
        // R1:AID.#2 (CER#1-Ref)- AR-APDU-NEVER
        std::string refData = REF_DO_A1_H1;
        std::shared_ptr<BerTlv> refBt = BerTlv::StrToBerTlv(refData);
        std::shared_ptr<RefDo> refDo = RefDo::BerTlvToRefDo(refBt);
        std::string refData2 = REF_DO_A2_H1;
        std::shared_ptr<BerTlv> refBt2 = BerTlv::StrToBerTlv(refData2);
        std::shared_ptr<RefDo> refDo2 = RefDo::BerTlvToRefDo(refBt2);
        std::string arData = AR_DO_DATA_ONLY_APDU_NEVER;
        std::shared_ptr<BerTlv> arBt = BerTlv::StrToBerTlv(arData);
        std::shared_ptr<ArDo> arDo = ArDo::BerTlvToArDo(arBt);
        accessRuleCache_->AddAccessRule(refDo, arDo);
        accessRuleCache_->AddAccessRule(refDo2, arDo);

        std::vector<std::string> hashes1;
        hashes1.push_back(HASH_1);
        CheckChannelAccessRule(AID_1, hashes1, ApduNeverNfcNever);
        CheckChannelAccessRule(AID_2, hashes1, ApduNeverNfcNever);

        std::vector<std::string> hashes2;
        hashes2.push_back(HASH_2);
        CheckChannelAccessRule(AID_1, hashes2, ApduNeverNfcNever);
        CheckChannelAccessRule(AID_2, hashes2, ApduNeverNfcNever);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : NFC_TC_SE_AccessControlTest_006_001
 * @tc.name      : AccessControlTest_006_001
 * @tc.desc      : Access control test
 */
TEST_F(NfcTcSeTest, AccessControlTest_006_001)
{
    try {
        // R0:AID.#1 (All-Ref)- AR-NFC-ALWAYS
        std::string refData = REF_DO_HASH_ALL;
        std::shared_ptr<BerTlv> refBt = BerTlv::StrToBerTlv(refData);
        std::shared_ptr<RefDo> refDo = RefDo::BerTlvToRefDo(refBt);
        std::string arData = AR_DO_DATA_ONLY_NFC_ALWAYS;
        std::shared_ptr<BerTlv> arBt = BerTlv::StrToBerTlv(arData);
        std::shared_ptr<ArDo> arDo = ArDo::BerTlvToArDo(arBt);
        accessRuleCache_->AddAccessRule(refDo, arDo);

        std::vector<std::string> hashes1;
        hashes1.push_back(HASH_1);
        CheckChannelAccessRule(AID_HASH_ALL, hashes1, ApduNeverNfcAlways);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : NFC_TC_SE_AccessControlTest_006_002
 * @tc.name      : AccessControlTest_006_002
 * @tc.desc      : Access control test
 */
TEST_F(NfcTcSeTest, AccessControlTest_006_002)
{
    try {
        // R0:AID.#1 (All-Ref)- AR-NFC-Never
        std::string refData = REF_DO_HASH_ALL;
        std::shared_ptr<BerTlv> refBt = BerTlv::StrToBerTlv(refData);
        std::shared_ptr<RefDo> refDo = RefDo::BerTlvToRefDo(refBt);
        std::string arData = AR_DO_DATA_ONLY_NFC_NEVER;
        std::shared_ptr<BerTlv> arBt = BerTlv::StrToBerTlv(arData);
        std::shared_ptr<ArDo> arDo = ArDo::BerTlvToArDo(arBt);
        accessRuleCache_->AddAccessRule(refDo, arDo);

        std::vector<std::string> hashes1;
        hashes1.push_back(HASH_1);
        CheckChannelAccessRule(AID_HASH_ALL, hashes1, ApduNeverNfcNever);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : NFC_TC_SE_AccessControlTest_006_003
 * @tc.name      : AccessControlTest_006_003
 * @tc.desc      : Access control test
 */
TEST_F(NfcTcSeTest, AccessControlTest_006_003)
{
    try {
        // R0:AID.#1 (All-Ref)- AR-APDU-ALWAYS-NFC-ALWAYS
        std::string refData = REF_DO_HASH_ALL;
        std::shared_ptr<BerTlv> refBt = BerTlv::StrToBerTlv(refData);
        std::shared_ptr<RefDo> refDo = RefDo::BerTlvToRefDo(refBt);
        std::string arData = AR_DO_DATA_11;
        std::shared_ptr<BerTlv> arBt = BerTlv::StrToBerTlv(arData);
        std::shared_ptr<ArDo> arDo = ArDo::BerTlvToArDo(arBt);
        accessRuleCache_->AddAccessRule(refDo, arDo);

        std::vector<std::string> hashes1;
        hashes1.push_back(HASH_1);
        CheckChannelAccessRule(AID_HASH_ALL, hashes1, ApduAlwaysNfcAlways);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : NFC_TC_SE_AccessControlTest_006_004
 * @tc.name      : AccessControlTest_006_004
 * @tc.desc      : Access control test
 */
TEST_F(NfcTcSeTest, AccessControlTest_006_004)
{
    try {
        // R0:AID.#1 (All-Ref)- AR-APDU-ALWAYS-NFC-Never
        std::string refData = REF_DO_HASH_ALL;
        std::shared_ptr<BerTlv> refBt = BerTlv::StrToBerTlv(refData);
        std::shared_ptr<RefDo> refDo = RefDo::BerTlvToRefDo(refBt);
        std::string arData = AR_DO_DATA_10;
        std::shared_ptr<BerTlv> arBt = BerTlv::StrToBerTlv(arData);
        std::shared_ptr<ArDo> arDo = ArDo::BerTlvToArDo(arBt);
        accessRuleCache_->AddAccessRule(refDo, arDo);

        std::vector<std::string> hashes1;
        hashes1.push_back(HASH_1);
        CheckChannelAccessRule(AID_HASH_ALL, hashes1, ApduAlwaysNfcNever);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : NFC_TC_SE_AccessControlTest_006_005
 * @tc.name      : AccessControlTest_006_005
 * @tc.desc      : Access control test
 */
TEST_F(NfcTcSeTest, AccessControlTest_006_005)
{
    try {
        // R0:AID.#1 (All-Ref)- AR-APDU-NEVER-NFC-ALWAYS
        std::string refData = REF_DO_HASH_ALL;
        std::shared_ptr<BerTlv> refBt = BerTlv::StrToBerTlv(refData);
        std::shared_ptr<RefDo> refDo = RefDo::BerTlvToRefDo(refBt);
        std::string arData = AR_DO_DATA_01;
        std::shared_ptr<BerTlv> arBt = BerTlv::StrToBerTlv(arData);
        std::shared_ptr<ArDo> arDo = ArDo::BerTlvToArDo(arBt);
        accessRuleCache_->AddAccessRule(refDo, arDo);

        std::vector<std::string> hashes1;
        hashes1.push_back(HASH_1);
        CheckChannelAccessRule(AID_HASH_ALL, hashes1, ApduNeverNfcAlways);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : NFC_TC_SE_AccessControlTest_006_006
 * @tc.name      : AccessControlTest_006_006
 * @tc.desc      : Access control test
 */
TEST_F(NfcTcSeTest, AccessControlTest_006_006)
{
    try {
        // R0:AID.#1 (All-Ref)- AR-APDU-NEVER-NFC-Never
        std::string refData = REF_DO_HASH_ALL;
        std::shared_ptr<BerTlv> refBt = BerTlv::StrToBerTlv(refData);
        std::shared_ptr<RefDo> refDo = RefDo::BerTlvToRefDo(refBt);
        std::string arData = AR_DO_DATA_00;
        std::shared_ptr<BerTlv> arBt = BerTlv::StrToBerTlv(arData);
        std::shared_ptr<ArDo> arDo = ArDo::BerTlvToArDo(arBt);
        accessRuleCache_->AddAccessRule(refDo, arDo);

        std::vector<std::string> hashes1;
        hashes1.push_back(HASH_1);
        CheckChannelAccessRule(AID_HASH_ALL, hashes1, ApduNeverNfcNever);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : NFC_TC_SE_AccessRulerTest_001_001
 * @tc.name      : AccessRulerTest_001_001
 * @tc.desc      : Access ruler test
 */
TEST_F(NfcTcSeTest, AccessRulerTest_001_001)
{
    try {
        // R0:AID.#1 (All-Ref)- AR-APDU-ALWAYS
        // R1:AID.#1 (CER#1-Ref)- AR-APDU-ALWAYS
        std::string refData = REF_DO_HASH_ALL;
        std::shared_ptr<BerTlv> refBt = BerTlv::StrToBerTlv(refData);
        std::shared_ptr<RefDo> refDo = RefDo::BerTlvToRefDo(refBt);
        std::string refData2 = REF_DO_HASHALLAID_AIDALLHASH;
        std::shared_ptr<BerTlv> refBt2 = BerTlv::StrToBerTlv(refData2);
        std::shared_ptr<RefDo> refDo2 = RefDo::BerTlvToRefDo(refBt2);
        std::string arData = AR_DO_DATA_ONLY_APDU_ALWAYS;
        std::shared_ptr<BerTlv> arBt = BerTlv::StrToBerTlv(arData);
        std::shared_ptr<ArDo> arDo = ArDo::BerTlvToArDo(arBt);
        accessRuleCache_->AddAccessRule(refDo, arDo);
        accessRuleCache_->AddAccessRule(refDo2, arDo);

        std::vector<std::string> hashes1;
        hashes1.push_back(HASH_AID_ALL);
        CheckChannelAccessRule(AID_HASH_ALL, hashes1, ApduAlwaysNfcAlways);

        std::vector<std::string> hashes2;
        hashes2.push_back(HASH_1);
        CheckChannelAccessRule(AID_HASH_ALL, hashes2, ApduNeverNfcNever);

        std::vector<std::string> hashes3;
        CheckChannelAccessRule(AID_HASH_ALL, hashes3, ApduNeverNfcNever);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : NFC_TC_SE_AccessRulerTest_001_002
 * @tc.name      : AccessRulerTest_001_002
 * @tc.desc      : Access ruler test
 */
TEST_F(NfcTcSeTest, AccessRulerTest_001_002)
{
    try {
        // R0:AID.#1 (CER#1-Ref)- AR-APDU-ALWAYS
        // R1:AID.#2 (CER#1-Ref)- AR-APDU-ALWAYS
        std::string refData = REF_DO_A1_H1;
        std::shared_ptr<BerTlv> refBt = BerTlv::StrToBerTlv(refData);
        std::shared_ptr<RefDo> refDo = RefDo::BerTlvToRefDo(refBt);
        std::string refData2 = REF_DO_A2_H1;
        std::shared_ptr<BerTlv> refBt2 = BerTlv::StrToBerTlv(refData2);
        std::shared_ptr<RefDo> refDo2 = RefDo::BerTlvToRefDo(refBt2);
        std::string arData = AR_DO_DATA_ONLY_APDU_ALWAYS;
        std::shared_ptr<BerTlv> arBt = BerTlv::StrToBerTlv(arData);
        std::shared_ptr<ArDo> arDo = ArDo::BerTlvToArDo(arBt);
        accessRuleCache_->AddAccessRule(refDo, arDo);
        accessRuleCache_->AddAccessRule(refDo2, arDo);

        std::vector<std::string> hashes1;
        hashes1.push_back(HASH_1);
        CheckChannelAccessRule(AID_1, hashes1, ApduAlwaysNfcAlways);
        CheckChannelAccessRule(AID_2, hashes1, ApduAlwaysNfcAlways);

        std::vector<std::string> hashes2;
        hashes2.push_back(HASH_2);
        CheckChannelAccessRule(AID_1, hashes2, ApduNeverNfcNever);
        CheckChannelAccessRule(AID_2, hashes2, ApduNeverNfcNever);

        std::vector<std::string> hashes3;
        CheckChannelAccessRule(AID_1, hashes3, ApduNeverNfcNever);
        CheckChannelAccessRule(AID_2, hashes3, ApduNeverNfcNever);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : NFC_TC_SE_AccessRulerTest_001_003
 * @tc.name      : AccessRulerTest_001_003
 * @tc.desc      : Access ruler test
 */
TEST_F(NfcTcSeTest, AccessRulerTest_001_003)
{
    try {
        // R0:AID.#1 (CER#1-Ref)- AR-APDU-ALWAYS
        // R1:AID.#All (CER#1-Ref)- AR -APDU-ALWAYS
        std::string refData = REF_DO_HASHALLAID_AIDALLHASH;
        std::shared_ptr<BerTlv> refBt = BerTlv::StrToBerTlv(refData);
        std::shared_ptr<RefDo> refDo = RefDo::BerTlvToRefDo(refBt);
        std::string refData2 = REF_DO_AID_ALL;
        std::shared_ptr<BerTlv> refBt2 = BerTlv::StrToBerTlv(refData2);
        std::shared_ptr<RefDo> refDo2 = RefDo::BerTlvToRefDo(refBt2);
        std::string arData = AR_DO_DATA_ONLY_APDU_ALWAYS;
        std::shared_ptr<BerTlv> arBt = BerTlv::StrToBerTlv(arData);
        std::shared_ptr<ArDo> arDo = ArDo::BerTlvToArDo(arBt);
        accessRuleCache_->AddAccessRule(refDo, arDo);
        accessRuleCache_->AddAccessRule(refDo2, arDo);

        std::vector<std::string> hashes1;
        hashes1.push_back(HASH_AID_ALL);
        CheckChannelAccessRule(AID_HASH_ALL, hashes1, ApduAlwaysNfcAlways);
        CheckChannelAccessRule(AID_1, hashes1, ApduAlwaysNfcAlways);

        std::vector<std::string> hashes2;
        hashes2.push_back(HASH_1);
        CheckChannelAccessRule(AID_HASH_ALL, hashes2, ApduNeverNfcNever);
        CheckChannelAccessRule(AID_1, hashes2, ApduNeverNfcNever);

        std::vector<std::string> hashes3;
        CheckChannelAccessRule(AID_HASH_ALL, hashes3, ApduNeverNfcNever);
        CheckChannelAccessRule(AID_1, hashes3, ApduNeverNfcNever);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : NFC_TC_SE_AccessRulerTest_001_004
 * @tc.name      : AccessRulerTest_001_004
 * @tc.desc      : Access ruler test
 */
TEST_F(NfcTcSeTest, AccessRulerTest_001_004)
{
    try {
        // R0:AID.#1 (CER#1-Ref)- AR-APDU-NEVER
        // R1:AID.#1 (CER#1-Ref)- AR-APDU-ALWAYS
        std::string refData = REF_DO_A1_H1;
        std::shared_ptr<BerTlv> refBt = BerTlv::StrToBerTlv(refData);
        std::shared_ptr<RefDo> refDo = RefDo::BerTlvToRefDo(refBt);
        std::string arData = AR_DO_DATA_ONLY_APDU_ALWAYS;
        std::shared_ptr<BerTlv> arBt = BerTlv::StrToBerTlv(arData);
        std::shared_ptr<ArDo> arDo = ArDo::BerTlvToArDo(arBt);
        std::string arData2 = AR_DO_DATA_ONLY_APDU_NEVER;
        std::shared_ptr<BerTlv> arBt2 = BerTlv::StrToBerTlv(arData2);
        std::shared_ptr<ArDo> arDo2 = ArDo::BerTlvToArDo(arBt2);
        accessRuleCache_->AddAccessRule(refDo, arDo);
        accessRuleCache_->AddAccessRule(refDo, arDo2);

        std::vector<std::string> hashes1;
        hashes1.push_back(HASH_1);
        CheckChannelAccessRule(AID_1, hashes1, ApduNeverNfcNever);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : NFC_TC_SE_AccessRulerTest_001_005
 * @tc.name      : AccessRulerTest_001_005
 * @tc.desc      : Access ruler test
 */
TEST_F(NfcTcSeTest, AccessRulerTest_001_005)
{
    try {
        // R0:AID.#1 (CER#1-Ref)- AR-APDU-NEVER
        // R1:AID.#1 (CER#1-Ref)- AR-APDU-Filter(APDU1,APDU2)
        std::string refData = REF_DO_A1_H1;
        std::shared_ptr<BerTlv> refBt = BerTlv::StrToBerTlv(refData);
        std::shared_ptr<RefDo> refDo = RefDo::BerTlvToRefDo(refBt);
        std::string arData = AR_DO_DATA_ONLY_APDU_FILTER_2;
        std::shared_ptr<BerTlv> arBt = BerTlv::StrToBerTlv(arData);
        std::shared_ptr<ArDo> arDo = ArDo::BerTlvToArDo(arBt);
        std::string arData2 = AR_DO_DATA_ONLY_APDU_NEVER;
        std::shared_ptr<BerTlv> arBt2 = BerTlv::StrToBerTlv(arData2);
        std::shared_ptr<ArDo> arDo2 = ArDo::BerTlvToArDo(arBt2);
        accessRuleCache_->AddAccessRule(refDo, arDo);
        accessRuleCache_->AddAccessRule(refDo, arDo2);

        std::vector<std::string> hashes1;
        hashes1.push_back(HASH_1);
        CheckChannelAccessRule(AID_1, hashes1, ApduNeverNfcNever);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : NFC_TC_SE_AccessRulerTest_001_006
 * @tc.name      : AccessRulerTest_001_006
 * @tc.desc      : Access ruler test
 */
TEST_F(NfcTcSeTest, AccessRulerTest_001_006)
{
    try {
        // R0:AID.#1 (CER#1-Ref)- AR-APDU-Filter(APDU1,APDU2)
        // R1:AID.#1 (CER#1-Ref)- AR-APDU-ALWAYS
        std::string refData = REF_DO_A1_H1;
        std::shared_ptr<BerTlv> refBt = BerTlv::StrToBerTlv(refData);
        std::shared_ptr<RefDo> refDo = RefDo::BerTlvToRefDo(refBt);
        std::string arData = AR_DO_DATA_ONLY_APDU_ALWAYS;
        std::shared_ptr<BerTlv> arBt = BerTlv::StrToBerTlv(arData);
        std::shared_ptr<ArDo> arDo = ArDo::BerTlvToArDo(arBt);
        std::string arData2 = AR_DO_DATA_ONLY_APDU_FILTER_2;
        std::shared_ptr<BerTlv> arBt2 = BerTlv::StrToBerTlv(arData2);
        std::shared_ptr<ArDo> arDo2 = ArDo::BerTlvToArDo(arBt2);
        accessRuleCache_->AddAccessRule(refDo, arDo);
        accessRuleCache_->AddAccessRule(refDo, arDo2);

        std::vector<std::string> hashes1;
        hashes1.push_back(HASH_1);
        CheckChannelAccessRule(AID_1, hashes1, ApduFilter23);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : NFC_TC_SE_AccessRulerTest_001_007
 * @tc.name      : AccessRulerTest_001_007
 * @tc.desc      : Access ruler test
 */
TEST_F(NfcTcSeTest, AccessRulerTest_001_007)
{
    try {
        // R0:AID.#1 (CER#1-Ref)- AR-NFC-NEVER
        // R1:AID.#1 (CER#1-Ref)- AR-NFC-ALWAYS
        std::string refData = REF_DO_A1_H1;
        std::shared_ptr<BerTlv> refBt = BerTlv::StrToBerTlv(refData);
        std::shared_ptr<RefDo> refDo = RefDo::BerTlvToRefDo(refBt);
        std::string arData = AR_DO_DATA_ONLY_NFC_ALWAYS;
        std::shared_ptr<BerTlv> arBt = BerTlv::StrToBerTlv(arData);
        std::shared_ptr<ArDo> arDo = ArDo::BerTlvToArDo(arBt);
        std::string arData2 = AR_DO_DATA_ONLY_NFC_NEVER;
        std::shared_ptr<BerTlv> arBt2 = BerTlv::StrToBerTlv(arData2);
        std::shared_ptr<ArDo> arDo2 = ArDo::BerTlvToArDo(arBt2);
        accessRuleCache_->AddAccessRule(refDo, arDo);
        accessRuleCache_->AddAccessRule(refDo, arDo2);

        std::vector<std::string> hashes1;
        hashes1.push_back(HASH_1);
        CheckChannelAccessRule(AID_1, hashes1, ApduNeverNfcNever);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : NFC_TC_SE_AccessRulerTest_002_001
 * @tc.name      : AccessRulerTest_002_001
 * @tc.desc      : Access ruler test
 */
TEST_F(NfcTcSeTest, AccessRulerTest_002_001)
{
    try {
        // R0:AID.#1 (CER#1-Ref)- AR-APDU-ALWAYS
        // R1:AID.#ALL (ALL-Ref)- AR -APDU-ALWAYS
        std::string refData = REF_DO_A1_H1;
        std::shared_ptr<BerTlv> refBt = BerTlv::StrToBerTlv(refData);
        std::shared_ptr<RefDo> refDo = RefDo::BerTlvToRefDo(refBt);
        std::string refData2 = REF_DO_AID_ALL_HASH_ALL;
        std::shared_ptr<BerTlv> refBt2 = BerTlv::StrToBerTlv(refData2);
        std::shared_ptr<RefDo> refDo2 = RefDo::BerTlvToRefDo(refBt2);
        std::string arData = AR_DO_DATA_ONLY_APDU_ALWAYS;
        std::shared_ptr<BerTlv> arBt = BerTlv::StrToBerTlv(arData);
        std::shared_ptr<ArDo> arDo = ArDo::BerTlvToArDo(arBt);
        accessRuleCache_->AddAccessRule(refDo, arDo);
        accessRuleCache_->AddAccessRule(refDo2, arDo);

        std::vector<std::string> hashes1;
        hashes1.push_back(HASH_1);
        CheckChannelAccessRule(AID_1, hashes1, ApduAlwaysNfcAlways);
        CheckChannelAccessRule(AID_2, hashes1, ApduAlwaysNfcAlways);

        std::vector<std::string> hashes2;
        hashes2.push_back(HASH_2);
        CheckChannelAccessRule(AID_1, hashes2, ApduNeverNfcNever);
        CheckChannelAccessRule(AID_2, hashes2, ApduAlwaysNfcAlways);

        std::vector<std::string> hashes3;
        CheckChannelAccessRule(AID_1, hashes3, ApduNeverNfcNever);
        CheckChannelAccessRule(AID_2, hashes3, ApduNeverNfcNever);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : NFC_TC_SE_AccessRulerTest_002_002
 * @tc.name      : AccessRulerTest_002_002
 * @tc.desc      : Access ruler test
 */
TEST_F(NfcTcSeTest, AccessRulerTest_002_002)
{
    try {
        // R0:AID.#1 (ALL-Ref)- AR -APDU-ALWAYS
        // R1:AID.#ALL (CER#1-Ref)- AR -APDU-ALWAYS
        std::string refData = REF_DO_HASH_ALL;
        std::shared_ptr<BerTlv> refBt = BerTlv::StrToBerTlv(refData);
        std::shared_ptr<RefDo> refDo = RefDo::BerTlvToRefDo(refBt);
        std::string refData2 = REF_DO_AID_ALL;
        std::shared_ptr<BerTlv> refBt2 = BerTlv::StrToBerTlv(refData2);
        std::shared_ptr<RefDo> refDo2 = RefDo::BerTlvToRefDo(refBt2);
        std::string arData = AR_DO_DATA_ONLY_APDU_ALWAYS;
        std::shared_ptr<BerTlv> arBt = BerTlv::StrToBerTlv(arData);
        std::shared_ptr<ArDo> arDo = ArDo::BerTlvToArDo(arBt);
        accessRuleCache_->AddAccessRule(refDo, arDo);
        accessRuleCache_->AddAccessRule(refDo2, arDo);

        std::vector<std::string> hashes1;
        hashes1.push_back(HASH_AID_ALL);
        CheckChannelAccessRule(AID_HASH_ALL, hashes1, ApduAlwaysNfcAlways);
        CheckChannelAccessRule(AID_1, hashes1, ApduAlwaysNfcAlways);

        std::vector<std::string> hashes2;
        hashes2.push_back(HASH_1);
        CheckChannelAccessRule(AID_HASH_ALL, hashes2, ApduAlwaysNfcAlways);
        CheckChannelAccessRule(AID_1, hashes2, ApduNeverNfcNever);

        std::vector<std::string> hashes3;
        CheckChannelAccessRule(AID_HASH_ALL, hashes3, ApduNeverNfcNever);
        CheckChannelAccessRule(AID_1, hashes3, ApduNeverNfcNever);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : NFC_TC_SE_AccessRulerTest_002_003
 * @tc.name      : AccessRulerTest_002_003
 * @tc.desc      : Access ruler test
 */
TEST_F(NfcTcSeTest, AccessRulerTest_002_003)
{
    try {
        // R0:AID.#1 (All-Ref)- AR-APDU-NEVER
        // R1:AID.#1 (CER#1-Ref)- AR-APDU-ALWAYS
        std::string refData = REF_DO_HASH_ALL;
        std::shared_ptr<BerTlv> refBt = BerTlv::StrToBerTlv(refData);
        std::shared_ptr<RefDo> refDo = RefDo::BerTlvToRefDo(refBt);
        std::string refData2 = REF_DO_HASHALLAID_AIDALLHASH;
        std::shared_ptr<BerTlv> refBt2 = BerTlv::StrToBerTlv(refData2);
        std::shared_ptr<RefDo> refDo2 = RefDo::BerTlvToRefDo(refBt2);
        std::string arData = AR_DO_DATA_ONLY_APDU_NEVER;
        std::shared_ptr<BerTlv> arBt = BerTlv::StrToBerTlv(arData);
        std::shared_ptr<ArDo> arDo = ArDo::BerTlvToArDo(arBt);
        std::string arData2 = AR_DO_DATA_ONLY_APDU_ALWAYS;
        std::shared_ptr<BerTlv> arBt2 = BerTlv::StrToBerTlv(arData2);
        std::shared_ptr<ArDo> arDo2 = ArDo::BerTlvToArDo(arBt2);
        accessRuleCache_->AddAccessRule(refDo, arDo);
        accessRuleCache_->AddAccessRule(refDo2, arDo2);

        std::vector<std::string> hashes1;
        hashes1.push_back(HASH_AID_ALL);
        CheckChannelAccessRule(AID_HASH_ALL, hashes1, ApduAlwaysNfcAlways);

        std::vector<std::string> hashes2;
        hashes2.push_back(HASH_1);
        CheckChannelAccessRule(AID_HASH_ALL, hashes2, ApduNeverNfcNever);

        std::vector<std::string> hashes3;
        CheckChannelAccessRule(AID_HASH_ALL, hashes3, ApduNeverNfcNever);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : NFC_TC_SE_AccessRulerTest_002_004
 * @tc.name      : AccessRulerTest_002_004
 * @tc.desc      : Access ruler test
 */
TEST_F(NfcTcSeTest, AccessRulerTest_002_004)
{
    try {
        // R0:AID.#1 (All-Ref)- AR-APDU-ALWAYS
        // R1:AID.#1 (CER#1-Ref)- AR-APDU-NEVER
        std::string refData = REF_DO_HASH_ALL;
        std::shared_ptr<BerTlv> refBt = BerTlv::StrToBerTlv(refData);
        std::shared_ptr<RefDo> refDo = RefDo::BerTlvToRefDo(refBt);
        std::string refData2 = REF_DO_HASHALLAID_AIDALLHASH;
        std::shared_ptr<BerTlv> refBt2 = BerTlv::StrToBerTlv(refData2);
        std::shared_ptr<RefDo> refDo2 = RefDo::BerTlvToRefDo(refBt2);
        std::string arData = AR_DO_DATA_ONLY_APDU_ALWAYS;
        std::shared_ptr<BerTlv> arBt = BerTlv::StrToBerTlv(arData);
        std::shared_ptr<ArDo> arDo = ArDo::BerTlvToArDo(arBt);
        std::string arData2 = AR_DO_DATA_ONLY_APDU_NEVER;
        std::shared_ptr<BerTlv> arBt2 = BerTlv::StrToBerTlv(arData2);
        std::shared_ptr<ArDo> arDo2 = ArDo::BerTlvToArDo(arBt2);
        accessRuleCache_->AddAccessRule(refDo, arDo);
        accessRuleCache_->AddAccessRule(refDo2, arDo2);

        std::vector<std::string> hashes1;
        hashes1.push_back(HASH_AID_ALL);
        CheckChannelAccessRule(AID_HASH_ALL, hashes1, ApduNeverNfcNever);

        std::vector<std::string> hashes2;
        hashes2.push_back(HASH_1);
        CheckChannelAccessRule(AID_HASH_ALL, hashes2, ApduNeverNfcNever);

        std::vector<std::string> hashes3;
        CheckChannelAccessRule(AID_HASH_ALL, hashes3, ApduNeverNfcNever);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : NFC_TC_SE_AccessRulerTest_002_005
 * @tc.name      : AccessRulerTest_002_005
 * @tc.desc      : Access ruler test
 */
TEST_F(NfcTcSeTest, AccessRulerTest_002_005)
{
    try {
        // R0:AID.#1 (CER#1-Ref)- AR-APDU-ALWAYS
        // R1:AID.#1 (CER#2-Ref)- AR-APDU-NEVER
        std::string refData = REF_DO_A1_H1;
        std::shared_ptr<BerTlv> refBt = BerTlv::StrToBerTlv(refData);
        std::shared_ptr<RefDo> refDo = RefDo::BerTlvToRefDo(refBt);
        std::string refData2 = REF_DO_A1_H2;
        std::shared_ptr<BerTlv> refBt2 = BerTlv::StrToBerTlv(refData2);
        std::shared_ptr<RefDo> refDo2 = RefDo::BerTlvToRefDo(refBt2);
        std::string arData = AR_DO_DATA_ONLY_APDU_ALWAYS;
        std::shared_ptr<BerTlv> arBt = BerTlv::StrToBerTlv(arData);
        std::shared_ptr<ArDo> arDo = ArDo::BerTlvToArDo(arBt);
        std::string arData2 = AR_DO_DATA_ONLY_APDU_NEVER;
        std::shared_ptr<BerTlv> arBt2 = BerTlv::StrToBerTlv(arData2);
        std::shared_ptr<ArDo> arDo2 = ArDo::BerTlvToArDo(arBt2);
        accessRuleCache_->AddAccessRule(refDo, arDo);
        accessRuleCache_->AddAccessRule(refDo2, arDo2);

        std::vector<std::string> hashes1;
        hashes1.push_back(HASH_1);
        CheckChannelAccessRule(AID_1, hashes1, ApduAlwaysNfcAlways);

        std::vector<std::string> hashes2;
        hashes2.push_back(HASH_2);
        CheckChannelAccessRule(AID_1, hashes2, ApduNeverNfcNever);

        std::vector<std::string> hashes3;
        CheckChannelAccessRule(AID_1, hashes3, ApduNeverNfcNever);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : NFC_TC_SE_AccessRulerTest_002_006
 * @tc.name      : AccessRulerTest_002_006
 * @tc.desc      : Access ruler test
 */
TEST_F(NfcTcSeTest, AccessRulerTest_002_006)
{
    try {
        // R0:AID.#1 (CER#1-Ref)- AR-APDU-Filter(APDU1,APDU2)
        // R1:AID.#1 (ALL-Ref)- AR -APDU-NEVER
        std::string refData = REF_DO_HASHALLAID_AIDALLHASH;
        std::shared_ptr<BerTlv> refBt = BerTlv::StrToBerTlv(refData);
        std::shared_ptr<RefDo> refDo = RefDo::BerTlvToRefDo(refBt);
        std::string refData2 = REF_DO_HASH_ALL;
        std::shared_ptr<BerTlv> refBt2 = BerTlv::StrToBerTlv(refData2);
        std::shared_ptr<RefDo> refDo2 = RefDo::BerTlvToRefDo(refBt2);
        std::string arData = AR_DO_DATA_ONLY_APDU_FILTER_2;
        std::shared_ptr<BerTlv> arBt = BerTlv::StrToBerTlv(arData);
        std::shared_ptr<ArDo> arDo = ArDo::BerTlvToArDo(arBt);
        std::string arData2 = AR_DO_DATA_ONLY_APDU_NEVER;
        std::shared_ptr<BerTlv> arBt2 = BerTlv::StrToBerTlv(arData2);
        std::shared_ptr<ArDo> arDo2 = ArDo::BerTlvToArDo(arBt2);
        accessRuleCache_->AddAccessRule(refDo, arDo);
        accessRuleCache_->AddAccessRule(refDo2, arDo2);

        std::vector<std::string> hashes1;
        hashes1.push_back(HASH_AID_ALL);
        CheckChannelAccessRule(AID_HASH_ALL, hashes1, ApduFilter23);

        std::vector<std::string> hashes2;
        hashes2.push_back(HASH_1);
        CheckChannelAccessRule(AID_HASH_ALL, hashes2, ApduNeverNfcNever);

        std::vector<std::string> hashes3;
        CheckChannelAccessRule(AID_HASH_ALL, hashes3, ApduNeverNfcNever);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : NFC_TC_SE_AccessRulerTest_002_007
 * @tc.name      : AccessRulerTest_002_007
 * @tc.desc      : Access ruler test
 */
TEST_F(NfcTcSeTest, AccessRulerTest_002_007)
{
    try {
        // R0:AID.#1 (CER#1-Ref)- AR-APDU-NEVER
        // R1:AID.#1 (ALL-Ref)- AR -APDU-Filter(APDU1,APDU2)
        std::string refData = REF_DO_HASHALLAID_AIDALLHASH;
        std::shared_ptr<BerTlv> refBt = BerTlv::StrToBerTlv(refData);
        std::shared_ptr<RefDo> refDo = RefDo::BerTlvToRefDo(refBt);
        std::string refData2 = REF_DO_HASH_ALL;
        std::shared_ptr<BerTlv> refBt2 = BerTlv::StrToBerTlv(refData2);
        std::shared_ptr<RefDo> refDo2 = RefDo::BerTlvToRefDo(refBt2);
        std::string arData = AR_DO_DATA_ONLY_APDU_NEVER;
        std::shared_ptr<BerTlv> arBt = BerTlv::StrToBerTlv(arData);
        std::shared_ptr<ArDo> arDo = ArDo::BerTlvToArDo(arBt);
        std::string arData2 = AR_DO_DATA_ONLY_APDU_FILTER_2;
        std::shared_ptr<BerTlv> arBt2 = BerTlv::StrToBerTlv(arData2);
        std::shared_ptr<ArDo> arDo2 = ArDo::BerTlvToArDo(arBt2);
        accessRuleCache_->AddAccessRule(refDo, arDo);
        accessRuleCache_->AddAccessRule(refDo2, arDo2);

        std::vector<std::string> hashes1;
        hashes1.push_back(HASH_AID_ALL);
        CheckChannelAccessRule(AID_HASH_ALL, hashes1, ApduNeverNfcNever);

        std::vector<std::string> hashes2;
        hashes2.push_back(HASH_1);
        CheckChannelAccessRule(AID_HASH_ALL, hashes2, ApduNeverNfcNever);

        std::vector<std::string> hashes3;
        CheckChannelAccessRule(AID_HASH_ALL, hashes3, ApduNeverNfcNever);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : NFC_TC_SE_AccessRulerTest_002_008
 * @tc.name      : AccessRulerTest_002_008
 * @tc.desc      : Access ruler test
 */
TEST_F(NfcTcSeTest, AccessRulerTest_002_008)
{
    try {
        // R0:AID.#1 (CER#1-Ref)- AR-APDU- Filter(APDU3)
        // R1:AID.#1 (ALL-Ref)- AR -APDU- Filter(APDU1,APDU2)
        std::string refData = REF_DO_HASHALLAID_AIDALLHASH;
        std::shared_ptr<BerTlv> refBt = BerTlv::StrToBerTlv(refData);
        std::shared_ptr<RefDo> refDo = RefDo::BerTlvToRefDo(refBt);
        std::string refData2 = REF_DO_HASH_ALL;
        std::shared_ptr<BerTlv> refBt2 = BerTlv::StrToBerTlv(refData2);
        std::shared_ptr<RefDo> refDo2 = RefDo::BerTlvToRefDo(refBt2);
        std::string arData = AR_DO_DATA_ONLY_APDU_FILTER;
        std::shared_ptr<BerTlv> arBt = BerTlv::StrToBerTlv(arData);
        std::shared_ptr<ArDo> arDo = ArDo::BerTlvToArDo(arBt);
        std::string arData2 = AR_DO_DATA_ONLY_APDU_FILTER_2;
        std::shared_ptr<BerTlv> arBt2 = BerTlv::StrToBerTlv(arData2);
        std::shared_ptr<ArDo> arDo2 = ArDo::BerTlvToArDo(arBt2);
        accessRuleCache_->AddAccessRule(refDo, arDo);
        accessRuleCache_->AddAccessRule(refDo2, arDo2);

        std::vector<std::string> hashes1;
        hashes1.push_back(HASH_AID_ALL);
        CheckChannelAccessRule(AID_HASH_ALL, hashes1, ApduFilter1);

        std::vector<std::string> hashes2;
        hashes2.push_back(HASH_1);
        CheckChannelAccessRule(AID_HASH_ALL, hashes2, ApduNeverNfcNever);

        std::vector<std::string> hashes3;
        CheckChannelAccessRule(AID_HASH_ALL, hashes3, ApduNeverNfcNever);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : NFC_TC_SE_AccessRulerTest_002_009
 * @tc.name      : AccessRulerTest_002_009
 * @tc.desc      : Access ruler test
 */
TEST_F(NfcTcSeTest, AccessRulerTest_002_009)
{
    try {
        // R0:AID.#1 (CER#1-Ref)- AR -NFC-ALWAYS
        // R1:AID.#1 (ALL-Ref)- AR -NFC-NEVER
        std::string refData = REF_DO_HASHALLAID_AIDALLHASH;
        std::shared_ptr<BerTlv> refBt = BerTlv::StrToBerTlv(refData);
        std::shared_ptr<RefDo> refDo = RefDo::BerTlvToRefDo(refBt);
        std::string refData2 = REF_DO_HASH_ALL;
        std::shared_ptr<BerTlv> refBt2 = BerTlv::StrToBerTlv(refData2);
        std::shared_ptr<RefDo> refDo2 = RefDo::BerTlvToRefDo(refBt2);
        std::string arData = AR_DO_DATA_ONLY_NFC_ALWAYS;
        std::shared_ptr<BerTlv> arBt = BerTlv::StrToBerTlv(arData);
        std::shared_ptr<ArDo> arDo = ArDo::BerTlvToArDo(arBt);
        std::string arData2 = AR_DO_DATA_ONLY_NFC_NEVER;
        std::shared_ptr<BerTlv> arBt2 = BerTlv::StrToBerTlv(arData2);
        std::shared_ptr<ArDo> arDo2 = ArDo::BerTlvToArDo(arBt2);
        accessRuleCache_->AddAccessRule(refDo, arDo);
        accessRuleCache_->AddAccessRule(refDo2, arDo2);

        std::vector<std::string> hashes1;
        hashes1.push_back(HASH_AID_ALL);
        CheckChannelAccessRule(AID_HASH_ALL, hashes1, NfcAlways);

        std::vector<std::string> hashes2;
        hashes2.push_back(HASH_1);
        CheckChannelAccessRule(AID_HASH_ALL, hashes2, NfcNever);

        std::vector<std::string> hashes3;
        CheckChannelAccessRule(AID_HASH_ALL, hashes3, NfcNever);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : NFC_TC_SE_AccessRulerTest_002_010
 * @tc.name      : AccessRulerTest_002_010
 * @tc.desc      : Access ruler test
 */
TEST_F(NfcTcSeTest, AccessRulerTest_002_010)
{
    try {
        // R0:AID.#1 (CER#1-Ref)- AR -NFC-NEVER
        // R1:AID.#1 (ALL-Ref)- AR -NFC-ALWAYS
        std::string refData = REF_DO_HASHALLAID_AIDALLHASH;
        std::shared_ptr<BerTlv> refBt = BerTlv::StrToBerTlv(refData);
        std::shared_ptr<RefDo> refDo = RefDo::BerTlvToRefDo(refBt);
        std::string refData2 = REF_DO_HASH_ALL;
        std::shared_ptr<BerTlv> refBt2 = BerTlv::StrToBerTlv(refData2);
        std::shared_ptr<RefDo> refDo2 = RefDo::BerTlvToRefDo(refBt2);
        std::string arData = AR_DO_DATA_ONLY_NFC_NEVER;
        std::shared_ptr<BerTlv> arBt = BerTlv::StrToBerTlv(arData);
        std::shared_ptr<ArDo> arDo = ArDo::BerTlvToArDo(arBt);
        std::string arData2 = AR_DO_DATA_ONLY_NFC_ALWAYS;
        std::shared_ptr<BerTlv> arBt2 = BerTlv::StrToBerTlv(arData2);
        std::shared_ptr<ArDo> arDo2 = ArDo::BerTlvToArDo(arBt2);
        accessRuleCache_->AddAccessRule(refDo, arDo);
        accessRuleCache_->AddAccessRule(refDo2, arDo2);

        std::vector<std::string> hashes1;
        hashes1.push_back(HASH_AID_ALL);
        CheckChannelAccessRule(AID_HASH_ALL, hashes1, NfcNever);

        std::vector<std::string> hashes2;
        hashes2.push_back(HASH_1);
        CheckChannelAccessRule(AID_HASH_ALL, hashes2, NfcNever);

        std::vector<std::string> hashes3;
        CheckChannelAccessRule(AID_HASH_ALL, hashes3, NfcNever);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : NFC_TC_SE_AccessRulerTest_002_011
 * @tc.name      : AccessRulerTest_002_011
 * @tc.desc      : Access ruler test
 */
TEST_F(NfcTcSeTest, AccessRulerTest_002_011)
{
    try {
        // R0:AID.#1 (CER#1-Ref)- AR -APDU-ALWAYS
        // R1:AID.#ALL (CER#1-Ref)- AR -APDU-NEVER
        std::string refData = REF_DO_HASHALLAID_AIDALLHASH;
        std::shared_ptr<BerTlv> refBt = BerTlv::StrToBerTlv(refData);
        std::shared_ptr<RefDo> refDo = RefDo::BerTlvToRefDo(refBt);
        std::string refData2 = REF_DO_AID_ALL;
        std::shared_ptr<BerTlv> refBt2 = BerTlv::StrToBerTlv(refData2);
        std::shared_ptr<RefDo> refDo2 = RefDo::BerTlvToRefDo(refBt2);
        std::string arData = AR_DO_DATA_ONLY_APDU_ALWAYS;
        std::shared_ptr<BerTlv> arBt = BerTlv::StrToBerTlv(arData);
        std::shared_ptr<ArDo> arDo = ArDo::BerTlvToArDo(arBt);
        std::string arData2 = AR_DO_DATA_ONLY_APDU_NEVER;
        std::shared_ptr<BerTlv> arBt2 = BerTlv::StrToBerTlv(arData2);
        std::shared_ptr<ArDo> arDo2 = ArDo::BerTlvToArDo(arBt2);
        accessRuleCache_->AddAccessRule(refDo, arDo);
        accessRuleCache_->AddAccessRule(refDo2, arDo2);

        std::vector<std::string> hashes1;
        hashes1.push_back(HASH_AID_ALL);
        CheckChannelAccessRule(AID_HASH_ALL, hashes1, ApduAlwaysNfcAlways);

        std::vector<std::string> hashes2;
        hashes2.push_back(HASH_1);
        CheckChannelAccessRule(AID_HASH_ALL, hashes2, ApduNeverNfcNever);

        std::vector<std::string> hashes3;
        CheckChannelAccessRule(AID_HASH_ALL, hashes3, ApduNeverNfcNever);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : NFC_TC_SE_AccessRulerTest_002_012
 * @tc.name      : AccessRulerTest_002_012
 * @tc.desc      : Access ruler test
 */
TEST_F(NfcTcSeTest, AccessRulerTest_002_012)
{
    try {
        // R0:AID.#1 (CER#1-Ref)- AR -APDU-NEVER
        // R1:AID.#ALL (CER#1-Ref)- AR -APDU-ALWAYS
        std::string refData = REF_DO_HASHALLAID_AIDALLHASH;
        std::shared_ptr<BerTlv> refBt = BerTlv::StrToBerTlv(refData);
        std::shared_ptr<RefDo> refDo = RefDo::BerTlvToRefDo(refBt);
        std::string refData2 = REF_DO_AID_ALL;
        std::shared_ptr<BerTlv> refBt2 = BerTlv::StrToBerTlv(refData2);
        std::shared_ptr<RefDo> refDo2 = RefDo::BerTlvToRefDo(refBt2);
        std::string arData = AR_DO_DATA_ONLY_APDU_NEVER;
        std::shared_ptr<BerTlv> arBt = BerTlv::StrToBerTlv(arData);
        std::shared_ptr<ArDo> arDo = ArDo::BerTlvToArDo(arBt);
        std::string arData2 = AR_DO_DATA_ONLY_APDU_ALWAYS;
        std::shared_ptr<BerTlv> arBt2 = BerTlv::StrToBerTlv(arData2);
        std::shared_ptr<ArDo> arDo2 = ArDo::BerTlvToArDo(arBt2);
        accessRuleCache_->AddAccessRule(refDo, arDo);
        accessRuleCache_->AddAccessRule(refDo2, arDo2);

        std::vector<std::string> hashes1;
        hashes1.push_back(HASH_AID_ALL);
        CheckChannelAccessRule(AID_1, hashes1, ApduAlwaysNfcAlways);
        CheckChannelAccessRule(AID_HASH_ALL, hashes1, ApduNeverNfcNever);

        std::vector<std::string> hashes2;
        hashes2.push_back(HASH_1);
        CheckChannelAccessRule(AID_HASH_ALL, hashes2, ApduNeverNfcNever);

        std::vector<std::string> hashes3;
        CheckChannelAccessRule(AID_HASH_ALL, hashes3, ApduNeverNfcNever);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : NFC_TC_SE_AccessRulerTest_002_013
 * @tc.name      : AccessRulerTest_002_013
 * @tc.desc      : Access ruler test
 */
TEST_F(NfcTcSeTest, AccessRulerTest_002_013)
{
    try {
        // R0:AID.#1 (CER#1-Ref)- AR -APDU-Filter(APDU1,APDU2)
        // R1:AID.#ALL (CER#1-Ref)- AR -APDU-NEVER
        std::string refData = REF_DO_HASHALLAID_AIDALLHASH;
        std::shared_ptr<BerTlv> refBt = BerTlv::StrToBerTlv(refData);
        std::shared_ptr<RefDo> refDo = RefDo::BerTlvToRefDo(refBt);
        std::string refData2 = REF_DO_AID_ALL;
        std::shared_ptr<BerTlv> refBt2 = BerTlv::StrToBerTlv(refData2);
        std::shared_ptr<RefDo> refDo2 = RefDo::BerTlvToRefDo(refBt2);
        std::string arData = AR_DO_DATA_ONLY_APDU_FILTER_2;
        std::shared_ptr<BerTlv> arBt = BerTlv::StrToBerTlv(arData);
        std::shared_ptr<ArDo> arDo = ArDo::BerTlvToArDo(arBt);
        std::string arData2 = AR_DO_DATA_ONLY_APDU_NEVER;
        std::shared_ptr<BerTlv> arBt2 = BerTlv::StrToBerTlv(arData2);
        std::shared_ptr<ArDo> arDo2 = ArDo::BerTlvToArDo(arBt2);
        accessRuleCache_->AddAccessRule(refDo, arDo);
        accessRuleCache_->AddAccessRule(refDo2, arDo2);

        std::vector<std::string> hashes1;
        hashes1.push_back(HASH_AID_ALL);
        CheckChannelAccessRule(AID_1, hashes1, ApduNeverNfcNever);
        CheckChannelAccessRule(AID_HASH_ALL, hashes1, ApduFilter23);

        std::vector<std::string> hashes2;
        hashes2.push_back(HASH_1);
        CheckChannelAccessRule(AID_HASH_ALL, hashes2, ApduNeverNfcNever);

        std::vector<std::string> hashes3;
        CheckChannelAccessRule(AID_HASH_ALL, hashes3, ApduNeverNfcNever);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : NFC_TC_SE_AccessRulerTest_002_014
 * @tc.name      : AccessRulerTest_002_014
 * @tc.desc      : Access ruler test
 */
TEST_F(NfcTcSeTest, AccessRulerTest_002_014)
{
    try {
        // R0:AID.#1 (CER#1-Ref)- AR -APDU-NEVER
        // R1:AID.#ALL (CER#1-Ref)- AR -APDU-Filter(APDU1,APDU2)
        std::string refData = REF_DO_HASHALLAID_AIDALLHASH;
        std::shared_ptr<BerTlv> refBt = BerTlv::StrToBerTlv(refData);
        std::shared_ptr<RefDo> refDo = RefDo::BerTlvToRefDo(refBt);
        std::string refData2 = REF_DO_AID_ALL;
        std::shared_ptr<BerTlv> refBt2 = BerTlv::StrToBerTlv(refData2);
        std::shared_ptr<RefDo> refDo2 = RefDo::BerTlvToRefDo(refBt2);
        std::string arData = AR_DO_DATA_ONLY_APDU_NEVER;
        std::shared_ptr<BerTlv> arBt = BerTlv::StrToBerTlv(arData);
        std::shared_ptr<ArDo> arDo = ArDo::BerTlvToArDo(arBt);
        std::string arData2 = AR_DO_DATA_ONLY_APDU_FILTER_2;
        std::shared_ptr<BerTlv> arBt2 = BerTlv::StrToBerTlv(arData2);
        std::shared_ptr<ArDo> arDo2 = ArDo::BerTlvToArDo(arBt2);
        accessRuleCache_->AddAccessRule(refDo, arDo);
        accessRuleCache_->AddAccessRule(refDo2, arDo2);

        std::vector<std::string> hashes1;
        hashes1.push_back(HASH_AID_ALL);
        CheckChannelAccessRule(AID_1, hashes1, ApduFilter23);
        CheckChannelAccessRule(AID_HASH_ALL, hashes1, ApduNeverNfcNever);

        std::vector<std::string> hashes2;
        hashes2.push_back(HASH_1);
        CheckChannelAccessRule(AID_HASH_ALL, hashes2, ApduNeverNfcNever);

        std::vector<std::string> hashes3;
        CheckChannelAccessRule(AID_HASH_ALL, hashes3, ApduNeverNfcNever);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : NFC_TC_SE_AccessRulerTest_002_015
 * @tc.name      : AccessRulerTest_002_015
 * @tc.desc      : Access ruler test
 */
TEST_F(NfcTcSeTest, AccessRulerTest_002_015)
{
    try {
        // R0:AID.#1 (CER#1-Ref)- AR -APDU- Filter(APDU3)
        // R1:AID.#ALL (CER#1-Ref)- AR -APDU- Filter(APDU1,APDU2)
        std::string refData = REF_DO_HASHALLAID_AIDALLHASH;
        std::shared_ptr<BerTlv> refBt = BerTlv::StrToBerTlv(refData);
        std::shared_ptr<RefDo> refDo = RefDo::BerTlvToRefDo(refBt);
        std::string refData2 = REF_DO_AID_ALL;
        std::shared_ptr<BerTlv> refBt2 = BerTlv::StrToBerTlv(refData2);
        std::shared_ptr<RefDo> refDo2 = RefDo::BerTlvToRefDo(refBt2);
        std::string arData = AR_DO_DATA_ONLY_APDU_FILTER;
        std::shared_ptr<BerTlv> arBt = BerTlv::StrToBerTlv(arData);
        std::shared_ptr<ArDo> arDo = ArDo::BerTlvToArDo(arBt);
        std::string arData2 = AR_DO_DATA_ONLY_APDU_FILTER_2;
        std::shared_ptr<BerTlv> arBt2 = BerTlv::StrToBerTlv(arData2);
        std::shared_ptr<ArDo> arDo2 = ArDo::BerTlvToArDo(arBt2);
        accessRuleCache_->AddAccessRule(refDo, arDo);
        accessRuleCache_->AddAccessRule(refDo2, arDo2);

        std::vector<std::string> hashes1;
        hashes1.push_back(HASH_AID_ALL);
        CheckChannelAccessRule(AID_1, hashes1, ApduFilter23);
        CheckChannelAccessRule(AID_HASH_ALL, hashes1, ApduFilter1);

        std::vector<std::string> hashes2;
        hashes2.push_back(HASH_1);
        CheckChannelAccessRule(AID_HASH_ALL, hashes2, ApduNeverNfcNever);

        std::vector<std::string> hashes3;
        CheckChannelAccessRule(AID_HASH_ALL, hashes3, ApduNeverNfcNever);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : NFC_TC_SE_AccessRulerTest_002_016
 * @tc.name      : AccessRulerTest_002_016
 * @tc.desc      : Access ruler test
 */
TEST_F(NfcTcSeTest, AccessRulerTest_002_016)
{
    try {
        // R0:AID.#1 (CER#1-Ref)- AR -NFC-ALWAYS
        // R1:AID.#ALL (CER#1-Ref)- AR -NFC-NEVER
        std::string refData = REF_DO_HASHALLAID_AIDALLHASH;
        std::shared_ptr<BerTlv> refBt = BerTlv::StrToBerTlv(refData);
        std::shared_ptr<RefDo> refDo = RefDo::BerTlvToRefDo(refBt);
        std::string refData2 = REF_DO_AID_ALL;
        std::shared_ptr<BerTlv> refBt2 = BerTlv::StrToBerTlv(refData2);
        std::shared_ptr<RefDo> refDo2 = RefDo::BerTlvToRefDo(refBt2);
        std::string arData = AR_DO_DATA_ONLY_APDU_ALWAYS;
        std::shared_ptr<BerTlv> arBt = BerTlv::StrToBerTlv(arData);
        std::shared_ptr<ArDo> arDo = ArDo::BerTlvToArDo(arBt);
        std::string arData2 = AR_DO_DATA_ONLY_APDU_NEVER;
        std::shared_ptr<BerTlv> arBt2 = BerTlv::StrToBerTlv(arData2);
        std::shared_ptr<ArDo> arDo2 = ArDo::BerTlvToArDo(arBt2);
        accessRuleCache_->AddAccessRule(refDo, arDo);
        accessRuleCache_->AddAccessRule(refDo2, arDo2);

        std::vector<std::string> hashes1;
        hashes1.push_back(HASH_AID_ALL);
        CheckChannelAccessRule(AID_1, hashes1, NfcNever);
        CheckChannelAccessRule(AID_HASH_ALL, hashes1, NfcAlways);

        std::vector<std::string> hashes2;
        hashes2.push_back(HASH_1);
        CheckChannelAccessRule(AID_HASH_ALL, hashes2, NfcNever);

        std::vector<std::string> hashes3;
        CheckChannelAccessRule(AID_HASH_ALL, hashes3, NfcNever);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : NFC_TC_SE_AccessRulerTest_002_017
 * @tc.name      : AccessRulerTest_002_017
 * @tc.desc      : Access ruler test
 */
TEST_F(NfcTcSeTest, AccessRulerTest_002_017)
{
    try {
        // R0:AID.#1 (CER#1-Ref)- AR -NFC-NEVER
        // R1:AID.#ALL (CER#1-Ref)- AR -NFC-ALWAYS
        std::string refData = REF_DO_HASHALLAID_AIDALLHASH;
        std::shared_ptr<BerTlv> refBt = BerTlv::StrToBerTlv(refData);
        std::shared_ptr<RefDo> refDo = RefDo::BerTlvToRefDo(refBt);
        std::string refData2 = REF_DO_AID_ALL;
        std::shared_ptr<BerTlv> refBt2 = BerTlv::StrToBerTlv(refData2);
        std::shared_ptr<RefDo> refDo2 = RefDo::BerTlvToRefDo(refBt2);
        std::string arData = AR_DO_DATA_ONLY_APDU_NEVER;
        std::shared_ptr<BerTlv> arBt = BerTlv::StrToBerTlv(arData);
        std::shared_ptr<ArDo> arDo = ArDo::BerTlvToArDo(arBt);
        std::string arData2 = AR_DO_DATA_ONLY_APDU_ALWAYS;
        std::shared_ptr<BerTlv> arBt2 = BerTlv::StrToBerTlv(arData2);
        std::shared_ptr<ArDo> arDo2 = ArDo::BerTlvToArDo(arBt2);
        accessRuleCache_->AddAccessRule(refDo, arDo);
        accessRuleCache_->AddAccessRule(refDo2, arDo2);

        std::vector<std::string> hashes1;
        hashes1.push_back(HASH_AID_ALL);
        CheckChannelAccessRule(AID_1, hashes1, NfcAlways);
        CheckChannelAccessRule(AID_HASH_ALL, hashes1, NfcNever);

        std::vector<std::string> hashes2;
        hashes2.push_back(HASH_1);
        CheckChannelAccessRule(AID_HASH_ALL, hashes2, NfcNever);

        std::vector<std::string> hashes3;
        CheckChannelAccessRule(AID_HASH_ALL, hashes3, NfcNever);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : NFC_TC_SE_AccessRulerTest_002_018
 * @tc.name      : AccessRulerTest_002_018
 * @tc.desc      : Access ruler test
 */
TEST_F(NfcTcSeTest, AccessRulerTest_002_018)
{
    try {
        // R0:AID.#1 (CER#1-Ref)- AR -APDU-ALWAYS -NFC-ALWAYS
        // R1:AID.#1 (CER#1-Ref)- AR -APDU-NEVER -NFC-NEVER
        std::string refData = REF_DO_A1_H1;
        std::shared_ptr<BerTlv> refBt = BerTlv::StrToBerTlv(refData);
        std::shared_ptr<RefDo> refDo = RefDo::BerTlvToRefDo(refBt);
        std::string arData = AR_DO_DATA_11;
        std::shared_ptr<BerTlv> arBt = BerTlv::StrToBerTlv(arData);
        std::shared_ptr<ArDo> arDo = ArDo::BerTlvToArDo(arBt);
        std::string arData2 = AR_DO_DATA_00;
        std::shared_ptr<BerTlv> arBt2 = BerTlv::StrToBerTlv(arData2);
        std::shared_ptr<ArDo> arDo2 = ArDo::BerTlvToArDo(arBt2);
        accessRuleCache_->AddAccessRule(refDo, arDo);
        accessRuleCache_->AddAccessRule(refDo, arDo2);

        std::vector<std::string> hashes1;
        hashes1.push_back(HASH_1);
        CheckChannelAccessRule(AID_1, hashes1, ApduNeverNfcNever);

        std::vector<std::string> hashes2;
        hashes2.push_back(HASH_2);
        CheckChannelAccessRule(AID_1, hashes2, ApduNeverNfcNever);

        std::vector<std::string> hashes3;
        CheckChannelAccessRule(AID_1, hashes3, ApduNeverNfcNever);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : NFC_TC_SE_AccessRulerTest_002_019
 * @tc.name      : AccessRulerTest_002_019
 * @tc.desc      : Access ruler test
 */
TEST_F(NfcTcSeTest, AccessRulerTest_002_019)
{
    try {
        // R0:AID.#1 (CER#1-Ref)- AR -APDU-ALWAYS -NFC-NEVER
        // R1:AID.#1 (CER#1-Ref)- AR -APDU-NEVER -NFC-ALWAYS
        std::string refData = REF_DO_A1_H1;
        std::shared_ptr<BerTlv> refBt = BerTlv::StrToBerTlv(refData);
        std::shared_ptr<RefDo> refDo = RefDo::BerTlvToRefDo(refBt);
        std::string arData = AR_DO_DATA_10;
        std::shared_ptr<BerTlv> arBt = BerTlv::StrToBerTlv(arData);
        std::shared_ptr<ArDo> arDo = ArDo::BerTlvToArDo(arBt);
        std::string arData2 = AR_DO_DATA_01;
        std::shared_ptr<BerTlv> arBt2 = BerTlv::StrToBerTlv(arData2);
        std::shared_ptr<ArDo> arDo2 = ArDo::BerTlvToArDo(arBt2);
        accessRuleCache_->AddAccessRule(refDo, arDo);
        accessRuleCache_->AddAccessRule(refDo, arDo2);

        std::vector<std::string> hashes1;
        hashes1.push_back(HASH_1);
        CheckChannelAccessRule(AID_1, hashes1, ApduNeverNfcNever);

        std::vector<std::string> hashes2;
        hashes2.push_back(HASH_2);
        CheckChannelAccessRule(AID_1, hashes2, ApduNeverNfcNever);

        std::vector<std::string> hashes3;
        CheckChannelAccessRule(AID_1, hashes3, ApduNeverNfcNever);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : NFC_TC_SE_AccessRulerTest_003_001
 * @tc.name      : AccessRulerTest_003_001
 * @tc.desc      : Access ruler test
 */
TEST_F(NfcTcSeTest, AccessRulerTest_003_001)
{
    try {
        // R0:AID.#1 (CER#1-Ref)- AR -APDU-NEVER
        // R1:AID.#ALL (ALL-Ref)- AR -APDU-ALWAYS
        std::string refData = REF_DO_A1_H1;
        std::shared_ptr<BerTlv> refBt = BerTlv::StrToBerTlv(refData);
        std::shared_ptr<RefDo> refDo = RefDo::BerTlvToRefDo(refBt);
        std::string refData2 = REF_DO_AID_ALL_HASH_ALL;
        std::shared_ptr<BerTlv> refBt2 = BerTlv::StrToBerTlv(refData2);
        std::shared_ptr<RefDo> refDo2 = RefDo::BerTlvToRefDo(refBt2);
        std::string arData = AR_DO_DATA_ONLY_APDU_NEVER;
        std::shared_ptr<BerTlv> arBt = BerTlv::StrToBerTlv(arData);
        std::shared_ptr<ArDo> arDo = ArDo::BerTlvToArDo(arBt);
        std::string arData2 = AR_DO_DATA_ONLY_APDU_ALWAYS;
        std::shared_ptr<BerTlv> arBt2 = BerTlv::StrToBerTlv(arData2);
        std::shared_ptr<ArDo> arDo2 = ArDo::BerTlvToArDo(arBt2);
        accessRuleCache_->AddAccessRule(refDo, arDo);
        accessRuleCache_->AddAccessRule(refDo2, arDo2);

        std::vector<std::string> hashes1;
        hashes1.push_back(HASH_1);
        CheckChannelAccessRule(AID_1, hashes1, ApduNeverNfcNever);
        CheckChannelAccessRule(AID_2, hashes1, ApduAlwaysNfcAlways);

        std::vector<std::string> hashes2;
        hashes2.push_back(HASH_2);
        CheckChannelAccessRule(AID_1, hashes2, ApduNeverNfcNever);
        CheckChannelAccessRule(AID_2, hashes2, ApduAlwaysNfcAlways);

        std::vector<std::string> hashes3;
        CheckChannelAccessRule(AID_1, hashes3, ApduNeverNfcNever);
        CheckChannelAccessRule(AID_2, hashes3, ApduNeverNfcNever);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : NFC_TC_SE_AccessRulerTest_003_002
 * @tc.name      : AccessRulerTest_003_002
 * @tc.desc      : Access ruler test
 */
TEST_F(NfcTcSeTest, AccessRulerTest_003_002)
{
    try {
        // R0:AID.#1 (CER#1-Ref)- AR -APDU-ALWAYS
        // R1:AID.#ALL (ALL-Ref)- AR -APDU-NEVER
        std::string refData = REF_DO_A1_H1;
        std::shared_ptr<BerTlv> refBt = BerTlv::StrToBerTlv(refData);
        std::shared_ptr<RefDo> refDo = RefDo::BerTlvToRefDo(refBt);
        std::string refData2 = REF_DO_AID_ALL_HASH_ALL;
        std::shared_ptr<BerTlv> refBt2 = BerTlv::StrToBerTlv(refData2);
        std::shared_ptr<RefDo> refDo2 = RefDo::BerTlvToRefDo(refBt2);
        std::string arData = AR_DO_DATA_ONLY_APDU_ALWAYS;
        std::shared_ptr<BerTlv> arBt = BerTlv::StrToBerTlv(arData);
        std::shared_ptr<ArDo> arDo = ArDo::BerTlvToArDo(arBt);
        std::string arData2 = AR_DO_DATA_ONLY_APDU_NEVER;
        std::shared_ptr<BerTlv> arBt2 = BerTlv::StrToBerTlv(arData2);
        std::shared_ptr<ArDo> arDo2 = ArDo::BerTlvToArDo(arBt2);
        accessRuleCache_->AddAccessRule(refDo, arDo);
        accessRuleCache_->AddAccessRule(refDo2, arDo2);

        std::vector<std::string> hashes1;
        hashes1.push_back(HASH_1);
        CheckChannelAccessRule(AID_1, hashes1, ApduAlwaysNfcAlways);
        CheckChannelAccessRule(AID_2, hashes1, ApduNeverNfcNever);

        std::vector<std::string> hashes2;
        hashes2.push_back(HASH_2);
        CheckChannelAccessRule(AID_1, hashes2, ApduNeverNfcNever);
        CheckChannelAccessRule(AID_2, hashes2, ApduNeverNfcNever);

        std::vector<std::string> hashes3;
        CheckChannelAccessRule(AID_1, hashes3, ApduNeverNfcNever);
        CheckChannelAccessRule(AID_2, hashes3, ApduNeverNfcNever);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : NFC_TC_SE_AccessRulerTest_003_003
 * @tc.name      : AccessRulerTest_003_003
 * @tc.desc      : Access ruler test
 */
TEST_F(NfcTcSeTest, AccessRulerTest_003_003)
{
    try {
        // R0:AID.#1 (ALL-Ref)- AR -APDU-NEVER
        // R1:AID.#ALL (CER#1-Ref)- AR -APDU-ALWAYS
        std::string refData = REF_DO_HASH_ALL;
        std::shared_ptr<BerTlv> refBt = BerTlv::StrToBerTlv(refData);
        std::shared_ptr<RefDo> refDo = RefDo::BerTlvToRefDo(refBt);
        std::string refData2 = REF_DO_AID_ALL;
        std::shared_ptr<BerTlv> refBt2 = BerTlv::StrToBerTlv(refData2);
        std::shared_ptr<RefDo> refDo2 = RefDo::BerTlvToRefDo(refBt2);
        std::string arData = AR_DO_DATA_ONLY_APDU_NEVER;
        std::shared_ptr<BerTlv> arBt = BerTlv::StrToBerTlv(arData);
        std::shared_ptr<ArDo> arDo = ArDo::BerTlvToArDo(arBt);
        std::string arData2 = AR_DO_DATA_ONLY_APDU_ALWAYS;
        std::shared_ptr<BerTlv> arBt2 = BerTlv::StrToBerTlv(arData2);
        std::shared_ptr<ArDo> arDo2 = ArDo::BerTlvToArDo(arBt2);
        accessRuleCache_->AddAccessRule(refDo, arDo);
        accessRuleCache_->AddAccessRule(refDo2, arDo2);

        std::vector<std::string> hashes1;
        hashes1.push_back(HASH_AID_ALL);
        CheckChannelAccessRule(AID_HASH_ALL, hashes1, ApduNeverNfcNever);
        CheckChannelAccessRule(AID_1, hashes1, ApduAlwaysNfcAlways);

        std::vector<std::string> hashes2;
        hashes2.push_back(HASH_1);
        CheckChannelAccessRule(AID_HASH_ALL, hashes2, ApduNeverNfcNever);

        std::vector<std::string> hashes3;
        CheckChannelAccessRule(AID_HASH_ALL, hashes3, ApduNeverNfcNever);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : NFC_TC_SE_AccessRulerTest_003_004
 * @tc.name      : AccessRulerTest_003_004
 * @tc.desc      : Access ruler test
 */
TEST_F(NfcTcSeTest, AccessRulerTest_003_004)
{
    try {
        // R0:AID.#1 (ALL-Ref)- AR -APDU-ALWAYS
        // R1:AID.#ALL (CER#1-Ref)- AR -APDU-NEVER
        std::string refData = REF_DO_HASH_ALL;
        std::shared_ptr<BerTlv> refBt = BerTlv::StrToBerTlv(refData);
        std::shared_ptr<RefDo> refDo = RefDo::BerTlvToRefDo(refBt);
        std::string refData2 = REF_DO_AID_ALL;
        std::shared_ptr<BerTlv> refBt2 = BerTlv::StrToBerTlv(refData2);
        std::shared_ptr<RefDo> refDo2 = RefDo::BerTlvToRefDo(refBt2);
        std::string arData = AR_DO_DATA_ONLY_APDU_ALWAYS;
        std::shared_ptr<BerTlv> arBt = BerTlv::StrToBerTlv(arData);
        std::shared_ptr<ArDo> arDo = ArDo::BerTlvToArDo(arBt);
        std::string arData2 = AR_DO_DATA_ONLY_APDU_NEVER;
        std::shared_ptr<BerTlv> arBt2 = BerTlv::StrToBerTlv(arData2);
        std::shared_ptr<ArDo> arDo2 = ArDo::BerTlvToArDo(arBt2);
        accessRuleCache_->AddAccessRule(refDo, arDo);
        accessRuleCache_->AddAccessRule(refDo2, arDo2);

        std::vector<std::string> hashes1;
        hashes1.push_back(HASH_AID_ALL);
        CheckChannelAccessRule(AID_HASH_ALL, hashes1, ApduAlwaysNfcAlways);
        CheckChannelAccessRule(AID_1, hashes1, ApduNeverNfcNever);

        std::vector<std::string> hashes2;
        hashes2.push_back(HASH_1);
        CheckChannelAccessRule(AID_HASH_ALL, hashes2, ApduAlwaysNfcAlways);

        std::vector<std::string> hashes3;
        CheckChannelAccessRule(AID_HASH_ALL, hashes3, ApduNeverNfcNever);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : NFC_TC_SE_AccessRulerTest_003_005
 * @tc.name      : AccessRulerTest_003_005
 * @tc.desc      : Access ruler test
 */
TEST_F(NfcTcSeTest, AccessRulerTest_003_005)
{
    try {
        // R0:AID.#1 (CER#1-Ref)- AR -APDU-NEVER
        // R1:AID.#ALL (ALL-Ref)- AR -APDU- Filter(APDU1,APDU2)
        std::string refData = REF_DO_A1_H1;
        std::shared_ptr<BerTlv> refBt = BerTlv::StrToBerTlv(refData);
        std::shared_ptr<RefDo> refDo = RefDo::BerTlvToRefDo(refBt);
        std::string refData2 = REF_DO_AID_ALL_HASH_ALL;
        std::shared_ptr<BerTlv> refBt2 = BerTlv::StrToBerTlv(refData2);
        std::shared_ptr<RefDo> refDo2 = RefDo::BerTlvToRefDo(refBt2);
        std::string arData = AR_DO_DATA_ONLY_APDU_NEVER;
        std::shared_ptr<BerTlv> arBt = BerTlv::StrToBerTlv(arData);
        std::shared_ptr<ArDo> arDo = ArDo::BerTlvToArDo(arBt);
        std::string arData2 = AR_DO_DATA_ONLY_APDU_FILTER_2;
        std::shared_ptr<BerTlv> arBt2 = BerTlv::StrToBerTlv(arData2);
        std::shared_ptr<ArDo> arDo2 = ArDo::BerTlvToArDo(arBt2);
        accessRuleCache_->AddAccessRule(refDo, arDo);
        accessRuleCache_->AddAccessRule(refDo2, arDo2);

        std::vector<std::string> hashes1;
        hashes1.push_back(HASH_1);
        CheckChannelAccessRule(AID_1, hashes1, ApduNeverNfcNever);
        CheckChannelAccessRule(AID_3, hashes1, ApduFilter23);

        std::vector<std::string> hashes2;
        hashes2.push_back(HASH_2);
        CheckChannelAccessRule(AID_1, hashes2, ApduNeverNfcNever);
        CheckChannelAccessRule(AID_3, hashes2, ApduFilter23);

        std::vector<std::string> hashes3;
        CheckChannelAccessRule(AID_1, hashes3, ApduNeverNfcNever);
        CheckChannelAccessRule(AID_3, hashes3, ApduNeverNfcNever);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : NFC_TC_SE_AccessRulerTest_003_006
 * @tc.name      : AccessRulerTest_003_006
 * @tc.desc      : Access ruler test
 */
TEST_F(NfcTcSeTest, AccessRulerTest_003_006)
{
    try {
        // R0:AID.#1 (CER#1-Ref)- AR -APDU- Filter(APDU1,APDU2)
        // R1:AID.#ALL  (ALL-Ref)- AR -APDU-NEVER
        std::string refData = REF_DO_A1_H1;
        std::shared_ptr<BerTlv> refBt = BerTlv::StrToBerTlv(refData);
        std::shared_ptr<RefDo> refDo = RefDo::BerTlvToRefDo(refBt);
        std::string refData2 = REF_DO_AID_ALL_HASH_ALL;
        std::shared_ptr<BerTlv> refBt2 = BerTlv::StrToBerTlv(refData2);
        std::shared_ptr<RefDo> refDo2 = RefDo::BerTlvToRefDo(refBt2);
        std::string arData = AR_DO_DATA_ONLY_APDU_FILTER_2;
        std::shared_ptr<BerTlv> arBt = BerTlv::StrToBerTlv(arData);
        std::shared_ptr<ArDo> arDo = ArDo::BerTlvToArDo(arBt);
        std::string arData2 = AR_DO_DATA_ONLY_APDU_NEVER;
        std::shared_ptr<BerTlv> arBt2 = BerTlv::StrToBerTlv(arData2);
        std::shared_ptr<ArDo> arDo2 = ArDo::BerTlvToArDo(arBt2);
        accessRuleCache_->AddAccessRule(refDo, arDo);
        accessRuleCache_->AddAccessRule(refDo2, arDo2);

        std::vector<std::string> hashes1;
        hashes1.push_back(HASH_1);
        CheckChannelAccessRule(AID_1, hashes1, ApduFilter23);
        CheckChannelAccessRule(AID_3, hashes1, ApduNeverNfcNever);

        std::vector<std::string> hashes2;
        hashes2.push_back(HASH_2);
        CheckChannelAccessRule(AID_1, hashes2, ApduNeverNfcNever);

        std::vector<std::string> hashes3;
        CheckChannelAccessRule(AID_1, hashes3, ApduNeverNfcNever);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : NFC_TC_SE_AccessRulerTest_003_007
 * @tc.name      : AccessRulerTest_003_007
 * @tc.desc      : Access ruler test
 */
TEST_F(NfcTcSeTest, AccessRulerTest_003_007)
{
    try {
        // R0:AID.#1 (ALL-Ref)- AR -APDU-NEVER
        // R1:AID.#ALL (CER#1-Ref)- AR -APDU-Filter(APDU1,APDU2)
        std::string refData = REF_DO_HASH_ALL;
        std::shared_ptr<BerTlv> refBt = BerTlv::StrToBerTlv(refData);
        std::shared_ptr<RefDo> refDo = RefDo::BerTlvToRefDo(refBt);
        std::string refData2 = REF_DO_AID_ALL;
        std::shared_ptr<BerTlv> refBt2 = BerTlv::StrToBerTlv(refData2);
        std::shared_ptr<RefDo> refDo2 = RefDo::BerTlvToRefDo(refBt2);
        std::string arData = AR_DO_DATA_ONLY_APDU_NEVER;
        std::shared_ptr<BerTlv> arBt = BerTlv::StrToBerTlv(arData);
        std::shared_ptr<ArDo> arDo = ArDo::BerTlvToArDo(arBt);
        std::string arData2 = AR_DO_DATA_ONLY_APDU_FILTER_2;
        std::shared_ptr<BerTlv> arBt2 = BerTlv::StrToBerTlv(arData2);
        std::shared_ptr<ArDo> arDo2 = ArDo::BerTlvToArDo(arBt2);
        accessRuleCache_->AddAccessRule(refDo, arDo);
        accessRuleCache_->AddAccessRule(refDo2, arDo2);

        std::vector<std::string> hashes1;
        hashes1.push_back(HASH_AID_ALL);
        CheckChannelAccessRule(AID_HASH_ALL, hashes1, ApduNeverNfcNever);
        CheckChannelAccessRule(AID_1, hashes1, ApduFilter23);

        std::vector<std::string> hashes2;
        hashes2.push_back(HASH_1);
        CheckChannelAccessRule(AID_HASH_ALL, hashes2, ApduNeverNfcNever);

        std::vector<std::string> hashes3;
        CheckChannelAccessRule(AID_HASH_ALL, hashes3, ApduNeverNfcNever);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : NFC_TC_SE_AccessRulerTest_003_008
 * @tc.name      : AccessRulerTest_003_008
 * @tc.desc      : Access ruler test
 */
TEST_F(NfcTcSeTest, AccessRulerTest_003_008)
{
    try {
        // R0:AID.#1 (ALL-Ref)- AR -APDU-Filter(APDU1,APDU2)
        // R1:AID.#ALL (CER#1-Ref)- AR -APDU-NEVER
        std::string refData = REF_DO_HASH_ALL;
        std::shared_ptr<BerTlv> refBt = BerTlv::StrToBerTlv(refData);
        std::shared_ptr<RefDo> refDo = RefDo::BerTlvToRefDo(refBt);
        std::string refData2 = REF_DO_AID_ALL;
        std::shared_ptr<BerTlv> refBt2 = BerTlv::StrToBerTlv(refData2);
        std::shared_ptr<RefDo> refDo2 = RefDo::BerTlvToRefDo(refBt2);
        std::string arData = AR_DO_DATA_ONLY_APDU_FILTER_2;
        std::shared_ptr<BerTlv> arBt = BerTlv::StrToBerTlv(arData);
        std::shared_ptr<ArDo> arDo = ArDo::BerTlvToArDo(arBt);
        std::string arData2 = AR_DO_DATA_ONLY_APDU_NEVER;
        std::shared_ptr<BerTlv> arBt2 = BerTlv::StrToBerTlv(arData2);
        std::shared_ptr<ArDo> arDo2 = ArDo::BerTlvToArDo(arBt2);
        accessRuleCache_->AddAccessRule(refDo, arDo);
        accessRuleCache_->AddAccessRule(refDo2, arDo2);

        std::vector<std::string> hashes1;
        hashes1.push_back(HASH_AID_ALL);
        CheckChannelAccessRule(AID_HASH_ALL, hashes1, ApduFilter23);
        CheckChannelAccessRule(AID_2, hashes1, ApduNeverNfcNever);

        std::vector<std::string> hashes2;
        hashes2.push_back(HASH_1);
        CheckChannelAccessRule(AID_HASH_ALL, hashes2, ApduFilter23);
        CheckChannelAccessRule(AID_2, hashes2, ApduNeverNfcNever);

        std::vector<std::string> hashes3;
        CheckChannelAccessRule(AID_HASH_ALL, hashes3, ApduNeverNfcNever);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : NFC_TC_SE_AccessRulerTest_003_009
 * @tc.name      : AccessRulerTest_003_009
 * @tc.desc      : Access ruler test
 */
TEST_F(NfcTcSeTest, AccessRulerTest_003_009)
{
    try {
        // R0:AID.#1 (ALL-Ref)- AR -APDU-Filter(APDU1,APDU2)
        // R1:AID.#ALL (CER#1-Ref)- AR -APDU-ALWAYS
        std::string refData = REF_DO_HASH_ALL;
        std::shared_ptr<BerTlv> refBt = BerTlv::StrToBerTlv(refData);
        std::shared_ptr<RefDo> refDo = RefDo::BerTlvToRefDo(refBt);
        std::string refData2 = REF_DO_AID_ALL;
        std::shared_ptr<BerTlv> refBt2 = BerTlv::StrToBerTlv(refData2);
        std::shared_ptr<RefDo> refDo2 = RefDo::BerTlvToRefDo(refBt2);
        std::string arData = AR_DO_DATA_ONLY_APDU_FILTER_2;
        std::shared_ptr<BerTlv> arBt = BerTlv::StrToBerTlv(arData);
        std::shared_ptr<ArDo> arDo = ArDo::BerTlvToArDo(arBt);
        std::string arData2 = AR_DO_DATA_ONLY_APDU_ALWAYS;
        std::shared_ptr<BerTlv> arBt2 = BerTlv::StrToBerTlv(arData2);
        std::shared_ptr<ArDo> arDo2 = ArDo::BerTlvToArDo(arBt2);
        accessRuleCache_->AddAccessRule(refDo, arDo);
        accessRuleCache_->AddAccessRule(refDo2, arDo2);

        std::vector<std::string> hashes1;
        hashes1.push_back(HASH_AID_ALL);
        CheckChannelAccessRule(AID_HASH_ALL, hashes1, ApduFilter23);
        CheckChannelAccessRule(AID_2, hashes1, ApduAlwaysNfcAlways);

        std::vector<std::string> hashes2;
        hashes2.push_back(HASH_1);
        CheckChannelAccessRule(AID_HASH_ALL, hashes2, ApduFilter23);
        CheckChannelAccessRule(AID_2, hashes2, ApduNeverNfcNever);

        std::vector<std::string> hashes3;
        CheckChannelAccessRule(AID_HASH_ALL, hashes3, ApduNeverNfcNever);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : NFC_TC_SE_AccessRulerTest_003_010
 * @tc.name      : AccessRulerTest_003_010
 * @tc.desc      : Access ruler test
 */
TEST_F(NfcTcSeTest, AccessRulerTest_003_010)
{
    try {
        // R0:AID.#1 (ALL-Ref)- AR -APDU-ALWAYS
        // R1:AID.#ALL (CER#1-Ref)- AR -APDU-Filter(APDU1,APDU2)
        std::string refData = REF_DO_HASH_ALL;
        std::shared_ptr<BerTlv> refBt = BerTlv::StrToBerTlv(refData);
        std::shared_ptr<RefDo> refDo = RefDo::BerTlvToRefDo(refBt);
        std::string refData2 = REF_DO_AID_ALL;
        std::shared_ptr<BerTlv> refBt2 = BerTlv::StrToBerTlv(refData2);
        std::shared_ptr<RefDo> refDo2 = RefDo::BerTlvToRefDo(refBt2);
        std::string arData = AR_DO_DATA_ONLY_APDU_ALWAYS;
        std::shared_ptr<BerTlv> arBt = BerTlv::StrToBerTlv(arData);
        std::shared_ptr<ArDo> arDo = ArDo::BerTlvToArDo(arBt);
        std::string arData2 = AR_DO_DATA_ONLY_APDU_FILTER_2;
        std::shared_ptr<BerTlv> arBt2 = BerTlv::StrToBerTlv(arData2);
        std::shared_ptr<ArDo> arDo2 = ArDo::BerTlvToArDo(arBt2);
        accessRuleCache_->AddAccessRule(refDo, arDo);
        accessRuleCache_->AddAccessRule(refDo2, arDo2);

        std::vector<std::string> hashes1;
        hashes1.push_back(HASH_AID_ALL);
        CheckChannelAccessRule(AID_HASH_ALL, hashes1, ApduAlwaysNfcAlways);
        CheckChannelAccessRule(AID_2, hashes1, ApduFilter23);

        std::vector<std::string> hashes2;
        hashes2.push_back(HASH_1);
        CheckChannelAccessRule(AID_HASH_ALL, hashes2, ApduAlwaysNfcAlways);
        CheckChannelAccessRule(AID_2, hashes2, ApduNeverNfcNever);

        std::vector<std::string> hashes3;
        CheckChannelAccessRule(AID_HASH_ALL, hashes3, ApduNeverNfcNever);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : NFC_TC_SE_AccessRulerTest_003_011
 * @tc.name      : AccessRulerTest_003_011
 * @tc.desc      : Access ruler test
 */
TEST_F(NfcTcSeTest, AccessRulerTest_003_011)
{
    try {
        // R0:AID.#1 (ALL-Ref)- AR -APDU-Filter(APDU1,APDU2)
        // R1:AID.#ALL (CER#1-Ref)- AR -APDU-Filter(APDU3)
        std::string refData = REF_DO_HASH_ALL;
        std::shared_ptr<BerTlv> refBt = BerTlv::StrToBerTlv(refData);
        std::shared_ptr<RefDo> refDo = RefDo::BerTlvToRefDo(refBt);
        std::string refData2 = REF_DO_AID_ALL;
        std::shared_ptr<BerTlv> refBt2 = BerTlv::StrToBerTlv(refData2);
        std::shared_ptr<RefDo> refDo2 = RefDo::BerTlvToRefDo(refBt2);
        std::string arData = AR_DO_DATA_ONLY_APDU_FILTER_2;
        std::shared_ptr<BerTlv> arBt = BerTlv::StrToBerTlv(arData);
        std::shared_ptr<ArDo> arDo = ArDo::BerTlvToArDo(arBt);
        std::string arData2 = AR_DO_DATA_ONLY_APDU_FILTER;
        std::shared_ptr<BerTlv> arBt2 = BerTlv::StrToBerTlv(arData2);
        std::shared_ptr<ArDo> arDo2 = ArDo::BerTlvToArDo(arBt2);
        accessRuleCache_->AddAccessRule(refDo, arDo);
        accessRuleCache_->AddAccessRule(refDo2, arDo2);

        std::vector<std::string> hashes1;
        hashes1.push_back(HASH_AID_ALL);
        CheckChannelAccessRule(AID_HASH_ALL, hashes1, ApduFilter23);
        CheckChannelAccessRule(AID_2, hashes1, ApduFilter1);

        std::vector<std::string> hashes2;
        hashes2.push_back(HASH_1);
        CheckChannelAccessRule(AID_HASH_ALL, hashes2, ApduFilter23);
        CheckChannelAccessRule(AID_2, hashes2, ApduNeverNfcNever);

        std::vector<std::string> hashes3;
        CheckChannelAccessRule(AID_HASH_ALL, hashes3, ApduNeverNfcNever);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : NFC_TC_SE_AccessRulerTest_003_012
 * @tc.name      : AccessRulerTest_003_012
 * @tc.desc      : Access ruler test
 */
TEST_F(NfcTcSeTest, AccessRulerTest_003_012)
{
    try {
        // R0:AID.#1 (CER#1-Ref)- AR -NFC-NEVER
        // R1:AID.#ALL (ALL-Ref)- AR -NFC-ALWAYS
        std::string refData = REF_DO_A1_H1;
        std::shared_ptr<BerTlv> refBt = BerTlv::StrToBerTlv(refData);
        std::shared_ptr<RefDo> refDo = RefDo::BerTlvToRefDo(refBt);
        std::string refData2 = REF_DO_AID_ALL_HASH_ALL;
        std::shared_ptr<BerTlv> refBt2 = BerTlv::StrToBerTlv(refData2);
        std::shared_ptr<RefDo> refDo2 = RefDo::BerTlvToRefDo(refBt2);
        std::string arData = AR_DO_DATA_ONLY_NFC_NEVER;
        std::shared_ptr<BerTlv> arBt = BerTlv::StrToBerTlv(arData);
        std::shared_ptr<ArDo> arDo = ArDo::BerTlvToArDo(arBt);
        std::string arData2 = AR_DO_DATA_ONLY_NFC_ALWAYS;
        std::shared_ptr<BerTlv> arBt2 = BerTlv::StrToBerTlv(arData2);
        std::shared_ptr<ArDo> arDo2 = ArDo::BerTlvToArDo(arBt2);
        accessRuleCache_->AddAccessRule(refDo, arDo);
        accessRuleCache_->AddAccessRule(refDo2, arDo2);

        std::vector<std::string> hashes1;
        hashes1.push_back(HASH_1);
        CheckChannelAccessRule(AID_1, hashes1, NfcNever);
        CheckChannelAccessRule(AID_3, hashes1, NfcAlways);

        std::vector<std::string> hashes2;
        hashes2.push_back(HASH_2);
        CheckChannelAccessRule(AID_1, hashes2, NfcNever);
        CheckChannelAccessRule(AID_3, hashes2, NfcAlways);

        std::vector<std::string> hashes3;
        CheckChannelAccessRule(AID_1, hashes3, NfcNever);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : NFC_TC_SE_AccessRulerTest_003_013
 * @tc.name      : AccessRulerTest_003_013
 * @tc.desc      : Access ruler test
 */
TEST_F(NfcTcSeTest, AccessRulerTest_003_013)
{
    try {
        // R0:AID.#1 (CER#1-Ref)- AR -NFC -ALWAYS
        // R1:AID.#ALL (ALL-Ref)- AR -NFC -NEVER
        std::string refData = REF_DO_A1_H1;
        std::shared_ptr<BerTlv> refBt = BerTlv::StrToBerTlv(refData);
        std::shared_ptr<RefDo> refDo = RefDo::BerTlvToRefDo(refBt);
        std::string refData2 = REF_DO_AID_ALL_HASH_ALL;
        std::shared_ptr<BerTlv> refBt2 = BerTlv::StrToBerTlv(refData2);
        std::shared_ptr<RefDo> refDo2 = RefDo::BerTlvToRefDo(refBt2);
        std::string arData = AR_DO_DATA_ONLY_NFC_ALWAYS;
        std::shared_ptr<BerTlv> arBt = BerTlv::StrToBerTlv(arData);
        std::shared_ptr<ArDo> arDo = ArDo::BerTlvToArDo(arBt);
        std::string arData2 = AR_DO_DATA_ONLY_NFC_NEVER;
        std::shared_ptr<BerTlv> arBt2 = BerTlv::StrToBerTlv(arData2);
        std::shared_ptr<ArDo> arDo2 = ArDo::BerTlvToArDo(arBt2);
        accessRuleCache_->AddAccessRule(refDo, arDo);
        accessRuleCache_->AddAccessRule(refDo2, arDo2);

        std::vector<std::string> hashes1;
        hashes1.push_back(HASH_1);
        CheckChannelAccessRule(AID_1, hashes1, NfcAlways);
        CheckChannelAccessRule(AID_3, hashes1, NfcNever);

        std::vector<std::string> hashes2;
        hashes2.push_back(HASH_2);
        CheckChannelAccessRule(AID_1, hashes2, NfcNever);
        CheckChannelAccessRule(AID_3, hashes2, NfcNever);

        std::vector<std::string> hashes3;
        CheckChannelAccessRule(AID_1, hashes3, NfcNever);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : NFC_TC_SE_AccessRulerTest_003_014
 * @tc.name      : AccessRulerTest_003_014
 * @tc.desc      : Access ruler test
 */
TEST_F(NfcTcSeTest, AccessRulerTest_003_014)
{
    try {
        // R0:AID.#1 (ALL-Ref)- AR -NFC-NEVER
        // R1:AID.#ALL (CER#1-Ref)- AR -NFC-ALWAYS
        std::string refData = REF_DO_HASH_ALL;
        std::shared_ptr<BerTlv> refBt = BerTlv::StrToBerTlv(refData);
        std::shared_ptr<RefDo> refDo = RefDo::BerTlvToRefDo(refBt);
        std::string refData2 = REF_DO_AID_ALL;
        std::shared_ptr<BerTlv> refBt2 = BerTlv::StrToBerTlv(refData2);
        std::shared_ptr<RefDo> refDo2 = RefDo::BerTlvToRefDo(refBt2);
        std::string arData = AR_DO_DATA_ONLY_NFC_NEVER;
        std::shared_ptr<BerTlv> arBt = BerTlv::StrToBerTlv(arData);
        std::shared_ptr<ArDo> arDo = ArDo::BerTlvToArDo(arBt);
        std::string arData2 = AR_DO_DATA_ONLY_NFC_ALWAYS;
        std::shared_ptr<BerTlv> arBt2 = BerTlv::StrToBerTlv(arData2);
        std::shared_ptr<ArDo> arDo2 = ArDo::BerTlvToArDo(arBt2);
        accessRuleCache_->AddAccessRule(refDo, arDo);
        accessRuleCache_->AddAccessRule(refDo2, arDo2);

        std::vector<std::string> hashes1;
        hashes1.push_back(HASH_AID_ALL);
        CheckChannelAccessRule(AID_HASH_ALL, hashes1, NfcNever);
        CheckChannelAccessRule(AID_2, hashes1, NfcAlways);

        std::vector<std::string> hashes2;
        hashes2.push_back(HASH_1);
        CheckChannelAccessRule(AID_HASH_ALL, hashes2, NfcNever);
        CheckChannelAccessRule(AID_2, hashes2, NfcNever);

        std::vector<std::string> hashes3;
        CheckChannelAccessRule(AID_HASH_ALL, hashes3, NfcNever);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : NFC_TC_SE_AccessRulerTest_003_015
 * @tc.name      : AccessRulerTest_003_015
 * @tc.desc      : Access ruler test
 */
TEST_F(NfcTcSeTest, AccessRulerTest_003_015)
{
    try {
        // R0:AID.#1 (ALL-Ref)- AR -NFC-ALWAYS
        // R1:AID.#ALL (CER#1-Ref)- AR -NFC-NEVER
        std::string refData = REF_DO_HASH_ALL;
        std::shared_ptr<BerTlv> refBt = BerTlv::StrToBerTlv(refData);
        std::shared_ptr<RefDo> refDo = RefDo::BerTlvToRefDo(refBt);
        std::string refData2 = REF_DO_AID_ALL;
        std::shared_ptr<BerTlv> refBt2 = BerTlv::StrToBerTlv(refData2);
        std::shared_ptr<RefDo> refDo2 = RefDo::BerTlvToRefDo(refBt2);
        std::string arData = AR_DO_DATA_ONLY_NFC_ALWAYS;
        std::shared_ptr<BerTlv> arBt = BerTlv::StrToBerTlv(arData);
        std::shared_ptr<ArDo> arDo = ArDo::BerTlvToArDo(arBt);
        std::string arData2 = AR_DO_DATA_ONLY_NFC_NEVER;
        std::shared_ptr<BerTlv> arBt2 = BerTlv::StrToBerTlv(arData2);
        std::shared_ptr<ArDo> arDo2 = ArDo::BerTlvToArDo(arBt2);
        accessRuleCache_->AddAccessRule(refDo, arDo);
        accessRuleCache_->AddAccessRule(refDo2, arDo2);

        std::vector<std::string> hashes1;
        hashes1.push_back(HASH_AID_ALL);
        CheckChannelAccessRule(AID_HASH_ALL, hashes1, NfcAlways);
        CheckChannelAccessRule(AID_2, hashes1, NfcNever);

        std::vector<std::string> hashes2;
        hashes2.push_back(HASH_1);
        CheckChannelAccessRule(AID_HASH_ALL, hashes2, NfcAlways);
        CheckChannelAccessRule(AID_2, hashes2, NfcNever);

        std::vector<std::string> hashes3;
        CheckChannelAccessRule(AID_HASH_ALL, hashes3, NfcNever);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}
}  // namespace OHOS::se::test