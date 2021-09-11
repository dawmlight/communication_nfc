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

#ifndef NFC_SDK_OFFHOST_CARD_EMULATION_H
#define NFC_SDK_OFFHOST_CARD_EMULATION_H

#include <memory>
#include <mutex>

#include "element_name.h"
#include "ioffhost_card_emulation.h"
#include "ioffhost_card_emulation_service.h"

namespace OHOS {
namespace nfc::sdk::cardemulation {
class ICardEmulationAgent;
class OffHostCardEmulation : public IOffHostCardEmulation, public IOffHostCardEmulationService {
public:
    ~OffHostCardEmulation() override = default;
    OffHostCardEmulation(const OHOS::AppExecFwk::ElementName& serviceName, const OHOS::wptr<ICardEmulationAgent>& ces);
    bool AddAidsForService(const std::vector<std::string>& aids, std::string cardemulationServiceType) override;
    bool RemoveAidsForService(std::string cardemulationServiceType) override;
    bool RegisterOffHostService(std::string offHostSecureElement) override;
    bool UnregisterOffHostService() override;

private:
    OHOS::AppExecFwk::ElementName GetName() const;

private:
    std::mutex mu_;
    OHOS::wptr<ICardEmulationAgent> ces_;
    OHOS::AppExecFwk::ElementName name_;
};
}  // namespace nfc::sdk::cardemulation
}  // namespace OHOS

#endif  // !NFC_SDK_OFFHOST_CARD_EMULATION_H
