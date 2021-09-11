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
#include "ioffhost_card_emulation.h"
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
class TcNfcCeOffHostServiceTest : public testing::Test {
public:
    virtual void SetUp() override
    {
        std::cout << " SetUp TcNfcCeHostServiceTest." << std::endl;
        mCeOffHostServiceData_ = std::make_shared<TcNfcCeOffHostServiceData>();
        mCeOffHostServiceData_1 = std::make_shared<TcNfcCeOffHostServiceData>();
    }

    virtual void TearDown() override
    {
        std::cout << " TearDown TcNfcCeHostServiceTest." << std::endl;
        if (mCeOffHostServiceData_->GetTestNfcCEAgent()) {
            mCeOffHostServiceData_->StopOffHceLiteTest();
        }
        if (mCeOffHostServiceData_1->GetTestNfcCEAgent()) {
            mCeOffHostServiceData_1->StopOffHceLiteTest();
        }
    }

public:
    std::shared_ptr<TcNfcCeOffHostServiceData> mCeOffHostServiceData_;
    std::shared_ptr<TcNfcCeOffHostServiceData> mCeOffHostServiceData_1;
};

/**
 * @tc.number    : NFC_TC_CE_OffHostLiteTest_003_001
 * @tc.name      : set default route as DH, App1 has installed in SE, regist app1 as offhost service in SE1
 * @tc.desc      : Api test
 */
TEST_F(TcNfcCeOffHostServiceTest, AddAidsForService_Test301)
{
    DebugLog("AddAidsForService_Test301 in");
    mCeOffHostServiceData_->StartOffHceLiteTest(TcNfcCeData::SE_SECURE_ABILITY_NAME_1,
                                                TcNfcCeData::ABILITY_TYPE_SECURE);
    mCeOffHostServiceData_->SetPrimaryServiceTest();
    mCeOffHostServiceData_->RegisterOffHostServiceTest(false);
    mCeOffHostServiceData_->AddDynamicAidsLiteTest(false);

    mCeOffHostServiceData_->RemoveAidsLiteTestOk(false);
    mCeOffHostServiceData_->UnRegisterOffHostServiceTestOk(false);
}

/**
 * @tc.number    : NFC_TC_CE_OffHostLiteTest_004_001
 * @tc.name      : set default route as DH, App1 has installed in SE, regist app1 as offhost service in SE1
 * @tc.desc      : Api test
 */
TEST_F(TcNfcCeOffHostServiceTest, RegisterOffHostService_Test401)
{
    DebugLog("AddAidsForService_Test301 in");
    mCeOffHostServiceData_->StartOffHceLiteTest(TcNfcCeData::SE_SECURE_ABILITY_NAME_1,
                                                TcNfcCeData::ABILITY_TYPE_SECURE);
    mCeOffHostServiceData_->SetPrimaryServiceTest();

    mCeOffHostServiceData_->RegisterOffHostServiceTest(false);
    mCeOffHostServiceData_->UnRegisterOffHostServiceTestOk(false);
}

/**
 * @tc.number    : NFC_TC_CE_OffHostLiteTest_004_002
 * @tc.name      : set default route as DH, App1 has installed in SE, regist app1 as offhost service in SE1
 * @tc.desc      : Api test
 */
TEST_F(TcNfcCeOffHostServiceTest, RegisterOffHostService_Test402)
{
    DebugLog("RegisterOffHostService_Test402 in");
    mCeOffHostServiceData_->StartOffHceLiteTest(TcNfcCeData::SE_SECURE_ABILITY_NAME_1,
                                                TcNfcCeData::ABILITY_TYPE_SECURE);
    mCeOffHostServiceData_->SetPrimaryServiceTest();

    bool res = mCeOffHostServiceData_->GetOffHceLite()->RegisterOffHostService("host");
    EXPECT_FALSE(res);
}

/**
 * @tc.number    : NFC_TC_CE_OffHostLiteTest_004_007
 * @tc.name      : set default route as DH, App1 has installed in DH, regist app1 as offhost service in DH, but app1 can
 * not recieve any command from service
 * @tc.desc      :  Card swiping verification
 */
TEST_F(TcNfcCeOffHostServiceTest, RegisterOffHostService_Test407)
{
    DebugLog("RegisterOffHostService_Test407 in");
    mCeOffHostServiceData_->StartOffHceLiteTest(TcNfcCeData::HOST_SECURE_ABILITY_NAME_1,
                                                TcNfcCeData::ABILITY_TYPE_SECURE);

    bool res = mCeOffHostServiceData_->GetOffHceLite()->RegisterOffHostService(
        mCeOffHostServiceData_->GetCEAbilityInfo()->env_);
    EXPECT_FALSE(res);

    res = mCeOffHostServiceData_->GetOffHceLite()->UnregisterOffHostService();
    EXPECT_FALSE(res);
}

/**
 * @tc.number    : NFC_TC_CE_OffHostLiteTest_004_008
 * @tc.name      : set default route as DH, App1 has installed in SE, add aids test
 * @tc.desc      : Api test
 */
TEST_F(TcNfcCeOffHostServiceTest, RegisterOffHostService_Test408)
{
    DebugLog("RegisterOffHostService_Test408 in");
    mCeOffHostServiceData_->StartOffHceLiteTest(TcNfcCeData::SE_SECURE_ABILITY_NAME_1,
                                                TcNfcCeData::ABILITY_TYPE_SECURE);
    mCeOffHostServiceData_->SetPrimaryServiceTest();

    mCeOffHostServiceData_->RegisterOffHostServiceTest(false);

    mCeOffHostServiceData_->AddDynamicAidsLiteTest(false);
    mCeOffHostServiceData_->RemoveAidsLiteTestOk(false);

    mCeOffHostServiceData_->UnRegisterOffHostServiceTestOk(false);
}

/**
 * @tc.number    : NFC_TC_CE_OffHostLiteTest_004_009
 * @tc.name      : set default route as DH, App1 has installed in SE, add aids test
 * @tc.desc      : Api test
 */
TEST_F(TcNfcCeOffHostServiceTest, RegisterOffHostService_Test409)
{
    DebugLog("RegisterOffHostService_Test409 in");

    mCeOffHostServiceData_->StartOffHceLiteTest(TcNfcCeData::SE_SECURE_ABILITY_NAME_2,
                                                TcNfcCeData::ABILITY_TYPE_SECURE);
    mCeOffHostServiceData_->SetPrimaryServiceTest();

    mCeOffHostServiceData_1->StartOffHceLiteTest(TcNfcCeData::SE_SECURE_ABILITY_NAME_1,
                                                 TcNfcCeData::ABILITY_TYPE_SECURE);

    bool res = mCeOffHostServiceData_1->GetOffHceLite()->RegisterOffHostService(
        mCeOffHostServiceData_1->GetCEAbilityInfo()->env_);
    EXPECT_TRUE(res);
    std::vector<std::string> testAids =
        mCeOffHostServiceData_1->GetTestNfcCEAgent()->GetAids(TcNfcCeData::ABILITY_TYPE_SECURE, false);
    mCeOffHostServiceData_1->SetExpectAids({}, testAids, false);

    mCeOffHostServiceData_1->AddDynamicAidsLiteTest(false, {}, false);
    res = mCeOffHostServiceData_1->GetOffHceLite()->RemoveAidsForService(TcNfcCeData::ABILITY_TYPE_SECURE);
    EXPECT_TRUE(res);

    res = mCeOffHostServiceData_1->GetOffHceLite()->UnregisterOffHostService();
    EXPECT_TRUE(res);
    mCeOffHostServiceData_1->SetExpectAids({}, testAids, false);
}

/**
 * @tc.number    : NFC_TC_CE_OffHostLiteTest_004_010
 * @tc.name      : set default route as DH, App1 has installed in SE, add aids test
 * @tc.desc      : Api test
 */
TEST_F(TcNfcCeOffHostServiceTest, RegisterOffHostService_Test410)
{
    DebugLog("RegisterOffHostService_Test410 in");

    mCeOffHostServiceData_->StartOffHceLiteTest(TcNfcCeData::SE_SECURE_ABILITY_NAME_2,
                                                TcNfcCeData::ABILITY_TYPE_SECURE);
    mCeOffHostServiceData_->SetPrimaryServiceTest();

    mCeOffHostServiceData_1->StartOffHceLiteTest(TcNfcCeData::SE_NORMAL_ABILITY_NAME_1,
                                                 TcNfcCeData::ABILITY_TYPE_NORMAL);

    mCeOffHostServiceData_1->RegisterOffHostServiceTest(true);

    mCeOffHostServiceData_1->AddDynamicAidsLiteTest(true);
    mCeOffHostServiceData_1->RemoveAidsLiteTestOk(true);
    mCeOffHostServiceData_1->UnRegisterOffHostServiceTestOk(true);
}

/**
 * @tc.number    : NFC_TC_CE_OffHostLiteTest_004_011
 * @tc.name      : set default route as DH, App1 has installed in SE, add aids test
 * @tc.desc      : Api test
 */
TEST_F(TcNfcCeOffHostServiceTest, RegisterOffHostService_Test411)
{
    DebugLog("RegisterOffHostService_Test411 in");
    mCeOffHostServiceData_->StartOffHceLiteTest(TcNfcCeData::SE_SECURE_ABILITY_NAME_1,
                                                TcNfcCeData::ABILITY_TYPE_SECURE);

    std::vector<std::string> testAids =
        mCeOffHostServiceData_->GetTestNfcCEAgent()->GetAids(TcNfcCeData::ABILITY_TYPE_SECURE, false);
    bool res = mCeOffHostServiceData_->GetOffHceLite()->RegisterOffHostService("eSE3");
    EXPECT_TRUE(res);
    mCeOffHostServiceData_->SetExpectAids({}, testAids, false);

    res = mCeOffHostServiceData_->GetOffHceLite()->UnregisterOffHostService();
    EXPECT_TRUE(res);

    mCeOffHostServiceData_->SetExpectAids({}, testAids, false);
}
}  // namespace OHOS::nfc::test
