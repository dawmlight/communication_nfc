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

#include "card_emulation_agent_proxy.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <cstdio>

#include "card_emulation_agent_mock.h"
#include "card_emulation_service_info_lite.h"
#include "element_name.h"
#include "nfc_sdk_common.h"

#define DELETE_STUB(stub)             \
    if (stub) {                       \
        delete stub.GetRefPtr();      \
        stub.ForceSetRefPtr(nullptr); \
    }

using namespace OHOS::nfc::sdk::cardemulation;
using namespace OHOS::nfc::sdk::cardemulation::test;
using namespace OHOS::nfc::sdk;
using namespace testing;
namespace OHOS::nfc::sdk::cardemulation::test {
const std::string TYPE_SECURE = OHOS::nfc::sdk::cardemulation::CARDEMULATION_SERVICE_TYPE_SECURE;
const std::string TYPE_NORMAL = OHOS::nfc::sdk::cardemulation::CARDEMULATION_SERVICE_TYPE_NORMAL;

/**
* @tc.number:
* @tc.name  :
* @tc.desc  :
*/
TEST(CardEmulationAgentProxy, RemoteObjectIsNull)
{
    OHOS::sptr<OHOS::IRemoteObject> ro;
    auto cap = std::make_unique<CardEmulationAgentProxy>(ro);
    std::vector<std::string> aids{};
    std::string offHostSecureElement{};
    OHOS::AppExecFwk::ElementName serviceName{};
    OHOS::sptr<OHOS::IRemoteObject> apduChannel{};
    std::string type = TYPE_NORMAL;
    bool mode = false;
    int userId = 1;
    EXPECT_EQ(cap->IsSupportedAidPrefixMode(mode), NFC_SDK_ERROR_NOT_INITIALIZED);

    EXPECT_EQ(cap->GetAidsForService(userId, serviceName, type, aids), NFC_SDK_ERROR_NOT_INITIALIZED);
    EXPECT_EQ(cap->RegisterRemoteObject(userId, serviceName, apduChannel), NFC_SDK_ERROR_NOT_INITIALIZED);

    EXPECT_EQ(cap->AddAidsForService(userId, serviceName, aids, type), NFC_SDK_ERROR_NOT_INITIALIZED);

    EXPECT_EQ(cap->RemoveAidsForService(userId, serviceName, type), NFC_SDK_ERROR_NOT_INITIALIZED);

    EXPECT_EQ(cap->RegisterOffHostService(userId, serviceName, offHostSecureElement), NFC_SDK_ERROR_NOT_INITIALIZED);

    EXPECT_EQ(cap->UnregisterOffHostService(userId, serviceName), NFC_SDK_ERROR_NOT_INITIALIZED);

    std::string desc;
    EXPECT_EQ(cap->GetDescriptionForPreferredService(userId, type, desc), NFC_SDK_ERROR_NOT_INITIALIZED);

    EXPECT_EQ(cap->SetPreferredForegroundService(userId, serviceName), NFC_SDK_ERROR_NOT_INITIALIZED);

    EXPECT_EQ(cap->UnsetPreferredForegroundService(userId), NFC_SDK_ERROR_NOT_INITIALIZED);

    EXPECT_EQ(cap->GetPrimaryServiceForType(userId, type, serviceName), NFC_SDK_ERROR_NOT_INITIALIZED);

    EXPECT_EQ(cap->SetPrimaryServiceForType(userId, serviceName, type), NFC_SDK_ERROR_NOT_INITIALIZED);
}

/**
* @tc.number:
* @tc.name  :
* @tc.desc  :
*/
TEST(CardEmulationAgentProxy, GetAidsForService)
{
    auto test = [](int rv) {
        int userId = 1;
        OHOS::AppExecFwk::ElementName serviceName{};
        std::string type = TYPE_NORMAL;
        std::vector<std::string> aids{};
        // successful
        std::vector<std::string> presetResult{"F00102030401",
                                              "F00102030402",
                                              "F00102030403",
                                              "F00102030404",
                                              "F00102030405"};

        auto stub = OHOS::sptr<MockCardEmulationAgent>(new MockCardEmulationAgent());
        EXPECT_CALL(*stub, SendRequest(_, _, _, _)).WillRepeatedly(Return(rv));

        EXPECT_CALL(*stub, AsObject()).WillRepeatedly(Return(stub));
        auto cap = std::make_unique<CardEmulationAgentProxy>(stub->AsObject());

        EXPECT_EQ(cap->GetAidsForService(userId, serviceName, type, aids), rv);
        if (!aids.empty()) {
            EXPECT_TRUE(memcmp(&presetResult[0], &aids[0], aids.size()) == 0);
        }
        DELETE_STUB(stub);
    };

    test(NFC_SUCCESS);
    test(NFC_SER_ERROR_DISCONNECT);
}

/**
* @tc.number:
* @tc.name  :
* @tc.desc  :
*/
TEST(CardEmulationAgentProxy, AddAidsForService)
{
    auto test = [](int rv) {
        int userId = 1;
        OHOS::AppExecFwk::ElementName serviceName{};
        std::string type = TYPE_NORMAL;
        // successful
        std::vector<std::string> presetResult{"F00102030401",
                                              "F00102030402",
                                              "F00102030403",
                                              "F00102030404",
                                              "F00102030405"};

        auto stub = OHOS::sptr<MockCardEmulationAgent>(new MockCardEmulationAgent());
        EXPECT_CALL(*stub, SendRequest(_, _, _, _)).WillRepeatedly(Return(rv));
        EXPECT_CALL(*stub, AsObject()).WillRepeatedly(Return(stub));
        auto cap = std::make_unique<CardEmulationAgentProxy>(stub->AsObject());

        EXPECT_EQ(cap->AddAidsForService(userId, serviceName, presetResult, type), rv);
        DELETE_STUB(stub);
    };
    test(NFC_SUCCESS);
    test(NFC_SER_ERROR_DISCONNECT);
}
/**
* @tc.number:
* @tc.name  :
* @tc.desc  :
*/
TEST(CardEmulationAgentProxy, RemoveAidsForService)
{
    auto test = [](int rv) {
        int userId = 1;
        OHOS::AppExecFwk::ElementName serviceName{};
        std::string type = TYPE_NORMAL;
        // successful
        auto stub = OHOS::sptr<MockCardEmulationAgent>(new MockCardEmulationAgent());
        EXPECT_CALL(*stub, AsObject()).WillRepeatedly(Return(stub));

        EXPECT_CALL(*stub, SendRequest(_, _, _, _)).WillRepeatedly(Return(rv));
        auto cap = std::make_unique<CardEmulationAgentProxy>(stub->AsObject());

        EXPECT_EQ(cap->RemoveAidsForService(userId, serviceName, type), rv);
        DELETE_STUB(stub);
    };
    test(NFC_SUCCESS);
    test(NFC_SER_ERROR_DISCONNECT);
}
/**
* @tc.number:
* @tc.name  :
* @tc.desc  :
*/
TEST(CardEmulationAgentProxy, RegisterOffHostService)
{
    auto test = [](int rv) {
        int userId = 1;
        OHOS::AppExecFwk::ElementName serviceName{};
        std::string seName{"eSE1"};
        // successful
        auto stub = OHOS::sptr<MockCardEmulationAgent>(new MockCardEmulationAgent());
        EXPECT_CALL(*stub, AsObject()).WillRepeatedly(Return(stub));

        EXPECT_CALL(*stub, SendRequest(_, _, _, _)).WillRepeatedly(Return(rv));
        auto cap = std::make_unique<CardEmulationAgentProxy>(stub->AsObject());

        EXPECT_EQ(cap->RegisterOffHostService(userId, serviceName, seName), rv);
        DELETE_STUB(stub);
    };
    test(NFC_SUCCESS);
    test(NFC_SER_ERROR_DISCONNECT);
}
/**
* @tc.number:
* @tc.name  :
* @tc.desc  :
*/
TEST(CardEmulationAgentProxy, UnregisterOffHostService)
{
    auto test = [](int rv) {
        int userId = 1;
        OHOS::AppExecFwk::ElementName serviceName{};
        auto stub = OHOS::sptr<MockCardEmulationAgent>(new MockCardEmulationAgent());
        EXPECT_CALL(*stub, AsObject()).WillRepeatedly(Return(stub));

        EXPECT_CALL(*stub, SendRequest(_, _, _, _)).WillRepeatedly(Return(rv));
        auto cap = std::make_unique<CardEmulationAgentProxy>(stub->AsObject());

        EXPECT_EQ(cap->UnregisterOffHostService(userId, serviceName), rv);
        DELETE_STUB(stub);
    };
    test(NFC_SUCCESS);
    test(NFC_SER_ERROR_DISCONNECT);
}
/**
* @tc.number:
* @tc.name  :
* @tc.desc  :
*/
TEST(CardEmulationAgentProxy, IsSupportedAidPrefixMode)
{
    auto test = [](int rv, bool result) {
        auto stub = OHOS::sptr<MockCardEmulationAgent>(new MockCardEmulationAgent());
        EXPECT_CALL(*stub, AsObject()).WillRepeatedly(Return(stub));

        EXPECT_CALL(*stub, SendRequest(_, _, _, _)).WillRepeatedly(Return(rv));
        auto cap = std::make_unique<CardEmulationAgentProxy>(stub->AsObject());

        bool mode;
        EXPECT_EQ(cap->IsSupportedAidPrefixMode(mode), rv);
        EXPECT_EQ(mode, result);
        DELETE_STUB(stub);
    };
    test(NFC_SUCCESS, false);
    test(NFC_SER_ERROR_DISCONNECT, false);
}
/**
* @tc.number:
* @tc.name  :
* @tc.desc  :
*/
TEST(CardEmulationAgentProxy, GetDescriptionForPreferredService)
{
    auto test = [](int rv, std::string result) {
        int userId = 1;
        std::string type = TYPE_NORMAL;
        auto stub = OHOS::sptr<MockCardEmulationAgent>(new MockCardEmulationAgent());
        EXPECT_CALL(*stub, AsObject()).WillRepeatedly(Return(stub));

        EXPECT_CALL(*stub, SendRequest(_, _, _, _)).WillRepeatedly(Return(rv));
        auto cap = std::make_unique<CardEmulationAgentProxy>(stub->AsObject());

        std::string desc;
        EXPECT_EQ(cap->GetDescriptionForPreferredService(userId, type, desc), rv);
        EXPECT_STREQ(desc.c_str(), result.c_str());
        DELETE_STUB(stub);
    };
    test(NFC_SER_ERROR_DISCONNECT, "");
}
/**
* @tc.number:
* @tc.name  :
* @tc.desc  :
*/
TEST(CardEmulationAgentProxy, SetPreferredForegroundService)
{
    auto test = [](int rv) {
        int userId = 1;
        OHOS::AppExecFwk::ElementName serviceName{};
        // successful
        auto stub = OHOS::sptr<MockCardEmulationAgent>(new MockCardEmulationAgent());
        EXPECT_CALL(*stub, AsObject()).WillRepeatedly(Return(stub));

        EXPECT_CALL(*stub, SendRequest(_, _, _, _)).WillRepeatedly(Return(rv));
        auto cap = std::make_unique<CardEmulationAgentProxy>(stub->AsObject());

        EXPECT_EQ(cap->UnregisterOffHostService(userId, serviceName), rv);
        DELETE_STUB(stub);
    };
    test(NFC_SUCCESS);
    test(NFC_SER_ERROR_DISCONNECT);
}
/**
* @tc.number:
* @tc.name  :
* @tc.desc  :
*/
TEST(CardEmulationAgentProxy, UnsetPreferredForegroundService)
{
    auto test = [](int rv) {
        int userId = 1;
        // successful
        auto stub = OHOS::sptr<MockCardEmulationAgent>(new MockCardEmulationAgent());
        EXPECT_CALL(*stub, AsObject()).WillRepeatedly(Return(stub));

        EXPECT_CALL(*stub, SendRequest(_, _, _, _)).WillRepeatedly(Return(rv));
        auto cap = std::make_unique<CardEmulationAgentProxy>(stub->AsObject());

        EXPECT_EQ(cap->UnsetPreferredForegroundService(userId), rv);
        DELETE_STUB(stub);
    };
    test(NFC_SUCCESS);
    test(NFC_SER_ERROR_DISCONNECT);
}
/**
* @tc.number:
* @tc.name  :
* @tc.desc  :
*/
TEST(CardEmulationAgentProxy, GetPrimaryServiceForType)
{
    auto test = [](int rv, const std::string& type, const OHOS::AppExecFwk::ElementName& serviceName) {
        int userId = 1;
        auto stub = OHOS::sptr<MockCardEmulationAgent>(new MockCardEmulationAgent());
        EXPECT_CALL(*stub, AsObject()).WillRepeatedly(Return(stub));

        EXPECT_CALL(*stub, SendRequest(_, _, _, _)).WillRepeatedly(Return(rv));
        auto cap = std::make_unique<CardEmulationAgentProxy>(stub->AsObject());

        OHOS::AppExecFwk::ElementName sn;
        EXPECT_EQ(cap->GetPrimaryServiceForType(userId, type, sn), rv);
        EXPECT_EQ(sn, serviceName);
        EXPECT_TRUE(sn.operator==(serviceName));
        DELETE_STUB(stub);
    };
    OHOS::AppExecFwk::ElementName en;
    en.SetAbilityName("hce1");
    test(NFC_SER_ERROR_DISCONNECT, TYPE_SECURE, OHOS::AppExecFwk::ElementName());
}
/**
* @tc.number:
* @tc.name  :
* @tc.desc  :
*/
TEST(CardEmulationAgentProxy, SetPrimaryServiceForType)
{
    auto test = [](int rv, const std::string& type, const OHOS::AppExecFwk::ElementName& serviceName) {
        int userId = 1;
        // successful
        auto stub = OHOS::sptr<MockCardEmulationAgent>(new MockCardEmulationAgent());
        EXPECT_CALL(*stub, AsObject()).WillRepeatedly(Return(stub));

        EXPECT_CALL(*stub, SendRequest(_, _, _, _)).WillRepeatedly(Return(rv));
        auto cap = std::make_unique<CardEmulationAgentProxy>(stub->AsObject());

        EXPECT_EQ(cap->SetPrimaryServiceForType(userId, serviceName, type), rv);
        DELETE_STUB(stub);
    };
    OHOS::AppExecFwk::ElementName serviceName{};
    serviceName.SetAbilityName("hce1");
    test(NFC_SUCCESS, TYPE_SECURE, serviceName);
    test(NFC_SER_ERROR_DISCONNECT, TYPE_SECURE, OHOS::AppExecFwk::ElementName());
}
} 
