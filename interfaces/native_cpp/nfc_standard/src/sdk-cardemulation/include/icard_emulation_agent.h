/*
  * Copyright (C) 2021 Huawei Device Co., Ltd.
  * Licensed under the Apache License, Version 2.0 (the "License");

 * * you may not use this file except in compliance with the License.
  * You may obtain a copy of the License at
  *

 * *     http://www.apache.org/licenses/LICENSE-2.0
  *
  * Unless required by applicable law or agreed to in writing,
 * software
  * distributed under the License is distributed on an "AS IS" BASIS,
  * WITHOUT WARRANTIES OR CONDITIONS
 * OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  *
 * limitations under the License.
  */

#ifndef ICARD_EMULATION_AGENT_H
#define ICARD_EMULATION_AGENT_H

#include <string>
#include <vector>

#include "iremote_broker.h"
#include "iremote_object.h"
namespace OHOS::AppExecFwk {
class ElementName;
}

namespace OHOS {
namespace nfc::sdk::cardemulation {

constexpr uint32_t CODE_GET_AIDS_FOR_SERVICE = 1;
constexpr uint32_t CODE_ADD_AIDS_FOR_SERVICE = 2;
constexpr uint32_t CODE_REMOVE_AIDS_FOR_SERVICE = 3;
constexpr uint32_t CODE_REGISTER_OFF_HOST_SERVICE = 4;
constexpr uint32_t CODE_UNREGISTER_OFF_HOST_SERVICE = 5;
constexpr uint32_t CODE_IS_SUPPORTED_AID_PREFIX_MODE = 6;
constexpr uint32_t CODE_GET_DESCRIPTION_FOR_PREFERRED_SERVICE = 7;
constexpr uint32_t CODE_SET_PREFERRED_FOREGROUND_SERVICE = 8;
constexpr uint32_t CODE_UNSET_PREFERRED_FOREGROUND_SERVICE = 9;
constexpr uint32_t CODE_GET_PRIMARY_SERVICE_FOR_TYPE = 10;
constexpr uint32_t CODE_SET_PRIMARY_SERVICE_FOR_TYPE = 11;
constexpr uint32_t CODE_GET_SERVICES_BY_TYPE = 12;
constexpr uint32_t CODE_REGISTER_REMOTE_OBJECT = 13;

const std::string CARDEMULATION_SERVICE_TYPE_SECURE = "secure";
const std::string CARDEMULATION_SERVICE_TYPE_NORMAL = "normal";

const std::string NFC_EXECUTION_ENVIRONMENT_HOST = "host";
const std::string NFC_EXECUTION_ENVIRONMENT_ESE = "eSE";
const std::string NFC_EXECUTION_ENVIRONMENT_UICC = "SIM";

class CardEmulationServiceInfoLite;

class ICardEmulationAgent : public OHOS::IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS::nfc::sdk::cardemulation.ICardEmulationAgent");
    virtual ~ICardEmulationAgent() = default;
    virtual int GetServices(int userId,
                            const std::string& type,
                            std::vector<std::unique_ptr<CardEmulationServiceInfoLite>>&) = 0;
    virtual int GetAidsForService(int userId,
                                  const OHOS::AppExecFwk::ElementName& serviceName,
                                  const std::string& type,
                                  std::vector<std::string>&) = 0;
    virtual int RegisterRemoteObject(int userId,
                                     const OHOS::AppExecFwk::ElementName& serviceName,
                                     const OHOS::sptr<OHOS::IRemoteObject>& apduChannel) = 0;
    virtual int AddAidsForService(int userId,
                                  const OHOS::AppExecFwk::ElementName& serviceName,
                                  std::vector<std::string> aids,
                                  std::string type) = 0;
    virtual int RemoveAidsForService(int userId,
                                     const OHOS::AppExecFwk::ElementName& serviceName,
                                     std::string type) = 0;
    virtual int RegisterOffHostService(int userId,
                                       const OHOS::AppExecFwk::ElementName& serviceName,
                                       std::string offHostSecureElement) = 0;
    virtual int UnregisterOffHostService(int userId, const OHOS::AppExecFwk::ElementName& serviceName) = 0;
    virtual int IsSupportedAidPrefixMode(bool&) = 0;
    virtual int GetDescriptionForPreferredService(int userId, std::string type, std::string&) = 0;
    virtual int SetPreferredForegroundService(int userId, const OHOS::AppExecFwk::ElementName& serviceName) = 0;
    virtual int UnsetPreferredForegroundService(int userId) = 0;
    virtual int GetPrimaryServiceForType(int userId, const std::string& type, OHOS::AppExecFwk::ElementName&) = 0;
    virtual int SetPrimaryServiceForType(int userId,
                                         const OHOS::AppExecFwk::ElementName& serviceName,
                                         const std::string& type) = 0;
};
}  // namespace nfc::sdk::cardemulation
}  // namespace OHOS
#endif  // !ICARD_EMULATION_AGENT_H
