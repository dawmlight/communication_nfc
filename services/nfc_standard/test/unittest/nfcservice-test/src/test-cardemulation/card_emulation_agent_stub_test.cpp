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

#include "card_emulation_agent.h"
#include "card_emulation_def.h"
#include "card_emulation_error.h"
#include "card_emulation_service_mock.h"
#include "test_util.h"

using namespace OHOS::nfc::cardemulation;
using namespace OHOS::nfc::cardemulation::test;
using namespace OHOS::nfc::sdk::cardemulation;
using namespace testing;
namespace OHOS::nfc::cardemulation::test{
const std::string TYPE_SECURE = OHOS::nfc::cardemulation::CARDEMULATION_SERVICE_TYPE_SECURE;
const std::string TYPE_NORMAL = OHOS::nfc::cardemulation::CARDEMULATION_SERVICE_TYPE_NORMAL;
/**
 * @tc.number:
 * @tc.name  :
 * @tc.desc  :
 */
TEST(CardEmulationAgent, with_null_CardEmulationService)
{
    auto agent = std::make_shared<CardEmulationAgent>(std::shared_ptr<ICardEmulationService>());

    EXPECT_EQ(agent->AddAidsForService(0, Util::CreateElementName("hce1"), {}, TYPE_SECURE),
              ERR_CARD_EMULATION_SERVICE_NOT_INIT);
    std::vector<std::string> result;
    EXPECT_EQ(agent->GetAidsForService({}, {}, {}, result), ERR_CARD_EMULATION_SERVICE_NOT_INIT);

    EXPECT_EQ(agent->RemoveAidsForService({}, {}, {}), ERR_CARD_EMULATION_SERVICE_NOT_INIT);
    EXPECT_EQ(agent->RegisterOffHostService({}, {}, {}), ERR_CARD_EMULATION_SERVICE_NOT_INIT);
    EXPECT_EQ(agent->UnregisterOffHostService({}, {}), ERR_CARD_EMULATION_SERVICE_NOT_INIT);
    bool mode;
    EXPECT_EQ(agent->IsSupportedAidPrefixMode(mode), ERR_CARD_EMULATION_SERVICE_NOT_INIT);

    std::string desc;
    EXPECT_EQ(agent->GetDescriptionForPreferredService({}, {}, desc), ERR_CARD_EMULATION_SERVICE_NOT_INIT);
    EXPECT_EQ(agent->SetPreferredForegroundService({}, {}), ERR_CARD_EMULATION_SERVICE_NOT_INIT);
    EXPECT_EQ(agent->UnsetPreferredForegroundService({}), ERR_CARD_EMULATION_SERVICE_NOT_INIT);

    OHOS::AppExecFwk::ElementName elementName;
    EXPECT_EQ(agent->GetPrimaryServiceForType({}, {}, elementName), ERR_CARD_EMULATION_SERVICE_NOT_INIT);
    EXPECT_EQ(agent->SetPrimaryServiceForType({}, {}, {}), ERR_CARD_EMULATION_SERVICE_NOT_INIT);
}

/**
 * @tc.number:
 * @tc.name  :
 * @tc.desc  :
 */
TEST(CardEmulationAgent, 0_service)
{
    auto hceName = Util::CreateElementName("hce1");
    int presetUserId = 0;
    std::vector<std::string> presetAids{"F00102030401", "F00102030402", "F00102030403", "F00102030404", "F00102030405"};
    std::vector<std::string> presetResult;

    auto srv = std::make_shared<MockCardEmulationService>();
    EXPECT_CALL(*srv, Init()).WillOnce(Return(ERR_OK));
    EXPECT_CALL(*srv, Deinit()).WillOnce(Return(ERR_OK));

    EXPECT_CALL(*srv, GetAidsForService(_, _, _)).WillRepeatedly(Return(presetResult));

    EXPECT_EQ(srv->Init(), ERR_OK);

    auto agent = std::make_shared<CardEmulationAgent>(srv);
    // Test 0 service
    std::vector<std::string> result;

    for (int i = -1; i < 5; ++i) {
        // userId bounds
        EXPECT_EQ(agent->GetAidsForService(i, hceName, TYPE_SECURE, result), ERR_OK);
        EXPECT_TRUE(result.empty());
    }
    EXPECT_CALL(*srv, AddAidsForService(presetUserId, _, _, _)).WillRepeatedly(Return(ERR_AIDSET_IS_EMPTY));
    EXPECT_EQ(agent->AddAidsForService(presetUserId, hceName, {}, TYPE_NORMAL), ERR_AIDSET_IS_EMPTY);

    EXPECT_CALL(*srv, RemoveAidsForService(presetUserId, _, _)).WillRepeatedly(Return(ERR_NOT_FOUND_USER_INFO));
    EXPECT_EQ(agent->RemoveAidsForService(presetUserId, hceName, TYPE_NORMAL), ERR_NOT_FOUND_USER_INFO);

    EXPECT_CALL(*srv, MarkOffHostForService(presetUserId, _, _)).WillRepeatedly(Return(ERR_NOT_FOUND_USER_INFO));
    EXPECT_EQ(agent->RegisterOffHostService(presetUserId, hceName, TYPE_NORMAL), ERR_NOT_FOUND_USER_INFO);

    EXPECT_CALL(*srv, UnmarkOffHostForService(presetUserId, _)).WillRepeatedly(Return(ERR_NOT_FOUND_USER_INFO));
    EXPECT_EQ(agent->UnregisterOffHostService(presetUserId, hceName), ERR_NOT_FOUND_USER_INFO);

    EXPECT_CALL(*srv, SetPreferredService(presetUserId, _)).WillRepeatedly(Return(ERR_NOT_FOUND_USER_INFO));
    EXPECT_EQ(agent->SetPreferredForegroundService(presetUserId, hceName), ERR_NOT_FOUND_USER_INFO);

    EXPECT_CALL(*srv, UnsetPreferredService(presetUserId)).WillRepeatedly(Return(ERR_NOT_FOUND_USER_INFO));
    EXPECT_EQ(agent->UnsetPreferredForegroundService(presetUserId), ERR_NOT_FOUND_USER_INFO);

    OHOS::AppExecFwk::ElementName elementName;

    EXPECT_CALL(*srv, GetPrimaryServiceForType(_, _)).WillRepeatedly(Return(elementName));
    EXPECT_EQ(agent->GetPrimaryServiceForType(presetUserId, TYPE_SECURE, elementName), ERR_OK);

    EXPECT_CALL(*srv, SetPrimaryServiceForType(_, _, _)).WillRepeatedly(Return(ERR_NOT_FOUND_USER_INFO));
    EXPECT_EQ(agent->SetPrimaryServiceForType(presetUserId, hceName, TYPE_SECURE), ERR_NOT_FOUND_USER_INFO);

    EXPECT_EQ(srv->Deinit(), ERR_OK);
}

/**
 * @tc.number:
 * @tc.name  :
 * @tc.desc  :
 */
TEST(CardEmulationAgent, 1_service)
{
    const std::string TYPE_SECURE = OHOS::nfc::cardemulation::CARDEMULATION_SERVICE_TYPE_SECURE;
    const std::string TYPE_NORMAL = OHOS::nfc::cardemulation::CARDEMULATION_SERVICE_TYPE_NORMAL;
    auto srv = std::make_shared<MockCardEmulationService>();

    EXPECT_CALL(*srv, Init()).WillOnce(Return(ERR_OK));
    EXPECT_CALL(*srv, Deinit()).WillOnce(Return(ERR_OK));

    std::vector<std::string> presetResult{"F00102030401",
                                          "F00102030402",
                                          "F00102030403",
                                          "F00102030404",
                                          "F00102030405"};
    int presetUserId = 1;
    EXPECT_CALL(*srv, GetAidsForService(presetUserId, _, _)).WillOnce(Return(presetResult));
    EXPECT_CALL(*srv, GetAidsForService(Ne(presetUserId), _, _)).WillRepeatedly(Return(std::vector<std::string>{}));

    EXPECT_EQ(srv->Init(), ERR_OK);

    auto agent = std::make_shared<CardEmulationAgent>(srv);
    // Test 0 service
    std::vector<std::string> result;

    for (int i = -1; i < 5; ++i) {
        // userId bounds
        if (i == presetUserId) {
            EXPECT_EQ(agent->GetAidsForService(i, Util::CreateElementName("hce1"), TYPE_SECURE, result), ERR_OK);
            EXPECT_TRUE(result.size() == presetResult.size());

        } else {
            EXPECT_EQ(agent->GetAidsForService(i, Util::CreateElementName("hce1"), TYPE_SECURE, result), ERR_OK);
            EXPECT_TRUE(result.empty());
        }
    }

    EXPECT_EQ(srv->Deinit(), ERR_OK);
}
}