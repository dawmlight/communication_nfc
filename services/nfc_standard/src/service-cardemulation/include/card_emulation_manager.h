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

#ifndef CARD_EMULATION_MANAGER_H
#define CARD_EMULATION_MANAGER_H

#include <memory>
#include <string>
#include <vector>

#include "refbase.h"

namespace OHOS {
namespace nfc {
class NfcService;
namespace sdk::cardemulation {
class ICardEmulationAgent;
}  // namespace sdk::cardemulation
namespace cardemulation {
class CardEmulationAgentStub;
class ICardEmulationService;
class IUserSwitchHandler;
class CardEmulationManager final : public std::enable_shared_from_this<CardEmulationManager> {
public:
    CardEmulationManager(CardEmulationManager const&) = delete;
    CardEmulationManager& operator=(CardEmulationManager const&) = delete;
    explicit CardEmulationManager(std::shared_ptr<nfc::NfcService> nfcService);
    ~CardEmulationManager();

    /**
     * brief: initialize card emulation service
     * parameter: void
     * return: 0 - succeeded, not 0 -- error code
     */
    int Init(void);
    /**
     * brief: deinitialize card emulation service
     * parameter: void
     * return: void
     */
    void Deinit(void);
    /**
     * brief: Handle user switchover events
     * parameter: userId - user id
     * return: 0 - succeeded, not 0 -- error code
     */
    int OnUserSwitched(int userId);
    /**
     * brief: get card emulation agent object
     * parameter: void
     * return: card emulation agent object. may be nullptr.
     */
    OHOS::sptr<sdk::cardemulation::ICardEmulationAgent> GetCardEmulationAgent();
    /**
     * brief: handle HCE activated events
     * parameter: void
     * return: void
     */
    void OnHostCardEmulationActivated(int technology);
    /**
     * brief: Handle HCE data transfer events
     * parameter:
     *     technology - card technology
     *     data - Data transmitted by NCI
     * return: void
     */
    void OnHostCardEmulationData(int technology, const std::string& data);
    /**
     * brief: Handle HCE deactivated events
     * parameter:
     *     technology - card technology
     * return: void
     */
    void OnHostCardEmulationDeactivated(int technology);
    /**
     * brief: Handle offHost transaction events
     * parameter: void
     * return: void
     */
    void OnOffHostTransaction(void);
    /**
     * brief: Handle enable nfc events
     * parameter: void
     * return: void
     */
    void OnNfcEnabled();
    /**
     * brief: Handle disable nfc events
     * parameter: void
     * return: void
     */
    void OnNfcDisabled();
    /**
     * brief: Handle toggle secure NFC events
     * parameter: void
     * return: void
     */
    void OnSecureNfcToggled();

protected:
private:
    std::shared_ptr<ICardEmulationService> ceService_{};
    std::vector<std::shared_ptr<IUserSwitchHandler>> userSwitchHandlers_{};
    OHOS::sptr<CardEmulationAgentStub> ceAgent_{};
};
}  // namespace cardemulation
}  // namespace nfc
}  // namespace OHOS
#endif  // CARD_EMULATION_MANAGER_H
