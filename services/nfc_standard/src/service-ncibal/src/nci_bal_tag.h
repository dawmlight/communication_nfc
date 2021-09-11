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
#ifndef NCI_BAL_TAG_H
#define NCI_BAL_TAG_H

#include <memory>
#include <mutex>
#include <string>
#include <vector>

#include "nfa_api.h"
#include "nfa_rw_api.h"

namespace OHOS::nfc {
class SynchronizeEvent;
}

namespace OHOS {
namespace nfc {
namespace ncibal {
static const int DEFAULT_TIMEOUT = 1000;
static const int ISO14443_3A_DEFAULT_TIMEOUT = 618;   // NfcA
static const int ISO14443_3B_DEFAULT_TIMEOUT = 1000;  // NfcB
static const int ISO14443_4_DEFAULT_TIMEOUT = 618;    // ISO-DEP
static const int FELICA_DEFAULT_TIMEOUT = 255;        // Felica
static const int ISO15693_DEFAULT_TIMEOUT = 1000;     // NfcV
static const int NDEF_DEFAULT_TIMEOUT = 1000;
static const int NDEF_FORMATABLE_DEFAULT_TIMEOUT = 1000;
static const int MIFARE_CLASSIC_DEFAULT_TIMEOUT = 618;  // MifareClassic
static const int MIFARE_UL_DEFAULT_TIMEOUT = 618;       // MifareUltralight
static const int POS_NFCF_STSTEM_CODE_HIGH = 8;
static const int POS_NFCF_STSTEM_CODE_LOW = 9;
static const int TOPAZ512_MAX_MESSAGE_SIZE = 462;
static const int TOPAZ96_MAX_MESSAGE_SIZE = 90;
static const int SENSF_RES_LENGTH = 8;
static const int TEST_TIME_CONVERSION = 1000;
static const int SENS_RES_LENGTH = 2;
static const int SENSB_RES_POLL_POS = 4;
static const int SYSTEM_CODE_SHIFT = 8;
static const int F_POLL_LENGTH = 10;
static const int I93_POLL_LENGTH = 2;
static const int I93_ACT_LENGTH = 2;
static const int MIFACE_DES_FIRE_RESPONSE_LENGTH = 9;
static const int MIFACE_DES_FIRE_RESPONSE_NDEF_FORMATTABLE_FIRST = 0x91;
static const int MIFACE_DES_FIRE_RESPONSE_NDEF_FORMATTABLE_SECOND = 0xAF;
static const int NDEF_MODE_READ_ONLY = 1;
static const int NDEF_MODE_READ_WRITE = 2;
static const int NDEF_MODE_UNKNOWN = 3;
static const int NCI_MAX_DATA_LEN = 300;

class INfcNci;
class NciBalTag final {
public:
    static NciBalTag& GetInstance();
    static void HandleSelectResult();
    static void HandleTranceiveData(unsigned char status, unsigned char* data, int dataLen);
    static void HandlePresenceCheckResult(unsigned char status);
    static void ResetIsTagPresent();
    static void HandleReadComplete(unsigned char status);
    static void HandleWriteComplete(unsigned char status);
    static void HandleFormatComplete(unsigned char status);
    static bool IsNdefFormatable();
    static void HandleNdefCheckResult(unsigned char status, int currentSize, int flag, int maxSize);
    static void HandleActivatedResult();
    static void HandleDeactivatedResult();
    static void AbortWait();
    tNFA_STATUS Connect(int discId, int protocol, int tech);
    bool Disconnect();
    bool Reconnect(int discId, int protocol, int tech, bool restart);
    int Transceive(std::string& request, std::string& response);
    bool PresenceCheck();
    int GetTimeOut(int technology) const;
    void ResetTimeOut();
    bool MakeReadOnly() const;
    void RegisterNdefHandler();
    void ReadNdef(std::string& response);
    bool WriteNdef(std::string& ndefMessage);
    bool FormatNdef();
    bool CheckNdef(std::vector<int>& ndefInfo);
    void CreateTagEndPoint(const tNFA_CONN_EVT_DATA* eventData);
    void ResetTag();
    void HandleDiscResult(tNFA_CONN_EVT_DATA* eventData);
    void StartRfField();
    void EndRfField();
    void SetNfcNciImpl(std::shared_ptr<INfcNci> nfcNciImpl);
    bool IsNdefFormattable();

private:
    NciBalTag();
    ~NciBalTag();
    void GetT1tMaxMessageSize(tNFA_ACTIVATED activated) const;
    int GetTechFromData(tNFA_ACTIVATED activated) const;
    std::string GetUidFromData(tNFA_ACTIVATED activated) const;
    std::string GetTechPollFromData(tNFA_ACTIVATED activated) const;
    std::string GetTechActFromData(tNFA_ACTIVATED activated) const;
    tNFA_INTF_TYPE GetRfInterface(int protocol) const;
    bool CheckTagState() const;
    void CheckSpecTagType(tNFA_ACTIVATED activated);
    static void NdefCallback(unsigned char event, tNFA_NDEF_EVT_DATA* eventData);
    static std::string UnsignedCharArrayToString(const unsigned char* charArray, int length);

    static std::mutex mRfDiscoveryMutex_;
    static OHOS::nfc::SynchronizeEvent mTransceiveEvent_;
    static OHOS::nfc::SynchronizeEvent mPresenceCheckEvent_;
    static OHOS::nfc::SynchronizeEvent mReadNdefEvent_;
    static OHOS::nfc::SynchronizeEvent mWriteNdefEvent_;
    static OHOS::nfc::SynchronizeEvent mFormatNdefEvent_;
    static OHOS::nfc::SynchronizeEvent mCheckNdefEvent_;
    static OHOS::nfc::SynchronizeEvent mSelectEvent_;
    static OHOS::nfc::SynchronizeEvent mActivatedEvent_;
    static OHOS::nfc::SynchronizeEvent mDeactivatedEvent_;
    static bool mIsTagPresent_;
    static int mConnectProtocol_;
    static int mConnectTargetType_;
    static int mConnectHandle_;
    static bool mIsReconnect_;
    static bool mIsInTransceive_;
    static int mT1tMaxMessageSize_;
    static std::string mReceivedData_;
    static int mCheckNdefStatus_;
    static bool mCheckNdefCapable_;
    static int mCheckNdefCurrentSize_;
    static int mCheckNdefMaxSize_;
    static int mCheckNdefReadOnly_;
    static bool mWriteNdefStatus_;
    static bool mIsNdefFormatSuccess_;
    static unsigned short int mNdefTypeHandle_;
    static std::string mReadNdefData;
    static const int MAX_NUM_TECHNOLOGY = 10;
    static std::shared_ptr<INfcNci> mNfcNciImpl_;
    int mTechnologyTimeoutsTable_[MAX_NUM_TECHNOLOGY]{};
    std::vector<int> mBalTechList_{};         // tag type
    std::vector<int> mBalTechHandles_{};      // disc id
    std::vector<int> mBalTechLibNfcTypes_{};  // protocol
    std::vector<std::string> mBalTechPollBytes_{};
    std::vector<std::string> mBalTechActBytes_{};
    int mBalTechListIndex_;                    // current tech list index
    std::vector<int> mDiscTechHandles_{};      // disc id
    std::vector<int> mDiscTechLibNfcTypes_{};  // protocol
    int mProtocol_;
    bool mIsFelicaLite_;
    bool mIsMifareUltralight_;
    bool mIsInfineonMyDMove_;
    bool mIsKovioType2Tag_;
    bool mIsMifareDESFire_;
    tNFA_RW_PRES_CHK_OPTION mPresChkOption_;
};
}  // namespace ncibal
}  // namespace nfc
}  // namespace OHOS
#endif /* NCI_BAL_TAG_H */