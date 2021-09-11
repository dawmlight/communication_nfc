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
#include "ioffhost_card_emulation.h"
#include "loghelper.h"
#include "nfc_agent.h"
#include "tc_nfc_ce_host_service_data.h"
#include "tc_nfc_ce_offhost_service_data.h"
#include "test_nfc_ce_ability.h"
#include "test_nfc_ce_agent.h"
#include "want.h"

// Every cases here can only be carried out by adjusting the aid configuration file or modifying the default
// configuration file. Automatic tests cannot be carried out
using namespace testing;
using namespace std;
using namespace OHOS::AAFwk;
using namespace OHOS::AppExecFwk;
using namespace OHOS::nfc::sdk::cardemulation;
namespace OHOS::nfc::test {
class TcNfcCeRoutingTestEnvSe : public testing::Test {
public:
    virtual void SetUp() override
    {
        std::cout << " SetUp TcNfcCeHostServiceTest." << std::endl;
        mCeHostServiceData_ = std::make_shared<TcNfcCeHostServiceData>();
        mCeOffHostServiceData_ = std::make_shared<TcNfcCeOffHostServiceData>();
        mCeHostServiceData_N = std::make_shared<TcNfcCeHostServiceData>();
    }

    virtual void TearDown() override
    {
        std::cout << " TearDown TcNfcCeHostServiceTest." << std::endl;

        if (mCeHostServiceData_->GetTestNfcCEAgent()) {
            mCeHostServiceData_->StopHceTest();
        }

        if (mCeOffHostServiceData_->GetTestNfcCEAgent()) {
            mCeOffHostServiceData_->StopOffHceLiteTest();
        }
        if (mCeHostServiceData_N->GetTestNfcCEAgent()) {
            mCeHostServiceData_N->StopHceTest();
        }
    }

public:
    std::shared_ptr<TcNfcCeHostServiceData> mCeHostServiceData_;
    std::shared_ptr<TcNfcCeOffHostServiceData> mCeOffHostServiceData_;
    std::shared_ptr<TcNfcCeHostServiceData> mCeHostServiceData_N;
};

/**
 * @tc.number    : NFC_TC_CE_RoutingOverTest_001_001
 * @tc.name      : set default route as SE1, App1 has installed in DH, set App1 as PrimaryService, add a large AIDS for
 * app1 success, not add into routing table
 * @tc.desc      : change to configuration NFC_TC_CE_RoutingOverTest_001_001
 */
TEST_F(TcNfcCeRoutingTestEnvSe, RoutingOverTest_001_001)
{
    DebugLog("NFC_TC_CE_RoutingOverTest_001_001 in");
    std::string type = TcNfcCeData::ABILITY_TYPE_SECURE;
    mCeHostServiceData_->StartHceTest(TcNfcCeData::HOST_SECURE_ABILITY_NAME_1, type);

    ElementName serviceName;
    bool res = mCeHostServiceData_->GetElementName(type, serviceName);
    EXPECT_TRUE(res);

    std::vector<std::string> staticAids = mCeHostServiceData_->GetTestNfcCEAgent()->GetAids(type, false);
    res = mCeHostServiceData_->GetHce()->SetPrimaryServiceForType(serviceName, type);
    EXPECT_TRUE(res);
    mCeHostServiceData_->SetExpectAids({}, staticAids);

    std::string sname = mCeHostServiceData_->GetHce()->GetPrimaryServiceForType(type);
    res = (sname == serviceName.GetAbilityName()) ? true : false;
    EXPECT_TRUE(res);

    DebugLog("PrimaryService name.%s expect name.%s", sname.c_str(), serviceName.GetAbilityName().c_str());

    // other app add aids
    mCeHostServiceData_N->StartHceTest(TcNfcCeData::HOST_NORMAL_ABILITY_NAME_1, TcNfcCeData::ABILITY_TYPE_NORMAL);

    std::string type_1 = TcNfcCeData::ABILITY_TYPE_NORMAL;
    ElementName serviceName_1;
    res = mCeHostServiceData_N->GetElementName(type_1, serviceName_1);
    EXPECT_TRUE(res);

    std::vector<std::string> testAids = mCeHostServiceData_N->GetTestNfcCEAgent()->GetAids(type_1, true);
    res = mCeHostServiceData_N->GetHce()->AddAidsForService(serviceName_1, testAids, type_1);
    EXPECT_TRUE(res);
    mCeHostServiceData_N->SetExpectAids({}, testAids);

    std::vector<std::string> excludeAids = mCeHostServiceData_N->GetTestNfcCEAgent()->GetAids(type_1, false);
    res = mCeHostServiceData_N->GetHce()->RemoveAidsForService(serviceName_1, type_1);
    EXPECT_TRUE(res);
    mCeHostServiceData_N->SetExpectAids({}, excludeAids);
}

/**
 * @tc.number    : NFC_TC_CE_RoutingOverTest_001_002
 * @tc.name      : set default route as SE1, App1 has installed in DH, there are too many aids to add into routing table
 * @tc.desc      : 1. change to configuration NFC_TC_CE_RoutingOverTest_001_002
 */
TEST_F(TcNfcCeRoutingTestEnvSe, RoutingOverTest_001_002)
{
    DebugLog("RoutingOverTest_001_002 in");
    mCeHostServiceData_->StartHceTest(TcNfcCeData::HOST_NORMAL_ABILITY_NAME_1, TcNfcCeData::ABILITY_TYPE_NORMAL);

    mCeHostServiceData_->AddDynamicAidsTest(true);
    mCeHostServiceData_->RemoveAidsTestOk(true);

    // other app
    mCeHostServiceData_N->StartHceTest(TcNfcCeData::HOST_NORMAL_ABILITY_NAME_N, TcNfcCeData::ABILITY_TYPE_NORMAL);

    std::string type = TcNfcCeData::ABILITY_TYPE_NORMAL;
    ElementName serviceName;
    bool res = mCeHostServiceData_N->GetElementName(type, serviceName);
    EXPECT_TRUE(res);

    std::vector<std::string> testAids = mCeHostServiceData_N->GetTestNfcCEAgent()->GetAids(type, true);
    res = mCeHostServiceData_N->GetHce()->AddAidsForService(serviceName, testAids, type);
    EXPECT_TRUE(res);
    mCeHostServiceData_N->SetExpectAids({}, testAids);

    std::vector<std::string> excludeAids =
        mCeHostServiceData_N->GetTestNfcCEAgent()->GetAids(TcNfcCeData::ABILITY_TYPE_SECURE, false);
    mCeHostServiceData_N->GetHce()->RemoveAidsForService(serviceName, type);
    mCeHostServiceData_N->SetExpectAids({}, excludeAids);
}

/**
 * @tc.number    : NFC_TC_CE_RoutingOverTest_001_003
 * @tc.name      : set default route as SE1, App1 has installed in DH, there are too many aids to add into routing
 * table, uninstall some app, then the behind aids can be add into routing table
 * @tc.desc      : 1. change to configuration NFC_TC_CE_RoutingOverTest_001_003
 */
TEST_F(TcNfcCeRoutingTestEnvSe, RoutingOverTest_001_003)
{
    DebugLog("RoutingOverTest_001_002 in");
    mCeHostServiceData_->StartHceTest(TcNfcCeData::HOST_NORMAL_ABILITY_NAME_1, TcNfcCeData::ABILITY_TYPE_NORMAL);

    mCeHostServiceData_->AddDynamicAidsTest(true);
    mCeHostServiceData_->RemoveAidsTestOk(true);

    // other app
    mCeHostServiceData_N->StartHceTest(TcNfcCeData::HOST_NORMAL_ABILITY_NAME_N, TcNfcCeData::ABILITY_TYPE_NORMAL);

    mCeHostServiceData_N->AddDynamicAidsTest(true);
    mCeHostServiceData_N->RemoveAidsTestOk(true);
}
/**
 * @tc.number    : NFC_TC_CE_RoutingOverTest_001_004
 * @tc.name      : set default route as SE1, App1 has installed in DH, there are too many aids to add into routing
 * table, set app as preferred service, then its aids ca be add into routing table
 * @tc.desc      : 1. change to configuration NFC_TC_CE_RoutingOverTest_001_004
 */
TEST_F(TcNfcCeRoutingTestEnvSe, RoutingOverTest_001_004)
{
    DebugLog("NFC_TC_CE_RoutingOverTest_001_004 in");
    // add normal
    std::string type = TcNfcCeData::ABILITY_TYPE_NORMAL;
    mCeHostServiceData_->StartHceTest(TcNfcCeData::HOST_NORMAL_ABILITY_NAME_N, type);

    ElementName serviceName;
    bool res = mCeHostServiceData_->GetElementName(type, serviceName);
    EXPECT_TRUE(res);

    mCeHostServiceData_->GetHce()->RemoveAidsForService(serviceName, type);
    std::vector<std::string> excludeAids = mCeHostServiceData_->GetTestNfcCEAgent()->GetAids(type, false);
    mCeHostServiceData_->SetExpectAids({}, excludeAids);

    mCeHostServiceData_->SetPreferredServiceTest();
    mCeHostServiceData_->AddDynamicAidsTest(true);
    mCeHostServiceData_->GetHce()->RemoveAidsForService(serviceName, type);
    mCeHostServiceData_->SetExpectAids(excludeAids);
}

/**
 * @tc.number    : NFC_TC_CE_RoutingOverTest_001_005
 * @tc.name      : set default route as SE1, App1 has installed in DH, there are too many aids to add into routing
 * table, set app as primary service, then its aids ca be add into routing table
 * @tc.desc      : 1. change to configuration NFC_TC_CE_RoutingOverTest_001_005
 */
TEST_F(TcNfcCeRoutingTestEnvSe, RoutingOverTest_001_005)
{
    DebugLog("NFC_TC_CE_RoutingOverTest_001_005 in");
    // add secure
    std::string type = TcNfcCeData::ABILITY_TYPE_SECURE;
    mCeHostServiceData_->StartHceTest(TcNfcCeData::HOST_SN_ABILITY_NAME_1, type);

    ElementName serviceName;
    bool res = mCeHostServiceData_->GetElementName(type, serviceName);
    EXPECT_TRUE(res);

    mCeHostServiceData_->GetHce()->RemoveAidsForService(serviceName, type);
    std::vector<std::string> excludeAids = mCeHostServiceData_->GetTestNfcCEAgent()->GetAids(type, false);
    mCeHostServiceData_->SetExpectAids({}, excludeAids);

    mCeHostServiceData_->SetPrimaryServiceTest();
    mCeHostServiceData_->AddDynamicAidsTest(true);
    mCeHostServiceData_->GetHce()->RemoveAidsForService(serviceName, type);
    mCeHostServiceData_->SetExpectAids(excludeAids);
}

/**
 * @tc.number    : NFC_TC_CE_RoutingOverTest_001_006 \ NFC_TC_CE_RoutingOverTest_001_007
 * @tc.name      : NFC_TC_CE_RoutingOverTest_001_006 : set default route as SE1, App1 has installed in SE1, all the app
 * in SE1 can be add aids success NFC_TC_CE_RoutingOverTest_001_007 : set default route as SE1, App1 has installed in
 * SE1, the app in DH can be add aids success
 * @tc.desc      : 1.  change to configuration NFC_TC_CE_RoutingOverTest_001_006 to test case
NFC_TC_CE_RoutingOverTest_001_006
 *                 2. change to configuration NFC_TC_CE_RoutingOverTest_001_007 to test case
NFC_TC_CE_RoutingOverTest_001_007
 */
TEST_F(TcNfcCeRoutingTestEnvSe, RoutingOverTest_001_006_007)
{
    DebugLog("NFC_TC_CE_RoutingOverTest_001_006_007 in");
    std::string type = TcNfcCeData::ABILITY_TYPE_SECURE;
    mCeOffHostServiceData_->StartOffHceLiteTest(TcNfcCeData::SE_SECURE_ABILITY_NAME_1, type);

    std::vector<std::string> testAids = mCeOffHostServiceData_->GetTestNfcCEAgent()->GetAids(type, true);
    bool res = mCeOffHostServiceData_->GetOffHceLite()->AddAidsForService(testAids, type);
    EXPECT_TRUE(res);
    mCeOffHostServiceData_->SetExpectAids(testAids, {}, false);

    mCeOffHostServiceData_->GetOffHceLite()->RemoveAidsForService(type);
    mCeOffHostServiceData_->SetExpectAids({}, testAids, false);

    // other app in DH
    mCeHostServiceData_->StartHceTest(TcNfcCeData::HOST_NORMAL_ABILITY_NAME_1, TcNfcCeData::ABILITY_TYPE_NORMAL);
    mCeHostServiceData_->AddDynamicAidsTest(true);
    mCeHostServiceData_->RemoveAidsTestOk(true);
}
}  // namespace OHOS::nfc::test
