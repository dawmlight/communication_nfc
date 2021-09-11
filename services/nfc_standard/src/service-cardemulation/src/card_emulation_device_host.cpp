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

#include "card_emulation_device_host.h"

#include "card_emulation_error.h"
#include "nfc_service.h"
namespace OHOS::nfc::cardemulation {
CardEmulationDeviceHost::CardEmulationDeviceHost(std::weak_ptr<nfc::NfcService> nfcService)
    : nfcService_(std::move(nfcService))
{
}
CardEmulationDeviceHost::~CardEmulationDeviceHost(){};
bool CardEmulationDeviceHost::AddAidRouting(std::string aid, int route, int aidInfo)
{
    auto s = nfcService_.lock();
    if (!s) {
        return false;
    }
    return s->AddAidRouting(aid, route, aidInfo);
}
bool CardEmulationDeviceHost::RemoveAidRouting(const std::string& aid)
{
    auto s = nfcService_.lock();
    if (!s) {
        return false;
    }
    std::string t(aid);
    return s->RemoveAidRouting(t);
}
bool CardEmulationDeviceHost::ClearRouting()
{
    auto s = nfcService_.lock();
    if (!s) {
        return false;
    }
    return s->ClearRouting();
}

bool CardEmulationDeviceHost::CommitRouting()
{
    auto s = nfcService_.lock();
    if (!s) {
        return false;
    }
    return s->CommitRouting();
}
int CardEmulationDeviceHost::GetRemainRoutingTableSize()
{
    auto s = nfcService_.lock();
    if (!s) {
        return 0;
    }
    return s->GetRemainRoutingTableSize();
}

int CardEmulationDeviceHost::GetAidRoutingTableSize()
{
    auto s = nfcService_.lock();
    if (!s) {
        return 0;
    }
    return s->GetAidRoutingTableSize();
}
int CardEmulationDeviceHost::GetDefaultRoute()
{
    auto s = nfcService_.lock();
    if (!s) {
        return 0;
    }
    return s->GetDefaultRoute();
}
int CardEmulationDeviceHost::GetDefaultOffHostRoute()
{
    auto s = nfcService_.lock();
    if (!s) {
        return 0;
    }
    return s->GetDefaultOffHostRoute();
}
std::vector<int> CardEmulationDeviceHost::GetOffHostUiccRoute()
{
    auto s = nfcService_.lock();
    if (!s) {
        return {};
    }
    return s->GetOffHostUiccRoute();
}
std::vector<int> CardEmulationDeviceHost::GetOffHostEseRoute()
{
    auto s = nfcService_.lock();
    if (!s) {
        return {};
    }
    return s->GetOffHostEseRoute();
}
int CardEmulationDeviceHost::GetAidMatchingMode()
{
    auto s = nfcService_.lock();
    if (!s) {
        return 0;
    }
    return s->GetAidMatchingMode();
}
int CardEmulationDeviceHost::GetDefaultIsoDepRouteDestination()
{
    auto s = nfcService_.lock();
    if (!s) {
        return 0;
    }
    return s->GetDefaultIsoDepRouteDestination();
}
bool CardEmulationDeviceHost::GetExtendedLengthApdusSupported()
{
    auto s = nfcService_.lock();
    if (!s) {
        return false;
    }
    return s->GetExtendedLengthApdusSupported();
}
bool CardEmulationDeviceHost::SendData(std::vector<unsigned char> data)
{
    auto s = nfcService_.lock();
    if (!s) {
        return false;
    }
    return s->SendData(std::move(data));
}
bool CardEmulationDeviceHost::DumpRoutingTable(int fd)
{
    auto s = nfcService_.lock();
    if (!s) {
        return false;
    }
    return s->DumpRoutingTable(fd);
}
}  // namespace OHOS::nfc::cardemulation
