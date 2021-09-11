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
#include "nci_bal_manager.h"

#include <mutex>

#include "device_host.h"
#include "loghelper.h"
#include "nci_bal_tag.h"
#include "nfc_config.h"
#include "nfc_nci_impl.h"
#include "utils/synchronize_event.h"

#ifdef _NFC_SERVICE_HCE_
#include "hci_manager.h"
#include "nci_bal_ce.h"
#endif

using namespace OHOS::nfc;

namespace OHOS {
namespace nfc {
namespace ncibal {
OHOS::nfc::SynchronizeEvent NciBalManager::mNfcEnableEvent_;   // event for NFA_Enable()
OHOS::nfc::SynchronizeEvent NciBalManager::mNfcDisableEvent_;  // event for NFA_Disable()
OHOS::nfc::SynchronizeEvent NciBalManager::mPollingEvent_;     // event for polling
OHOS::nfc::SynchronizeEvent NciBalManager::setScreenStateEvent_;
OHOS::nfc::SynchronizeEvent NciBalManager::setConfigEvent_;

bool NciBalManager::mIsNfcEnabled_ = false;
bool NciBalManager::mRfEnabled_ = false;
bool NciBalManager::mDiscoveryEnabled_ = false;  // is polling or listening
bool NciBalManager::mPollingEnabled_ = false;    // is polling for tag
bool NciBalManager::mIsDisabling_ = false;
bool NciBalManager::mReaderModeEnabled_ = false;
unsigned long NciBalManager::mDiscoveryDuration_;
bool NciBalManager::mIsReconnect_ = false;
bool NciBalManager::mIsTagActive_ = false;
unsigned char NciBalManager::curScreenState_ = NFA_SCREEN_STATE_OFF_LOCKED;
std::shared_ptr<INfcNci> NciBalManager::mNfcNciImpl_ = std::make_shared<NfcNciImpl>();

NciBalManager::NciBalManager() = default;

NciBalManager::~NciBalManager() = default;

NciBalManager& NciBalManager::GetInstance()
{
    static NciBalManager mNciBalManager;
    return mNciBalManager;
}

bool NciBalManager::IsNfcActive()
{
    bool isActive = (mIsNfcEnabled_ && !mIsDisabling_);
    return isActive;
}

bool NciBalManager::IsTagActive() const
{
    return mIsTagActive_;
}

void NciBalManager::SetNfcNciImpl(std::shared_ptr<INfcNci> nfcNciImpl)
{
    mNfcNciImpl_ = nfcNciImpl;
}

void NciBalManager::StartRfDiscovery(bool isStart) const
{
    DebugLog("NciBalManager::StartRfDiscovery: isStart= %d", isStart);
    NciBalTag::GetInstance().StartRfField();
    tNFA_STATUS status;
    SynchronizeGuard guard(mPollingEvent_);
    if (isStart) {
        status = mNfcNciImpl_->NfaStartRfDiscovery();
    } else {
        status = mNfcNciImpl_->NfaStopRfDiscovery();
    }
    if (status == NFA_STATUS_OK) {
        mPollingEvent_.Wait();  // wait for NFA_RF_DISCOVERY_xxxx_EVT
        mRfEnabled_ = isStart;
    } else {
        DebugLog("NciBalManager::StartRfDiscovery: Failed to start/stop RF discovery; error=0x%X", status);
    }
    NciBalTag::GetInstance().EndRfField();
}

tNFA_STATUS NciBalManager::StartPolling(tNFA_TECHNOLOGY_MASK techMask) const
{
    DebugLog("NciBalManager::StartPolling, techMask = 0x%02X", techMask);
    NciBalTag::GetInstance().StartRfField();
    SynchronizeGuard guard(mPollingEvent_);
    tNFA_STATUS status = mNfcNciImpl_->NfaEnablePolling(techMask);
    if (status == NFA_STATUS_OK) {
        DebugLog("StartPolling: wait for enable event");
        mPollingEnabled_ = true;
        mPollingEvent_.Wait();  // wait for NFA_POLL_ENABLED_EVT
    } else {
        DebugLog("NciBalManager::StartPolling: fail enable polling; error = 0x%X", status);
    }
    NciBalTag::GetInstance().EndRfField();
    return status;
}

tNFA_STATUS NciBalManager::StopPolling() const
{
    DebugLog("NciBalManager::StopPolling");
    NciBalTag::GetInstance().StartRfField();
    SynchronizeGuard guard(mPollingEvent_);
    tNFA_STATUS status = mNfcNciImpl_->NfaDisablePolling();
    if (status == NFA_STATUS_OK) {
        mPollingEnabled_ = false;
        mPollingEvent_.Wait();  // wait for NFA_POLL_DISABLED_EVT
    } else {
        DebugLog("NciBalManager::StopPolling: fail disable polling; error = 0x%X", status);
    }
    NciBalTag::GetInstance().EndRfField();
    return status;
}

void NciBalManager::NfcConnectionCallback(uint8_t connEvent, tNFA_CONN_EVT_DATA* eventData)
{
    tNFA_STATUS status = NFA_STATUS_FAILED;

    switch (connEvent) {
        /* whether polling successfully started */
        case NFA_POLL_ENABLED_EVT: {
            DebugLog("NfaConnectionCallback: NFA_POLL_ENABLED_EVT: status = %u", eventData->status);
            SynchronizeGuard guard(mPollingEvent_);
            mPollingEvent_.NotifyOne();
            break;
        }
        /* Listening/Polling stopped */
        case NFA_POLL_DISABLED_EVT: {
            DebugLog("NfaConnectionCallback: NFA_POLL_DISABLED_EVT: status = %u", eventData->status);
            SynchronizeGuard guard(mPollingEvent_);
            mPollingEvent_.NotifyOne();
            break;
        }
        /* RF Discovery started event */
        case NFA_RF_DISCOVERY_STARTED_EVT: {
            DebugLog("NfaConnectionCallback: NFA_RF_DISCOVERY_STARTED_EVT: status = %u", eventData->status);
            SynchronizeGuard guard(mPollingEvent_);
            mPollingEvent_.NotifyOne();
            break;
        }
        /* RF Discovery stopped event */
        case NFA_RF_DISCOVERY_STOPPED_EVT: {
            DebugLog("NfaConnectionCallback: NFA_RF_DISCOVERY_STOPPED_EVT: status = %u", eventData->status);
            SynchronizeGuard guard(mPollingEvent_);
            mPollingEvent_.NotifyOne();
            break;
        }
        /* NFC link/protocol activated */
        case NFA_ACTIVATED_EVT: {
            DebugLog("NfaConnectionCallback: NFA_ACTIVATED_EVT");
            if (mIsDisabling_) {
                break;
            }
            if (eventData->activated.activate_ntf.protocol == NCI_PROTOCOL_NFC_DEP) {
                DebugLog("Is peer to peer");
                break;
            }
            if (eventData->activated.activate_ntf.rf_tech_param.mode < NCI_DISCOVERY_TYPE_LISTEN_A &&
                eventData->activated.activate_ntf.intf_param.type != NFC_INTERFACE_EE_DIRECT_RF) {
                mIsTagActive_ = true;
                /* Is polling and is not ee direct rf */
                if (mIsReconnect_) {
                    DebugLog("sIsReconnect, %d", mIsReconnect_);
                    NciBalTag::GetInstance().HandleActivatedResult();
                    break;
                }
                NciBalTag::GetInstance().ResetIsTagPresent();
                NciBalTag::GetInstance().CreateTagEndPoint(eventData);
            }
            break;
        }
        /* NFC link/protocol deactivated */
        case NFA_DEACTIVATED_EVT: {
            DebugLog("NfaConnectionCallback: NFA_DEACTIVATED_EVT");
            if (eventData->deactivated.type == NFA_DEACTIVATE_TYPE_SLEEP) {
                DebugLog("Enter sleep mode");
                mIsReconnect_ = true;
                NciBalTag::GetInstance().HandleDeactivatedResult();
                break;
            }
            mIsTagActive_ = false;
            mIsReconnect_ = false;
            NciBalTag::GetInstance().ResetTag();
            break;
        }
        case NFA_DISC_RESULT_EVT: {
            status = eventData->disc_result.status;
            DebugLog("NfaConnectionCallback: NFA_DISC_RESULT_EVT: status = 0x%X", status);
            if (status == NFA_STATUS_OK) {
                NciBalTag::GetInstance().HandleDiscResult(eventData);
            }
            break;
        }
        case NFA_SELECT_RESULT_EVT: {
            DebugLog("NfaConnectionCallback: NFA_SELECT_RESULT_EVT: status = 0x%X", eventData->status);
            NciBalTag::GetInstance().HandleSelectResult();
            break;
        }
        /* Data message received (for non-NDEF reads) */
        case NFA_DATA_EVT: {
            DebugLog(
                "NfaConnectionCallback: NFA_DATA_EVT: status = 0x%X, len = %d", eventData->status, eventData->data.len);
            NciBalTag::GetInstance().HandleTranceiveData(
                eventData->status, eventData->data.p_data, eventData->data.len);
            break;
        }
        case NFA_PRESENCE_CHECK_EVT: {
            static tNFA_STATUS curStatus = NFA_STATUS_FAILED;
            if (eventData->status != curStatus) {
                DebugLog("NfaConnectionCallback: NFA_PRESENCE_CHECK_EVT: status = 0x%X", eventData->status);
                curStatus = eventData->status;
            }
            NciBalTag::GetInstance().HandlePresenceCheckResult(eventData->status);
            break;
        }
        case NFA_READ_CPLT_EVT: {
            DebugLog("NfaConnectionCallback: NFA_READ_CPLT_EVT: status = 0x%X", eventData->status);
            NciBalTag::GetInstance().HandleReadComplete(eventData->status);
            break;
        }
        case NFA_WRITE_CPLT_EVT: {
            DebugLog("NfaConnectionCallback: NFA_WRITE_CPLT_EVT: status = 0x%X", eventData->status);
            NciBalTag::GetInstance().HandleWriteComplete(eventData->status);
            break;
        }
        case NFA_FORMAT_CPLT_EVT: {
            DebugLog("NfaConnectionCallback: NFA_FORMAT_CPLT_EVT: status = 0x%X", eventData->status);
            NciBalTag::GetInstance().HandleFormatComplete(eventData->status);
            break;
        }
        case NFA_NDEF_DETECT_EVT: {
            DebugLog(
                "NfaConnectionCallback: NFA_NDEF_DETECT_EVT: status = 0x%X, protocol = 0x%X, max_size = %u, cur_size "
                "= %u, flags = 0x%X",
                eventData->ndef_detect.status,
                eventData->ndef_detect.protocol,
                eventData->ndef_detect.max_size,
                eventData->ndef_detect.cur_size,
                eventData->ndef_detect.flags);
            NciBalTag::GetInstance().HandleNdefCheckResult(eventData->ndef_detect.status,
                                                           eventData->ndef_detect.cur_size,
                                                           eventData->ndef_detect.flags,
                                                           eventData->ndef_detect.max_size);
            break;
        }
        default: {
            DebugLog("NfaConnectionCallback: unknown event %u", connEvent);
            break;
        }
    }
}

void NciBalManager::NfcDeviceManagementCallback(uint8_t dmEvent, tNFA_DM_CBACK_DATA* eventData)
{
    DebugLog("NfaDeviceManagementCallback: event= %u", dmEvent);

    switch (dmEvent) {
        /* Result of NFA_Enable */
        case NFA_DM_ENABLE_EVT: {
            SynchronizeGuard guard(mNfcEnableEvent_);
            DebugLog("NfaDeviceManagementCallback: NFA_DM_ENABLE_EVT; status = 0x%X", eventData->status);
            mIsNfcEnabled_ = (eventData->status == NFA_STATUS_OK);
            mIsDisabling_ = false;
            mNfcEnableEvent_.NotifyOne();
            break;
        }
        /* Result of NFA_Disable */
        case NFA_DM_DISABLE_EVT: {
            SynchronizeGuard guard(mNfcDisableEvent_);
            DebugLog("NfaDeviceManagementCallback: NFA_DM_DISABLE_EVT");
            mIsNfcEnabled_ = false;
            mIsDisabling_ = false;
            mNfcDisableEvent_.NotifyOne();
            break;
        }

        case NFA_DM_RF_FIELD_EVT: {
            DebugLog("NfaDeviceManagementCallback: NFA_DM_RF_FIELD_EVT; status = 0x%X; field status = %u",
                     eventData->rf_field.status,
                     eventData->rf_field.rf_field_status);
            if (eventData->rf_field.status == NFA_STATUS_OK) {
                if (eventData->rf_field.rf_field_status == NFA_DM_RF_FIELD_ON) {
                    DeviceHost::RemoteFieldActivated();
                } else {
                    DeviceHost::RemoteFieldDeactivated();
                }
            }
            break;
        }

        case NFA_DM_NFCC_TRANSPORT_ERR_EVT:
        case NFA_DM_NFCC_TIMEOUT_EVT: {
            NciBalTag::GetInstance().AbortWait();
            {
                DebugLog("NfaDeviceManagementCallback: aborting sNfaEnableDisablePollingEvent");
                SynchronizeGuard guard(mPollingEvent_);
                mPollingEvent_.NotifyOne();
            }
            {
                DebugLog("NfaDeviceManagementCallback: aborting  sNfaEnableEvent");
                SynchronizeGuard guard(mNfcEnableEvent_);
                mNfcEnableEvent_.NotifyOne();
            }
            {
                DebugLog("NfaDeviceManagementCallback: aborting  sNfaDisableEvent");
                SynchronizeGuard guard(mNfcDisableEvent_);
                mNfcDisableEvent_.NotifyOne();
            }
            mDiscoveryEnabled_ = false;
            mPollingEnabled_ = false;

            if (IsNfcActive()) {
                mNfcNciImpl_->NfaDisable(FALSE);
                mIsDisabling_ = true;
            } else {
                mIsNfcEnabled_ = false;
                mIsDisabling_ = false;
            }
            break;
        }

        case NFA_DM_SET_CONFIG_EVT: {
            DebugLog("NfaDeviceManagementCallback: NFA_DM_SET_CONFIG_EVT");
            SynchronizeGuard guard(setConfigEvent_);
            setConfigEvent_.NotifyOne();
            break;
        }

        case NFA_DM_SET_POWER_SUB_STATE_EVT: {
            DebugLog("NfaDeviceManagementCallback: NFA_DM_SET_POWER_SUB_STATE_EVT; status=0x%X",
                     eventData->power_sub_state.status);
            SynchronizeGuard guard(setScreenStateEvent_);
            setScreenStateEvent_.NotifyOne();
            break;
        }
        
        default: {
            DebugLog("NfaDeviceManagementCallback: unknown event %d", dmEvent);
            break;
        }
    }
}

bool NciBalManager::Initialize()
{
    DebugLog("NciBalManager::Initialize");
    tNFA_STATUS status;
    std::lock_guard<std::mutex> lock(mMutex_);
    if (mIsNfcEnabled_) {
        DebugLog("NciBalManager::Initialize: already enabled");
        return mIsNfcEnabled_;
    }

    mNfcNciImpl_->NfcAdaptationInitialize();  // start GKI, NCI task, NFC task
    {
        SynchronizeGuard guard(mNfcEnableEvent_);
        tHAL_NFC_ENTRY* halFuncEntries = mNfcNciImpl_->NfcAdaptationGetHalEntryFuncs();

        mNfcNciImpl_->NfaInit(halFuncEntries);
        status = mNfcNciImpl_->NfaEnable(NfcDeviceManagementCallback, NfcConnectionCallback);
        if (status == NFA_STATUS_OK) {
            mNfcEnableEvent_.Wait();  // wait for NFA command to finish
        }
    }

    if (status == NFA_STATUS_OK) {
        // sIsNfaEnabled indicates whether stack started successfully
        if (mIsNfcEnabled_) {
            NciBalTag::GetInstance().RegisterNdefHandler();
#ifdef _NFC_SERVICE_HCE_
            NciBalCe::GetInstance().InitializeCe();
            HciManager::GetInstance().Initialize();
#endif
            mDiscoveryDuration_ = DEFAULT_DISCOVERY_DURATION;
            mNfcNciImpl_->NfaSetRfDiscoveryDuration((uint16_t)mDiscoveryDuration_);
            DebugLog("NciBalManager::Initialize: nfc enabled = %x", mIsNfcEnabled_);
            return mIsNfcEnabled_;
        }
    }
    ErrorLog("NciBalManager::Initialize: fail nfa enable; error = 0x%X", status);
    if (mIsNfcEnabled_) {
        /* ungraceful */
        status = mNfcNciImpl_->NfaDisable(false);
    }
    mNfcNciImpl_->NfcAdaptationFinalize();
    DebugLog("NciBalManager::Initialize: nfc enabled = %x", mIsNfcEnabled_);
    return mIsNfcEnabled_;
}

bool NciBalManager::Deinitialize()
{
    DebugLog("NciBalManager::Deinitialize");
    if (!IsNfcActive()) {
        DebugLog("NciBalManager::Deinitialize: Nfc not initialized");
        return NFA_STATUS_OK;
    }

    std::lock_guard<std::mutex> lock(mMutex_);
    tNFA_STATUS status = NFA_STATUS_OK;
    mIsDisabling_ = true;

#ifdef _NFC_SERVICE_HCE_
    NciBalCe::GetInstance().Deinitialize();
#endif

    // Stop the discovery before calling NFA_Disable.
    if (mRfEnabled_) {
        StartRfDiscovery(false);
    }

    if (mIsNfcEnabled_) {
        SynchronizeGuard guard(mNfcDisableEvent_);
        /* graceful */
        status = mNfcNciImpl_->NfaDisable(true);
        if (status == NFA_STATUS_OK) {
            DebugLog("NciBalManager::Deinitialize: wait for completion");
            mNfcDisableEvent_.Wait();  // wait for NFA command to finish
        } else {
            DebugLog("NciBalManager::Deinitialize: fail disable; error = 0x%X", status);
        }
    }
    mIsNfcEnabled_ = false;
    mDiscoveryEnabled_ = false;
    mIsDisabling_ = false;
    mPollingEnabled_ = false;
    NciBalTag::GetInstance().AbortWait();
    {
        // unblock NFA_EnablePolling() and NFA_DisablePolling()
        SynchronizeGuard guard(mPollingEvent_);
        mPollingEvent_.NotifyOne();
    }

    mNfcNciImpl_->NfcAdaptationFinalize();
    DebugLog("NciBalManager::Deinitialize: exit");
    return (status == NFA_STATUS_OK);
}

void NciBalManager::EnableDiscovery(int techMask, bool enableReaderMode, bool enableHostRouting, bool restart)
{
    DebugLog("NciBalManager::EnableDiscovery");
    std::lock_guard<std::mutex> lock(mMutex_);
    if (!IsNfcActive()) {
        ErrorLog("NciBalManager::EnableDiscovery: Nfc not initialized.");
        return;
    }

    if (mDiscoveryEnabled_ && !restart) {
        DebugLog("NciBalManager::EnableDiscovery: already discovering");
        return;
    }

    if (mRfEnabled_) {
        // Stop RF discovery to reconfigure
        StartRfDiscovery(false);
    }

    tNFA_TECHNOLOGY_MASK technologyMask = DEFAULT_TECH_MASK;
    if (techMask != -1) {
        technologyMask = techMask & DEFAULT_TECH_MASK;
    }

    if (technologyMask != 0) {
        StopPolling();
        StartPolling(technologyMask);
        if (mPollingEnabled_) {
            if (enableReaderMode && !mReaderModeEnabled_) {
                mReaderModeEnabled_ = true;
                mNfcNciImpl_->NfaDisableListening();
                mNfcNciImpl_->NfaSetRfDiscoveryDuration(DISCOVERY_DURATION);
            } else if (!enableReaderMode && mReaderModeEnabled_) {
                mReaderModeEnabled_ = false;
                mNfcNciImpl_->NfaEnableListening();
                mNfcNciImpl_->NfaSetRfDiscoveryDuration(DISCOVERY_DURATION);
            }
        }
    } else {
        StopPolling();
    }
#ifdef _NFC_SERVICE_HCE_
    NciBalCe::GetInstance().EnableHostRouting(enableHostRouting);
    NciBalCe::GetInstance().CommitRouting();
#endif

    StartRfDiscovery(true);
    mDiscoveryEnabled_ = true;
    mIsReconnect_ = false;
    DebugLog("NciBalManager::EnableDiscovery: exit");
}

void NciBalManager::DisableDiscovery()
{
    DebugLog("NciBalManager::DisableDiscovery");
    std::lock_guard<std::mutex> lock(mMutex_);
    if (!IsNfcActive()) {
        ErrorLog("NciBalManager::DisableDiscovery: Nfc not initialized.");
        return;
    }
    if (mDiscoveryEnabled_ == false) {
        DebugLog("NciBalManager::DisableDiscovery: already disabled");
        return;
    }
    // Stop RF Discovery.
    StartRfDiscovery(false);
    if (mPollingEnabled_) {
        StopPolling();
    }
    mDiscoveryEnabled_ = false;
    mReaderModeEnabled_ = false;
    DebugLog("NciBalManager::DisableDiscovery: exit");
}

bool NciBalManager::SendRawFrame(std::string& rawData)
{
    DebugLog("NciBalManager::SendRawFrame");
    std::lock_guard<std::mutex> lock(mMutex_);
    mNfcNciImpl_->NfaSendRawFrame((uint8_t*)rawData.c_str(), (uint16_t)rawData.length(), 0);
    return true;
}

void NciBalManager::SetScreenStatus(unsigned char screenStateMask) const
{
    DebugLog("NciBalManager::SetScreenStatus");
    unsigned char screenState = screenStateMask & NFA_SCREEN_STATE_MASK;
    if (curScreenState_ == screenState) {
        DebugLog("Screen state not changed");
        return;
    }
    if (!IsNfcActive() || GetNciVersion() != NCI_VERSION_2_0) {
        curScreenState_ = screenState;
        return;
    }

    // NCI_VERSION_2_0
    tNFA_STATUS status;
    if (curScreenState_ == NFA_SCREEN_STATE_OFF_LOCKED || curScreenState_ == NFA_SCREEN_STATE_OFF_UNLOCKED ||
        curScreenState_ == NFA_SCREEN_STATE_ON_LOCKED || curScreenState_ == NFA_SCREEN_STATE_UNKNOWN) {
        SynchronizeGuard guard(setScreenStateEvent_);
        status = mNfcNciImpl_->NfcSetPowerSubStateForScreenState(screenState);
        if (status != NFA_STATUS_OK) {
            DebugLog("NFA_SetPowerSubStateForScreenState fail, error=0x%X", status);
            return;
        }
        setScreenStateEvent_.Wait();
    }

    SynchronizeGuard guard(setConfigEvent_);
    uint8_t discovryParam = NCI_POLLING_DH_ENABLE_MASK | NCI_LISTEN_DH_NFCEE_ENABLE_MASK;
    if (screenState == NFA_SCREEN_STATE_OFF_LOCKED || screenState == NFA_SCREEN_STATE_OFF_UNLOCKED) {
        discovryParam = NCI_POLLING_DH_DISABLE_MASK | NCI_LISTEN_DH_NFCEE_ENABLE_MASK;
    }
    if (screenState == NFA_SCREEN_STATE_ON_LOCKED) {
        discovryParam = (screenStateMask & NFA_SCREEN_POLLING_TAG_MASK)
                            ? (NCI_POLLING_DH_ENABLE_MASK | NCI_LISTEN_DH_NFCEE_ENABLE_MASK)
                            : (NCI_POLLING_DH_DISABLE_MASK | NCI_LISTEN_DH_NFCEE_ENABLE_MASK);
    }
    if (screenState == NFA_SCREEN_STATE_ON_UNLOCKED) {
        discovryParam = NCI_POLLING_DH_ENABLE_MASK | NCI_LISTEN_DH_NFCEE_ENABLE_MASK;
    }
    status =
        mNfcNciImpl_->NfcSetConfig(NCI_PARAM_ID_CON_DISCOVERY_PARAM, NCI_PARAM_LEN_CON_DISCOVERY_PARAM, &discovryParam);
    if (status != NFA_STATUS_OK) {
        DebugLog("NFA_SetConfig fail, error=0x%X", status);
        return;
    }
    setConfigEvent_.Wait();

    if (curScreenState_ == NFA_SCREEN_STATE_ON_UNLOCKED) {
        SynchronizeGuard guard(setScreenStateEvent_);
        status = mNfcNciImpl_->NfcSetPowerSubStateForScreenState(screenState);
        if (status != NFA_STATUS_OK) {
            DebugLog("NFA_SetPowerSubStateForScreenState fail, error=0x%X", status);
            return;
        }
        setScreenStateEvent_.Wait();
    }
    curScreenState_ = screenState;
    return;
}

int NciBalManager::GetNciVersion() const
{
    DebugLog("NciBalManager::GetNciVersion");
    unsigned char version = mNfcNciImpl_->NfcGetNciVersion();
    return version;
}

int NciBalManager::GetIsoDepMaxTransceiveLength()
{
    DebugLog("NciBalManager::GetIsoDepMaxTransceiveLength");
    /* The maximum length of a default IsoDep frame consists of:
     * CLA, INS, P1, P2, LC, LE + 255 payload bytes = 261 bytes
     */
    return NfcConfig::getUnsigned(NAME_ISO_DEP_MAX_TRANSCEIVE, 261);
    ;
}

bool NciBalManager::RegisterT3tIdentifier(const std::string& lfT3tIdentifier) const
{
    DebugLog("NciBalManager::RegisterT3tIdentifier");
    if (lfT3tIdentifier.empty()) {
    }
    return false;
}

void NciBalManager::DeregisterT3tIdentifier(int handle) const
{
    DebugLog("NciBalManager::DeregisterT3tIdentifier");
    if (handle < 0) {
    }
}

void NciBalManager::ClearT3tIdentifiersCache()
{
    DebugLog("NciBalManager::ClearT3tIdentifiersCache");
}

int NciBalManager::GetLfT3tMax()
{
    DebugLog("NciBalManager::GetLfT3tMax");
    return 0;
}

int NciBalManager::GetLastError()
{
    DebugLog("NciBalManager::GetLastError");
    return 0;
}

void NciBalManager::Abort()
{
    DebugLog("NciBalManager::Abort");
    {
        SynchronizeGuard guard(mNfcEnableEvent_);
        mNfcEnableEvent_.NotifyOne();
    }
    {
        SynchronizeGuard guard(mNfcDisableEvent_);
        mNfcDisableEvent_.NotifyOne();
    }
    {
        SynchronizeGuard guard(mPollingEvent_);
        mPollingEvent_.NotifyOne();
    }
    {
        SynchronizeGuard guard(setScreenStateEvent_);
        setScreenStateEvent_.NotifyOne();
    }
    {
        SynchronizeGuard guard(setConfigEvent_);
        setConfigEvent_.NotifyOne();
    }
    NciBalTag::GetInstance().AbortWait();
#ifdef _NFC_SERVICE_HCE_
    NciBalCe::GetInstance().AbortWait();
#endif
}

bool NciBalManager::CheckFirmware()
{
    DebugLog("NciBalManager::CheckFirmware");
    std::lock_guard<std::mutex> lock(mMutex_);
    mNfcNciImpl_->NfcAdaptationInitialize();
    mNfcNciImpl_->NfcAdaptationDownloadFirmware();
    mNfcNciImpl_->NfcAdaptationFinalize();
    return true;
}

void NciBalManager::Dump(int fd) const
{
    DebugLog("NciBalManager::Dump, fd=%d", fd);
    mNfcNciImpl_->NfcAdaptationDump(fd);
}

void NciBalManager::FactoryReset() const
{
    DebugLog("NciBalManager::FactoryReset");
    mNfcNciImpl_->NfcAdaptationFactoryReset();
}

void NciBalManager::Shutdown() const
{
    DebugLog("NciBalManager::Shutdown");
    mNfcNciImpl_->NfcAdaptationDeviceShutdown();
}

bool NciBalManager::IsRfEbabled()
{
    return mRfEnabled_;
}
}  // namespace ncibal
}  // namespace nfc
}  // namespace OHOS