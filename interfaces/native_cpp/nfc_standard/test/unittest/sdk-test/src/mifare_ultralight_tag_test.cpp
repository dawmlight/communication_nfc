#include "mifare_ultralight_tag.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "infc_agent_service_mock.h"
#include "ohos_application.h"
#include "tag_data.h"

using namespace OHOS::nfc::sdk;
using namespace OHOS::nfc::reader;
using namespace testing;

class MifareUltralightTagTest : public ::testing::Test {
public:
    virtual void SetUp() override
    {
        std::cout << " SetUpTestCase MifareUltralightTagTest." << std::endl;

        // invalid tag for exception test
        invalidtag_ = TagData::GetValidTag(Tag::EmTagTechnology::NFC_ISO_DEP_TECH);
        t_ = MifareUltralightTag::GetTag(invalidtag_);

        tsm_ = std::make_shared<ITagSessionMock>();
    }

    virtual void TearDown() override
    {
        std::cout << " TearDownTestCase MifareUltralightTagTest." << std::endl;
        invalidtag_ = nullptr;
        t_ = nullptr;
        tsm_ = nullptr;
    }

public:
    // tag data
    std::shared_ptr<Tag> invalidtag_;
    std::shared_ptr<MifareUltralightTag> t_;
    std::shared_ptr<ITagSessionMock> tsm_;
};

/**
 * @tc.number    : NFC_TAG_MifareUltralight_API_0001
 * @tc.name      : API Connect exception test
 * @tc.desc      : Api test
 */
TEST_F(MifareUltralightTagTest, Connect_Failed)
{
    EXPECT_NE(t_->Connect(), 1);
}

/**
 * @tc.number    : NFC_TAG_MifareUltralight_API_0002
 * @tc.name      : API GetTag exception test
 * @tc.desc      : Api test
 */
TEST_F(MifareUltralightTagTest, GetTag_Failed)
{
    auto tag =
        TagData::GetInvalidTag(Tag::EmTagTechnology::NFC_MIFARE_ULTRALIGHT_TECH, EMTagParamInvalid::INVALID_EXTRA_DATA);
    auto t = MifareUltralightTag::GetTag(tag);
    EXPECT_FALSE(t);
}

/**
 * @tc.number    : NFC_TAG_MifareUltralight_API_0003
 * @tc.name      : API ReadMultiplePages exception test
 * @tc.desc      : Api test
 */
TEST_F(MifareUltralightTagTest, ReadMultiplePages_Failed)
{
    EXPECT_EQ(t_->ReadMultiplePages(1), "");

    EXPECT_EQ(t_->Connect(), 0);
    EXPECT_EQ(t_->ReadMultiplePages(0), "");
    EXPECT_EQ(t_->ReadMultiplePages(-1), "");
    EXPECT_EQ(t_->ReadMultiplePages(MifareUltralightTag::MU_MAX_PAGE_COUNT), "");
    EXPECT_EQ(t_->ReadMultiplePages(MifareUltralightTag::MU_MAX_PAGE_COUNT + 1), "");
}

/**
 * @tc.number    : NFC_TAG_MifareUltralight_API_0004
 * @tc.name      : API WriteSinglePages exception test
 * @tc.desc      : Api test
 */
TEST_F(MifareUltralightTagTest, WriteSinglePages_Failed)
{
    // excption for not connect tag first
    EXPECT_NE(t_->WriteSinglePages(1, "RAND"), 0);

    // tag connnected, exception for pageindex err
    EXPECT_EQ(t_->Connect(), 0);
    EXPECT_NE(t_->WriteSinglePages(0, "RANDOM"), 0);
    EXPECT_NE(t_->WriteSinglePages(-1, "RANDOM"), 0);
    EXPECT_NE(t_->WriteSinglePages(MifareUltralightTag::MU_MAX_PAGE_COUNT, "RANDOM"), 0);
    EXPECT_NE(t_->WriteSinglePages(MifareUltralightTag::MU_MAX_PAGE_COUNT + 1, "RANDOM"), 0);
}

/**
 * @tc.number    : NFC_TAG_MifareUltralight_API_0005
 * @tc.name      : API GetType exception test
 * @tc.desc      : Api test
 */
TEST_F(MifareUltralightTagTest, GetType_Failed)
{
    EXPECT_EQ(t_->GetType(), MifareUltralightTag::EmMifareUltralightType::TYPE_UNKOWN);
}

/**
 * @tc.number    : NFC_TAG_MifareUltralight_API_0006
 * @tc.name      : API MifareUltralight Tag normal  test
 * @tc.desc      : basic business test, include connect, ReadMultiplePages, WriteSinglePages.
 */
TEST_F(MifareUltralightTagTest, Connect_Ok)
{
    std::string id = data_valid_id;
    auto itsm = std::make_shared<ITagSessionMock>();
    std::shared_ptr<NfcMap> extra = std::make_shared<NfcMap>();
    std::shared_ptr<Tag> invalidtag_ = TagData::GetTag(
        id, data_valid_service_handle, Tag::EmTagTechnology::NFC_ISO_15693_TECH, data_techlist_all, itsm, extra);
    auto vt = MifareUltralightTag::GetTag(invalidtag_);

    EXPECT_EQ(vt->Connect(), 0);
    EXPECT_EQ(vt->ReadMultiplePages(1), "");

    std::unique_ptr<ResResult> res = std::make_unique<ResResult>();
    res->SetResult(0);
    EXPECT_CALL(*itsm, SendRawFrame(_, _, _)).WillRepeatedly(testing::Return(testing::ByMove(std::move(res))));
    EXPECT_EQ(vt->WriteSinglePages(1, "RAND"), 0);
}

/**
 * @tc.number    : NFC_TAG_MifareUltralight_API_0006
 * @tc.name      : API MifareUltralight get tag normal  test
 * @tc.desc      : Api test
 */
TEST_F(MifareUltralightTagTest, GetTag_Ok)
{
    // construct a valid tag
    char id[4] = {0x04, 0x02, 0x03, 0x01};
    std::string ivtid(id, 4);
    auto ivt = TagData::GetValidMCTags(Tag::EmTagTechnology::NFC_MIFARE_ULTRALIGHT_TECH, 0x0F, ivtid, data_techlist_1U);
    auto ut = MifareUltralightTag::GetTag(ivt);
    EXPECT_TRUE(ut);

    ivt = TagData::GetValidMCTags(Tag::EmTagTechnology::NFC_MIFARE_ULTRALIGHT_TECH, 0, ivtid, data_techlist_1U);
    ut = MifareUltralightTag::GetTag(ivt);
    EXPECT_TRUE(ut);
}