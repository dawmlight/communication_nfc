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
#include "test_nfc_ce_ability.h"

#include "ability_handler.h"
#include "ability_info.h"
#include "ability_loader.h"
#include "loghelper.h"
#include "nfc_agent.h"
#include "nfc_map.h"
#include "nfc_sdk_common.h"
#include "tag.h"
#include "test_nfc_ce_app.h"
#include "want.h"

using namespace OHOS::nfc::sdk;
using namespace std;
namespace OHOS {
using AppExecFwk::AbilityHandler;
using AppExecFwk::AbilityInfo;
namespace nfc {
namespace test {
TestNfcCEAbility::TestNfcCEAbility() {}

TestNfcCEAbility::~TestNfcCEAbility() {}

std::shared_ptr<TestNfcCEAbility> TestNfcCEAbility::GenAbility(const std::shared_ptr<TestNfcCEApp>& application)
{
    std::string abilityName("test.nfc.ability");
    return GenAbility(abilityName, application);
}

std::shared_ptr<TestNfcCEAbility> TestNfcCEAbility::GenAbility(std::string& abilityName,
                                                               const std::shared_ptr<TestNfcCEApp>& application)
{
    std::shared_ptr<AbilityInfo> abilityInfo = std::make_shared<AbilityInfo>();
    abilityInfo->name = abilityName;
    return GenAbility(abilityInfo, application);
}

std::shared_ptr<TestNfcCEAbility> TestNfcCEAbility::GenAbility(const std::shared_ptr<AbilityInfo>& abilityInfo,
                                                               const std::shared_ptr<TestNfcCEApp>& application)
{
    std::shared_ptr<TestNfcCEAbility> ability = std::make_shared<TestNfcCEAbility>();
    std::shared_ptr<AbilityInfo> info = abilityInfo;
    if (!info) {
        info = std::make_shared<AbilityInfo>();
        info->name = "test.nfc.ability";
    }

    std::shared_ptr<AppExecFwk::AbilityHandler> hander{};
    sptr<IRemoteObject> token{};
    ability->Init(info, application, hander, token);
    ability->SetCEApp(application);
    return ability;
}
std::shared_ptr<TestNfcCEAbility> TestNfcCEAbility::GenAbility(const std::shared_ptr<TestNfcCEApp>& application,
                                                               std::shared_ptr<CardEmulationAbilityInfo> abilityInfo)
{
    std::shared_ptr<TestNfcCEAbility> ability = std::make_shared<TestNfcCEAbility>();
    std::shared_ptr<AbilityInfo> info = std::make_shared<AbilityInfo>();
    info->name = abilityInfo->abilityName_;
    info->iconPath = abilityInfo->icon_;
    info->kind = abilityInfo->abilityType_;

    std::shared_ptr<AppExecFwk::AbilityHandler> hander{};
    sptr<IRemoteObject> token{};
    ability->Init(info, application, hander, token);
    ability->SetCEApp(application);
    return ability;
}
void TestNfcCEAbility::OnStart(const Want& want)
{
    DebugLog("TestNfcCEAbility::OnStart");
}

void TestNfcCEAbility::OnStop()
{
    // mExtras_ = std::shared_ptr<NfcMap>();
    Ability::OnStop();
}

void TestNfcCEAbility::SetCEApp(std::shared_ptr<TestNfcCEApp> ceApp)
{
    ceApp_ = ceApp;
}

std::vector<std::string> TestNfcCEAbility::GetAbilityDynamicAidsByType(const std::string& type, int aidsIndex)
{
    std::vector<std::shared_ptr<AidSetData>> dynamicAidSets = abilityInfo_->dynamicAidsets_;
    DebugLog(
        "TestNfcCEAbility::GetAbilityDynamicAidsByType in aids num.%d aidsIndex.%d", dynamicAidSets.size(), aidsIndex);
    if (aidsIndex > 0) {
        if ((dynamicAidSets.size() - aidsIndex >= 0) && (dynamicAidSets.at(aidsIndex - 1)->type_ == type)) {
            return dynamicAidSets.at(aidsIndex - 1)->aids_;
        }
    } else {
        for (auto i : dynamicAidSets) {
            if (i->type_ == type) {
                return i->aids_;
            }
        }
    }

    return std::vector<std::string>();
}
std::vector<std::string> TestNfcCEAbility::GetAbilityStaticAidsByType(const std::string& type, int aidsIndex)
{
    std::vector<std::shared_ptr<AidSetData>> aidSets = abilityInfo_->aidsets_;
    DebugLog("TestNfcCEAbility::GetAbilityStaticAidsByType in aids num.%d aidsIndex.%d", aidSets.size(), aidsIndex);

    if (aidsIndex > 0) {
        if ((aidSets.size() - aidsIndex >= 0) && (aidSets.at(aidsIndex - 1)->type_ == type)) {
            return aidSets.at(aidsIndex - 1)->aids_;
        }
    } else {
        for (auto i : aidSets) {
            if (i->type_ == type) {
                return i->aids_;
            }
        }
    }

    return std::vector<std::string>();
}

void TestNfcCEAbility::SetAbilityInfo(std::shared_ptr<CardEmulationAbilityInfo> abilityInfo)
{
    abilityInfo_ = abilityInfo;
}

std::shared_ptr<CardEmulationAbilityInfo> TestNfcCEAbility::GetCEAbilityInfo()
{
    return abilityInfo_;
}
}  // namespace test
}  // namespace nfc
using nfc::test::TestNfcCEAbility;
namespace AppExecFwk {
REGISTER_AA(TestNfcCEAbility)
}
}  // namespace OHOS