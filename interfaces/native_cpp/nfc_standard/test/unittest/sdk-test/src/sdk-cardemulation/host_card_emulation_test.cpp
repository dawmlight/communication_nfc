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

#include "host_card_emulation.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "apdu_channel.h"
#include "card_emulation_agent_mock.h"
#include "card_emulation_agent_proxy.h"
#include "card_emulation_service_info_lite.h"
#include "iremote_object.h"
#include "nfc_sdk_common.h"
#include "refbase.h"

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
TEST(HostCardEmulation, AddAidsForService)
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
        auto hce = std::make_shared<HostCardEmulation>(serviceName, cea);
        EXPECT_EQ(hce->AddAidsForService({}, CARDEMULATION_SERVICE_TYPE_SECURE), false);
    }
    auto test = [&presetResult](bool result, int serviceRet, const OHOS::AppExecFwk::ElementName& serviceName) {
        auto cea = OHOS::sptr<MockCardEmulationAgentProxy>(new MockCardEmulationAgentProxy(nullptr));

        EXPECT_CALL(*cea, AddAidsForService(_, _, _, _)).WillOnce(Return(serviceRet));
        auto hce = std::make_shared<HostCardEmulation>(serviceName, cea);

        EXPECT_EQ(hce->AddAidsForService(presetResult, CARDEMULATION_SERVICE_TYPE_SECURE), result);
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
TEST(HostCardEmulation, RemoveAidsForService)
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
        auto hce = std::make_shared<HostCardEmulation>(serviceName, cea);
        EXPECT_EQ(hce->RemoveAidsForService(CARDEMULATION_SERVICE_TYPE_SECURE), false);
    }
    auto test = [&presetResult](bool result, int serviceRet, const OHOS::AppExecFwk::ElementName& serviceName) {
        auto cea = OHOS::sptr<MockCardEmulationAgentProxy>(new MockCardEmulationAgentProxy(nullptr));

        EXPECT_CALL(*cea, RemoveAidsForService(_, _, _)).WillOnce(Return(serviceRet));
        auto hce = std::make_shared<HostCardEmulation>(serviceName, cea);

        EXPECT_EQ(hce->RemoveAidsForService(CARDEMULATION_SERVICE_TYPE_SECURE), result);
    };
    test(true, NFC_SUCCESS, serviceName);
    test(false, NFC_SER_ERROR_DISCONNECT, serviceName);
    test(false, NFC_SDK_ERROR_NFC_STATE_INVALID, serviceName);
}

struct SetApduCommandCallback {
    explicit SetApduCommandCallback(const std::vector<unsigned char>& apdu) : expectedApdu_(apdu)
    {
    }

    std::vector<unsigned char> operator()(const std::vector<unsigned char>& apdu)
    {
        EXPECT_EQ(memcmp(&expectedApdu_[0], &apdu[0], apdu.size()), 0);
        return expectedApdu_;
    }

private:
    std::vector<unsigned char> expectedApdu_;
};

struct OnDeactive {
    explicit OnDeactive(int expectedReason) : expectedReason_(expectedReason)
    {
    }
    void operator()(int reason) const
    {
        EXPECT_EQ(expectedReason_, reason);
    }

private:
    int expectedReason_;
};
/**
* @tc.number:
* @tc.name  :
* @tc.desc  :
*/
TEST(HostCardEmulation, SetApduCommandCallback)
{
    OHOS::AppExecFwk::ElementName serviceName;
    OHOS::sptr<ICardEmulationAgent> cea;
    auto hce = std::make_shared<HostCardEmulation>(serviceName, cea);
    std::vector<unsigned char> apdu{0x01, 0x02, 0x03};
    EXPECT_EQ(hce->SetApduCommandCallback(SetApduCommandCallback(apdu)), true);
}
/**
* @tc.number:
* @tc.name  :
* @tc.desc  :
*/
TEST(HostCardEmulation, SetDeactiveEventCallback)
{
    OHOS::AppExecFwk::ElementName serviceName;
    OHOS::sptr<ICardEmulationAgent> cea;
    auto hce = std::make_shared<HostCardEmulation>(serviceName, cea);

    int expectedReason = 1;
    EXPECT_EQ(hce->SetDeactiveEventCallback(OnDeactive(expectedReason)), true);
}

/**
* @tc.number:
* @tc.name  :
* @tc.desc  :
*/
TEST(HostCardEmulation, OnConnect)
{
    OHOS::AppExecFwk::ElementName serviceName;
    OHOS::sptr<ICardEmulationAgent> cea;

    serviceName.SetAbilityName("ability1");
    serviceName.SetBundleName("bundle1");
    serviceName.SetDeviceID("device1");
    auto hces = std::make_shared<HostCardEmulation>(serviceName, cea);
    EXPECT_TRUE(hces->OnConnect({}));
}
}
