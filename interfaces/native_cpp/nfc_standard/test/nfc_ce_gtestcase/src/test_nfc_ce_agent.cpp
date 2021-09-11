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
#include "test_nfc_ce_agent.h"

#include <thread>

#include "ability_local_record.h"
#include "card_emulation_ability_info.h"
#include "loghelper.h"
#include "nfc_agent.h"
#include "nfc_map.h"
#include "test_nfc_ce_ability.h"
#include "test_nfc_ce_app.h"
#include "want.h"

using namespace OHOS::nfc::sdk;
using OHOS::AAFwk::Want;

namespace OHOS {
using AppExecFwk::AbilityLocalRecord;
namespace nfc {
namespace test {
// Initailize Nfc card emulation
bool TestNfcCEAgent::Initailize(std::shared_ptr<CardEmulationAbilityInfo> abilityInfo)
{
    std::string appName("test.nfc.reader.app");
    application_ = TestNfcCEApp::GenAppliaction(appName);

    ability_ = TestNfcCEAbility::GenAbility(application_, abilityInfo);
    ability_->SetAbilityInfo(abilityInfo);
    // start ability
    application_->OnAbilityStart(ability_);

    InfoLog("Get the NFC Agent Instance");
    std::shared_ptr<NfcAgent> nfcAgent = NfcAgent::GetNFCAgent(application_);
    if (!nfcAgent) {
        InfoLog("The NFC Agent Instance Is Empty.");
    } else {
        application_->SetNfcAgent(nfcAgent);
    }
    return true;
}

void TestNfcCEAgent::DeInitailize()
{
    application_ = std::shared_ptr<TestNfcCEApp>();
    ability_ = std::shared_ptr<TestNfcCEAbility>();
}

// Turn On Nfc
void TestNfcCEAgent::TurnOnNfc()
{
    application_->TurnOnNfc();
}

// Turn Off Nfc
void TestNfcCEAgent::TurnOffNfc()
{
    application_->TurnOffNfc();
}

// Start up
void TestNfcCEAgent::StartAbility()
{
    Want want;
    if (ability_) {
        ability_->OnStart(want);
    } else {
        InfoLog("ability_ is empty.");
    }
}

void TestNfcCEAgent::TerminalAbility()
{
    ability_->OnStop();
}

void TestNfcCEAgent::ActiveAbility()
{
    ability_->OnActive();
    application_->OnAbilityActive(ability_);
}

void TestNfcCEAgent::InactiveAbility()
{
    ability_->OnInactive();
    application_->OnAbilityInactive(ability_);
}

std::shared_ptr<NfcAgent> TestNfcCEAgent::GetNfcAgent()
{
    return application_->GetNfcAgent();
}

std::vector<std::string> TestNfcCEAgent::GetAids(const std::string& type, bool isDynamicAid, int aidsIndex)
{
    std::vector<std::string> aids;
    if (!isDynamicAid) {
        aids = ability_->GetAbilityStaticAidsByType(type, aidsIndex);
    } else {
        aids = ability_->GetAbilityDynamicAidsByType(type, aidsIndex);
    }
    return aids;
}

std::shared_ptr<TestNfcCEAbility> TestNfcCEAgent::GetAbility()
{
    return ability_;
}
}  // namespace test
}  // namespace nfc
}  // namespace OHOS