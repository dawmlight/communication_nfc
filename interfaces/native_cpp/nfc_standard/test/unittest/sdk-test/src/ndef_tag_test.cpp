#include "ndef_tag.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "infc_agent_service_mock.h"
#include "ndef_message.h"
#include "ohos_application.h"
#include "tag_data.h"

using namespace OHOS::nfc::sdk;
using namespace OHOS::nfc::reader;
using namespace testing;

class NdefTagTest : public ::testing::Test {
public:
    virtual void SetUp() override
    {
        std::cout << " SetUpTestCase NdefTagTest." << std::endl;

        // invalid tag for exception test
        invalidtag_ = TagData::GetValidTag(Tag::EmTagTechnology::NFC_ISO_DEP_TECH);
        t_ = NdefTag::GetTag(invalidtag_);

        tsm_ = std::make_shared<ITagSessionMock>();
    }

    virtual void TearDown() override
    {
        std::cout << " TearDownTestCase NdefTagTest." << std::endl;
        invalidtag_ = nullptr;
        t_ = nullptr;
        tsm_ = nullptr;
    }

public:
    // tag data
    std::shared_ptr<Tag> invalidtag_;
    std::shared_ptr<NdefTag> t_;
    std::shared_ptr<ITagSessionMock> tsm_;
};

/**
 * @tc.number    : NFC_Tag_NdefTag_API_0001
 * @tc.name      : API Connect test
 * @tc.desc      : Api test
 */
TEST_F(NdefTagTest, Connect_Failed)
{
    EXPECT_EQ(t_->Connect(), 0);
}

/**
 * @tc.number    : NFC_Tag_NdefTag_API_0002
 * @tc.name      : API GetTag exception test
 * @tc.desc      : Api test
 */
TEST_F(NdefTagTest, GetTag_Failed)
{
    auto tag = TagData::GetInvalidTag(Tag::EmTagTechnology::NFC_NDEF_TECH, EMTagParamInvalid::INVALID_EXTRA_DATA);
    auto t = NdefTag::GetTag(tag);
    EXPECT_FALSE(t);
}

/**
 * @tc.number    : NFC_Tag_NdefTag_API_0003
 * @tc.name      : API GetNdefTagType test
 * @tc.desc      : Api test
 */
TEST_F(NdefTagTest, GetNdefTagType_Failed)
{
    t_->GetNdefTagType();
}

/**
 * @tc.number    : NFC_Tag_NdefTag_API_0004
 * @tc.name      : API GetNdefTagMode test
 * @tc.desc      : Api test
 */
TEST_F(NdefTagTest, GetNdefTagMode_Failed)
{
    t_->GetNdefTagMode();
}

/**
 * @tc.number    : NFC_Tag_NdefTag_API_0005
 * @tc.name      : API GetCachedNdefMsg test
 * @tc.desc      : Api test
 */
TEST_F(NdefTagTest, GetCachedNdefMsg_Failed)
{
    auto msg = t_->GetCachedNdefMsg();
    EXPECT_FALSE(msg);
}

/**
 * @tc.number    : NFC_Tag_NdefTag_API_0006
 * @tc.name      : API IsNdefWritable test
 * @tc.desc      : Api test
 */
TEST_F(NdefTagTest, IsNdefWritable_Failed)
{
    auto tag =
        TagData::GetInValidNdefTags(NdefTag::EmNfcForumType::NFC_FORUM_TYPE_2, NdefTag::EmNdefTagMode::MODE_INVALID);
    auto t = NdefTag::GetTag(tag);
    EXPECT_EQ(t->IsNdefWritable(), false);
}

/**
 * @tc.number    : NFC_Tag_NdefTag_API_0007
 * @tc.name      : API ReadNdef test
 * @tc.desc      : Api test
 */
TEST_F(NdefTagTest, ReadNdef_Failed)
{
    auto d = t_->ReadNdef();
    EXPECT_FALSE(d);

    std::string id = data_valid_id;
    auto itsm = std::make_shared<ITagSessionMock>();
    auto extra = TagData::GetValidExtraData(data_techlist_all);
    std::shared_ptr<Tag> invalidtag_ = TagData::GetTag(
        id, data_valid_service_handle, Tag::EmTagTechnology::NFC_NDEF_TECH, data_techlist_all, itsm, extra);
    auto ivt = NdefTag::GetTag(invalidtag_);
    EXPECT_CALL(*itsm, IsPresent(_)).WillRepeatedly(testing::Return(false));
    d = ivt->ReadNdef();
    EXPECT_FALSE(d);

    EXPECT_CALL(*itsm, IsPresent(_)).WillRepeatedly(testing::Return(true));
    d = ivt->ReadNdef();
    EXPECT_FALSE(d);
}

/**
 * @tc.number    : NFC_Tag_NdefTag_API_0008
 * @tc.name      : API WriteNdef test
 * @tc.desc      : Api test
 */
TEST_F(NdefTagTest, WriteNdef_Failed)
{
    auto msg = NdefMessage::GetNdefMessage("invalidNdefMessage");
    EXPECT_NE(t_->WriteNdef(msg), 0);

    std::string id = data_valid_id;
    auto itsm = std::make_shared<ITagSessionMock>();
    auto extra = TagData::GetValidExtraData(data_techlist_all);
    std::shared_ptr<Tag> invalidtag_ = TagData::GetTag(
        id, data_valid_service_handle, Tag::EmTagTechnology::NFC_NDEF_TECH, data_techlist_all, itsm, extra);
    auto ivt = NdefTag::GetTag(invalidtag_);
    EXPECT_CALL(*itsm, IsNdef(_)).WillRepeatedly(testing::Return(false));
    EXPECT_NE(ivt->WriteNdef(msg), 0);
}

/**
 * @tc.number    : NFC_Tag_NdefTag_API_0009
 * @tc.name      : API IsEnableReadOnly test
 * @tc.desc      : Api test
 */
TEST_F(NdefTagTest, IsEnableReadOnly_Failed)
{
    EXPECT_EQ(t_->IsEnableReadOnly(), false);
}

/**
 * @tc.number    : NFC_Tag_NdefTag_API_0010
 * @tc.name      : API EnableReadOnly test
 * @tc.desc      : Api test
 */
TEST_F(NdefTagTest, EnableReadOnly_Failed)
{
    EXPECT_NE(t_->EnableReadOnly(), 0);

    std::string id = data_valid_id;
    auto itsm = std::make_shared<ITagSessionMock>();
    auto extra = TagData::GetValidExtraData(data_techlist_all);
    std::shared_ptr<Tag> invalidtag_ = TagData::GetTag(
        id, data_valid_service_handle, Tag::EmTagTechnology::NFC_NDEF_TECH, data_techlist_all, itsm, extra);
    auto ivt = NdefTag::GetTag(invalidtag_);
    EXPECT_CALL(*itsm, NdefMakeReadOnly(_)).WillRepeatedly(testing::Return(1));
    ivt->Connect();
    EXPECT_EQ(ivt->EnableReadOnly(), 1);
}

/**
 * @tc.number    : NFC_Tag_NdefTag_API_0011
 * @tc.name      : API GetNdefTagTypeString exception test
 * @tc.desc      : Api test
 */
TEST_F(NdefTagTest, GetNdefTagTypeString_Failed)
{
    std::string tt = t_->GetNdefTagTypeString(NdefTag::EmNfcForumType(22));
    EXPECT_STREQ(tt.c_str(), "");
}

/**
 * @tc.number    : NFC_Tag_NdefTag_API_0012
 * @tc.name      : API GetNdefTagTypeString test
 * @tc.desc      : normal basic business test, include connect, GetCachedNdefMsg, IsNdefWritable, ReadNdef, WriteNdef.
 */
TEST_F(NdefTagTest, Connect_Ok)
{
    // construct NdefMessage
    // 1 short record data
    std::vector<unsigned char> ms = {0xd1, 0x01, 0x12, 0x55, 0x03, 0x61, 0x73, 0x63, 0x69, 0x69, 0x2e,
                                     0x39, 0x31, 0x31, 0x63, 0x68, 0x61, 0x2e, 0x63, 0x6f, 0x6d, 0x2f};
    std::string msgs = std::string(ms.begin(), ms.end());
    auto msgdata = NdefMessage::GetNdefMessage(msgs);
    // std::shared_ptr<char[]> msgArray(new char[msgs.length()]);
    // for (std::size_t i = 0; i < msgs.length(); i++) {
    //    msgArray.get()[i] = msgs.at(i);
    //}

    std::string id = data_valid_id;
    auto itsm = std::make_shared<ITagSessionMock>();
    std::shared_ptr<NfcMap> extra =
        TagData::GetValidExtraData(data_techlist_1, 0x08, 1, 2, std::make_shared<std::string>(msgs));
    std::shared_ptr<Tag> invalidtag_ = TagData::GetTag(
        id, data_valid_service_handle, Tag::EmTagTechnology::NFC_NDEF_FORMATABLE_TECH, data_techlist_all, itsm, extra);
    auto vt = NdefTag::GetTag(invalidtag_);

    EXPECT_CALL(*itsm, NdefMakeReadOnly(_)).WillRepeatedly(testing::Return(1));
    EXPECT_CALL(*itsm, IsNdef(_)).WillRepeatedly(testing::Return(true));
    EXPECT_CALL(*itsm, IsPresent(_)).WillRepeatedly(testing::Return(true));
    EXPECT_CALL(*itsm, NdefRead(_)).WillRepeatedly(testing::Return(msgs));

    EXPECT_EQ(vt->Connect(), 0);

    auto msg = vt->GetCachedNdefMsg();
    EXPECT_TRUE(msg);

    EXPECT_EQ(vt->IsNdefWritable(), true);
    auto rd = vt->ReadNdef();
    EXPECT_TRUE(rd);

    EXPECT_EQ(vt->WriteNdef(msgdata), 0);
}

/**
 * @tc.number    : NFC_Tag_NdefTag_API_0013
 * @tc.name      : API GetNdefTagTypeString test
 * @tc.desc      : Api test
 */
TEST_F(NdefTagTest, GetNdefTagTypeString_Ok)
{
    auto tag = TagData::GetValidTag();
    auto t = NdefTag::GetTag(tag);
    std::vector<int> nft = {NdefTag::NFC_FORUM_TYPE_1,
                            NdefTag::NFC_FORUM_TYPE_2,
                            NdefTag::NFC_FORUM_TYPE_3,
                            NdefTag::NFC_FORUM_TYPE_4,
                            NdefTag::MIFARE_CLASSIC,
                            NdefTag::ICODE_SLI};
    for (auto i : nft) {
        std::string tt = t_->GetNdefTagTypeString(NdefTag::EmNfcForumType(i));
        EXPECT_STRNE(tt.c_str(), "");
    }

    std::string o = t_->GetNdefTagTypeString(NdefTag::EmNfcForumType::NFC_FORUM_TYPE_1_OTHER);
    EXPECT_STREQ(o.c_str(), "");
}