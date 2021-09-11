#include "ndef_formatable_tag.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "infc_agent_service_mock.h"
#include "ndef_message.h"
#include "ohos_application.h"
#include "tag_data.h"

using namespace OHOS::nfc::sdk;
using namespace OHOS::nfc::reader;
using namespace testing;

class NdefFormatableTagTest : public ::testing::Test {
public:
    virtual void SetUp() override
    {
        std::cout << " SetUpTestCase NdefFormatableTagTest." << std::endl;

        // invalid tag for exception test
        invalidtag_ = TagData::GetValidTag(Tag::EmTagTechnology::NFC_ISO_DEP_TECH);
        t_ = NdefFormatableTag::GetTag(invalidtag_);

        tsm_ = std::make_shared<ITagSessionMock>();
    }

    virtual void TearDown() override
    {
        std::cout << " TearDownTestCase NdefFormatableTagTest." << std::endl;
        invalidtag_ = nullptr;
        t_ = nullptr;
        tsm_ = nullptr;
    }

public:
    // tag data
    std::shared_ptr<Tag> invalidtag_;
    std::shared_ptr<NdefFormatableTag> t_;
    std::shared_ptr<ITagSessionMock> tsm_;
};

/**
 * @tc.number    : NFC_TAG_NdefFormatable_API_0001
 * @tc.name      : API NdefFormatable tag connect exception test
 * @tc.desc      : Api test
 */
TEST_F(NdefFormatableTagTest, Connect_Failed)
{
    EXPECT_EQ(t_->Connect(), 0);
}

/**
 * @tc.number    : NFC_TAG_NdefFormatable_API_0002
 * @tc.name      : API NdefFormatable tag GetTag exception test
 * @tc.desc      : Api test
 */
TEST_F(NdefFormatableTagTest, GetTag_Failed)
{
    auto tag =
        TagData::GetInvalidTag(Tag::EmTagTechnology::NFC_NDEF_FORMATABLE_TECH, EMTagParamInvalid::INVALID_EXTRA_DATA);
    auto t = NdefFormatableTag::GetTag(tag);
    EXPECT_FALSE(t);
}

/**
 * @tc.number    : NFC_TAG_NdefFormatable_API_0003
 * @tc.name      : API NdefFormatable tag Format exception test
 * @tc.desc      : Api test
 */
TEST_F(NdefFormatableTagTest, Format_Failed)
{
    auto msg = NdefMessage::GetNdefMessage("invalidNdefMessage");
    EXPECT_NE(t_->Format(msg), 0);

    std::shared_ptr<ITagSessionMock> tsm = std::make_shared<ITagSessionMock>();
    std::string id = data_valid_id;
    auto extra = TagData::GetValidExtraData(data_techlist_all);
    std::shared_ptr<Tag> invalidtag_ = TagData::GetTag(
        id, data_valid_service_handle, Tag::EmTagTechnology::NFC_NDEF_FORMATABLE_TECH, data_techlist_all, tsm, extra);
    std::shared_ptr<NdefFormatableTag> vt = NdefFormatableTag::GetTag(invalidtag_);
    EXPECT_EQ(vt->Connect(), 0);

    // 1 short record data
    std::vector<unsigned char> ms = {0xd1, 0x01, 0x12, 0x55, 0x03, 0x61, 0x73, 0x63, 0x69, 0x69, 0x2e,
                                     0x39, 0x31, 0x31, 0x63, 0x68, 0x61, 0x2e, 0x63, 0x6f, 0x6d, 0x2f};
    msg = NdefMessage::GetNdefMessage(std::string(ms.begin(), ms.end()));
    EXPECT_NE(vt->Format(msg), 0);

    EXPECT_CALL(*tsm, FormatNdef(_, _)).WillRepeatedly(testing::Return(true));
    EXPECT_NE(vt->Format(msg), 0);

    EXPECT_CALL(*tsm, IsNdef(_)).WillRepeatedly(testing::Return(true));
    EXPECT_NE(vt->Format(msg), 0);

    EXPECT_CALL(*tsm, IsNdef(_)).WillRepeatedly(testing::Return(false));
    EXPECT_NE(vt->Format(msg), 0);

    // 1 short record data
    ms = {0xdc, 0x01, 0x00, 0x00};
    msg = NdefMessage::GetNdefMessage(std::string(ms.begin(), ms.end()));
    EXPECT_CALL(*tsm, IsNdef(_)).WillRepeatedly(testing::Return(true));
    EXPECT_NE(vt->Format(msg), 0);
}

/**
 * @tc.number    : NFC_TAG_NdefFormatable_API_0004
 * @tc.name      : API NdefFormatable tag FormatReadOnly exception test
 * @tc.desc      : Api test
 */
TEST_F(NdefFormatableTagTest, FormatReadOnly_Failed)
{
    auto msg = NdefMessage::GetNdefMessage("invalidNdefMessage");
    EXPECT_NE(t_->FormatReadOnly(msg), 0);

    std::shared_ptr<ITagSessionMock> tsm = std::make_shared<ITagSessionMock>();
    std::string id = data_valid_id;
    auto extra = TagData::GetValidExtraData(data_techlist_all);
    std::shared_ptr<Tag> invalidtag_ = TagData::GetTag(
        id, data_valid_service_handle, Tag::EmTagTechnology::NFC_NDEF_FORMATABLE_TECH, data_techlist_all, tsm, extra);
    std::shared_ptr<NdefFormatableTag> vt = NdefFormatableTag::GetTag(invalidtag_);
    EXPECT_EQ(vt->Connect(), 0);

    EXPECT_NE(vt->FormatReadOnly(msg), 0);

    EXPECT_CALL(*tsm, CanMakeReadOnly(_)).WillRepeatedly(testing::Return(true));
    EXPECT_NE(vt->FormatReadOnly(msg), 0);
}

/**
 * @tc.number    : NFC_TAG_NdefFormatable_API_0005
 * @tc.name      : API NdefFormatable tag normal test
 * @tc.desc      : basic business test, include connect, Format, FormatReadOnly.
 */
TEST_F(NdefFormatableTagTest, Connect_Ok)
{
    std::shared_ptr<ITagSessionMock> tsm = std::make_shared<ITagSessionMock>();
    std::string id = data_valid_id;
    auto extra = TagData::GetValidExtraData(data_techlist_all);
    std::shared_ptr<Tag> invalidtag_ = TagData::GetTag(
        id, data_valid_service_handle, Tag::EmTagTechnology::NFC_NDEF_FORMATABLE_TECH, data_techlist_all, tsm, extra);
    std::shared_ptr<NdefFormatableTag> vt = NdefFormatableTag::GetTag(invalidtag_);
    EXPECT_EQ(vt->Connect(), 0);

    // 1 short record data
    std::vector<unsigned char> ms = {0xd1, 0x01, 0x12, 0x55, 0x03, 0x61, 0x73, 0x63, 0x69, 0x69, 0x2e,
                                     0x39, 0x31, 0x31, 0x63, 0x68, 0x61, 0x2e, 0x63, 0x6f, 0x6d, 0x2f};
    auto msg = NdefMessage::GetNdefMessage(std::string(ms.begin(), ms.end()));

    EXPECT_CALL(*tsm, IsNdef(_)).WillRepeatedly(testing::Return(true));
    EXPECT_CALL(*tsm, CanMakeReadOnly(_)).WillRepeatedly(testing::Return(true));

    EXPECT_EQ(vt->Format(msg), 0);
    EXPECT_EQ(vt->FormatReadOnly(msg), 0);
}