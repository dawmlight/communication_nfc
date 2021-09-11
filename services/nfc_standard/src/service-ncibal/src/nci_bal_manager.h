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
#ifndef NCI_BAL_MANAGER_H
#define NCI_BAL_MANAGER_H

#include <memory>
#include <mutex>

#include "nfa_api.h"

namespace OHOS::nfc {
class SynchronizeEvent;
}

namespace OHOS {
namespace nfc {
namespace ncibal {
class INfcNci;
class NciBalManager final {
public:
    static NciBalManager& GetInstance();
    static int GetIsoDepMaxTransceiveLength();
    static void ClearT3tIdentifiersCache();
    static int GetLfT3tMax();
    static int GetLastError();
    static void Abort();
    static bool IsNfcActive();
    bool Initialize();
    bool Deinitialize();
    void EnableDiscovery(int techMask, bool enableReaderMode, bool enableHostRouting, bool restart);
    void DisableDiscovery();
    bool SendRawFrame(std::string& rawData);
    void SetScreenStatus(unsigned char screenStateMask) const;
    int GetNciVersion() const;
    bool RegisterT3tIdentifier(const std::string& lfT3tIdentifier) const;
    void DeregisterT3tIdentifier(int handle) const;
    bool CheckFirmware();
    void Dump(int fd) const;
    void FactoryReset() const;
    void Shutdown() const;
    bool IsTagActive() const;
    void SetNfcNciImpl(std::shared_ptr<INfcNci> nfcNciImpl);
    void StartRfDiscovery(bool isStart) const;
    bool IsRfEbabled();

private:
    static const tNFA_TECHNOLOGY_MASK DEFAULT_TECH_MASK =
        (NFA_TECHNOLOGY_MASK_A | NFA_TECHNOLOGY_MASK_B | NFA_TECHNOLOGY_MASK_F | NFA_TECHNOLOGY_MASK_V);
    static const int DEFAULT_DISCOVERY_DURATION = 500;
    static const int DISCOVERY_DURATION = 200;
    static const int NFA_SCREEN_POLLING_TAG_MASK = 0x10;
    NciBalManager();
    ~NciBalManager();
    tNFA_STATUS StartPolling(tNFA_TECHNOLOGY_MASK techMask) const;
    tNFA_STATUS StopPolling() const;
    static void NfcConnectionCallback(uint8_t connEvent, tNFA_CONN_EVT_DATA* eventData);
    static void NfcDeviceManagementCallback(uint8_t dmEvent, tNFA_DM_CBACK_DATA* eventData);
    std::mutex mMutex_{};
    static OHOS::nfc::SynchronizeEvent mNfcEnableEvent_;   // event for NFA_Enable()
    static OHOS::nfc::SynchronizeEvent mNfcDisableEvent_;  // event for NFA_Disable()
    static OHOS::nfc::SynchronizeEvent mPollingEvent_;     // event for polling
    static OHOS::nfc::SynchronizeEvent setScreenStateEvent_;
    static OHOS::nfc::SynchronizeEvent setConfigEvent_;
    static bool mIsNfcEnabled_;
    static bool mRfEnabled_;
    static bool mDiscoveryEnabled_;  // is polling or listening
    static bool mPollingEnabled_;    // is polling for tag
    static bool mIsDisabling_;
    static bool mReaderModeEnabled_;
    static unsigned long mDiscoveryDuration_;
    static bool mIsReconnect_;
    static bool mIsTagActive_;
    static unsigned char curScreenState_;
    static std::shared_ptr<INfcNci> mNfcNciImpl_;
};
}  // namespace ncibal
}  // namespace nfc
}  // namespace OHOS
#endif /* NCI_BAL_MANAGER_H */