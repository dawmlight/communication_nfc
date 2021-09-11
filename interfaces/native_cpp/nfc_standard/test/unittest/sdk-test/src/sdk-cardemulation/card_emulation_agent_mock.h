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

#ifndef NFC_CARD_EMULATION_AGENT_STUB_MOCK_H
#define NFC_CARD_EMULATION_AGENT_STUB_MOCK_H

#include <gmock/gmock.h>

#include <map>

#include "card_emulation_service_info_lite.h"
#include "icard_emulation_agent.h"
#include "iremote_proxy.h"
#include "iremote_stub.h"

namespace OHOS::nfc::sdk::cardemulation::test {
class MockRemoteObject : public OHOS::IRemoteObject {
public:
    MOCK_METHOD0(GetObjectRefCount, int32_t());
    MOCK_METHOD4(SendRequest, int(uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option));
    MOCK_METHOD1(AddDeathRecipient, bool(const sptr<DeathRecipient>& recipient));
    MOCK_METHOD1(RemoveDeathRecipient, bool(const sptr<DeathRecipient>& recipient));
    MOCK_METHOD2(Dump, int(int fd, const std::vector<std::u16string>& args));
};

class MockCardEmulationAgent : public OHOS::IRemoteStub<ICardEmulationAgent> {
public:
    MOCK_METHOD4(SendRequest, int(uint32_t, MessageParcel&, MessageParcel&, MessageOption&));
    MOCK_METHOD3(GetServices,
                 int(int userId, const std::string& type, std::vector<std::unique_ptr<CardEmulationServiceInfoLite>>&));
    MOCK_METHOD4(GetAidsForService,
                 int(int userId,
                     const OHOS::AppExecFwk::ElementName& serviceName,
                     const std::string& type,
                     std::vector<std::string>& result));
    MOCK_METHOD3(RegisterRemoteObject,
                 int(int userId,
                     const OHOS::AppExecFwk::ElementName& elementName,
                     const OHOS::sptr<OHOS::IRemoteObject>& apduChannel));

    MOCK_METHOD4(AddAidsForService,
                 int(int userId,
                     const OHOS::AppExecFwk::ElementName& serviceName,
                     std::vector<std::string> aids,
                     std::string type));
    MOCK_METHOD3(RemoveAidsForService,
                 int(int userId, const OHOS::AppExecFwk::ElementName& serviceName, std::string type));
    MOCK_METHOD3(RegisterOffHostService,
                 int(int userId, const OHOS::AppExecFwk::ElementName& serviceName, std::string offHostSecureElement));
    MOCK_METHOD2(UnregisterOffHostService, int(int userId, const OHOS::AppExecFwk::ElementName& serviceName));
    MOCK_METHOD1(IsSupportedAidPrefixMode, int(bool& result));
    MOCK_METHOD3(GetDescriptionForPreferredService, int(int userId, std::string type, std::string& result));
    MOCK_METHOD2(SetPreferredForegroundService, int(int userId, const OHOS::AppExecFwk::ElementName& serviceName));
    MOCK_METHOD1(UnsetPreferredForegroundService, int(int userId));
    MOCK_METHOD3(GetPrimaryServiceForType,
                 int(int userId, const std::string& type, OHOS::AppExecFwk::ElementName& result));
    MOCK_METHOD3(SetPrimaryServiceForType,
                 int(int userId, const OHOS::AppExecFwk::ElementName& serviceName, const std::string& type));

    MOCK_METHOD0(AsObject, sptr<IRemoteObject>());
};

class MockCardEmulationAgentProxy : public OHOS::IRemoteProxy<ICardEmulationAgent> {
public:
    MockCardEmulationAgentProxy(const OHOS::sptr<OHOS::IRemoteObject>& ro)
      : OHOS::IRemoteProxy<ICardEmulationAgent>(ro){};
    MOCK_METHOD3(GetServices,
                 int(int userId, const std::string& type, std::vector<std::unique_ptr<CardEmulationServiceInfoLite>>&));
    MOCK_METHOD4(GetAidsForService,
                 int(int userId,
                     const OHOS::AppExecFwk::ElementName& serviceName,
                     const std::string& type,
                     std::vector<std::string>& result));
    MOCK_METHOD3(RegisterRemoteObject,
                 int(int userId,
                     const OHOS::AppExecFwk::ElementName& elementName,
                     const OHOS::sptr<OHOS::IRemoteObject>& apduChannel));
    MOCK_METHOD4(AddAidsForService,
                 int(int userId,
                     const OHOS::AppExecFwk::ElementName& serviceName,
                     std::vector<std::string> aids,
                     std::string type));

    MOCK_METHOD3(RemoveAidsForService,
                 int(int userId, const OHOS::AppExecFwk::ElementName& serviceName, std::string type));
    MOCK_METHOD3(RegisterOffHostService,
                 int(int userId, const OHOS::AppExecFwk::ElementName& serviceName, std::string offHostSecureElement));
    MOCK_METHOD2(UnregisterOffHostService, int(int userId, const OHOS::AppExecFwk::ElementName& serviceName));
    MOCK_METHOD1(IsSupportedAidPrefixMode, int(bool& result));
    MOCK_METHOD3(GetDescriptionForPreferredService, int(int userId, std::string type, std::string& result));
    MOCK_METHOD2(SetPreferredForegroundService, int(int userId, const OHOS::AppExecFwk::ElementName& serviceName));
    MOCK_METHOD1(UnsetPreferredForegroundService, int(int userId));
    MOCK_METHOD3(GetPrimaryServiceForType,
                 int(int userId, const std::string& type, OHOS::AppExecFwk::ElementName& result));
    MOCK_METHOD3(SetPrimaryServiceForType,
                 int(int userId, const OHOS::AppExecFwk::ElementName& serviceName, const std::string& type));

    MOCK_METHOD0(AsObject, OHOS::sptr<OHOS::IRemoteObject>());
};
}  // namespace OHOS::nfc::sdk::cardemulation::test
#endif  // !NFC_CARD_EMULATION_AGENT_STUB_MOCK_H
