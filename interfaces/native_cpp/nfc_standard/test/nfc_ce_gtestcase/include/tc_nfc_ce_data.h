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
#ifndef TC_NFC_CE_DATA_H
#define TC_NFC_CE_DATA_H
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "card_emulation_ability_info.h"

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

class TcNfcCeData {
public:
    // APDU command
    static const std::vector<unsigned char> T4T_NDEF_EMU_OK;
    static const std::vector<unsigned char> T4T_NDEF_EMU_NOK;

    static const std::vector<unsigned char> APDU_NDEF_APP_SELECT_HEAD;
    static const std::vector<unsigned char> APDU_NDEF_APP_SELECT_AID;
    static const std::vector<unsigned char> APDU_NDEF_CC;
    static const std::vector<unsigned char> APDU_NDEF_CC_SELECT;
    static const std::vector<unsigned char> APDU_NDEF_SELECT;
    static const std::vector<unsigned char> APDU_NDEF_READ;

    // ability type
    static const std::string ABILITY_TYPE_SECURE;
    static const std::string ABILITY_TYPE_NORMAL;
    static const std::string ABILITY_HOST;
    static const std::string ABILITY_OFFHOST;

    // ability name
    static const std::string HOST_SECURE_ABILITY_NAME_1;
    static const std::string HOST_SECURE_ABILITY_NAME_2;
    static const std::string HOST_NORMAL_ABILITY_NAME_1;
    static const std::string HOST_NORMAL_ABILITY_NAME_2;
    static const std::string HOST_NORMAL_ABILITY_NAME_N;
    static const std::string PRE_HOST_SECURE_ABILITY_NAME_1;
    static const std::string PRE_HOST_NORMAL_ABILITY_NAME_1;
    static const std::string SE_SECURE_ABILITY_NAME_1;
    static const std::string SE_SECURE_ABILITY_NAME_2;
    static const std::string SE_NORMAL_ABILITY_NAME_2;
    static const std::string SE_NORMAL_ABILITY_NAME_1;
    static const std::string PRE_SE_SECURE_ABILITY_NAME_1;
    static const std::string PRE_SE_NORMAL_ABILITY_NAME_1;
    static const std::string HOST_SN_ABILITY_NAME_1;
    static const std::string HOST_SN_ABILITY_NAME_N;
    static const std::string SE_SN_ABILITY_NAME_1;
    static const std::string INVALID_ENV_SE_SECURE_ABILITY_NAME;

    // test aids
    static const std::vector<std::string> CONFICT_AIDS_WITH_PHS_1;
    static const std::vector<std::string> CONFICT_PRE_AIDS_WITH_PHS_1;
    static const std::vector<std::string> CONFICT_AIDS;
    static const std::vector<std::string> CONFICT_AIDS_WITH_PRE;
    static const std::vector<std::string> CONFICT_AIDS_WITH_PRE_1;
    static const std::vector<std::string> CONFICT_AIDS_WITH_HS_1;
    static const std::vector<std::string> CONFICT_AIDS_WITH_PHN_1;
    static const std::vector<std::string> CONFICT_PRE_AIDS_WITH_PHN_2S;
    static const std::vector<std::string> CONFICT_PRE_AIDS_WITH_PHN_2L;
    static const std::vector<std::string> CONFICT_AIDS_WITH_HN_1;
    static const std::vector<std::string> CONFICT_PRE_AIDS_WITH_HN_1;
    static const std::vector<std::string> CONFICT_PRE_AIDS_WITH_HN_2;
    static const std::vector<std::string> CONFICT_PRE_AIDS_WITH_HS_1;
    static const std::vector<std::string> CONFICT_PRE_AIDS_WITH_HS_2;
    static const std::vector<std::string> IGNORE_AIDS_LETTERS;
    static const std::string AID_LEN_8;
    static const std::string AID_LEN_10;
    static const std::string AID_LEN_18;
    static const std::string AID_LEN_25;
    static const std::string AID_LEN_30;
    static const std::string AID_LEN_32;
    static const std::string AID_LEN_36;

public:
    TcNfcCeData(){};
    ~TcNfcCeData(){};

    void Init(const std::string& abilityName);

    std::vector<std::unique_ptr<sdk::cardemulation::CardEmulationServiceInfoLite>> GetServiceByType(
        const std::string& type, const std::string& executionEnvironment);
    std::shared_ptr<CardEmulationAbilityInfo> GetAbilityInfoFromConfig(const std::string& abilityName);
    bool GetElementName(const std::string& type, OHOS::AppExecFwk::ElementName& elementName);
    void SetExpectAids(std::vector<std::string> includeAids,
                       std::vector<std::string> excludeAids = {},
                       bool bHost = true);

    std::shared_ptr<TestNfcCEAgent> GetTestNfcCEAgent() const;
    std::shared_ptr<CardEmulationAbilityInfo> GetCEAbilityInfo() const;

    std::shared_ptr<TestNfcCEHostService> GetTestNfcCEHostService() const;
    std::shared_ptr<sdk::cardemulation::CardEmulation> GetHce() const;

private:
    std::shared_ptr<TestNfcCEAgent> mTestNfcCEAgent_;
    std::shared_ptr<CardEmulationAbilityInfo> mCEAbilityInfo_;
    std::shared_ptr<TestNfcCEHostService> mTestNfcCEHostService_;
    std::shared_ptr<sdk::cardemulation::CardEmulation> mHce_;

    std::vector<std::string> mApduAids_;
    static std::vector<std::vector<unsigned char>> mApduCommand_;
    std::shared_ptr<DefaultInfo> mDefaultInfo_;

    static bool mStartNextStepFlag_;

    friend class TcNfcCeHostServiceData;
    friend class TcNfcCeOffHostServiceData;
};
}  // namespace test
}  // namespace nfc
}  // namespace OHOS

#endif /*TC_NFC_CE_DATA_H*/