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

#include "aid_string.h"

#include <gtest/gtest.h>

#include <algorithm>
#include <vector>

#include "card_emulation_error.h"
#include "card_emulation_util.h"

using namespace OHOS::nfc::cardemulation;
namespace OHOS::nfc::cardemulation::test{
/**
 * @tc.number:
 * @tc.name  :
 * @tc.desc  :
 */
TEST(AidString, IsValidAidChar)
{
    std::string validAidChars = "0123456789abcdefABCDEF#*";
    std::for_each(validAidChars.begin(), validAidChars.end(), [](char ch) {
        EXPECT_TRUE(AidString::IsValidAidChar(ch));
    });
}

/**
 * @tc.number:
 * @tc.name  :
 * @tc.desc  :
 */
TEST(AidString, From_String_OK)
{
    AidString aidString;

    EXPECT_EQ(aidString.From("A001020304"), ERR_OK);
    EXPECT_STRCASEEQ(aidString.ToString().c_str(), "A001020304");
    AidString aidstr(aidString);
    EXPECT_STRCASEEQ(aidstr.ToString().c_str(), "A001020304");
    AidString movedAidstr(std::move(aidstr));
    EXPECT_STRCASEEQ(movedAidstr.ToString().c_str(), "A001020304");
    EXPECT_FALSE(aidstr.IsValid());
    EXPECT_EQ(aidstr.From(std::vector<unsigned char>()), ERR_AID_ILLIGLE_LENGTH);
    EXPECT_EQ(aidstr.From(std::vector<unsigned char>{0xa0, 0x01, 0x02, 0x03, 0x04, 0x05}), ERR_OK);
    EXPECT_EQ(aidString.From("a001020304"), ERR_OK);
    EXPECT_STRCASEEQ(aidString.ToString().c_str(), "A001020304");
    EXPECT_STRCASEEQ(aidString.ToString(false, true).c_str(), "A0 01 02 03 04");
    EXPECT_EQ(aidString.From("0123456789abcdef0123456789abcdef"), ERR_OK);
    EXPECT_STRCASEEQ(aidString.ToString().c_str(), "0123456789ABCDEF0123456789ABCDEF");

    // subset
    EXPECT_EQ(aidString.From("A001020304#"), ERR_OK);
    EXPECT_STRCASEEQ(aidString.ToString().c_str(), "A001020304#");

    EXPECT_EQ(aidString.From("0123456789abcdef0123456789abcd#"), ERR_OK);
    EXPECT_STRCASEEQ(aidString.ToString().c_str(), "0123456789ABCDEF0123456789ABCD#");

    // prefix
    EXPECT_EQ(aidString.From("A001020304*"), ERR_OK);
    EXPECT_STRCASEEQ(aidString.ToString().c_str(), "A001020304*");
    EXPECT_STRCASEEQ(aidString.ToString(false, true).c_str(), "A0 01 02 03 04 *");

    EXPECT_EQ(aidString.From("0123456789abcdef0123456789abcd*"), ERR_OK);
    EXPECT_STRCASEEQ(aidString.ToString().c_str(), "0123456789ABCDEF0123456789ABCD*");
}

/**
 * @tc.number:
 * @tc.name  :
 * @tc.desc  :
 */
TEST(AidString, From_String_NotOK)
{
    AidString aidString;
    EXPECT_EQ(aidString.From(""), ERR_AID_ILLIGLE_LENGTH);
    EXPECT_EQ(aidString.From("A0010203"), ERR_AID_ILLIGLE_LENGTH);

    EXPECT_EQ(aidString.From("0123456789abcdef0123456789abcdef01"), ERR_AID_ILLIGLE_LENGTH);

    EXPECT_EQ(aidString.From("A001020304050"), ERR_AID_INVALID);
    EXPECT_EQ(aidString.From("A001020304050*"), ERR_AID_INVALID);
    EXPECT_EQ(aidString.From("A001020304050#"), ERR_AID_INVALID);

    EXPECT_EQ(aidString.From("A001020304-+"), ERR_AID_INVALID);

    EXPECT_EQ(aidString.From("A001020304##"), ERR_AID_INVALID);
    EXPECT_EQ(aidString.From("A001020304*#"), ERR_AID_INVALID);

    EXPECT_EQ(aidString.From("#A001020304"), ERR_AID_INVALID);
    EXPECT_EQ(aidString.From("*A001020304"), ERR_AID_INVALID);
    EXPECT_EQ(aidString.From("A0#01020304"), ERR_AID_INVALID);
    EXPECT_EQ(aidString.From("A0*01020304"), ERR_AID_INVALID);
}

/**
 * @tc.number:
 * @tc.name  :
 * @tc.desc  :
 */
TEST(AidString, ToString)
{
    AidString aidString;
    EXPECT_STRCASEEQ(aidString.ToString().c_str(), "");
    EXPECT_STRCASEEQ(aidString.ToString(false).c_str(), "");

    EXPECT_EQ(aidString.From("0123456789abcdef#"), ERR_OK);
    EXPECT_STRCASEEQ(aidString.ToString().c_str(), "0123456789ABCDEF#");
    EXPECT_STRCASEEQ(aidString.ToString(false).c_str(), "0123456789abcdef#");
}

/**
 * @tc.number:
 * @tc.name  :
 * @tc.desc  :
 */
TEST(AidString, ToBytes)
{
    auto test = [](std::string const& aid, std::vector<unsigned char> const& exp_bytes) {
        AidString aidString(aid);
        std::vector<unsigned char> act_bytes;

        aidString.ToBytes(act_bytes);
        return std::equal(act_bytes.begin(), act_bytes.end(), exp_bytes.begin());
    };

    EXPECT_TRUE(test("", {}));
    EXPECT_TRUE(test("0123456789abcdef",
                     {
                         0x1,
                         0x23,
                         0x45,
                         0x67,
                         0x89,
                         0xab,
                         0xcd,
                         0xef,
                     }));
    EXPECT_TRUE(test("0123456789abcdef*",
                     {
                         0x1,
                         0x23,
                         0x45,
                         0x67,
                         0x89,
                         0xab,
                         0xcd,
                         0xef,
                     }));
    EXPECT_TRUE(test("0123456789abcdef#",
                     {
                         0x1,
                         0x23,
                         0x45,
                         0x67,
                         0x89,
                         0xab,
                         0xcd,
                         0xef,
                     }));
}

/**
 * @tc.number:
 * @tc.name  :
 * @tc.desc  :
 */
TEST(AidString, IsValid)
{
    AidString aidString;
    EXPECT_FALSE(aidString.IsValid());
    aidString.From("0123456789abcdef#");
    EXPECT_TRUE(aidString.IsValid());

    aidString.From("0123456789abcdef");
    EXPECT_TRUE(aidString.IsValid());

    aidString.From("012");
    EXPECT_FALSE(aidString.IsValid());
}

/**
 * @tc.number:
 * @tc.name  :
 * @tc.desc  :
 */
TEST(AidString, swap)
{
    AidString aidString;
    EXPECT_TRUE(aidString.ExactlyEqualTo(AidString()));
    aidString.From("0123456789abcdef#");

    AidString aidStr1;
    AidString aidStr2;

    aidStr1.swap(aidStr2);
    EXPECT_TRUE(aidStr1.ExactlyEqualTo(AidString()));
    EXPECT_TRUE(aidStr2.ExactlyEqualTo(AidString()));

    aidStr2.From("0123456789ABCDEF#");
    aidStr1.swap(aidStr2);
    EXPECT_TRUE(aidStr2.ExactlyEqualTo(AidString()));
    EXPECT_TRUE(aidString.ExactlyEqualTo(aidStr1));

    AidString aidStr3;
    AidString aidStr4;
    aidStr4.From("0123456789ABCDEF#");
    aidStr4.swap(aidStr3);
    EXPECT_TRUE(aidStr4.ExactlyEqualTo(AidString()));
    EXPECT_TRUE(aidString.ExactlyEqualTo(aidStr3));
}

/**
 * @tc.number:
 * @tc.name  :
 * @tc.desc  :
 */
TEST(AidString, IsXXX)
{
    AidString aidStr1;
    EXPECT_FALSE(aidStr1.IsExact());
    EXPECT_FALSE(aidStr1.IsPrefix());
    EXPECT_FALSE(aidStr1.IsSubset());

    AidString aidStr2;
    aidStr2.From("123456789ABCDEF0");
    EXPECT_TRUE(aidStr2.IsExact());
    EXPECT_FALSE(aidStr2.IsPrefix());
    EXPECT_FALSE(aidStr2.IsSubset());

    AidString aidStr3;
    aidStr3.From("123456789ABCDEF0*");
    EXPECT_FALSE(aidStr3.IsExact());
    EXPECT_TRUE(aidStr3.IsPrefix());
    EXPECT_FALSE(aidStr3.IsSubset());

    AidString aidStr4;
    aidStr4.From("123456789ABCDEF0#");
    EXPECT_FALSE(aidStr4.IsExact());
    EXPECT_FALSE(aidStr4.IsPrefix());
    EXPECT_TRUE(aidStr4.IsSubset());
}
/**
 * @tc.number:
 * @tc.name  :
 * @tc.desc  :
 */
TEST(AidString, eq)
{
    AidString aidstr = AidString("F01020304050");
    EXPECT_EQ(aidstr, aidstr);
    EXPECT_TRUE(aidstr.ExactlyEqualTo(aidstr));
    AidString aidstr1;
    aidstr1 = aidstr;
    EXPECT_EQ(aidstr, aidstr1);

    AidString aidstr2;
    aidstr2 = std::move(aidstr1);
    EXPECT_EQ(aidstr2, aidstr);

    AidString aidstr3("F01020304050*");
    AidString aidstr5("f01020304050*");
    EXPECT_FALSE(aidstr3.ExactlyEqualTo(aidstr));
    EXPECT_FALSE(aidstr.ExactlyEqualTo(aidstr3));

    EXPECT_TRUE(aidstr3.ExactlyEqualTo(aidstr5));
    EXPECT_TRUE(aidstr5.ExactlyEqualTo(aidstr3));

    AidString aidstr4("F01020304050#");
    EXPECT_FALSE(aidstr4.ExactlyEqualTo(aidstr));
}
/**
 * @tc.number:
 * @tc.name  :
 * @tc.desc  :
 */
TEST(AidString, EqualToWithoutPattern)
{
    AidString aidstr = AidString("F01020304050");
    AidString aidstr1 = AidString("F0102030405060");

    AidString aidstr3("F01020304050*");
    AidString aidstr5("f01020304050*");

    EXPECT_FALSE(aidstr3.EqualToWithoutPattern(aidstr1));
    EXPECT_FALSE(aidstr1.EqualToWithoutPattern(aidstr3));

    EXPECT_TRUE(aidstr3.EqualToWithoutPattern(aidstr));
    EXPECT_TRUE(aidstr.EqualToWithoutPattern(aidstr3));

    AidString aidstr4("F01020304050#");
    EXPECT_TRUE(aidstr4.EqualToWithoutPattern(aidstr));
}

/**
 * @tc.number:
 * @tc.name  :
 * @tc.desc  :
 */
TEST(AidString, less)
{
    AidString aidStr1 = AidString("F01020304050");
    EXPECT_TRUE(aidStr1.IsExact());
    AidString aidStr2 = AidString("F0102030405060");
    EXPECT_TRUE(aidStr2.IsExact());
    EXPECT_TRUE(aidStr1 < aidStr2);
    EXPECT_EQ(aidStr1, aidStr1);

    AidString aidStr3 = AidString("F0A020304050");
    EXPECT_TRUE(aidStr3.IsExact());
    EXPECT_TRUE(aidStr1 < aidStr2);

    AidString aidStr4 = AidString("f010203040506070");
    EXPECT_TRUE(aidStr4.IsExact());
    EXPECT_TRUE(aidStr1 < aidStr4);

    // prefix
    AidString aidStr5 = AidString("f0102030405060*");
    EXPECT_TRUE(aidStr5.IsPrefix());
    EXPECT_FALSE(aidStr5 < AidString("f0102030405060"));
    EXPECT_FALSE(aidStr5 < AidString("f010203040506070"));

    AidString aidStr6 = AidString("f0102030405060*");
    EXPECT_TRUE(aidStr6.IsPrefix());
    EXPECT_FALSE(aidStr5 < aidStr6);
    EXPECT_FALSE(aidStr6 < aidStr5);
    AidString aidStr7 = AidString("f010203040506070*");
    EXPECT_TRUE(aidStr7.IsPrefix());
    EXPECT_FALSE(aidStr5 < aidStr7);
    EXPECT_FALSE(aidStr7 < aidStr5);
    AidString aidStr8 = AidString("f01020304050*");
    EXPECT_TRUE(aidStr8.IsPrefix());
    EXPECT_FALSE(aidStr8 < aidStr5);
    EXPECT_FALSE(aidStr5 < aidStr8);

    // subset

    AidString aidStr9 = AidString("f0102030405060#");
    EXPECT_TRUE(aidStr9.IsSubset());
    EXPECT_FALSE(aidStr9 < AidString("f0102030405060"));
    EXPECT_FALSE(aidStr9 < AidString("f01020304050"));
    EXPECT_TRUE(aidStr9 < AidString("f01020304050607080"));
    AidString aidStr10 = AidString("f0102030405060#");
    EXPECT_TRUE(aidStr10.IsSubset());
    EXPECT_FALSE(aidStr9 < aidStr10);
    EXPECT_FALSE(aidStr10 < aidStr9);
    AidString aidStr11 = AidString("f010203040506070#");
    EXPECT_TRUE(aidStr11.IsSubset());
    EXPECT_FALSE(aidStr9 < aidStr11);
    EXPECT_FALSE(aidStr11 < aidStr9);
    AidString aidStr12 = AidString("f01020304050#");
    EXPECT_TRUE(aidStr12.IsSubset());
    EXPECT_FALSE(aidStr12 < aidStr9);
    EXPECT_FALSE(aidStr9 < aidStr12);
}
/**
 * @tc.number:
 * @tc.name  :
 * @tc.desc  :
 */
TEST(AidString, Conflict)
{
    AidString aidstr;
    EXPECT_TRUE(aidstr.ConflictWith(aidstr));
    EXPECT_FALSE(AidString().IsValid());
    EXPECT_TRUE(AidString("A00000000301").IsValid());
    EXPECT_TRUE(AidString("A00000000301").IsExact());
    EXPECT_TRUE(AidString("A00000000301*").IsPrefix());
    EXPECT_TRUE(AidString("A00000000301#").IsSubset());
    EXPECT_TRUE(AidString("A0000000030102").IsExact());
    EXPECT_TRUE(AidString("A0000000030102*").IsPrefix());
    EXPECT_TRUE(AidString("A0000000030102#").IsSubset());
    EXPECT_FALSE(AidString().ConflictWith(AidString()));
    EXPECT_FALSE(AidString().ConflictWith(AidString("A00000000301")));
    EXPECT_FALSE(AidString().ConflictWith(AidString("A00000000301*")));
    EXPECT_FALSE(AidString().ConflictWith(AidString("A00000000301#")));
    EXPECT_FALSE(AidString("A00000000301").ConflictWith(AidString()));
    EXPECT_FALSE(AidString("A00000000301*").ConflictWith(AidString()));
    EXPECT_FALSE(AidString("A00000000301#").ConflictWith(AidString()));

    // a: exact, b: exact, a==b
    EXPECT_TRUE(AidString("A00000000301").ConflictWith(AidString("A00000000301")));
    // a: exact, b: prefix
    EXPECT_TRUE(AidString("A00000000301").ConflictWith(AidString("A00000000301*")));
    EXPECT_TRUE(AidString("A00000000301").ConflictWith(AidString("A000000003*")));
    EXPECT_FALSE(AidString("A00000000301").ConflictWith(AidString("A0000000030102*")));
    // a: exact, b: subset
    EXPECT_TRUE(AidString("A00000000301").ConflictWith(AidString("A00000000301#")));
    EXPECT_FALSE(AidString("A00000000301").ConflictWith(AidString("A000000003#")));
    EXPECT_TRUE(AidString("A00000000301").ConflictWith(AidString("A0000000030102#")));
    // a: prefix, b: exact
    EXPECT_TRUE(AidString("A00000000301*").ConflictWith(AidString("A00000000301")));
    EXPECT_TRUE(AidString("A00000000301*").ConflictWith(AidString("A0000000030102")));
    EXPECT_FALSE(AidString("A00000000301*").ConflictWith(AidString("A000000003")));
    // a: prefix, b: prefix
    EXPECT_TRUE(AidString("A00000000301*").ConflictWith(AidString("A00000000301*")));
    EXPECT_TRUE(AidString("A00000000301*").ConflictWith(AidString("A0000000030102*")));
    EXPECT_TRUE(AidString("A00000000301*").ConflictWith(AidString("A000000003*")));

    // a: prefix, b: subset
    EXPECT_TRUE(AidString("A00000000301*").ConflictWith(AidString("A00000000301#")));
    EXPECT_TRUE(AidString("A00000000301*").ConflictWith(AidString("A0000000030102#")));
    EXPECT_FALSE(AidString("A00000000301*").ConflictWith(AidString("A000000003#")));
    // a: subset, b: exact
    EXPECT_TRUE(AidString("A00000000301#").ConflictWith(AidString("A00000000301")));
    EXPECT_FALSE(AidString("A00000000301#").ConflictWith(AidString("A0000000030102")));
    EXPECT_TRUE(AidString("A00000000301#").ConflictWith(AidString("A000000003")));

    // a: subset, b: prefix
    EXPECT_TRUE(AidString("A00000000301#").ConflictWith(AidString("A00000000301*")));
    EXPECT_FALSE(AidString("A00000000301#").ConflictWith(AidString("A0000000030102*")));
    EXPECT_TRUE(AidString("A00000000301#").ConflictWith(AidString("A000000003*")));
    // a: subset, b: subset
    EXPECT_TRUE(AidString("A00000000301#").ConflictWith(AidString("A00000000301#")));
    EXPECT_TRUE(AidString("A00000000301#").ConflictWith(AidString("A0000000030102#")));
    EXPECT_TRUE(AidString("A00000000301#").ConflictWith(AidString("A000000003#")));
}
/**
 * @tc.number:
 * @tc.name  :
 * @tc.desc  :
 */
TEST(AidString, Intersection)
{
    AidString aidstr;
    aidstr = AidString("A00000000301");
    EXPECT_STREQ(aidstr.Intersection(aidstr).c_str(), aidstr.ToString().c_str());

    EXPECT_STREQ(AidString().Intersection(AidString()).c_str(), "");
    EXPECT_STREQ(AidString("A00000000301").Intersection(AidString("")).c_str(), "");
    EXPECT_STREQ(AidString("A00000000301").Intersection(AidString("AAAAAAAAAA01")).c_str(), "");
    EXPECT_STREQ(AidString("A00000000301").Intersection(AidString("A00000000301")).c_str(), "A00000000301");
    EXPECT_STREQ(AidString("A00000000301").Intersection(AidString("A00000000301*")).c_str(), "A00000000301*");
    EXPECT_STREQ(AidString("A00000000301").Intersection(AidString("A000000003*")).c_str(), "A000000003*");
    EXPECT_STREQ(AidString("A00000000301").Intersection(AidString("A00000000301#")).c_str(), "A00000000301#");
    EXPECT_STREQ(AidString("A00000000301").Intersection(AidString("A000000003010203#")).c_str(), "A000000003010203#");
    EXPECT_STREQ(AidString("A00000000301*").Intersection(AidString("")).c_str(), "");
    EXPECT_STREQ(AidString("A00000000301*").Intersection(AidString("AAAAAAAAAA01")).c_str(), "");
    EXPECT_STREQ(AidString("A00000000301*").Intersection(AidString("AAAAAAAAAA01*")).c_str(), "");
    EXPECT_STREQ(AidString("A00000000301*").Intersection(AidString("AAAAAAAAAA01#")).c_str(), "");
    EXPECT_STREQ(AidString("A00000000301*").Intersection(AidString("A00000000301")).c_str(), "A00000000301*");
    EXPECT_STREQ(AidString("A00000000301*").Intersection(AidString("A00000000301*")).c_str(), "A00000000301*");
    EXPECT_STREQ(AidString("A00000000301*").Intersection(AidString("A000000003010203*")).c_str(), "A00000000301*");
    EXPECT_STREQ(AidString("A00000000301*").Intersection(AidString("A000000003*")).c_str(), "A000000003*");
    EXPECT_STREQ(AidString("A00000000301#").Intersection(AidString("AAAAAAAAAA01")).c_str(), "");
    EXPECT_STREQ(AidString("A00000000301#").Intersection(AidString("AAAAAAAAAA01*")).c_str(), "");
    EXPECT_STREQ(AidString("A00000000301#").Intersection(AidString("AAAAAAAAAA01#")).c_str(), "");
    EXPECT_STREQ(AidString("A00000000301#").Intersection(AidString("A00000000301")).c_str(), "A00000000301#");
    EXPECT_STREQ(AidString("A00000000301#").Intersection(AidString("A000000003010203#")).c_str(), "A000000003010203#");
    EXPECT_STREQ(AidString("A00000000301#").Intersection(AidString("A000000003#")).c_str(), "A00000000301#");
}

/**
 * @tc.number:
 * @tc.name  :
 * @tc.desc  :
 */
TEST(AidString, AidTypeToInt)
{
    EXPECT_EQ(AidTypeToInt(AidString().GetType()), 0);
    EXPECT_EQ(AidTypeToInt(AidType::INVALID), 0);
    EXPECT_EQ(AidTypeToInt(AidType::EXACT), 0);
    EXPECT_EQ(AidTypeToInt(AidType::PREFIX), 0x10);
    EXPECT_EQ(AidTypeToInt(AidType::SUBSET), 0x20);
}
}