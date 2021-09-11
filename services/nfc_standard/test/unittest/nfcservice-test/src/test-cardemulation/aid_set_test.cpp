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

#include "aid_set.h"

#include <gtest/gtest.h>

#include <algorithm>
#include <functional>
#include <iostream>
#include <map>
#include <set>

#include "aid_string.h"
#include "card_emulation_def.h"
#include "card_emulation_service_info.h"
#include "card_emulation_util.h"
#include "element_name.h"
#include "test_util.h"

using namespace OHOS::nfc::cardemulation::test;
using namespace OHOS::nfc::cardemulation;
namespace OHOS::nfc::cardemulation::test{
/**
* @tc.number:
* @tc.name  :
* @tc.desc  :
*/
TEST(AidSet, From_Strings)
{
    std::vector<std::string> aidStrs{"F00102030401", "F00102030402", "F00102030403", "F00102030404", "F00102030405"};
    auto aidset = AidSet::From(aidStrs);
    EXPECT_EQ(aidset.Size(), aidStrs.size());

    std::for_each(aidStrs.cbegin(), aidStrs.cend(), [&aidset](std::string const& s) {
        EXPECT_TRUE(aidset.HasAidString(AidString(s)));
    });

    std::vector<std::string> invalidAidStrs{"",
                                            "F0",
                                            "F0010203",
                                            "#F00102030404",
                                            "F001020304050",
                                            "F0000102030405060708090A0B0C0D0E0F"};
    auto aidset1 = AidSet::From(invalidAidStrs);
    EXPECT_EQ(aidset1.Size(), 0u);
}

/**
* @tc.number:
* @tc.name  :
* @tc.desc  :
*/
TEST(AidSet, From_AidStrings)
{
    auto buildTestDatas = [](std::vector<std::string> const& strs) -> std::vector<AidString> {
        std::vector<AidString> aidStrs;
        std::for_each(strs.cbegin(), strs.cend(), [&aidStrs](std::string s) {
            aidStrs.emplace_back(AidString(std::move(s)));
        });
        return aidStrs;
    };
    std::vector<std::string> strs{"F00102030401",
                                  "F00102030402",
                                  "F00102030403",
                                  "F00102030404",
                                  "F00102030405",
                                  "F00102030405"};

    std::vector<AidString> aidStrs = buildTestDatas(strs);

    auto aidset = AidSet::From(aidStrs);
    EXPECT_EQ(aidset.Size(), aidStrs.size() - 1);

    std::vector<std::string> strs_c1{"F00102030401",
                                     "F00102030402",
                                     "F001020304*",
                                     "F00102030403",
                                     "F00102030404",
                                     "F00102030405"};

    std::vector<AidString> aidStrs_c1 = buildTestDatas(strs_c1);
    auto aidset_c1 = AidSet::From(aidStrs_c1);
    EXPECT_EQ(aidset_c1.Size(), aidStrs_c1.size() - 1);

    std::vector<std::string> invalidAidStrs{"",
                                            "F0",
                                            "F0010203",
                                            "#F00102030404",
                                            "F001020304050",
                                            "F0000102030405060708090A0B0C0D0E0F"};
    std::vector<AidString> aidStrs1 = buildTestDatas(invalidAidStrs);
    auto aidset1 = AidSet::From(aidStrs1);
    EXPECT_EQ(aidset1.Size(), 0u);
}
/**
* @tc.number:
* @tc.name  :
* @tc.desc  :
*/
TEST(AidSet, AddAidStrings)
{
    AidSet aidSet;
    EXPECT_FALSE(aidSet.AddAidString(AidString("")));
    EXPECT_TRUE(aidSet.AddAidString(AidString("F00102030401")));
    EXPECT_FALSE(aidSet.AddAidString(AidString("F00102030401")));
    EXPECT_FALSE(aidSet.AddAidString(AidString("f00102030401")));
    EXPECT_FALSE(aidSet.AddAidString(AidString("f00102030401*")));
    EXPECT_FALSE(aidSet.AddAidString(AidString("f00102030401#")));

    auto all = aidSet.GetAll();
    EXPECT_EQ(all.size(), 1u);
    auto allRawString = aidSet.GetAllAidRawString();
    EXPECT_EQ(allRawString.size(), 1u);
    EXPECT_STRCASEEQ(allRawString[0].c_str(), "F00102030401");
    auto allRawStringWithSpace = aidSet.GetAllAidRawString(true);
    EXPECT_EQ(allRawStringWithSpace.size(), 1u);
    EXPECT_STRCASEEQ(allRawStringWithSpace[0].c_str(), "F0 01 02 03 04 01");
}

/**
* @tc.number:
* @tc.name  :
* @tc.desc  :
*/
TEST(AidSet, conflict_example)
{
    auto last = [](std::string const& x) { return x.cbegin() + (x.size() - 1); };
    auto cmp = [last](std::string const& a, std::string const& b) -> bool {
        if (a.empty() || (a.size() > b.size())) {
            return false;
        }
        return !std::equal(a.cbegin(), last(a), b.cbegin());
    };
    EXPECT_EQ(*(last("12345") - 1), '4');

    std::set<std::string, decltype(cmp)> aidset(cmp);

    EXPECT_TRUE(aidset.insert("111").second);
    EXPECT_FALSE(aidset.insert("111").second);
    EXPECT_FALSE(aidset.insert("11").second);
    EXPECT_FALSE(aidset.insert("1111").second);
    EXPECT_EQ(aidset.size(), 1u);
}
/**
* @tc.number:
* @tc.name  :
* @tc.desc  :
*/
TEST(AidSet, ConflictWith)
{
    AidString aid(AidString("F0102030405060"));

    AidSet aidset1;
    aidset1.AddAidString(aid);
    AidSet aidset2;
    aidset2.AddAidString(aid);

    EXPECT_TRUE(aidset1.ConflictWith(aidset1, false));
    EXPECT_TRUE(aidset1.ConflictWith(aidset2, false));
    EXPECT_TRUE(aidset1.ConflictWith(aidset2, true));
    aidset2.RemoveAidString(aid);
    EXPECT_FALSE(aidset1.ConflictWith(aidset2, false));
    EXPECT_TRUE(aidset1.ConflictWith(aidset2, true));

    AidSet aidset3("type");
    AidSet aidset4("type");
    EXPECT_TRUE(aidset3.ConflictWith(aidset4, true));
}

/**
* @tc.number:
* @tc.name  :
* @tc.desc  :
*/
TEST(AidSet, eq)
{
    {
        std::vector<AidString> aids{};
        auto aidset = AidSet::FromAidString(aids);
        ASSERT_TRUE(aidset);
        EXPECT_TRUE(aidset->GetExecutionEnvironment().empty());
        AidSet cp(*aidset);
        EXPECT_EQ(cp, cp);
        EXPECT_EQ(cp, *aidset);
        EXPECT_TRUE(cp == *aidset);
        EXPECT_TRUE(cp.GetAll().empty());
        EXPECT_TRUE(cp.GetAllAidRawString().empty());
    }
    {
        AidSet as1;
        AidSet as2("as2");
        EXPECT_FALSE(as2.Equals(as1));
    }
}
/**
* @tc.number:
* @tc.name  :
* @tc.desc  :
*/
TEST(AidSet, constructor)
{
    AidSet as1("type");
    EXPECT_STREQ(as1.GetType().c_str(), "type");
    EXPECT_FALSE(as1.GetOwnerName());

    auto aids = as1.GetAll();
    EXPECT_TRUE(aids.empty());
    as1.RemoveAidString(AidString(""));

    AidSet as2 = as1;
    EXPECT_STREQ(as2.GetType().c_str(), "type");

    AidSet as3 = std::move(as1);
    EXPECT_STREQ(as3.GetType().c_str(), "type");
    AidSet as4(std::move(as3));
    EXPECT_STREQ(as4.GetType().c_str(), "type");

    AidSet as5;
    as5.swap(as4);
    EXPECT_STREQ(as5.GetType().c_str(), "type");
    EXPECT_TRUE(as4.GetType().empty());

    AidSet as6;
    std::swap(as5, as6);
    EXPECT_STREQ(as6.GetType().c_str(), "type");
    EXPECT_TRUE(as5.GetType().empty());

    AidSet as7;
    as7 = as6;
    EXPECT_STREQ(as6.GetType().c_str(), "type");
    EXPECT_STREQ(as7.GetType().c_str(), "type");

    std::shared_ptr<CardEmulationServiceInfo> appInfo = std::make_shared<CardEmulationServiceInfo>(NFC_EE_HOST);
    Util::CardEmulationServiceInfoSetName(appInfo, "app");

    as7.SetOwner(appInfo);
    ASSERT_TRUE(as7.GetOwnerName());
    EXPECT_STREQ(as7.GetOwnerName()->GetAbilityName().c_str(), "app");
}

/**
* @tc.number:
* @tc.name  :
* @tc.desc  :
*/
TEST(AidSet, HasAidString)
{
    AidSet as1("type");
    AidString ndefAid0("D2760000850100");
    AidString ndefAid1("D2760000850101");
    as1.AddAidString(ndefAid0);
    EXPECT_TRUE(as1.HasAidString(ndefAid0));
    EXPECT_FALSE(as1.HasAidString(ndefAid1));
    as1.AddAidString(ndefAid1);
    EXPECT_TRUE(as1.HasAidString(ndefAid0));
    EXPECT_TRUE(as1.HasAidString(ndefAid1));
}

/**
* @tc.number:
* @tc.name  :
* @tc.desc  :
*/
TEST(AidSet, Owner)
{
    AidSet as1("type");
    EXPECT_TRUE(as1.GetOwner().expired());
}
/**
* @tc.number:
* @tc.name  :
* @tc.desc  :
*/
TEST(AidSet, FromJson)
{
    nlohmann::json j =
        R"({"description": "desc1", "type": "normal", "aids":["D2760000850100", "D2760000850101"]})"_json;

    AidString ndefAid0("D2760000850100");
    AidString ndefAid1("D2760000850101");

    AidSet as0;
    from_json(j, as0);
    AidSet as1;
    as1.FromJson(j);
    EXPECT_TRUE(as1.HasAidString(ndefAid0));
    EXPECT_TRUE(as1.HasAidString(ndefAid1));
    EXPECT_STREQ(as1.GetType().c_str(), "normal");
    std::shared_ptr<AidSet> as2;
    from_json(j, as2);
    EXPECT_TRUE(as2->HasAidString(ndefAid0));
    EXPECT_TRUE(as2->HasAidString(ndefAid1));
    EXPECT_STREQ(as2->GetType().c_str(), "normal");
}
/**
* @tc.number:
* @tc.name  :
* @tc.desc  :
*/
TEST(AidSet, ToJson)
{
    AidSet as1("type");
    AidString ndefAid0("D2760000850100");
    AidString ndefAid1("D2760000850101");
    as1.AddAidString(ndefAid0);
    as1.AddAidString(ndefAid1);

    {
        nlohmann::json j;
        as1.ToJson(j);
        EXPECT_TRUE(j["type"].get<std::string>() == "type");
        auto aids = j["aids"].get<std::vector<std::string>>();
        EXPECT_EQ(aids.size(), 2u);
        EXPECT_STREQ(aids[0].c_str(), "D2760000850100");
        EXPECT_STREQ(aids[1].c_str(), "D2760000850101");
    }

    {
        nlohmann::json j;
        auto as2 = std::make_shared<AidSet>("type");
        as2->AddAidString(ndefAid0);
        as2->AddAidString(ndefAid1);
        as2->ToJson(j);
        EXPECT_TRUE(j["type"].get<std::string>() == "type");
        auto aids = j["aids"].get<std::vector<std::string>>();
        EXPECT_EQ(aids.size(), 2u);
        EXPECT_STREQ(aids[0].c_str(), "D2760000850100");
        EXPECT_STREQ(aids[1].c_str(), "D2760000850101");
    }
}
}