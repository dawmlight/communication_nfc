#include "iso15693_tag.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>

#include "infc_agent_service_mock.h"
#include "itagsession_mock.h"
#include "ohos_application.h"
#include "tag_data.h"

using namespace OHOS::nfc::sdk;
using namespace OHOS::nfc::reader;
using namespace testing;
using OHOS::nfc::sdk::NfcMap;

class Iso15693TagTest : public ::testing::Test {
public:
    virtual void SetUp() override
    {
        std::cout << " SetUpTestCase Iso15693TagTest." << std::endl;

        // invalid tag for exception test
        invalidtag_ = TagData::GetValidTag(Tag::EmTagTechnology::NFC_ISO_DEP_TECH);
        t_ = Iso15693Tag::GetTag(invalidtag_);

        tsm_ = std::make_shared<ITagSessionMock>();
    }

    virtual void TearDown() override
    {
        std::cout << " TearDownTestCase Iso15693TagTest." << std::endl;
        invalidtag_ = nullptr;
        t_ = nullptr;
        tsm_ = nullptr;
    }

public:
    // tag data
    std::shared_ptr<Tag> invalidtag_;
    std::shared_ptr<Iso15693Tag> t_;
    std::shared_ptr<ITagSessionMock> tsm_;
};

/**
 * @tc.number    : NFC_TAG_ISO15693_API_0001
 * @tc.name      : API Connect fail test
 * @tc.desc      : Api test
 */
TEST_F(Iso15693TagTest, Connect_Failed)
{
    EXPECT_CALL(*tsm_, Connect(_, _)).WillRepeatedly(Return(true));
    EXPECT_EQ(t_->Connect(), 0);
}

/**
 * @tc.number    : NFC_TAG_ISO15693_API_0002
 * @tc.name      : API IsConnect fail test
 * @tc.desc      : Api test
 */
TEST_F(Iso15693TagTest, IsConnect_Failed)
{
    EXPECT_FALSE(t_->IsConnect());
}

/**
 * @tc.number    : NFC_TAG_ISO15693_API_0003
 * @tc.name      : API close fail test
 * @tc.desc      : Api test
 */
TEST_F(Iso15693TagTest, Close_Failed)
{
    EXPECT_NE(t_->Close(), 0);
}

/**
 * @tc.number    : NFC_TAG_ISO15693_API_0004
 * @tc.name      : API GetTagId test
 * @tc.desc      : Api test
 */
TEST_F(Iso15693TagTest, GetTagId_Test)
{
    t_->GetTagId();
}

/**
 * @tc.number    : NFC_TAG_ISO15693_API_0005
 * @tc.name      : API SendCommand test
 * @tc.desc      : Api test
 */
TEST_F(Iso15693TagTest, SendCommand_Failed)
{
    std::string cmd = "invalid command";
    int res = 0;
    std::string id = t_->SendCommand(cmd, false, res);
    EXPECT_STREQ(id.c_str(), "");
}

/**
 * @tc.number    : NFC_TAG_ISO15693_API_0006
 * @tc.name      : API GetMaxSendCommandLength test
 * @tc.desc      : Api test
 */
TEST_F(Iso15693TagTest, GetMaxSendCommandLength_Test)
{
    EXPECT_NE(t_->GetMaxSendCommandLength(), 0);
}

/**
 * @tc.number    : NFC_TAG_ISO15693_API_0007
 * @tc.name      : API IsSupportApduExtended test
 * @tc.desc      : Api test
 */
TEST_F(Iso15693TagTest, IsSupportApduExtended_Test)
{
    EXPECT_FALSE(t_->IsSupportApduExtended());
}

/**
 * @tc.number    : NFC_TAG_ISO15693_API_0008
 * @tc.name      : API GetInvalidTag exception test
 * @tc.desc      : Api test
 */
TEST_F(Iso15693TagTest, GetTag_Failed)
{
    auto tag = TagData::GetInvalidTag(Tag::EmTagTechnology::NFC_ISO_15693_TECH, EMTagParamInvalid::INVALID_EXTRA_DATA);
    auto it = Iso15693Tag::GetTag(tag);
    EXPECT_FALSE(it);

    std::string id = data_valid_id;
    auto extra = std::make_shared<NfcMap>();
    auto itsm = std::make_shared<ITagSessionMock>();
    auto ut = std::make_shared<Tag>(id, data_techlist_1, extra, data_valid_service_handle, itsm);
    it = Iso15693Tag::GetTag(ut);
    EXPECT_FALSE(it);
}

/**
 * @tc.number    : NFC_TAG_ISO15693_API_0009
 * @tc.name      : API ReadSingleBlock exception test
 * @tc.desc      : Api test
 */
TEST_F(Iso15693TagTest, ReadSingleBlock_Failed)
{
    std::string d = t_->ReadSingleBlock(1, 1);
    EXPECT_STREQ(d.c_str(), "");

    d = t_->ReadSingleBlock(-1, 1);
    EXPECT_STREQ(d.c_str(), "");
    d = t_->ReadSingleBlock(Iso15693Tag::ISO15693_MAX_FLAG_COUNT, 1);
    EXPECT_STREQ(d.c_str(), "");
    d = t_->ReadSingleBlock(Iso15693Tag::ISO15693_MAX_FLAG_COUNT + 1, 1);
    EXPECT_STREQ(d.c_str(), "");

    d = t_->ReadSingleBlock(1, -1);
    EXPECT_STREQ(d.c_str(), "");
    d = t_->ReadSingleBlock(1, Iso15693Tag::ISO15693_MAX_BLOCK_INDEX);
    EXPECT_STREQ(d.c_str(), "");
    d = t_->ReadSingleBlock(1, Iso15693Tag::ISO15693_MAX_BLOCK_INDEX + 1);
    EXPECT_STREQ(d.c_str(), "");
}

/**
 * @tc.number    : NFC_TAG_ISO15693_API_0010
 * @tc.name      : API WriteSingleBlock exception test
 * @tc.desc      : Api test
 */
TEST_F(Iso15693TagTest, WriteSingleBlock_Failed)
{
    EXPECT_NE(t_->WriteSingleBlock(1, 1, "RANDOM"), 0);

    t_->Connect();
    EXPECT_NE(t_->WriteSingleBlock(-1, 1, "RANDOM"), 0);
    EXPECT_NE(t_->WriteSingleBlock(Iso15693Tag::ISO15693_MAX_FLAG_COUNT, 1, "RANDOM"), 0);
    EXPECT_NE(t_->WriteSingleBlock(Iso15693Tag::ISO15693_MAX_FLAG_COUNT + 1, 1, "RANDOM"), 0);

    EXPECT_NE(t_->WriteSingleBlock(1, -1, "RANDOM"), 0);
    EXPECT_NE(t_->WriteSingleBlock(1, Iso15693Tag::ISO15693_MAX_FLAG_COUNT, "RANDOM"), 0);
    EXPECT_NE(t_->WriteSingleBlock(1, Iso15693Tag::ISO15693_MAX_FLAG_COUNT + 1, "RANDOM"), 0);
}

/**
 * @tc.number    : NFC_TAG_ISO15693_API_0011
 * @tc.name      : API LockSingleBlock exception test
 * @tc.desc      : Api test
 */
TEST_F(Iso15693TagTest, LockSingleBlock_Failed)
{
    EXPECT_NE(t_->LockSingleBlock(1, 1), 0);

    t_->Connect();
    EXPECT_NE(t_->LockSingleBlock(-1, 1), 0);
    EXPECT_NE(t_->LockSingleBlock(Iso15693Tag::ISO15693_MAX_FLAG_COUNT, 1), 0);
    EXPECT_NE(t_->LockSingleBlock(Iso15693Tag::ISO15693_MAX_FLAG_COUNT + 1, 1), 0);

    EXPECT_NE(t_->LockSingleBlock(1, -1), 0);
    EXPECT_NE(t_->LockSingleBlock(1, Iso15693Tag::ISO15693_MAX_BLOCK_INDEX), 0);
    EXPECT_NE(t_->LockSingleBlock(1, Iso15693Tag::ISO15693_MAX_BLOCK_INDEX + 1), 0);
}

/**
 * @tc.number    : NFC_TAG_ISO15693_API_0012
 * @tc.name      : API ReadMultipleBlock exception test
 * @tc.desc      : Api test
 */
TEST_F(Iso15693TagTest, ReadMultipleBlock_Failed)
{
    std::string d = t_->ReadMultipleBlock(1, 1, 2);
    EXPECT_STREQ(d.c_str(), "");

    t_->Connect();
    d = t_->ReadMultipleBlock(-1, 1, 2);
    EXPECT_STREQ(d.c_str(), "");
    d = t_->ReadMultipleBlock(Iso15693Tag::ISO15693_MAX_FLAG_COUNT, 1, 2);
    EXPECT_STREQ(d.c_str(), "");
    d = t_->ReadMultipleBlock(Iso15693Tag::ISO15693_MAX_FLAG_COUNT + 1, 1, 2);
    EXPECT_STREQ(d.c_str(), "");

    d = t_->ReadMultipleBlock(1, -1, 2);
    EXPECT_STREQ(d.c_str(), "");
    d = t_->ReadMultipleBlock(1, Iso15693Tag::ISO15693_MAX_BLOCK_INDEX, 2);
    EXPECT_STREQ(d.c_str(), "");
    d = t_->ReadMultipleBlock(1, Iso15693Tag::ISO15693_MAX_BLOCK_INDEX + 1, 2);
    EXPECT_STREQ(d.c_str(), "");

    d = t_->ReadMultipleBlock(1, 1, -1);
    EXPECT_STREQ(d.c_str(), "");
    d = t_->ReadMultipleBlock(1, 1, Iso15693Tag::ISO15693_MAX_BLOCK_INDEX);
    EXPECT_STREQ(d.c_str(), "");
    d = t_->ReadMultipleBlock(1, 1, Iso15693Tag::ISO15693_MAX_BLOCK_INDEX + 1);
    EXPECT_STREQ(d.c_str(), "");
}

/**
 * @tc.number    : NFC_TAG_ISO15693_API_0013
 * @tc.name      : API WriteMultipleBlock exception test
 * @tc.desc      : Api test
 */
TEST_F(Iso15693TagTest, WriteMultipleBlock_Failed)
{
    EXPECT_NE(t_->WriteMultipleBlock(1, 1, 1, "RANDOM"), 0);

    t_->Connect();
    EXPECT_NE(t_->WriteMultipleBlock(-1, 1, 1, "RANDOM"), 0);
    EXPECT_NE(t_->WriteMultipleBlock(Iso15693Tag::ISO15693_MAX_FLAG_COUNT, 1, 1, "RANDOM"), 0);
    EXPECT_NE(t_->WriteMultipleBlock(Iso15693Tag::ISO15693_MAX_FLAG_COUNT + 1, 1, 1, "RANDOM"), 0);

    EXPECT_NE(t_->WriteMultipleBlock(1, -1, 1, "RANDOM"), 0);
    EXPECT_NE(t_->WriteMultipleBlock(1, Iso15693Tag::ISO15693_MAX_BLOCK_INDEX, 1, "RANDOM"), 0);
    EXPECT_NE(t_->WriteMultipleBlock(1, Iso15693Tag::ISO15693_MAX_BLOCK_INDEX + 1, 1, "RANDOM"), 0);

    EXPECT_NE(t_->WriteMultipleBlock(1, 1, -1, "RANDOM"), 0);
    EXPECT_NE(t_->WriteMultipleBlock(1, 1, Iso15693Tag::ISO15693_MAX_BLOCK_INDEX + 1, "RANDOM"), 0);
}

/**
 * @tc.number    : NFC_TAG_ISO15693_API_0014
 * @tc.name      : API GetDsfId test
 * @tc.desc      : Api test
 */
TEST_F(Iso15693TagTest, GetDsfId_Test)
{
    t_->GetDsfId();
}

/**
 * @tc.number    : NFC_TAG_ISO15693_API_0015
 * @tc.name      : API GetRespFlags test
 * @tc.desc      : Api test
 */
TEST_F(Iso15693TagTest, GetRespFlags_Test)
{
    t_->GetRespFlags();
}

/**
 * @tc.number    : NFC_TAG_ISO15693_API_0016
 * @tc.name      : API normal test
 * @tc.desc      : WriteSingleBlock test ok.
 */
TEST_F(Iso15693TagTest, WriteSingleBlock_Ok)
{
    std::string id = data_valid_id;
    auto itsm = std::make_shared<ITagSessionMock>();
    std::shared_ptr<NfcMap> extra = std::make_shared<NfcMap>();
    std::shared_ptr<Tag> invalidtag_ = TagData::GetTag(
        id, data_valid_service_handle, Tag::EmTagTechnology::NFC_ISO_15693_TECH, data_techlist_all, itsm, extra);
    auto vt = Iso15693Tag::GetTag(invalidtag_);

    EXPECT_EQ(vt->Connect(), 0);

    EXPECT_TRUE(vt->IsConnect());

    vt->ReadSingleBlock(1, 1);

    std::unique_ptr<ResResult> res = std::make_unique<ResResult>();
    res->SetResult(0);
    EXPECT_CALL(*itsm, SendRawFrame(_, _, _)).WillRepeatedly(testing::Return(testing::ByMove(std::move(res))));
    EXPECT_EQ(vt->WriteSingleBlock(1, 1, "RANDOM"), 0);

    EXPECT_EQ(vt->Close(), 0);
}

/**
 * @tc.number    : NFC_TAG_ISO15693_API_0017
 * @tc.name      : API normal test
 * @tc.desc      : WriteMultipleBlock test ok.
 */
TEST_F(Iso15693TagTest, WriteMultipleBlock_Ok)
{
    std::string id = data_valid_id;
    auto itsm = std::make_shared<ITagSessionMock>();
    std::shared_ptr<NfcMap> extra = std::make_shared<NfcMap>();
    std::shared_ptr<Tag> invalidtag_ = TagData::GetTag(
        id, data_valid_service_handle, Tag::EmTagTechnology::NFC_ISO_15693_TECH, data_techlist_all, itsm, extra);
    auto vt = Iso15693Tag::GetTag(invalidtag_);

    EXPECT_EQ(vt->Connect(), 0);

    EXPECT_TRUE(vt->IsConnect());
    vt->ReadMultipleBlock(1, 1, 1);

    std::unique_ptr<ResResult> res = std::make_unique<ResResult>();
    res->SetResult(0);
    EXPECT_CALL(*itsm, SendRawFrame(_, _, _)).WillRepeatedly(testing::Return(testing::ByMove(std::move(res))));

    EXPECT_EQ(vt->WriteMultipleBlock(1, 1, 1, "RANDOM"), 0);

    EXPECT_EQ(vt->Close(), 0);
}

/**
 * @tc.number    : NFC_TAG_ISO15693_API_0018
 * @tc.name      : API normal test
 * @tc.desc      : LockSingleBlock test ok.
 */
TEST_F(Iso15693TagTest, LockSingleBlock_Ok)
{
    std::string id = data_valid_id;
    auto itsm = std::make_shared<ITagSessionMock>();
    std::shared_ptr<NfcMap> extra = std::make_shared<NfcMap>();
    std::shared_ptr<Tag> invalidtag_ = TagData::GetTag(
        id, data_valid_service_handle, Tag::EmTagTechnology::NFC_ISO_15693_TECH, data_techlist_all, itsm, extra);
    auto vt = Iso15693Tag::GetTag(invalidtag_);

    EXPECT_EQ(vt->Connect(), 0);

    EXPECT_TRUE(vt->IsConnect());

    std::unique_ptr<ResResult> res = std::make_unique<ResResult>();
    res->SetResult(0);
    EXPECT_CALL(*itsm, SendRawFrame(_, _, _)).WillRepeatedly(testing::Return(testing::ByMove(std::move(res))));

    EXPECT_EQ(vt->LockSingleBlock(1, 1), 0);
    EXPECT_EQ(vt->Close(), 0);
}