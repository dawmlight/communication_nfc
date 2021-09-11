#include "tag_data.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <cstring>

#include "infc_agent_service_mock.h"
#include "ndef_tag.h"
#include "ohos_application.h"
#include "tag.h"

using namespace std;
std::shared_ptr<ITagSessionMock> TagData::tagservice_ = data_valid_tagservice;
std::shared_ptr<NfcMap> TagData::extradata_ = data_null_extra;

std::shared_ptr<OHOS::nfc::sdk::Tag> TagData::GetValidTag(int tagTech)
{
    std::string id = data_valid_id;
    extradata_ = GetValidExtraData(data_techlist_all);
    return GetTag(id, data_valid_service_handle, tagTech, data_techlist_all, data_valid_tagservice, extradata_);
}

std::shared_ptr<OHOS::nfc::sdk::Tag> TagData::GetValidMCTags(int tagTech,
                                                             int sak,
                                                             std::string id,
                                                             std::vector<int> techList)
{
    extradata_ = GetValidExtraData(techList, sak);
    return GetTag(id, data_valid_service_handle, tagTech, techList, data_valid_tagservice, extradata_);
}

std::shared_ptr<OHOS::nfc::sdk::Tag> TagData::GetInValidNdefTags(int nfcForumType, int ndefTagMode)
{
    int tagTech = Tag::EmTagTechnology::NFC_NDEF_TECH;
    std::string id = data_valid_id;
    extradata_ = GetValidExtraData(data_techlist_all, 0x08, nfcForumType, ndefTagMode);
    return GetTag(id, data_valid_service_handle, tagTech, data_techlist_all, data_valid_tagservice, extradata_);
}

std::shared_ptr<OHOS::nfc::sdk::Tag> TagData::GetValidNdefFormatableTags()
{
    std::string id = data_valid_id;
    int tagTech = Tag::EmTagTechnology::NFC_NDEF_FORMATABLE_TECH;
    extradata_ = GetValidExtraData(data_techlist_all, 0x08);

    EXPECT_CALL(*tagservice_, IsNdef(testing::_)).WillRepeatedly(testing::Return(true));
    EXPECT_CALL(*tagservice_, CanMakeReadOnly(testing::_)).WillRepeatedly(testing::Return(true));
    return GetTag(id, data_valid_service_handle, tagTech, data_techlist_all, data_valid_tagservice, extradata_);
}

std::shared_ptr<OHOS::nfc::sdk::Tag> TagData::GetInvalidTag(int tagTech, int invalidType)
{
    std::string id = (invalidType & INVALID_ID) ? data_invalid_id : data_valid_id;
    int serviceHandle =
        (invalidType & data_invalid_service_handle) ? data_invalid_service_handle : data_valid_service_handle;
    int connectTech = (invalidType & INVALID_CONNECT_TECH) ? data_invalid_connect_tech : tagTech;
    std::vector<int> techList = (invalidType & INVALID_TECH_LIST) ? data_techlist_0 : data_techlist_2;

    extradata_ = GetValidExtraData(techList);
    if (invalidType & INVALID_TAG_SERVICE) {
        tagservice_ = data_invalid_tagservice;
    }
    if (invalidType & INVALID_EXTRA_DATA) {
        extradata_ = data_invalid_extra;
    }

    std::shared_ptr<OHOS::nfc::sdk::Tag> tag =
        GetTag(id, serviceHandle, connectTech, techList, tagservice_, extradata_);

    return tag;
}

std::shared_ptr<OHOS::nfc::sdk::Tag> TagData::GetTag(std::string& id,
                                                     int serviceHandle,
                                                     int connectTech,
                                                     std::vector<int> techList,
                                                     std::weak_ptr<ITagSessionMock> tagservice,
                                                     std::weak_ptr<NfcMap> extra)
{
    auto t = std::make_shared<Tag>(id, techList, extra, serviceHandle, tagservice);
    t->SetConnectTagTech(connectTech);
    return t;
}

std::shared_ptr<NfcMap> TagData::GetValidExtraData(
    std::vector<int> techList, int sak, int nfcForumType, int ndefTagMode, std::shared_ptr<std::string> msg)
{
    std::shared_ptr<NfcMap> extra = std::make_shared<NfcMap>();
    for (int i = 0; i < int(techList.size()); i++) {
        std::string key = "Tech_Extra_Data_" + std::to_string(i);
        std::shared_ptr<NfcMap> extra_value = std::make_shared<NfcMap>();
        extra->PutNfcMap(key, extra_value);
        switch (techList[i]) {
            case Tag::NFC_NDEF_TECH:
                extra_value->PutLong(Tag::NDEF_FORUM_TYPE, nfcForumType);
                extra_value->PutLong(Tag::NDEF_TAG_MODE, ndefTagMode);
                if (msg) {
                    extra_value->PutCharArray(Tag::NDEF_MSG, msg);
                } else {
                    extra_value->PutCharArray(Tag::NDEF_MSG, std::make_shared<std::string>());
                }
                break;
            case Tag::NFC_ISO_DEP_TECH:
                // isodep
                extra_value->PutCharArray(Tag::HISTORICAL_BYTES, std::make_shared<std::string>());
                extra_value->PutCharArray(Tag::HILAYER_RESPONSE, std::make_shared<std::string>());
                // iso 14443-3a
                extra_value->PutLong(Tag::SAK, sak);
                extra_value->PutCharArray(Tag::ATQA, std::make_shared<std::string>());
                // iso 14443-3b
                extra_value->PutCharArray(Tag::APP_DATA, std::make_shared<std::string>());
                extra_value->PutCharArray(Tag::PROTOCOL_INFO, std::make_shared<std::string>());
                break;
            case Tag::NFC_MIFARE_CLASSIC_TECH:
                extra_value->PutLong(Tag::SAK, sak);
                break;
            case Tag::NFC_MIFARE_ULTRALIGHT_TECH:
                extra_value->PutLong(Tag::SAK, (sak & 0xF0));
                extra_value->PutLong(Tag::MIFARE_ULTRALIGHT_C_TYPE, (sak & 0x0F));
                break;
            case Tag::NFC_ISO_15693_TECH:
                extra_value->PutChar(Tag::DSF_ID, data_valid_dsf_id);
                extra_value->PutChar(Tag::RESPONSE_FLAGS, data_invalid_resp_flags);
                break;
            case Tag::NFC_FELICA_TECH:
                break;
            case Tag::NFC_NDEF_FORMATABLE_TECH:
                break;
            default:
                break;
        }
    }
    return extra;
}

std::shared_ptr<ITagSessionMock> TagData::GetTagServiceForTest()
{
    return tagservice_;
}