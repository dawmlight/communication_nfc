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
class TcNfcCeHostServiceTestEnvDh : public testing::Test {
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
 * @tc.number    : NFC_TC_CE_HostTest_010_001
 * @tc.name      : set default route as DH, App1 has installed in DH, set App1 as PrimaryService, add fully matched
 * security AIDS
 * @tc.desc      :
 */
TEST_F(TcNfcCeHostServiceTestEnvDh, AddAidsForService_DhEnv_Test1001)
{
    DebugLog("AddAidsForService_DhEnv_Test1001 in");
    mCeHostServiceData_->StartHceTest(TcNfcCeData::HOST_SECURE_ABILITY_NAME_1, TcNfcCeData::ABILITY_TYPE_SECURE);

    mCeHostServiceData_->SetPrimaryServiceTest();
    mCeHostServiceData_->AddDynamicAidsTest(false);
    mCeHostServiceData_->RemoveAidsTestOk(false);
}

/**
 * @tc.number    : NFC_TC_CE_HostTest_010_002
 * @tc.name      : set default route as DH, App1 has installed in DH, set App1 as PrimaryService, add prefix
 * matching security AIDS
 * @tc.desc      :
 */
TEST_F(TcNfcCeHostServiceTestEnvDh, AddAidsForService_DhEnv_Test1002)
{
    DebugLog("AddAidsForService_DhEnv_Test1002 in");
    mCeHostServiceData_->StartHceTest(TcNfcCeData::PRE_HOST_SECURE_ABILITY_NAME_1, TcNfcCeData::ABILITY_TYPE_SECURE);

    mCeHostServiceData_->SetPrimaryServiceTest();
    mCeHostServiceData_->AddDynamicAidsTest(false);
    mCeHostServiceData_->RemoveAidsTestOk(false);
}

/**
 * @tc.number    : NFC_TC_CE_HostTest_010_003
 * @tc.name      : set default route as DH, App1 has installed in DH, set App1 as PrimaryService, add fully matched
 * normal type AIDS
 * @tc.desc      :
 */
TEST_F(TcNfcCeHostServiceTestEnvDh, AddAidsForService_DhEnv_Test1003)
{
    DebugLog("AddAidsForService_DhEnv_Test1003 in");
    mCeHostServiceData_->StartHceTest(TcNfcCeData::HOST_SN_ABILITY_NAME_1, TcNfcCeData::ABILITY_TYPE_SECURE);

    mCeHostServiceData_->SetPrimaryServiceTest();
    mCeHostServiceData_->AddDynamicAidsTest(true);
    mCeHostServiceData_->RemoveAidsTestOk(true);
}

/**
 * @tc.number    : NFC_TC_CE_HostTest_010_004
 * @tc.name      : Route table overflow test when verifying that the default route is DH
 * @tc.desc      :
 */
TEST_F(TcNfcCeHostServiceTestEnvDh, AddAidsForService_DhEnv_Test1004)
{
    DebugLog("AddAidsForService_DhEnv_Test1004 in");
    mCeHostServiceData_->StartHceTest(TcNfcCeData::HOST_SN_ABILITY_NAME_1, TcNfcCeData::ABILITY_TYPE_SECURE);

    mCeHostServiceData_->SetPrimaryServiceTest();
    mCeHostServiceData_->AddDynamicAidsTest(true);
    mCeHostServiceData_->RemoveAidsTestOk(true);
}
}  // namespace OHOS::nfc::test
