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
#ifndef TAG_H
#define TAG_H

#include <array>
#include <memory>
#include <string>
#include <vector>

#include "parcel.h"

namespace OHOS {
class IRemoteObject;
namespace nfc {
namespace reader {
class ITagSession;
}

namespace sdk {
class BasicTagSession;
class NdefTag;
class NdefFormatableTag;
class NfcMap;

class Tag final : public Parcelable {
public:
    const int MAX_TAG_TECH_NUM = 10;
    static const int SEND_COMMAND_HEAD_LEN_2 = 2;
    static const int SEND_COMMAND_MAX_LEN = 256;

    // define TagExternData Name
    // ISODEP
    static constexpr const auto HISTORICAL_BYTES = "HistoricalBytes";
    static constexpr const auto HILAYER_RESPONSE = "HiLayerResponse";
    // iso 14443-3a
    static constexpr const auto SAK = "Sak";
    static constexpr const auto ATQA = "Atqa";
    // iso 14443-3b
    static constexpr const auto APP_DATA = "AppData";
    static constexpr const auto PROTOCOL_INFO = "ProtocolInfo";
    // NDEF
    static constexpr const auto NDEF_MSG = "NdefMsg";
    static constexpr const auto NDEF_FORUM_TYPE = "NdefForumType";
    static constexpr const auto NDEF_TAG_MODE = "NdefTagMode";
    // MifareUltralight
    static constexpr const auto MIFARE_ULTRALIGHT_C_TYPE = "MifareUltralightC";
    // Iso15693
    static constexpr const auto RESPONSE_FLAGS = "ResponseFlags";
    static constexpr const auto DSF_ID = "DsfId";

    enum EmTagTechnology {
        NFC_INVALID_TECH = -1,
        NFC_NDEF_TECH = 1,
        NFC_ISO_DEP_TECH = 2,
        NFC_MIFARE_CLASSIC_TECH = 3,
        NFC_MIFARE_ULTRALIGHT_TECH = 4,
        NFC_ISO_15693_TECH = 5,
        NFC_FELICA_TECH = 6,
        NFC_NDEF_FORMATABLE_TECH = 7
    };

public:
    Tag(std::string& id,
        std::vector<int> technologyList,
        std::weak_ptr<NfcMap> tagExternData,
        int serviceHandle,
        OHOS::sptr<reader::ITagSession> tagService);
    ~Tag();

    std::string GetTagId() const;
    std::vector<int> GetTagTechnologyList() const;

    bool Marshalling(Parcel& parcel) const override;
    static Tag* Unmarshalling(Parcel& parcel);

    std::string GetStringExternData(std::weak_ptr<NfcMap> extraData, const std::string& externName);
    int GetIntExternData(std::weak_ptr<NfcMap> extraData, const std::string& externName);
    std::weak_ptr<NfcMap> GetTagExternData() const;
    std::weak_ptr<NfcMap> GetTechExtras(int tech);
    bool IsSupportTech(int technology);
    int GetServiceHandle() const;
    int GetConnectTagTech() const;
    void SetConnectTagTech(int connectTech);

private:
    OHOS::sptr<reader::ITagSession> GetTagService() const;

private:
    int mServiceHandle_;
    int mConnectTagTech_;
    std::string mId_;
    std::vector<int> mTechnologyList_;

    OHOS::sptr<reader::ITagSession> mTagService_;
    std::shared_ptr<NfcMap> mTagExternData_;
    friend class BasicTagSession;
    friend class NdefTag;
    friend class NdefFormatableTag;
};
}  // namespace sdk
}  // namespace nfc
}  // namespace OHOS

#endif  // TAG_H
