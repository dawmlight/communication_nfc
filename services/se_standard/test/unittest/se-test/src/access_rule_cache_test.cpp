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
#include "access_rule_cache.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>
#include <string>
#include <vector>

#include "access_rule_data_common.h"
#include "channel_access_rule.h"
#include "general-data-objects/ar_do.h"
#include "general-data-objects/ref_do.h"

using namespace OHOS::se::security;

namespace OHOS::se::test {
class AccessRuleCacheTest : public ::testing::Test {
public:
    virtual void SetUp() override
    {
        std::cout << " AccessRuleCacheTest set up." << std::endl;
    }

    virtual void TearDown() override
    {
        std::cout << " AccessRuleCacheTest tear down." << std::endl;
    }

public:
};

/**
 * @tc.number    : ACCESS_RULE_CACHE_TEST_001
 * @tc.name      : CompareRefreshTag_Test
 * @tc.desc      : Compare refresh tag
 */
TEST_F(AccessRuleCacheTest, CompareRefreshTag_Test)
{
    try {
        std::shared_ptr<AccessRuleCache> accessRuleCache = std::make_shared<AccessRuleCache>();
        EXPECT_FALSE(accessRuleCache->CompareRefreshTag(RESPONSE_REFRESH_TAG));

        accessRuleCache->SetRefreshTag(RESPONSE_REFRESH_TAG);
        EXPECT_TRUE(accessRuleCache->CompareRefreshTag(RESPONSE_REFRESH_TAG));
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : ACCESS_RULE_CACHE_TEST_002
 * @tc.name      : GetAccessRule_Hash_Aid_Test
 * @tc.desc      : Get access rule with normal aid and normal hash
 */
TEST_F(AccessRuleCacheTest, GetAccessRule_Hash_Aid_Test)
{
    try {
        std::shared_ptr<AccessRuleCache> accessRuleCache = std::make_shared<AccessRuleCache>();
        std::string refData = REF_DO_A1_H1;
        std::shared_ptr<BerTlv> refBt = BerTlv::StrToBerTlv(refData);
        std::shared_ptr<RefDo> refDo = RefDo::BerTlvToRefDo(refBt);
        std::string arData = AR_DO_DATA_11;
        std::shared_ptr<BerTlv> arBt = BerTlv::StrToBerTlv(arData);
        std::shared_ptr<ArDo> arDo = ArDo::BerTlvToArDo(arBt);
        accessRuleCache->AddAccessRule(refDo, arDo);

        std::vector<std::string> hashes;
        hashes.push_back(HASH_1);
        ChannelAccessRule channelAccessRule = accessRuleCache->GetAccessRule(AID_1, hashes);
        EXPECT_EQ(ChannelAccessRule::ACCESSRULE::ALWAYS, channelAccessRule.GetAccessRule());
        EXPECT_EQ(ChannelAccessRule::ACCESSRULE::ALWAYS, channelAccessRule.GetApduAccessRule());
        EXPECT_FALSE(channelAccessRule.HasApduFilter());
        EXPECT_EQ(ChannelAccessRule::ACCESSRULE::ALWAYS, channelAccessRule.GetNFCEventAccessRule());
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : ACCESS_RULE_CACHE_TEST_003
 * @tc.name      : GetAccessRule_Hash_AidAll_Test
 * @tc.desc      : Get access rule with aid-all and normal hash
 */
TEST_F(AccessRuleCacheTest, GetAccessRule_Hash_AidAll_Test)
{
    try {
        std::shared_ptr<AccessRuleCache> accessRuleCache = std::make_shared<AccessRuleCache>();
        std::string refData = REF_DO_AID_ALL;
        std::shared_ptr<BerTlv> refBt = BerTlv::StrToBerTlv(refData);
        std::shared_ptr<RefDo> refDo = RefDo::BerTlvToRefDo(refBt);
        std::string arData = AR_DO_DATA_11;
        std::shared_ptr<BerTlv> arBt = BerTlv::StrToBerTlv(arData);
        std::shared_ptr<ArDo> arDo = ArDo::BerTlvToArDo(arBt);
        accessRuleCache->AddAccessRule(refDo, arDo);

        std::vector<std::string> hashes;
        hashes.push_back(HASH_AID_ALL);
        ChannelAccessRule channelAccessRule = accessRuleCache->GetAccessRule(AID_1, hashes);
        EXPECT_EQ(ChannelAccessRule::ACCESSRULE::ALWAYS, channelAccessRule.GetAccessRule());
        EXPECT_EQ(ChannelAccessRule::ACCESSRULE::ALWAYS, channelAccessRule.GetApduAccessRule());
        EXPECT_FALSE(channelAccessRule.HasApduFilter());
        EXPECT_EQ(ChannelAccessRule::ACCESSRULE::ALWAYS, channelAccessRule.GetNFCEventAccessRule());
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : ACCESS_RULE_CACHE_TEST_004
 * @tc.name      : GetAccessRule_HashAll_Aid_Test
 * @tc.desc      : Get access rule with normal aid and hash-all
 */
TEST_F(AccessRuleCacheTest, GetAccessRule_HashAll_Aid_Test)
{
    try {
        std::shared_ptr<AccessRuleCache> accessRuleCache = std::make_shared<AccessRuleCache>();
        std::string refData = REF_DO_HASH_ALL;
        std::shared_ptr<BerTlv> refBt = BerTlv::StrToBerTlv(refData);
        std::shared_ptr<RefDo> refDo = RefDo::BerTlvToRefDo(refBt);
        std::string arData = AR_DO_DATA_11;
        std::shared_ptr<BerTlv> arBt = BerTlv::StrToBerTlv(arData);
        std::shared_ptr<ArDo> arDo = ArDo::BerTlvToArDo(arBt);
        accessRuleCache->AddAccessRule(refDo, arDo);

        std::vector<std::string> hashes;
        hashes.push_back(HASH_1);
        ChannelAccessRule channelAccessRule = accessRuleCache->GetAccessRule(AID_HASH_ALL, hashes);
        EXPECT_EQ(ChannelAccessRule::ACCESSRULE::ALWAYS, channelAccessRule.GetAccessRule());
        EXPECT_EQ(ChannelAccessRule::ACCESSRULE::ALWAYS, channelAccessRule.GetApduAccessRule());
        EXPECT_FALSE(channelAccessRule.HasApduFilter());
        EXPECT_EQ(ChannelAccessRule::ACCESSRULE::ALWAYS, channelAccessRule.GetNFCEventAccessRule());
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : ACCESS_RULE_CACHE_TEST_005
 * @tc.name      : GetAccessRule_HashAll_AidAll_Test
 * @tc.desc      : Get access rule with aid-all and hash-all
 */
TEST_F(AccessRuleCacheTest, GetAccessRule_HashAll_AidAll_Test)
{
    try {
        std::shared_ptr<AccessRuleCache> accessRuleCache = std::make_shared<AccessRuleCache>();
        std::string refData = REF_DO_AID_ALL_HASH_ALL;
        std::shared_ptr<BerTlv> refBt = BerTlv::StrToBerTlv(refData);
        std::shared_ptr<RefDo> refDo = RefDo::BerTlvToRefDo(refBt);
        std::string arData = AR_DO_DATA_11;
        std::shared_ptr<BerTlv> arBt = BerTlv::StrToBerTlv(arData);
        std::shared_ptr<ArDo> arDo = ArDo::BerTlvToArDo(arBt);
        accessRuleCache->AddAccessRule(refDo, arDo);

        std::vector<std::string> hashes;
        hashes.push_back(HASH_1);
        ChannelAccessRule channelAccessRule = accessRuleCache->GetAccessRule(AID_1, hashes);
        EXPECT_EQ(ChannelAccessRule::ACCESSRULE::ALWAYS, channelAccessRule.GetAccessRule());
        EXPECT_EQ(ChannelAccessRule::ACCESSRULE::ALWAYS, channelAccessRule.GetApduAccessRule());
        EXPECT_FALSE(channelAccessRule.HasApduFilter());
        EXPECT_EQ(ChannelAccessRule::ACCESSRULE::ALWAYS, channelAccessRule.GetNFCEventAccessRule());
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : ACCESS_RULE_CACHE_TEST_006
 * @tc.name      : GetAccessRule_DiffHash_Aid_Test
 * @tc.desc      : Get access rule with normal aid and different hash
 */
TEST_F(AccessRuleCacheTest, GetAccessRule_DiffHash_Aid_Test)
{
    try {
        std::shared_ptr<AccessRuleCache> accessRuleCache = std::make_shared<AccessRuleCache>();
        std::string refData = REF_DO_A1_H1;
        std::shared_ptr<BerTlv> refBt = BerTlv::StrToBerTlv(refData);
        std::shared_ptr<RefDo> refDo = RefDo::BerTlvToRefDo(refBt);
        std::string arData = AR_DO_DATA_11;
        std::shared_ptr<BerTlv> arBt = BerTlv::StrToBerTlv(arData);
        std::shared_ptr<ArDo> arDo = ArDo::BerTlvToArDo(arBt);
        accessRuleCache->AddAccessRule(refDo, arDo);

        std::vector<std::string> hashes;
        hashes.push_back(HASH_2);
        ChannelAccessRule channelAccessRule = accessRuleCache->GetAccessRule(AID_1, hashes);
        EXPECT_EQ(ChannelAccessRule::ACCESSRULE::NEVER, channelAccessRule.GetAccessRule());
        EXPECT_EQ(ChannelAccessRule::ACCESSRULE::NEVER, channelAccessRule.GetApduAccessRule());
        EXPECT_FALSE(channelAccessRule.HasApduFilter());
        EXPECT_EQ(ChannelAccessRule::ACCESSRULE::NEVER, channelAccessRule.GetNFCEventAccessRule());
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : ACCESS_RULE_CACHE_TEST_007
 * @tc.name      : GetAccessRule_DiffHash_AllAid_Test
 * @tc.desc      : Get access rule with aid-all and different hash
 */
TEST_F(AccessRuleCacheTest, GetAccessRule_DiffHash_AllAid_Test)
{
    try {
        std::shared_ptr<AccessRuleCache> accessRuleCache = std::make_shared<AccessRuleCache>();
        std::string refData = REF_DO_AID_ALL;
        std::shared_ptr<BerTlv> refBt = BerTlv::StrToBerTlv(refData);
        std::shared_ptr<RefDo> refDo = RefDo::BerTlvToRefDo(refBt);
        std::string arData = AR_DO_DATA_11;
        std::shared_ptr<BerTlv> arBt = BerTlv::StrToBerTlv(arData);
        std::shared_ptr<ArDo> arDo = ArDo::BerTlvToArDo(arBt);
        accessRuleCache->AddAccessRule(refDo, arDo);

        std::vector<std::string> hashes;
        hashes.push_back(HASH_1);
        ChannelAccessRule channelAccessRule = accessRuleCache->GetAccessRule(AID_1, hashes);
        EXPECT_EQ(ChannelAccessRule::ACCESSRULE::NEVER, channelAccessRule.GetAccessRule());
        EXPECT_EQ(ChannelAccessRule::ACCESSRULE::NEVER, channelAccessRule.GetApduAccessRule());
        EXPECT_FALSE(channelAccessRule.HasApduFilter());
        EXPECT_EQ(ChannelAccessRule::ACCESSRULE::NEVER, channelAccessRule.GetNFCEventAccessRule());
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : ACCESS_RULE_CACHE_TEST_008
 * @tc.name      : GetAccessRule_Hash_DiffAid_Test
 * @tc.desc      : Get access rule with different aid and normal hash
 */
TEST_F(AccessRuleCacheTest, GetAccessRule_Hash_DiffAid_Test)
{
    try {
        std::shared_ptr<AccessRuleCache> accessRuleCache = std::make_shared<AccessRuleCache>();
        std::string refData = REF_DO_A1_H1;
        std::shared_ptr<BerTlv> refBt = BerTlv::StrToBerTlv(refData);
        std::shared_ptr<RefDo> refDo = RefDo::BerTlvToRefDo(refBt);
        std::string arData = AR_DO_DATA_11;
        std::shared_ptr<BerTlv> arBt = BerTlv::StrToBerTlv(arData);
        std::shared_ptr<ArDo> arDo = ArDo::BerTlvToArDo(arBt);
        accessRuleCache->AddAccessRule(refDo, arDo);

        std::vector<std::string> hashes;
        hashes.push_back(HASH_1);
        ChannelAccessRule channelAccessRule = accessRuleCache->GetAccessRule(AID_2, hashes);
        EXPECT_EQ(ChannelAccessRule::ACCESSRULE::NEVER, channelAccessRule.GetAccessRule());
        EXPECT_EQ(ChannelAccessRule::ACCESSRULE::NEVER, channelAccessRule.GetApduAccessRule());
        EXPECT_FALSE(channelAccessRule.HasApduFilter());
        EXPECT_EQ(ChannelAccessRule::ACCESSRULE::NEVER, channelAccessRule.GetNFCEventAccessRule());
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : ACCESS_RULE_CACHE_TEST_009
 * @tc.name      : GetAccessRule_AllHash_DiffAid_Test
 * @tc.desc      : Get access rule with different aid and hash-all
 */
TEST_F(AccessRuleCacheTest, GetAccessRule_AllHash_DiffAid_Test)
{
    try {
        std::shared_ptr<AccessRuleCache> accessRuleCache = std::make_shared<AccessRuleCache>();
        std::string refData = REF_DO_HASH_ALL;
        std::shared_ptr<BerTlv> refBt = BerTlv::StrToBerTlv(refData);
        std::shared_ptr<RefDo> refDo = RefDo::BerTlvToRefDo(refBt);
        std::string arData = AR_DO_DATA_11;
        std::shared_ptr<BerTlv> arBt = BerTlv::StrToBerTlv(arData);
        std::shared_ptr<ArDo> arDo = ArDo::BerTlvToArDo(arBt);
        accessRuleCache->AddAccessRule(refDo, arDo);

        std::vector<std::string> hashes;
        hashes.push_back(HASH_1);
        ChannelAccessRule channelAccessRule = accessRuleCache->GetAccessRule(AID_1, hashes);
        EXPECT_EQ(ChannelAccessRule::ACCESSRULE::NEVER, channelAccessRule.GetAccessRule());
        EXPECT_EQ(ChannelAccessRule::ACCESSRULE::NEVER, channelAccessRule.GetApduAccessRule());
        EXPECT_FALSE(channelAccessRule.HasApduFilter());
        EXPECT_EQ(ChannelAccessRule::ACCESSRULE::NEVER, channelAccessRule.GetNFCEventAccessRule());
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : ACCESS_RULE_CACHE_TEST_010
 * @tc.name      : GetAccessRule_NoRule_Test
 * @tc.desc      : Get access rule with no rules
 */
TEST_F(AccessRuleCacheTest, GetAccessRule_NoRule_Test)
{
    try {
        std::shared_ptr<AccessRuleCache> accessRuleCache = std::make_shared<AccessRuleCache>();
        
        std::vector<std::string> hashes;
        hashes.push_back(HASH_1);
        ChannelAccessRule channelAccessRule = accessRuleCache->GetAccessRule(AID_1, hashes);
        EXPECT_EQ(ChannelAccessRule::ACCESSRULE::NEVER, channelAccessRule.GetAccessRule());
        EXPECT_EQ(ChannelAccessRule::ACCESSRULE::NEVER, channelAccessRule.GetApduAccessRule());
        EXPECT_FALSE(channelAccessRule.HasApduFilter());
        EXPECT_EQ(ChannelAccessRule::ACCESSRULE::NEVER, channelAccessRule.GetNFCEventAccessRule());
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : ACCESS_RULE_CACHE_TEST_011
 * @tc.name      : AddAccessRule_Never_Always_Test
 * @tc.desc      : Add access rule with apdu-never and apdu-always, nfc-never and nfc-always
 */
TEST_F(AccessRuleCacheTest, AddAccessRule_Never_Always_Test)
{
    try {
        std::shared_ptr<AccessRuleCache> accessRuleCache = std::make_shared<AccessRuleCache>();
        std::string refData = REF_DO_A1_H1;
        std::shared_ptr<BerTlv> refBt = BerTlv::StrToBerTlv(refData);
        std::shared_ptr<RefDo> refDo = RefDo::BerTlvToRefDo(refBt);
        std::string arData = AR_DO_DATA_11;
        std::shared_ptr<BerTlv> arBt = BerTlv::StrToBerTlv(arData);
        std::shared_ptr<ArDo> arDo = ArDo::BerTlvToArDo(arBt);
        std::string arData2 = AR_DO_DATA_00;
        std::shared_ptr<BerTlv> arBt2 = BerTlv::StrToBerTlv(arData2);
        std::shared_ptr<ArDo> arDo2 = ArDo::BerTlvToArDo(arBt2);
        accessRuleCache->AddAccessRule(refDo, arDo);
        accessRuleCache->AddAccessRule(refDo, arDo2);

        std::vector<std::string> hashes;
        hashes.push_back(HASH_1);
        ChannelAccessRule channelAccessRule = accessRuleCache->GetAccessRule(AID_1, hashes);
        EXPECT_EQ(ChannelAccessRule::ACCESSRULE::NEVER, channelAccessRule.GetAccessRule());
        EXPECT_EQ(ChannelAccessRule::ACCESSRULE::NEVER, channelAccessRule.GetApduAccessRule());
        EXPECT_FALSE(channelAccessRule.HasApduFilter());
        EXPECT_EQ(ChannelAccessRule::ACCESSRULE::NEVER, channelAccessRule.GetNFCEventAccessRule());
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : ACCESS_RULE_CACHE_TEST_012
 * @tc.name      : AddAccessRule_Never_Filter_Test
 * @tc.desc      : Add access rule with apdu-never and apdu-filter
 */
TEST_F(AccessRuleCacheTest, AddAccessRule_Never_Filter_Test)
{
    try {
        std::shared_ptr<AccessRuleCache> accessRuleCache = std::make_shared<AccessRuleCache>();
        std::string refData = REF_DO_A1_H1;
        std::shared_ptr<BerTlv> refBt = BerTlv::StrToBerTlv(refData);
        std::shared_ptr<RefDo> refDo = RefDo::BerTlvToRefDo(refBt);
        std::string arData = AR_DO_DATA_00;
        std::shared_ptr<BerTlv> arBt = BerTlv::StrToBerTlv(arData);
        std::shared_ptr<ArDo> arDo = ArDo::BerTlvToArDo(arBt);
        std::string arData2 = AR_DO_DATA_APDU_FILTER;
        std::shared_ptr<BerTlv> arBt2 = BerTlv::StrToBerTlv(arData2);
        std::shared_ptr<ArDo> arDo2 = ArDo::BerTlvToArDo(arBt2);
        accessRuleCache->AddAccessRule(refDo, arDo);
        accessRuleCache->AddAccessRule(refDo, arDo2);

        std::vector<std::string> hashes;
        hashes.push_back(HASH_1);
        ChannelAccessRule channelAccessRule = accessRuleCache->GetAccessRule(AID_1, hashes);
        EXPECT_EQ(ChannelAccessRule::ACCESSRULE::NEVER, channelAccessRule.GetAccessRule());
        EXPECT_EQ(ChannelAccessRule::ACCESSRULE::NEVER, channelAccessRule.GetApduAccessRule());
        EXPECT_FALSE(channelAccessRule.HasApduFilter());
        EXPECT_EQ(ChannelAccessRule::ACCESSRULE::NEVER, channelAccessRule.GetNFCEventAccessRule());
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : ACCESS_RULE_CACHE_TEST_013
 * @tc.name      : AddAccessRule_Filter_Always_Test
 * @tc.desc      : Add access rule with apdu-filter and apdu-always
 */
TEST_F(AccessRuleCacheTest, AddAccessRule_Filter_Always_Test)
{
    try {
        std::shared_ptr<AccessRuleCache> accessRuleCache = std::make_shared<AccessRuleCache>();
        std::string refData = REF_DO_A1_H1;
        std::shared_ptr<BerTlv> refBt = BerTlv::StrToBerTlv(refData);
        std::shared_ptr<RefDo> refDo = RefDo::BerTlvToRefDo(refBt);
        std::string arData = AR_DO_DATA_11;
        std::shared_ptr<BerTlv> arBt = BerTlv::StrToBerTlv(arData);
        std::shared_ptr<ArDo> arDo = ArDo::BerTlvToArDo(arBt);
        std::string arData2 = AR_DO_DATA_APDU_FILTER;
        std::shared_ptr<BerTlv> arBt2 = BerTlv::StrToBerTlv(arData2);
        std::shared_ptr<ArDo> arDo2 = ArDo::BerTlvToArDo(arBt2);
        accessRuleCache->AddAccessRule(refDo, arDo);
        accessRuleCache->AddAccessRule(refDo, arDo2);

        std::vector<std::string> hashes;
        hashes.push_back(HASH_1);
        ChannelAccessRule channelAccessRule = accessRuleCache->GetAccessRule(AID_1, hashes);
        EXPECT_EQ(ChannelAccessRule::ACCESSRULE::ALWAYS, channelAccessRule.GetAccessRule());
        EXPECT_TRUE(channelAccessRule.HasApduFilter());
        EXPECT_EQ(1U, channelAccessRule.GetApduFilters().size());
        EXPECT_EQ(ChannelAccessRule::ACCESSRULE::ALWAYS, channelAccessRule.GetNFCEventAccessRule());
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : ACCESS_RULE_CACHE_TEST_014
 * @tc.name      : AddAccessRule_Same_Test
 * @tc.desc      : Add same access rule
 */
TEST_F(AccessRuleCacheTest, AddAccessRule_Same_Test)
{
    try {
        std::shared_ptr<AccessRuleCache> accessRuleCache = std::make_shared<AccessRuleCache>();
        std::string refData = REF_DO_A1_H1;
        std::shared_ptr<BerTlv> refBt = BerTlv::StrToBerTlv(refData);
        std::shared_ptr<RefDo> refDo = RefDo::BerTlvToRefDo(refBt);
        std::string arData = AR_DO_DATA_11;
        std::shared_ptr<BerTlv> arBt = BerTlv::StrToBerTlv(arData);
        std::shared_ptr<ArDo> arDo = ArDo::BerTlvToArDo(arBt);
        std::string arData2 = AR_DO_DATA_11;
        std::shared_ptr<BerTlv> arBt2 = BerTlv::StrToBerTlv(arData2);
        std::shared_ptr<ArDo> arDo2 = ArDo::BerTlvToArDo(arBt2);
        accessRuleCache->AddAccessRule(refDo, arDo);
        accessRuleCache->AddAccessRule(refDo, arDo2);

        std::vector<std::string> hashes;
        hashes.push_back(HASH_1);
        ChannelAccessRule channelAccessRule = accessRuleCache->GetAccessRule(AID_1, hashes);
        EXPECT_EQ(ChannelAccessRule::ACCESSRULE::ALWAYS, channelAccessRule.GetAccessRule());
        EXPECT_EQ(ChannelAccessRule::ACCESSRULE::ALWAYS, channelAccessRule.GetApduAccessRule());
        EXPECT_FALSE(channelAccessRule.HasApduFilter());
        EXPECT_EQ(ChannelAccessRule::ACCESSRULE::ALWAYS, channelAccessRule.GetNFCEventAccessRule());
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : ACCESS_RULE_CACHE_TEST_015
 * @tc.name      : AddAccessRule_Filter_Combine_Test
 * @tc.desc      : Add access rule with two apdu-filter
 */
TEST_F(AccessRuleCacheTest, AddAccessRule_Filter_Combine_Test)
{
    try {
        std::shared_ptr<AccessRuleCache> accessRuleCache = std::make_shared<AccessRuleCache>();
        std::string refData = REF_DO_A1_H1;
        std::shared_ptr<BerTlv> refBt = BerTlv::StrToBerTlv(refData);
        std::shared_ptr<RefDo> refDo = RefDo::BerTlvToRefDo(refBt);
        std::string arData = AR_DO_DATA_APDU_FILTER;
        std::shared_ptr<BerTlv> arBt = BerTlv::StrToBerTlv(arData);
        std::shared_ptr<ArDo> arDo = ArDo::BerTlvToArDo(arBt);
        std::string arData2 = AR_DO_DATA_APDU_FILTER_2;
        std::shared_ptr<BerTlv> arBt2 = BerTlv::StrToBerTlv(arData2);
        std::shared_ptr<ArDo> arDo2 = ArDo::BerTlvToArDo(arBt2);
        accessRuleCache->AddAccessRule(refDo, arDo);
        accessRuleCache->AddAccessRule(refDo, arDo2);

        std::vector<std::string> hashes;
        hashes.push_back(HASH_1);
        ChannelAccessRule channelAccessRule = accessRuleCache->GetAccessRule(AID_1, hashes);
        EXPECT_EQ(ChannelAccessRule::ACCESSRULE::ALWAYS, channelAccessRule.GetAccessRule());
        EXPECT_TRUE(channelAccessRule.HasApduFilter());
        EXPECT_EQ(2U, channelAccessRule.GetApduFilters().size());
        EXPECT_EQ(ChannelAccessRule::ACCESSRULE::ALWAYS, channelAccessRule.GetNFCEventAccessRule());
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : ACCESS_RULE_CACHE_TEST_016
 * @tc.name      : CheckCarrierPrivilege_Test
 * @tc.desc      : Checl carrier privilege
 */
TEST_F(AccessRuleCacheTest, CheckCarrierPrivilege_Test)
{
    try {
        std::shared_ptr<AccessRuleCache> accessRuleCache = std::make_shared<AccessRuleCache>();
        std::string packageName = {};
        std::vector<std::string> hashes;
        EXPECT_FALSE(accessRuleCache->CheckCarrierPrivilege(packageName, hashes));
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : ACCESS_RULE_CACHE_TEST_017
 * @tc.name      : ClearAccessRules_Test
 * @tc.desc      : Clear access rules
 */
TEST_F(AccessRuleCacheTest, ClearAccessRules_Test)
{
    try {
        std::shared_ptr<AccessRuleCache> accessRuleCache = std::make_shared<AccessRuleCache>();
        std::string refData = REF_DO_A1_H2;
        std::shared_ptr<BerTlv> refBt = BerTlv::StrToBerTlv(refData);
        std::shared_ptr<RefDo> refDo = RefDo::BerTlvToRefDo(refBt);
        std::string arData = AR_DO_DATA_11;
        std::shared_ptr<BerTlv> arBt = BerTlv::StrToBerTlv(arData);
        std::shared_ptr<ArDo> arDo = ArDo::BerTlvToArDo(arBt);
        accessRuleCache->AddAccessRule(refDo, arDo);

        std::vector<std::string> hashes;
        hashes.push_back(HASH_2);
        ChannelAccessRule channelAccessRule = accessRuleCache->GetAccessRule(AID_1, hashes);
        EXPECT_EQ(ChannelAccessRule::ACCESSRULE::ALWAYS, channelAccessRule.GetAccessRule());
        EXPECT_EQ(ChannelAccessRule::ACCESSRULE::ALWAYS, channelAccessRule.GetApduAccessRule());
        EXPECT_FALSE(channelAccessRule.HasApduFilter());
        EXPECT_EQ(ChannelAccessRule::ACCESSRULE::ALWAYS, channelAccessRule.GetNFCEventAccessRule());

        accessRuleCache->ClearAccessRules();

        ChannelAccessRule channelAccessRule2 = accessRuleCache->GetAccessRule(AID_1, hashes);
        EXPECT_EQ(ChannelAccessRule::ACCESSRULE::NEVER, channelAccessRule2.GetAccessRule());
        EXPECT_EQ(ChannelAccessRule::ACCESSRULE::NEVER, channelAccessRule2.GetApduAccessRule());
        EXPECT_FALSE(channelAccessRule2.HasApduFilter());
        EXPECT_EQ(ChannelAccessRule::ACCESSRULE::NEVER, channelAccessRule2.GetNFCEventAccessRule());
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}
}  // namespace OHOS::se::test