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

#ifndef CARD_EMULATION_DEVICE_HOST_H
#define CARD_EMULATION_DEVICE_HOST_H

#include <memory>
#include <string>
#include <vector>

#include "icard_emulation_device_host.h"
namespace OHOS::nfc {
class NfcService;
namespace cardemulation {
class CardEmulationDeviceHost : public ICardEmulationDeviceHost {
public:
    explicit CardEmulationDeviceHost(std::weak_ptr<nfc::NfcService> nfcService);
    ~CardEmulationDeviceHost() override;
    bool AddAidRouting(std::string aid, int route, int aidInfo) override;
    bool RemoveAidRouting(const std::string& aid) override;
    bool ClearRouting() override;
    bool CommitRouting() override;
    int GetRemainRoutingTableSize() override;
    int GetAidRoutingTableSize() override;
    int GetDefaultRoute() override;
    int GetDefaultOffHostRoute() override;
    std::vector<int> GetOffHostUiccRoute() override;
    std::vector<int> GetOffHostEseRoute() override;
    int GetAidMatchingMode() override;
    int GetDefaultIsoDepRouteDestination() override;
    bool GetExtendedLengthApdusSupported() override;
    bool SendData(std::vector<unsigned char> data) override;
    bool DumpRoutingTable(int fd) override;

private:
    std::weak_ptr<nfc::NfcService> nfcService_;
};
}  // namespace cardemulation
}  // namespace OHOS::nfc
#endif  // !CARD_EMULATION_DEVICE_HOST_H
