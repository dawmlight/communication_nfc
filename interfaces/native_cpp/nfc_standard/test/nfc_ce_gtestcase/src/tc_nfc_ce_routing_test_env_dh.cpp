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
class TcNfcCeRoutingTestEnvDh : public testing::Test {
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
 * @tc.number    : NFC_TC_CE_RoutingOverTest_001_008 \ NFC_TC_CE_RoutingOverTest_001_009
 * @tc.name      : NFC_TC_CE_RoutingOverTest_001_008 : set default route as DH, App1 has installed in DH, set App1 as
 * PrimaryService, add a large AIDS for app1 success
 *                 NFC_TC_CE_RoutingOverTest_001_009 : set default route as DH, all the app can add the normal aids
success
 * @tc.desc      : 1. Change to configuration NFC_TC_CE_RoutingOverTest_001_008 to test case
NFC_TC_CE_RoutingOverTest_001_008
 *                 2. Change to configuration NFC_TC_CE_RoutingOverTest_001_009 to test case
NFC_TC_CE_RoutingOverTest_001_009
 */
TEST_F(TcNfcCeRoutingTestEnvDh, RoutingOverTest_001_008_009)
{
    DebugLog("NFC_TC_CE_RoutingOverTest_001_008_009 in");
    std::string type = TcNfcCeData::ABILITY_TYPE_SECURE;
    mCeHostServiceData_->StartHceTest(TcNfcCeData::HOST_SECURE_ABILITY_NAME_1, type);

    ElementName serviceName;
    bool res = mCeHostServiceData_->GetElementName(type, serviceName);
    EXPECT_TRUE(res);

    std::vector<std::string> staticAids = mCeHostServiceData_->GetTestNfcCEAgent()->GetAids(type, false);
    res = mCeHostServiceData_->GetHce()->SetPrimaryServiceForType(serviceName, type);
    EXPECT_TRUE(res);
    mCeHostServiceData_->SetExpectAids(staticAids);

    std::string sname = mCeHostServiceData_->GetHce()->GetPrimaryServiceForType(type);
    res = (sname == serviceName.GetAbilityName()) ? true : false;
    EXPECT_TRUE(res);

    DebugLog("PrimaryService name.%s expect name.%s", sname.c_str(), serviceName.GetAbilityName().c_str());

    // other app in host can add aids
    mCeHostServiceData_N->StartHceTest(TcNfcCeData::HOST_NORMAL_ABILITY_NAME_1, TcNfcCeData::ABILITY_TYPE_NORMAL);
    mCeHostServiceData_N->AddDynamicAidsTest(true);
    mCeHostServiceData_N->RemoveAidsTestOk(true);

    // other app in se can add aids
    mCeOffHostServiceData_->StartOffHceLiteTest(TcNfcCeData::SE_NORMAL_ABILITY_NAME_1,
                                                TcNfcCeData::ABILITY_TYPE_NORMAL);
    mCeOffHostServiceData_->AddDynamicAidsLiteTest(true);
    mCeOffHostServiceData_->RemoveAidsLiteTestOk(true);
}
}  // namespace OHOS::nfc::test
