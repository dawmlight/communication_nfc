#include "tag.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "tag_data.h"

using namespace OHOS::nfc::sdk;
using namespace OHOS::nfc::reader;

TEST(Tag, IsSupportTech_Failed)
{
    auto t = TagData::GetValidTag();
    EXPECT_EQ(t->IsSupportTech(0), false);
}

TEST(Tag, GetTagId_Failed)
{
    auto t = TagData::GetInvalidTag(Tag::NFC_NDEF_TECH, EMTagParamInvalid::INVALID_ID);
    std::string id = t->GetTagId();
    EXPECT_STREQ(id.c_str(), "");
}

TEST(Tag, GetTagTechnologyList_Failed)
{
    auto t = TagData::GetInvalidTag(Tag::NFC_NDEF_TECH, EMTagParamInvalid::INVALID_TECH_LIST);
    EXPECT_EQ(t->GetTagTechnologyList(), data_techlist_0);
}

TEST(Tag, GetConnectTagTech_Failed)
{
    auto t = TagData::GetValidTag();
    EXPECT_EQ(t->GetConnectTagTech(), 1);
}

TEST(Tag, GetServiceHandle_Failed)
{
    auto t = TagData::GetValidTag();
    EXPECT_EQ(t->GetServiceHandle(), 1);
}

TEST(Tag, GetTagExternData_Failed)
{
    auto t = TagData::GetInvalidTag(Tag::NFC_NDEF_TECH, EMTagParamInvalid::INVALID_EXTRA_DATA);
    auto extra = t->GetTagExternData();
    EXPECT_FALSE(extra.lock());
}

TEST(Tag, GetStringExternData_Failed)
{
    auto t = TagData::GetValidTag();
    std::string en = t->GetStringExternData(data_invalid_extra, "RANDOM");
    EXPECT_STRNE(en.c_str(), "Wrong");
}

TEST(Tag, GetIntExternData_Failed)
{
    auto t = TagData::GetValidTag();
    EXPECT_NE(t->GetIntExternData(data_invalid_extra, "RANDOM"), 1);
}

TEST(Tag, GetTechExtras_Failed)
{
    auto t = TagData::GetValidTag();
    auto extra = t->GetTechExtras(data_invalid_connect_tech);
    EXPECT_FALSE(extra.lock());
}

TEST(Tag, SetConnectTagTech_Failed)
{
    auto t = TagData::GetValidTag();
    t->SetConnectTagTech(data_invalid_connect_tech);
}