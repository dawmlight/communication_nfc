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
#include "nfc_service.h"

#include <algorithm>
#include <sstream>
#include <thread>

#include "common_event_manager.h"
#include "context.h"
#include "device_host.h"
#include "itag_end_point.h"
#include "key_guard_manager.h"
#include "loghelper.h"
#include "nfc_agent_service.h"
#include "nfc_discovery_params.h"
#include "nfc_service_define.h"
#include "nfc_service_handler.h"
#include "power_manager.h"
#include "resources.h"
#include "shared_preferences.h"
#include "tag.h"
#include "tag_dispatcher.h"
#include "tag_session.h"
#include "utils/common_utils.h"
#include "utils/foreground_utils.h"
#include "utils/screen_state_helper.h"
#include "want.h"
#include "watch_dog.h"

#ifdef _NFC_SERVICE_HCE_
#include "card_emulation_manager.h"
#include "icard_emulation_agent.h"
#endif

using namespace osal;

namespace OHOS {
using AAFwk::Want;
using EventFwk::CommonEventData;
using EventFwk::CommonEventManager;
using EventFwk::CommonEventPublishInfo;
using EventFwk::CommonEventSubscribeInfo;
using EventFwk::CommonEventSubscriber;
using EventFwk::MatchingSkills;
// Service Name
const std::u16string NFC_SERVICE_NAME = OHOS::to_utf16("ohos.nfc.service");
// Screen Common Event
const std::string ACTION_SCREEN_ON = "ohos.intent.action.SCREEN_ON";
const std::string ACTION_SCREEN_OFF = "ohos.intent.action.SCREEN_OFF";
const std::string ACTION_USER_PRESENT = "ohos.intent.action.USER_PRESENT";
const std::string ACTION_USER_SWITCHED = "ohos.intent.action.USER_SWITCHED";
// Package Common Event
const std::string ACTION_PACKAGE_REMOVED = "ohos.intent.action.PACKAGE_REMOVED";
const std::string ACTION_PACKAGE_ADDED = "ohos.intent.action.PACKAGE_ADDED";
const std::string ACTION_EXTERNAL_APPLICATIONS_AVAILABLE = "ohos.intent.action.EXTERNAL_APPLICATIONS_AVAILABLE";
const std::string ACTION_EXTERNAL_APPLICATIONS_UNAVAILABLE = "ohos.intent.action.EXTERNAL_APPLICATIONS_UNAVAILABLE";
const std::string ACTION_SHUTDOWN = "ohos.intent.action.ACTION_SHUTDOWN";
// Package Common Event Permission
const std::string ACTION_PACKAGE_PERMISSION = "package";

namespace nfc {
using ObjectMapIter = std::map<int, std::shared_ptr<ITagEndPoint>>::iterator;
int NfcService::mNciVersion_ = 0x02;
#ifdef _NFC_SERVICE_READER_
using reader::ITagSession;
using reader::TagDispatcher;
using reader::TagSession;
OHOS::sptr<ITagSession> NfcService::GetTagSession()
{
    return mTagSession_;
}
std::weak_ptr<TagDispatcher> NfcService::GetTagDispatcher()
{
    return mTagDispatcher_;
}
std::weak_ptr<AppExecFwk::EventHandler> NfcService::GetMessageHandler()
{
    return mHandler_;
}
#endif

class NfcService::ScreenChangeReceive : public CommonEventSubscriber {
public:
    explicit ScreenChangeReceive(std::weak_ptr<NfcService> nfcService, const CommonEventSubscribeInfo& subscribeInfo);
    ~ScreenChangeReceive()
    {
    }
    void OnReceiveEvent(const CommonEventData& data) override;

private:
    std::weak_ptr<NfcService> mNfcService_{};
    std::weak_ptr<osal::KeyguardManager> mKeyguard_{};
    std::weak_ptr<AppExecFwk::EventHandler> mHandler_{};
    std::weak_ptr<ScreenStateHelper> mScreenStateHelper_{};
};

NfcService::ScreenChangeReceive::ScreenChangeReceive(std::weak_ptr<NfcService> nfcService,
                                                     const CommonEventSubscribeInfo& subscribeInfo)
  : CommonEventSubscriber(subscribeInfo),
    mNfcService_(nfcService),
    mKeyguard_(nfcService.lock()->mKeyguard_),
    mHandler_(nfcService.lock()->mHandler_),
    mScreenStateHelper_(nfcService.lock()->mScreenStateHelper_)
{
}

void NfcService::ScreenChangeReceive::OnReceiveEvent(const CommonEventData& data)
{
    std::string action = data.GetWant().GetAction();
    if (action.compare(ACTION_SCREEN_ON) == 0 || action.compare(ACTION_SCREEN_OFF) == 0 ||
        action.compare(ACTION_USER_PRESENT) == 0) {
        // Perform applyRouting() in AsyncTask to serialize blocking calls
        int screenState = mScreenStateHelper_.lock()->CheckScreenState();
        if (action.compare(ACTION_SCREEN_OFF) == 0) {
            if (mNfcService_.lock()->mScreenState_ != SCREEN_STATE_OFF_LOCKED) {
                screenState =
                    mKeyguard_.lock()->IsKeyguardLocked() ? SCREEN_STATE_OFF_LOCKED : SCREEN_STATE_OFF_UNLOCKED;
            }
        } else if (action.compare(ACTION_SCREEN_ON) == 0) {
            screenState = mKeyguard_.lock()->IsKeyguardLocked() ? SCREEN_STATE_ON_LOCKED : SCREEN_STATE_ON_UNLOCKED;
        } else if (action.compare(ACTION_USER_PRESENT) == 0) {
            screenState = SCREEN_STATE_ON_UNLOCKED;
        }

        if (mNfcService_.lock()->mNciVersion_ != NCI_VERSION_2_0) {
            mNfcService_.lock()->StartApplyDiscoveringTask(screenState);
        }
        mHandler_.lock()->SendEvent(MSG_APPLY_SCREEN_STATE, screenState, (int64_t)0);
    } else if (action.compare(ACTION_USER_SWITCHED) == 0) {
#ifdef _NFC_SERVICE_HCE_
        int userId = 0;
        auto nfcService = mNfcService_.lock();
        if (nfcService && nfcService->mIsHceCapable_) {
            nfcService->mCardEmulationManager_->OnUserSwitched(userId);
        }
#endif  // _NFC_SERVICE_HCE_
        int screenState = mScreenStateHelper_.lock()->CheckScreenState();
        if (screenState != mNfcService_.lock()->mScreenState_) {
            mNfcService_.lock()->StartApplyDiscoveringTask(screenState);
        }
    }
}

class NfcService::PackageChangeReceive : public CommonEventSubscriber {
public:
    explicit PackageChangeReceive(std::weak_ptr<NfcService> nfcService, const CommonEventSubscribeInfo& subscribeInfo);
    ~PackageChangeReceive()
    {
    }
    void OnReceiveEvent(const CommonEventData& data) override;

private:
    std::weak_ptr<NfcService> mNfcService_{};
    std::weak_ptr<IDeviceHost> mDeviceHost_{};
};

NfcService::PackageChangeReceive::PackageChangeReceive(std::weak_ptr<NfcService> nfcService,
                                                       const CommonEventSubscribeInfo& subscribeInfo)
  : CommonEventSubscriber(subscribeInfo),
    mNfcService_(nfcService),
    mDeviceHost_(nfcService.lock()->mDeviceHost_)
{
}

void NfcService::PackageChangeReceive::OnReceiveEvent(const CommonEventData& data)
{
    std::string action = data.GetWant().GetAction();
    if (action.compare(ACTION_PACKAGE_REMOVED) == 0 || action.compare(ACTION_PACKAGE_ADDED) == 0 ||
        action.compare(ACTION_EXTERNAL_APPLICATIONS_AVAILABLE) == 0 ||
        action.compare(ACTION_EXTERNAL_APPLICATIONS_UNAVAILABLE) == 0) {
        mNfcService_.lock()->UpdatePackageCache();
    } else if (action.compare(ACTION_SHUTDOWN) == 0) {
        DebugLog("Device is shutting down.");
        if (mNfcService_.lock()->IsNfcEnabled()) {
            mDeviceHost_.lock()->Shutdown();
        }
    }
}

void NfcService::OnRemoteFieldActivated()
{
    mHandler_->SendEvent(MSG_RF_FIELD_ACTIVATED);
}

void NfcService::OnRemoteFieldDeactivated()
{
    mHandler_->SendEvent(MSG_RF_FIELD_DEACTIVATED);
}

void NfcService::OnHostCardEmulationActivated(int tech)
{
    InfoLog("Host Card Emulation Activated by tech(%d)", tech);
#ifdef _NFC_SERVICE_HCE_
    if (mCardEmulationManager_) {
        mCardEmulationManager_->OnHostCardEmulationActivated(tech);
    }
#endif
}

void NfcService::OnHostCardEmulationDeactivated(int tech)
{
    InfoLog("Host Card Emulation Deactivated By tech(%d)", tech);
#ifdef _NFC_SERVICE_HCE_
    if (mCardEmulationManager_) {
        // Do metrics here so we don't slow the CE path down
        mCardEmulationManager_->OnHostCardEmulationDeactivated(tech);
    }
#endif  // _NFC_SERVICE_HCE_
}

void NfcService::OnHostCardEmulationDataReceived(int tech, std::string& data)
{
    InfoLog("Host Card Emulation Data(%u) Received By tech(%d)", data.length(), tech);
#ifdef _NFC_SERVICE_HCE_
    if (mCardEmulationManager_) {
        mCardEmulationManager_->OnHostCardEmulationData(tech, data);
    }
#endif  // _NFC_SERVICE_HCE_
}

void NfcService::OnOffHostTransactionEvent(std::string& aid, std::string& data, std::string& seName)
{
    std::shared_ptr<OffHostTransactionData> dataObj = std::make_shared<OffHostTransactionData>();
    dataObj->aid_ = aid;
    dataObj->data_ = data;
    dataObj->seName_ = seName;
    mHandler_->SendEvent<OffHostTransactionData>(MSG_TRANSACTION_EVENT, dataObj);
}

void NfcService::OnEeUpdate()
{
    StartApplyDiscoveringTask();
}

void NfcService::OnTagDiscovered(std::shared_ptr<ITagEndPoint> tagEndPoint)
{
    InfoLog("NfcService::OnTagDiscovered");
    mHandler_->SendEvent<ITagEndPoint>(MSG_NDEF_TAG, tagEndPoint);
}

int NfcService::GetState()
{
    std::lock_guard<std::mutex> lock(mtx_);
    return mState_;
}

bool NfcService::IsTurningOff()
{
    std::lock_guard<std::mutex> lock(mtx_);
    return mState_ == sdk::STATE_TURNING_OFF;
}

bool NfcService::IsNfcTaskReady(std::future<int>& future) const
{
    if (future.valid()) {
        if (std::future_status::ready != future.wait_for(std::chrono::seconds(1))) {
            return false;
        }
    }
    return true;
}

void NfcService::StartEnableDisableNfcTask(int param, bool saveState)
{
    // Check the current state
    {
        std::lock_guard<std::mutex> lock(mtx_);
        if (sdk::STATE_TURNING_OFF == mState_ || sdk::STATE_TURNING_ON == mState_) {
            ErrorLog("Processing EnableDisable task %d from bad state %d", param, mState_);
            return;
        }
        if (param == TASK_ENABLE && mState_ == sdk::STATE_ON) {
            DebugLog("NFC Turn On");
            return;
        }
        if (param == TASK_DISABLE && mState_ == sdk::STATE_OFF) {
            DebugLog("NFC Turn Off");
            return;
        }
    }

    if (saveState) {
        SaveNfcOnSetting(param == TASK_ENABLE);
    }

    std::promise<int> promise;
    if (mRootTask_) {
        if (!IsNfcTaskReady(mFuture_)) {
            // working
            return;
        }
        if (mTask_ && mTask_->joinable()) {
            mTask_->join();
        }
        mFuture_ = promise.get_future();
        mTask_ = std::make_unique<std::thread>(&NfcService::EnableDisableNfc, this, param, std::move(promise));
    } else {
        mRootTask_ = std::make_unique<std::thread>(&NfcService::EnableDisableNfc, this, param, std::move(promise));
    }
}

void NfcService::SaveNfcOnSetting(bool on)
{
    mPrefs_.lock()->PutBool(PREF_NFC_ON, on);
}

void NfcService::EnableDisableNfc(int params, std::promise<int> promise)
{
    DebugLog("Enable Disable Nfc. Params %d", params);
    switch (params) {
        case TASK_ENABLE:
            EnableInternal();
            break;
        case TASK_DISABLE:
            DisableInternal();
            break;
        case TASK_INITIALIZE: {
            bool initialized = false;
            if (mPrefs_.lock()->GetBool(PREF_FIRST_BOOT, true)) {
                InfoLog("First Boot");
                mDeviceHost_->FactoryReset();
                mPrefs_.lock()->PutBool(PREF_FIRST_BOOT, false);
            }

            DebugLog("checking on firmware download");
            if (mPrefs_.lock()->GetBool(PREF_NFC_ON, NFC_ON_DEFAULT)) {
                DebugLog("NFC is on. Doing normal stuff");
                initialized = EnableInternal();
            } else {
                DebugLog("NFC is off.  Checking firmware version");
                initialized = mDeviceHost_->CheckFirmware();
            }
#ifdef _NFC_SERVICE_READER_
            mTagDispatcher_->InitializeReaderParams();
#endif  // _NFC_SERVICE_READER_
            if (initialized) {
#if _SYSTEM_PROPERTIES_
                SystemProperties.set("nfc.initialized", "true");
#endif
            }
            break;
        }
        default:
            break;
    }
    promise.set_value_at_thread_exit(0);
    return;
}

bool NfcService::EnableInternal()
{
    DebugLog("Enabling NFC. Current State %d", mState_);
    UpdateState(sdk::STATE_TURNING_ON);

    WatchDog watchDog("enableInternal", INIT_WATCHDOG_MS, mDeviceHost_);
    watchDog.Run();
    // Routing WakeLock acquire
    if (!mDeviceHost_->Initialize()) {
        WarnLog("Error enabling NFC");
        UpdateState(sdk::STATE_OFF);
        // Routing Wake Lock release
        watchDog.Cancel();
        return false;
    }
    // Routing Wake Lock release
    watchDog.Cancel();

#ifdef _NFC_SERVICE_HCE_
    if (mIsHceCapable_) {
        // Generate the initial card emulation routing table
        mCardEmulationManager_->OnNfcEnabled();
    }
#endif

    mNciVersion_ = mDeviceHost_->GetNciVersion();
    DebugLog("NCI_Version: %d", mNciVersion_);

#ifdef _NFC_SERVICE_READER_
    mTagDispatcher_->ResetTagParams();
#endif
    UpdateState(sdk::STATE_ON);
    //
    OnPreferredPaymentChanged(PRE_PAYMENT_LOADED);

    InitSoundPool();

    mScreenState_ = mScreenStateHelper_->CheckScreenState();

    mDeviceHost_->SetScreenStatus(static_cast<unsigned char>(mScreenState_ & 0xff));

    /* Start polling loop */
    ApplyDiscovering(true);
    return true;
}

/**
 * Disable all NFC adapter functions.
 * Does not toggle preferences.
 */
bool NfcService::DisableInternal()
{
    DebugLog("Disabling NFC %d", mState_);
    UpdateState(sdk::STATE_TURNING_OFF);

    /* Sometimes mDeviceHost.deinitialize() hangs, use a watch-dog.
     * Implemented with a new thread (instead of a Handler or AsyncTask),
     * because the UI Thread and AsyncTask thread-pools can also get hung
     * when the NFC controller stops responding */
    WatchDog watchDog("disableInternal", ROUTING_WATCHDOG_MS, mDeviceHost_);
    watchDog.Run();

#ifdef _NFC_SERVICE_HCE_
    if (mIsHceCapable_) {
        mCardEmulationManager_->OnNfcDisabled();
    }
#endif  // _NFC_SERVICE_HCE_

#ifdef _NFC_SERVICE_READER_
    // Stop watchdog if tag present
    // A convenient way to stop the watchdog properly consists of
    // disconnecting the tag. The polling loop shall be stopped before
    // to avoid the tag being discovered again.
    if (IsNfcEnabledOrShuttingDown()) {
        mTagDispatcher_->MaybeDisconnectTarget();
    }

    mTagDispatcher_->SetFrontDispatch(nullptr);
#endif  // _NFC_SERVICE_READER_

    bool result = mDeviceHost_->Deinitialize();
    DebugLog("mDeviceHost.deinitialize() = %d", result);

    watchDog.Cancel();

    {
        std::lock_guard<std::mutex> lock(mtx_);
        mCurrentDiscoveryParams_ = NfcDiscoveryParams::GetNfcOffParameters();
    }
    UpdateState(sdk::STATE_OFF);

    ReleaseSoundPool();
    return result;
}

void NfcService::UpdateState(int newState)
{
    DebugLog("UpdateState Old State %d and New State %d", mState_, newState);
    {
        std::lock_guard<std::mutex> lock(mtx_);
        if (newState == mState_) {
            return;
        }
        mState_ = newState;
    }

    Want want;
    want.SetAction(ACTION_ADAPTER_STATE_CHANGED);
    want.SetFlags(FLAG_RECEIVER_REGISTERED_ONLY_BEFORE_BOOT);
    want.SetParam(EXTRA_ADAPTER_STATE, newState);
    CommonEventData data;
    data.SetWant(want);
    CommonEventManager::PublishCommonEvent(data);
}

void NfcService::PauseDiscovering(int timeoutInMs)
{
    if (timeoutInMs <= 0 || timeoutInMs > MAX_POLLING_PAUSE_TIMEOUT) {
        ErrorLog("Refusing to pause polling for %d ms.", timeoutInMs);
        return;
    }

    std::lock_guard<std::mutex> lock(mtx_);
    mPollingPaused_ = true;
    mDeviceHost_->DisableDiscovery();
    int64_t newTime = CommonUtils::GetCurrentMilliSecondsTime();
    mHandler_->SendTimingEvent(MSG_EVENT_RESUME_POLLING, newTime + timeoutInMs);
}

void NfcService::ResumeDiscovering()
{
    std::lock_guard<std::mutex> lock(mtx_);
    if (!mPollingPaused_) {
        return;
    }

    mHandler_->RemoveEvent(MSG_EVENT_RESUME_POLLING);
    mPollingPaused_ = false;
    StartApplyDiscoveringTask();
}

bool NfcService::DeviceSupportsNfcSecure() const
{
    // SystemProperties Parameters "ro.boot.hardware.sku" to Supports Secure Nfc.
    return true;
}

bool NfcService::SetNfcSecure(bool enable)
{
    if (mKeyguard_.lock()->IsKeyguardLocked() && !enable) {
        InfoLog("KeyGuard need to be unlocked before setting Secure NFC OFF");
        return false;
    }

    {
        std::lock_guard<std::mutex> lock(mtx_);
        DebugLog("setting Secure NFC %d", enable);
        mPrefs_.lock()->PutBool(PREF_SECURE_NFC_ON, enable);
#ifdef _BACKUP_MANAGER_
        mBackupManager.dataChanged();
#endif
        mIsSecureNfcEnabled_ = enable;
        mDeviceHost_->SetSecureNfc(enable);
    }
#ifdef _NFC_SERVICE_HCE_
    if (mIsHceCapable_) {
        mCardEmulationManager_->OnSecureNfcToggled();
    }
#endif  // _NFC_SERVICE_HCE_
    return true;
}

bool NfcService::IsNfcSecureEnabled()
{
    std::lock_guard<std::mutex> lock(mtx_);
    return mIsSecureNfcEnabled_;
}

void NfcService::StartApplyDiscoveringTask()
{
    std::unique_ptr<int> param(nullptr);
    std::thread trd(&NfcService::ApplyDiscoveringTaskHandler, this, std::move(param));
    trd.detach();
}

void NfcService::StartApplyDiscoveringTask(int params)
{
    std::unique_ptr<int> param = std::make_unique<int>(params);
    std::thread trd(&NfcService::ApplyDiscoveringTaskHandler, this, std::move(param));
    trd.detach();
}

void NfcService::ApplyDiscoveringTaskHandler(std::unique_ptr<int> params)
{
    if (!params) {
        // force apply current routing
        ApplyDiscovering(true);
        return;
    }
    mScreenState_ = *params;

    // Routing Wake Lock acquire
    ApplyDiscovering(false);
    // Routing Wake Lock release
}

bool NfcService::IsNfcEnabledOrShuttingDown()
{
    std::lock_guard<std::mutex> lock(mtx_);
    return (mState_ == sdk::STATE_ON || mState_ == sdk::STATE_TURNING_OFF);
}

bool NfcService::IsNfcEnabled()
{
    std::lock_guard<std::mutex> lock(mtx_);
    return (mState_ == sdk::STATE_ON);
}

bool NfcService::IsScreenUnlock()
{
    std::lock_guard<std::mutex> lock(mtx_);
    return IsScreenUnlockLack();
}

bool NfcService::IsInForeground(int uid)
{
    return mForegroundUtils_->IsInForeground(uid);
}

bool NfcService::IsScreenUnlockLack() const
{
    return mScreenState_ == SCREEN_STATE_ON_UNLOCKED;
}

void NfcService::ApplyScreenState(int screenState)
{
    mScreenState_ = screenState;
    DebugLog("MSG_APPLY_SCREEN_STATE %d", mScreenState_);

    // If NFC is turning off, we shouldn't need any changes here
    if (IsTurningOff()) {
        return;
    }

    if (mScreenState_ == SCREEN_STATE_ON_UNLOCKED) {
        ApplyDiscovering(false);
    }

    mDeviceHost_->SetScreenStatus(static_cast<unsigned char>(mScreenState_ & 0xff));
}

void NfcService::ApplyDiscovering(bool force)
{
    DebugLog("ApplyDiscovering.");
    if (!IsNfcEnabledOrShuttingDown()) {
        return;
    }
    std::lock_guard<std::mutex> lock(mtx_);
#ifdef _NFC_SERVICE_READER_
    if (IsScreenUnlockLack() && mTagDispatcher_->IsTagPresent()) {
        InfoLog("Not updating discovery parameters, tag connected.");
        int64_t newTime = CommonUtils::GetCurrentMilliSecondsTime();
        mHandler_->SendTimingEvent(MSG_EVENT_RESUME_POLLING, newTime + APPLY_ROUTING_RETRY_TIMEOUT_MS);
        return;
    }
#endif  // _NFC_SERVICE_READER_

    WatchDog watchDog("applyRouting", ROUTING_WATCHDOG_MS, mDeviceHost_);
    watchDog.Run();
    // Compute new polling parameters
    std::unique_ptr<NfcDiscoveryParams> newParams = GetDiscoveryParameters(mScreenState_);
    if (force || !((*newParams) == (*mCurrentDiscoveryParams_))) {
        if (newParams->ShouldEnableDiscovery()) {
            bool shouldRestart = mCurrentDiscoveryParams_->ShouldEnableDiscovery();
            mDeviceHost_->EnableDiscovery(newParams->GetTechMask(),
                                          newParams->ShouldEnableReaderMode(),
                                          newParams->ShouldEnableHostRouting(),
                                          shouldRestart);
        } else {
            mDeviceHost_->DisableDiscovery();
        }
        mCurrentDiscoveryParams_ = std::move(newParams);
    } else {
        DebugLog("Discovery configuration equal, not updating.");
    }
    newParams = nullptr;
    watchDog.Cancel();
}

std::unique_ptr<NfcDiscoveryParams> NfcService::GetDiscoveryParameters(int screenState)
{
    // Recompute discovery parameters based on screen state
    NfcDiscoveryParams::Builder paramsBuilder;
    // Polling
    std::weak_ptr<reader::ReaderModeParams> readerParams = mTagDispatcher_->GetReaderDatas();
    if (screenState >= SCREEN_STATE_ON_UNLOCKED) {
        // Check if reader-mode is enabled
        if (readerParams.expired()) {
            paramsBuilder.SetTechMask(NFC_POLL_DEFAULT);
        } else {
            int techMask = 0;
            if ((readerParams.lock()->mFlags_ & sdk::NFC_POLL_A) != 0) {
                techMask |= sdk::NFC_POLL_A;
            }
            if ((readerParams.lock()->mFlags_ & sdk::NFC_POLL_B) != 0) {
                techMask |= sdk::NFC_POLL_B;
            }
            if ((readerParams.lock()->mFlags_ & sdk::NFC_POLL_F) != 0) {
                techMask |= sdk::NFC_POLL_F;
            }
            if ((readerParams.lock()->mFlags_ & sdk::NFC_POLL_V) != 0) {
                techMask |= sdk::NFC_POLL_V;
            }

            paramsBuilder.SetTechMask(techMask);
            paramsBuilder.SetEnableReaderMode(true);
        }
    }

    if (mIsHceCapable_ && mScreenState_ >= SCREEN_STATE_ON_LOCKED && readerParams.expired()) {
        // Host routing is always enabled at lock screen or later, provided we
        // aren't in reader mode
        paramsBuilder.SetEnableHostRouting(true);
    }
    return paramsBuilder.Build();
}
#ifdef _NFC_SERVICE_HCE_
int NfcService::GetAidRoutingTableSize()
{
    return mDeviceHost_->GetAidRoutingTableSize();
}

int NfcService::GetDefaultRoute()
{
    return mDeviceHost_->GetDefaultRoute();
}

int NfcService::GetDefaultOffHostRoute()
{
    return mDeviceHost_->GetDefaultOffHostRoute();
}

std::vector<int> NfcService::GetOffHostUiccRoute()
{
    return mDeviceHost_->GetOffHostUiccRoute();
}

std::vector<int> NfcService::GetOffHostEseRoute()
{
    return mDeviceHost_->GetOffHostEseRoute();
}

int NfcService::GetAidMatchingMode()
{
    return mDeviceHost_->GetAidMatchingMode();
}

int NfcService::GetDefaultIsoDepRouteDestination()
{
    return mDeviceHost_->GetDefaultIsoDepRouteDestination();
}

bool NfcService::GetExtendedLengthApdusSupported()
{
    return mDeviceHost_->GetExtendedLengthApdusSupported();
}

bool NfcService::AddAidRouting(std::string aid, int route, int aidInfo)
{
    auto entry = std::make_shared<RouteAidsEntry>();
    entry->route_ = route;
    entry->aid_ = std::move(aid);
    entry->aidInfo_ = aidInfo;
    return mHandler_->SendEvent<RouteAidsEntry>(MSG_ROUTE_AID, entry);
}

bool NfcService::RemoveAidRouting(const std::string& aid)
{
    return mHandler_->SendEvent<std::string>(MSG_UNROUTE_AID, std::make_shared<std::string>(aid));
}

bool NfcService::ClearRouting()
{
    mHandler_->RemoveEvent(MSG_ROUTE_AID);
    mHandler_->RemoveEvent(MSG_COMMIT_ROUTING);
    return mDeviceHost_->ClearAidTable();
}

bool NfcService::CommitRouting()
{
    return mHandler_->SendEvent(MSG_COMMIT_ROUTING);
}

int NfcService::GetRemainRoutingTableSize()
{
    return mDeviceHost_->GetRemainRoutingTableSize();
}

std::string GetT3tIdentifierBytes(std::string& systemCode, std::string& nfcId2, std::string& t3tPmm)
{
    const int TWO_MULTIPLE = 2;
    std::string t3tIdentifier = (systemCode.length() % TWO_MULTIPLE) == 0 ? systemCode : "0" + systemCode;
    t3tIdentifier += (nfcId2.length() % TWO_MULTIPLE) == 0 ? nfcId2 : ("0" + nfcId2);
    t3tIdentifier += (t3tPmm.length() % TWO_MULTIPLE) == 0 ? t3tPmm : ("0" + t3tPmm);
    return t3tIdentifier;
}

void NfcService::RegisterT3tIdentifier(std::string& systemCode, std::string& nfcId2, std::string& t3tPmm)
{
    DebugLog("request to register LF_T3T_IDENTIFIER");

    std::string t3tIdentifier = GetT3tIdentifierBytes(systemCode, nfcId2, t3tPmm);
    mHandler_->SendEvent<std::string>(MSG_REGISTER_T3T_IDENTIFIER, std::make_shared<std::string>(t3tIdentifier));
}

void NfcService::DeregisterT3tIdentifier(std::string& systemCode, std::string& nfcId2, std::string& t3tPmm)
{
    DebugLog("request to deregister LF_T3T_IDENTIFIER");

    std::string t3tIdentifier = GetT3tIdentifierBytes(systemCode, nfcId2, t3tPmm);
    mHandler_->SendEvent<std::string>(MSG_DEREGISTER_T3T_IDENTIFIER, std::make_shared<std::string>(t3tIdentifier));
}

void NfcService::ClearT3tIdentifiersCache()
{
    DebugLog("clear T3t Identifiers Cache");
    mDeviceHost_->ClearT3tIdentifiersCache();
}

int NfcService::GetLfT3tMax()
{
    return mDeviceHost_->GetLfT3tMax();
}

bool NfcService::SendData(std::vector<unsigned char> data)
{
    std::stringstream ss;
    std::for_each(data.begin(), data.end(), [&ss](unsigned char c) { ss << c; });
    auto s = ss.str();
    return mDeviceHost_->SendRawFrame(s);
}
bool NfcService::DumpRoutingTable(int fd)
{
    if (mDeviceHost_) {
        mDeviceHost_->Dump(fd);
        return true;
    }
    return false;
}

/* Returns the list of packages that have access to NFC Events on any SE */
std::vector<std::string> NfcService::GetSEAccessAllowedPackages()
{
    if (!IsSEServiceAvailable() || mNfcEventInstalledPackages_.empty()) {
        return mNfcEventInstalledPackages_;
    }
    return std::vector<std::string>();
}

void NfcService::SendNfcEeAccessProtectedBroadcast(std::shared_ptr<AAFwk::Want> intent)
{
    intent->AddFlags(FLAG_INCLUDE_STOPPED_PACKAGES);
    // Resume app switches so the receivers can start activites without delay
#ifdef _NFC_SERVICE_READER_
    mTagDispatcher_->ResumeAppSwitches();
#endif  // _NFC_SERVICE_READER_
}

void NfcService::SendOffHostTransactionEvent(std::string aid, std::string data, std::string readerByteArray)
{
    InfoLog("Send Off Host Transaction Event. Aid Is (%s). Data Length Is (%u). Reader Byte Array Size Is (%u)",
            aid.c_str(),
            data.length(),
            readerByteArray.length());
    if (!IsSEServiceAvailable() || mNfcEventInstalledPackages_.empty()) {
        return;
    }
}

bool NfcService::IsSEServiceAvailable()
{
    DebugLog("IsSEServiceAvailable");
    return true;
}

void NfcService::SendPreferredPaymentChangedEvent(std::shared_ptr<Want> intent)
{
    intent->AddFlags(FLAG_INCLUDE_STOPPED_PACKAGES);
    // Resume app switches so the receivers can start activities without delay
#ifdef _NFC_SERVICE_READER_
    mTagDispatcher_->ResumeAppSwitches();
#endif
}
OHOS::sptr<OHOS::nfc::sdk::cardemulation::ICardEmulationAgent> NfcService::GetNfcCardEmulationInterface()
{
    DebugLog("hce is %s\n", mIsHceCapable_ ? "capable." : "not capable.");

    if (mIsHceCapable_ && mCardEmulationManager_) {
        DebugLog("GetCardEmulationAgent...\n");
        return mCardEmulationManager_->GetCardEmulationAgent();
    }
    return OHOS::sptr<OHOS::nfc::sdk::cardemulation::ICardEmulationAgent>();
}
#ifdef _NFC_F_CE_
std::shared_ptr<OHOS::nfc::sdk::cardemulation::INfcFCardEmulation> NfcService::GetNfcFCardEmulationInterface()
{
    return std::shared_ptr<OHOS::nfc::sdk::cardemulation::INfcFCardEmulation>();
}
#endif
#endif
void NfcService::UpdatePackageCache()
{
    std::lock_guard<std::mutex> lock(mtx_);
    mNfcEventInstalledPackages_.clear();
    mNfcPreferredPaymentChangedInstalledPackages_.clear();
}

void NfcService::OnPreferredPaymentChanged(int reason)
{
    mHandler_->SendEvent(MSG_PREFERRED_PAYMENT_CHANGED, reason, (int64_t)0);
}

void NfcService::InitSoundPool()
{
    DebugLog("Initialize Sound Pool");
#ifdef _NFC_SOUND_POOL_
    std::lock_guard<std::mutex> lock(mtx_);
    if (mSoundPool == null) {
        // Create the Sound Pool and load the mStartSound/mEndSound/mErrorSound sound resource
    }
#endif  // _NFC_SOUND_POOL_
}

void NfcService::ReleaseSoundPool()
{
    DebugLog("Release Sound Pool");
#ifdef _NFC_SOUND_POOL_
    if (mSoundPool != null) {
        mSoundPool.release();
        mSoundPool = null;
    }
#endif
}

void NfcService::PlaySound(int sound)
{
    DebugLog("Play Sound Of (%d)", sound);
#if _NFC_SOUND_POOL_
    std::lock_guard<std::mutex> lock(mtx_);
    if (mSoundPool == null) {
        WarnLog("Not playing sound when NFC is disabled");
        return;
    }

    if (mIsVrModeEnabled) {
        DebugLog("Not playing NFC sound when Vr Mode is enabled");
        return;
    }
    // Play the SOUND_START/SOUND_END/SOUND_ERROR sound
#endif
}

void NfcService::PowerActivity()
{
    if (IsScreenUnlock()) {
        // Get screet up time, reset the PowerManager::UserActivity
    }
}

std::weak_ptr<NfcService> NfcService::GetInstance() const
{
    return mNfcService_;
}

bool NfcService::Initialize()
{
    mNfcService_ = shared_from_this();
    InfoLog("Nfc Service Initialize.");
    if (mDeviceHost_) {
        mDeviceHost_->SetDeviceHostListener(mNfcService_);
    } else {
        mDeviceHost_ = std::make_shared<nfc::ncibal::DeviceHost>(mNfcService_);
    }

    mPowerManager_ = mContext_.lock()->GetPowerManagerService();
    mKeyguard_ = mContext_.lock()->GetKeyguardManagerService();
    mPrefs_ = mContext_.lock()->GetSharedPreferences(PREF);
    // screen state helper
    mScreenStateHelper_ = std::make_shared<ScreenStateHelper>(mContext_);
    mScreenState_ = mScreenStateHelper_->CheckScreenState();
    // foreground utils
    mForegroundUtils_ = ForegroundUtils::GetInstance();

    mCurrentDiscoveryParams_ = NfcDiscoveryParams::GetNfcOffParameters();

    // inner message handler，used by other modules as initialization parameters
    std::shared_ptr<AppExecFwk::EventRunner> runner = AppExecFwk::EventRunner::Create("nfc service handler");
    mHandler_ = std::make_shared<NfcServiceHandler>(runner, shared_from_this());
    mTagDispatcher_ = std::make_shared<TagDispatcher>(mContext_, shared_from_this());
    mTagSession_ = new TagSession(shared_from_this(), mContext_);

    // Get The Current(Activity) User Id
#ifdef _NFC_SERVICE_HCE_
    mCardEmulationManager_ = std::make_shared<cardemulation::CardEmulationManager>(shared_from_this());
    mCardEmulationManager_->Init();
    mIsHceCapable_ = true;
#endif
    // To be structured after Tag and HCE, the agent module is the agent of tag and HCE module
    mNfcAgenter_ = new NfcAgentService(shared_from_this(), mContext_);

    // Subscribe Screen Common Event
    MatchingSkills matchingSkills;
    matchingSkills.AddEvent(ACTION_SCREEN_ON);
    matchingSkills.AddEvent(ACTION_USER_PRESENT);
    matchingSkills.AddEvent(ACTION_USER_SWITCHED);
    CommonEventSubscribeInfo subscribeInfo(matchingSkills);
    mReceiver_ = std::make_shared<ScreenChangeReceive>(shared_from_this(), subscribeInfo);
    CommonEventManager::SubscribeCommonEvent(mReceiver_);
    // Subscribe Package Change Receive
    MatchingSkills pkMatchingSkills;
    pkMatchingSkills.AddEvent(ACTION_EXTERNAL_APPLICATIONS_AVAILABLE);
    pkMatchingSkills.AddEvent(ACTION_EXTERNAL_APPLICATIONS_UNAVAILABLE);
    pkMatchingSkills.AddEvent(ACTION_SHUTDOWN);
    CommonEventSubscribeInfo pkSubscribeInfo(pkMatchingSkills);
    mPkgReceiver_ = std::make_shared<PackageChangeReceive>(shared_from_this(), pkSubscribeInfo);
    CommonEventManager::SubscribeCommonEvent(mPkgReceiver_);
    // Subscribe Package Change Receive 2
    MatchingSkills pk2MatchingSkills;
    pk2MatchingSkills.AddEvent(ACTION_PACKAGE_ADDED);
    pk2MatchingSkills.AddEvent(ACTION_PACKAGE_REMOVED);
    CommonEventSubscribeInfo pk2SubscribeInfo(pk2MatchingSkills);
    pk2SubscribeInfo.SetPermission(ACTION_PACKAGE_PERMISSION);
    mPkg2Receiver_ = std::make_shared<PackageChangeReceive>(shared_from_this(), pk2SubscribeInfo);
    CommonEventManager::SubscribeCommonEvent(mPkg2Receiver_);

    UpdatePackageCache();

    bool isSecureNfcCapable = DeviceSupportsNfcSecure();
    mIsSecureNfcEnabled_ = mPrefs_.lock()->GetBool(PREF_SECURE_NFC_ON, SECURE_NFC_ON_DEFAULT) && isSecureNfcCapable;
    mDeviceHost_->SetSecureNfc(mIsSecureNfcEnabled_);

    mHandler_->Intialize(mTagDispatcher_);
    runner->Run();
    // NFC ROOT
    StartEnableDisableNfcTask(TASK_INITIALIZE);
    return true;
}

NfcService::NfcService(std::weak_ptr<Context> context, std::unique_ptr<nfc::ncibal::IDeviceHost> deviceHost)
  : mContext_(context),
    mDeviceHost_(std::move(deviceHost)),
    mNfcAgenter_(nullptr),
    mHandler_(nullptr),
#ifdef _NFC_SERVICE_READER_
    mTagSession_(nullptr),
    mTagDispatcher_(nullptr),
#endif
    mState_(sdk::STATE_OFF),
    mScreenState_(SCREEN_STATE_OFF_UNLOCKED),
    mIsHceCapable_(false),
    mIsHceFCapable_(false),
    mNfcMessageEnabled_(false),
    mIsSecureNfcEnabled_(false),
    mCurrentDiscoveryParams_(nullptr),
    mScreenStateHelper_(nullptr)
{
}

NfcService::~NfcService()
{
    InfoLog("NfcService Destructor");
    mNfcAgenter_ = nullptr;
    CommonEventManager::UnSubscribeCommonEvent(mPkg2Receiver_);
    CommonEventManager::UnSubscribeCommonEvent(mPkgReceiver_);
    CommonEventManager::UnSubscribeCommonEvent(mReceiver_);
    if (mTask_ && mTask_->joinable()) {
        mTask_->join();
    }
    if (mRootTask_ && mRootTask_->joinable()) {
        mRootTask_->join();
    }
}
}  // namespace nfc
}  // namespace OHOS