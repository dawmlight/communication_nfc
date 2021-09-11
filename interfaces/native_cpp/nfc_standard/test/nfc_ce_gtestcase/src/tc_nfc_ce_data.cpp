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
#include "tc_nfc_ce_data.h"

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
#include "routing_table_common_event.hpp"
#include "stdlib.h"
#include "string_ex.h"
#include "tc_nfc_ce_host_service_data.h"
#include "test_nfc_ce_ability.h"
#include "test_nfc_ce_agent.h"
#include "test_nfc_ce_host_service.h"
#include "want.h"

using namespace std;
using namespace OHOS::AAFwk;
using namespace OHOS::AppExecFwk;
using namespace OHOS::nfc::sdk::cardemulation;
namespace OHOS::nfc::test {
static std::shared_ptr<RoutingChangedSubscriber> subscriber;

std::vector<std::vector<unsigned char>> TcNfcCeData::mApduCommand_;
bool TcNfcCeData::mStartNextStepFlag_ = false;

// APDU command
const std::vector<unsigned char> TcNfcCeData::T4T_NDEF_EMU_OK{0x90, 0x00};
const std::vector<unsigned char> TcNfcCeData::T4T_NDEF_EMU_NOK{0x6A, 0x82};

const std::vector<unsigned char> TcNfcCeData::APDU_NDEF_APP_SELECT_HEAD{0x00, 0xA4, 0x04, 0x00};
const std::vector<unsigned char> TcNfcCeData::APDU_NDEF_APP_SELECT_AID{
    0x00, 0xA4, 0x04, 0x00, 0x07, 0xD2, 0x76, 0x00, 0x00, 0x85, 0x01, 0x01};
const std::vector<unsigned char> TcNfcCeData::APDU_NDEF_CC{
    0x00, 0x0F, 0x20, 0x00, 0xFF, 0x00, 0xFF, 0x04, 0x06, 0xE1, 0x04, 0x00, 0xFF, 0x00, 0xFF};
const std::vector<unsigned char> TcNfcCeData::APDU_NDEF_CC_SELECT{0x00, 0xA4, 0x00, 0x0C, 0x02, 0xE1, 0x03};
const std::vector<unsigned char> TcNfcCeData::APDU_NDEF_SELECT{0x00, 0xA4, 0x00, 0x0C, 0x02, 0xE1, 0x04};
const std::vector<unsigned char> TcNfcCeData::APDU_NDEF_READ{0x00, 0xB0};

// ability type
const std::string TcNfcCeData::ABILITY_TYPE_SECURE = "secure";
const std::string TcNfcCeData::ABILITY_TYPE_NORMAL = "normal";
const std::string TcNfcCeData::ABILITY_HOST = "host_card_emulation";
const std::string TcNfcCeData::ABILITY_OFFHOST = "offhost_card_emulation";

// ability name
const std::string TcNfcCeData::HOST_NORMAL_ABILITY_NAME_1 = "ability_hn_1";
const std::string TcNfcCeData::HOST_SECURE_ABILITY_NAME_1 = "ability_hs_1";
const std::string TcNfcCeData::HOST_NORMAL_ABILITY_NAME_2 = "ability_hn_2";
const std::string TcNfcCeData::HOST_NORMAL_ABILITY_NAME_N = "ability_hn_n";
const std::string TcNfcCeData::HOST_SECURE_ABILITY_NAME_2 = "ability_hs_2";
const std::string TcNfcCeData::SE_SECURE_ABILITY_NAME_1 = "ability_ess_1";
const std::string TcNfcCeData::SE_NORMAL_ABILITY_NAME_1 = "ability_esn_1";
const std::string TcNfcCeData::SE_SECURE_ABILITY_NAME_2 = "ability_ess_2";
const std::string TcNfcCeData::SE_NORMAL_ABILITY_NAME_2 = "ability_esn_2";
const std::string TcNfcCeData::HOST_SN_ABILITY_NAME_1 = "ability_hsn_1";
const std::string TcNfcCeData::HOST_SN_ABILITY_NAME_N = "ability_hsn_n";
const std::string TcNfcCeData::SE_SN_ABILITY_NAME_1 = "ability_essn_1";

const std::string TcNfcCeData::PRE_HOST_NORMAL_ABILITY_NAME_1 = "ability_phn_1";
const std::string TcNfcCeData::PRE_HOST_SECURE_ABILITY_NAME_1 = "ability_phs_1";
const std::string TcNfcCeData::PRE_SE_SECURE_ABILITY_NAME_1 = "ability_pess_1";
const std::string TcNfcCeData::PRE_SE_NORMAL_ABILITY_NAME_1 = "ability_pesn_1";

// test aids
const std::vector<std::string> TcNfcCeData::CONFICT_AIDS_WITH_PHS_1 = {"E04E31314050607099", "F04F31314050607099"};
const std::vector<std::string> TcNfcCeData::CONFICT_PRE_AIDS_WITH_PHS_1 = {"E04E3131405060*", "F04F31314050*"};
const std::vector<std::string> TcNfcCeData::CONFICT_AIDS = {"D01D202040506070", "D01D2020405060"};
const std::vector<std::string> TcNfcCeData::CONFICT_AIDS_WITH_PRE = {"D01D20204050*", "D01D2020405060"};
const std::vector<std::string> TcNfcCeData::CONFICT_AIDS_WITH_PRE_1 = {"D01D2020405060", "D01D20204050*"};
const std::vector<std::string> TcNfcCeData::CONFICT_AIDS_WITH_HS_1 = {"E02E303040506070", "F02F30304050607099"};
const std::vector<std::string> TcNfcCeData::CONFICT_AIDS_WITH_PHN_1 = {"E06E31314050607099", "F06F31314050607099"};
const std::vector<std::string> TcNfcCeData::CONFICT_PRE_AIDS_WITH_PHN_2S = {"E06E31314050*", "F06F31314050*"};
const std::vector<std::string> TcNfcCeData::CONFICT_PRE_AIDS_WITH_PHN_2L = {"E06E313140506099*", "F06F31314050607099*"};
const std::vector<std::string> TcNfcCeData::CONFICT_PRE_AIDS_WITH_HN_1 = {"E01E1010405060*", "F01F10104050*"};
const std::vector<std::string> TcNfcCeData::CONFICT_AIDS_WITH_HN_1 = {"E01E10104050607080", "F01F10104050"};
const std::vector<std::string> TcNfcCeData::CONFICT_PRE_AIDS_WITH_HN_2 = {"E07E1010405060*", "F07F10104050*"};
const std::vector<std::string> TcNfcCeData::CONFICT_PRE_AIDS_WITH_HS_1 = {"E02E3030405060*", "F02F30304050*"};
const std::vector<std::string> TcNfcCeData::CONFICT_PRE_AIDS_WITH_HS_2 = {"E03E3131405060*", "F03F31314050*"};
const std::vector<std::string> TcNfcCeData::IGNORE_AIDS_LETTERS = {"E01E1A1B4C5D6F", "e01e1a1b4c5d6f"};

const std::string TcNfcCeData::AID_LEN_8 = "08080808";
const std::string TcNfcCeData::AID_LEN_10 = "10101010AA";
const std::string TcNfcCeData::AID_LEN_18 = "18181818AA12345678";
const std::string TcNfcCeData::AID_LEN_30 = "30303030AA30303030AA30303030AA";
const std::string TcNfcCeData::AID_LEN_32 = "32323232AA32323232AA32323232AA12";
const std::string TcNfcCeData::AID_LEN_36 = "36363636AA36363636AA36363636AA123456";
const std::string TcNfcCeData::AID_LEN_25 = "25252525AA25252525AA12345";

void TcNfcCeData::Init(const std::string& abilityName)
{
    DebugLog("TcNfcCeData::Init in");
    if (!subscriber) {
        subscriber = OHOS::nfc::test::RoutingChangedSubscriber::SubscribeRoutingChanged();
    }

    mTestNfcCEAgent_ = std::make_shared<TestNfcCEAgent>();

    std::shared_ptr<CardEmulationAbilityInfo> abilityInfo = GetAbilityInfoFromConfig(abilityName);
    EXPECT_TRUE(abilityInfo);

    mTestNfcCEAgent_->Initailize(abilityInfo);
    mTestNfcCEAgent_->TurnOnNfc();
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));

    mTestNfcCEAgent_->StartAbility();
    mTestNfcCEAgent_->ActiveAbility();

    mCEAbilityInfo_ = mTestNfcCEAgent_->GetAbility()->GetCEAbilityInfo();

    DebugLog("Init ability name.%s type.%s",
             mCEAbilityInfo_->abilityName_.c_str(),
             mCEAbilityInfo_->dynamicAidsets_.at(0)->type_.c_str());

    mTestNfcCEHostService_ = std::make_shared<TestNfcCEHostService>();
    mHce_ = CardEmulation::GetInstance(mTestNfcCEAgent_->GetNfcAgent());
    EXPECT_TRUE(mHce_);
}

bool TcNfcCeData::GetElementName(const std::string& type, OHOS::AppExecFwk::ElementName& elementName)
{
    std::vector<std::unique_ptr<sdk::cardemulation::CardEmulationServiceInfoLite>> serviceInfo =
        GetServiceByType(type, mCEAbilityInfo_->env_);
    EXPECT_TRUE(serviceInfo.size() != 0);
    DebugLog("TcNfcCeData::GetElementName ability name.%s type.%s env.%s, info.%d",
             mCEAbilityInfo_->abilityName_.c_str(),
             type.c_str(),
             mCEAbilityInfo_->env_.c_str(),
             serviceInfo.size());
    for (size_t i = 0; i < serviceInfo.size(); i++) {
        DebugLog("serviceInfo.%d ability name.%s ", i, serviceInfo.at(i)->GetName().GetAbilityName().c_str());
        if (serviceInfo.at(i)->GetName().GetAbilityName() == mCEAbilityInfo_->abilityName_) {
            elementName.SetDeviceID(serviceInfo.at(i)->GetName().GetDeviceID());
            elementName.SetBundleName(serviceInfo.at(i)->GetName().GetBundleName());
            elementName.SetAbilityName(serviceInfo.at(i)->GetName().GetAbilityName());

            DebugLog("[TcNfcCeData::GetElementName] device.%s bundlename.%s abilityname.%s",
                     elementName.GetDeviceID().c_str(),
                     elementName.GetBundleName().c_str(),
                     elementName.GetAbilityName().c_str());
            return true;
        }
    }
    return false;
}

std::shared_ptr<CardEmulationAbilityInfo> TcNfcCeData::GetAbilityInfoFromConfig(const std::string& abilityName)
{
    int userid = 0;
    std::vector<std::shared_ptr<CardEmulationAbilityInfo>> abilityInfoList = LoadAllAbilityInfosByUserId(userid);
    DebugLog("TcNfcCeData::GetAbilityInfoFromConfig LoadAllAbilityInfosByUserId abilityInfoList.%d",
             abilityInfoList.size());

    mDefaultInfo_ = LoadDefaultInfosByUserId(userid);
    DebugLog("TcNfcCeData::GetAbilityInfoFromConfig LoadDefaultInfosByUserId ability name.%s testflag.%d",
             mDefaultInfo_->abilityName_.c_str(),
             mDefaultInfo_->testWithRoutingTable_);

    for (size_t i = 0; i < abilityInfoList.size(); i++) {
        if (abilityInfoList.at(i)->abilityName_ == abilityName) {
            return abilityInfoList.at(i);
        }
    }
    return std::shared_ptr<CardEmulationAbilityInfo>();
}

std::vector<std::unique_ptr<sdk::cardemulation::CardEmulationServiceInfoLite>> TcNfcCeData::GetServiceByType(
    const std::string& type, const std::string& executionEnvironment)
{
    std::vector<std::unique_ptr<CardEmulationServiceInfoLite>> serviceInfo = mHce_->GetServices(type);
    std::vector<std::unique_ptr<CardEmulationServiceInfoLite>> res;
    for (size_t i = 0; i < serviceInfo.size(); i++) {
        DebugLog("ability name.%s env.%s",
                 serviceInfo.at(i)->GetName().GetAbilityName().c_str(),
                 serviceInfo.at(i)->GetExecutionEnvironment().c_str());
        if (serviceInfo.at(i)->GetExecutionEnvironment() == executionEnvironment) {
            res.push_back(std::move(serviceInfo.at(i)));
        }
    }

    return res;
}

void TcNfcCeData::SetExpectAids(std::vector<std::string> testAids, std::vector<std::string> excludeAids, bool bHost)
{
    DebugLog("TcNfcCeData::Set Expect include Aids in aidlen.%d except len.%d testWithRouting.%d",
             testAids.size(),
             excludeAids.size(),
             mDefaultInfo_->testWithRoutingTable_);

    if ((excludeAids.size() == 0) && (testAids.size() == 0)) {
        DebugLog("TcNfcCeData::SetExpectAids no expect aids");
        return;
    }

    int rv;
    if (excludeAids.size() == 0) {
        rv = subscriber->ExpectInclude(testAids);
    } else if (testAids.size() == 0) {
        rv = subscriber->ExpectExclude(excludeAids);
    } else {
        rv = subscriber->Expect(testAids, excludeAids);
    }

    if (mDefaultInfo_->testWithRoutingTable_) {
        EXPECT_EQ(rv, RoutingChangedSubscriber::RESULT_OK);
        return;
    }

    // test with select aid command with POS
    mApduAids_.clear();
    mApduCommand_.clear();
    mApduAids_ = std::move(testAids);

    std::vector<unsigned char> apduPrefix = TcNfcCeData::APDU_NDEF_APP_SELECT_HEAD;
    int headLen = apduPrefix.size();
    int len_size = 2;
    std::string apduCommandString;
    std::string apduCommandHead;
    for (auto i : mApduAids_) {
        DebugLog("aid.%s", i.c_str());
        if (i.back() == '*' || i.back() == '#') {
            i.pop_back();
        }
        int len = (i.size() / len_size) & 0xFF;
        apduPrefix.push_back(len);

        std::vector<unsigned char> aidHex;
        cardemulation::HexStrToBytes(i, aidHex);
        apduPrefix.insert(apduPrefix.end(), aidHex.begin(), aidHex.end());

        DebugLog("expect apdu command.%s", cardemulation::BytesToHexStr(apduPrefix, true).c_str());
        mApduCommand_.push_back(apduPrefix);

        DebugLog("apdu head info: headlen.%d prelen.%d", headLen, apduPrefix.size());
        apduPrefix.erase(apduPrefix.begin() + headLen, apduPrefix.end());
        DebugLog("expect apdu head.%s", cardemulation::BytesToHexStr(apduPrefix, true).c_str());
    }

    DebugLog("start read tag, select aid command list info(%d)!!!", mApduCommand_.size());
    for (size_t a = 0; a < mApduCommand_.size(); a++) {
        DebugLog("Apdu(%d) %s", a, cardemulation::BytesToHexStr(mApduCommand_.at(a), true).c_str());
    }

    if (mApduCommand_.size() == 0) {
        DebugLog("aid command is null !!!");
        return;
    }

    if (bHost) {
        DebugLog("send select aid command to continue!!!");
        // getchar();
        while (!mStartNextStepFlag_) {
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }

        mStartNextStepFlag_ = false;
        DebugLog("start next step!");
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    } else {
        DebugLog("send select aid command to offhost, and press any key to continue !!!");
        getchar();
    }
    return;
}
std::shared_ptr<TestNfcCEAgent> TcNfcCeData::GetTestNfcCEAgent() const
{
    return mTestNfcCEAgent_;
}
std::shared_ptr<CardEmulationAbilityInfo> TcNfcCeData::GetCEAbilityInfo() const
{
    return mCEAbilityInfo_;
}

std::shared_ptr<TestNfcCEHostService> TcNfcCeData::GetTestNfcCEHostService() const
{
    return mTestNfcCEHostService_;
}
std::shared_ptr<sdk::cardemulation::CardEmulation> TcNfcCeData::GetHce() const
{
    return mHce_;
}
}  // namespace OHOS::nfc::test
