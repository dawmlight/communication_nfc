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

#ifndef IOFFHOST_CARD_EMULATION_H
#define IOFFHOST_CARD_EMULATION_H

#include <string>
#include <vector>

namespace OHOS {
namespace nfc::sdk::cardemulation {
class IOffHostCardEmulation {
public:
    virtual ~IOffHostCardEmulation() {}
    virtual bool AddAidsForService(const std::vector<std::string>& aids, std::string cardemulationServiceType) = 0;
    virtual bool RemoveAidsForService(std::string cardemulationServiceType) = 0;
    virtual bool RegisterOffHostService(std::string offHostSecureElement) = 0;
    virtual bool UnregisterOffHostService() = 0;
};
}  // namespace nfc::sdk::cardemulation
}  // namespace OHOS

#endif  // !IOFFHOST_CARD_EMULATION_H
