#include "mifare_classic_tag.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "infc_agent_service_mock.h"
#include "ohos_application.h"
#include "tag_data.h"

using namespace OHOS::nfc::sdk;
using namespace OHOS::nfc::reader;
using namespace testing;

class MifareClassicTagTest : public ::testing::Test {
public:
    virtual void SetUp() override
    {
        std::cout << " SetUpTEST_FCase MifareClassicTagTest." << std::endl;

        // invalid tag for exception test
        invalidtag_ = TagData::GetValidTag(Tag::EmTagTechnology::NFC_ISO_DEP_TECH);
        t_ = MifareClassicTag::GetTag(invalidtag_);

        tsm_ = std::make_shared<ITagSessionMock>();
    }

    virtual void TearDown() override
    {
        std::cout << " TearDownTestCase MifareClassicTagTest." << std::endl;
        invalidtag_ = nullptr;
        t_ = nullptr;
        tsm_ = nullptr;
    }

public:
    // tag data
    std::shared_ptr<Tag> invalidtag_;
    std::shared_ptr<MifareClassicTag> t_;
    std::shared_ptr<ITagSessionMock> tsm_;
};

/**
 * @tc.number    : NFC_TAG_MifareClassic_API_0001
 * @tc.name      : API Connect exception test
 * @tc.desc      : Api test
 */
TEST_F(MifareClassicTagTest, Connect_Failed)
{
    EXPECT_NE(t_->Connect(), 1);
}

/**
 * @tc.number    : NFC_TAG_MifareClassic_API_0002
 * @tc.name      : API AuthenticateSector exception test
 * @tc.desc      : Api test
 */
TEST_F(MifareClassicTagTest, AuthenticateSector_Failed)
{
    EXPECT_EQ(t_->AuthenticateSector(1, "RANDOM", false), false);

    t_->Connect();
    EXPECT_EQ(t_->AuthenticateSector(-1, "RANDOM", false), false);
    EXPECT_EQ(t_->AuthenticateSector(MifareClassicTag::MC_MAX_SECTOR_COUNT, "RANDOM", false), false);
    EXPECT_EQ(t_->AuthenticateSector(MifareClassicTag::MC_MAX_SECTOR_COUNT + 1, "RANDOM", false), false);

    EXPECT_EQ(t_->AuthenticateSector(-1, "", false), false);
}

/**
 * @tc.number    : NFC_TAG_MifareClassic_API_0003
 * @tc.name      : API ReadSingleBlock exception test
 * @tc.desc      : Api test
 */
TEST_F(MifareClassicTagTest, ReadSingleBlock_Failed)
{
    EXPECT_EQ(t_->ReadSingleBlock(1), "");

    t_->Connect();
    EXPECT_EQ(t_->ReadSingleBlock(-1), "");
    EXPECT_EQ(t_->ReadSingleBlock(MifareClassicTag::MC_MAX_BLOCK_INDEX), "");
    EXPECT_EQ(t_->ReadSingleBlock(MifareClassicTag::MC_MAX_BLOCK_INDEX + 1), "");
}

/**
 * @tc.number    : NFC_TAG_MifareClassic_API_0004
 * @tc.name      : API WriteSingleBlock exception test
 * @tc.desc      : Api test
 */
TEST_F(MifareClassicTagTest, WriteSingleBlock_Failed)
{
    EXPECT_NE(t_->WriteSingleBlock(1, "0123456789ABCDEF"), 0);

    t_->Connect();
    EXPECT_NE(t_->WriteSingleBlock(1, "RANDOM"), 0);

    EXPECT_NE(t_->WriteSingleBlock(-1, "0123456789ABCDEF"), 0);
    EXPECT_NE(t_->WriteSingleBlock(MifareClassicTag::MC_MAX_BLOCK_INDEX, "0123456789ABCDEF"), 0);
    EXPECT_NE(t_->WriteSingleBlock(MifareClassicTag::MC_MAX_BLOCK_INDEX + 1, "0123456789ABCDEF"), 0);
}

/**
 * @tc.number    : NFC_TAG_MifareClassic_API_0005
 * @tc.name      : API IncrementBlock exception test
 * @tc.desc      : Api test, only for applet block
 */
TEST_F(MifareClassicTagTest, IncrementBlock_Failed)
{
    EXPECT_NE(t_->IncrementBlock(1, 10), 0);

    t_->Connect();
    EXPECT_NE(t_->IncrementBlock(-1, 1), 0);
    EXPECT_NE(t_->IncrementBlock(MifareClassicTag::MC_MAX_BLOCK_INDEX, 1), 0);
    EXPECT_NE(t_->IncrementBlock(MifareClassicTag::MC_MAX_BLOCK_INDEX + 1, 1), 0);

    EXPECT_NE(t_->IncrementBlock(1, -1), 0);
}

/**
 * @tc.number    : NFC_TAG_MifareClassic_API_0006
 * @tc.name      : API DecrementBlock exception test
 * @tc.desc      : Api test, only for applet block
 */
TEST_F(MifareClassicTagTest, DecrementBlock_Failed)
{
    EXPECT_NE(t_->DecrementBlock(1, 10), 0);

    t_->Connect();
    EXPECT_NE(t_->DecrementBlock(-1, 1), 0);
    EXPECT_NE(t_->DecrementBlock(MifareClassicTag::MC_MAX_BLOCK_INDEX, 1), 0);
    EXPECT_NE(t_->DecrementBlock(MifareClassicTag::MC_MAX_BLOCK_INDEX + 1, 1), 0);

    EXPECT_NE(t_->DecrementBlock(1, -1), 0);
}

/**
 * @tc.number    : NFC_TAG_MifareClassic_API_0007
 * @tc.name      : API TransferToBlock exception test
 * @tc.desc      : Api test, only for applet block
 */
TEST_F(MifareClassicTagTest, TransferToBlock_Failed)
{
    EXPECT_NE(t_->TransferToBlock(1), 0);

    t_->Connect();
    EXPECT_NE(t_->TransferToBlock(-1), 0);
    EXPECT_NE(t_->TransferToBlock(MifareClassicTag::MC_MAX_BLOCK_INDEX), 0);
    EXPECT_NE(t_->TransferToBlock(MifareClassicTag::MC_MAX_BLOCK_INDEX + 1), 0);
}

/**
 * @tc.number    : NFC_TAG_MifareClassic_API_0008
 * @tc.name      : API RestoreFromBlock exception test
 * @tc.desc      : Api test, only for applet block
 */
TEST_F(MifareClassicTagTest, RestoreFromBlock_Failed)
{
    EXPECT_NE(t_->RestoreFromBlock(1), 0);

    t_->Connect();
    EXPECT_NE(t_->RestoreFromBlock(-1), 0);
    EXPECT_NE(t_->RestoreFromBlock(MifareClassicTag::MC_MAX_BLOCK_INDEX), 0);
    EXPECT_NE(t_->RestoreFromBlock(MifareClassicTag::MC_MAX_BLOCK_INDEX + 1), 0);
}

/**
 * @tc.number    : NFC_TAG_MifareClassic_API_0009
 * @tc.name      : API GetSectorCount test
 * @tc.desc      : Api test
 */
TEST_F(MifareClassicTagTest, GetSectorCount_Test)
{
    auto tag = TagData::GetValidMCTags(Tag::EmTagTechnology::NFC_MIFARE_CLASSIC_TECH, 0);
    auto t = MifareClassicTag::GetTag(tag);
    t_->GetSectorCount();
}

/**
 * @tc.number    : NFC_TAG_MifareClassic_API_0010
 * @tc.name      : API GetBlockCountInSector test
 * @tc.desc      : Api test
 */
TEST_F(MifareClassicTagTest, GetBlockCountInSector_Failed)
{
    EXPECT_NE(t_->GetBlockCountInSector(-1), 4);
    EXPECT_NE(t_->GetBlockCountInSector(MifareClassicTag::MC_MAX_SECTOR_COUNT), 16);
    EXPECT_NE(t_->GetBlockCountInSector(MifareClassicTag::MC_MAX_SECTOR_COUNT + 1), 16);
}

/**
 * @tc.number    : NFC_TAG_MifareClassic_API_0011
 * @tc.name      : API GetMifareTagType test
 * @tc.desc      : Api test
 */
TEST_F(MifareClassicTagTest, GetMifareTagType_Failed)
{
    t_->GetMifareTagType();
}

/**
 * @tc.number    : NFC_TAG_MifareClassic_API_0012
 * @tc.name      : API GetSize test
 * @tc.desc      : Api test
 */
TEST_F(MifareClassicTagTest, GetSize_Failed)
{
    auto tag = TagData::GetValidMCTags(Tag::EmTagTechnology::NFC_MIFARE_CLASSIC_TECH, 0);
    auto t = MifareClassicTag::GetTag(tag);
    t_->GetSize();
}

/**
 * @tc.number    : NFC_TAG_MifareClassic_API_0013
 * @tc.name      : API IsEmulated exception test
 * @tc.desc      : Api test
 */
TEST_F(MifareClassicTagTest, IsEmulated_Failed)
{
    EXPECT_EQ(t_->IsEmulated(), false);
}

/**
 * @tc.number    : NFC_TAG_MifareClassic_API_0014
 * @tc.name      : API GetBlockIdexFromSector exception test
 * @tc.desc      : Api test
 */
TEST_F(MifareClassicTagTest, GetBlockIdexFromSector_Failed)
{
    EXPECT_GE((unsigned int)t_->GetBlockIdexFromSector(-1), 0x80000100);
    EXPECT_GE((unsigned int)t_->GetBlockIdexFromSector(MifareClassicTag::MC_MAX_SECTOR_COUNT), 0x80000100);
    EXPECT_GE((unsigned int)t_->GetBlockIdexFromSector(MifareClassicTag::MC_MAX_SECTOR_COUNT + 1), 0x80000100);
}

/**
 * @tc.number    : NFC_TAG_MifareClassic_API_0015
 * @tc.name      : API MifareClassic tag normal  test
 * @tc.desc      : basic business test, include connect
 */
TEST_F(MifareClassicTagTest, Connect_Ok)
{
    auto tag = TagData::GetValidTag(Tag::EmTagTechnology::NFC_MIFARE_CLASSIC_TECH);
    auto vt = MifareClassicTag::GetTag(tag);
    EXPECT_EQ(vt->Connect(), 0);

    EXPECT_TRUE(vt->IsConnect());
    EXPECT_EQ(vt->Close(), 0);
}

/**
 * @tc.number    : NFC_TAG_MifareClassic_API_0016
 * @tc.name      : API GetSize normal test
 * @tc.desc      : Api test
 */
TEST_F(MifareClassicTagTest, GetSize_Ok)
{
    // mini size tag has 5 sectors, 4 blocks per sector
    int MC_SIZE_MINI = 320;
    int MC_SECTOR_COUNT_OF_SIZE_MINI = 5;
    // 1K size tag has 16 sectors, 4 blocks per sector
    int MC_SIZE_1K = 1024;
    int MC_SECTOR_COUNT_OF_SIZE_1K = 16;
    // 2K size tag has 32 sectors, 4 blocks per sector
    int MC_SIZE_2K = 2048;
    int MC_SECTOR_COUNT_OF_SIZE_2K = 32;
    // 4K size tag has 40 sectors, sector 0-32,4 blocks per sector, sector 33-40, 16 blocks per sector
    int MC_SIZE_4K = 4096;
    int MC_MAX_SECTOR_COUNT = 40;

    std::vector<int> sakList = {0x01, 0x08, 0x09, 0x10, 0x11, 0x18, 0x28, 0x38, 0x88, 0x98, 0xB8};
    for (auto i : sakList) {
        auto tag = TagData::GetValidMCTags(Tag::EmTagTechnology::NFC_MIFARE_CLASSIC_TECH, i);
        auto vt = MifareClassicTag::GetTag(tag);
        switch (i) {
            case 0x01:
            case 0x08:
                EXPECT_EQ(vt->GetSize(), MC_SIZE_1K);
                EXPECT_EQ(vt->GetMifareTagType(), (size_t)MifareClassicTag::EmMifareTagType::TYPE_CLASSIC);
                EXPECT_FALSE(vt->IsEmulated());
                break;
            case 0x09:
                EXPECT_EQ(vt->GetSize(), MC_SIZE_MINI);
                EXPECT_EQ(vt->GetMifareTagType(), (size_t)MifareClassicTag::EmMifareTagType::TYPE_CLASSIC);
                EXPECT_FALSE(vt->IsEmulated());
                break;
            case 0x10:
                EXPECT_EQ(vt->GetSize(), MC_SIZE_2K);
                EXPECT_EQ(vt->GetMifareTagType(), (size_t)MifareClassicTag::EmMifareTagType::TYPE_PLUS);
                EXPECT_FALSE(vt->IsEmulated());
                break;
            case 0x11:
                EXPECT_EQ(vt->GetSize(), MC_SIZE_4K);
                EXPECT_EQ(vt->GetMifareTagType(), (size_t)MifareClassicTag::EmMifareTagType::TYPE_PLUS);
                EXPECT_FALSE(vt->IsEmulated());
                break;
            case 0x18:
                EXPECT_EQ(vt->GetSize(), MC_SIZE_4K);
                EXPECT_EQ(vt->GetMifareTagType(), (size_t)MifareClassicTag::EmMifareTagType::TYPE_CLASSIC);
                EXPECT_FALSE(vt->IsEmulated());
                break;
            case 0x28:
                EXPECT_EQ(vt->GetSize(), MC_SIZE_1K);
                EXPECT_EQ(vt->GetMifareTagType(), (size_t)MifareClassicTag::EmMifareTagType::TYPE_CLASSIC);
                EXPECT_TRUE(vt->IsEmulated());
                break;
            case 0x38:
                EXPECT_EQ(vt->GetSize(), MC_SIZE_4K);
                EXPECT_EQ(vt->GetMifareTagType(), (size_t)MifareClassicTag::EmMifareTagType::TYPE_CLASSIC);
                EXPECT_TRUE(vt->IsEmulated());
                break;
            case 0x88:
                EXPECT_EQ(vt->GetSize(), MC_SIZE_1K);
                EXPECT_EQ(vt->GetMifareTagType(), (size_t)MifareClassicTag::EmMifareTagType::TYPE_CLASSIC);
                EXPECT_FALSE(vt->IsEmulated());
                break;
            case 0x98:
            case 0xB8:
                EXPECT_EQ(vt->GetSize(), MC_SIZE_4K);
                EXPECT_EQ(vt->GetMifareTagType(), (size_t)MifareClassicTag::EmMifareTagType::TYPE_PRO);
                EXPECT_FALSE(vt->IsEmulated());
                break;
            default:
                break;
        }

        switch (vt->GetSize()) {
            case MifareClassicTag::MC_SIZE_MINI:
                EXPECT_EQ(vt->GetSectorCount(), MC_SECTOR_COUNT_OF_SIZE_MINI);
                break;
            case MifareClassicTag::MC_SIZE_1K:
                EXPECT_EQ(vt->GetSectorCount(), MC_SECTOR_COUNT_OF_SIZE_1K);
                break;
            case MifareClassicTag::MC_SIZE_2K:
                EXPECT_EQ(vt->GetSectorCount(), MC_SECTOR_COUNT_OF_SIZE_2K);
                break;
            case MifareClassicTag::MC_SIZE_4K:
                EXPECT_EQ(vt->GetSectorCount(), MC_MAX_SECTOR_COUNT);
                break;
            default:
                break;
        }
    }
}

/**
 * @tc.number    : NFC_TAG_MifareClassic_API_0017
 * @tc.name      : API GetBlockCountInSector normal test
 * @tc.desc      : Api test
 */
TEST_F(MifareClassicTagTest, GetBlockCountInSector_Index__Ok)
{
    int MC_BLOCK_COUNT = 4;
    int MC_BLOCK_COUNT_OF_4K = 16;
    // sector index boundary test
    EXPECT_EQ(t_->GetBlockCountInSector(0), MC_BLOCK_COUNT);
    EXPECT_EQ(t_->GetBlockIdexFromSector(0), 0);

    EXPECT_EQ(t_->GetBlockCountInSector(31), MC_BLOCK_COUNT);
    EXPECT_NE(t_->GetBlockIdexFromSector(31), 0);

    EXPECT_EQ(t_->GetBlockCountInSector(32), MC_BLOCK_COUNT_OF_4K);
    EXPECT_NE(t_->GetBlockIdexFromSector(32), 0);

    EXPECT_EQ(t_->GetBlockCountInSector(39), MC_BLOCK_COUNT_OF_4K);
    EXPECT_NE(t_->GetBlockIdexFromSector(39), 0);
}

/**
 * @tc.number    : NFC_TAG_MifareClassic_API_0018
 * @tc.name      : API MifareClassic tag normal  test
 * @tc.desc      : AuthenticateSector with keyA ok
 */
TEST_F(MifareClassicTagTest, AuthenticateSector_A_Ok)
{
    std::string id = data_valid_id;
    auto itsm = std::make_shared<ITagSessionMock>();
    std::shared_ptr<NfcMap> extra = std::make_shared<NfcMap>();
    std::shared_ptr<Tag> invalidtag_ = TagData::GetTag(
        id, data_valid_service_handle, Tag::EmTagTechnology::NFC_ISO_15693_TECH, data_techlist_all, itsm, extra);
    auto vt = MifareClassicTag::GetTag(invalidtag_);
    EXPECT_EQ(vt->Connect(), 0);

    EXPECT_TRUE(vt->IsConnect());

    std::unique_ptr<ResResult> res = std::make_unique<ResResult>();
    res->SetResult(0);
    EXPECT_CALL(*itsm, SendRawFrame(_, _, _)).WillRepeatedly(testing::Return(testing::ByMove(std::move(res))));
    EXPECT_TRUE(vt->AuthenticateSector(1, "FFFFFF", true));

    EXPECT_EQ(vt->Close(), 0);
}

/**
 * @tc.number    : NFC_TAG_MifareClassic_API_0019
 * @tc.name      : API MifareClassic tag normal  test
 * @tc.desc      : AuthenticateSector with keyB ok
 */
TEST_F(MifareClassicTagTest, AuthenticateSector_B_Ok)
{
    std::string id = data_valid_id;
    auto itsm = std::make_shared<ITagSessionMock>();
    std::shared_ptr<NfcMap> extra = std::make_shared<NfcMap>();
    std::shared_ptr<Tag> invalidtag_ = TagData::GetTag(
        id, data_valid_service_handle, Tag::EmTagTechnology::NFC_ISO_15693_TECH, data_techlist_all, itsm, extra);
    auto vt = MifareClassicTag::GetTag(invalidtag_);
    EXPECT_EQ(vt->Connect(), 0);

    EXPECT_TRUE(vt->IsConnect());

    std::unique_ptr<ResResult> res = std::make_unique<ResResult>();
    res->SetResult(0);
    EXPECT_CALL(*itsm, SendRawFrame(_, _, _)).WillRepeatedly(testing::Return(testing::ByMove(std::move(res))));
    EXPECT_TRUE(vt->AuthenticateSector(1, "FFFFFF", false));
    EXPECT_EQ(vt->Close(), 0);
}

/**
 * @tc.number    : NFC_TAG_MifareClassic_API_0020
 * @tc.name      : API MifareClassic tag normal  test
 * @tc.desc      : WriteSingleBlock ok
 */
TEST_F(MifareClassicTagTest, WriteSingleBlock_Ok)
{
    std::string id = data_valid_id;
    auto itsm = std::make_shared<ITagSessionMock>();
    std::shared_ptr<NfcMap> extra = std::make_shared<NfcMap>();
    std::shared_ptr<Tag> invalidtag_ = TagData::GetTag(
        id, data_valid_service_handle, Tag::EmTagTechnology::NFC_ISO_15693_TECH, data_techlist_all, itsm, extra);
    auto vt = MifareClassicTag::GetTag(invalidtag_);
    EXPECT_EQ(vt->Connect(), 0);

    EXPECT_TRUE(vt->IsConnect());
    EXPECT_EQ(vt->ReadSingleBlock(1), "");

    std::unique_ptr<ResResult> res = std::make_unique<ResResult>();
    res->SetResult(0);
    EXPECT_CALL(*itsm, SendRawFrame(_, _, _)).WillRepeatedly(testing::Return(testing::ByMove(std::move(res))));

    EXPECT_EQ(vt->WriteSingleBlock(1, "0123456789ABCDEF"), 0);
    EXPECT_EQ(vt->Close(), 0);
}

/**
 * @tc.number    : NFC_TAG_MifareClassic_API_0021
 * @tc.name      : API MifareClassic tag normal  test
 * @tc.desc      : IncrementBlock ok
 */
TEST_F(MifareClassicTagTest, IncrementBlock_Ok)
{
    std::string id = data_valid_id;
    auto itsm = std::make_shared<ITagSessionMock>();
    std::shared_ptr<NfcMap> extra = std::make_shared<NfcMap>();
    std::shared_ptr<Tag> invalidtag_ = TagData::GetTag(
        id, data_valid_service_handle, Tag::EmTagTechnology::NFC_ISO_15693_TECH, data_techlist_all, itsm, extra);
    auto vt = MifareClassicTag::GetTag(invalidtag_);
    EXPECT_EQ(vt->Connect(), 0);

    EXPECT_TRUE(vt->IsConnect());

    std::unique_ptr<ResResult> res = std::make_unique<ResResult>();
    res->SetResult(0);
    EXPECT_CALL(*itsm, SendRawFrame(_, _, _)).WillRepeatedly(testing::Return(testing::ByMove(std::move(res))));
    EXPECT_EQ(vt->IncrementBlock(1, 20), 0);
    EXPECT_EQ(vt->Close(), 0);
}

/**
 * @tc.number    : NFC_TAG_MifareClassic_API_0022
 * @tc.name      : API MifareClassic tag normal  test
 * @tc.desc      : DecrementBlock ok
 */
TEST_F(MifareClassicTagTest, DecrementBlock_Ok)
{
    std::string id = data_valid_id;
    auto itsm = std::make_shared<ITagSessionMock>();
    std::shared_ptr<NfcMap> extra = std::make_shared<NfcMap>();
    std::shared_ptr<Tag> invalidtag_ = TagData::GetTag(
        id, data_valid_service_handle, Tag::EmTagTechnology::NFC_ISO_15693_TECH, data_techlist_all, itsm, extra);
    auto vt = MifareClassicTag::GetTag(invalidtag_);
    EXPECT_EQ(vt->Connect(), 0);

    EXPECT_TRUE(vt->IsConnect());

    std::unique_ptr<ResResult> res = std::make_unique<ResResult>();
    res->SetResult(0);
    EXPECT_CALL(*itsm, SendRawFrame(_, _, _)).WillRepeatedly(testing::Return(testing::ByMove(std::move(res))));
    EXPECT_EQ(vt->DecrementBlock(1, 20), 0);
    EXPECT_EQ(vt->Close(), 0);
}

/**
 * @tc.number    : NFC_TAG_MifareClassic_API_0023
 * @tc.name      : API MifareClassic tag normal  test
 * @tc.desc      : TransferToBlock ok
 */
TEST_F(MifareClassicTagTest, TransferToBlock_Ok)
{
    std::string id = data_valid_id;
    auto itsm = std::make_shared<ITagSessionMock>();
    std::shared_ptr<NfcMap> extra = std::make_shared<NfcMap>();
    std::shared_ptr<Tag> invalidtag_ = TagData::GetTag(
        id, data_valid_service_handle, Tag::EmTagTechnology::NFC_ISO_15693_TECH, data_techlist_all, itsm, extra);
    auto vt = MifareClassicTag::GetTag(invalidtag_);
    EXPECT_EQ(vt->Connect(), 0);

    EXPECT_TRUE(vt->IsConnect());

    std::unique_ptr<ResResult> res = std::make_unique<ResResult>();
    res->SetResult(0);
    EXPECT_CALL(*itsm, SendRawFrame(_, _, _)).WillRepeatedly(testing::Return(testing::ByMove(std::move(res))));
    EXPECT_EQ(vt->TransferToBlock(1), 0);
    EXPECT_EQ(vt->Close(), 0);
}

/**
 * @tc.number    : NFC_TAG_MifareClassic_API_0024
 * @tc.name      : API MifareClassic tag normal  test
 * @tc.desc      : RestoreFromBlock ok
 */
TEST_F(MifareClassicTagTest, RestoreFromBlock_Ok)
{
    std::string id = data_valid_id;
    auto itsm = std::make_shared<ITagSessionMock>();
    std::shared_ptr<NfcMap> extra = std::make_shared<NfcMap>();
    std::shared_ptr<Tag> invalidtag_ = TagData::GetTag(
        id, data_valid_service_handle, Tag::EmTagTechnology::NFC_ISO_15693_TECH, data_techlist_all, itsm, extra);
    auto vt = MifareClassicTag::GetTag(invalidtag_);
    EXPECT_EQ(vt->Connect(), 0);

    EXPECT_TRUE(vt->IsConnect());

    std::unique_ptr<ResResult> res = std::make_unique<ResResult>();
    res->SetResult(0);
    EXPECT_CALL(*itsm, SendRawFrame(_, _, _)).WillRepeatedly(testing::Return(testing::ByMove(std::move(res))));
    EXPECT_EQ(vt->RestoreFromBlock(1), 0);
    EXPECT_EQ(vt->Close(), 0);
}