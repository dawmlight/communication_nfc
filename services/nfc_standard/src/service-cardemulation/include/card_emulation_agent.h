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

#ifndef CARD_EMULATION_AGENT_H
#define CARD_EMULATION_AGENT_H

#include <memory>

#include "card_emulation_agent_stub.h"

namespace OHOS {
namespace nfc {
namespace skd::cardemulation {
class CardEmulationServiceInfoLite;
}
namespace cardemulation {
class ICardEmulationService;
class CardEmulationAgent : public CardEmulationAgentStub {
public:
    /**
     * brief: constructor
     * parameter:
     *   ceService -- ICardEmulationService instance
     * return: CardEmulationAgent instance
     */
    explicit CardEmulationAgent(std::shared_ptr<nfc::cardemulation::ICardEmulationService> ceService);
    /**
     * brief: destructor
     * parameter: void
     * return: void
     */
    ~CardEmulationAgent() override;
    /**
     * brief: get the card emulation service infomations
     * parameter:
     *   userId -- user id
     *   type -- service type @see card_emulation_def.h CARDEMULATION_SERVICE_TYPE_*
     *   result[out] -- result
     * return: 0 -- succeeded, not 0 -- error code
     */
    int GetServices(
        int userId,
        const std::string& type,
        std::vector<std::unique_ptr<OHOS::nfc::sdk::cardemulation::CardEmulationServiceInfoLite>>&) override;
    /**
     * brief: get aids for service
     * parameter:
     *   userId -- user id
     *   elementName -- the card emulation service name
     *   type -- service type
     *   result[out] -- result
     * return: 0 -- succeeded, not 0 -- error code
     */
    int GetAidsForService(int userId,
                          const OHOS::AppExecFwk::ElementName& elementName,
                          const std::string& type,
                          std::vector<std::string>& result) override;
    /**
     * brief: register remote object
     * parameter:
     *   userId -- user id
     *   elementName -- the card emulation service name
     *   apduChannel -- apdu channel object
     * return: 0 -- succeeded, not 0 -- error code
     */
    int RegisterRemoteObject(int userId,
                             const OHOS::AppExecFwk::ElementName& elementName,
                             const OHOS::sptr<OHOS::IRemoteObject>& apduChannel) override;
    /**
     * brief: add aids for service
     * parameter:
     *   userId -- user id
     *   elementName -- the card emulation service name
     *   aids -- aids
     *   type -- service type
     * return: 0 -- succeeded, not 0 -- error code
     */
    int AddAidsForService(int userId,
                          const OHOS::AppExecFwk::ElementName& elementName,
                          std::vector<std::string> aids,
                          std::string type) override;
    /**
     * brief: remove aids for service
     * parameter:
     *   userId -- user id
     *   elementName -- the card emulation service name
     *   type -- service type
     * return: 0 -- succeeded, not 0 -- error code
     */
    int RemoveAidsForService(int userId, const OHOS::AppExecFwk::ElementName& elementName, std::string type) override;
    /**
     * brief: register offhost service execution environment
     * parameter:
     *   userId -- user id
     *   elementName -- the card emulation service name
     *   offHostSecureElement -- secure element name (execution environment)
     * return: 0 -- succeeded, not 0 -- error code
     */
    int RegisterOffHostService(int userId,
                               const OHOS::AppExecFwk::ElementName& elementName,
                               std::string offHostSecureElement) override;
    /**
     * brief: unregister offhost service execution environment
     * parameter:
     *   userId -- user id
     *   elementName -- the card emulation service name
     * return: 0 -- succeeded, not 0 -- error code
     */
    int UnregisterOffHostService(int userId, const OHOS::AppExecFwk::ElementName& elementName) override;
    /**
     * brief: whether the NCI supports prefix mode
     * parameter:
     *   result[out] -- result
     * return: 0 -- succeeded, not 0 -- error code
     */
    int IsSupportedAidPrefixMode(bool& result) override;
    /**
     * brief: get description for service
     * parameter:
     *   userId -- user id
     *   type -- service type
     *   result[out] -- result
     * return: 0 -- succeeded, not 0 -- error code
     */
    int GetDescriptionForPreferredService(int userId, std::string type, std::string& result) override;
    /**
     * brief: set preferred foreground service
     * parameter:
     *   userId -- user id
     *   elementName -- the card emulation service name
     * return: 0 -- succeeded, not 0 -- error code
     */
    int SetPreferredForegroundService(int userId, const OHOS::AppExecFwk::ElementName& elementName) override;
    /**
     * brief: unset preferred foreground service
     * parameter:
     *   userId -- user id
     * return: 0 -- succeeded, not 0 -- error code
     */
    int UnsetPreferredForegroundService(int userId) override;
    /**
     * brief: get primary service by type
     * parameter:
     *   userId -- user id
     *   type -- service type
     *   elementName -- the card emulation service name
     *   result[out] -- result
     * return: 0 -- succeeded, not 0 -- error code
     */
    int GetPrimaryServiceForType(int userId, const std::string& type, OHOS::AppExecFwk::ElementName& result) override;
    /**
     * brief: set primary service by type
     * parameter:
     *   userId -- user id
     *   elementName -- the card emulation service name
     *   type -- service type
     * return: 0 -- succeeded, not 0 -- error code
     */
    int SetPrimaryServiceForType(int userId,
                                 const OHOS::AppExecFwk::ElementName& elementName,
                                 const std::string& type) override;

private:
    std::shared_ptr<ICardEmulationService> ceService_;
};
}  // namespace cardemulation
}  // namespace nfc
}  // namespace OHOS
#endif  // !CARD_EMULATION_AGENT_H
