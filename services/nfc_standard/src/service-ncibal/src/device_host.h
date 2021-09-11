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
#ifndef DEVICE_HOST_H
#define DEVICE_HOST_H

#include <memory>
#include <string>

#include "../../service-ncibal/include/idevice_host.h"

namespace OHOS {
namespace nfc {
namespace ncibal {
class ITagEndPoint;
class INfcNci;
class DeviceHost : public IDeviceHost {
public:
    explicit DeviceHost(std::weak_ptr<IDeviceHostListener> listener);
    virtual ~DeviceHost() override;
    virtual void SetDeviceHostListener(std::weak_ptr<IDeviceHostListener> listener) override;
    virtual bool Initialize() override;
    virtual bool Deinitialize() override;
    virtual void EnableDiscovery(int techMask, bool enableReaderMode, bool enableHostRouting, bool restart) override;
    virtual void DisableDiscovery() override;
    virtual bool SendRawFrame(std::string& rawData) override;
    virtual bool SetScreenStatus(unsigned char screenStateMask) override;
    virtual int GetNciVersion() override;
    virtual bool SetSecureNfc(bool secure) override;
    virtual int GetIsoDepMaxTransceiveLength() override;
    virtual int RegisterT3tIdentifier(std::string& lfT3tIdentifier) override;
    virtual void DeregisterT3tIdentifier(std::string& lfT3tIdentifier) override;
    virtual void ClearT3tIdentifiersCache() override;
    virtual int GetLfT3tMax() override;
    virtual int GetLastError() override;
    virtual void Abort() override;
    virtual bool CheckFirmware() override;
    virtual void Dump(int fd) override;
    virtual void FactoryReset() override;
    virtual void Shutdown() override;
    virtual bool AddAidRouting(std::string& aid, int route, int aidInfo) override;
    virtual bool RemoveAidRouting(std::string& aid) override;
    virtual bool CommitRouting() override;
    virtual int GetAidRoutingTableSize() override;
    virtual int GetDefaultRoute() override;
    virtual int GetDefaultOffHostRoute() override;
    virtual std::vector<int> GetOffHostUiccRoute() override;
    virtual std::vector<int> GetOffHostEseRoute() override;
    virtual int GetAidMatchingMode() override;
    virtual int GetDefaultIsoDepRouteDestination() override;
    virtual bool CanMakeReadOnly(int ndefType) override;
    virtual bool GetExtendedLengthApdusSupported() override;
    void SetNfcNciImpl(std::shared_ptr<INfcNci> nfcNciImpl);
    static void RemoteFieldActivated();
    static void RemoteFieldDeactivated();
    static void HostCardEmulationActivated(int technology);
    static void HostCardEmulationDeactivated(int technology);
    static void HostCardEmulationDataReceived(int technology, std::string& data);
    static void TagDiscovered(std::shared_ptr<ITagEndPoint> tagEndPoint);
    static void OffHostTransactionEvent(std::string& aid, std::string& data, std::string& seName);
    static void EeUpdate();
    virtual bool ClearAidTable() override;
    virtual int GetRemainRoutingTableSize() override;

private:
    static std::weak_ptr<IDeviceHostListener> mDeviceHostListener_;
};
}  // namespace ncibal
}  // namespace nfc
}  // namespace OHOS
#endif /* DEVICE_HOST_H */