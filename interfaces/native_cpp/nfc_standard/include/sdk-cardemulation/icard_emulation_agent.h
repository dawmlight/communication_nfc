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
    /**
     * brief: get card emulation service infomations
     * parameter:
     *   userId -- user id
     *   type -- service type @see card_emulation_def.h CARDEMULATION_SERVICE_TYPE_*
     *   result[out] -- result
     * return: 0 -- succeeded, not 0 -- error code
     */
    virtual int GetServices(int userId,
                            const std::string& type,
                            std::vector<std::unique_ptr<CardEmulationServiceInfoLite>>&) = 0;
    /**
     * brief: get aids for service
     * parameter:
     *   userId -- user id
     *   elementName -- the card emulation service name
     *   type -- service type
     *   result[out] -- result
     * return: 0 -- succeeded, not 0 -- error code
     */
    virtual int GetAidsForService(int userId,
                                  const OHOS::AppExecFwk::ElementName& elementName,
                                  const std::string& type,
                                  std::vector<std::string>&) = 0;
    /**
     * brief: register remote object
     * parameter:
     *   userId -- user id
     *   elementName -- the card emulation service name
     *   apduChannel -- apdu channel object
     * return: 0 -- succeeded, not 0 -- error code
     */
    virtual int RegisterRemoteObject(int userId,
                                     const OHOS::AppExecFwk::ElementName& elementName,
                                     const OHOS::sptr<OHOS::IRemoteObject>& apduChannel) = 0;
    /**
     * brief: add aids for service
     * parameter:
     *   userId -- user id
     *   elementName -- the card emulation service name
     *   aids -- aids
     *   type -- service type
     * return: 0 -- succeeded, not 0 -- error code
     */
    virtual int AddAidsForService(int userId,
                                  const OHOS::AppExecFwk::ElementName& elementName,
                                  std::vector<std::string> aids,
                                  std::string type) = 0;
    /**
     * brief: remove aids for service
     * parameter:
     *   userId -- user id
     *   elementName -- the card emulation service name
     *   type -- service type
     * return: 0 -- succeeded, not 0 -- error code
     */
    virtual int RemoveAidsForService(int userId,
                                     const OHOS::AppExecFwk::ElementName& elementName,
                                     std::string type) = 0;
    /**
     * brief: register offhost service execution environment
     * parameter:
     *   userId -- user id
     *   elementName -- the card emulation service name
     *   offHostSecureElement -- the secure element name (execution environment)
     * return: 0 -- succeeded, not 0 -- error code
     */
    virtual int RegisterOffHostService(int userId,
                                       const OHOS::AppExecFwk::ElementName& elementName,
                                       std::string offHostSecureElement) = 0;
    /**
     * brief: unregister offhost service execution environment
     * parameter:
     *   userId -- user id
     *   elementName -- the card emulation service name
     * return: 0 -- succeeded, not 0 -- error code
     */
    virtual int UnregisterOffHostService(int userId, const OHOS::AppExecFwk::ElementName& elementName) = 0;
    /**
     * brief: whether the NCI supports prefix mode
     * parameter:
     *   result[out] -- result
     * return: 0 -- succeeded, not 0 -- error code
     */
    virtual int IsSupportedAidPrefixMode(bool&) = 0;
    /**
     * brief: get description for service
     * parameter:
     *   userId -- user id
     *   type -- service type
     *   result[out] -- result
     * return: 0 -- succeeded, not 0 -- error code
     */
    virtual int GetDescriptionForPreferredService(int userId, std::string type, std::string&) = 0;
    /**
     * brief: set preferred foreground service
     * parameter:
     *   userId -- user id
     *   elementName -- the card emulation service name
     * return: 0 -- succeeded, not 0 -- error code
     */
    virtual int SetPreferredForegroundService(int userId, const OHOS::AppExecFwk::ElementName& elementName) = 0;
    /**
     * brief: unset preferred foreground service
     * parameter:
     *   userId -- user id
     * return: 0 -- succeeded, not 0 -- error code
     */
    virtual int UnsetPreferredForegroundService(int userId) = 0;
    /**
     * brief: get primary service by type
     * parameter:
     *   userId -- user id
     *   type -- service type
     *   elementName -- the card emulation service name
     *   result[out] -- result
     * return: 0 -- succeeded, not 0 -- error code
     */
    virtual int GetPrimaryServiceForType(int userId, const std::string& type, OHOS::AppExecFwk::ElementName&) = 0;
    /**
     * brief: set primary service by type
     * parameter:
     *   userId -- user id
     *   elementName -- the card emulation service name
     *   type -- service type
     * return: 0 -- succeeded, not 0 -- error code
     */
    virtual int SetPrimaryServiceForType(int userId,
                                         const OHOS::AppExecFwk::ElementName& elementName,
                                         const std::string& type) = 0;
};
}  // namespace nfc::sdk::cardemulation
}  // namespace OHOS
#endif  // !ICARD_EMULATION_AGENT_H
