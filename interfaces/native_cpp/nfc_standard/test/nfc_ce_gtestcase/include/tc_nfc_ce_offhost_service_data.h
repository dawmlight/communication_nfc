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
#ifndef TC_NFC_CE_OFFHOST_SERVICE_DATA_H
#define TC_NFC_CE_OFFHOST_SERVICE_DATA_H
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "card_emulation_ability_info.h"
#include "tc_nfc_ce_data.h"

namespace OHOS {
namespace AppExecFwk {
class ElementName;
}
namespace nfc {
namespace sdk::cardemulation {
class CardEmulationServiceInfoLite;
class CardEmulation;
class IOffHostCardEmulation;
}  // namespace sdk::cardemulation

namespace test {
class TestNfcCEOffHostService;
class TestNfcCEAgent;

class TcNfcCeOffHostServiceData : public TcNfcCeData {
public:
    TcNfcCeOffHostServiceData(){};
    ~TcNfcCeOffHostServiceData(){};

    // off host cardemulation lite test
    void StartOffHceLiteTest(const std::string& abilityName, const std::string type);
    void StopOffHceLiteTest();
    void AddDynamicAidsLiteTest(bool isNormal = true,
                                std::vector<std::string> aids = {},
                                bool bAddinRoutingTable = true);
    void RemoveAidsLiteTestOk(bool isNormal = true);
    void RegisterOffHostServiceTest(bool isNormal = true);
    void UnRegisterOffHostServiceTestOk(bool isNormal = true);

    void SetPrimaryServiceTest();
    std::shared_ptr<sdk::cardemulation::IOffHostCardEmulation> GetOffHceLite();

private:
    std::shared_ptr<sdk::cardemulation::IOffHostCardEmulation> mOffHceLite_;
};
}  // namespace test
}  // namespace nfc
}  // namespace OHOS

#endif /*TC_NFC_CE_OFFHOST_SERVICE_DATA_H*/