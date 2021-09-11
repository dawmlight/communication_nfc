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

#include "offhost_card_emulation.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "card_emulation_agent_mock.h"
#include "card_emulation_agent_proxy.h"
#include "card_emulation_service_info_lite.h"
#include "icard_emulation_agent.h"
#include "nfc_sdk_common.h"

using namespace OHOS::nfc::sdk::cardemulation::test;
using namespace OHOS::nfc::sdk::cardemulation;
using namespace OHOS::nfc::sdk;
using namespace testing;
namespace OHOS::nfc::sdk::cardemulation::test {
/**
* @tc.number:
* @tc.name  :
* @tc.desc  :
*/
TEST(OffHostCardEmulation, AddAidsForService)
{
    OHOS::AppExecFwk::ElementName serviceName;
    std::vector<std::string> presetResult{"F00102030401",
                                          "F00102030402",
                                          "F00102030403",
                                          "F00102030404",
                                          "F00102030405"};

    {
        OHOS::sptr<ICardEmulationAgent> cea;

        EXPECT_FALSE(cea);
        // std::shared_ptr<ICardEmulationAgent> is empty
        auto ce = std::make_shared<OffHostCardEmulation>(serviceName, cea);
        EXPECT_EQ(ce->AddAidsForService({}, CARDEMULATION_SERVICE_TYPE_SECURE), false);
    }
    auto test = [&presetResult](bool result, int serviceRet, const OHOS::AppExecFwk::ElementName& serviceName) {
        auto cea = OHOS::sptr<MockCardEmulationAgentProxy>(new MockCardEmulationAgentProxy(nullptr));

        EXPECT_CALL(*cea, AddAidsForService(_, _, _, _)).WillOnce(Return(serviceRet));
        auto ce = std::make_shared<OffHostCardEmulation>(serviceName, cea);

        EXPECT_EQ(ce->AddAidsForService(presetResult, CARDEMULATION_SERVICE_TYPE_SECURE), result);
    };
    test(true, NFC_SUCCESS, serviceName);
    test(false, NFC_SER_ERROR_DISCONNECT, serviceName);
    test(false, NFC_SDK_ERROR_NFC_STATE_INVALID, serviceName);
}

/**
* @tc.number:
* @tc.name  :
* @tc.desc  :
*/
TEST(OffHostCardEmulation, RemoveAidsForService)
{
    OHOS::AppExecFwk::ElementName serviceName;
    std::vector<std::string> presetResult{"F00102030401",
                                          "F00102030402",
                                          "F00102030403",
                                          "F00102030404",
                                          "F00102030405"};

    {
        OHOS::sptr<ICardEmulationAgent> cea;
        EXPECT_FALSE(cea);
        // std::shared_ptr<ICardEmulationAgent> is empty
        auto ce = std::make_shared<OffHostCardEmulation>(serviceName, cea);
        EXPECT_EQ(ce->RemoveAidsForService(CARDEMULATION_SERVICE_TYPE_SECURE), false);
    }
    auto test = [&presetResult](bool result, int serviceRet, const OHOS::AppExecFwk::ElementName& serviceName) {
        auto cea = OHOS::sptr<MockCardEmulationAgentProxy>(new MockCardEmulationAgentProxy(nullptr));

        EXPECT_CALL(*cea, RemoveAidsForService(_, _, _)).WillOnce(Return(serviceRet));
        auto ce = std::make_shared<OffHostCardEmulation>(serviceName, cea);

        EXPECT_EQ(ce->RemoveAidsForService(CARDEMULATION_SERVICE_TYPE_SECURE), result);
    };
    test(true, NFC_SUCCESS, serviceName);
    test(false, NFC_SER_ERROR_DISCONNECT, serviceName);
    test(false, NFC_SDK_ERROR_NFC_STATE_INVALID, serviceName);
}
/**
* @tc.number:
* @tc.name  :
* @tc.desc  :
*/
TEST(OffHostCardEmulation, RegisterOffHostService)
{
    OHOS::AppExecFwk::ElementName serviceName;

    {
        OHOS::sptr<ICardEmulationAgent> cea;
        EXPECT_FALSE(cea);
        // std::shared_ptr<ICardEmulationAgent> is empty
        auto ce = std::make_shared<OffHostCardEmulation>(serviceName, cea);
        EXPECT_EQ(ce->RegisterOffHostService(NFC_EXECUTION_ENVIRONMENT_ESE), false);
    }
    auto test = [](bool result,
                   int serviceRet,
                   const OHOS::AppExecFwk::ElementName& serviceName,
                   const std::string& executionEnvironment) {
        auto cea = OHOS::sptr<MockCardEmulationAgentProxy>(new MockCardEmulationAgentProxy(nullptr));
        EXPECT_CALL(*cea, RegisterOffHostService(_, _, _)).WillOnce(Return(serviceRet));
        auto ce = std::make_shared<OffHostCardEmulation>(serviceName, cea);

        EXPECT_EQ(ce->RegisterOffHostService(executionEnvironment), result);
    };
    test(true, NFC_SUCCESS, serviceName, NFC_EXECUTION_ENVIRONMENT_ESE);
    test(false, NFC_SER_ERROR_DISCONNECT, serviceName, NFC_EXECUTION_ENVIRONMENT_ESE);
    test(false, NFC_SDK_ERROR_NFC_STATE_INVALID, serviceName, NFC_EXECUTION_ENVIRONMENT_ESE);
}
/**
* @tc.number:
* @tc.name  :
* @tc.desc  :
*/
TEST(OffHostCardEmulation, UnregisterOffHostService)
{
    OHOS::AppExecFwk::ElementName serviceName;

    {
        OHOS::sptr<ICardEmulationAgent> cea;
        EXPECT_FALSE(cea);
        // std::shared_ptr<ICardEmulationAgent> is empty
        auto ce = std::make_shared<OffHostCardEmulation>(serviceName, cea);
        EXPECT_EQ(ce->UnregisterOffHostService(), false);
    }
    auto test = [](bool result, int serviceRet, const OHOS::AppExecFwk::ElementName& serviceName) {
        auto cea = OHOS::sptr<MockCardEmulationAgentProxy>(new MockCardEmulationAgentProxy(nullptr));
        EXPECT_CALL(*cea, UnregisterOffHostService(_, _)).WillOnce(Return(serviceRet));
        auto ce = std::make_shared<OffHostCardEmulation>(serviceName, cea);

        EXPECT_EQ(ce->UnregisterOffHostService(), result);
    };
    test(true, NFC_SUCCESS, serviceName);
    test(false, NFC_SER_ERROR_DISCONNECT, serviceName);
    test(false, NFC_SDK_ERROR_NFC_STATE_INVALID, serviceName);
}
}