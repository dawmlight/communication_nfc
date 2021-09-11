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

#ifndef HOST_CARD_EMULATION_H
#define HOST_CARD_EMULATION_H

#include <mutex>

#include "ihost_card_emulation.h"
#include "ihost_card_emulation_service.h"

namespace OHOS {
namespace nfc::sdk::cardemulation {
class ICardEmulationAgent;
class HostCardEmulation : public IHostCardEmulation, public IHostCardEmulationService {
public:
    HostCardEmulation(const OHOS::AppExecFwk::ElementName& serviceName, const OHOS::wptr<ICardEmulationAgent>& ces);
    ~HostCardEmulation() override;
    bool AddAidsForService(const std::vector<std::string>& aids, std::string cardemulationServiceType) override;
    bool RemoveAidsForService(std::string cardemulationServiceType) override;
    bool SetApduCommandCallback(ApduCommandHandler handler) override;
    bool SetDeactiveEventCallback(DeactiveEventHandler handler) override;

protected:
    std::vector<unsigned char> HandleApdu(const std::vector<unsigned char>&) override;
    void OnDeactivated(int reason) override;

private:
    OHOS::AppExecFwk::ElementName GetName() const;
    bool RegisterRemoteObject();
    bool RegisterRemoteObject(const OHOS::sptr<ICardEmulationAgent>& ces, int userId);

private:
    std::mutex mu_;
    ApduCommandHandler apduCommandHandler_;
    DeactiveEventHandler deactiveEventHandler_;
    OHOS::wptr<ICardEmulationAgent> ces_;
    OHOS::AppExecFwk::ElementName name_;
    OHOS::wptr<IRemoteObject> channel_;
};
}  // namespace nfc::sdk::cardemulation
}  // namespace OHOS
#endif  // !HOST_CARD_EMULATION_H
