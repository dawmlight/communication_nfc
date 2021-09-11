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

#include "card_emulation.h"

#include <gtest/gtest.h>

#include "card_emulation_agent_mock.h"
#include "element_name.h"
#include "if_system_ability_manager.h"
#include "infc_agent_service_mock.h"
#include "iservice_registry.h"
#include "nfc_agent.h"
#include "ohos_application.h"
#include "system_ability_definition.h"
using namespace OHOS;

using namespace OHOS::nfc::sdk;
using namespace OHOS::nfc::sdk::cardemulation;

namespace OHOS::nfc::sdk::cardemulation::test {
OHOS::AppExecFwk::ElementName elementName1("device1", "bundle1", "ability1");
class CardEmulationTest : public ::testing::Test {
public:
    CardEmulationTest()=default;
protected:
    void SetUp() override
    {
        ns_ = OHOS::sptr<INfcAgentServiceMock>(new INfcAgentServiceMock());
        ca_ = OHOS::sptr<MockCardEmulationAgent>(new MockCardEmulationAgent());
        EXPECT_CALL(*ns_, GetNfcCardEmulationInterface()).WillRepeatedly(testing::Return(ca_));
        EXPECT_CALL(*ns_, AsObject()).WillRepeatedly(testing::Return(ns_));

        sptr<ISystemAbilityManager> systemAbilityMgr =
            SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
        if (systemAbilityMgr &&
            (systemAbilityMgr->AddSystemAbility(NFC_MANAGER_SYS_ABILITY_ID + 10, ns_->AsObject()) == 0)) {
        } else {
            ASSERT_TRUE(false);
            return;
        }
        nfcAgent_ = std::make_shared<NfcAgent>();

        ce_ = CardEmulation::GetInstance(nfcAgent_);
    }
    void TearDown() override
    {
        if (ca_.GetRefPtr()) {
            delete ca_.GetRefPtr();
            ca_.clear();
        }
        if (ns_.GetRefPtr()) {
            delete ns_.GetRefPtr();
            ns_.clear();
        }
    }
protected:
    OHOS::sptr<INfcAgentServiceMock> ns_;
    std::shared_ptr<NfcAgent> nfcAgent_;
    std::shared_ptr<CardEmulation> ce_;
    OHOS::sptr<ICardEmulationAgent> ca_;
};

/**
* @tc.number:
* @tc.name  :
* @tc.desc  :
*/
TEST_F(CardEmulationTest, GetServices)
{
    ASSERT_TRUE(ns_);
    ASSERT_TRUE(ca_);
    ASSERT_TRUE(ce_);
}
/**
* @tc.number:
* @tc.name  :
* @tc.desc  :
*/
TEST_F(CardEmulationTest, GetAidsForService)
{
    if (ce_) {
        auto aids = ce_->GetAidsForService(elementName1, CARDEMULATION_SERVICE_TYPE_NORMAL);
        EXPECT_TRUE(aids.empty());
    }
}
/**
* @tc.number:
* @tc.name  :
* @tc.desc  :
*/
TEST_F(CardEmulationTest, AddAidsForService)
{
    if (ce_) {
        auto rv = ce_->AddAidsForService(elementName1, {}, CARDEMULATION_SERVICE_TYPE_NORMAL);
        EXPECT_EQ(rv, false);
    }
}
/**
* @tc.number:
* @tc.name  :
* @tc.desc  :
*/
TEST_F(CardEmulationTest, RemoveAidsForService)
{
    if (ce_) {
        auto rv = ce_->RemoveAidsForService(elementName1, CARDEMULATION_SERVICE_TYPE_NORMAL);
        EXPECT_EQ(rv, false);
    }
}
/**
* @tc.number:
* @tc.name  :
* @tc.desc  :
*/
TEST_F(CardEmulationTest, RegisterOffHostService)
{
    if (ce_) {
        auto rv = ce_->RegisterOffHostService(elementName1, NFC_EXECUTION_ENVIRONMENT_HOST);
        EXPECT_EQ(rv, false);
    }
}
/**
* @tc.number:
* @tc.name  :
* @tc.desc  :
*/
TEST_F(CardEmulationTest, UnregisterOffHostService)
{
    if (ce_) {
        auto rv = ce_->UnregisterOffHostService(elementName1);
        EXPECT_EQ(rv, false);
    }
}
/**
* @tc.number:
* @tc.name  :
* @tc.desc  :
*/
TEST_F(CardEmulationTest, IsSupportedAidPrefixMode)
{
    if (ce_) {
        auto rv = ce_->IsSupportedAidPrefixMode();
        EXPECT_EQ(rv, false);
    }
}
/**
* @tc.number:
* @tc.name  :
* @tc.desc  :
*/
TEST_F(CardEmulationTest, GetDescriptionForPreferredService)
{
    if (ce_) {
        auto rv = ce_->GetDescriptionForPreferredService();
        EXPECT_EQ(rv, "");
    }
}
/**
* @tc.number:
* @tc.name  :
* @tc.desc  :
*/
TEST_F(CardEmulationTest, SetPreferredForegroundService)
{
    if (ce_) {
        auto rv = ce_->SetPreferredForegroundService(elementName1);
        EXPECT_EQ(rv, false);
    }
}
/**
* @tc.number:
* @tc.name  :
* @tc.desc  :
*/
TEST_F(CardEmulationTest, UnsetPreferredForegroundService)
{
    if (ce_) {
        auto rv = ce_->UnsetPreferredForegroundService();
        EXPECT_EQ(rv, false);
    }
}
/**
* @tc.number:
* @tc.name  :
* @tc.desc  :
*/
TEST_F(CardEmulationTest, GetPrimaryServiceForType)
{
    if (ce_) {
        auto rv = ce_->GetPrimaryServiceForType(CARDEMULATION_SERVICE_TYPE_SECURE);
        EXPECT_EQ(rv, "");
    }
}
/**
* @tc.number:
* @tc.name  :
* @tc.desc  :
*/
TEST_F(CardEmulationTest, SetPrimaryServiceForType)
{
    if (ce_) {
        auto rv = ce_->SetPrimaryServiceForType(elementName1, CARDEMULATION_SERVICE_TYPE_SECURE);
        EXPECT_EQ(rv, false);
    }
}
} 