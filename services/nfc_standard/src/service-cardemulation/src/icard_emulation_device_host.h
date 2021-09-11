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

#ifndef ICARD_EMULATION_DEVICE_HOST_H
#define ICARD_EMULATION_DEVICE_HOST_H

#include <string>
#include <vector>

namespace OHOS::nfc::cardemulation {
class ICardEmulationDeviceHost {
public:
    virtual ~ICardEmulationDeviceHost() = default;
    virtual bool AddAidRouting(std::string aid, int route, int aidInfo) = 0;
    virtual bool RemoveAidRouting(const std::string& aid) = 0;
    virtual bool ClearRouting() = 0;
    virtual bool CommitRouting() = 0;
    virtual int GetRemainRoutingTableSize() = 0;
    virtual int GetAidRoutingTableSize() = 0;
    virtual int GetDefaultRoute() = 0;
    virtual int GetDefaultOffHostRoute() = 0;
    virtual std::vector<int> GetOffHostUiccRoute() = 0;
    virtual std::vector<int> GetOffHostEseRoute() = 0;
    virtual int GetAidMatchingMode() = 0;
    virtual int GetDefaultIsoDepRouteDestination() = 0;
    virtual bool GetExtendedLengthApdusSupported() = 0;
    virtual bool SendData(std::vector<unsigned char> data) = 0;
    virtual bool DumpRoutingTable(int fd) = 0;
};

}  // namespace OHOS::nfc::cardemulation

#endif  // !ICARD_EMULATION_DEVICE_HOST_H
