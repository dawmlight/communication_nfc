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
#include "device_host.h"

#include "idevice_host.h"
#include "loghelper.h"
#ifdef _NFC_SERVICE_HCE_
#include "hci_manager.h"
#include "nci_bal_ce.h"
#endif
#include "nci_bal_manager.h"
#include "nci_bal_tag.h"
#include "tag_end_point.h"

namespace OHOS {
namespace nfc {
namespace ncibal {
std::weak_ptr<DeviceHost::IDeviceHostListener> DeviceHost::mDeviceHostListener_;

DeviceHost::DeviceHost(std::weak_ptr<IDeviceHostListener> listener)
{
    mDeviceHostListener_ = listener;
}

void DeviceHost::SetDeviceHostListener(std::weak_ptr<IDeviceHostListener> listener)
{
    mDeviceHostListener_ = listener;
}

DeviceHost::~DeviceHost()
{
    this->Deinitialize();
}

bool DeviceHost::Initialize()
{
    DebugLog("DeviceHost::Initialize");
    return NciBalManager::GetInstance().Initialize();
}

bool DeviceHost::Deinitialize()
{
    DebugLog("DeviceHost::Deinitialize");
    return NciBalManager::GetInstance().Deinitialize();
}

void DeviceHost::EnableDiscovery(int techMask, bool enableReaderMode, bool enableHostRouting, bool restart)
{
    DebugLog("DeviceHost::EnableDiscovery");
    NciBalManager::GetInstance().EnableDiscovery(techMask, enableReaderMode, enableHostRouting, restart);
}

void DeviceHost::DisableDiscovery()
{
    DebugLog("DeviceHost::DisableDiscovery");
    NciBalManager::GetInstance().DisableDiscovery();
}

bool DeviceHost::SendRawFrame(std::string& rawData)
{
    DebugLog("DeviceHost::SendRawFrame");
    return NciBalManager::GetInstance().SendRawFrame(rawData);
}

bool DeviceHost::SetScreenStatus(unsigned char screenStateMask)
{
    DebugLog("DeviceHost::SetScreenStatus");
    NciBalManager::GetInstance().SetScreenStatus(screenStateMask);
    return true;
}

int DeviceHost::GetNciVersion()
{
    DebugLog("DeviceHost::GetNciVersion");
    return NciBalManager::GetInstance().GetNciVersion();
}

bool DeviceHost::SetSecureNfc(bool secure)
{
    DebugLog("DeviceHost::SetSecureNfc");
#ifdef _NFC_SERVICE_HCE_
    NciBalCe::GetInstance().SetSecureNfc(secure);
#endif
    return true;
}

int DeviceHost::GetIsoDepMaxTransceiveLength()
{
    DebugLog("DeviceHost::GetIsoDepMaxTransceiveLength");
    return NciBalManager::GetInstance().GetIsoDepMaxTransceiveLength();
}

int DeviceHost::RegisterT3tIdentifier(std::string& lfT3tIdentifier)
{
    DebugLog("DeviceHost::RegisterT3tIdentifier");
    return NciBalManager::GetInstance().RegisterT3tIdentifier(lfT3tIdentifier);
}

void DeviceHost::DeregisterT3tIdentifier(std::string& lfT3tIdentifier)
{
    DebugLog("DeviceHost::DeregisterT3tIdentifier");
    // get handle from mT3tIdentifiers
    if (!lfT3tIdentifier.empty()) {
        int handle = -1;
        NciBalManager::GetInstance().DeregisterT3tIdentifier(handle);
    }
}

void DeviceHost::ClearT3tIdentifiersCache()
{
    DebugLog("DeviceHost::ClearT3tIdentifiersCache");
    NciBalManager::GetInstance().ClearT3tIdentifiersCache();
}

int DeviceHost::GetLfT3tMax()
{
    DebugLog("DeviceHost::GetLfT3tMax");
    return NciBalManager::GetInstance().GetLfT3tMax();
}

int DeviceHost::GetLastError()
{
    DebugLog("DeviceHost::GetLastError");
    return NciBalManager::GetInstance().GetLastError();
}

void DeviceHost::Abort()
{
    DebugLog("DeviceHost::Abort");
    NciBalManager::GetInstance().Abort();
}

bool DeviceHost::CheckFirmware()
{
    DebugLog("DeviceHost::CheckFirmware");
    return NciBalManager::GetInstance().CheckFirmware();
}

void DeviceHost::Dump(int fd)
{
    DebugLog("DeviceHost::Dump");
    NciBalManager::GetInstance().Dump(fd);
}

void DeviceHost::FactoryReset()
{
    DebugLog("DeviceHost::FactoryReset");
    NciBalManager::GetInstance().FactoryReset();
}

void DeviceHost::Shutdown()
{
    DebugLog("DeviceHost::Shutdown");
    NciBalManager::GetInstance().Shutdown();
}

bool DeviceHost::AddAidRouting(std::string& aid, int route, int aidInfo)
{
    DebugLog("DeviceHost::AddAidRouting");
#ifdef _NFC_SERVICE_HCE_
    return NciBalCe::GetInstance().AddAidRouting(aid, route, aidInfo);
#else
    return true;
#endif
}

bool DeviceHost::RemoveAidRouting(std::string& aid)
{
    DebugLog("DeviceHost::RemoveAidRouting");
#ifdef _NFC_SERVICE_HCE_
    return NciBalCe::GetInstance().RemoveAidRouting(aid);
#else
    return true;
#endif
}

bool DeviceHost::CommitRouting()
{
    DebugLog("DeviceHost::CommitRouting");
#ifdef _NFC_SERVICE_HCE_
    bool restart = NciBalManager::GetInstance().IsRfEbabled();
    if (restart) {
        NciBalManager::GetInstance().StartRfDiscovery(false);
    }
    bool commitResult = NciBalCe::GetInstance().CommitRouting();
    if (restart) {
        NciBalManager::GetInstance().StartRfDiscovery(true);
    }
    return commitResult;
#else
    return true;
#endif
}

int DeviceHost::GetAidRoutingTableSize()
{
    DebugLog("DeviceHost::GetAidRoutingTableSize");
#ifdef _NFC_SERVICE_HCE_
    return NciBalCe::GetInstance().GetAidRoutingTableSize();
#endif
    return 0;
}

int DeviceHost::GetDefaultRoute()
{
    DebugLog("DeviceHost::GetDefaultRoute");
#ifdef _NFC_SERVICE_HCE_
    return NciBalCe::GetInstance().GetDefaultRoute();
#endif
    return 0;
}

int DeviceHost::GetDefaultOffHostRoute()
{
    DebugLog("DeviceHost::GetDefaultOffHostRoute");
#ifdef _NFC_SERVICE_HCE_
    return NciBalCe::GetInstance().GetDefaultOffHostRoute();
#endif
    return 0;
}

std::vector<int> DeviceHost::GetOffHostUiccRoute()
{
    DebugLog("DeviceHost::GetOffHostUiccRoute");
#ifdef _NFC_SERVICE_HCE_
    return NciBalCe::GetInstance().GetOffHostUiccRoute();
#endif
    return {};
}

std::vector<int> DeviceHost::GetOffHostEseRoute()
{
    DebugLog("DeviceHost::GetOffHostEseRoute");
#ifdef _NFC_SERVICE_HCE_
    return NciBalCe::GetInstance().GetOffHostEseRoute();
#else
    return {};
#endif
}

int DeviceHost::GetAidMatchingMode()
{
    DebugLog("DeviceHost::GetAidMatchingMode");
#ifdef _NFC_SERVICE_HCE_
    return NciBalCe::GetInstance().GetAidMatchingMode();
#else
    return 0;
#endif
}

int DeviceHost::GetDefaultIsoDepRouteDestination()
{
    DebugLog("DeviceHost::GetDefaultIsoDepRouteDestination");
#ifdef _NFC_SERVICE_HCE_
    return NciBalCe::GetInstance().GetDefaultIsoDepRouteDestination();
#else
    return 0;
#endif
}

bool DeviceHost::CanMakeReadOnly(int ndefType)
{
    return (ndefType == NDEF_TYPE1_TAG || ndefType == NDEF_TYPE2_TAG);
}

bool DeviceHost::GetExtendedLengthApdusSupported()
{
    if (NciBalManager::GetInstance().GetIsoDepMaxTransceiveLength() > 261) {
        return true;
    }
    return false;
}

void DeviceHost::SetNfcNciImpl(std::shared_ptr<INfcNci> nfcNciImpl)
{
    NciBalManager::GetInstance().SetNfcNciImpl(nfcNciImpl);
    NciBalTag::GetInstance().SetNfcNciImpl(nfcNciImpl);
#ifdef _NFC_SERVICE_HCE_
    NciBalCe::GetInstance().SetNfcNciImpl(nfcNciImpl);
    HciManager::GetInstance().SetNfcNciImpl(nfcNciImpl);
#endif
}

void DeviceHost::RemoteFieldActivated()
{
    DebugLog("DeviceHost::RemoteFieldActivated");
    if (mDeviceHostListener_.expired()) {
        ErrorLog("Device host listener is null");
        return;
    }
    mDeviceHostListener_.lock()->OnRemoteFieldActivated();
}

void DeviceHost::RemoteFieldDeactivated()
{
    DebugLog("DeviceHost::RemoteFieldDeactivated");
    if (mDeviceHostListener_.expired()) {
        ErrorLog("Device host listener is null");
        return;
    }
    mDeviceHostListener_.lock()->OnRemoteFieldDeactivated();
}

void DeviceHost::HostCardEmulationActivated(int technology)
{
    DebugLog("DeviceHost::HostCardEmulationActivated");
    if (mDeviceHostListener_.expired()) {
        ErrorLog("Device host listener is null");
        return;
    }
    mDeviceHostListener_.lock()->OnHostCardEmulationActivated(technology);
}

void DeviceHost::HostCardEmulationDeactivated(int technology)
{
    DebugLog("DeviceHost::HostCardEmulationDeactivated");
    if (mDeviceHostListener_.expired()) {
        ErrorLog("Device host listener is null");
        return;
    }
    mDeviceHostListener_.lock()->OnHostCardEmulationDeactivated(technology);
}

void DeviceHost::HostCardEmulationDataReceived(int technology, std::string& data)
{
    DebugLog("DeviceHost::HostCardEmulationDataReceived");
    if (mDeviceHostListener_.expired()) {
        ErrorLog("Device host listener is null");
        return;
    }
    mDeviceHostListener_.lock()->OnHostCardEmulationDataReceived(technology, data);
}

void DeviceHost::TagDiscovered(std::shared_ptr<ITagEndPoint> tagEndPoint)
{
    DebugLog("DeviceHost::TagDiscovered");
    if (mDeviceHostListener_.expired()) {
        ErrorLog("Device host listener is null");
        return;
    }
    mDeviceHostListener_.lock()->OnTagDiscovered(tagEndPoint);
}

void DeviceHost::OffHostTransactionEvent(std::string& aid, std::string& data, std::string& seName)
{
    DebugLog("DeviceHost::OffHostTransactionEvent");
    if (mDeviceHostListener_.expired()) {
        ErrorLog("Device host listener is null");
        return;
    }
    mDeviceHostListener_.lock()->OnOffHostTransactionEvent(aid, data, seName);
}

void DeviceHost::EeUpdate()
{
    DebugLog("DeviceHost::EeUpdate");
    if (mDeviceHostListener_.expired()) {
        ErrorLog("Device host listener is null");
        return;
    }
    mDeviceHostListener_.lock()->OnEeUpdate();
}

bool DeviceHost::ClearAidTable()
{
    DebugLog("DeviceHost::ClearAidTable");
#ifdef _NFC_SERVICE_HCE_
    return NciBalCe::GetInstance().ClearAidTable();
#else
    return true;
#endif
}

int DeviceHost::GetRemainRoutingTableSize()
{
    DebugLog("DeviceHost::GetRemainRoutingTableSize");
#ifdef _NFC_SERVICE_HCE_
    return NciBalCe::GetInstance().GetRemainRoutingTableSize();
#endif
    return 0;
}
}  // namespace ncibal
}  // namespace nfc
}  // namespace OHOS