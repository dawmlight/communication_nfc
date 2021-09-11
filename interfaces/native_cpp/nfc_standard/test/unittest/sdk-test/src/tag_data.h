#ifndef TAG_DATA_H
#define TAG_DATA_H

#include "itagsession_mock.h"
#include "nfc_agent.h"
#include "nfc_map.h"
#include "tag.h"

using OHOS::AppExecFwk::Ability;
using OHOS::AppExecFwk::OHOSApplication;
using OHOS::nfc::sdk::ITagSessionMock;
using OHOS::nfc::sdk::NfcMap;
using OHOS::nfc::sdk::Tag;

// TagId
static constexpr const auto data_valid_id = "4020";
static constexpr const auto data_invalid_id = "";

static constexpr auto data_valid_service_handle = 1;
static constexpr auto data_invalid_service_handle = 0;

static constexpr auto data_valid_connect_tech = 0x01;
static constexpr auto data_invalid_connect_tech = -1;

static auto data_valid_tagservice = std::make_shared<ITagSessionMock>();
static auto data_invalid_tagservice = std::shared_ptr<ITagSessionMock>();

static auto data_null_extra = std::make_shared<NfcMap>();
static auto data_invalid_extra = std::shared_ptr<NfcMap>();

static std::vector<int> data_techlist_0 = {Tag::EmTagTechnology::NFC_INVALID_TECH};
static std::vector<int> data_techlist_1 = {Tag::EmTagTechnology::NFC_NDEF_TECH};
static std::vector<int> data_techlist_1U = {Tag::EmTagTechnology::NFC_MIFARE_ULTRALIGHT_TECH};
static std::vector<int> data_techlist_2 = {Tag::EmTagTechnology::NFC_ISO_DEP_TECH,
                                           Tag::EmTagTechnology::NFC_MIFARE_CLASSIC_TECH};
static std::vector<int> data_techlist_3 = {Tag::EmTagTechnology::NFC_NDEF_TECH,
                                           Tag::EmTagTechnology::NFC_ISO_DEP_TECH,
                                           Tag::EmTagTechnology::NFC_MIFARE_CLASSIC_TECH};
static std::vector<int> data_techlist_4 = {Tag::EmTagTechnology::NFC_NDEF_TECH,
                                           Tag::EmTagTechnology::NFC_ISO_DEP_TECH,
                                           Tag::EmTagTechnology::NFC_MIFARE_CLASSIC_TECH,
                                           Tag::EmTagTechnology::NFC_MIFARE_ULTRALIGHT_TECH};
static std::vector<int> data_techlist_5 = {Tag::EmTagTechnology::NFC_NDEF_TECH,
                                           Tag::EmTagTechnology::NFC_ISO_DEP_TECH,
                                           Tag::EmTagTechnology::NFC_MIFARE_CLASSIC_TECH,
                                           Tag::EmTagTechnology::NFC_MIFARE_ULTRALIGHT_TECH,
                                           Tag::EmTagTechnology::NFC_ISO_15693_TECH};
static std::vector<int> data_techlist_6 = {Tag::EmTagTechnology::NFC_NDEF_TECH,
                                           Tag::EmTagTechnology::NFC_ISO_DEP_TECH,
                                           Tag::EmTagTechnology::NFC_MIFARE_CLASSIC_TECH,
                                           Tag::EmTagTechnology::NFC_MIFARE_ULTRALIGHT_TECH,
                                           Tag::EmTagTechnology::NFC_ISO_15693_TECH,
                                           Tag::EmTagTechnology::NFC_FELICA_TECH};
static std::vector<int> data_techlist_all = {Tag::EmTagTechnology::NFC_NDEF_TECH,
                                             Tag::EmTagTechnology::NFC_ISO_DEP_TECH,
                                             Tag::EmTagTechnology::NFC_MIFARE_CLASSIC_TECH,
                                             Tag::EmTagTechnology::NFC_MIFARE_ULTRALIGHT_TECH,
                                             Tag::EmTagTechnology::NFC_ISO_15693_TECH,
                                             Tag::EmTagTechnology::NFC_FELICA_TECH,
                                             Tag::EmTagTechnology::NFC_NDEF_FORMATABLE_TECH};

enum EMTagParamInvalid {
    VALID = 0,
    INVALID_ID = 0x01,
    INVALID_SERVICE_HANDLE = 0x02,
    INVALID_CONNECT_TECH = 0x04,
    INVALID_TECH_LIST = 0x08,
    INVALID_TAG_SERVICE = 0x10,
    INVALID_EXTRA_DATA = 0x20,
    INVALID_AllPARAM = 0xFF
};

// ISO15693
static constexpr auto data_valid_dsf_id = 0x01;
static constexpr auto data_invalid_dsf_id = 0;
static constexpr auto data_valid_resp_flags = 0x01;
static constexpr auto data_invalid_resp_flags = 0;

class TagData {
public:
    static std::shared_ptr<OHOS::nfc::sdk::Tag> GetValidTag(int tagTech = Tag::EmTagTechnology::NFC_NDEF_TECH);
    // mifareClassic tag
    static std::shared_ptr<OHOS::nfc::sdk::Tag> GetValidMCTags(
        int tagTech = Tag::EmTagTechnology::NFC_MIFARE_CLASSIC_TECH,
        int sak = 0x08,
        std::string id = data_invalid_id,
        std::vector<int> techList = data_techlist_all);
    // Ndef
    static std::shared_ptr<OHOS::nfc::sdk::Tag> GetInValidNdefTags(int nfcForumType = 1, int ndefTagMode = 2);
    // NdefFormatable
    static std::shared_ptr<OHOS::nfc::sdk::Tag> GetValidNdefFormatableTags();

    static std::shared_ptr<OHOS::nfc::sdk::Tag> GetInvalidTag(int tagTech, int invalidType);
    static std::shared_ptr<OHOS::nfc::sdk::Tag> GetTag(std::string& id,
                                                       int serviceHandle,
                                                       int connectTech,
                                                       std::vector<int> techList,
                                                       std::weak_ptr<ITagSessionMock> tagservice,
                                                       std::weak_ptr<NfcMap> extra);

    static std::shared_ptr<NfcMap> GetValidExtraData(
        std::vector<int> techList,
        int sak = 0x08,
        int nfcForumType = 1,
        int ndefTagMode = 2,
        std::shared_ptr<std::string> msg = std::make_shared<std::string>());

    static std::shared_ptr<ITagSessionMock> GetTagServiceForTest();

private:
    // tag related
    static std::shared_ptr<ITagSessionMock> tagservice_;
    static std::shared_ptr<NfcMap> extradata_;
};

#endif  // !TAG_DATA_H