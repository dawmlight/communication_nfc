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
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <iostream>
#include <memory>
#include <string>
#include <thread>

#include "card_emulation_ability_info.h"
#include "icard_emulation_agent.h"
#include "ihost_card_emulation.h"
#include "loghelper.h"
#include "nfc_agent.h"
#include "test_nfc_ce_agent.h"

using namespace testing;
using namespace std;
using namespace OHOS::nfc::sdk;
namespace OHOS::nfc::test {
class TcNfcCeBasicTest : public testing::Test {
public:
    virtual void SetUp() override
    {
        std::cout << " SetUp TcNfcCeBasicTest." << std::endl;
        mTestNfcCEAgent_ = std::make_shared<TestNfcCEAgent>();
    }

    void StartBasicTest();
    virtual void TearDown() override
    {
        std::cout << " TearDown TcNfcCeBasicTest." << std::endl;
    }

public:
    std::shared_ptr<TestNfcCEAgent> mTestNfcCEAgent_;
};
void TcNfcCeBasicTest::StartBasicTest()
{
    int userid = 0;
    std::vector<std::shared_ptr<CardEmulationAbilityInfo>> abilityInfoList = LoadAllAbilityInfosByUserId(userid);
    std::shared_ptr<CardEmulationAbilityInfo> abilityInfo = abilityInfoList.at(0);
    mTestNfcCEAgent_->Initailize(abilityInfo);

    mTestNfcCEAgent_->TurnOnNfc();
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    mTestNfcCEAgent_->StartAbility();
    mTestNfcCEAgent_->ActiveAbility();
}

TEST_F(TcNfcCeBasicTest, TestGetAgent_Test)
{
    DebugLog("TestGetAgent_Test in");
    StartBasicTest();
    std::shared_ptr<NfcAgent> agent = mTestNfcCEAgent_->GetNfcAgent();
    EXPECT_TRUE(agent);

    if (agent) {
        bool bNfcCe = agent->HasNfcHceCapability();
        EXPECT_TRUE(bNfcCe);
        OHOS::wptr<cardemulation::ICardEmulationAgent> ceAgent = agent->GetNfcCardEmulationService();
        EXPECT_TRUE(ceAgent.promote());
    }
}
}  // namespace OHOS::nfc::test
