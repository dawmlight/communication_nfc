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
#ifndef TAG_END_POINT_H
#define TAG_END_POINT_H

#include <memory>
#include <mutex>
#include <vector>

#include "../../service-ncibal/include/itag_end_point.h"

namespace OHOS::nfc {
class SynchronizeEvent;
}

namespace OHOS {
namespace nfc {
namespace ncibal {
enum EnumNcibalTagTechnology {
    ENUM_NFC_INVALID_TECH = 0,
    ENUM_NFC_NDEF_TECH = 1,
    ENUM_NFC_ISODEP_TECH = 2,
    ENUM_NFC_MIFARE_CLASSIC_TECH = 3,
    ENUM_NFC_MIFARE_ULTRALIGHT_TECH = 4,
    ENUM_NFC_ISO15693_TECH = 5,
    ENUM_NFC_FELICA_TECH = 6,
    ENUM_NFC_NDEF_FORMATABLE_TECH = 7
};
static const auto NCIBAL_TECH_EXTRA_BUNDLE_PREIFIX = "Tech_Extra_Data_";
// ISODEP
static const auto NCIBAL_HISTORICAL_BYTES = "HistoricalBytes";
static const auto NCIBAL_HILAYER_RESPONSE = "HiLayerResponse";
static const auto NCIBAL_SAK = "Sak";
static const auto NCIBAL_ATQA = "Atqa";
static const int NCIBAL_APP_DATA_LENGTH = 4;
static const auto NCIBAL_APP_DATA = "AppData";
static const int NCIBAL_PROTOCOL_INFO_LENGTH = 3;
static const auto NCIBAL_PROTOCOL_INFO = "ProtocolInfo";
// NDEF
static const auto NCIBAL_NDEF_MSG = "NdefMsg";
static const auto NCIBAL_NDEF_FORUM_TYPE = "NdefForumType";
static const auto NCIBAL_NDEF_TAG_MODE = "NdefTagMode";
// MifareUltralight
static const auto NCIBAL_MIFARE_ULTRALIGHT_C_TYPE = "MifareUltralightC";
static const int NCIBAL_MIFARE_ULTRALIGHT_C_RESPONSE_LENGTH = 16;
static const int NCIBAL_MIFARE_ULTRALIGHT_C_BLANK_CARD = 0;
static const int NCIBAL_MIFARE_ULTRALIGHT_C_VERSION_INFO_FIRST = 0x02;
static const int NCIBAL_MIFARE_ULTRALIGHT_C_VERSION_INFO_SECOND = 0x00;
static const int NCIBAL_MIFARE_ULTRALIGHT_C_NDEF_CC = 0xE1;
static const int NCIBAL_MIFARE_ULTRALIGHT_C_NDEF_MAJOR_VERSION = 0x20;
static const int NCIBAL_MIFARE_ULTRALIGHT_C_NDEF_TAG_SIZE = 0x06;
// Iso15693
static const auto NCIBAL_RESPONSE_FLAGS = "ResponseFlags";
static const auto NCIBAL_DSF_ID = "DsfId";
static const int NCIBAL_POLL_LENGTH_MIN = 2;
static const int DEFAULT_PRESENCE_CHECK_WATCH_DOG_TIMEOUT = 125;

static const auto NDEF_UNKNOWN_TYPE = -1;
static const auto NDEF_TYPE1_TAG = 1;
static const auto NDEF_TYPE2_TAG = 2;
static const auto NDEF_TYPE3_TAG = 3;
static const auto NDEF_TYPE4_TAG = 4;
static const auto NDEF_MIFARE_CLASSIC_TAG = 101;

static const auto TARGET_TYPE_UNKNOWN = -1;
static const auto TARGET_TYPE_ISO14443_3A = 1;
static const auto TARGET_TYPE_ISO14443_3B = 2;
static const auto TARGET_TYPE_ISO14443_4 = 3;
static const auto TARGET_TYPE_FELICA = 4;
static const auto TARGET_TYPE_V = 5;
static const auto TARGET_TYPE_NDEF = 6;
static const auto TARGET_TYPE_NDEF_FORMATABLE = 7;
static const auto TARGET_TYPE_MIFARE_CLASSIC = 8;
static const auto TARGET_TYPE_MIFARE_UL = 9;
static const auto TARGET_TYPE_KOVIO_BARCODE = 10;

class TagEndPoint : public ITagEndPoint {
public:
    TagEndPoint(const std::vector<int>& techList,
                const std::vector<int>& techHandles,
                const std::vector<int>& techLibNfcTypes,
                const std::string& uid,
                const std::vector<std::string>& techPollBytes,
                const std::vector<std::string>& techActBytes);
    virtual ~TagEndPoint() override;
    virtual bool Connect(int technology) override;
    virtual bool Disconnect() override;
    virtual bool Reconnect() override;
    virtual int Transceive(std::string& request, std::string& response) override;
    virtual bool PresenceCheck() override;
    virtual bool IsPresent() override;
    virtual void StartPresenceChecking(int presenceCheckDelay, TagDisconnectedCallBack callback) override;
    virtual void StopPresenceChecking() override;
    virtual std::vector<int> GetTechList() override;
    virtual void RemoveTechnology(int technology) override;
    virtual std::string GetUid() override;
    virtual std::weak_ptr<sdk::NfcMap> GetTechExtras() override;
    virtual int GetHandle() override;
    virtual bool MakeReadOnly() override;
    virtual std::string ReadNdef() override;
    virtual bool WriteNdef(std::string& data) override;
    virtual bool FormatNdef(const std::string& key) override;
    virtual bool IsNdefFormatable() override;
    virtual bool CheckNdef(std::vector<int>& ndefInfo) override;
    virtual int GetConnectedTechnology() override;

private:
    std::shared_ptr<sdk::NfcMap> GenerateBundle(int index);
    void PresenceChecking(int presenceCheckDelay, TagEndPoint::TagDisconnectedCallBack callback);
    void PausePresenceChecking();
    void ResumePresenceChecking();
    void AddNdefTech();
    int GetNdefType(int protocol) const;
    bool IsUltralightC();

    static OHOS::nfc::SynchronizeEvent mPresenceCheckWatchDog_;
    std::mutex mMutex_{};
    /* NFC-A NFC-B NFC-F NFC-V... */
    std::vector<int> mTechList_;
    std::vector<int> mTechHandles_;
    std::vector<int> mTechLibNfcTypes_;
    std::string mUid_;
    std::shared_ptr<sdk::NfcMap> mTechExtras_;
    std::vector<std::string> mTechPollBytes_;
    std::vector<std::string> mTechActBytes_;
    int mConnectedHandle_;
    int mConnectedTechIndex_;
    volatile bool mIsPresent_;
    volatile bool mIsPresenceChecking_;
    volatile bool mIsPausePresenceChecking_;
    bool mAddNdefTech_;
    /* IsoDep Felica ISO15693... */
    std::vector<int> mTechnologyList_{};
};
}  // namespace ncibal
}  // namespace nfc
}  // namespace OHOS
#endif /* TAG_END_POINT_H */