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
#include "tc_nfc_ce_host_service_data.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <thread>

#include "card_emulation.h"
#include "card_emulation_service_info_lite.h"
#include "card_emulation_util.h"
#include "element_name.h"
#include "ihost_card_emulation.h"
#include "iservice_registry.h"
#include "loghelper.h"
#include "nfc_agent.h"
#include "routing_dump_decoder.hpp"
#include "stdlib.h"
#include "string_ex.h"
#include "test_nfc_ce_ability.h"
#include "test_nfc_ce_agent.h"
#include "test_nfc_ce_host_service.h"
#include "want.h"

using namespace std;
using namespace OHOS::AAFwk;
using namespace OHOS::AppExecFwk;
using namespace OHOS::nfc::sdk::cardemulation;
namespace OHOS::nfc::test {
void TcNfcCeHostServiceData::StartHceTest(const std::string& abilityName, const std::string type)
{
    Init(abilityName);

    int userid = 0;
    ElementName serviceName;
    bool res = GetElementName(type, serviceName);
    EXPECT_TRUE(res);
    Want want;
    OHOS::sptr<IRemoteObject> obj = mTestNfcCEHostService_->OnConnect(want);
    if (!obj) {
        DebugLog("AddAidTestOk obj err");
    }
    res = mHce_->RegisterRemoteObject(userid, serviceName, obj);
    EXPECT_TRUE(res);

    mTestNfcCEHostService_->SetApduExpection(TcNfcCeHostServiceData::ApduSlecetAidProcess);
}

void TcNfcCeHostServiceData::StopHceTest()
{
    mTestNfcCEAgent_->TerminalAbility();
    mHce_ = std::shared_ptr<CardEmulation>();
}

void TcNfcCeHostServiceData::StartHceLiteTest(const std::string& abilityName, const std::string type)
{
    Init(abilityName);
    ElementName serviceName;
    GetElementName(type, serviceName);
    mHceLite_ = mTestNfcCEAgent_->GetNfcAgent()->CreateHostCardEmulation(serviceName);
    mHceLite_->SetApduCommandCallback(TcNfcCeHostServiceData::ApduSlecetAidProcess);
    mHceLite_->SetDeactiveEventCallback(TcNfcCeHostServiceData::DeactiveEventCallbackProcess);
}

void TcNfcCeHostServiceData::StopHceLiteTest()
{
    mTestNfcCEAgent_->TerminalAbility();
    mHceLite_ = std::shared_ptr<IHostCardEmulation>();
}

std::vector<unsigned char> TcNfcCeHostServiceData::ApduSlecetAidProcess(std::vector<unsigned char> adpu)
{
    DebugLog("TcNfcCeHostServiceData::ApduSlecetAidProcess in aid num.%d recieve apdu command.%d(%s) ",
             mApduCommand_.size(),
             adpu.size(),
             cardemulation::BytesToHexStr(adpu, true).c_str());

    if (adpu.size() == 0) {
        return T4T_NDEF_EMU_NOK;
    }

    mStartNextStepFlag_ = true;
    DebugLog(
        "TcNfcCeHostServiceData::ApduSlecetAidProcess recieve apdu commmand. start next step mStartNextCastFlag_.%d!",
        mStartNextStepFlag_);

    bool ok = false;
    for (auto i : mApduCommand_) {
        DebugLog("aid.%s", cardemulation::BytesToHexStr(i, true).c_str());
        ok = TestNfcCEHostService::IsApduBeginWith(adpu, i);
        if (ok) {
            DebugLog("TcNfcCeHostServiceData::ApduSlecetAidProcess return ok!!");
            return T4T_NDEF_EMU_OK;
        }
    }

    return T4T_NDEF_EMU_NOK;
}

void TcNfcCeHostServiceData::DeactiveEventCallbackProcess(int reason)
{
    DebugLog("TcNfcCeHostServiceData::DeactiveEventCallbackProcess deactive reason.%d", reason);
}

void TcNfcCeHostServiceData::AddDynamicAidsTest(bool isNormal, bool bAddinRoutingTable)
{
    DebugLog("TcNfcCeHostServiceData::AddDynamicAidsTestOk in");
    // // device nama bundlename should be null
    std::string type = isNormal ? TcNfcCeData::ABILITY_TYPE_NORMAL : TcNfcCeData::ABILITY_TYPE_SECURE;
    ElementName serviceName;
    bool res = GetElementName(type, serviceName);
    EXPECT_TRUE(res);

    EXPECT_TRUE(mTestNfcCEAgent_);
    std::vector<std::string> testAids = mTestNfcCEAgent_->GetAids(type, true);

    DebugLog("AddAidTestOk AddAidsForService type.%s", type.c_str());
    for (size_t i = 0; i < testAids.size(); i++) {
        printf("aid.%d(%s)\n", i, testAids.at(i).c_str());
    }

    res = mHce_->AddAidsForService(serviceName, testAids, type);
    EXPECT_TRUE(res);

    std::vector<std::string> excludeAids, includeAids;
    if (bAddinRoutingTable) {
        includeAids = testAids;
        excludeAids = mTestNfcCEAgent_->GetAids(type, false);
    } else {
        includeAids = mTestNfcCEAgent_->GetAids(type, false);
        excludeAids = testAids;
    }
    SetExpectAids(testAids, excludeAids);

    return;
}

void TcNfcCeHostServiceData::AddDynamicAidsTestWithAids(bool isNormal,
                                                        std::vector<std::string> testAids,
                                                        bool bAddinUnCommmit)
{
    DebugLog("TcNfcCeHostServiceData::AddDynamicAidsTestWithAids in aids.%d", testAids.size());
    EXPECT_TRUE(testAids.size() > 0);

    // // device nama bundlename should be null
    std::string type = isNormal ? TcNfcCeData::ABILITY_TYPE_NORMAL : TcNfcCeData::ABILITY_TYPE_SECURE;
    ElementName serviceName;
    bool res = GetElementName(type, serviceName);
    EXPECT_TRUE(res);

    std::vector<std::string> aidsBefore = mHce_->GetAidsForService(serviceName, type);
    EXPECT_TRUE((aidsBefore.size() != 0));
    DebugLog("AddDynamicAidsTestWithAids AddAidsForService type.%s", type.c_str());
    for (size_t i = 0; i < testAids.size(); i++) {
        printf("aid.%d(%s)\n", i, testAids.at(i).c_str());
    }

    res = mHce_->AddAidsForService(serviceName, testAids, type);
    EXPECT_TRUE(res);

    std::vector<std::string> excludeAids, includeAids;
    EXPECT_TRUE(mTestNfcCEAgent_);
    if (bAddinUnCommmit) {
        includeAids = testAids;
        excludeAids = mTestNfcCEAgent_->GetAids(type, false);
    } else {
        includeAids = {};
        excludeAids = testAids;
    }
    SetExpectAids(includeAids, excludeAids);

    return;
}

void TcNfcCeHostServiceData::RemoveAidsTestOk(bool isNormal)
{
    // device nama bundlename should be null
    std::string type = isNormal ? TcNfcCeData::ABILITY_TYPE_NORMAL : TcNfcCeData::ABILITY_TYPE_SECURE;
    ElementName serviceName;
    GetElementName(type, serviceName);

    bool res = mHce_->RemoveAidsForService(serviceName, type);
    EXPECT_TRUE(res);
    std::vector<std::string> aids = mHce_->GetAidsForService(serviceName, type);
    DebugLog("[RemoveAidsTestOk] GetAidsForService aid num.%d!", aids.size());
    EXPECT_TRUE((aids.size() > 0));

    std::vector<std::string> testAids = mTestNfcCEAgent_->GetAids(type, false);
    std::vector<std::string> excludeAids = mTestNfcCEAgent_->GetAids(type, true);
    SetExpectAids(testAids, excludeAids);
}

void TcNfcCeHostServiceData::SetPrimaryServiceTest()
{
    DebugLog("SetPrimaryServiceTest in");
    OHOS::AppExecFwk::ElementName serviceName;
    bool res = GetElementName(TcNfcCeData::ABILITY_TYPE_SECURE, serviceName);
    EXPECT_TRUE(res);

    res = mHce_->SetPrimaryServiceForType(serviceName, mCEAbilityInfo_->aidsets_.at(0)->type_);
    EXPECT_TRUE(res);

    std::string sname = mHce_->GetPrimaryServiceForType(mCEAbilityInfo_->aidsets_.at(0)->type_);
    res = (sname == serviceName.GetAbilityName()) ? true : false;
    EXPECT_TRUE(res);
    DebugLog("PrimaryService name.%s expect name.%s", sname.c_str(), serviceName.GetAbilityName().c_str());

    std::vector<std::string> testAids = mTestNfcCEAgent_->GetAids(TcNfcCeData::ABILITY_TYPE_SECURE, false);
    std::vector<std::string> excludeAids = mTestNfcCEAgent_->GetAids(TcNfcCeData::ABILITY_TYPE_SECURE, true);
    SetExpectAids(testAids, excludeAids);
}

void TcNfcCeHostServiceData::SetPreferredServiceTest(bool isNormal, bool bAddinUnCommmit)
{
    DebugLog("TcNfcCeHostServiceData::SetPreferredServiceTestOk in");
    // // device nama bundlename should be null
    std::string type = isNormal ? TcNfcCeData::ABILITY_TYPE_NORMAL : TcNfcCeData::ABILITY_TYPE_SECURE;
    ElementName serviceName;
    EXPECT_TRUE(mCEAbilityInfo_);
    bool res = GetElementName(type, serviceName);
    EXPECT_TRUE(res);

    res = mHce_->SetPreferredForegroundService(serviceName);
    EXPECT_TRUE(res);

    if (bAddinUnCommmit) {
        std::vector<std::string> testAids = mHce_->GetAidsForService(serviceName, type);
        SetExpectAids(testAids);
    }
    return;
}
void TcNfcCeHostServiceData::UnSetPreferredServiceTestOk(bool isNormal)
{
    DebugLog("TcNfcCeHostServiceData::UnSetPreferredServiceTestOk in");
    std::string type = isNormal ? TcNfcCeData::ABILITY_TYPE_NORMAL : TcNfcCeData::ABILITY_TYPE_SECURE;
    bool res = mHce_->UnsetPreferredForegroundService();
    EXPECT_TRUE(res);

    ElementName serviceName;
    res = GetElementName(type, serviceName);
    std::vector<std::string> testAids = mHce_->GetAidsForService(serviceName, type);
    SetExpectAids(testAids);
}

std::shared_ptr<sdk::cardemulation::IHostCardEmulation> TcNfcCeHostServiceData::GetHceLite() const
{
    return mHceLite_;
}

void TcNfcCeHostServiceData::LiteAddDynamicAidsTestOk(bool isNormal,
                                                      std::vector<std::string> aids,
                                                      bool bAddinUnCommmit)
{
    DebugLog("TcNfcCeHostServiceData::LiteAddDynamicAidsTestOk in ability.%s", mCEAbilityInfo_->abilityName_.c_str());
    // // device nama bundlename should be null
    std::string type = isNormal ? TcNfcCeData::ABILITY_TYPE_NORMAL : TcNfcCeData::ABILITY_TYPE_SECURE;

    EXPECT_TRUE(mTestNfcCEAgent_);
    std::vector<std::string> testAids = (aids.size() == 0) ? mTestNfcCEAgent_->GetAids(type, true) : std::move(aids);
    EXPECT_TRUE(testAids.size() > 0);

    DebugLog("LiteAddDynamicAidsTestOk AddAidsForService type.%s", type.c_str());
    for (size_t i = 0; i < testAids.size(); i++) {
        printf("aid.%d(%s)\n", i, testAids.at(i).c_str());
    }

    bool res = mHceLite_->AddAidsForService(testAids, type);
    EXPECT_TRUE(res);

    if (!bAddinUnCommmit) {
    } else {
        SetExpectAids({}, testAids);
        SetExpectAids(testAids);
    }
}

void TcNfcCeHostServiceData::LiteRemoveDynamicAidsTestOk(bool isNormal)
{
    DebugLog("TcNfcCeHostServiceData::LiteRemoveDynamicAidsTestOk in ability.%s",
             mCEAbilityInfo_->abilityName_.c_str());
    std::string type = isNormal ? TcNfcCeData::ABILITY_TYPE_NORMAL : TcNfcCeData::ABILITY_TYPE_SECURE;

    bool res = mHceLite_->RemoveAidsForService(type);
    EXPECT_TRUE(res);
    std::vector<std::string> testAids = mTestNfcCEAgent_->GetAids(type, false);
    std::vector<std::string> excludeAids = mTestNfcCEAgent_->GetAids(type, true);
    SetExpectAids(testAids, excludeAids);
}
}  // namespace OHOS::nfc::test
