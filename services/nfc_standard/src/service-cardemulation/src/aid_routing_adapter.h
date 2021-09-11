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

#ifndef AID_ROUTING_ADAPTER_H
#define AID_ROUTING_ADAPTER_H

#include <memory>
#include <string>
#include <vector>

#include "icard_emulation_device_host.h"
#include "iexecution_environment_manager.h"

namespace OHOS::nfc::cardemulation {
// TAG(1B) + ROUTE(1B) + LENGTH_BYTE(1B) + POWER(1B)
constexpr int AID_HEAD_LENGTH = 0x04;
constexpr int TECH_PROTO_LENGTH = 0x14;
class AidRoutingAdapter : public IExecutionEnvironmentManager {
public:
    explicit AidRoutingAdapter(const std::shared_ptr<ICardEmulationDeviceHost>& nci);
    virtual ~AidRoutingAdapter();
    int GetDefaultRoute() override;
    int GetDefaultOffHostRoute() override;
    int GetAidRoutingMode();
    int GetAidRoutingTableCapacity();
    int GetRemainRoutingTableSize();

    bool ClearRoutingTable();

    virtual int AddAidRoutingEntry(const std::vector<unsigned char>& aid, int target, int aidType);
    virtual int RemoveAidRoutingEntry(const std::vector<unsigned char>& aid);
    virtual int CommitAidRouting();
    int LocationStringToInt(const std::string& loc) const override;
    std::string ReviseSecureElementName(const std::string& loc) const override;
    bool IsSurpported(const std::string& loc) const override;
    std::vector<std::string> GetSupportedEses();
    std::vector<std::string> GetSupportedUiccs();

    bool IsDefaultRoute(const std::string& location);

private:
    void Init();
    int ValidLocationStringToInt(const std::string& loc, int defaultId) const;

private:
    int defaultRoute_;
    int defaultOffHostRoute_;
    int routingMode_;
    int routingTableCapacity_;
    bool inited_;
    std::weak_ptr<ICardEmulationDeviceHost> nci_;
    std::vector<int> uiccs_;
    std::vector<int> eSes_;
};

}  // namespace OHOS::nfc::cardemulation
#endif  // AID_ROUTING_ADAPTER_H
