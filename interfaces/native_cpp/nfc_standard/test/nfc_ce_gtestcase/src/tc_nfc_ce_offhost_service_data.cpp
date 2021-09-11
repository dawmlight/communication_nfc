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
#include "tc_nfc_ce_offhost_service_data.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <thread>

#include "card_emulation.h"
#include "card_emulation_util.h"
#include "element_name.h"
#include "ioffhost_card_emulation.h"
#include "iservice_registry.h"
#include "loghelper.h"
#include "nfc_agent.h"
#include "stdlib.h"
#include "string_ex.h"
#include "tc_nfc_ce_data.h"
#include "test_nfc_ce_ability.h"
#include "test_nfc_ce_agent.h"
#include "want.h"

using namespace std;
using namespace OHOS::AAFwk;
using namespace OHOS::AppExecFwk;
using namespace OHOS::nfc::sdk::cardemulation;
namespace OHOS::nfc::test {
void TcNfcCeOffHostServiceData::StartOffHceLiteTest(const std::string& abilityName, const std::string type)
{
    Init(abilityName);
    ElementName serviceName;
    GetElementName(type, serviceName);
    mOffHceLite_ = mTestNfcCEAgent_->GetNfcAgent()->CreateOffHostCardEmulation(serviceName);
}

void TcNfcCeOffHostServiceData::StopOffHceLiteTest()
{
    mTestNfcCEAgent_->TerminalAbility();
    mOffHceLite_ = std::shared_ptr<IOffHostCardEmulation>();
}

void TcNfcCeOffHostServiceData::AddDynamicAidsLiteTest(bool isNormal,
                                                       std::vector<std::string> aids,
                                                       bool bAddinRoutingTable)
{
    DebugLog("TcNfcCeOffHostServiceData::AddDynamicAidsTestOk in");
    // // device nama bundlename should be null
    std::string type = isNormal ? TcNfcCeData::ABILITY_TYPE_NORMAL : TcNfcCeData::ABILITY_TYPE_SECURE;

    EXPECT_TRUE(mTestNfcCEAgent_);
    std::vector<std::string> testAids = (aids.size() == 0) ? mTestNfcCEAgent_->GetAids(type, true) : std::move(aids);

    DebugLog("AddAidTestOk AddAidsForService ability.%s type.%s", mCEAbilityInfo_->abilityName_.c_str(), type.c_str());
    for (size_t i = 0; i < testAids.size(); i++) {
        printf("aid.%d(%s)\n", i, testAids.at(i).c_str());
    }

    bool res = mOffHceLite_->AddAidsForService(testAids, type);
    EXPECT_TRUE(res);

    if (bAddinRoutingTable) {
        SetExpectAids(testAids, {}, false);
    }

    return;
}

void TcNfcCeOffHostServiceData::RemoveAidsLiteTestOk(bool isNormal)
{
    // device nama bundlename should be null
    std::string type = isNormal ? TcNfcCeData::ABILITY_TYPE_NORMAL : TcNfcCeData::ABILITY_TYPE_SECURE;
    DebugLog("RemoveAidsLiteTestOk ability.%s type.%s", mCEAbilityInfo_->abilityName_.c_str(), type.c_str());
    bool res = mOffHceLite_->RemoveAidsForService(type);
    EXPECT_TRUE(res);

    std::vector<std::string> testAids = mTestNfcCEAgent_->GetAids(type, false);
    SetExpectAids(testAids, {}, false);
}

void TcNfcCeOffHostServiceData::SetPrimaryServiceTest()
{
    DebugLog("SetPrimaryServiceTest in");
    DebugLog("SetPrimaryServiceTest ability.%s", mCEAbilityInfo_->abilityName_.c_str());
    OHOS::AppExecFwk::ElementName serviceName;
    bool res = GetElementName(TcNfcCeData::ABILITY_TYPE_SECURE, serviceName);
    EXPECT_TRUE(res);

    res = mHce_->SetPrimaryServiceForType(serviceName, mCEAbilityInfo_->aidsets_.at(0)->type_);
    EXPECT_TRUE(res);

    std::vector<std::string> testAids = mTestNfcCEAgent_->GetAids(TcNfcCeData::ABILITY_TYPE_SECURE, false);
    SetExpectAids(testAids, {}, false);
}

void TcNfcCeOffHostServiceData::RegisterOffHostServiceTest(bool isNormal)
{
    std::string type = isNormal ? TcNfcCeData::ABILITY_TYPE_NORMAL : TcNfcCeData::ABILITY_TYPE_SECURE;
    DebugLog("RemoveAidsLiteTestOk ability.%s type.%s", mCEAbilityInfo_->abilityName_.c_str(), type.c_str());

    bool res = mOffHceLite_->RegisterOffHostService(mCEAbilityInfo_->env_);
    EXPECT_TRUE(res);

    std::vector<std::string> testAids = mTestNfcCEAgent_->GetAids(type, false);
    SetExpectAids(testAids, {}, false);
}

void TcNfcCeOffHostServiceData::UnRegisterOffHostServiceTestOk(bool isNormal)
{
    std::string type = isNormal ? TcNfcCeData::ABILITY_TYPE_NORMAL : TcNfcCeData::ABILITY_TYPE_SECURE;
    DebugLog("RemoveAidsLiteTestOk ability.%s type.%s", mCEAbilityInfo_->abilityName_.c_str(), type.c_str());

    bool res = mOffHceLite_->UnregisterOffHostService();
    EXPECT_TRUE(res);

    std::vector<std::string> testAids = mTestNfcCEAgent_->GetAids(type, false);
    SetExpectAids(testAids, {}, false);
}
std::shared_ptr<sdk::cardemulation::IOffHostCardEmulation> TcNfcCeOffHostServiceData::GetOffHceLite()
{
    return mOffHceLite_;
}
}  // namespace OHOS::nfc::test
