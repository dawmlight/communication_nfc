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

#include <iostream>
#include <memory>
#include <string>

#include "card_emulation.h"
#include "card_emulation_service_info_lite.h"
#include "element_name.h"
#include "ihost_card_emulation.h"
#include "loghelper.h"
#include "nfc_agent.h"
#include "tc_nfc_ce_host_service_data.h"
#include "tc_nfc_ce_offhost_service_data.h"
#include "test_nfc_ce_ability.h"
#include "test_nfc_ce_agent.h"
#include "want.h"

using namespace testing;
using namespace std;
using namespace OHOS::AAFwk;
using namespace OHOS::AppExecFwk;
using namespace OHOS::nfc::sdk::cardemulation;
namespace OHOS::nfc::test {
class TcNfcCeHostServiceTest : public testing::Test {
public:
    virtual void SetUp() override
    {
        std::cout << " SetUp TcNfcCeHostServiceTest." << std::endl;
        mCeHostServiceData_ = std::make_shared<TcNfcCeHostServiceData>();
        mCeHostServiceData_N = std::make_shared<TcNfcCeHostServiceData>();
        mCeOffHostServiceData_ = std::make_shared<TcNfcCeOffHostServiceData>();
    }

    virtual void TearDown() override
    {
        std::cout << " TearDown TcNfcCeHostServiceTest." << std::endl;
        if (mCeHostServiceData_->GetTestNfcCEAgent()) {
            mCeHostServiceData_->StopHceTest();
        }
        if (mCeHostServiceData_N->GetTestNfcCEAgent()) {
            mCeHostServiceData_N->StopHceTest();
        }

        if (mCeOffHostServiceData_->GetTestNfcCEAgent()) {
            mCeOffHostServiceData_->StopOffHceLiteTest();
        }
    }

public:
    std::shared_ptr<TcNfcCeHostServiceData> mCeHostServiceData_;
    std::shared_ptr<TcNfcCeHostServiceData> mCeHostServiceData_N;
    std::shared_ptr<TcNfcCeOffHostServiceData> mCeOffHostServiceData_;
};

/**
 * @tc.number    : NFC_TC_CE_HostTest_001_001
 * @tc.name      : set default route as ESE1, App1 has installed in DH, set App1 as PrimaryService, add fully matched
 * security AIDS
 * @tc.desc      :
 */
TEST_F(TcNfcCeHostServiceTest, AddAidsForService_Test101)
{
    DebugLog("AddAidsForService_Test101 in");
    mCeHostServiceData_->StartHceTest(TcNfcCeData::HOST_SECURE_ABILITY_NAME_1, TcNfcCeData::ABILITY_TYPE_SECURE);
    mCeHostServiceData_->SetPrimaryServiceTest();
    mCeHostServiceData_->AddDynamicAidsTest(false);
    mCeHostServiceData_->RemoveAidsTestOk(false);
}

/**
 * @tc.number    : NFC_TC_CE_HostTest_001_002
 * @tc.name      : set default route as ESE1, App1 has installed in DH, set App1 as PrimaryService, add prefix matched
 * security AIDS
 * @tc.desc      :
 */
TEST_F(TcNfcCeHostServiceTest, AddAidsForService_Test102)
{
    DebugLog("AddAidsForService_Test102 in");
    mCeHostServiceData_->StartHceTest(TcNfcCeData::PRE_HOST_SECURE_ABILITY_NAME_1, TcNfcCeData::ABILITY_TYPE_SECURE);
    mCeHostServiceData_->SetPrimaryServiceTest();
    mCeHostServiceData_->AddDynamicAidsTest(false);
    mCeHostServiceData_->RemoveAidsTestOk(false);
}

/**
 * @tc.number    : NFC_TC_CE_HostTest_001_003
 * @tc.name      : set default route as ESE1, App1 has installed in DH, set App1 as PrimaryService, add fully matched
 * security AIDS with different length
 * @tc.desc      :
 */
TEST_F(TcNfcCeHostServiceTest, AddAidsForService_Test103)
{
    DebugLog("AddAidsForService_Test103 in");
    mCeHostServiceData_->StartHceTest(TcNfcCeData::HOST_SECURE_ABILITY_NAME_1, TcNfcCeData::ABILITY_TYPE_SECURE);
    mCeHostServiceData_->SetPrimaryServiceTest();

    std::vector<std::string> testAids;
    testAids.push_back(TcNfcCeData::AID_LEN_10);
    mCeHostServiceData_->AddDynamicAidsTestWithAids(false, testAids);

    testAids.clear();
    testAids.push_back(TcNfcCeData::AID_LEN_18);
    mCeHostServiceData_->AddDynamicAidsTestWithAids(false, testAids);

    testAids.clear();
    testAids.push_back(TcNfcCeData::AID_LEN_32);
    mCeHostServiceData_->AddDynamicAidsTestWithAids(false, testAids);

    mCeHostServiceData_->RemoveAidsTestOk(false);
}

/**
 * @tc.number    : NFC_TC_CE_HostTest_001_004
 * @tc.name      : set default route as ESE1, App1 has installed in DH, set App1 as PrimaryService, add prefix matched
 * security AIDS with different length
 * @tc.desc      :
 */
TEST_F(TcNfcCeHostServiceTest, AddAidsForService_Test104)
{
    DebugLog("AddAidsForService_Test104 in");
    mCeHostServiceData_->StartHceTest(TcNfcCeData::HOST_SECURE_ABILITY_NAME_1, TcNfcCeData::ABILITY_TYPE_SECURE);
    mCeHostServiceData_->SetPrimaryServiceTest();

    std::vector<std::string> testAids;
    testAids.push_back(TcNfcCeData::AID_LEN_10 + "*");
    mCeHostServiceData_->AddDynamicAidsTestWithAids(false, testAids);

    testAids.clear();
    testAids.push_back(TcNfcCeData::AID_LEN_18 + "*");
    mCeHostServiceData_->AddDynamicAidsTestWithAids(false, testAids);

    testAids.clear();
    testAids.push_back(TcNfcCeData::AID_LEN_30 + "*");
    mCeHostServiceData_->AddDynamicAidsTestWithAids(false, testAids);

    mCeHostServiceData_->RemoveAidsTestOk(false);
}

/**
 * @tc.number    : NFC_TC_CE_HostTest_002_001
 * @tc.name      : set default route as ESE1, App1 has installed in DH, set App1 as PrimaryService, add fully matched
 * security AIDS with invalid length
 * @tc.desc      :
 */
TEST_F(TcNfcCeHostServiceTest, AddAidsForService_Test201)
{
    DebugLog("AddAidsForService_Test201 in");
    std::string type = TcNfcCeData::ABILITY_TYPE_SECURE;
    mCeHostServiceData_->StartHceTest(TcNfcCeData::HOST_SECURE_ABILITY_NAME_1, type);
    mCeHostServiceData_->SetPrimaryServiceTest();

    ElementName serviceName;
    bool res = mCeHostServiceData_->GetElementName(type, serviceName);
    EXPECT_TRUE(res);

    std::vector<std::string> testAids;
    testAids.push_back(TcNfcCeData::AID_LEN_8);
    res = mCeHostServiceData_->GetHce()->AddAidsForService(serviceName, testAids, type);
    EXPECT_FALSE(res);

    testAids.clear();
    testAids.push_back(TcNfcCeData::AID_LEN_25);
    res = mCeHostServiceData_->GetHce()->AddAidsForService(serviceName, testAids, type);
    EXPECT_FALSE(res);

    testAids.clear();
    testAids.push_back(TcNfcCeData::AID_LEN_36);
    res = mCeHostServiceData_->GetHce()->AddAidsForService(serviceName, testAids, type);
    EXPECT_FALSE(res);
}
/**
 * @tc.number    : NFC_TC_CE_HostTest_002_002
 * @tc.name      : The device does not support prefix matching tests
 * @tc.desc      : The current environment supports prefix matching, this case is not tested in this version
 */
TEST_F(TcNfcCeHostServiceTest, AddAidsForService_Test202)
{
    DebugLog("AddAidsForService_Test202 in");
    // mCeHostServiceData_->StartHceTest(TcNfcCeData::PRE_HOST_SECURE_ABILITY_NAME_1, TcNfcCeData::ABILITY_TYPE_SECURE);
    // mCeHostServiceData_->SetPrimaryServiceTest();
    // bool bSupportedAidPre = mCeHostServiceData_->GetHce()->IsSupportedAidPrefixMode();
    // EXPECT_FALSE(bSupportedAidPre);
    // mCeHostServiceData_->AddDynamicAidsTest(false, false);
}

/**
 * @tc.number    : NFC_TC_CE_HostTest_002_003
 * @tc.name      : test with invalid prefix matched security aids
 * @tc.desc      :
 */
TEST_F(TcNfcCeHostServiceTest, AddAidsForService_Test203)
{
    DebugLog("AddAidsForService_Test203 in");
    std::string type = TcNfcCeData::ABILITY_TYPE_SECURE;
    mCeHostServiceData_->StartHceTest(TcNfcCeData::HOST_SECURE_ABILITY_NAME_1, type);
    mCeHostServiceData_->SetPrimaryServiceTest();

    ElementName serviceName;
    bool res = mCeHostServiceData_->GetElementName(type, serviceName);
    EXPECT_TRUE(res);

    std::vector<std::string> testAids = {"01020304*005"};
    res = mCeHostServiceData_->GetHce()->AddAidsForService(serviceName, testAids, type);
    EXPECT_FALSE(res);

    testAids = {"*01020304050"};
    res = mCeHostServiceData_->GetHce()->AddAidsForService(serviceName, testAids, type);
    EXPECT_FALSE(res);
}

/**
 * @tc.number    : NFC_TC_CE_HostTest_002_004
 * @tc.name      : test with invalid prefix matched security aids
 * @tc.desc      :
 */
TEST_F(TcNfcCeHostServiceTest, AddAidsForService_Test204)
{
    DebugLog("AddAidsForService_Test204 in");
    std::string type = TcNfcCeData::ABILITY_TYPE_SECURE;
    mCeHostServiceData_->StartHceTest(TcNfcCeData::HOST_SECURE_ABILITY_NAME_1, type);
    mCeHostServiceData_->SetPrimaryServiceTest();

    ElementName serviceName;
    bool res = mCeHostServiceData_->GetElementName(type, serviceName);
    EXPECT_TRUE(res);

    std::vector<std::string> testAids;
    testAids.push_back(TcNfcCeData::AID_LEN_8 + "*");
    res = mCeHostServiceData_->GetHce()->AddAidsForService(serviceName, testAids, type);
    EXPECT_FALSE(res);

    testAids.clear();
    testAids.push_back(TcNfcCeData::AID_LEN_25 + "*");
    res = mCeHostServiceData_->GetHce()->AddAidsForService(serviceName, testAids, type);
    EXPECT_FALSE(res);

    testAids.clear();
    testAids.push_back(TcNfcCeData::AID_LEN_32 + "*");
    res = mCeHostServiceData_->GetHce()->AddAidsForService(serviceName, testAids, type);
    EXPECT_FALSE(res);
}

/**
 * @tc.number    : NFC_TC_CE_HostTest_003_001
 * @tc.name      : test with fully matched normal type aids
 * @tc.desc      :
 */
TEST_F(TcNfcCeHostServiceTest, AddAidsForService_Test301)
{
    DebugLog("AddAidsForService_Test301 in");
    mCeHostServiceData_->StartHceTest(TcNfcCeData::HOST_NORMAL_ABILITY_NAME_1, TcNfcCeData::ABILITY_TYPE_NORMAL);
    mCeHostServiceData_->AddDynamicAidsTest(true);
    mCeHostServiceData_->RemoveAidsTestOk(true);
}

/**
 * @tc.number    : NFC_TC_CE_HostTest_003_002
 * @tc.name      : test with prefix matched normal type aids
 * @tc.desc      :
 */
TEST_F(TcNfcCeHostServiceTest, AddAidsForService_Test302)
{
    DebugLog("AddAidsForService_Test302 in");
    mCeHostServiceData_->StartHceTest(TcNfcCeData::PRE_HOST_NORMAL_ABILITY_NAME_1, TcNfcCeData::ABILITY_TYPE_NORMAL);
    mCeHostServiceData_->AddDynamicAidsTest(true);
    mCeHostServiceData_->RemoveAidsTestOk(true);
}

/**
 * @tc.number    : NFC_TC_CE_HostTest_004_001
 * @tc.name      : test remove security aids
 * @tc.desc      : Api test
 */
TEST_F(TcNfcCeHostServiceTest, RemoveAidsForService_Test_401)
{
    DebugLog("RemoveAidsForService_Test_401 in");
    mCeHostServiceData_->StartHceTest(TcNfcCeData::HOST_SECURE_ABILITY_NAME_1, TcNfcCeData::ABILITY_TYPE_SECURE);
    mCeHostServiceData_->SetPrimaryServiceTest();
    mCeHostServiceData_->AddDynamicAidsTest(false);

    mCeHostServiceData_->RemoveAidsTestOk(false);
}

/**
 * @tc.number    : NFC_TC_CE_HostTest_004_002
 * @tc.name      : test remove normal aids
 * @tc.desc      : Api test
 */
TEST_F(TcNfcCeHostServiceTest, RemoveAidsForService_Test_402)
{
    DebugLog("RemoveAidsForService_Test_402 in");
    mCeHostServiceData_->StartHceTest(TcNfcCeData::HOST_SN_ABILITY_NAME_1, TcNfcCeData::ABILITY_TYPE_SECURE);
    mCeHostServiceData_->SetPrimaryServiceTest();

    std::vector<std::string> testAids = {TcNfcCeData::AID_LEN_10, TcNfcCeData::AID_LEN_18};
    mCeHostServiceData_->AddDynamicAidsTestWithAids(true, testAids);

    mCeHostServiceData_->RemoveAidsTestOk(true);
}

/**
 * @tc.number    : NFC_TC_CE_HostTest_004_003
 * @tc.name      : test remove security aids
 * @tc.desc      : Api test
 */
TEST_F(TcNfcCeHostServiceTest, RemoveAidsForService_Test_403)
{
    DebugLog("RemoveAidsForService_Test_403 in");
    std::string type = TcNfcCeData::ABILITY_TYPE_SECURE;
    mCeHostServiceData_->StartHceTest(TcNfcCeData::HOST_SECURE_ABILITY_NAME_1, type);

    ElementName serviceName;
    bool res = mCeHostServiceData_->GetElementName(type, serviceName);
    EXPECT_TRUE(res);

    std::vector<std::string> testAids = mCeHostServiceData_->GetTestNfcCEAgent()->GetAids(type, true);
    res = mCeHostServiceData_->GetHce()->AddAidsForService(serviceName, testAids, type);
    EXPECT_TRUE(res);
    mCeHostServiceData_->SetExpectAids({}, testAids);

    std::vector<std::string> excludeAids =
        mCeHostServiceData_->GetTestNfcCEAgent()->GetAids(TcNfcCeData::ABILITY_TYPE_SECURE, false);
    mCeHostServiceData_->GetHce()->RemoveAidsForService(serviceName, type);
    mCeHostServiceData_->SetExpectAids({}, excludeAids);
}

/**
 * @tc.number    : NFC_TC_CE_HostTest_004_004
 * @tc.name      : test remove normal aids
 * @tc.desc      : Api test
 */
TEST_F(TcNfcCeHostServiceTest, RemoveAidsForService_Test_404)
{
    DebugLog("RemoveAidsForService_Test_404 in");
    mCeHostServiceData_->StartHceTest(TcNfcCeData::HOST_NORMAL_ABILITY_NAME_1, TcNfcCeData::ABILITY_TYPE_NORMAL);
    mCeHostServiceData_->AddDynamicAidsTest(true);

    mCeHostServiceData_->RemoveAidsTestOk(true);
}

/**
 * @tc.number    : NFC_TC_CE_HostTest_004_005
 * @tc.name      : test repeat remove aids(normal and secure)
 * @tc.desc      : Api test
 */
TEST_F(TcNfcCeHostServiceTest, RemoveAidsForService_Test_405)
{
    DebugLog("RemoveAidsForService_Test_405 in");
    mCeHostServiceData_->StartHceTest(TcNfcCeData::HOST_SN_ABILITY_NAME_1, TcNfcCeData::ABILITY_TYPE_NORMAL);
    mCeHostServiceData_->AddDynamicAidsTest(true);

    // remove normal
    mCeHostServiceData_->RemoveAidsTestOk(true);
    mCeHostServiceData_->RemoveAidsTestOk(true);

    // remove secure
    mCeHostServiceData_->RemoveAidsTestOk(false);
    mCeHostServiceData_->RemoveAidsTestOk(false);
}

/**
 * @tc.number    : NFC_TC_CE_HostTest_005_001
 * @tc.name      : set app1 as PrimaryService , then set app1 as prefer service
 * @tc.desc      :
 */
TEST_F(TcNfcCeHostServiceTest, SetPreferredService_Test501)
{
    DebugLog("SetPreferredService_Test501 in");
    std::string type = TcNfcCeData::ABILITY_TYPE_SECURE;
    mCeHostServiceData_->StartHceTest(TcNfcCeData::HOST_SECURE_ABILITY_NAME_1, type);
    mCeHostServiceData_->SetPrimaryServiceTest();

    ElementName serviceName;
    bool res = mCeHostServiceData_->GetElementName(type, serviceName);
    EXPECT_TRUE(res);

    res = mCeHostServiceData_->GetHce()->SetPreferredForegroundService(serviceName);
    EXPECT_FALSE(res);
}

/**
 * @tc.number    : NFC_TC_CE_HostTest_005_002
 * @tc.name      : set app1 as PrimaryService fail
 * @tc.desc      :
 */
TEST_F(TcNfcCeHostServiceTest, SetPreferredService_Test502)
{
    DebugLog("SetPreferredService_Test502 in");
    mCeHostServiceData_->StartHceTest(TcNfcCeData::HOST_SECURE_ABILITY_NAME_1, TcNfcCeData::ABILITY_TYPE_SECURE);
    mCeHostServiceData_->SetPrimaryServiceTest();

    mCeHostServiceData_N->StartHceTest(TcNfcCeData::HOST_SECURE_ABILITY_NAME_2, TcNfcCeData::ABILITY_TYPE_SECURE);

    std::string type = TcNfcCeData::ABILITY_TYPE_SECURE;
    ElementName serviceName;
    bool res = mCeHostServiceData_N->GetElementName(type, serviceName);
    EXPECT_TRUE(res);

    res = mCeHostServiceData_N->GetHce()->SetPreferredForegroundService(serviceName);
    EXPECT_FALSE(res);
}

/**
 * @tc.number    : NFC_TC_CE_HostTest_005_003
 * @tc.name      : set app1 as PrimaryService fail
 * @tc.desc      :
 */
TEST_F(TcNfcCeHostServiceTest, SetPreferredService_Test503)
{
    DebugLog("SetPreferredService_Test503 in");
    mCeHostServiceData_->StartHceTest(TcNfcCeData::HOST_SN_ABILITY_NAME_1, TcNfcCeData::ABILITY_TYPE_SECURE);

    std::vector<std::string> testAids = {TcNfcCeData::AID_LEN_10, TcNfcCeData::AID_LEN_18};
    mCeHostServiceData_->AddDynamicAidsTestWithAids(true, testAids);

    std::string type = TcNfcCeData::ABILITY_TYPE_SECURE;
    ElementName serviceName;
    bool res = mCeHostServiceData_->GetElementName(type, serviceName);
    EXPECT_TRUE(res);

    res = mCeHostServiceData_->GetHce()->SetPreferredForegroundService(serviceName);
    EXPECT_FALSE(res);

    mCeHostServiceData_->RemoveAidsTestOk(true);
}

/**
 * @tc.number    : NFC_TC_CE_HostTest_005_004
 * @tc.name      : set app1 as PrimaryService success
 * @tc.desc      :
 */
TEST_F(TcNfcCeHostServiceTest, SetPreferredService_Test504)
{
    DebugLog("SetPreferredService_Test504 in");
    mCeHostServiceData_->StartHceTest(TcNfcCeData::HOST_NORMAL_ABILITY_NAME_1, TcNfcCeData::ABILITY_TYPE_NORMAL);
    mCeHostServiceData_->SetPreferredServiceTest(true);
}

/**
 * @tc.number    : NFC_TC_CE_HostTest_005_005
 * @tc.name      : set app1 as PrimaryService success
 * @tc.desc      :
 */
TEST_F(TcNfcCeHostServiceTest, SetPreferredService_Test505)
{
    DebugLog("SetPreferredService_Test505 in");
    mCeHostServiceData_->StartHceTest(TcNfcCeData::HOST_NORMAL_ABILITY_NAME_1, TcNfcCeData::ABILITY_TYPE_NORMAL);

    mCeHostServiceData_->AddDynamicAidsTestWithAids(true, TcNfcCeData::CONFICT_PRE_AIDS_WITH_HN_2);
    mCeHostServiceData_->SetPreferredServiceTest(true);

    mCeHostServiceData_->AddDynamicAidsTestWithAids(true, TcNfcCeData::CONFICT_PRE_AIDS_WITH_HN_2);

    mCeHostServiceData_->RemoveAidsTestOk(true);
}

/**
 * @tc.number    : NFC_TC_CE_HostTest_005_006
 * @tc.name      : set app1 as PrimaryService success
 * @tc.desc      :
 */
TEST_F(TcNfcCeHostServiceTest, SetPreferredService_Test506)
{
    DebugLog("SetPreferredService_Test506 in");
    mCeHostServiceData_->StartHceTest(TcNfcCeData::HOST_NORMAL_ABILITY_NAME_2, TcNfcCeData::ABILITY_TYPE_NORMAL);

    mCeHostServiceData_->AddDynamicAidsTestWithAids(true, TcNfcCeData::CONFICT_PRE_AIDS_WITH_HN_1);
    mCeHostServiceData_->SetPreferredServiceTest(true);

    mCeHostServiceData_->AddDynamicAidsTestWithAids(true, TcNfcCeData::CONFICT_PRE_AIDS_WITH_HN_1);
    mCeHostServiceData_->RemoveAidsTestOk(true);
}

/**
 * @tc.number    : NFC_TC_CE_HostTest_005_007
 * @tc.name      : set app1 as PrimaryService success
 * @tc.desc      :
 */
TEST_F(TcNfcCeHostServiceTest, SetPreferredService_Test507)
{
    DebugLog("SetPreferredService_Test507 in");
    mCeHostServiceData_->StartHceTest(TcNfcCeData::HOST_NORMAL_ABILITY_NAME_1, TcNfcCeData::ABILITY_TYPE_NORMAL);

    mCeHostServiceData_->AddDynamicAidsTestWithAids(true, TcNfcCeData::CONFICT_AIDS_WITH_PHN_1);
    mCeHostServiceData_->SetPreferredServiceTest(true);

    mCeHostServiceData_->AddDynamicAidsTestWithAids(true, TcNfcCeData::CONFICT_AIDS_WITH_PHN_1);
    mCeHostServiceData_->RemoveAidsTestOk(true);
}

/**
 * @tc.number    : NFC_TC_CE_HostTest_005_008
 * @tc.name      : set app1 as PrimaryService success
 * @tc.desc      :
 */
TEST_F(TcNfcCeHostServiceTest, SetPreferredService_Test508)
{
    DebugLog("SetPreferredService_Test508 in");
    mCeHostServiceData_->StartHceTest(TcNfcCeData::HOST_NORMAL_ABILITY_NAME_2, TcNfcCeData::ABILITY_TYPE_NORMAL);

    mCeHostServiceData_->AddDynamicAidsTestWithAids(true, TcNfcCeData::CONFICT_AIDS_WITH_PHN_1);
    mCeHostServiceData_->SetPreferredServiceTest(true);

    mCeHostServiceData_->AddDynamicAidsTestWithAids(true, TcNfcCeData::CONFICT_AIDS_WITH_PHN_1);
    mCeHostServiceData_->RemoveAidsTestOk(true);
}

/**
 * @tc.number    : NFC_TC_CE_HostTest_006_001
 * @tc.name      : set app1 as PrimaryService success
 * @tc.desc      :
 */
TEST_F(TcNfcCeHostServiceTest, SetPreferredService_Test601)
{
    DebugLog("SetPreferredService_Test601 in");
    mCeHostServiceData_->StartHceTest(TcNfcCeData::HOST_NORMAL_ABILITY_NAME_2, TcNfcCeData::ABILITY_TYPE_NORMAL);

    mCeHostServiceData_->SetPreferredServiceTest(true);
    mCeHostServiceData_->AddDynamicAidsTestWithAids(true, TcNfcCeData::CONFICT_PRE_AIDS_WITH_HN_1);

    mCeHostServiceData_->UnSetPreferredServiceTestOk(true);
    mCeHostServiceData_->RemoveAidsTestOk(true);
}
/**
 * @tc.number    : NFC_TC_CE_HostTest_006_002
 * @tc.name      : set app1 as PrimaryService success
 * @tc.desc      :
 */
TEST_F(TcNfcCeHostServiceTest, SetPreferredService_Test602)
{
    DebugLog("SetPreferredService_Test602 in");
    mCeHostServiceData_->StartHceTest(TcNfcCeData::HOST_NORMAL_ABILITY_NAME_2, TcNfcCeData::ABILITY_TYPE_NORMAL);

    mCeHostServiceData_->AddDynamicAidsTestWithAids(true, TcNfcCeData::CONFICT_AIDS_WITH_PHN_1);
    mCeHostServiceData_->SetPreferredServiceTest(true);

    mCeHostServiceData_->AddDynamicAidsTestWithAids(true, TcNfcCeData::CONFICT_AIDS_WITH_PHN_1);
    mCeHostServiceData_->UnSetPreferredServiceTestOk(true);
    mCeHostServiceData_->RemoveAidsTestOk(true);
}

/**
 * @tc.number    : NFC_TC_CE_HostTest_006_003
 * @tc.name      : unset app1 as PrimaryService success
 * @tc.desc      :
 */
TEST_F(TcNfcCeHostServiceTest, SetPreferredService_Test603)
{
    DebugLog("SetPreferredService_Test603 in");
    mCeHostServiceData_->StartHceTest(TcNfcCeData::HOST_NORMAL_ABILITY_NAME_1, TcNfcCeData::ABILITY_TYPE_NORMAL);
    mCeHostServiceData_->SetPreferredServiceTest(true);
    mCeHostServiceData_->UnSetPreferredServiceTestOk(true);
}

/**
 * @tc.number    : NFC_TC_CE_HostTest_007_001
 * @tc.name      : add static aids test
 * @tc.desc      :
 */
TEST_F(TcNfcCeHostServiceTest, GetAidsForService_Test701)
{
    DebugLog("GetAidsForService_Test701 in");
    std::string type = TcNfcCeData::ABILITY_TYPE_NORMAL;
    mCeHostServiceData_->StartHceTest(TcNfcCeData::HOST_NORMAL_ABILITY_NAME_1, type);

    ElementName serviceName;
    mCeHostServiceData_->GetElementName(type, serviceName);
    std::vector<std::string> aids = mCeHostServiceData_->GetHce()->GetAidsForService(serviceName, type);
    EXPECT_TRUE((aids.size() > 0));

    mCeHostServiceData_->RemoveAidsTestOk(true);
}

/**
 * @tc.number    : NFC_TC_CE_HostTest_007_002
 * @tc.name      : add static aids test
 * @tc.desc      :
 */
TEST_F(TcNfcCeHostServiceTest, GetAidsForService_Test702)
{
    DebugLog("GetAidsForService_Test702 in");
    std::string type = TcNfcCeData::ABILITY_TYPE_NORMAL;
    mCeHostServiceData_->StartHceTest(TcNfcCeData::PRE_HOST_NORMAL_ABILITY_NAME_1, type);

    ElementName serviceName;
    mCeHostServiceData_->GetElementName(type, serviceName);
    std::vector<std::string> aids = mCeHostServiceData_->GetHce()->GetAidsForService(serviceName, type);
    EXPECT_TRUE((aids.size() > 0));

    mCeHostServiceData_->RemoveAidsTestOk(true);
}

/**
 * @tc.number    : NFC_TC_CE_HostTest_008_001
 * @tc.name      : set app1 as primary service success
 * @tc.desc      : Api test
 */
TEST_F(TcNfcCeHostServiceTest, SetPrimaryServiceForType_Test801)
{
    DebugLog("SetPrimaryServiceForType_Test801 in");
    mCeOffHostServiceData_->StartOffHceLiteTest(TcNfcCeData::SE_SECURE_ABILITY_NAME_1,
                                                TcNfcCeData::ABILITY_TYPE_SECURE);
    mCeOffHostServiceData_->SetPrimaryServiceTest();
}

/**
 * @tc.number    : NFC_TC_CE_HostTest_008_002
 * @tc.name      : set app1 as primary service success
 * @tc.desc      :
 */
TEST_F(TcNfcCeHostServiceTest, SetPrimaryServiceForType_Test802)
{
    DebugLog("SetPrimaryServiceForType_Test802 in");
    mCeHostServiceData_->StartHceTest(TcNfcCeData::HOST_SECURE_ABILITY_NAME_1, TcNfcCeData::ABILITY_TYPE_SECURE);
    mCeHostServiceData_->SetPrimaryServiceTest();
}

/**
 * @tc.number    : NFC_TC_CE_HostTest_008_004
 * @tc.name      : switch primary service success(app1 in se switch into app2 in DH)
 * @tc.desc      :
 */
TEST_F(TcNfcCeHostServiceTest, SetPrimaryServiceForType_Test804)
{
    DebugLog("SetPrimaryServiceForType_Test804 in test ability.%s", TcNfcCeData::SE_SECURE_ABILITY_NAME_1.c_str());
    mCeOffHostServiceData_->StartOffHceLiteTest(TcNfcCeData::SE_SECURE_ABILITY_NAME_1,
                                                TcNfcCeData::ABILITY_TYPE_SECURE);
    mCeOffHostServiceData_->SetPrimaryServiceTest();

    DebugLog("SetPrimaryServiceForType_Test804 in");
    mCeHostServiceData_->StartHceTest(TcNfcCeData::HOST_SECURE_ABILITY_NAME_1, TcNfcCeData::ABILITY_TYPE_SECURE);
    mCeHostServiceData_->SetPrimaryServiceTest();
}

/**
 * @tc.number    : NFC_TC_CE_HostTest_008_005
 * @tc.name      : switch primary service success(app1 in DH switch into app2 in SE)
 * @tc.desc      :
 */
TEST_F(TcNfcCeHostServiceTest, SetPrimaryServiceForType_Test805)
{
    DebugLog("SetPrimaryServiceForType_Test805 in test ability.%s", TcNfcCeData::HOST_SECURE_ABILITY_NAME_1.c_str());
    mCeHostServiceData_->StartHceTest(TcNfcCeData::HOST_SECURE_ABILITY_NAME_1, TcNfcCeData::ABILITY_TYPE_SECURE);
    mCeHostServiceData_->SetPrimaryServiceTest();

    DebugLog("SetPrimaryServiceForType_Test805 in");
    mCeOffHostServiceData_->StartOffHceLiteTest(TcNfcCeData::SE_SECURE_ABILITY_NAME_1,
                                                TcNfcCeData::ABILITY_TYPE_SECURE);
    mCeOffHostServiceData_->SetPrimaryServiceTest();
}

/**
 * @tc.number    : NFC_TC_CE_HostTest_008_010
 * @tc.name      : switch primary service success(app1 in DH switch into app2 in DH)
 * @tc.desc      :
 */
TEST_F(TcNfcCeHostServiceTest, SetPrimaryServiceForType_Test810)
{
    DebugLog("SetPrimaryServiceForType_Test810 in test ability.%s", TcNfcCeData::HOST_SECURE_ABILITY_NAME_1.c_str());
    mCeHostServiceData_->StartHceTest(TcNfcCeData::HOST_SECURE_ABILITY_NAME_1, TcNfcCeData::ABILITY_TYPE_SECURE);
    mCeHostServiceData_->SetPrimaryServiceTest();

    DebugLog("SetPrimaryServiceForType_Test810 in");
    mCeHostServiceData_N->StartHceTest(TcNfcCeData::HOST_SECURE_ABILITY_NAME_2, TcNfcCeData::ABILITY_TYPE_SECURE);
    mCeHostServiceData_N->SetPrimaryServiceTest();
}

/**
 * @tc.number    : NFC_TC_CE_HostTest_009_001
 * @tc.name      : set app1 as primary service fail
 * @tc.desc      :
 */
TEST_F(TcNfcCeHostServiceTest, SetPrimaryServiceForType_Test901)
{
    DebugLog("SetPrimaryServiceForType_Test901 in");
    mCeHostServiceData_->StartHceTest(TcNfcCeData::HOST_NORMAL_ABILITY_NAME_1, TcNfcCeData::ABILITY_TYPE_NORMAL);

    OHOS::AppExecFwk::ElementName serviceName;
    bool res = mCeHostServiceData_->GetElementName(TcNfcCeData::ABILITY_TYPE_NORMAL, serviceName);
    EXPECT_TRUE(res);

    res = mCeHostServiceData_->GetHce()->SetPrimaryServiceForType(
        serviceName, mCeHostServiceData_->GetCEAbilityInfo()->aidsets_.at(0)->type_);
    EXPECT_FALSE(res);

    std::string sname =
        mCeHostServiceData_->GetHce()->GetPrimaryServiceForType(mCeHostServiceData_->GetCEAbilityInfo()->abilityType_);
    res = (sname != serviceName.GetAbilityName()) ? true : false;
    EXPECT_TRUE(res);
}
/**
 * @tc.number    : NFC_TC_CE_HostTest_009_002
 * @tc.name      : set app1 as primary service fail
 * @tc.desc      :
 */
TEST_F(TcNfcCeHostServiceTest, SetPrimaryServiceForType_Test902)
{
    DebugLog("SetPrimaryServiceForType_Test902 in");
    mCeHostServiceData_->StartHceTest(TcNfcCeData::HOST_NORMAL_ABILITY_NAME_1, TcNfcCeData::ABILITY_TYPE_NORMAL);

    OHOS::AppExecFwk::ElementName serviceName("", "", "abilityNotExist");
    bool res = mCeHostServiceData_->GetHce()->SetPrimaryServiceForType(
        serviceName, mCeHostServiceData_->GetCEAbilityInfo()->aidsets_.at(0)->type_);
    EXPECT_FALSE(res);

    std::string sname =
        mCeHostServiceData_->GetHce()->GetPrimaryServiceForType(mCeHostServiceData_->GetCEAbilityInfo()->abilityType_);
    res = (sname != serviceName.GetAbilityName()) ? true : false;
    EXPECT_TRUE(res);
}
}  // namespace OHOS::nfc::test
