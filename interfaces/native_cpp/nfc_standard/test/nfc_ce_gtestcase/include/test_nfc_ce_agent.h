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
#ifndef TEST_NFC_CE_AGENT_H
#define TEST_NFC_CE_AGENT_H

#include <memory>
#include <string>

#include "card_emulation_ability_info.h"

namespace OHOS {
namespace nfc {
namespace sdk {
class NfcMap;
class NfcAgent;
namespace cardemulation {
class IHostCardEmulation;
class IOffHostCardEmulation;
}  // namespace cardemulation
}  // namespace sdk
namespace test {
class TestNfcCEAbility;
class TestNfcCEApp;
class TestNfcCEAgent {
public:
    TestNfcCEAgent() {}
    ~TestNfcCEAgent() {}
    // Initailize Nfc Reader
    bool Initailize(std::shared_ptr<CardEmulationAbilityInfo> abilityInfo);
    // De initialization
    void DeInitailize();
    // Turn On Nfc
    void TurnOnNfc();
    // Turn Off Nfc
    void TurnOffNfc();
    // Start up
    void StartAbility();
    void TerminalAbility();
    void ActiveAbility();
    void InactiveAbility();

    std::shared_ptr<sdk::NfcAgent> GetNfcAgent();
    std::vector<std::string> GetAids(const std::string& type, bool isDynamicAid = true, int aidsIndex = 0);
    std::shared_ptr<TestNfcCEAbility> GetAbility();

private:
    std::shared_ptr<TestNfcCEApp> application_{};
    std::shared_ptr<TestNfcCEAbility> ability_{};
    std::shared_ptr<sdk::cardemulation::IHostCardEmulation> hce_{};
    std::shared_ptr<sdk::cardemulation::IOffHostCardEmulation> offHce_{};
};
}  // namespace test
}  // namespace nfc
}  // namespace OHOS
#endif /* TEST_NFC_CE_AGENT_H */