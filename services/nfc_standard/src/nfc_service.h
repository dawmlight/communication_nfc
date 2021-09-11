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
#ifndef NFC_SERVICE_H
#define NFC_SERVICE_H

#include <atomic>
#include <condition_variable>
#include <future>
#include <map>
#include <mutex>
#include <string>
#include <vector>

#include "idevice_host.h"
#include "infc_service.h"
#include "refbase.h"

#ifdef _NFC_SERVICE_HCE_
#include "icard_emulation_device_host.h"
#endif

namespace osal {
class Context;
class SharedPreferences;
class IBinder;
class KeyguardManager;
class PowerManager;
}  // namespace osal

namespace OHOS {
namespace AAFwk {
class Want;
}
namespace AppExecFwk {
class EventHandler;
}
namespace EventFwk {
class CommonEventSubscriber;
}
namespace nfc::ncibal {
class IDeviceHost;
class ITagEndPoint;
class DeviceHost;
}  // namespace nfc::ncibal
using nfc::ncibal::DeviceHost;
using nfc::ncibal::IDeviceHost;
using nfc::ncibal::ITagEndPoint;

#ifdef _NFC_SERVICE_HCE_
namespace nfc::cardemulation {
class CardEmulationManager;
}  // namespace nfc::cardemulation
namespace nfc::sdk::cardemulation {
class ICardEmulationAgent;
#ifdef _NFC_F_CE_
class INfcFCardEmulation;
#endif
}  // namespace nfc::sdk::cardemulation
#endif

namespace reader {
#ifdef _NFC_SERVICE_READER_
class TagSession;
class TagDispatcher;
#endif  // _NFC_SERVICE_READER_
}  // namespace reader

namespace nfc {
class NfcAgentService;
class NfcServiceHandler;
class NfcDiscoveryParams;
class ScreenStateHelper;
class ForegroundUtils;
class INfcUnlockHandler;
class INfcAgentService;

struct RouteAidsEntry {
public:
    std::string aid_{""};
    int route_{0};
    int aidInfo_{0};
};

static const int NCI_VERSION_2_0 = 0x20;
static const int NCI_VERSION_1_0 = 0x10;

static const std::string ACTION_ADAPTER_STATE_CHANGED = "ohos.nfc.action.ADAPTER_STATE_CHANGED";
static const std::string EXTRA_ADAPTER_STATE = "ohos.nfc.extra.ADAPTER_STATE";
class NfcService final : public IDeviceHost::IDeviceHostListener,
#ifdef _NFC_SERVICE_HCE_
                         public OHOS::nfc::cardemulation::ICardEmulationDeviceHost,
#endif
                         public INfcService,
                         public std::enable_shared_from_this<NfcService> {
public:
    explicit NfcService(std::weak_ptr<osal::Context> context, std::unique_ptr<IDeviceHost> deviceHost = nullptr);
    ~NfcService() override;
    NfcService& operator=(const NfcService&) = delete;
    NfcService(const NfcService&) = delete;

    bool Initialize();

    // NCI CallBack
    void OnRemoteFieldActivated() override;
    void OnRemoteFieldDeactivated() override;
    void OnHostCardEmulationActivated(int tech) override;
    void OnHostCardEmulationDeactivated(int tech) override;
    void OnHostCardEmulationDataReceived(int tech, std::string& data) override;
    void OnOffHostTransactionEvent(std::string& aid, std::string& data, std::string& seName) override;
    void OnEeUpdate() override;
#ifdef _NFC_SERVICE_READER_
    void OnTagDiscovered(std::shared_ptr<ITagEndPoint> tagEndPoint) override;
#endif

    // HCE
#ifdef _NFC_SERVICE_HCE_
    int GetAidRoutingTableSize() override;
    int GetDefaultRoute() override;
    int GetDefaultOffHostRoute() override;
    std::vector<int> GetOffHostUiccRoute() override;
    std::vector<int> GetOffHostEseRoute() override;
    int GetAidMatchingMode() override;
    int GetDefaultIsoDepRouteDestination() override;
    bool GetExtendedLengthApdusSupported() override;
    bool AddAidRouting(std::string aid, int route, int aidInfo) override;
    bool RemoveAidRouting(const std::string& aid) override;
    bool ClearRouting() override;
    bool CommitRouting() override;
    int GetRemainRoutingTableSize() override;

    void RegisterT3tIdentifier(std::string& systemCode, std::string& nfcId2, std::string& t3tPmm);
    void DeregisterT3tIdentifier(std::string& systemCode, std::string& nfcId2, std::string& t3tPmm);
    void ClearT3tIdentifiersCache();
    int GetLfT3tMax();
    bool SendData(std::vector<unsigned char> data) override;
    bool DumpRoutingTable(int fd) override;
    std::vector<std::string> GetSEAccessAllowedPackages();
    void SendNfcEeAccessProtectedBroadcast(std::shared_ptr<AAFwk::Want> intent);
    void SendOffHostTransactionEvent(std::string aid, std::string data, std::string readerByteArray);
    bool IsSEServiceAvailable();
    void SendPreferredPaymentChangedEvent(std::shared_ptr<AAFwk::Want> intent);

    OHOS::sptr<OHOS::nfc::sdk::cardemulation::ICardEmulationAgent> GetNfcCardEmulationInterface();
#ifdef _NFC_F_CE_
    std::shared_ptr<OHOS::nfc::sdk::cardemulation::INfcFCardEmulation> GetNfcFCardEmulationInterface();
#endif
#endif
    std::weak_ptr<NfcService> GetInstance() const;

protected:
    void PowerActivity() override;
    // Sound
    void InitSoundPool();
    void ReleaseSoundPool();
    void PlaySound(int sound) override;

    // Screen
    bool IsScreenUnlockLack() const;
    void ApplyScreenState(int screenState);

    void UpdatePackageCache();
    void OnPreferredPaymentChanged(int reason);

    /*
     * Screen Change Receiver
     */
    class ScreenChangeReceive;
    /**
     * @brief Package Change Receiver；
     */
    class PackageChangeReceive;

private:
    void ApplyDiscovering(bool force) override;

    bool IsNfcEnabled() override;
    bool IsScreenUnlock() override;
    bool IsInForeground(int uid) override;
    std::weak_ptr<IDeviceHost> GetDeviceHost() override
    {
        return mDeviceHost_;
    }
#ifdef _NFC_SERVICE_READER_
    OHOS::sptr<reader::ITagSession> GetTagSession() override;
    std::weak_ptr<reader::TagDispatcher> GetTagDispatcher() override;
    std::weak_ptr<AppExecFwk::EventHandler> GetMessageHandler() override;
#endif
    bool IsNfcTaskReady(std::future<int>& future) const;
    void StartEnableDisableNfcTask(int param, bool saveState = false);
    bool IsNfcEnabledOrShuttingDown();
    void SaveNfcOnSetting(bool on);
    // Apply Discovering
    void StartApplyDiscoveringTask();
    void StartApplyDiscoveringTask(int params);
    void ApplyDiscoveringTaskHandler(std::unique_ptr<int> params);
    std::unique_ptr<NfcDiscoveryParams> GetDiscoveryParameters(int screenState);
    // Nfc State
    int GetState();
    bool IsTurningOff();
    // Enable/Disable Nfc
    void EnableDisableNfc(int params, std::promise<int> promise);
    bool EnableInternal();
    bool DisableInternal();
    void UpdateState(int newState);
    void PauseDiscovering(int timeoutInMs) override;
    void ResumeDiscovering();
    // Secure Nfc
    bool DeviceSupportsNfcSecure() const;
    bool SetNfcSecure(bool enable);
    bool IsNfcSecureEnabled();

private:
    /**
     * Flag for use with Enable Reader Mode. Setting this flag
     * allows the caller to prevent the platform from performing
     * an NDEF check on the tags it finds.
     */
    static constexpr const int FLAG_READER_SKIP_NDEF_CHECK = 0x80;
    /**
     * Flag for use with Enable Reader Mode. Setting this flag
     * allows the caller to prevent the platform
     * from playing sounds when it discovers a tag.
     */
    static constexpr const int FLAG_READER_NO_PLATFORM_SOUNDS = 0x100;
    // Update stats every 4 hours
    static constexpr const long STATS_UPDATE_INTERVAL_MS = 4 * 60 * 60 * 1000;
    static constexpr const long MAX_POLLING_PAUSE_TIMEOUT = 40000;

    /*
     * Time to wait for NFC controller to initialize before watchdog
     * goes off. This time is chosen large, because firmware download
     * may be a part of initialization.
     */
    static constexpr const int INIT_WATCHDOG_MS = 90000;
    // Time to wait for routing to be applied before watchdog goes off
    static constexpr const int ROUTING_WATCHDOG_MS = 10000;
    // Timeout to re-apply routing if a tag was present and we postponed it
    static constexpr const int APPLY_ROUTING_RETRY_TIMEOUT_MS = 5000;
    /**
     * Nfc is enabled and the preferred payment aids are registered.
     */
    static const int PRE_PAYMENT_LOADED = 1;
    static const int NFC_POLL_DEFAULT = -1;

    static constexpr const auto PREF_NFC_ON = "nfc_on";
    static const bool NFC_ON_DEFAULT = true;
    static constexpr const auto PREF_SECURE_NFC_ON = "secure_nfc_on";
    static const bool SECURE_NFC_ON_DEFAULT = false;
    static constexpr const auto PREF_FIRST_BEAM = "first_beam";
    static constexpr const auto PREF_FIRST_BOOT = "first_boot";
    static int mNciVersion_;
    // context
    std::weak_ptr<osal::Context> mContext_{};
    std::weak_ptr<osal::PowerManager> mPowerManager_{};
    std::weak_ptr<osal::KeyguardManager> mKeyguard_{};
    std::weak_ptr<osal::SharedPreferences> mPrefs_{};
    // service
    std::weak_ptr<NfcService> mNfcService_{};
    // NCI
    std::shared_ptr<IDeviceHost> mDeviceHost_{};
    // Reader
    OHOS::sptr<NfcAgentService> mNfcAgenter_;
    // EventHandler
    std::shared_ptr<NfcServiceHandler> mHandler_{};
#ifdef _NFC_SERVICE_READER_
    OHOS::sptr<reader::ITagSession> mTagSession_{};
    std::shared_ptr<reader::TagDispatcher> mTagDispatcher_{};
#endif
#ifdef _NFC_SERVICE_HCE_
    std::shared_ptr<nfc::cardemulation::CardEmulationManager> mCardEmulationManager_;
#endif  // _NFC_SERVICE_HCE_

    std::shared_ptr<EventFwk::CommonEventSubscriber> mReceiver_{};
    std::shared_ptr<EventFwk::CommonEventSubscriber> mPkgReceiver_{};
    std::shared_ptr<EventFwk::CommonEventSubscriber> mPkg2Receiver_{};

    int mState_;
    int mScreenState_;
    bool mIsHceCapable_;
    bool mIsHceFCapable_;
    bool mNfcMessageEnabled_;
    bool mPollingPaused_{false};
    bool mIsSecureNfcEnabled_;
    int mUserId_{-1};

    // Metrics
    std::atomic_int mNumTagsDetected_{};
    std::atomic_int mNumHceDetected_{};

    std::unique_ptr<NfcDiscoveryParams> mCurrentDiscoveryParams_;
    std::shared_ptr<ScreenStateHelper> mScreenStateHelper_;
    ForegroundUtils* mForegroundUtils_{};
    // lock
    std::mutex mtx_{};
    std::future<int> mFuture_{};
    std::unique_ptr<std::thread> mTask_{};
    std::future<int> mRootFuture_{};
    std::unique_ptr<std::thread> mRootTask_{};

    std::vector<std::string> mNfcEventInstalledPackages_{};
    std::vector<std::string> mNfcPreferredPaymentChangedInstalledPackages_{};

    friend class WatchDog;
    friend class NfcAgentService;
#ifdef _NFC_SERVICE_READER_
    friend class reader::TagDispatcher;
#endif
    friend class NfcServiceHandler;
    friend class NfcServiceManager;
};
}  // namespace nfc
}  // namespace OHOS
#endif  // !NFC_SERVICE_H
