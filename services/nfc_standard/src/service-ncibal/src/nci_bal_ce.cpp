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
#include "nci_bal_ce.h"

#include <mutex>

#include "device_host.h"
#include "hci_manager.h"
#include "infc_nci.h"
#include "loghelper.h"
#include "nfc_config.h"
#include "nfc_nci_impl.h"
#include "utils/synchronize_event.h"

namespace OHOS {
namespace nfc {
namespace ncibal {
OHOS::nfc::SynchronizeEvent NciBalCe::mEeRegisterEvent_;
OHOS::nfc::SynchronizeEvent NciBalCe::mAidEvent_;
OHOS::nfc::SynchronizeEvent NciBalCe::mRoutingCfgEvent_;
OHOS::nfc::SynchronizeEvent NciBalCe::mCommitRoutingEvent_;
OHOS::nfc::SynchronizeEvent NciBalCe::mModeSetEvent_;
OHOS::nfc::SynchronizeEvent NciBalCe::remainSizeEvent_;

bool NciBalCe::mAidRoutingConfigured_ = false;
tNFA_EE_DISCOVER_REQ NciBalCe::mEeInfo_{};
std::string NciBalCe::mHceData_{};
std::shared_ptr<INfcNci> NciBalCe::mNfcNciImpl_ = std::make_shared<NfcNciImpl>();
int NciBalCe::remainTableSize_ = 0;

void NciBalCe::NfcEeCallback(tNFA_EE_EVT event, tNFA_EE_CBACK_DATA* eventData)
{
    DebugLog("NfcEeCallback: event = %u", event);
    switch (event) {
        case NFA_EE_REGISTER_EVT: {
            DebugLog("NfcEeCallback: NFA_EE_REGISTER_EVT status=%u", eventData->ee_register);
            SynchronizeGuard guard(mEeRegisterEvent_);
            mEeRegisterEvent_.NotifyOne();
            break;
        }

        case NFA_EE_SET_TECH_CFG_EVT: {
            DebugLog("NfcEeCallback: NFA_EE_SET_TECH_CFG_EVT status=%u", eventData->status);
            SynchronizeGuard guard(mRoutingCfgEvent_);
            mRoutingCfgEvent_.NotifyOne();
            break;
        }

        case NFA_EE_CLEAR_TECH_CFG_EVT: {
            DebugLog("NfcEeCallback: NFA_EE_CLEAR_TECH_CFG_EVT status=%u", eventData->status);
            SynchronizeGuard guard(mRoutingCfgEvent_);
            mRoutingCfgEvent_.NotifyOne();
            break;
        }

        case NFA_EE_SET_PROTO_CFG_EVT: {
            DebugLog("NfcEeCallback: NFA_EE_SET_PROTO_CFG_EVT status=%u", eventData->status);
            SynchronizeGuard guard(mRoutingCfgEvent_);
            mRoutingCfgEvent_.NotifyOne();
            break;
        }

        case NFA_EE_CLEAR_PROTO_CFG_EVT: {
            DebugLog("NfcEeCallback: NFA_EE_CLEAR_PROTO_CFG_EVT status=%u", eventData->status);
            SynchronizeGuard guard(mRoutingCfgEvent_);
            mRoutingCfgEvent_.NotifyOne();
            break;
        }

        case NFA_EE_ADD_AID_EVT: {
            DebugLog("NfcEeCallback: NFA_EE_ADD_AID_EVT status=%u", eventData->status);
            SynchronizeGuard guard(mAidEvent_);
            mAidRoutingConfigured_ = (eventData->status == NFA_STATUS_OK);
            mAidEvent_.NotifyOne();
            break;
        }

        case NFA_EE_REMOVE_AID_EVT: {
            DebugLog("NfcEeCallback: NFA_EE_REMOVE_AID_EVT status=%u", eventData->status);
            SynchronizeGuard guard(mAidEvent_);
            mAidRoutingConfigured_ = (eventData->status == NFA_STATUS_OK);
            mAidEvent_.NotifyOne();
            break;
        }

        case NFA_EE_UPDATED_EVT: {
            DebugLog("NfcEeCallback: NFA_EE_UPDATED_EVT");
            SynchronizeGuard guard(mCommitRoutingEvent_);
            mCommitRoutingEvent_.NotifyOne();
            break;
        }

        case NFA_EE_DISCOVER_REQ_EVT: {
            DebugLog("NfcEeCallback: NFA_EE_DISCOVER_REQ_EVT status=0x%X num=%u",
                     eventData->discover_req.status,
                     eventData->discover_req.num_ee);
            mEeInfo_ = eventData->discover_req;
            DeviceHost::EeUpdate();
            break;
        }

        case NFA_EE_MODE_SET_EVT: {
            DebugLog("NfcEeCallback: NFA_EE_MODE_SET_EVT status=%u", eventData->mode_set.status);
            SynchronizeGuard guard(mModeSetEvent_);
            mModeSetEvent_.NotifyOne();
            break;
        }

        case NFA_EE_REMAINING_SIZE_EVT: {
            DebugLog("NfcEeCallback: NFA_EE_REMAINING_SIZE_EVT size=%u", eventData->size);
            SynchronizeGuard guard(remainSizeEvent_);
            remainTableSize_ = eventData->size;
            remainSizeEvent_.NotifyOne();
            break;
        }

        default: {
            DebugLog("NfcEeCallback: unknown event %d", event);
            break;
        }
    }
}

void NciBalCe::NfcCeCallback(uint8_t event, tNFA_CONN_EVT_DATA* eventData)
{
    DebugLog("NfcCeCallback: event = %u", event);
    switch (event) {
        case NFA_CE_ACTIVATED_EVT: {
            tNFC_PROTOCOL protocol = eventData->ce_activated.activate_ntf.protocol;
            DebugLog("NfcCeCallback: NFA_CE_ACTIVATED_EVT protocol=0x%X", protocol);
            DeviceHost::HostCardEmulationActivated(NFA_TECHNOLOGY_MASK_A);
            break;
        }

        case NFA_CE_DEACTIVATED_EVT:
        case NFA_DEACTIVATED_EVT: {
            DebugLog("NfcCeCallback: NFA_DEACTIVATED_EVT");
            DeviceHost::HostCardEmulationDeactivated(NFA_TECHNOLOGY_MASK_A);
            break;
        }

        case NFA_CE_DATA_EVT: {
            tNFA_CE_DATA& ceData = eventData->ce_data;
            DebugLog("NfcCeCallback: NFA_CE_DATA_EVT status=0x%X handle=0x%X len=%u",
                     ceData.status,
                     ceData.handle,
                     ceData.len);
            HandleHostCardEmulationData(NFA_TECHNOLOGY_MASK_A, ceData.p_data, ceData.len, ceData.status);
            break;
        }

        default: {
            DebugLog("NfcCeCallback: unknown event %d", event);
            break;
        }
    }
}

NciBalCe::NciBalCe()
    : mNfcSecure_(false),
      mDefaultRoute_(NfcConfig::getUnsigned(NAME_DEFAULT_ROUTE, 0x00)),
      mDefaultOffHostRoute_(NfcConfig::getUnsigned(NAME_DEFAULT_OFFHOST_ROUTE, 0x00)),
      mDefaultIsoDepRoute_(NfcConfig::getUnsigned(NAME_DEFAULT_ISODEP_ROUTE, 0x00)),
      mAidMatchingMode_(NfcConfig::getUnsigned(NAME_AID_MATCHING_MODE, AID_ROUTE_EXACT_ONLY)),
      mOffHostAidRoutingPowerState_(NfcConfig::getUnsigned(NAME_OFFHOST_AID_ROUTE_PWR_STATE, 0x01)),
      mHostListenTech_(
          NfcConfig::getUnsigned(NAME_HOST_LISTEN_TECH_MASK, NFA_TECHNOLOGY_MASK_A | NFA_TECHNOLOGY_MASK_F)),
      mSeTech_(0x00)
{
    if (NfcConfig::hasKey(NAME_OFFHOST_ROUTE_UICC)) {
        mDefaultOffHostUiccRoute_ = NfcConfig::getBytes(NAME_OFFHOST_ROUTE_UICC);
    }
    if (NfcConfig::hasKey(NAME_OFFHOST_ROUTE_ESE)) {
        mDefaultOffHostEseRoute_ = NfcConfig::getBytes(NAME_OFFHOST_ROUTE_ESE);
    }
}

NciBalCe::~NciBalCe() = default;

NciBalCe& NciBalCe::GetInstance()
{
    static NciBalCe nciBalCe;
    return nciBalCe;
}

bool NciBalCe::InitializeCe()
{
    DebugLog("NciBalCe::InitializeCe");
    {
        SynchronizeGuard guard(mEeRegisterEvent_);
        tNFA_STATUS status = mNfcNciImpl_->NfcEeRegister(NfcEeCallback);
        if (status != NFA_STATUS_OK) {
            ErrorLog("EE register fail %d", status);
            return false;
        }
        mEeRegisterEvent_.Wait();
    }

    uint8_t actualNumEe = NFA_EE_MAX_EE_SUPPORTED;
    tNFA_EE_INFO eeInfo[NFA_EE_MAX_EE_SUPPORTED];
    tNFA_STATUS status = mNfcNciImpl_->NfcEeGetInfo(&actualNumEe, eeInfo);
    if (status != NFA_STATUS_OK) {
        ErrorLog("Fail get eeInfo, status = 0x%02X", status);
    } else {
        for (uint8_t i = 0; i < actualNumEe; i++) {
            if (eeInfo[i].ee_status == NFA_EE_STATUS_INACTIVE) {
                DebugLog("Try to active ee{0x%02X}", eeInfo[i].ee_handle);
                SynchronizeGuard guard(mModeSetEvent_);
                tNFA_STATUS stat = mNfcNciImpl_->NfcEeModeSet(eeInfo[i].ee_handle, NFA_EE_MD_ACTIVATE);
                if (stat == NFA_STATUS_OK) {
                    mModeSetEvent_.Wait();
                } else {
                    ErrorLog("Fail to set ee activate");
                }
            }
        }
    }

    mSeTech_ = UpdateEeTech();
    // Listen on Nfc-A/Nfc-B
    mNfcNciImpl_->NfcCeSetIsoDepListenTech(mHostListenTech_ & (NFA_TECHNOLOGY_MASK_A | NFA_TECHNOLOGY_MASK_B));

    status = mNfcNciImpl_->NfcCeRegisterAidOnDH(NULL, 0, NfcCeCallback);
    if (status != NFA_STATUS_OK) {
        ErrorLog("Failed to register wildcard AID for DH");
    }

    UpdateDefaultRoute();
    UpdateEeProto();
    return true;
}

tNFA_TECHNOLOGY_MASK NciBalCe::UpdateEeTech() const
{
    DebugLog("NciBalCe::UpdateEeTech");
    tNFA_TECHNOLOGY_MASK seTechMask = 0x00;
    if (mDefaultOffHostRoute_ == NFC_DH_ID) {
        return seTechMask;
    }

    DebugLog("Number of ee is %d ", mEeInfo_.num_ee);
    tNFA_STATUS status;
    for (uint8_t i = 0; i < mEeInfo_.num_ee; i++) {
        tNFA_HANDLE eeHandle = mEeInfo_.ee_disc_info[i].ee_handle;
        tNFA_TECHNOLOGY_MASK iSeTechMask = 0x00;
        if ((mDefaultOffHostRoute_ != NFC_DH_ID) && (eeHandle == (mDefaultOffHostRoute_ | NFA_HANDLE_GROUP_EE))) {
            if (mEeInfo_.ee_disc_info[i].la_protocol != 0) {
                iSeTechMask |= NFA_TECHNOLOGY_MASK_A;
            }
            if (mEeInfo_.ee_disc_info[i].lb_protocol != 0) {
                iSeTechMask |= NFA_TECHNOLOGY_MASK_B;
            }
        }

        if (iSeTechMask != 0) {
            SynchronizeGuard guard(mRoutingCfgEvent_);
            status = mNfcNciImpl_->NfcEeClearDefaultTechRouting(eeHandle, iSeTechMask);
            if (status != NFA_STATUS_OK) {
                DebugLog("Ee Clear Default Tech Routing fail");
            } else {
                mRoutingCfgEvent_.Wait();
            }
            status = mNfcNciImpl_->NfcEeSetDefaultTechRouting(eeHandle,
                                                              iSeTechMask,
                                                              mNfcSecure_ ? 0 : iSeTechMask,
                                                              0,
                                                              mNfcSecure_ ? 0 : iSeTechMask,
                                                              mNfcSecure_ ? 0 : iSeTechMask,
                                                              mNfcSecure_ ? 0 : iSeTechMask);
            if (status != NFA_STATUS_OK) {
                DebugLog("Ee Set Default Tech Routing fail");
            } else {
                mRoutingCfgEvent_.Wait();
            }
            seTechMask |= iSeTechMask;
        }
    }

    if ((mHostListenTech_ & NFA_TECHNOLOGY_MASK_A) && (seTechMask && NFA_TECHNOLOGY_MASK_A != 0)) {
        SynchronizeGuard guard(mRoutingCfgEvent_);
        status = mNfcNciImpl_->NfcEeClearDefaultTechRouting(NFC_DH_ID, NFA_TECHNOLOGY_MASK_A);
        if (status != NFA_STATUS_OK) {
            DebugLog("Ee Clear Default Tech Routing NFC-A fail");
        } else {
            mRoutingCfgEvent_.Wait();
        }
    }
    if ((mHostListenTech_ & NFA_TECHNOLOGY_MASK_B) && (seTechMask && NFA_TECHNOLOGY_MASK_B != 0)) {
        SynchronizeGuard guard(mRoutingCfgEvent_);
        status = mNfcNciImpl_->NfcEeClearDefaultTechRouting(NFC_DH_ID, NFA_TECHNOLOGY_MASK_B);
        if (status != NFA_STATUS_OK) {
            DebugLog("Ee Clear Default Tech Routing NFC-B fail");
        } else {
            mRoutingCfgEvent_.Wait();
        }
    }
    return seTechMask;
}

void NciBalCe::UpdateDefaultRoute()
{
    DebugLog("NciBalCe::UpdateDefaultRoute");
    if (mNfcNciImpl_->NfcGetNciVersion() != NCI_VERSION_2_0) {
        return;
    }

    uint8_t aidRoute = mDefaultRoute_;
    tNFA_HANDLE handle = HciManager::GetConnectedNfceeId(aidRoute);

    uint8_t powerState = 0x01;
    if (mNfcSecure_ == false) {
        powerState = (handle != NFC_DH_ID) ? mOffHostAidRoutingPowerState_ : 0x11;
    }

    SynchronizeGuard guard(mAidEvent_);
    mAidRoutingConfigured_ = false;
    tNFA_STATUS status = mNfcNciImpl_->NfcEeAddAidRouting(handle, 0, NULL, powerState, AID_ROUTE_QUAL_PREFIX);
    if (status == NFA_STATUS_OK) {
        mAidEvent_.Wait();
    }
    if (mAidRoutingConfigured_) {
        DebugLog("Added zero AID");
    } else {
        DebugLog("Failed to add zero AID");
    }
}

void NciBalCe::UpdateEeProto()
{
    DebugLog("NciBalCe::UpdateEeProto");
    tNFA_STATUS status;
    // Set default protocol route
    tNFA_PROTOCOL_MASK protoMask = NFA_PROTOCOL_MASK_ISO_DEP;
    tNFA_HANDLE isoDepRoute = HciManager::GetConnectedNfceeId(mDefaultOffHostRoute_);
    SynchronizeGuard guard(mRoutingCfgEvent_);
    {
        status = mNfcNciImpl_->NfcEeClearDefaultProtoRouting(isoDepRoute, protoMask);
        if (status == NFA_STATUS_OK) {
            mRoutingCfgEvent_.Wait();
        } else {
            DebugLog("Ee Clear Default Proto Routing fail");
        }
    }
    if (isoDepRoute != NFC_DH_ID) {
        status = mNfcNciImpl_->NfcEeSetDefaultProtoRouting(isoDepRoute,
                                                           protoMask,
                                                           mNfcSecure_ ? 0 : protoMask,
                                                           0,
                                                           mNfcSecure_ ? 0 : protoMask,
                                                           mNfcSecure_ ? 0 : protoMask,
                                                           mNfcSecure_ ? 0 : protoMask);
        if (status == NFA_STATUS_OK) {
            mRoutingCfgEvent_.Wait();
        }
    } else {
        status =
            mNfcNciImpl_->NfcEeSetDefaultProtoRouting(NFC_DH_ID, protoMask, 0, 0, mNfcSecure_ ? 0 : protoMask, 0, 0);
        if (status == NFA_STATUS_OK) {
            mRoutingCfgEvent_.Wait();
        }
    }
    if (status == NFA_STATUS_OK) {
        DebugLog("Succeed to register default ISO-DEP route");
    } else {
        DebugLog("Failed to register default ISO-DEP route");
    }
}

bool NciBalCe::AddAidRouting(std::string& aid, int route, int aidInfo)
{
    DebugLog("NciBalCe::AddAidRouting");
    SynchronizeGuard guard(mAidEvent_);
    uint8_t powerState = 0x01;
    if (mNfcSecure_ == false) {
        powerState = (route != NFC_DH_ID) ? mOffHostAidRoutingPowerState_ : 0x11;
    }
    mAidRoutingConfigured_ = false;
    tNFA_STATUS status = mNfcNciImpl_->NfcEeAddAidRouting(
        (tNFA_HANDLE)route, (uint8_t)aid.length(), (uint8_t*)aid.c_str(), powerState, (uint8_t)aidInfo);
    if (status == NFA_STATUS_OK) {
        mAidEvent_.Wait();
    }
    if (mAidRoutingConfigured_) {
        DebugLog("Added AID");
        return true;
    } else {
        ErrorLog("Failed to add AID");
        return false;
    }
}

bool NciBalCe::RemoveAidRouting(std::string& aid)
{
    DebugLog("NciBalCe::RemoveAidRouting");
    SynchronizeGuard guard(mAidEvent_);
    mAidRoutingConfigured_ = false;
    tNFA_STATUS status = mNfcNciImpl_->NfcEeRemoveAidRouting((uint8_t)aid.length(), (uint8_t*)aid.c_str());
    if (status == NFA_STATUS_OK) {
        mAidEvent_.Wait();
    }
    if (mAidRoutingConfigured_) {
        DebugLog("Removed AID");
        return true;
    } else {
        ErrorLog("Failed to remove AID");
        return false;
    }
}

bool NciBalCe::CommitRouting()
{
    DebugLog("NciBalCe::CommitRouting");
    SynchronizeGuard guard(mCommitRoutingEvent_);
    tNFA_STATUS status = mNfcNciImpl_->NfcEeUpdateNow();
    if (status == NFA_STATUS_OK) {
        mCommitRoutingEvent_.Wait();  // wait for NFA_EE_UPDATED_EVT
    }
    return (status == NFA_STATUS_OK);
}

int NciBalCe::GetAidRoutingTableSize()
{
    DebugLog("NciBalCe::GetAidRoutingTableSize");
    return mNfcNciImpl_->NfcGetAidTableSize();
}

void NciBalCe::EnableHostRouting(bool enable)
{
    if (enable == true) {
        DebugLog("Enable routing to host");
        EnableRoutingToHost();
    } else {
        DebugLog("Disable routing to host");
        DisableRoutingToHost();
    }
}

void NciBalCe::EnableRoutingToHost()
{
    SynchronizeGuard guard(mRoutingCfgEvent_);

    tNFA_PROTOCOL_MASK protoMask = NFA_PROTOCOL_MASK_ISO_DEP;
    if (mDefaultIsoDepRoute_ == NFC_DH_ID) {
        tNFA_STATUS status =
            mNfcNciImpl_->NfcEeSetDefaultProtoRouting(NFC_DH_ID, protoMask, 0, 0, mNfcSecure_ ? 0 : protoMask, 0, 0);
        if (status == NFA_STATUS_OK) {
            mRoutingCfgEvent_.Wait();
        } else {
            DebugLog("EnableRoutingToHost::NfcEeSetDefaultProtoRouting fail");
        }
    }

    tNFA_TECHNOLOGY_MASK techMask = NFA_TECHNOLOGY_MASK_A;
    if ((mHostListenTech_ & NFA_TECHNOLOGY_MASK_A) && (mSeTech_ & NFA_TECHNOLOGY_MASK_A) == 0) {
        tNFA_STATUS status =
            mNfcNciImpl_->NfcEeSetDefaultTechRouting(NFC_DH_ID, techMask, 0, 0, mNfcSecure_ ? 0 : techMask, 0, 0);
        if (status == NFA_STATUS_OK) {
            mRoutingCfgEvent_.Wait();
        } else {
            DebugLog("EnableRoutingToHost::NfcEeSetDefaultTechRouting NFC-A fail");
        }
    }

    techMask = NFA_TECHNOLOGY_MASK_B;
    if ((mHostListenTech_ & NFA_TECHNOLOGY_MASK_B) && (mSeTech_ & NFA_TECHNOLOGY_MASK_B) == 0) {
        tNFA_STATUS status =
            mNfcNciImpl_->NfcEeSetDefaultTechRouting(NFC_DH_ID, techMask, 0, 0, mNfcSecure_ ? 0 : techMask, 0, 0);
        if (status == NFA_STATUS_OK) {
            mRoutingCfgEvent_.Wait();
        } else {
            DebugLog("EnableRoutingToHost::NfcEeSetDefaultTechRouting NFC-B fail");
        }
    }
}

void NciBalCe::DisableRoutingToHost()
{
    SynchronizeGuard guard(mRoutingCfgEvent_);

    if (mDefaultIsoDepRoute_ == NFC_DH_ID) {
        tNFA_STATUS status = mNfcNciImpl_->NfcEeClearDefaultProtoRouting(NFC_DH_ID, NFA_PROTOCOL_MASK_ISO_DEP);
        if (status == NFA_STATUS_OK) {
            mRoutingCfgEvent_.Wait();
        } else {
            DebugLog("DisableRoutingToHost::NfcEeClearDefaultProtoRouting fail");
        }
    }

    if ((mHostListenTech_ & NFA_TECHNOLOGY_MASK_A) && (mSeTech_ & NFA_TECHNOLOGY_MASK_A) == 0) {
        tNFA_STATUS status = mNfcNciImpl_->NfcEeClearDefaultTechRouting(NFC_DH_ID, NFA_TECHNOLOGY_MASK_A);
        if (status == NFA_STATUS_OK) {
            mRoutingCfgEvent_.Wait();
        } else {
            DebugLog("DisableRoutingToHost::NfcEeClearDefaultTechRouting NFC-A fail");
        }
    }

    if ((mHostListenTech_ & NFA_TECHNOLOGY_MASK_B) && (mSeTech_ & NFA_TECHNOLOGY_MASK_B) == 0) {
        tNFA_STATUS status = mNfcNciImpl_->NfcEeClearDefaultTechRouting(NFC_DH_ID, NFA_TECHNOLOGY_MASK_B);
        if (status == NFA_STATUS_OK) {
            mRoutingCfgEvent_.Wait();
        } else {
            DebugLog("DisableRoutingToHost::NfcEeClearDefaultTechRouting NFC-B fail");
        }
    }
}

bool NciBalCe::SetSecureNfc(bool secure)
{
    DebugLog("NciBalCe::SetSecureNfc");
    mNfcSecure_ = secure;
    return true;
}

void NciBalCe::HandleHostCardEmulationData(uint8_t technology,
                                           const uint8_t* data,
                                           uint32_t dataLen,
                                           tNFA_STATUS status)
{
    if (status == NFC_STATUS_CONTINUE || status == NFA_STATUS_OK) {
        if (dataLen > 0) {
            mHceData_.append(std::string((const char*)data, dataLen));
        }
    } else {
        ErrorLog("NciBalCe::HandleHostCardEmulationData fail, status=%u", status);
        mHceData_ = "";
        return;
    }
    if (status == NFA_STATUS_OK) {
        DeviceHost::HostCardEmulationDataReceived(technology, mHceData_);
        mHceData_ = "";
    }
}

int NciBalCe::GetDefaultRoute() const
{
    DebugLog("NciBalCe::GetDefaultRoute");
    return mDefaultRoute_;
}

int NciBalCe::GetDefaultOffHostRoute() const
{
    DebugLog("NciBalCe::GetDefaultOffHostRoute");
    return mDefaultOffHostRoute_;
}

std::vector<int> NciBalCe::GetOffHostUiccRoute() const
{
    DebugLog("NciBalCe::GetOffHostUiccRoute");
    std::vector<int> uicc;
    for (auto i : mDefaultOffHostUiccRoute_) {
        uicc.push_back(i);
    }
    return uicc;
}

std::vector<int> NciBalCe::GetOffHostEseRoute() const
{
    DebugLog("NciBalCe::GetOffHostEseRoute");
    std::vector<int> ese;
    for (auto i : mDefaultOffHostEseRoute_) {
        ese.push_back(i);
    }
    return ese;
}

int NciBalCe::GetAidMatchingMode() const
{
    DebugLog("NciBalCe::GetAidMatchingMode");
    return mAidMatchingMode_;
}

int NciBalCe::GetDefaultIsoDepRouteDestination() const
{
    DebugLog("NciBalCe::GetDefaultIsoDepRouteDestination");
    return mDefaultIsoDepRoute_;
}

void NciBalCe::AbortWait()
{
    DebugLog("NciBalCe::AbortWait");
    {
        SynchronizeGuard guard(mEeRegisterEvent_);
        mEeRegisterEvent_.NotifyOne();
    }
    {
        SynchronizeGuard guard(mAidEvent_);
        mAidEvent_.NotifyOne();
    }
    {
        SynchronizeGuard guard(mRoutingCfgEvent_);
        mRoutingCfgEvent_.NotifyOne();
    }
    {
        SynchronizeGuard guard(mCommitRoutingEvent_);
        mCommitRoutingEvent_.NotifyOne();
    }
    {
        SynchronizeGuard guard(mModeSetEvent_);
        mModeSetEvent_.NotifyOne();
    }
    {
        SynchronizeGuard guard(remainSizeEvent_);
        remainSizeEvent_.NotifyOne();
    }
}

void NciBalCe::Deinitialize()
{
    DebugLog("NciBalCe::Deinitialize");
    DeactiveEe();
    mNfcNciImpl_->NfcEeDeregister(NfcEeCallback);
}

void NciBalCe::DeactiveEe()
{
    DebugLog("NciBalCe::DeactiveEe");
    if (mDefaultOffHostRoute_ == NFC_DH_ID) {
        return;
    }

    tNFA_EE_INFO eeInfo[NFA_EE_MAX_EE_SUPPORTED];
    uint8_t numEe = NFA_EE_MAX_EE_SUPPORTED;
    tNFA_STATUS status = mNfcNciImpl_->NfcEeGetInfo(&numEe, eeInfo);
    if (status != NFA_STATUS_OK) {
        ErrorLog("Get ee info fail, error = 0x%X", status);
        return;
    }
    if (numEe == 0) {
        DebugLog("No ee found");
        return;
    }
    uint8_t nciVersion = mNfcNciImpl_->NfcGetNciVersion();
    for (uint8_t i = 0; i < numEe; i++) {
        bool isEeActive = false;
        if (nciVersion == NCI_VERSION_1_0) {
            isEeActive = (eeInfo[i].num_interface != 0);
        } else {
            isEeActive = (eeInfo[i].ee_interface[0] != NCI_NFCEE_INTERFACE_HCI_ACCESS) &&
                         (eeInfo[i].ee_status == NFA_EE_STATUS_ACTIVE);
        }
        if (isEeActive) {
            SynchronizeGuard guard(mModeSetEvent_);
            status = mNfcNciImpl_->NfcEeModeSet(eeInfo[i].ee_handle, NFA_EE_MD_DEACTIVATE);
            if (status == NFA_STATUS_OK) {
                mModeSetEvent_.Wait();
            } else {
                ErrorLog("Fail to set ee deactivate");
            }
        }
    }
}

void NciBalCe::SetNfcNciImpl(std::shared_ptr<INfcNci> nfcNciImpl)
{
    mNfcNciImpl_ = nfcNciImpl;
}

bool NciBalCe::ClearAidTable()
{
    DebugLog("NciBalCe::ClearAidTable");
    SynchronizeGuard guard(mAidEvent_);
    mAidRoutingConfigured_ = false;
    tNFA_STATUS status = mNfcNciImpl_->NfcEeRemoveAidRouting(NFA_REMOVE_ALL_AID_LEN, (uint8_t*)NFA_REMOVE_ALL_AID);
    if (status == NFA_STATUS_OK) {
        mAidEvent_.Wait();
    }
    if (mAidRoutingConfigured_) {
        DebugLog("Cleared AID table");
        return true;
    } else {
        ErrorLog("Failed to clear AID table");
        return false;
    }
}

int NciBalCe::GetRemainRoutingTableSize()
{
    DebugLog("NciBalCe::GetRemainRoutingTableSize");
    SynchronizeGuard guard(remainSizeEvent_);
    remainTableSize_ = 0;
    tNFA_STATUS status = NFA_EeGetLmrtRemainingSize();
    if (status == NFA_STATUS_OK) {
        remainSizeEvent_.Wait();
    } else {
        ErrorLog("Get remain table size fail, error = 0x%02X", status);
    }
    return remainTableSize_;
}
}  // namespace ncibal
}  // namespace nfc
}  // namespace OHOS