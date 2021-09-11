#include "isodep_tag.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "tag_data.h"

using namespace OHOS::nfc::sdk;
using namespace OHOS::nfc::reader;
using namespace testing;

class IsoDepTagTest : public ::testing::Test {
public:
    virtual void SetUp() override
    {
        std::cout << " SetUpTestCase IsoDepTagTest." << std::endl;
        tag = TagData::GetValidTag(Tag::EmTagTechnology::NFC_ISO_DEP_TECH);
        t = IsoDepTag::GetTag(tag);
    }

    virtual void TearDown() override
    {
        std::cout << " TearDownTestCase IsoDepTagTest." << std::endl;
        tag = nullptr;
        t = nullptr;
    }

public:
    std::shared_ptr<Tag> tag;
    std::shared_ptr<IsoDepTag> t;
};

/**
 * @tc.number    : NFC_TAG_IsoDep_API_0001
 * @tc.name      : API Connect exception test
 * @tc.desc      : Api test
 */
TEST_F(IsoDepTagTest, Connect_Failed)
{
    EXPECT_EQ(t->Connect(), 0);
}

/**
 * @tc.number    : NFC_TAG_IsoDep_API_0002
 * @tc.name      : API GetHistoricalBytes test
 * @tc.desc      : Api test
 */
TEST_F(IsoDepTagTest, GetHistoricalBytes_Test)
{
    t->GetHistoricalBytes();
}

/**
 * @tc.number    : NFC_TAG_IsoDep_API_0003
 * @tc.name      : API GetHiLayerResponse test
 * @tc.desc      : Api test
 */
TEST_F(IsoDepTagTest, GetHiLayerResponse_Test)
{
    t->GetHiLayerResponse();
}

/**
 * @tc.number    : NFC_TAG_IsoDep_API_0004
 * @tc.name      : API GetSak test
 * @tc.desc      : Api test
 */
TEST_F(IsoDepTagTest, GetSak_Test)
{
    t->GetSak();
}

/**
 * @tc.number    : NFC_TAG_IsoDep_API_0005
 * @tc.name      : API GetAtqa test
 * @tc.desc      : Api test
 */
TEST_F(IsoDepTagTest, GetAtqa_Test)
{
    t->GetAtqa();
}

/**
 * @tc.number    : NFC_TAG_IsoDep_API_0006
 * @tc.name      : API GetAppData test
 * @tc.desc      : Api test
 */
TEST_F(IsoDepTagTest, GetAppData_Test)
{
    t->GetAppData();
}

/**
 * @tc.number    : NFC_TAG_IsoDep_API_0007
 * @tc.name      : API GetProtocolInfo test
 * @tc.desc      : Api test
 */
TEST_F(IsoDepTagTest, GetProtocolInfo_Test)
{
    t->GetProtocolInfo();
}