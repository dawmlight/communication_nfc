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

#include "card_emulation_util.h"

#include <gtest/gtest.h>

#include <algorithm>
#include <iostream>
using namespace OHOS::nfc::cardemulation;
namespace OHOS::nfc::cardemulation::test{
/**
* @tc.number:
* @tc.name  :
* @tc.desc  :
*/
TEST(CardEmulationUtil, StringToLower)
{
    EXPECT_STREQ(StringToLower("").c_str(), "");
    EXPECT_STREQ(StringToLower("123.#!").c_str(), "123.#!");
    EXPECT_STREQ(StringToLower("A1B2C3").c_str(), "a1b2c3");
    EXPECT_STREQ(StringToLower("a1B2c3").c_str(), "a1b2c3");
    EXPECT_STREQ(StringToLower("a1b2c3").c_str(), "a1b2c3");
}
/**
* @tc.number:
* @tc.name  :
* @tc.desc  :
*/
TEST(CardEmulationUtil, StringToUpper)
{
    EXPECT_STREQ(StringToUpper("").c_str(), "");
    EXPECT_STREQ(StringToUpper("123.#!").c_str(), "123.#!");
    EXPECT_STREQ(StringToUpper("A1B2C3").c_str(), "A1B2C3");
    EXPECT_STREQ(StringToUpper("a1B2c3").c_str(), "A1B2C3");
    EXPECT_STREQ(StringToUpper("a1b2c3").c_str(), "A1B2C3");
}
/**
* @tc.number:
* @tc.name  :
* @tc.desc  :
*/
TEST(CardEmulationUtil, StringStartsWith_OK)
{
    std::string src = "a001020304";
    for (size_t i = 1; i < src.size(); i++) {
        EXPECT_TRUE(StringStartsWith(src, src.substr(0, i)));
    }
    EXPECT_FALSE(StringStartsWith("SIM", "eSE", true));
    EXPECT_FALSE(StringStartsWith("SIM1", "eSE", true));
    EXPECT_FALSE(StringStartsWith("eSE", "SIM", true));
    EXPECT_FALSE(StringStartsWith("eSE1", "SIM", true));
}

/**
* @tc.number:
* @tc.name  :
* @tc.desc  :
*/
TEST(CardEmulationUtil, StringStartsWith_NotOK)
{
    std::string src = "a001020304";
    EXPECT_FALSE(StringStartsWith(src, ""));
    EXPECT_FALSE(StringStartsWith(src, "b"));
    EXPECT_FALSE(StringStartsWith(src, "ab"));
    EXPECT_FALSE(StringStartsWith(src, src + "ab"));
    EXPECT_FALSE(StringStartsWith("", ""));
}
/**
* @tc.number:
* @tc.name  :
* @tc.desc  :
*/
TEST(CardEmulationUtil, StringVectorToString)
{
    EXPECT_STREQ(StringVectorToString({}).c_str(), "[]");
    EXPECT_STREQ(StringVectorToString({""}).c_str(), "[ ]");
    EXPECT_STREQ(StringVectorToString({"abc"}).c_str(), "[abc ]");
    EXPECT_STREQ(StringVectorToString({"1", "2", "3"}).c_str(), "[1 2 3 ]");
}

/**
* @tc.number:
* @tc.name  :
* @tc.desc  :
*/
TEST(CardEmulationUtil, BytesToHexStr)
{
    std::vector<unsigned char> bytes;
    EXPECT_TRUE(BytesToHexStr(bytes, true).empty());
    EXPECT_TRUE(BytesToHexStr(bytes, false).empty());

    EXPECT_STRCASEEQ(BytesToHexStr(std::vector<unsigned char>{0x07, 0x89, 0xa1, 0xb2, 0xc3, 0xd4, 0xe5, 0xf6}).c_str(),
                     "0789A1B2C3D4E5F6");
    EXPECT_STRCASEEQ(
        BytesToHexStr(std::vector<unsigned char>{0x07, 0x89, 0xa1, 0xb2, 0xc3, 0xd4, 0xe5, 0xf6}, false).c_str(),
        "0789a1b2c3d4e5f6");

    EXPECT_STRCASEEQ(BytesToHexStr(std::vector<unsigned char>{0x6a, 0x82, 0x90, 0x00}, false).c_str(), "6a829000");
    EXPECT_STRCASEEQ(BytesToHexStr(std::vector<unsigned char>{0x6a, 0x82, 0x90, 0x00}, true).c_str(), "6A829000");
}

/**
* @tc.number:
* @tc.name  :
* @tc.desc  :
*/
TEST(CardEmulationUtil, HexStrToBytes)
{
    std::vector<unsigned char> res;
    HexStrToBytes("", res);
    EXPECT_EQ(res.size(), 0u);

    auto vecEQ = [](std::vector<unsigned char> const& actual, std::vector<unsigned char> const& expected) -> bool {
        return std::equal(actual.begin(), actual.end(), expected.begin());
    };

    HexStrToBytes("1234567890abcdef", res);
    EXPECT_EQ(res.size(), 8u);
    EXPECT_TRUE(vecEQ(res, decltype(res){0x12, 0x34, 0x56, 0x78, 0x90, 0xab, 0xcd, 0xef}));
    HexStrToBytes("234567890abcdef", res);
    EXPECT_TRUE(vecEQ(res, decltype(res){0x02, 0x34, 0x56, 0x78, 0x90, 0xab, 0xcd, 0xef}));
    HexStrToBytes("-234567890abcdef", res);
    EXPECT_TRUE(vecEQ(res, decltype(res){0x02, 0x34, 0x56, 0x78, 0x90, 0xab, 0xcd, 0xef}));
    HexStrToBytes("-234567890abcdef$", res);
    EXPECT_TRUE(vecEQ(res, decltype(res){0x00, 0x23, 0x45, 0x67, 0x89, 0x0a, 0xbc, 0xde, 0xf0}));
}

/**
* @tc.number:
* @tc.name  :
* @tc.desc  :
*/
TEST(CardEmulationUtil, StringTrim)
{
    std::string s0;
    EXPECT_TRUE(StringTrim(s0, "").empty());
    EXPECT_TRUE(StringTrimLeft(s0, "").empty());
    EXPECT_TRUE(StringTrimRight(s0, "").empty());
    std::string src("   hello world!!   ");
    StringTrim(src);
    EXPECT_STRCASEEQ(src.c_str(), "hello world!!");

    std::string s1 = StringTrim("aaa bcdef aaa", "a");
    EXPECT_STRCASEEQ(s1.c_str(), " bcdef ");

    std::string s2 = StringTrim("A0102030*", "*");
    EXPECT_STRCASEEQ(s2.c_str(), "A0102030");

    std::string s3 = StringTrim("A0102030#", "#");
    EXPECT_STRCASEEQ(s3.c_str(), "A0102030");

    std::string s4 = StringTrim("A0102030# ", "#* ");
    EXPECT_STRCASEEQ(s4.c_str(), "A0102030");
}

/**
* @tc.number:
* @tc.name  :
* @tc.desc  :
*/
TEST(CardEmulationUtil, StringSplit)
{
    auto re1 = StringSplit("", "");
    EXPECT_EQ(re1.size(), 1u);

    auto re2 = StringSplit("abc", "");
    EXPECT_EQ(re2.size(), 4u);

    auto re3 = StringSplit("abc", "\\.");
    EXPECT_EQ(re3.size(), 1u);

    auto re4 = StringSplit("abc.", "\\.");
    EXPECT_EQ(re4.size(), 1u);

    auto re5 = StringSplit("a.b.c", "\\.");
    EXPECT_EQ(re5.size(), 3u);
    auto re6 = StringSplit("a_b_c", "\\.");
    EXPECT_EQ(re6.size(), 1u);
}
/**
* @tc.number:
* @tc.name  :
* @tc.desc  :
*/
TEST(CardEmulationUtil, StringEqual)
{
    EXPECT_TRUE(StringEqual("", ""));
    EXPECT_TRUE(StringEqual("", "", true));
    EXPECT_TRUE(StringEqual("abcd", "abcd"));
    EXPECT_TRUE(StringEqual("abcd", "Abcd", true));
    EXPECT_TRUE(StringEqual("测试", "测试"));
    EXPECT_TRUE(StringEqual("测试", "测试", true));
    EXPECT_FALSE(StringEqual("0123", "01234"));
    EXPECT_FALSE(StringEqual("123", "789123", true));
    EXPECT_FALSE(StringEqual("abcd01234", "01234"));
    EXPECT_FALSE(StringEqual("12345678", "123", true));
}

/**
* @tc.number:
* @tc.name  :
* @tc.desc  :
*/
TEST(CardEmulationUtil, FindSelectAid)
{
    EXPECT_TRUE(FindSelectAid(nullptr, 0).empty());
    // select aid 
    // 01  02  03 04 05...
    // CLA INS P1 P2 AID_LEN
    EXPECT_STREQ("", FindSelectAid({}).c_str());

    // CLA != 0x00
    EXPECT_STREQ("", FindSelectAid({0x01, 0x02, 0x03, 0x04, 0x00, 0xa1, 0x02, 0x03, 0x04, 0x05}).c_str());
    // CLA == 0x00, INS != 0xA4
    EXPECT_STREQ("", FindSelectAid({0x00, 0x02, 0x03, 0x04, 0x00, 0xa1, 0x02, 0x03, 0x04, 0x05}).c_str());
    // CLA == 0x00, INS == 0xA4, P1 != 0x04
    EXPECT_STREQ("", FindSelectAid({0x00, 0x02, 0x03, 0x04, 0x00, 0xa1, 0x02, 0x03, 0x04, 0x05}).c_str());
    // CLA == 0x00, INS == 0xA4, P1 == 0x04, AID_LEN < aid最小长度
    EXPECT_STREQ("", FindSelectAid({0x00, 0x02, 0x04, 0x04, 0x04, 0xa1, 0x02, 0x03, 0x04, 0x05}).c_str());

    // CLA == 0x00, INS == 0xA4, P1 == 0x04, AID_LEN > aid最大长度
    EXPECT_STREQ("",
                 FindSelectAid({0x00, 0xa4, 0x04, 0x00, 0x11, 0xa1, 0x02, 0x03, 0x04, 0x05, 0x06,
                                0x07, 0x08, 0x9,  0xa,  0xb,  0xc,  0xd,  0xe,  0xf,  0x00, 0x1})
                     .c_str());
    // CLA == 0x00, INS == 0xA4, P1 == 0x04, AID_LEN >= min length(aid)
    // AID_LEN==5
    EXPECT_STREQ("A102030405", FindSelectAid({0x00, 0xa4, 0x04, 0x04, 0x05, 0xa1, 0x02, 0x03, 0x04, 0x05}).c_str());

    // AID_LEN=16
    EXPECT_STREQ("A102030405060708090A0B0C0D0E0F00",
                 FindSelectAid({0x00, 0xa4, 0x04, 0x00, 0x10, 0xa1, 0x02, 0x03, 0x04, 0x05, 0x06,
                                0x07, 0x08, 0x9,  0xa,  0xb,  0xc,  0xd,  0xe,  0xf,  0x00})
                     .c_str());
}
/**
* @tc.number:
* @tc.name  :
* @tc.desc  :
*/
TEST(CardEmulationUtil, StringToBytes)
{
    auto rv1 = StringToBytes("");
    EXPECT_TRUE(rv1.empty());

    std::vector<unsigned char> d2{0x01, 0x02, 0x0};
    auto rv2 = StringToBytes(std::string(d2.begin(), d2.end()));
    EXPECT_FALSE(rv2.empty());
    EXPECT_EQ(rv2.size(), 3u);
    EXPECT_TRUE(std::equal(d2.begin(), d2.end(), rv2.begin()));

    std::vector<unsigned char> d3{0xD1, 0xFF, 0x0, 0x04, 0x05};
    auto rv3 = StringToBytes(std::string(d3.begin(), d3.end()));
    EXPECT_FALSE(rv3.empty());
    EXPECT_EQ(rv3.size(), 5u);
    EXPECT_TRUE(std::equal(d3.begin(), d3.end(), rv3.begin()));
}
}