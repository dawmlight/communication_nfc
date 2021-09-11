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
#include <thread>

#include "card_emulation.h"
#include "element_name.h"
#include "ihost_card_emulation.h"
#include "loghelper.h"
#include "nfc_agent.h"
#include "tc_nfc_ce_host_service_data.h"
#include "test_nfc_ce_agent.h"

using namespace testing;
using namespace std;
using namespace OHOS::AppExecFwk;
using namespace OHOS::nfc::sdk::cardemulation;
namespace OHOS::nfc::test {
class TcNfcCeHostTest : public testing::Test {
public:
    virtual void SetUp() override
    {
        std::cout << " SetUp NfcCeHostTest." << std::endl;
        mCeHostServiceData_ = std::make_shared<TcNfcCeHostServiceData>();
        mCeHostServiceData_N = std::make_shared<TcNfcCeHostServiceData>();
    }

    virtual void TearDown() override
    {
        std::cout << " TearDown NfcCeHostTest." << std::endl;
        if (mCeHostServiceData_->GetTestNfcCEAgent()) {
            mCeHostServiceData_->StopHceTest();
        }
        if (mCeHostServiceData_N->GetTestNfcCEAgent()) {
            mCeHostServiceData_N->StopHceTest();
        }
    }

public:
    std::shared_ptr<TcNfcCeHostServiceData> mCeHostServiceData_;
    std::shared_ptr<TcNfcCeHostServiceData> mCeHostServiceData_N;
};

/**
 * @tc.number    : NFC_TC_CE_HostLiteTest_001_001
 * @tc.name      : set default route as ESE1, App1 has installed in DH, set App1 as PrimaryService, add fully matched
 * security AIDS
 * @tc.desc      :
 */
TEST_F(TcNfcCeHostTest, TestAddAidsForService_Test101)
{
    DebugLog("TestAddAidsForService_Test101 in");
    mCeHostServiceData_->StartHceLiteTest(TcNfcCeData::HOST_SECURE_ABILITY_NAME_1, TcNfcCeData::ABILITY_TYPE_SECURE);
    mCeHostServiceData_->SetPrimaryServiceTest();
    mCeHostServiceData_->LiteAddDynamicAidsTestOk(false);
    mCeHostServiceData_->LiteRemoveDynamicAidsTestOk(false);
}
/**
 * @tc.number    : NFC_TC_CE_HostLiteTest_001_002
 * @tc.name      : set default route as ESE1, App1 has installed in DH, set App1 as PrimaryService, add prefix matched
 * security AIDS
 * @tc.desc      :
 */
TEST_F(TcNfcCeHostTest, TestAddAidsForService_Test102)
{
    DebugLog("TestAddAidsForService_Test102 in");
    mCeHostServiceData_->StartHceLiteTest(TcNfcCeData::PRE_HOST_SECURE_ABILITY_NAME_1,
                                          TcNfcCeData::ABILITY_TYPE_SECURE);
    mCeHostServiceData_->SetPrimaryServiceTest();
    mCeHostServiceData_->LiteAddDynamicAidsTestOk(false);
    mCeHostServiceData_->LiteRemoveDynamicAidsTestOk(false);
}

/**
 * @tc.number    : NFC_TC_CE_HostLiteTest_001_003
 * @tc.name      :set default route as ESE1, App1 has installed in DH, set App1 as PrimaryService, add fully matched
 * security AIDS, only the last aid group can be add into routing table
 * @tc.desc      :
 */
TEST_F(TcNfcCeHostTest, TestAddAidsForService_Test103)
{
    DebugLog("TestAddAidsForService_Test103 in");
    mCeHostServiceData_->StartHceLiteTest(TcNfcCeData::HOST_SN_ABILITY_NAME_N, TcNfcCeData::ABILITY_TYPE_SECURE);

    OHOS::AppExecFwk::ElementName serviceName;
    bool res = mCeHostServiceData_->GetElementName(TcNfcCeData::ABILITY_TYPE_SECURE, serviceName);
    EXPECT_TRUE(res);

    res = mCeHostServiceData_->GetHce()->SetPrimaryServiceForType(serviceName, TcNfcCeData::ABILITY_TYPE_SECURE);
    EXPECT_TRUE(res);

    std::string sname = mCeHostServiceData_->GetHce()->GetPrimaryServiceForType(TcNfcCeData::ABILITY_TYPE_SECURE);
    res = (sname == serviceName.GetAbilityName()) ? true : false;
    EXPECT_TRUE(res);
    DebugLog("PrimaryService name.%s expect name.%s", sname.c_str(), serviceName.GetAbilityName().c_str());

    std::vector<std::string> testAids_1 =
        mCeHostServiceData_->GetTestNfcCEAgent()->GetAids(TcNfcCeData::ABILITY_TYPE_SECURE, false, 1);
    std::vector<std::string> testAids_2 =
        mCeHostServiceData_->GetTestNfcCEAgent()->GetAids(TcNfcCeData::ABILITY_TYPE_SECURE, false, 3);
    mCeHostServiceData_->SetExpectAids(testAids_2, testAids_1);
}
/**
 * @tc.number    : NFC_TC_CE_HostLiteTest_001_004
 * @tc.name      : repeat add aids for app in DH
 * @tc.desc      :
 */
TEST_F(TcNfcCeHostTest, TestAddAidsForService_Test104)
{
    DebugLog("TestAddAidsForService_Test104 in");
    mCeHostServiceData_->StartHceLiteTest(TcNfcCeData::HOST_SECURE_ABILITY_NAME_1, TcNfcCeData::ABILITY_TYPE_SECURE);
    mCeHostServiceData_->SetPrimaryServiceTest();

    std::vector<std::string> testAids =
        mCeHostServiceData_->GetTestNfcCEAgent()->GetAids(TcNfcCeData::ABILITY_TYPE_SECURE, true);

    testAids.insert(testAids.end(), testAids.begin(), testAids.end());
    mCeHostServiceData_->LiteAddDynamicAidsTestOk(false, testAids);
    mCeHostServiceData_->LiteRemoveDynamicAidsTestOk(false);
}

/**
 * @tc.number    : NFC_TC_CE_HostLiteTest_001_005
 * @tc.name      : repeat add aids for app in DH
 * @tc.desc      :
 */
TEST_F(TcNfcCeHostTest, TestAddAidsForService_Test105)
{
    DebugLog("TestAddAidsForService_Test105 in");
    std::string type = TcNfcCeData::ABILITY_TYPE_SECURE;
    mCeHostServiceData_->StartHceLiteTest(TcNfcCeData::HOST_SECURE_ABILITY_NAME_1, type);
    mCeHostServiceData_->SetPrimaryServiceTest();

    std::vector<std::string> testAids = mCeHostServiceData_->GetTestNfcCEAgent()->GetAids(type, true);
    std::vector<std::string> staticAids = mCeHostServiceData_->GetTestNfcCEAgent()->GetAids(type, false);

    bool res = mCeHostServiceData_->GetHceLite()->AddAidsForService(testAids, type);
    EXPECT_TRUE(res);
    mCeHostServiceData_->SetExpectAids(testAids);
    mCeHostServiceData_->GetHceLite()->RemoveAidsForService(type);
    mCeHostServiceData_->SetExpectAids(staticAids);

    res = mCeHostServiceData_->GetHceLite()->AddAidsForService(testAids, type);
    EXPECT_TRUE(res);
    mCeHostServiceData_->SetExpectAids(testAids);
    mCeHostServiceData_->GetHceLite()->RemoveAidsForService(type);
    mCeHostServiceData_->SetExpectAids(staticAids);
}

/**
 * @tc.number    : NFC_TC_CE_HostLiteTest_001_006
 * @tc.name      : add aids for app in DH, there is conflict in this group
 * @tc.desc      :
 */
TEST_F(TcNfcCeHostTest, TestAddAidsForService_Test106)
{
    DebugLog("TestAddAidsForService_Test106 in");
    mCeHostServiceData_->StartHceLiteTest(TcNfcCeData::HOST_SECURE_ABILITY_NAME_1, TcNfcCeData::ABILITY_TYPE_SECURE);
    mCeHostServiceData_->SetPrimaryServiceTest();

    mCeHostServiceData_->LiteAddDynamicAidsTestOk(false, TcNfcCeData::CONFICT_AIDS);
    mCeHostServiceData_->LiteRemoveDynamicAidsTestOk(false);
}

/**
 * @tc.number    : NFC_TC_CE_HostLiteTest_001_007
 * @tc.name      : add aids for app in DH, there is conflict in this group
 * @tc.desc      :
 */
TEST_F(TcNfcCeHostTest, TestAddAidsForService_Test107)
{
    DebugLog("TestAddAidsForService_Test107 in");
    mCeHostServiceData_->StartHceLiteTest(TcNfcCeData::HOST_SECURE_ABILITY_NAME_1, TcNfcCeData::ABILITY_TYPE_SECURE);
    mCeHostServiceData_->SetPrimaryServiceTest();

    std::vector<std::string> testAids = TcNfcCeData::CONFICT_AIDS_WITH_PRE;
    bool res = mCeHostServiceData_->GetHceLite()->AddAidsForService(testAids, TcNfcCeData::ABILITY_TYPE_SECURE);
    EXPECT_TRUE(res);

    testAids.pop_back();
    mCeHostServiceData_->SetExpectAids(testAids);

    mCeHostServiceData_->LiteRemoveDynamicAidsTestOk(false);
}

/**
 * @tc.number    : NFC_TC_CE_HostLiteTest_001_008
 * @tc.name      : add aids for app in DH, there is conflict in this group
 * @tc.desc      :
 */
TEST_F(TcNfcCeHostTest, TestAddAidsForService_Test108)
{
    DebugLog("TestAddAidsForService_Test108 in");
    mCeHostServiceData_->StartHceLiteTest(TcNfcCeData::HOST_SECURE_ABILITY_NAME_1, TcNfcCeData::ABILITY_TYPE_SECURE);
    mCeHostServiceData_->SetPrimaryServiceTest();

    std::vector<std::string> testAids = TcNfcCeData::CONFICT_AIDS_WITH_PRE_1;
    bool res = mCeHostServiceData_->GetHceLite()->AddAidsForService(testAids, TcNfcCeData::ABILITY_TYPE_SECURE);
    EXPECT_TRUE(res);

    testAids.pop_back();
    mCeHostServiceData_->SetExpectAids(testAids);

    mCeHostServiceData_->LiteRemoveDynamicAidsTestOk(false);
}

/**
 * @tc.number    : NFC_TC_CE_HostLiteTest_001_009
 * @tc.name      : add aids for app in DH, there is conflict with the aids in routing table
 * @tc.desc      :
 */
TEST_F(TcNfcCeHostTest, TestAddAidsForService_Test109)
{
    DebugLog("TestAddAidsForService_Test109 in");
    mCeHostServiceData_->StartHceLiteTest(TcNfcCeData::HOST_SECURE_ABILITY_NAME_1, TcNfcCeData::ABILITY_TYPE_SECURE);
    mCeHostServiceData_->SetPrimaryServiceTest();

    mCeHostServiceData_->LiteAddDynamicAidsTestOk(false, TcNfcCeData::CONFICT_AIDS_WITH_HS_1);
    mCeHostServiceData_->LiteRemoveDynamicAidsTestOk(false);
}

/**
 * @tc.number    : NFC_TC_CE_HostLiteTest_001_010
 * @tc.name      : add aids for app in DH, there is conflict with the aids in routing table
 * @tc.desc      :
 */
TEST_F(TcNfcCeHostTest, TestAddAidsForService_Test110)
{
    DebugLog("TestAddAidsForService_Test110 in");
    mCeHostServiceData_->StartHceLiteTest(TcNfcCeData::HOST_SECURE_ABILITY_NAME_1, TcNfcCeData::ABILITY_TYPE_SECURE);
    mCeHostServiceData_->SetPrimaryServiceTest();

    mCeHostServiceData_->LiteAddDynamicAidsTestOk(false, TcNfcCeData::CONFICT_AIDS_WITH_PHN_1);
    mCeHostServiceData_->LiteRemoveDynamicAidsTestOk(false);
}

/**
 * @tc.number    : NFC_TC_CE_HostLiteTest_001_011
 * @tc.name      : add aids for app in DH, there is conflict with the aids in routing table
 * @tc.desc      :
 */
TEST_F(TcNfcCeHostTest, TestAddAidsForService_Test111)
{
    DebugLog("TestAddAidsForService_Test111 in");
    mCeHostServiceData_->StartHceLiteTest(TcNfcCeData::HOST_SECURE_ABILITY_NAME_1, TcNfcCeData::ABILITY_TYPE_SECURE);
    mCeHostServiceData_->SetPrimaryServiceTest();

    mCeHostServiceData_->LiteAddDynamicAidsTestOk(false, TcNfcCeData::CONFICT_PRE_AIDS_WITH_HN_1);
    mCeHostServiceData_->LiteRemoveDynamicAidsTestOk(false);
}
/**
 * @tc.number    : NFC_TC_CE_HostLiteTest_001_012
 * @tc.name      : add aids for app in DH, there is conflict with the aids in routing table
 * @tc.desc      :
 */
TEST_F(TcNfcCeHostTest, TestAddAidsForService_Test112)
{
    DebugLog("TestAddAidsForService_Test112 in");
    mCeHostServiceData_->StartHceLiteTest(TcNfcCeData::HOST_SECURE_ABILITY_NAME_1, TcNfcCeData::ABILITY_TYPE_SECURE);
    mCeHostServiceData_->SetPrimaryServiceTest();

    mCeHostServiceData_->LiteAddDynamicAidsTestOk(false, TcNfcCeData::CONFICT_PRE_AIDS_WITH_PHN_2S);
    mCeHostServiceData_->LiteRemoveDynamicAidsTestOk(false);
}

/**
 * @tc.number    : NFC_TC_CE_HostLiteTest_001_013
 * @tc.name      : Case matching of aid
 * @tc.desc      :
 */
TEST_F(TcNfcCeHostTest, TestAddAidsForService_Test113)
{
    DebugLog("TestAddAidsForService_Test113 in");
    mCeHostServiceData_->StartHceLiteTest(TcNfcCeData::HOST_SECURE_ABILITY_NAME_1, TcNfcCeData::ABILITY_TYPE_SECURE);
    mCeHostServiceData_->SetPrimaryServiceTest();

    mCeHostServiceData_->LiteAddDynamicAidsTestOk(false, TcNfcCeData::IGNORE_AIDS_LETTERS);
    mCeHostServiceData_->LiteRemoveDynamicAidsTestOk(false);
}

/**
 * @tc.number    : NFC_TC_CE_HostLiteTest_001_014
 * @tc.name      : add aid for app1 which is not primary service
 * @tc.desc      :
 */
TEST_F(TcNfcCeHostTest, TestAddAidsForService_Test114)
{
    DebugLog("TestAddAidsForService_Test114 in");
    mCeHostServiceData_->StartHceLiteTest(TcNfcCeData::HOST_SECURE_ABILITY_NAME_2, TcNfcCeData::ABILITY_TYPE_SECURE);
    mCeHostServiceData_->SetPrimaryServiceTest();

    mCeHostServiceData_N->StartHceLiteTest(TcNfcCeData::HOST_SECURE_ABILITY_NAME_1, TcNfcCeData::ABILITY_TYPE_SECURE);

    std::string type = TcNfcCeData::ABILITY_TYPE_SECURE;
    std::vector<std::string> testAids = mCeHostServiceData_N->GetTestNfcCEAgent()->GetAids(type, true);
    std::vector<std::string> staticAids = mCeHostServiceData_N->GetTestNfcCEAgent()->GetAids(type, false);
    bool res = mCeHostServiceData_N->GetHceLite()->AddAidsForService(testAids, type);
    EXPECT_TRUE(res);
    mCeHostServiceData_N->SetExpectAids({}, testAids);
    mCeHostServiceData_N->GetHceLite()->RemoveAidsForService(type);
    mCeHostServiceData_N->SetExpectAids({}, staticAids);
}

/**
 * @tc.number    : NFC_TC_CE_HostLiteTest_002_001
 * @tc.name      : add aid for app1 with normal aids
 * @tc.desc      :
 */
TEST_F(TcNfcCeHostTest, TestAddNormalAidsForService_Test_201)
{
    DebugLog("TestAddNormalAidsForService_Test_201 in");
    mCeHostServiceData_->StartHceLiteTest(TcNfcCeData::HOST_NORMAL_ABILITY_NAME_1, TcNfcCeData::ABILITY_TYPE_NORMAL);
    mCeHostServiceData_->LiteAddDynamicAidsTestOk(true);
    mCeHostServiceData_->LiteRemoveDynamicAidsTestOk(true);
}

/**
 * @tc.number    : NFC_TC_CE_HostLiteTest_002_002
 * @tc.name      : add aid for app1 with normal aids
 * @tc.desc      :
 */
TEST_F(TcNfcCeHostTest, TestAddNormalAidsForService_Test_202)
{
    DebugLog("TestAddNormalAidsForService_Test_202 in");
    mCeHostServiceData_->StartHceLiteTest(TcNfcCeData::PRE_HOST_NORMAL_ABILITY_NAME_1,
                                          TcNfcCeData::ABILITY_TYPE_NORMAL);
    mCeHostServiceData_->LiteAddDynamicAidsTestOk(true);
    mCeHostServiceData_->LiteRemoveDynamicAidsTestOk(true);
}

/**
 * @tc.number    : NFC_TC_CE_HostLiteTest_002_003
 * @tc.name      : add aids for app in DH, there is conflict with the aids in routing table
 * @tc.desc      :
 */
TEST_F(TcNfcCeHostTest, TestAddNormalAidsForService_Test_203)
{
    DebugLog("TestAddNormalAidsForService_Test_203 in");
    mCeHostServiceData_->StartHceLiteTest(TcNfcCeData::HOST_NORMAL_ABILITY_NAME_2, TcNfcCeData::ABILITY_TYPE_NORMAL);
    mCeHostServiceData_->LiteAddDynamicAidsTestOk(true, TcNfcCeData::CONFICT_AIDS_WITH_HN_1);
    mCeHostServiceData_->LiteRemoveDynamicAidsTestOk(true);
}

/**
 * @tc.number    : NFC_TC_CE_HostLiteTest_002_004
 * @tc.name      : add aids for app in DH, there is conflict with the aids in routing table
 * @tc.desc      :
 */
TEST_F(TcNfcCeHostTest, TestAddNormalAidsForService_Test_204)
{
    DebugLog("TestAddNormalAidsForService_Test_204 in");
    mCeHostServiceData_->StartHceLiteTest(TcNfcCeData::HOST_NORMAL_ABILITY_NAME_2, TcNfcCeData::ABILITY_TYPE_NORMAL);
    mCeHostServiceData_->LiteAddDynamicAidsTestOk(true, TcNfcCeData::CONFICT_AIDS_WITH_PHN_1);
    mCeHostServiceData_->LiteRemoveDynamicAidsTestOk(true);
}

/**
 * @tc.number    : NFC_TC_CE_HostLiteTest_002_005
 * @tc.name      : add aids for app in DH, there is conflict with the aids in routing table
 * @tc.desc      :
 */
TEST_F(TcNfcCeHostTest, TestAddNormalAidsForService_Test_205)
{
    DebugLog("TestAddNormalAidsForService_Test_205 in");
    mCeHostServiceData_->StartHceLiteTest(TcNfcCeData::HOST_NORMAL_ABILITY_NAME_2, TcNfcCeData::ABILITY_TYPE_NORMAL);
    mCeHostServiceData_->LiteAddDynamicAidsTestOk(true, TcNfcCeData::CONFICT_PRE_AIDS_WITH_PHN_2S);
    mCeHostServiceData_->LiteRemoveDynamicAidsTestOk(true);
}

/**
 * @tc.number    : NFC_TC_CE_HostLiteTest_002_006
 * @tc.name      : add aids for app in DH, there is conflict with the aids in routing table
 * @tc.desc      :
 */
TEST_F(TcNfcCeHostTest, TestAddNormalAidsForService_Test_206)
{
    DebugLog("TestAddNormalAidsForService_Test_206 in");
    mCeHostServiceData_->StartHceLiteTest(TcNfcCeData::HOST_NORMAL_ABILITY_NAME_1, TcNfcCeData::ABILITY_TYPE_NORMAL);
    mCeHostServiceData_->LiteAddDynamicAidsTestOk(true, TcNfcCeData::CONFICT_PRE_AIDS_WITH_PHN_2L);
    mCeHostServiceData_->LiteRemoveDynamicAidsTestOk(true);
}

/**
 * @tc.number    : NFC_TC_CE_HostLiteTest_003_001
 * @tc.name      : Turn off NFC, then add aids for app in DH
 * @tc.desc      :
 */
TEST_F(TcNfcCeHostTest, TestAddNormalAidsForService_Test_301)
{
    DebugLog("TestAddNormalAidsForService_Test_301 in");
    mCeHostServiceData_->StartHceLiteTest(TcNfcCeData::HOST_NORMAL_ABILITY_NAME_1, TcNfcCeData::ABILITY_TYPE_NORMAL);
    mCeHostServiceData_->GetTestNfcCEAgent()->TurnOffNfc();
    mCeHostServiceData_->LiteAddDynamicAidsTestOk(true);
    mCeHostServiceData_->LiteRemoveDynamicAidsTestOk(true);
}

/**
 * @tc.number    : NFC_TC_CE_HostLiteTest_003_002
 * @tc.name      : Turn off NFC, then add aids for app in DH
 * @tc.desc      :
 */
TEST_F(TcNfcCeHostTest, TestAddNormalAidsForService_Test_302)
{
    DebugLog("TestAddNormalAidsForService_Test_302 in");
    mCeHostServiceData_->StartHceLiteTest(TcNfcCeData::HOST_SECURE_ABILITY_NAME_1, TcNfcCeData::ABILITY_TYPE_SECURE);
    mCeHostServiceData_->SetPrimaryServiceTest();
    mCeHostServiceData_->GetTestNfcCEAgent()->TurnOffNfc();
    mCeHostServiceData_->LiteAddDynamicAidsTestOk(false);
    mCeHostServiceData_->LiteRemoveDynamicAidsTestOk(false);
}
}  // namespace OHOS::nfc::test
