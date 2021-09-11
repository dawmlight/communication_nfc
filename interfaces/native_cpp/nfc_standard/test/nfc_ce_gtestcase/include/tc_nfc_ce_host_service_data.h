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
#ifndef TC_NFC_CE_HOST_SERVICE_DATA_H
#define TC_NFC_CE_HOST_SERVICE_DATA_H
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
class IHostCardEmulation;
}  // namespace sdk::cardemulation

namespace test {
class TestNfcCEHostService;
class TestNfcCEAgent;

class TcNfcCeHostServiceData : public TcNfcCeData {
public:
    TcNfcCeHostServiceData(){};
    ~TcNfcCeHostServiceData(){};

    static std::vector<unsigned char> ApduSlecetAidProcess(std::vector<unsigned char> adpu);
    static void DeactiveEventCallbackProcess(int reason);

    // cardemulation test
    void StartHceTest(const std::string& abilityName, const std::string type);
    void StopHceTest();
    void AddDynamicAidsTest(bool isNormal, bool bAddinUnCommmit = true);
    void AddDynamicAidsTestWithAids(bool isNormal, std::vector<std::string> aids, bool bAddinUnCommmit = true);
    void RemoveAidsTestOk(bool isNormal = true);
    void SetPrimaryServiceTest();
    void SetPreferredServiceTest(bool isNormal = true, bool bAddinUnCommmit = true);
    void UnSetPreferredServiceTestOk(bool isNormal = true);

    // cardemulation lite test
    void StartHceLiteTest(const std::string& abilityName, const std::string type);
    void StopHceLiteTest();
    void LiteAddDynamicAidsTestOk(bool isNormal, std::vector<std::string> aids = {}, bool bAddinUnCommmit = true);
    void LiteRemoveDynamicAidsTestOk(bool isNormal);

    std::shared_ptr<sdk::cardemulation::IHostCardEmulation> GetHceLite() const;

private:
    std::shared_ptr<sdk::cardemulation::IHostCardEmulation> mHceLite_;
};
}  // namespace test
}  // namespace nfc
}  // namespace OHOS
#endif /* TC_NFC_CE_HOST_SERVICE_DATA_H */