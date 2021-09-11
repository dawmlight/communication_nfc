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

#ifndef CARD_EMULATION_SERVICE_MOCK_H
#define CARD_EMULATION_SERVICE_MOCK_H

#include <gmock/gmock.h>

#include "card_emulation_service_info.h"
#include "card_emulation_service_info_lite.h"
#include "element_name.h"
#include "icard_emulation_service.h"
namespace OHOS::nfc::cardemulation::test{
class MockCardEmulationService : public OHOS::nfc::cardemulation::ICardEmulationService {
public:
    MOCK_METHOD0(Init, int());
    MOCK_METHOD0(Deinit, int());
    MOCK_CONST_METHOD3(IsDefaultServiceForType, bool(int, const OHOS::AppExecFwk::ElementName&, const std::string&));
    MOCK_CONST_METHOD3(IsDefaultServiceForAid,
                       bool(int userId, const OHOS::AppExecFwk::ElementName& elementName, const std::string& aid));
    MOCK_METHOD2(GetPrimaryServiceForType, OHOS::AppExecFwk::ElementName(int userId, const std::string& tag));
    MOCK_METHOD3(SetPrimaryServiceForType,
                 int(int userId, const OHOS::AppExecFwk::ElementName& elementName, const std::string& tag));
    MOCK_METHOD3(RegisterRemoteObject,
                 int(int userId,
                     const OHOS::AppExecFwk::ElementName& elementName,
                     const OHOS::sptr<OHOS::IRemoteObject>& apduChannel));
    MOCK_METHOD4(AddAidsForService,
                 int(int userId,
                     const OHOS::AppExecFwk::ElementName& elementName,
                     const std::string& tag,
                     const std::vector<std::string>& aidSet));
    MOCK_METHOD3(RemoveAidsForService,
                 int(int userId, const OHOS::AppExecFwk::ElementName& elementName, const std::string& tag));
    MOCK_METHOD3(MarkOffHostForService,
                 int(int userId,
                     const OHOS::AppExecFwk::ElementName& elementName,
                     const std::string& offHostSecureElement));
    MOCK_METHOD2(UnmarkOffHostForService, int(int userId, const OHOS::AppExecFwk::ElementName& elementName));
    MOCK_CONST_METHOD3(GetAidsForService,
                       std::vector<std::string>(int userId,
                                                const OHOS::AppExecFwk::ElementName& elementName,
                                                const std::string& tag));

    MOCK_METHOD2(GetDescriptionForPreferredService, std::string(int userId, std::string type));
    MOCK_METHOD2(SetPreferredService, int(int userId, const OHOS::AppExecFwk::ElementName& elementName));
    MOCK_METHOD1(UnsetPreferredService, int(int userId));
    MOCK_CONST_METHOD0(SupportsAidPrefix, bool());
    MOCK_METHOD2(GetServicesByType,
                 std::vector<std::unique_ptr<OHOS::nfc::sdk::cardemulation::CardEmulationServiceInfoLite>>(
                     int userId,
                     const std::string& type));

    MOCK_METHOD0(OnHCEActivated, int(void));
    MOCK_METHOD2(OnHCEData, int(const unsigned char* data, size_t len));
    MOCK_METHOD0(OnHCEDeactivated, int(void));
    MOCK_METHOD0(OnOffHostTransaction, int(void));
    MOCK_METHOD1(OnUserSwitched, int(int userId));
    MOCK_METHOD0(OnNfcEnabled, void());
    MOCK_METHOD0(OnNfcDisabled, void());
    MOCK_METHOD0(OnSecureNfcToggled, void());
};
}
#endif  // !CARD_EMULATION_SERVICE_MOCK_H
