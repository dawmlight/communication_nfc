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
#include "nci_bal_tag.h"

#include <mutex>

#include "device_host.h"
#include "loghelper.h"
#include "nci_bal_manager.h"
#include "nfc_config.h"
#include "nfc_nci_impl.h"
#include "rw_int.h"
#include "tag_end_point.h"
#include "utils/synchronize_event.h"

namespace OHOS {
namespace nfc {
namespace ncibal {
std::mutex NciBalTag::mRfDiscoveryMutex_;
OHOS::nfc::SynchronizeEvent NciBalTag::mTransceiveEvent_;
OHOS::nfc::SynchronizeEvent NciBalTag::mPresenceCheckEvent_;
OHOS::nfc::SynchronizeEvent NciBalTag::mReadNdefEvent_;
OHOS::nfc::SynchronizeEvent NciBalTag::mWriteNdefEvent_;
OHOS::nfc::SynchronizeEvent NciBalTag::mFormatNdefEvent_;
OHOS::nfc::SynchronizeEvent NciBalTag::mCheckNdefEvent_;
OHOS::nfc::SynchronizeEvent NciBalTag::mSelectEvent_;
OHOS::nfc::SynchronizeEvent NciBalTag::mActivatedEvent_;
OHOS::nfc::SynchronizeEvent NciBalTag::mDeactivatedEvent_;

bool NciBalTag::mIsTagPresent_ = true;
int NciBalTag::mConnectProtocol_ = NCI_PROTOCOL_UNKNOWN;
int NciBalTag::mConnectTargetType_ = TARGET_TYPE_UNKNOWN;
int NciBalTag::mConnectHandle_ = -1;
bool NciBalTag::mIsReconnect_ = false;
bool NciBalTag::mIsInTransceive_ = false;
int NciBalTag::mT1tMaxMessageSize_ = 0;
std::string NciBalTag::mReceivedData_ = "";
int NciBalTag::mCheckNdefStatus_ = NFA_STATUS_FAILED;
bool NciBalTag::mCheckNdefCapable_ = false;
int NciBalTag::mCheckNdefCurrentSize_ = 0;
int NciBalTag::mCheckNdefMaxSize_ = 0;
int NciBalTag::mCheckNdefReadOnly_ = NDEF_MODE_UNKNOWN;
bool NciBalTag::mWriteNdefStatus_ = false;
bool NciBalTag::mIsNdefFormatSuccess_ = false;
unsigned short int NciBalTag::mNdefTypeHandle_ = NFA_HANDLE_INVALID;
std::string NciBalTag::mReadNdefData = "";
std::shared_ptr<INfcNci> NciBalTag::mNfcNciImpl_ = std::make_shared<NfcNciImpl>();

NciBalTag::NciBalTag()
    : mBalTechListIndex_(0),
      mProtocol_(NCI_PROTOCOL_UNKNOWN),
      mIsFelicaLite_(false),
      mIsMifareUltralight_(false),
      mIsInfineonMyDMove_(false),
      mIsKovioType2Tag_(false),
      mIsMifareDESFire_(false),
      mPresChkOption_(NFA_RW_PRES_CHK_DEFAULT)
{
    ResetTimeOut();
    if (NfcConfig::hasKey(NAME_PRESENCE_CHECK_ALGORITHM)) {
        mPresChkOption_ = NfcConfig::getUnsigned(NAME_PRESENCE_CHECK_ALGORITHM);
    }
}

NciBalTag::~NciBalTag()
{
    mBalTechList_.clear();
    mBalTechHandles_.clear();
    mBalTechLibNfcTypes_.clear();
    mBalTechPollBytes_.clear();
    mBalTechActBytes_.clear();
    mDiscTechHandles_.clear();
    mDiscTechLibNfcTypes_.clear();
    mBalTechListIndex_ = 0;
    mProtocol_ = NCI_PROTOCOL_UNKNOWN;
    mIsFelicaLite_ = false;
    mIsMifareUltralight_ = false;
    mIsInfineonMyDMove_ = false;
    mIsKovioType2Tag_ = false;
    mIsMifareDESFire_ = false;
};

NciBalTag& NciBalTag::GetInstance()
{
    static NciBalTag mNciBalTag;
    return mNciBalTag;
}

void NciBalTag::NdefCallback(unsigned char event, tNFA_NDEF_EVT_DATA* eventData)
{
    DebugLog("NciBalTag::NdefCallback");
    switch (event) {
        case NFA_NDEF_REGISTER_EVT: {
            DebugLog("NdefCallback: NFA_NDEF_REGISTER_EVT; status=0x%X; handle=0x%X",
                     eventData->ndef_reg.status,
                     eventData->ndef_reg.ndef_type_handle);
            mNdefTypeHandle_ = eventData->ndef_reg.ndef_type_handle;
            break;
        }
        case NFA_NDEF_DATA_EVT: {
            DebugLog("NdefCallback: NFA_NDEF_DATA_EVT; data_len = %u", eventData->ndef_data.len);
            int ndefDataLen = eventData->ndef_data.len;
            mReadNdefData = UnsignedCharArrayToString(eventData->ndef_data.p_data, ndefDataLen);
            break;
        }
        default: {
            DebugLog("%s: Unknown event %u", "NdefCallback", event);
            break;
        }
    }
}

void NciBalTag::RegisterNdefHandler()
{
    DebugLog("NciBalTag::RegisterNdefHandler");
    mNdefTypeHandle_ = NFA_HANDLE_INVALID;
    mNfcNciImpl_->NfaRegisterNDefTypeHandler(true, NFA_TNF_DEFAULT, (unsigned char*)"", 0, NdefCallback);
}

tNFA_STATUS NciBalTag::Connect(int discId, int protocol, int tech)
{
    DebugLog("NciBalTag::Connect: discId: %d, protocol: %d, tech: %d", discId, protocol, tech);
    if (CheckTagState() == false) {
        return NFA_STATUS_BUSY;
    }
    SynchronizeGuard guard(mSelectEvent_);
    tNFA_INTF_TYPE rfInterface = GetRfInterface(protocol);
    mRfDiscoveryMutex_.lock();
    tNFA_STATUS status = mNfcNciImpl_->NfaSelect((uint8_t)discId, (tNFA_NFC_PROTOCOL)protocol, rfInterface);
    if (status != NFA_STATUS_OK) {
        ErrorLog("NciBalTag::Connect: select fail; error = 0x%X", status);
        mRfDiscoveryMutex_.unlock();
        return status;
    }
    if (mSelectEvent_.Wait(DEFAULT_TIMEOUT) == false) {
        ErrorLog("NciBalTag::Connect: Time out when select");
        status = mNfcNciImpl_->NfaDeactivate(false);
        if (status != NFA_STATUS_OK) {
            ErrorLog("NciBalTag::Connect: deactivate failed, error = 0x%X", status);
        }
        mRfDiscoveryMutex_.unlock();
        return NFA_STATUS_TIMEOUT;  // time out
    }
    mConnectProtocol_ = protocol;
    mConnectHandle_ = discId;
    mConnectTargetType_ = tech;
    mRfDiscoveryMutex_.unlock();
    return NFA_STATUS_OK;
}

bool NciBalTag::Disconnect()
{
    DebugLog("NciBalTag::Disconnect");
    mRfDiscoveryMutex_.lock();
    tNFA_STATUS status = mNfcNciImpl_->NfaDeactivate(false);
    if (status != NFA_STATUS_OK) {
        ErrorLog("NciBalTag::Disconnect: deactivate failed; error = 0x%X", status);
    }
    mConnectProtocol_ = NCI_PROTOCOL_UNKNOWN;
    mConnectHandle_ = -1;
    mConnectTargetType_ = TARGET_TYPE_UNKNOWN;
    mIsReconnect_ = false;
    ResetTimeOut();
    mRfDiscoveryMutex_.unlock();
    return (status == NFA_STATUS_OK);
}

bool NciBalTag::Reconnect(int discId, int protocol, int tech, bool restart)
{
    DebugLog("NciBalTag::Reconnect: discId: %d, protocol: %d, tech: %d, restart: %d", discId, protocol, tech, restart);
    if (CheckTagState() == false) {
        return false;
    }
    mRfDiscoveryMutex_.lock();
    if (mConnectProtocol_ == protocol && !restart) {
        mRfDiscoveryMutex_.unlock();
        return true;
    }
    {
        SynchronizeGuard guard(mDeactivatedEvent_);
        if (NFA_STATUS_OK != mNfcNciImpl_->NfaDeactivate(true)) {
            mRfDiscoveryMutex_.unlock();
            return false;
        }
        mDeactivatedEvent_.Wait(DEFAULT_TIMEOUT);
    }
    {
        SynchronizeGuard guard(mSelectEvent_);
        tNFA_INTF_TYPE rfInterface = GetRfInterface(protocol);
        tNFA_STATUS status = mNfcNciImpl_->NfaSelect((uint8_t)discId, (tNFA_NFC_PROTOCOL)protocol, rfInterface);
        if (status != NFA_STATUS_OK) {
            ErrorLog("NciBalTag::Reconnect: select failed, error=0x%X", status);
            mRfDiscoveryMutex_.unlock();
            return false;
        }
        if (mSelectEvent_.Wait(DEFAULT_TIMEOUT) == false) {
            ErrorLog("NciBalTag::Reconnect: Time out when select");
            status = mNfcNciImpl_->NfaDeactivate(false);
            if (status != NFA_STATUS_OK) {
                ErrorLog("NciBalTag::Reconnect: deactivate failed, error=0x%X", status);
            }
            mRfDiscoveryMutex_.unlock();
            return false;
        }
    }
    {
        SynchronizeGuard guard(mActivatedEvent_);
        mActivatedEvent_.Wait(DEFAULT_TIMEOUT);
    }
    mConnectProtocol_ = protocol;
    mConnectHandle_ = discId;
    mConnectTargetType_ = tech;
    mRfDiscoveryMutex_.unlock();
    return true;
}

int NciBalTag::Transceive(std::string& request, std::string& response)
{
    DebugLog("NciBalTag::Transceive");
    if (CheckTagState() == false) {
        return NFA_STATUS_BUSY;
    }
    tNFA_STATUS status = NFA_STATUS_FAILED;
    mIsInTransceive_ = true;
    bool retry = false;
    do {
        SynchronizeGuard guard(mTransceiveEvent_);
        mReceivedData_ = "";
        status = mNfcNciImpl_->NfaSendRawFrame(
            (uint8_t*)request.c_str(), (uint16_t)request.size(), NFA_DM_DEFAULT_PRESENCE_CHECK_START_DELAY);
        if (status != NFA_STATUS_OK) {
            ErrorLog("NciBalTag::Transceive: fail send; error=%d", status);
            break;
        }
        int transceiveTimeout = GetTimeOut(mConnectTargetType_);
        bool wait = mTransceiveEvent_.Wait(transceiveTimeout);
        if (wait == false) {
            ErrorLog("NciBalTag::Transceive: wait response timeout");
            status = NFA_STATUS_TIMEOUT;
            break;
        }
        response = mReceivedData_;

        // not auth
        if (retry) {
            retry = false;
        } else if (mConnectProtocol_ == NFA_PROTOCOL_MIFARE && (request[0] != 0x60 && request[0] != 0x61) &&
                   response.size() == 1 && response[0] != 0x00) {
            DebugLog("NFA_PROTOCOL_MIFARE retry");
            retry = true;
        }

        if (retry == false) {
            if (mConnectProtocol_ == NFA_PROTOCOL_MIFARE && response.size() == 1 && response[0] != 0x00) {
                DebugLog("Ready to reconnect");
                Reconnect(mConnectHandle_, NFA_PROTOCOL_MIFARE, TARGET_TYPE_MIFARE_CLASSIC, true);
            }
        }
    } while (retry);
    mIsInTransceive_ = false;
    return status;
}

void NciBalTag::HandleTranceiveData(unsigned char status, unsigned char* data, int dataLen)
{
    DebugLog("NciBalTag::HandleTranceiveData");
    SynchronizeGuard guard(mTransceiveEvent_);
    if (status == NFA_STATUS_OK || status == NFA_STATUS_CONTINUE) {
        mReceivedData_.append(UnsignedCharArrayToString(data, dataLen));
    }
    if (status == NFA_STATUS_OK) {
        mTransceiveEvent_.NotifyOne();
    }
}

bool NciBalTag::PresenceCheck()
{
    if (CheckTagState() == false) {
        return false;
    }
    if (mIsInTransceive_ == true) {
        return true;
    }
    if (!mRfDiscoveryMutex_.try_lock()) {
        return true;
    }

    {
        SynchronizeGuard guard(mPresenceCheckEvent_);
        tNFA_STATUS status = mNfcNciImpl_->NfaRwPresenceCheck(mPresChkOption_);
        if (status == NFA_STATUS_OK) {
            if (mPresenceCheckEvent_.Wait(DEFAULT_TIMEOUT) == false) {
                DebugLog("Presence check timeout...");
                mIsTagPresent_ = false;
            }
        }
    }
    mRfDiscoveryMutex_.unlock();
    return mIsTagPresent_;
}

void NciBalTag::HandlePresenceCheckResult(unsigned char status)
{
    SynchronizeGuard guard(mPresenceCheckEvent_);
    mIsTagPresent_ = (status == NFA_STATUS_OK);
    mPresenceCheckEvent_.NotifyOne();
}

void NciBalTag::HandleSelectResult()
{
    DebugLog("NciBalTag::HandleSelectResult");
    {
        SynchronizeGuard guard(mSelectEvent_);
        mSelectEvent_.NotifyOne();
    }
}

void NciBalTag::HandleActivatedResult()
{
    DebugLog("NciBalTag::HandleActivatedResult");
    {
        SynchronizeGuard guard(mActivatedEvent_);
        mActivatedEvent_.NotifyOne();
    }
}

void NciBalTag::HandleDeactivatedResult()
{
    DebugLog("NciBalTag::HandleDeactivatedResult");
    {
        SynchronizeGuard guard(mDeactivatedEvent_);
        mDeactivatedEvent_.NotifyOne();
    }
}

void NciBalTag::ResetIsTagPresent()
{
    DebugLog("NciBalTag::ResetIsTagPresent");
    mIsTagPresent_ = true;
}

int NciBalTag::GetTimeOut(int technology) const
{
    int timeout = DEFAULT_TIMEOUT;
    if (technology > 0 && technology < MAX_NUM_TECHNOLOGY) {
        timeout = mTechnologyTimeoutsTable_[technology];
    } else {
        WarnLog("NciBalTag::GetTimeOut, Unknown technology");
    }
    return timeout;
}

void NciBalTag::ResetTimeOut()
{
    mTechnologyTimeoutsTable_[TARGET_TYPE_ISO14443_3A] = ISO14443_3A_DEFAULT_TIMEOUT;  // NfcA
    mTechnologyTimeoutsTable_[TARGET_TYPE_ISO14443_3B] = ISO14443_3B_DEFAULT_TIMEOUT;  // NfcB
    mTechnologyTimeoutsTable_[TARGET_TYPE_ISO14443_4] = ISO14443_4_DEFAULT_TIMEOUT;    // ISO-DEP
    mTechnologyTimeoutsTable_[TARGET_TYPE_FELICA] = FELICA_DEFAULT_TIMEOUT;            // Felica
    mTechnologyTimeoutsTable_[TARGET_TYPE_V] = ISO15693_DEFAULT_TIMEOUT;               // NfcV
    mTechnologyTimeoutsTable_[TARGET_TYPE_NDEF] = NDEF_DEFAULT_TIMEOUT;
    mTechnologyTimeoutsTable_[TARGET_TYPE_NDEF_FORMATABLE] = NDEF_FORMATABLE_DEFAULT_TIMEOUT;
    mTechnologyTimeoutsTable_[TARGET_TYPE_MIFARE_CLASSIC] = MIFARE_CLASSIC_DEFAULT_TIMEOUT;  // MifareClassic
    mTechnologyTimeoutsTable_[TARGET_TYPE_MIFARE_UL] = MIFARE_UL_DEFAULT_TIMEOUT;            // MifareUltralight
}

bool NciBalTag::MakeReadOnly() const
{
    DebugLog("NciBalTag::MakeReadOnly");
    unsigned char status = mNfcNciImpl_->NfaRwSetTagReadOnly(true);
    if (status == NCI_STATUS_REJECTED) {
        status = mNfcNciImpl_->NfaRwSetTagReadOnly(false);
        if (status != NCI_STATUS_OK) {
            return false;
        }
    } else if (status != NCI_STATUS_OK) {
        return false;
    }
    return true;
}

void NciBalTag::ReadNdef(std::string& response)
{
    DebugLog("NciBalTag::ReadNdef");
    if (CheckTagState() == false) {
        return;
    }
    mRfDiscoveryMutex_.lock();
    mReadNdefData = "";
    SynchronizeGuard guard(mReadNdefEvent_);
    if (mCheckNdefCurrentSize_ > 0) {
        tNFA_STATUS status = mNfcNciImpl_->NfaRwReadNdef();
        if (status != NFA_STATUS_OK) {
            ErrorLog("Read ndef fail");
            return;
        }
        mReadNdefEvent_.Wait();

        if (!mReadNdefData.empty() && mReadNdefData.size() > 0) {
            response = mReadNdefData;
        }
    }
    mRfDiscoveryMutex_.unlock();
    return;
}

void NciBalTag::HandleReadComplete(unsigned char status)
{
    DebugLog("NciBalTag::HandleReadComplete");
    SynchronizeGuard guard(mReadNdefEvent_);
    if (status != NFA_STATUS_OK) {
        ErrorLog("Read ndef fail");
        mReadNdefData = "";
    }
    mReadNdefEvent_.NotifyOne();
}

bool NciBalTag::WriteNdef(std::string& ndefMessage)
{
    DebugLog("NciBalTag::WriteNdef");
    if (CheckTagState() == false) {
        return false;
    }
    mRfDiscoveryMutex_.lock();
    mWriteNdefStatus_ = false;
    tNFA_STATUS status = NFA_STATUS_FAILED;
    const uint32_t maxBufferSize = 1024;
    uint8_t buffer[maxBufferSize] = {0};
    uint32_t curDataSize = 0;
    SynchronizeGuard guard(mWriteNdefEvent_);
    int length = ndefMessage.length();
    unsigned char data[length];
    for (int i = 0; i < length; i++) {
        data[i] = ndefMessage.at(i);
    }
    if (mCheckNdefStatus_ == NFA_STATUS_FAILED) {
        if (mCheckNdefCapable_) {
            DebugLog("Format ndef first");
            this->FormatNdef();
        }
        status = mNfcNciImpl_->NfaRwWriteNdef(data, length);
    } else if (length == 0) {
        DebugLog("Create and write an empty ndef message");
        mNfcNciImpl_->NdefMsgInit(buffer, maxBufferSize, &curDataSize);
        mNfcNciImpl_->NdefMsgAddRec(buffer, maxBufferSize, &curDataSize, NDEF_TNF_EMPTY, NULL, 0, NULL, 0, NULL, 0);
        status = mNfcNciImpl_->NfaRwWriteNdef(buffer, curDataSize);
    } else {
        status = mNfcNciImpl_->NfaRwWriteNdef(data, length);
    }

    if (status == NCI_STATUS_OK) {
        mWriteNdefEvent_.Wait();
    } else {
        ErrorLog("Write ndef fail");
    }
    mRfDiscoveryMutex_.unlock();
    return mWriteNdefStatus_;
}

void NciBalTag::HandleWriteComplete(unsigned char status)
{
    DebugLog("NciBalTag::HandleWriteComplete");
    SynchronizeGuard guard(mWriteNdefEvent_);
    mWriteNdefStatus_ = (status == NFA_STATUS_OK);
    mWriteNdefEvent_.NotifyOne();
}

bool NciBalTag::FormatNdef()
{
    DebugLog("NciBalTag::FormatNdef");
    if (CheckTagState() == false) {
        return false;
    }
    SynchronizeGuard guard(mFormatNdefEvent_);
    mIsNdefFormatSuccess_ = false;
    tNFA_STATUS status = mNfcNciImpl_->NfaRwFormatTag();
    if (status == NFA_STATUS_OK) {
        mFormatNdefEvent_.Wait();
        if (!mIsNdefFormatSuccess_) {
            status = NFA_STATUS_FAILED;
        }
    } else {
        ErrorLog("Format Ndef error, status= %d", status);
    }
    return (status == NFA_STATUS_OK);
}

void NciBalTag::HandleFormatComplete(unsigned char status)
{
    DebugLog("NciBalTag::HandleFormatComplete");
    SynchronizeGuard guard(mFormatNdefEvent_);
    mIsNdefFormatSuccess_ = (status == NFA_STATUS_OK);
    mFormatNdefEvent_.NotifyOne();
}

bool NciBalTag::IsNdefFormatable()
{
    DebugLog("NciBalTag::IsNdefFormatable");
    return mIsNdefFormatSuccess_;
}

bool NciBalTag::CheckNdef(std::vector<int>& ndefInfo)
{
    DebugLog("NciBalTag::CheckNdef");
    if (CheckTagState() == false) {
        return false;
    }
    mRfDiscoveryMutex_.lock();
    SynchronizeGuard guard(mCheckNdefEvent_);
    tNFA_STATUS status = NFA_STATUS_FAILED;
    mIsReconnect_ = false;

    status = mNfcNciImpl_->NfaRwDetectNdef();
    if (status != NFA_STATUS_OK) {
        ErrorLog("NFA_RwDetectNDef failed, status: %d", status);
        mRfDiscoveryMutex_.unlock();
        return false;
    }
    if (mCheckNdefEvent_.Wait(DEFAULT_TIMEOUT) == false) {
        ErrorLog("NciBalTag::CheckNdef time out");
        mRfDiscoveryMutex_.unlock();
        return false;
    }

    if (mCheckNdefCapable_) {
        if (mConnectProtocol_ == NFA_PROTOCOL_T1T) {
            ndefInfo.push_back(mT1tMaxMessageSize_);
        } else {
            ndefInfo.push_back(mCheckNdefMaxSize_);
        }
        ndefInfo.push_back(mCheckNdefReadOnly_);
    }
    mRfDiscoveryMutex_.unlock();
    return mCheckNdefCapable_;
}

void NciBalTag::HandleNdefCheckResult(unsigned char status, int currentSize, int flag, int maxSize)
{
    DebugLog("NciBalTag::HandleNdefCheckResult");
    auto uFlag = static_cast<unsigned char>(flag & 0xFF);
    if (uFlag & RW_NDEF_FL_FORMATED) {
        DebugLog("Ndef check: Tag formated for NDEF");
    }
    if (uFlag & RW_NDEF_FL_SUPPORTED) {
        DebugLog("Ndef check: NDEF supported by the tag");
    }
    if (uFlag & RW_NDEF_FL_UNKNOWN) {
        DebugLog("Ndef check: Unable to find if tag is ndef capable/formated/read only");
    }
    if (uFlag & RW_NDEF_FL_FORMATABLE) {
        DebugLog("Ndef check: Tag supports format operation");
    }
    SynchronizeGuard guard(mCheckNdefEvent_);
    if (uFlag & RW_NDEF_FL_READ_ONLY) {
        DebugLog("Ndef check: Tag is read only");
        mCheckNdefReadOnly_ = NDEF_MODE_READ_ONLY;
    } else {
        mCheckNdefReadOnly_ = NDEF_MODE_READ_WRITE;
    }

    mCheckNdefStatus_ = status;
    if (mCheckNdefStatus_ != NFA_STATUS_OK && mCheckNdefStatus_ != NFA_STATUS_TIMEOUT) {
        mCheckNdefStatus_ = NFA_STATUS_FAILED;
        mCheckNdefCapable_ = false;
    }

    mCheckNdefCapable_ = false;
    if (mCheckNdefStatus_ == NFA_STATUS_OK) {
        mCheckNdefCurrentSize_ = currentSize;
        mCheckNdefMaxSize_ = maxSize;
        mCheckNdefCapable_ = true;
    } else if (mCheckNdefStatus_ == NFA_STATUS_FAILED) {
        mCheckNdefCurrentSize_ = 0;
        mCheckNdefMaxSize_ = 0;
        if ((uFlag & RW_NDEF_FL_SUPPORTED) && ((uFlag & RW_NDEF_FL_UNKNOWN) == 0)) {
            DebugLog("Tag is ndef capable");
            mCheckNdefCapable_ = true;
        }
    } else {
        mCheckNdefCurrentSize_ = 0;
        mCheckNdefMaxSize_ = 0;
    }
    mCheckNdefEvent_.NotifyOne();
}

int NciBalTag::GetTechFromData(tNFA_ACTIVATED activated) const
{
    int tech;
    if (activated.activate_ntf.protocol == NCI_PROTOCOL_T1T) {
        tech = TARGET_TYPE_ISO14443_3A;
    } else if (activated.activate_ntf.protocol == NCI_PROTOCOL_T2T) {
        tech = TARGET_TYPE_ISO14443_3A;
        // can also be mifare
        if (activated.activate_ntf.rf_tech_param.param.pa.nfcid1[0] == 0x04 &&
            activated.activate_ntf.rf_tech_param.param.pa.sel_rsp == 0) {
            tech = TARGET_TYPE_MIFARE_UL;
        }
    } else if (activated.activate_ntf.protocol == NCI_PROTOCOL_T3T) {
        tech = TARGET_TYPE_FELICA;
    } else if (activated.activate_ntf.protocol == NCI_PROTOCOL_ISO_DEP) {
        tech = TARGET_TYPE_ISO14443_4;
        // A OR B
        char discType = activated.activate_ntf.rf_tech_param.mode;
        if (discType == NCI_DISCOVERY_TYPE_POLL_A || discType == NCI_DISCOVERY_TYPE_POLL_A_ACTIVE ||
            discType == NCI_DISCOVERY_TYPE_LISTEN_A || discType == NCI_DISCOVERY_TYPE_LISTEN_A_ACTIVE) {
            tech = TARGET_TYPE_ISO14443_3A;
        } else if (discType == NCI_DISCOVERY_TYPE_POLL_B || discType == NFC_DISCOVERY_TYPE_POLL_B_PRIME ||
                   discType == NCI_DISCOVERY_TYPE_LISTEN_B || discType == NFC_DISCOVERY_TYPE_LISTEN_B_PRIME) {
            tech = TARGET_TYPE_ISO14443_3B;
        }
    } else if (activated.activate_ntf.protocol == NCI_PROTOCOL_15693) {
        tech = TARGET_TYPE_V;
    } else if (activated.activate_ntf.protocol == NFC_PROTOCOL_MIFARE) {
        tech = TARGET_TYPE_MIFARE_CLASSIC;
    } else {
        tech = TARGET_TYPE_UNKNOWN;
    }
    return tech;
}

std::string NciBalTag::GetUidFromData(tNFA_ACTIVATED activated) const
{
    std::string uid;
    tNFC_RF_TECH_PARAMS nfcRfTechParams = activated.activate_ntf.rf_tech_param;
    char discType = nfcRfTechParams.mode;
    if (discType == NCI_DISCOVERY_TYPE_POLL_A || discType == NCI_DISCOVERY_TYPE_POLL_A_ACTIVE ||
        discType == NCI_DISCOVERY_TYPE_LISTEN_A || discType == NCI_DISCOVERY_TYPE_LISTEN_A_ACTIVE) {
        int nfcid1Len = nfcRfTechParams.param.pa.nfcid1_len;
        uid = UnsignedCharArrayToString(nfcRfTechParams.param.pa.nfcid1, nfcid1Len);
    } else if (discType == NCI_DISCOVERY_TYPE_POLL_B || discType == NFC_DISCOVERY_TYPE_POLL_B_PRIME ||
               discType == NCI_DISCOVERY_TYPE_LISTEN_B || discType == NFC_DISCOVERY_TYPE_LISTEN_B_PRIME) {
        uid = UnsignedCharArrayToString(nfcRfTechParams.param.pb.nfcid0, NFC_NFCID0_MAX_LEN);
    } else if (discType == NCI_DISCOVERY_TYPE_POLL_F || discType == NCI_DISCOVERY_TYPE_POLL_F_ACTIVE ||
               discType == NCI_DISCOVERY_TYPE_LISTEN_F || discType == NCI_DISCOVERY_TYPE_LISTEN_F_ACTIVE) {
        uid = UnsignedCharArrayToString(nfcRfTechParams.param.pf.nfcid2, NFC_NFCID2_LEN);
    } else if (discType == NCI_DISCOVERY_TYPE_POLL_V || discType == NCI_DISCOVERY_TYPE_LISTEN_ISO15693) {
        unsigned char* i93Uid = activated.params.i93.uid;
        unsigned char i93UidReverse[I93_UID_BYTE_LEN];
        for (int i = 0; i < I93_UID_BYTE_LEN; i++) {
            i93UidReverse[i] = i93Uid[I93_UID_BYTE_LEN - i - 1];
        }
        uid = UnsignedCharArrayToString(i93UidReverse, I93_UID_BYTE_LEN);
    } else {
        uid = "";
    }
    return uid;
}

std::string NciBalTag::GetTechPollFromData(tNFA_ACTIVATED activated) const
{
    std::string techPoll;
    tNFC_RF_TECH_PARAMS nfcRfTechParams = activated.activate_ntf.rf_tech_param;
    char discType = nfcRfTechParams.mode;
    if (discType == NCI_DISCOVERY_TYPE_POLL_A || discType == NCI_DISCOVERY_TYPE_POLL_A_ACTIVE ||
        discType == NCI_DISCOVERY_TYPE_LISTEN_A || discType == NCI_DISCOVERY_TYPE_LISTEN_A_ACTIVE) {
        techPoll = UnsignedCharArrayToString(nfcRfTechParams.param.pa.sens_res, SENS_RES_LENGTH);
    } else if (discType == NCI_DISCOVERY_TYPE_POLL_B || discType == NFC_DISCOVERY_TYPE_POLL_B_PRIME ||
               discType == NCI_DISCOVERY_TYPE_LISTEN_B || discType == NFC_DISCOVERY_TYPE_LISTEN_B_PRIME) {
        int length = nfcRfTechParams.param.pb.sensb_res_len;
        if (length > NFC_NFCID0_MAX_LEN) {
            length = length - NFC_NFCID0_MAX_LEN;
        } else {
            WarnLog("sensb_res_len %d error", length);
            length = 0;
        }
        techPoll = UnsignedCharArrayToString(nfcRfTechParams.param.pb.sensb_res + SENSB_RES_POLL_POS, length);
    } else if (discType == NCI_DISCOVERY_TYPE_POLL_F || discType == NCI_DISCOVERY_TYPE_POLL_F_ACTIVE ||
               discType == NCI_DISCOVERY_TYPE_LISTEN_F || discType == NCI_DISCOVERY_TYPE_LISTEN_F_ACTIVE) {
        unsigned char cTechPoll[10];
        unsigned char* sensfRes = nfcRfTechParams.param.pf.sensf_res;
        for (int i = 0; i < SENSF_RES_LENGTH; i++) {
            cTechPoll[i] = static_cast<unsigned char>(sensfRes[i + SENSF_RES_LENGTH]);
        }
        if (activated.params.t3t.num_system_codes > 0) {
            unsigned short* pSystemCodes = activated.params.t3t.p_system_codes;
            cTechPoll[POS_NFCF_STSTEM_CODE_HIGH] = static_cast<unsigned char>(*pSystemCodes >> SYSTEM_CODE_SHIFT);
            cTechPoll[POS_NFCF_STSTEM_CODE_LOW] = static_cast<unsigned char>(*pSystemCodes);
        }
        techPoll = UnsignedCharArrayToString(cTechPoll, F_POLL_LENGTH);
    } else if (discType == NCI_DISCOVERY_TYPE_POLL_V || discType == NCI_DISCOVERY_TYPE_LISTEN_ISO15693) {
        unsigned char cTechPoll[2] = {activated.params.i93.afi, activated.params.i93.dsfid};
        techPoll = UnsignedCharArrayToString(cTechPoll, I93_POLL_LENGTH);
    } else {
        techPoll = "";
    }
    return techPoll;
}

std::string NciBalTag::GetTechActFromData(tNFA_ACTIVATED activated) const
{
    std::string techAct = "";
    unsigned char protocol = activated.activate_ntf.protocol;
    tNFC_RF_TECH_PARAMS nfcRfTechParams = activated.activate_ntf.rf_tech_param;
    if (protocol == NCI_PROTOCOL_T1T) {
        techAct = nfcRfTechParams.param.pa.sel_rsp;
    } else if (protocol == NCI_PROTOCOL_T2T) {
        techAct = nfcRfTechParams.param.pa.sel_rsp;
    } else if (protocol == NCI_PROTOCOL_T3T) {
        techAct = "";
    } else if (protocol == NCI_PROTOCOL_ISO_DEP) {
        char discType = nfcRfTechParams.mode;
        if (discType == NCI_DISCOVERY_TYPE_POLL_A || discType == NCI_DISCOVERY_TYPE_POLL_A_ACTIVE ||
            discType == NCI_DISCOVERY_TYPE_LISTEN_A || discType == NCI_DISCOVERY_TYPE_LISTEN_A_ACTIVE) {
            if (activated.activate_ntf.intf_param.type == NFC_INTERFACE_ISO_DEP) {
                tNFC_INTF_PA_ISO_DEP paIso = activated.activate_ntf.intf_param.intf_param.pa_iso;
                techAct = (paIso.his_byte_len > 0) ? UnsignedCharArrayToString(paIso.his_byte, paIso.his_byte_len) : "";
            }
        } else if (discType == NCI_DISCOVERY_TYPE_POLL_B || discType == NFC_DISCOVERY_TYPE_POLL_B_PRIME ||
                   discType == NCI_DISCOVERY_TYPE_LISTEN_B || discType == NFC_DISCOVERY_TYPE_LISTEN_B_PRIME) {
            if (activated.activate_ntf.intf_param.type == NFC_INTERFACE_ISO_DEP) {
                tNFC_INTF_PB_ISO_DEP pbIso = activated.activate_ntf.intf_param.intf_param.pb_iso;
                techAct = (pbIso.hi_info_len > 0) ? UnsignedCharArrayToString(pbIso.hi_info, pbIso.hi_info_len) : "";
            }
        }
    } else if (protocol == NCI_PROTOCOL_15693) {
        unsigned char cTechAct[2] = {activated.params.i93.afi, activated.params.i93.dsfid};
        techAct = UnsignedCharArrayToString(cTechAct, I93_ACT_LENGTH);
    } else if (protocol == NFC_PROTOCOL_MIFARE) {
        techAct = nfcRfTechParams.param.pa.sel_rsp;
    } else {
        techAct = "";
    }
    return techAct;
}

void NciBalTag::CheckSpecTagType(tNFA_ACTIVATED activated)
{
    // isFelicaLite
    if (activated.activate_ntf.protocol == NFC_PROTOCOL_T3T) {
        int i = 0;
        while (i < activated.params.t3t.num_system_codes) {
            if (activated.params.t3t.p_system_codes[i++] == T3T_SYSTEM_CODE_FELICA_LITE) {
                mIsFelicaLite_ = true;
                break;
            }
        }
    }
    // isMifareUltralight
    if (activated.activate_ntf.rf_tech_param.mode == NFC_DISCOVERY_TYPE_POLL_A) {
        // see NFC Digital Protocol, section 4.6.3 (SENS_RES); section 4.8.2
        // (SEL_RES).  see "MF0ICU1 Functional specification MIFARE Ultralight",
        // Rev. 3.4 - 4 February 2008,  section 6.7.
        if ((activated.activate_ntf.rf_tech_param.param.pa.sens_res[0] == 0x44) &&
            (activated.activate_ntf.rf_tech_param.param.pa.sens_res[1] == 0) &&
            ((activated.activate_ntf.rf_tech_param.param.pa.sel_rsp == 0) ||
            (activated.activate_ntf.rf_tech_param.param.pa.sel_rsp == 0x04)) &&
            (activated.activate_ntf.rf_tech_param.param.pa.nfcid1[0] == 0x04)) {
            mIsMifareUltralight_ = true;
        }
    }
    // isInfineonMyDMove
    if (activated.activate_ntf.rf_tech_param.mode == NFC_DISCOVERY_TYPE_POLL_A) {
        // see Infineon my-d move, my-d move NFC, SLE 66R01P, SLE 66R01PN,
        // Short Product Information, 2011-11-24, section 3.5
        if (activated.activate_ntf.rf_tech_param.param.pa.nfcid1[0] == 0x05) {
            uint8_t highNibble = activated.activate_ntf.rf_tech_param.param.pa.nfcid1[1] & 0xF0;
            if (highNibble == 0x30) {
                mIsInfineonMyDMove_ = true;
            }
        }
    }
    // isKovioType2Tag
    if (activated.activate_ntf.rf_tech_param.mode == NFC_DISCOVERY_TYPE_POLL_A) {
        // Kovio 2Kb RFID Tag, Functional Specification,
        // March 2, 2012, v2.0, section 8.3.
        if (activated.activate_ntf.rf_tech_param.param.pa.nfcid1[0] == 0x37) {
            mIsKovioType2Tag_ = true;
        }
    }
    // isMifareDESFire
    if ((activated.activate_ntf.rf_tech_param.mode == NFC_DISCOVERY_TYPE_POLL_A) ||
        (activated.activate_ntf.rf_tech_param.mode == NFC_DISCOVERY_TYPE_LISTEN_A) ||
        (activated.activate_ntf.rf_tech_param.mode == NFC_DISCOVERY_TYPE_LISTEN_A_ACTIVE)) {
        /* DESfire has one sak byte and 2 ATQA bytes */
        if ((activated.activate_ntf.rf_tech_param.param.pa.sens_res[0] == 0x44) &&
            (activated.activate_ntf.rf_tech_param.param.pa.sens_res[1] == 0x03) &&
            (activated.activate_ntf.rf_tech_param.param.pa.sel_rsp == 0x20)) {
            mIsMifareDESFire_ = true;
        }
    }
}

void NciBalTag::CreateTagEndPoint(const tNFA_CONN_EVT_DATA* eventData)
{
    DebugLog("NciBalTag::CreateTagEndPoint");
    if (mBalTechListIndex_ >= MAX_NUM_TECHNOLOGY) {
        return;
    }

    tNFA_ACTIVATED activated = eventData->activated;
    int tech = GetTechFromData(activated);
    int techHandle = activated.activate_ntf.rf_disc_id;
    int techLibNfcType = activated.activate_ntf.protocol;
    std::string uid = GetUidFromData(activated);
    std::string poll = GetTechPollFromData(activated);
    std::string act = GetTechActFromData(activated);

    mProtocol_ = activated.activate_ntf.protocol;
    GetT1tMaxMessageSize(activated);
    CheckSpecTagType(activated);

    mBalTechList_.push_back(tech);
    mBalTechHandles_.push_back(techHandle);
    mBalTechLibNfcTypes_.push_back(techLibNfcType);
    mBalTechPollBytes_.push_back(poll);
    mBalTechActBytes_.push_back(act);

    DebugLog("TagDiscover: discId: %d, protocol: %d, tech: %d", techHandle, techLibNfcType, tech);
    std::unique_ptr<ITagEndPoint> tagEndPoint = std::make_unique<TagEndPoint>(
        mBalTechList_, mBalTechHandles_, mBalTechLibNfcTypes_, uid, mBalTechPollBytes_, mBalTechActBytes_);
    DeviceHost::TagDiscovered(std::move(tagEndPoint));
    mBalTechListIndex_++;
}

void NciBalTag::ResetTag()
{
    DebugLog("NciBalTag::ResetTag");
    mBalTechList_.clear();
    mBalTechHandles_.clear();
    mBalTechLibNfcTypes_.clear();
    mBalTechPollBytes_.clear();
    mBalTechActBytes_.clear();
    mDiscTechHandles_.clear();
    mDiscTechLibNfcTypes_.clear();
    mBalTechListIndex_ = 0;
    mProtocol_ = NCI_PROTOCOL_UNKNOWN;
    mIsFelicaLite_ = false;
    mIsMifareUltralight_ = false;
    mIsInfineonMyDMove_ = false;
    mIsKovioType2Tag_ = false;
    mIsMifareDESFire_ = false;
    ResetTimeOut();
}

void NciBalTag::HandleDiscResult(tNFA_CONN_EVT_DATA* eventData)
{
    tNFC_RESULT_DEVT& discoveryNtf = eventData->disc_result.discovery_ntf;
    DebugLog("NciBalTag::HandleDiscResult, discId: %d, protocol: %d", discoveryNtf.rf_disc_id, discoveryNtf.protocol);

    mDiscTechHandles_.push_back(discoveryNtf.rf_disc_id);
    mDiscTechLibNfcTypes_.push_back(discoveryNtf.protocol);
    if (discoveryNtf.more == NCI_DISCOVER_NTF_MORE) {
        return;
    }

    int index = -1;
    for (std::size_t i = 0; i < mDiscTechLibNfcTypes_.size(); i++) {
        if (mDiscTechLibNfcTypes_[i] != NFA_PROTOCOL_NFC_DEP) {
            index = i;
            break;
        }
    }

    if (index == -1) {
        DebugLog("Only find nfc-dep technology");
        return;
    }
    tNFA_INTF_TYPE rfInterface;
    if (mDiscTechLibNfcTypes_[index] == NFA_PROTOCOL_ISO_DEP) {
        rfInterface = NFA_INTERFACE_ISO_DEP;
    } else if (mDiscTechLibNfcTypes_[index] == NFC_PROTOCOL_MIFARE) {
        rfInterface = NFA_INTERFACE_MIFARE;
    } else {
        rfInterface = NFA_INTERFACE_FRAME;
    }
    mRfDiscoveryMutex_.lock();
    tNFA_STATUS status = mNfcNciImpl_->NfaSelect(
        (uint8_t)mDiscTechHandles_[index], (tNFA_NFC_PROTOCOL)mDiscTechLibNfcTypes_[index], rfInterface);
    if (status != NFA_STATUS_OK) {
        ErrorLog("NciBalTag::HandleDiscResult: select fail; error = 0x%X", status);
    }
    mConnectProtocol_ = mDiscTechLibNfcTypes_[index];
    mConnectHandle_ = mDiscTechHandles_[index];
    mRfDiscoveryMutex_.unlock();
}

void NciBalTag::StartRfField()
{
    mRfDiscoveryMutex_.lock();
}

void NciBalTag::EndRfField()
{
    mRfDiscoveryMutex_.unlock();
}

void NciBalTag::SetNfcNciImpl(std::shared_ptr<INfcNci> nfcNciImpl)
{
    mNfcNciImpl_ = nfcNciImpl;
}

bool NciBalTag::IsNdefFormattable()
{
    if (NFA_PROTOCOL_T1T == mProtocol_ || NFA_PROTOCOL_T5T == mProtocol_ || NFC_PROTOCOL_MIFARE == mProtocol_) {
        return true;
    } else if (NFA_PROTOCOL_T2T == mProtocol_) {
        return mIsMifareUltralight_ | mIsInfineonMyDMove_ | mIsKovioType2Tag_;
    } else if (NFA_PROTOCOL_T3T == mProtocol_) {
        return mIsFelicaLite_;
    } else if (NFA_PROTOCOL_ISO_DEP == mProtocol_) {
        if (mIsMifareDESFire_) {
            std::string request = {(char)0x90, 0x60, 0x00, 0x00, 0x00};
            std::string response;
            Transceive(request, response);
            if (response.length() == MIFACE_DES_FIRE_RESPONSE_LENGTH &&
                response.at(7) == MIFACE_DES_FIRE_RESPONSE_NDEF_FORMATTABLE_FIRST &&
                response.at(8) == MIFACE_DES_FIRE_RESPONSE_NDEF_FORMATTABLE_SECOND) {
                return true;
            }
        }
    }
    return false;
}

void NciBalTag::AbortWait()
{
    DebugLog("NciBalTag::AbortWait");
    {
        SynchronizeGuard guard(mTransceiveEvent_);
        mTransceiveEvent_.NotifyOne();
    }
    {
        SynchronizeGuard guard(mPresenceCheckEvent_);
        mPresenceCheckEvent_.NotifyOne();
    }
    {
        SynchronizeGuard guard(mReadNdefEvent_);
        mReadNdefEvent_.NotifyOne();
    }
    {
        SynchronizeGuard guard(mWriteNdefEvent_);
        mWriteNdefEvent_.NotifyOne();
    }
    {
        SynchronizeGuard guard(mFormatNdefEvent_);
        mFormatNdefEvent_.NotifyOne();
    }
    {
        SynchronizeGuard guard(mCheckNdefEvent_);
        mCheckNdefEvent_.NotifyOne();
    }
    {
        SynchronizeGuard guard(mSelectEvent_);
        mSelectEvent_.NotifyOne();
    }
    {
        SynchronizeGuard guard(mActivatedEvent_);
        mActivatedEvent_.NotifyOne();
    }
    {
        SynchronizeGuard guard(mDeactivatedEvent_);
        mDeactivatedEvent_.NotifyOne();
    }
}

void NciBalTag::GetT1tMaxMessageSize(tNFA_ACTIVATED activated) const
{
    DebugLog("GetT1tMaxMessageSize");
    if (activated.activate_ntf.protocol != NFC_PROTOCOL_T1T) {
        mT1tMaxMessageSize_ = 0;
        return;
    }
    // examine the first byte of header ROM bytes
    switch (activated.params.t1t.hr[0]) {
        case RW_T1T_IS_TOPAZ96:
            mT1tMaxMessageSize_ = TOPAZ96_MAX_MESSAGE_SIZE;
            break;
        case RW_T1T_IS_TOPAZ512:
            mT1tMaxMessageSize_ = TOPAZ512_MAX_MESSAGE_SIZE;
            break;
        default:
            ErrorLog("GetT1tMaxMessageSize: unknown T1T HR0=%u", activated.params.t1t.hr[0]);
            mT1tMaxMessageSize_ = 0;
            break;
    }
}

tNFA_INTF_TYPE NciBalTag::GetRfInterface(int protocol) const
{
    tNFA_INTF_TYPE rfInterface;
    if (protocol == NFA_PROTOCOL_ISO_DEP) {
        rfInterface = NFA_INTERFACE_ISO_DEP;
    } else if (protocol == NFA_PROTOCOL_MIFARE) {
        rfInterface = NFA_INTERFACE_MIFARE;
    } else {
        rfInterface = NFA_INTERFACE_FRAME;
    }
    return rfInterface;
}

bool NciBalTag::CheckTagState() const
{
    if (NciBalManager::GetInstance().IsNfcActive() == false) {
        DebugLog("Nfc is not active");
        return false;
    }
    if (NciBalManager::GetInstance().IsTagActive() == false) {
        DebugLog("Tag already deactive");
        return false;
    }
    return true;
}

std::string NciBalTag::UnsignedCharArrayToString(const unsigned char* charArray, int length)
{
    std::string result = "";
    for (int i = 0; i < length; i++) {
        result += charArray[i];
    }
    return result;
}
}  // namespace ncibal
}  // namespace nfc
}  // namespace OHOS