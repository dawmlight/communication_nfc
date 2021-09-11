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
#ifndef TEST_NFC_CE_ABILITY_H
#define TEST_NFC_CE_ABILITY_H

#include <memory>
#include <string>
#include <vector>

#include "ability.h"
#include "card_emulation_ability_info.h"

namespace OHOS {
using AAFwk::Want;
using AppExecFwk::Ability;
using AppExecFwk::AbilityInfo;
using Application = AppExecFwk::OHOSApplication;

namespace nfc {
namespace test {
class TestNfcCEApp;

class TestNfcCEAbility : public Ability {
public:
    explicit TestNfcCEAbility();
    ~TestNfcCEAbility();

    static std::shared_ptr<TestNfcCEAbility> GenAbility(const std::shared_ptr<TestNfcCEApp>& application);
    static std::shared_ptr<TestNfcCEAbility> GenAbility(std::string& abilityName,
                                                        const std::shared_ptr<TestNfcCEApp>& application);
    static std::shared_ptr<TestNfcCEAbility> GenAbility(const std::shared_ptr<AbilityInfo>& abilityInfo,
                                                        const std::shared_ptr<TestNfcCEApp>& application);
    static std::shared_ptr<TestNfcCEAbility> GenAbility(const std::shared_ptr<TestNfcCEApp>& application,
                                                        std::shared_ptr<CardEmulationAbilityInfo> abilityInfo);

    void OnStart(const Want& want) override;
    void OnStop() override;

    void SetCEApp(std::shared_ptr<TestNfcCEApp>);
    std::vector<std::string> GetAbilityDynamicAidsByType(const std::string& type, int aidsIndex = 0);
    std::vector<std::string> GetAbilityStaticAidsByType(const std::string& type, int aidsIndex = 0);
    void SetAbilityInfo(std::shared_ptr<CardEmulationAbilityInfo> abilityInfo);
    std::shared_ptr<CardEmulationAbilityInfo> GetCEAbilityInfo();

private:
    std::shared_ptr<TestNfcCEApp> ceApp_;
    std::shared_ptr<CardEmulationAbilityInfo> abilityInfo_;
};
}  // namespace test
}  // namespace nfc
}  // namespace OHOS
#endif /* TEST_NFC_CE_ABILITY_H */
