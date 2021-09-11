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
class TcNfcCeOffHostTestEnvSe : public testing::Test {
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
 * @tc.number    : NFC_TC_CE_OffHostLiteTest_001_001
 * @tc.name      : set default route as ESE1, App1 has installed in SE, set App1 as PrimaryService, add fully matched
 * security AIDS
 * @tc.desc      : Api test
 */
TEST_F(TcNfcCeOffHostTestEnvSe, AddAidsForService_Test101)
{
    DebugLog("AddAidsForService_Test101 in");
    mCeOffHostServiceData_->StartOffHceLiteTest(TcNfcCeData::SE_SECURE_ABILITY_NAME_1,
                                                TcNfcCeData::ABILITY_TYPE_SECURE);
    mCeOffHostServiceData_->SetPrimaryServiceTest();
    mCeOffHostServiceData_->AddDynamicAidsLiteTest(false);

    mCeOffHostServiceData_->RemoveAidsLiteTestOk(false);
}

/**
 * @tc.number    : NFC_TC_CE_OffHostLiteTest_001_002
 * @tc.name      : set default route as ESE1, App1 has installed in SE, add aid test
 * @tc.desc      : Api test
 */
TEST_F(TcNfcCeOffHostTestEnvSe, AddAidsForService_Test102)
{
    DebugLog("AddAidsForService_Test102 in");
    mCeOffHostServiceData_->StartOffHceLiteTest(TcNfcCeData::SE_SN_ABILITY_NAME_1, TcNfcCeData::ABILITY_TYPE_SECURE);
    mCeOffHostServiceData_->SetPrimaryServiceTest();
    mCeOffHostServiceData_->AddDynamicAidsLiteTest(true);

    mCeOffHostServiceData_->RemoveAidsLiteTestOk(true);
}

/**
 * @tc.number    : NFC_TC_CE_OffHostLiteTest_001_003
 * @tc.name      : set default route as ESE1, App1 has installed in SE, add aid test
 * @tc.desc      : Api test
 */
TEST_F(TcNfcCeOffHostTestEnvSe, AddAidsForService_Test103)
{
    DebugLog("AddAidsForService_Test103 in");
    mCeOffHostServiceData_->StartOffHceLiteTest(TcNfcCeData::SE_SECURE_ABILITY_NAME_2,
                                                TcNfcCeData::ABILITY_TYPE_SECURE);
    mCeOffHostServiceData_->SetPrimaryServiceTest();

    mCeOffHostServiceData_1->StartOffHceLiteTest(TcNfcCeData::SE_SECURE_ABILITY_NAME_1,
                                                 TcNfcCeData::ABILITY_TYPE_SECURE);
    mCeOffHostServiceData_1->AddDynamicAidsLiteTest(false, {}, false);
    bool res = mCeOffHostServiceData_1->GetOffHceLite()->RemoveAidsForService(TcNfcCeData::ABILITY_TYPE_SECURE);

    EXPECT_TRUE(res);
}

/**
 * @tc.number    : NFC_TC_CE_OffHostLiteTest_001_004
 * @tc.name      : set default route as ESE1, App1 has installed in SE, add aid test
 * @tc.desc      : Api test
 */
TEST_F(TcNfcCeOffHostTestEnvSe, AddAidsForService_Test104)
{
    DebugLog("AddAidsForService_Test104 in");
    mCeOffHostServiceData_->StartOffHceLiteTest(TcNfcCeData::SE_SECURE_ABILITY_NAME_2,
                                                TcNfcCeData::ABILITY_TYPE_SECURE);
    mCeOffHostServiceData_->SetPrimaryServiceTest();

    mCeOffHostServiceData_1->StartOffHceLiteTest(TcNfcCeData::SE_NORMAL_ABILITY_NAME_1,
                                                 TcNfcCeData::ABILITY_TYPE_NORMAL);
    mCeOffHostServiceData_1->AddDynamicAidsLiteTest(true);

    mCeOffHostServiceData_1->RemoveAidsLiteTestOk(true);
}

/**
 * @tc.number    : NFC_TC_CE_OffHostLiteTest_002_001
 * @tc.name      : set default route as ESE1, App1 has installed in SE, remove aid test
 * @tc.desc      : Api test
 */
TEST_F(TcNfcCeOffHostTestEnvSe, AddAidsForService_Test201)
{
    DebugLog("AddAidsForService_Test201 in");
    mCeOffHostServiceData_->StartOffHceLiteTest(TcNfcCeData::SE_SECURE_ABILITY_NAME_1,
                                                TcNfcCeData::ABILITY_TYPE_SECURE);
    mCeOffHostServiceData_->SetPrimaryServiceTest();
    mCeOffHostServiceData_->AddDynamicAidsLiteTest(false);
    mCeOffHostServiceData_->RemoveAidsLiteTestOk(false);
}

/**
 * @tc.number    : NFC_TC_CE_OffHostLiteTest_002_002
 * @tc.name      : set default route as ESE1, App1 has installed in SE, remove aid test
 * @tc.desc      : Api test
 */
TEST_F(TcNfcCeOffHostTestEnvSe, AddAidsForService_Test202)
{
    DebugLog("AddAidsForService_Test202 in");
    mCeOffHostServiceData_->StartOffHceLiteTest(TcNfcCeData::SE_SN_ABILITY_NAME_1, TcNfcCeData::ABILITY_TYPE_SECURE);
    mCeOffHostServiceData_->SetPrimaryServiceTest();
    mCeOffHostServiceData_->AddDynamicAidsLiteTest(true);
    mCeOffHostServiceData_->RemoveAidsLiteTestOk(true);
}

/**
 * @tc.number    : NFC_TC_CE_OffHostLiteTest_002_003
 * @tc.name      : set default route as ESE1, App1 has installed in SE, remove aid test
 * @tc.desc      : Api test
 */
TEST_F(TcNfcCeOffHostTestEnvSe, AddAidsForService_Test203)
{
    DebugLog("AddAidsForService_Test203 in");
    mCeOffHostServiceData_->StartOffHceLiteTest(TcNfcCeData::SE_SECURE_ABILITY_NAME_1,
                                                TcNfcCeData::ABILITY_TYPE_SECURE);
    mCeOffHostServiceData_->SetPrimaryServiceTest();

    mCeOffHostServiceData_1->StartOffHceLiteTest(TcNfcCeData::SE_SECURE_ABILITY_NAME_2,
                                                 TcNfcCeData::ABILITY_TYPE_SECURE);
    mCeOffHostServiceData_1->AddDynamicAidsLiteTest(false, {}, false);
    bool res = mCeOffHostServiceData_1->GetOffHceLite()->RemoveAidsForService(TcNfcCeData::ABILITY_TYPE_SECURE);

    EXPECT_TRUE(res);
}

/**
 * @tc.number    : NFC_TC_CE_OffHostLiteTest_002_004
 * @tc.name      : set default route as ESE1, App1 has installed in SE, remove aid test
 * @tc.desc      : Api test
 */
TEST_F(TcNfcCeOffHostTestEnvSe, AddAidsForService_Test204)
{
    DebugLog("AddAidsForService_Test203 in");
    mCeOffHostServiceData_->StartOffHceLiteTest(TcNfcCeData::SE_SECURE_ABILITY_NAME_1,
                                                TcNfcCeData::ABILITY_TYPE_SECURE);
    mCeOffHostServiceData_->SetPrimaryServiceTest();

    mCeOffHostServiceData_1->StartOffHceLiteTest(TcNfcCeData::SE_NORMAL_ABILITY_NAME_1,
                                                 TcNfcCeData::ABILITY_TYPE_NORMAL);
    mCeOffHostServiceData_1->AddDynamicAidsLiteTest(true);
    mCeOffHostServiceData_1->RemoveAidsLiteTestOk(true);
}
}  // namespace OHOS::nfc::test
