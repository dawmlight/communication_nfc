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
#include "hci_manager.h"

#include <string>

#include "device_host.h"
#include "infc_nci.h"
#include "loghelper.h"
#include "nfa_hci_api.h"
#include "nfa_hci_defs.h"
#include "nfc_config.h"
#include "nfc_nci_impl.h"

namespace OHOS {
namespace nfc {
namespace ncibal {
unsigned char HciManager::mEsePipe_ = 0x16;
unsigned char HciManager::mSimPipe_ = 0x0A;
std::shared_ptr<INfcNci> HciManager::mNfcNciImpl_ = std::make_shared<NfcNciImpl>();

HciManager::HciManager() {}

HciManager::~HciManager() {}

HciManager& HciManager::GetInstance()
{
    static HciManager hciManager;
    return hciManager;
}

void HciManager::Initialize()
{
    DebugLog("HciManager::Initialize");
    tNFA_STATUS status =
        mNfcNciImpl_->NfcHciRegister(const_cast<char*>(APP_NAME), (tNFA_HCI_CBACK*)&NfcHciCallback, true);
    if (status != NFA_STATUS_OK) {
        ErrorLog("HCI register fail, status = %u", status);
    }
    mEsePipe_ = NfcConfig::getUnsigned(NAME_OFF_HOST_ESE_PIPE_ID, 0x16);
    mSimPipe_ = NfcConfig::getUnsigned(NAME_OFF_HOST_SIM_PIPE_ID, 0x0A);
}

void HciManager::NfcHciCallback(tNFA_HCI_EVT event, tNFA_HCI_EVT_DATA* eventData)
{
    if (eventData == nullptr) {
        return;
    }
    DebugLog("event=%d code=%d pipe=%d len=%d",
             event,
             eventData->rcvd_evt.evt_code,
             eventData->rcvd_evt.pipe,
             eventData->rcvd_evt.evt_len);
    if (eventData->rcvd_evt.evt_code != NFA_HCI_EVT_TRANSACTION) {
        return;
    }

    std::string seName;
    uint8_t nfceeId = 0x00;
    if (eventData->rcvd_evt.pipe == mEsePipe_) {
        nfceeId = GetConnectedNfceeId(0x82);
        if (nfceeId == 0x84) {
            seName = "DHSE1";
        } else {
            seName = "eSE1";
        }
    } else if (eventData->rcvd_evt.pipe == mSimPipe_) {
        nfceeId = GetConnectedNfceeId(0x81);
        if (nfceeId == 0x81) {
            seName = "SIM1";
        } else {
            seName = "SIM2";
        }
    } else {
        ErrorLog("Unknown pipe id");
        return;
    }

    uint16_t buffLength = eventData->rcvd_evt.evt_len;
    uint8_t* buffData = eventData->rcvd_evt.p_evt_buf;
    std::string buff = std::string((const char*)(buffData), buffLength);
    if (event == NFA_HCI_EVENT_RCVD_EVT && eventData->rcvd_evt.evt_code == NFA_HCI_EVT_TRANSACTION &&
        buffLength > HCI_TRANSACTION_DATA_LENGTH_MIN && buff.at(0) == BERTLV_LENGTH_ONE) {
        int aidLength = buff.at(HCI_TRANSACTION_AID_LENGTH_POS);
        std::string aid = buff.substr(HCI_TRANSACTION_AID_POS, aidLength);

        int berTlvLength = buffLength - aidLength - HCI_TRANSACTION_AID_BERTLV_HEAD_LENGTH;
        std::string data = "";
        if (berTlvLength > 0 && buff.at(HCI_TRANSACTION_AID_POS + aidLength) == BERTLV_LENGTH_TWO) {
            std::string berTlv = buff.substr(aidLength + HCI_TRANSACTION_AID_BERTLV_HEAD_LENGTH);
            data = GetDataFromBerTlv(berTlv);
        }
        DeviceHost::OffHostTransactionEvent(aid, data, seName);
    }
}

std::string HciManager::GetDataFromBerTlv(std::string& berTlv)
{
    if (berTlv.empty()) {
        return "";
    }
    int lengthTag = berTlv.at(0);
    int berTlvLength = berTlv.length();
    DebugLog("DecodeBerTlv: berTlv[0]=%d", lengthTag);
    /* First byte to determine the length and the start index */
    if (lengthTag < BERTLV_LENGTH_LIMITED && berTlvLength == lengthTag + 1) {
        return berTlv.substr(1);
    } else if (lengthTag == BERTLV_LENGTH_ONE) {
        int length = berTlv.at(1);
        if (berTlvLength == length + 2) {
            return berTlv.substr(2);
        }
    } else if (lengthTag == BERTLV_LENGTH_TWO) {
        int length = (berTlv.at(1) << 8) | berTlv.at(2);
        if (berTlvLength == length + 3) {
            return berTlv.substr(3);
        }
    } else if (lengthTag == BERTLV_LENGTH_THREE) {
        int length = (berTlv.at(1) << 16) | (berTlv.at(2) << 8) | berTlv.at(3);
        if (berTlvLength == length + 4) {
            return berTlv.substr(4);
        }
    } else if (lengthTag == BERTLV_LENGTH_FOUR) {
        int length = (berTlv.at(1) << 24) | (berTlv.at(2) << 16) | (berTlv.at(3) << 8) | berTlv.at(4);
        if (berTlvLength == length + 5) {
            return berTlv.substr(5);
        }
    }
    ErrorLog("Error in TLV length encoding");
    return "";
}

uint8_t HciManager::GetConnectedNfceeId(uint8_t nfceeId)
{
    DebugLog("HciManager::GetConnectedNfceeId");
    uint8_t actualNumEe = NFA_EE_MAX_EE_SUPPORTED;
    tNFA_EE_INFO eeInfo[NFA_EE_MAX_EE_SUPPORTED];
    tNFA_STATUS status = mNfcNciImpl_->NfcEeGetInfo(&actualNumEe, eeInfo);
    if (status != NFA_STATUS_OK) {
        ErrorLog("Fail get eeInfo, status = 0x%02X", status);
        return 0x00;
    }

    switch (nfceeId) {
        case 0x81:
        case 0x83:
        case 0x85:
            for (uint8_t i = 0; i < actualNumEe; i++) {
                if ((eeInfo[i].ee_status == 0) && ((eeInfo[i].ee_handle & 0x01) != 0)) {
                    return (eeInfo[i].ee_handle & 0xFF);
                }
            }
            break;

        case 0x82:
        case 0x84:
        case 0x86:
            for (uint8_t i = 0; i < actualNumEe; i++) {
                if ((eeInfo[i].ee_status == 0) && ((eeInfo[i].ee_handle & 0x01) == 0)) {
                    return (eeInfo[i].ee_handle & 0xFF);
                }
            }
            break;

        default:
            break;
    }

    return 0x00;
}

void HciManager::SetNfcNciImpl(std::shared_ptr<INfcNci> nfcNciImpl)
{
    mNfcNciImpl_ = nfcNciImpl;
}
}  // namespace ncibal
}  // namespace nfc
}  // namespace OHOS