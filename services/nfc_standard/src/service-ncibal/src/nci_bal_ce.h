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
#ifndef NCI_BAL_CE_H
#define NCI_BAL_CE_H

#include <mutex>
#include <string>
#include <vector>

#include "nfa_ee_api.h"

namespace OHOS::nfc {
class SynchronizeEvent;
}

namespace OHOS {
namespace nfc {
namespace ncibal {
class INfcNci;
class NciBalCe final {
public:
    static NciBalCe& GetInstance();
    bool InitializeCe();
    bool AddAidRouting(std::string& aid, int route, int aidInfo);
    bool RemoveAidRouting(std::string& aid);
    bool CommitRouting();
    int GetAidRoutingTableSize();
    void EnableHostRouting(bool enable);
    bool SetSecureNfc(bool secure);
    int GetDefaultRoute() const;
    int GetDefaultOffHostRoute() const;
    std::vector<int> GetOffHostUiccRoute() const;
    std::vector<int> GetOffHostEseRoute() const;
    int GetAidMatchingMode() const;
    int GetDefaultIsoDepRouteDestination() const;
    void AbortWait();
    void Deinitialize();
    void SetNfcNciImpl(std::shared_ptr<INfcNci> nfcNciImpl);
    bool ClearAidTable();
    int GetRemainRoutingTableSize();

private:
    static const int AID_ROUTE_EXACT_ONLY = 0x00;
    static const int AID_ROUTE_EXACT_OR_PREFIX = 0x01;
    static const int AID_ROUTE_PREFIX_ONLY = 0x02;
    static const int AID_ROUTE_EXACT_OR_SUBSET_OR_PREFIX = 0x03;
    static const int AID_ROUTE_QUAL_PREFIX = 0x10;

    NciBalCe();
    ~NciBalCe();
    tNFA_TECHNOLOGY_MASK UpdateEeTech() const;
    void UpdateDefaultRoute();
    void UpdateEeProto();
    void EnableRoutingToHost();
    void DisableRoutingToHost();
    void DeactiveEe();
    static void NfcEeCallback(tNFA_EE_EVT event, tNFA_EE_CBACK_DATA* eventData);
    static void NfcCeCallback(uint8_t event, tNFA_CONN_EVT_DATA* eventData);
    static void HandleHostCardEmulationData(uint8_t technology,
                                            const uint8_t* data,
                                            uint32_t dataLen,
                                            tNFA_STATUS status);
    static OHOS::nfc::SynchronizeEvent mEeRegisterEvent_;
    static OHOS::nfc::SynchronizeEvent mAidEvent_;
    static OHOS::nfc::SynchronizeEvent mRoutingCfgEvent_;
    static OHOS::nfc::SynchronizeEvent mCommitRoutingEvent_;
    static OHOS::nfc::SynchronizeEvent mModeSetEvent_;
    static OHOS::nfc::SynchronizeEvent remainSizeEvent_;
    static bool mAidRoutingConfigured_;
    static tNFA_EE_DISCOVER_REQ mEeInfo_;
    static std::string mHceData_;
    static std::shared_ptr<INfcNci> mNfcNciImpl_;
    static int remainTableSize_;
    bool mNfcSecure_;
    tNFA_HANDLE mDefaultRoute_;
    tNFA_HANDLE mDefaultOffHostRoute_;
    std::vector<uint8_t> mDefaultOffHostUiccRoute_{};
    std::vector<uint8_t> mDefaultOffHostEseRoute_{};
    tNFA_HANDLE mDefaultIsoDepRoute_;
    int mAidMatchingMode_;
    unsigned char mOffHostAidRoutingPowerState_;
    tNFA_TECHNOLOGY_MASK mHostListenTech_;
    tNFA_TECHNOLOGY_MASK mSeTech_;
};
}  // namespace ncibal
}  // namespace nfc
}  // namespace OHOS
#endif /* NCI_BAL_CE_H */
