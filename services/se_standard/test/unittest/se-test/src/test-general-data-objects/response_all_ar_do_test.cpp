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
#include "general-data-objects/response_all_ar_do.h"

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
#include "general-data-objects/ref_ar_do.h"
#include "general-data-objects/ref_do.h"

using namespace OHOS::se::security;

namespace OHOS::se::test {
class ResponseAllArDoTest : public ::testing::Test {
public:
    virtual void SetUp() override
    {
        std::cout << " ResponseAllArDoTest set up." << std::endl;
    }

    virtual void TearDown() override
    {
        std::cout << " ResponseAllArDoTest tear down." << std::endl;
    }

public:
};

/**
 * @tc.number    : RESPONSE_ALL_AR_DO_TEST_001
 * @tc.name      : SHA1_Apdu_Always_Nfc_Always_Test
 * @tc.desc      : Generate response-all-ar-do with SHA-1 hash apdu-always and nfc-always
 */
TEST_F(ResponseAllArDoTest, SHA1_Apdu_Always_Nfc_Always_Test)
{
    try {
        std::string value = REF_DO_A1_H1 + AR_DO_DATA_11;
        int length = value.length();
        std::string data =
            STRING_RESPONSE_ALL_AR_DO_TAG + (char)(length + 2) + (STRING_REF_AR_DO_TAG + (char)length + value);
        std::shared_ptr<BerTlv> bt = BerTlv::StrToBerTlv(data);
        std::shared_ptr<ResponseAllArDo> raad = ResponseAllArDo::BerTlvToResponseAllArDo(bt);
        std::vector<RefArDo> radList = raad->GetRefArDoArray();
        EXPECT_EQ(1U, radList.size());
        RefArDo rad = radList[0];

        std::shared_ptr<ArDo> ad = rad.GetArDo();
        std::shared_ptr<ApduArDo> aad = ad->GetApduArDo();
        std::shared_ptr<NfcArDo> nad = ad->GetNfcArDo();
        EXPECT_TRUE(aad->IsApduAllowed());
        EXPECT_TRUE(aad->GetApduFilters().empty());
        EXPECT_TRUE(nad->IsNfcAllowed());

        std::shared_ptr<RefDo> rd = rad.GetRefDo();
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
 * @tc.number    : RESPONSE_ALL_AR_DO_TEST_002
 * @tc.name      : SHA256_Apdu_Always_Nfc_Always_Test
 * @tc.desc      : Generate response-all-ar-do with SHA-256 hash apdu-always and nfc-always
 */
TEST_F(ResponseAllArDoTest, SHA256_Apdu_Always_Nfc_Always_Test)
{
    try {
        std::string value = REF_DO_SHA256 + AR_DO_DATA_11;
        int length = value.length();
        std::string data =
            STRING_RESPONSE_ALL_AR_DO_TAG + (char)(length + 2) + (STRING_REF_AR_DO_TAG + (char)length + value);
        std::shared_ptr<BerTlv> bt = BerTlv::StrToBerTlv(data);
        std::shared_ptr<ResponseAllArDo> raad = ResponseAllArDo::BerTlvToResponseAllArDo(bt);
        std::vector<RefArDo> radList = raad->GetRefArDoArray();
        EXPECT_EQ(1U, radList.size());
        RefArDo rad = radList[0];

        std::shared_ptr<ArDo> ad = rad.GetArDo();
        std::shared_ptr<ApduArDo> aad = ad->GetApduArDo();
        std::shared_ptr<NfcArDo> nad = ad->GetNfcArDo();
        EXPECT_TRUE(aad->IsApduAllowed());
        EXPECT_TRUE(aad->GetApduFilters().empty());
        EXPECT_TRUE(nad->IsNfcAllowed());

        std::shared_ptr<RefDo> rd = rad.GetRefDo();
        std::shared_ptr<AidRefDo> ard = rd->GetAidRefDo();
        std::shared_ptr<HashRefDo> hrd = rd->GetHashRefDo();
        EXPECT_EQ(AID_SHA256, ard->GetAid());
        EXPECT_EQ(HASH_SHA256, hrd->GetHash());
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : RESPONSE_ALL_AR_DO_TEST_003
 * @tc.name      : Empty_Aid_Test
 * @tc.desc      : Generate response-all-ar-do with empty aid
 */
TEST_F(ResponseAllArDoTest, Empty_Aid_Test)
{
    try {
        std::string value = REF_DO_AID_ALL + AR_DO_DATA_11;
        int length = value.length();
        std::string data =
            STRING_RESPONSE_ALL_AR_DO_TAG + (char)(length + 2) + (STRING_REF_AR_DO_TAG + (char)length + value);
        std::shared_ptr<BerTlv> bt = BerTlv::StrToBerTlv(data);
        std::shared_ptr<ResponseAllArDo> raad = ResponseAllArDo::BerTlvToResponseAllArDo(bt);
        std::vector<RefArDo> radList = raad->GetRefArDoArray();
        EXPECT_EQ(1U, radList.size());
        RefArDo rad = radList[0];

        std::shared_ptr<ArDo> ad = rad.GetArDo();
        std::shared_ptr<ApduArDo> aad = ad->GetApduArDo();
        std::shared_ptr<NfcArDo> nad = ad->GetNfcArDo();
        EXPECT_TRUE(aad->IsApduAllowed());
        EXPECT_TRUE(aad->GetApduFilters().empty());
        EXPECT_TRUE(nad->IsNfcAllowed());

        std::shared_ptr<RefDo> rd = rad.GetRefDo();
        std::shared_ptr<AidRefDo> ard = rd->GetAidRefDo();
        std::shared_ptr<HashRefDo> hrd = rd->GetHashRefDo();
        EXPECT_EQ(AID_AID_ALL, ard->GetAid());
        EXPECT_EQ(HASH_AID_ALL, hrd->GetHash());
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : RESPONSE_ALL_AR_DO_TEST_004
 * @tc.name      : Empty_Hash_Test
 * @tc.desc      : Generate response-all-ar-do with empty hash
 */
TEST_F(ResponseAllArDoTest, Empty_Hash_Test)
{
    try {
        std::string value = REF_DO_HASH_ALL + AR_DO_DATA_11;
        int length = value.length();
        std::string data =
            STRING_RESPONSE_ALL_AR_DO_TAG + (char)(length + 2) + (STRING_REF_AR_DO_TAG + (char)length + value);
        std::shared_ptr<BerTlv> bt = BerTlv::StrToBerTlv(data);
        std::shared_ptr<ResponseAllArDo> raad = ResponseAllArDo::BerTlvToResponseAllArDo(bt);
        std::vector<RefArDo> radList = raad->GetRefArDoArray();
        EXPECT_EQ(1U, radList.size());
        RefArDo rad = radList[0];

        std::shared_ptr<ArDo> ad = rad.GetArDo();
        std::shared_ptr<ApduArDo> aad = ad->GetApduArDo();
        std::shared_ptr<NfcArDo> nad = ad->GetNfcArDo();
        EXPECT_TRUE(aad->IsApduAllowed());
        EXPECT_TRUE(aad->GetApduFilters().empty());
        EXPECT_TRUE(nad->IsNfcAllowed());

        std::shared_ptr<RefDo> rd = rad.GetRefDo();
        std::shared_ptr<AidRefDo> ard = rd->GetAidRefDo();
        std::shared_ptr<HashRefDo> hrd = rd->GetHashRefDo();
        EXPECT_EQ(AID_HASH_ALL, ard->GetAid());
        EXPECT_EQ(HASH_HASH_ALL, hrd->GetHash());
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : RESPONSE_ALL_AR_DO_TEST_005
 * @tc.name      : Apdu_Always_Nfc_Never_Test
 * @tc.desc      : Generate response-all-ar-do with apdu-always and nfc-never
 */
TEST_F(ResponseAllArDoTest, Apdu_Always_Nfc_Never_Test)
{
    try {
        std::string value = REF_DO_A1_H1 + AR_DO_DATA_10;
        int length = value.length();
        std::string data =
            STRING_RESPONSE_ALL_AR_DO_TAG + (char)(length + 2) + (STRING_REF_AR_DO_TAG + (char)length + value);
        std::shared_ptr<BerTlv> bt = BerTlv::StrToBerTlv(data);
        std::shared_ptr<ResponseAllArDo> raad = ResponseAllArDo::BerTlvToResponseAllArDo(bt);
        std::vector<RefArDo> radList = raad->GetRefArDoArray();
        EXPECT_EQ(1U, radList.size());
        RefArDo rad = radList[0];

        std::shared_ptr<ArDo> ad = rad.GetArDo();
        std::shared_ptr<ApduArDo> aad = ad->GetApduArDo();
        std::shared_ptr<NfcArDo> nad = ad->GetNfcArDo();
        EXPECT_TRUE(aad->IsApduAllowed());
        EXPECT_TRUE(aad->GetApduFilters().empty());
        EXPECT_FALSE(nad->IsNfcAllowed());

        std::shared_ptr<RefDo> rd = rad.GetRefDo();
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
 * @tc.number    : RESPONSE_ALL_AR_DO_TEST_006
 * @tc.name      : Apdu_Never_Nfc_Always_Test
 * @tc.desc      : Generate response-all-ar-do with apdu-never and nfc-always
 */
TEST_F(ResponseAllArDoTest, Apdu_Never_Nfc_Always_Test)
{
    try {
        std::string value = REF_DO_A1_H1 + AR_DO_DATA_01;
        int length = value.length();
        std::string data =
            STRING_RESPONSE_ALL_AR_DO_TAG + (char)(length + 2) + (STRING_REF_AR_DO_TAG + (char)length + value);
        std::shared_ptr<BerTlv> bt = BerTlv::StrToBerTlv(data);
        std::shared_ptr<ResponseAllArDo> raad = ResponseAllArDo::BerTlvToResponseAllArDo(bt);
        std::vector<RefArDo> radList = raad->GetRefArDoArray();
        EXPECT_EQ(1U, radList.size());
        RefArDo rad = radList[0];

        std::shared_ptr<ArDo> ad = rad.GetArDo();
        std::shared_ptr<ApduArDo> aad = ad->GetApduArDo();
        std::shared_ptr<NfcArDo> nad = ad->GetNfcArDo();
        EXPECT_FALSE(aad->IsApduAllowed());
        EXPECT_TRUE(aad->GetApduFilters().empty());
        EXPECT_TRUE(nad->IsNfcAllowed());

        std::shared_ptr<RefDo> rd = rad.GetRefDo();
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
 * @tc.number    : RESPONSE_ALL_AR_DO_TEST_007
 * @tc.name      : Apdu_Never_Nfc_Never_Test
 * @tc.desc      : Generate response-all-ar-do with apdu-never and nfc-never
 */
TEST_F(ResponseAllArDoTest, Apdu_Never_Nfc_Never_Test)
{
    try {
        std::string value = REF_DO_A1_H1 + AR_DO_DATA_00;
        int length = value.length();
        std::string data =
            STRING_RESPONSE_ALL_AR_DO_TAG + (char)(length + 2) + (STRING_REF_AR_DO_TAG + (char)length + value);
        std::shared_ptr<BerTlv> bt = BerTlv::StrToBerTlv(data);
        std::shared_ptr<ResponseAllArDo> raad = ResponseAllArDo::BerTlvToResponseAllArDo(bt);
        std::vector<RefArDo> radList = raad->GetRefArDoArray();
        EXPECT_EQ(1U, radList.size());
        RefArDo rad = radList[0];

        std::shared_ptr<ArDo> ad = rad.GetArDo();
        std::shared_ptr<ApduArDo> aad = ad->GetApduArDo();
        std::shared_ptr<NfcArDo> nad = ad->GetNfcArDo();
        EXPECT_FALSE(aad->IsApduAllowed());
        EXPECT_TRUE(aad->GetApduFilters().empty());
        EXPECT_FALSE(nad->IsNfcAllowed());

        std::shared_ptr<RefDo> rd = rad.GetRefDo();
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
 * @tc.number    : RESPONSE_ALL_AR_DO_TEST_008
 * @tc.name      : Apdu_Only_Test
 * @tc.desc      : Generate response-all-ar-do with apdu-only
 */
TEST_F(ResponseAllArDoTest, Apdu_Only_Test)
{
    try {
        std::string value = REF_DO_A1_H1 + AR_DO_DATA_ONLY_APDU_ALWAYS;
        int length = value.length();
        std::string data =
            STRING_RESPONSE_ALL_AR_DO_TAG + (char)(length + 2) + (STRING_REF_AR_DO_TAG + (char)length + value);
        std::shared_ptr<BerTlv> bt = BerTlv::StrToBerTlv(data);
        std::shared_ptr<ResponseAllArDo> raad = ResponseAllArDo::BerTlvToResponseAllArDo(bt);
        std::vector<RefArDo> radList = raad->GetRefArDoArray();
        EXPECT_EQ(1U, radList.size());
        RefArDo rad = radList[0];

        std::shared_ptr<ArDo> ad = rad.GetArDo();
        std::shared_ptr<ApduArDo> aad = ad->GetApduArDo();
        std::shared_ptr<NfcArDo> nad = ad->GetNfcArDo();
        EXPECT_TRUE(aad->IsApduAllowed());
        EXPECT_TRUE(aad->GetApduFilters().empty());
        EXPECT_FALSE(nad);

        std::shared_ptr<RefDo> rd = rad.GetRefDo();
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
 * @tc.number    : RESPONSE_ALL_AR_DO_TEST_009
 * @tc.name      : Nfc_Only_Test
 * @tc.desc      : Generate response-all-ar-do with nfc-only
 */
TEST_F(ResponseAllArDoTest, Nfc_Only_Test)
{
    try {
        std::string value = REF_DO_A1_H1 + AR_DO_DATA_ONLY_NFC_ALWAYS;
        int length = value.length();
        std::string data =
            STRING_RESPONSE_ALL_AR_DO_TAG + (char)(length + 2) + (STRING_REF_AR_DO_TAG + (char)length + value);
        std::shared_ptr<BerTlv> bt = BerTlv::StrToBerTlv(data);
        std::shared_ptr<ResponseAllArDo> raad = ResponseAllArDo::BerTlvToResponseAllArDo(bt);
        std::vector<RefArDo> radList = raad->GetRefArDoArray();
        EXPECT_EQ(1U, radList.size());
        RefArDo rad = radList[0];

        std::shared_ptr<ArDo> ad = rad.GetArDo();
        std::shared_ptr<ApduArDo> aad = ad->GetApduArDo();
        std::shared_ptr<NfcArDo> nad = ad->GetNfcArDo();
        EXPECT_FALSE(aad);
        EXPECT_TRUE(nad->IsNfcAllowed());

        std::shared_ptr<RefDo> rd = rad.GetRefDo();
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
 * @tc.number    : RESPONSE_ALL_AR_DO_TEST_010
 * @tc.name      : Apdu_Filter_Test
 * @tc.desc      : Generate response-all-ar-do with apdu-filter
 */
TEST_F(ResponseAllArDoTest, Apdu_Filter_Test)
{
    try {
        std::string value = REF_DO_A1_H1 + AR_DO_DATA_APDU_FILTER;
        int length = value.length();
        std::string data =
            STRING_RESPONSE_ALL_AR_DO_TAG + (char)(length + 2) + (STRING_REF_AR_DO_TAG + (char)length + value);
        std::shared_ptr<BerTlv> bt = BerTlv::StrToBerTlv(data);
        std::shared_ptr<ResponseAllArDo> raad = ResponseAllArDo::BerTlvToResponseAllArDo(bt);
        std::vector<RefArDo> radList = raad->GetRefArDoArray();
        EXPECT_EQ(1U, radList.size());
        RefArDo rad = radList[0];

        std::shared_ptr<ArDo> ad = rad.GetArDo();
        std::shared_ptr<ApduArDo> aad = ad->GetApduArDo();
        std::shared_ptr<NfcArDo> nad = ad->GetNfcArDo();
        EXPECT_TRUE(aad->IsApduAllowed());
        EXPECT_EQ(1U, aad->GetApduFilters().size());
        EXPECT_TRUE(nad);

        std::shared_ptr<RefDo> rd = rad.GetRefDo();
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
 * @tc.number    : RESPONSE_ALL_AR_DO_TEST_011
 * @tc.name      : Length_Over_127_Test
 * @tc.desc      : Generate response-all-ar-do with length over 0x7F
 */
TEST_F(ResponseAllArDoTest, Length_Over_127_Test)
{
    try {
        std::string value1 = REF_DO_A1_H2 + AR_DO_DATA_11;
        int length1 = value1.length();
        std::string value2 = REF_DO_SHA256 + AR_DO_DATA_11;
        int length2 = value2.length();
        std::string value3 = REF_DO_AID_ALL + AR_DO_DATA_11;
        int length3 = value3.length();
        std::string value4 = REF_DO_HASH_ALL + AR_DO_DATA_11;
        int length4 = value4.length();
        std::string value5 = REF_DO_A1_H1 + AR_DO_DATA_11;
        int length5 = value5.length();
        std::string value6 = REF_DO_A3_H3 + AR_DO_DATA_11;
        int length6 = value6.length();
        std::string value7 = REF_DO_A2_H1 + AR_DO_DATA_11;
        int length7 = value7.length();
        std::string responseAllArDoLength = {(char)0x82, (char)0x01, (char)0x31};
        std::string responseAllArDoData =
            STRING_RESPONSE_ALL_AR_DO_TAG + responseAllArDoLength + (STRING_REF_AR_DO_TAG + (char)length1 + value1) +
            (STRING_REF_AR_DO_TAG + (char)length2 + value2) + (STRING_REF_AR_DO_TAG + (char)length3 + value3) +
            (STRING_REF_AR_DO_TAG + (char)length4 + value4) + (STRING_REF_AR_DO_TAG + (char)length5 + value5) +
            (STRING_REF_AR_DO_TAG + (char)length6 + value6) + (STRING_REF_AR_DO_TAG + (char)length7 + value7);
        std::shared_ptr<BerTlv> bt = BerTlv::StrToBerTlv(responseAllArDoData);
        std::shared_ptr<ResponseAllArDo> raad = ResponseAllArDo::BerTlvToResponseAllArDo(bt);
        std::vector<RefArDo> radList = raad->GetRefArDoArray();

        std::shared_ptr<ArDo> ad;
        std::shared_ptr<ApduArDo> aad;
        std::shared_ptr<NfcArDo> nad;
        std::shared_ptr<RefDo> rd;
        std::shared_ptr<AidRefDo> ard;
        std::shared_ptr<HashRefDo> hrd;

        EXPECT_EQ(7U, radList.size());
        RefArDo rad = radList[0];
        ad = rad.GetArDo();
        aad = ad->GetApduArDo();
        nad = ad->GetNfcArDo();
        EXPECT_TRUE(aad->IsApduAllowed());
        EXPECT_TRUE(aad->GetApduFilters().empty());
        EXPECT_TRUE(nad);
        rd = rad.GetRefDo();
        ard = rd->GetAidRefDo();
        hrd = rd->GetHashRefDo();
        EXPECT_EQ(AID_1, ard->GetAid());
        EXPECT_EQ(HASH_2, hrd->GetHash());

        rad = radList[1];
        ad = rad.GetArDo();
        aad = ad->GetApduArDo();
        nad = ad->GetNfcArDo();
        EXPECT_TRUE(aad->IsApduAllowed());
        EXPECT_TRUE(aad->GetApduFilters().empty());
        EXPECT_TRUE(nad);
        rd = rad.GetRefDo();
        ard = rd->GetAidRefDo();
        hrd = rd->GetHashRefDo();
        EXPECT_EQ(AID_SHA256, ard->GetAid());
        EXPECT_EQ(HASH_SHA256, hrd->GetHash());

        rad = radList[2];
        ad = rad.GetArDo();
        aad = ad->GetApduArDo();
        nad = ad->GetNfcArDo();
        EXPECT_TRUE(aad->IsApduAllowed());
        EXPECT_TRUE(aad->GetApduFilters().empty());
        EXPECT_TRUE(nad);
        rd = rad.GetRefDo();
        ard = rd->GetAidRefDo();
        hrd = rd->GetHashRefDo();
        EXPECT_EQ(AID_AID_ALL, ard->GetAid());
        EXPECT_EQ(HASH_AID_ALL, hrd->GetHash());

        rad = radList[3];
        ad = rad.GetArDo();
        aad = ad->GetApduArDo();
        nad = ad->GetNfcArDo();
        EXPECT_TRUE(aad->IsApduAllowed());
        EXPECT_TRUE(aad->GetApduFilters().empty());
        EXPECT_TRUE(nad);
        rd = rad.GetRefDo();
        ard = rd->GetAidRefDo();
        hrd = rd->GetHashRefDo();
        EXPECT_EQ(AID_HASH_ALL, ard->GetAid());
        EXPECT_EQ(HASH_HASH_ALL, hrd->GetHash());

        rad = radList[4];
        ad = rad.GetArDo();
        aad = ad->GetApduArDo();
        nad = ad->GetNfcArDo();
        EXPECT_TRUE(aad->IsApduAllowed());
        EXPECT_TRUE(aad->GetApduFilters().empty());
        EXPECT_TRUE(nad);
        rd = rad.GetRefDo();
        ard = rd->GetAidRefDo();
        hrd = rd->GetHashRefDo();
        EXPECT_EQ(AID_1, ard->GetAid());
        EXPECT_EQ(HASH_1, hrd->GetHash());

        rad = radList[5];
        ad = rad.GetArDo();
        aad = ad->GetApduArDo();
        nad = ad->GetNfcArDo();
        EXPECT_TRUE(aad->IsApduAllowed());
        EXPECT_TRUE(aad->GetApduFilters().empty());
        EXPECT_TRUE(nad);
        rd = rad.GetRefDo();
        ard = rd->GetAidRefDo();
        hrd = rd->GetHashRefDo();
        EXPECT_EQ(AID_3, ard->GetAid());
        EXPECT_EQ(HASH_3, hrd->GetHash());

        rad = radList[6];
        ad = rad.GetArDo();
        aad = ad->GetApduArDo();
        nad = ad->GetNfcArDo();
        EXPECT_TRUE(aad->IsApduAllowed());
        EXPECT_TRUE(aad->GetApduFilters().empty());
        EXPECT_TRUE(nad);
        rd = rad.GetRefDo();
        ard = rd->GetAidRefDo();
        hrd = rd->GetHashRefDo();
        EXPECT_EQ(AID_2, ard->GetAid());
        EXPECT_EQ(HASH_1, hrd->GetHash());
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : RESPONSE_ALL_AR_DO_TEST_012
 * @tc.name      : Illegal_Tag_Test
 * @tc.desc      : Generate response-all-ar-do with illegal tag
 */
TEST_F(ResponseAllArDoTest, Illegal_Tag_Test)
{
    try {
        std::string value = REF_DO_A1_H1 + AR_DO_DATA_11;
        int length = value.length();
        std::string errorTag = {0x00};
        std::string data = errorTag + (char)(length + 2) + (STRING_REF_AR_DO_TAG + (char)length + value);

        std::shared_ptr<BerTlv> bt = BerTlv::StrToBerTlv(data);
        std::shared_ptr<ResponseAllArDo> raad = ResponseAllArDo::BerTlvToResponseAllArDo(bt);
        EXPECT_TRUE(false);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(true);
    }
}

/**
 * @tc.number    : RESPONSE_ALL_AR_DO_TEST_013
 * @tc.name      : Data_Not_Enough_Test
 * @tc.desc      : Generate response-all-ar-do with not enough value
 */
TEST_F(ResponseAllArDoTest, Data_Not_Enough_Test)
{
    try {
        std::string value = REF_DO_A1_H1 + AR_DO_DATA_11;
        int length = value.length();
        std::string data =
            STRING_RESPONSE_ALL_AR_DO_TAG + (char)(length + 20) + (STRING_REF_AR_DO_TAG + (char)length + value);

        std::shared_ptr<BerTlv> bt = BerTlv::StrToBerTlv(data);
        std::shared_ptr<ResponseAllArDo> raad = ResponseAllArDo::BerTlvToResponseAllArDo(bt);
        EXPECT_TRUE(false);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(true);
    }
}
}  // namespace OHOS::se::test