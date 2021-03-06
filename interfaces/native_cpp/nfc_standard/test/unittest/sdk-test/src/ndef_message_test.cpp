#include "ndef_message.h"

#include <gtest/gtest.h>

#include <vector>
using namespace OHOS::nfc::sdk;
// 空的Ndef数据
// 1 ndef message no record
std::vector<unsigned char> data0 = {0xdc, 0x01, 0x00, 0x00};

// 1 short record data
std::vector<unsigned char> data1 = {0xd1, 0x01, 0x12, 0x55, 0x03, 0x61, 0x73, 0x63, 0x69, 0x69, 0x2e,
                                    0x39, 0x31, 0x31, 0x63, 0x68, 0x61, 0x2e, 0x63, 0x6f, 0x6d, 0x2f};

// 1 ndef message composed with 1 record data, it include 4 rtd type records: uri, text, mime, external
std::vector<unsigned char> data2 = {
    0x91, 0x01, 0x11, 0x55, 0x33, 0x4d, 0x75, 0x6c, 0x74, 0x69, 0x52, 0x65, 0x63, 0x6f, 0x72, 0x64, 0x2e, 0x63, 0x6f,
    0x6d, 0x2f, 0x11, 0x01, 0x2a, 0x54, 0x35, 0x55, 0x54, 0x46, 0x2d, 0x38, 0x54, 0x65, 0x73, 0x74, 0x57, 0x72, 0x69,
    0x74, 0x65, 0x4e, 0x64, 0x65, 0x66, 0x54, 0x61, 0x67, 0x57, 0x69, 0x74, 0x68, 0x4d, 0x75, 0x6c, 0x74, 0x69, 0x52,
    0x65, 0x63, 0x6f, 0x72, 0x64, 0x2d, 0x74, 0x65, 0x78, 0x74, 0x12, 0x25, 0x10, 0x61, 0x70, 0x70, 0x6c, 0x69, 0x63,
    0x61, 0x74, 0x69, 0x6f, 0x6e, 0x2f, 0x76, 0x6e, 0x64, 0x2e, 0x63, 0x6f, 0x6d, 0x2e, 0x65, 0x78, 0x61, 0x6d, 0x70,
    0x6c, 0x65, 0x2e, 0x68, 0x6f, 0x6d, 0x2e, 0x68, 0x65, 0x6c, 0x6c, 0x6f, 0x48, 0x65, 0x6c, 0x6c, 0x6f, 0x20, 0x74,
    0x65, 0x73, 0x74, 0x20, 0x74, 0x69, 0x6d, 0x65, 0x21, 0x54, 0x18, 0x28, 0x63, 0x6f, 0x6d, 0x2e, 0x65, 0x78, 0x61,
    0x6d, 0x70, 0x6c, 0x65, 0x3a, 0x65, 0x78, 0x74, 0x65, 0x72, 0x6e, 0x61, 0x6c, 0x74, 0x79, 0x70, 0x65, 0x54, 0x65,
    0x73, 0x74, 0x57, 0x72, 0x69, 0x74, 0x65, 0x4e, 0x64, 0x65, 0x66, 0x54, 0x61, 0x67, 0x57, 0x69, 0x74, 0x68, 0x4d,
    0x75, 0x6c, 0x74, 0x69, 0x52, 0x65, 0x63, 0x6f, 0x72, 0x64, 0x2d, 0x65, 0x78, 0x74, 0x65, 0x72, 0x6e, 0x61, 0x6c};

// 1 ndef message composed with multi records. long record data
std::vector<unsigned char> data3 = {
    0x92, 0x09, 0x0a, 0x64, 0x65, 0x6d, 0x6f, 0x2e, 0x66, 0x6d, 0x73, 0x68, 0x5e, 0x46, 0x5a, 0x69, 0x00, 0x23, 0x00,
    0x25, 0x00, 0x02, 0x12, 0x03, 0x07, 0x74, 0x2f, 0x70, 0x1d, 0x90, 0x28, 0x8a, 0x00, 0x00, 0x70, 0x42, 0x03, 0x10,
    0x02, 0x00, 0x00, 0x63, 0x2f, 0x6d, 0x70, 0x01, 0xbe, 0x00, 0x62, 0x00, 0x00, 0x00, 0x31, 0x00, 0x00, 0x44, 0x00,
    0x02, 0x00, 0x00, 0x8f, 0x8e, 0x8f, 0x8e, 0x5c, 0x8e, 0x98, 0x8e, 0xc2, 0x8e, 0xc5, 0x8e, 0xc5, 0x8e, 0xc6, 0x8e,
    0xc7, 0x8e, 0xc8, 0x8e, 0xc9, 0x8e, 0xc9, 0x8e, 0xc9, 0x8e, 0xc9, 0x8e, 0xca, 0x8e, 0xca, 0x8e, 0xca, 0x8e, 0xca,
    0x8e, 0xcb, 0x8e, 0xcb, 0x8e, 0xcb, 0x8e, 0xcb, 0x8e, 0xcc, 0x8e, 0xcc, 0x8e, 0xcc, 0x8e, 0xcc, 0x8e, 0xcc, 0x8e,
    0xcd, 0x8e, 0xcd, 0x8e, 0xcd, 0x8e, 0xcd, 0x8e, 0xce, 0x8e, 0xce, 0x8e, 0xce, 0x8e, 0xce, 0x8e, 0xce, 0x8e, 0xce,
    0x8e, 0xce, 0x8e, 0xce, 0x8e, 0xcf, 0x8e, 0xcf, 0x8e, 0xcf, 0x8e, 0xcf, 0x8e, 0xcf, 0x8e, 0xcf, 0x8e, 0xd0, 0x8e,
    0xd0, 0x8e, 0xcf, 0x8e, 0xd0, 0x8e, 0xd0, 0x8e, 0xd0, 0x8e, 0xd0, 0x8e, 0xd0, 0x8e, 0xd1, 0x8e, 0xd1, 0x8e, 0xd1,
    0x8e, 0xd1, 0x8e, 0xd1, 0x8e, 0xd1, 0x8e, 0xd1, 0x8e, 0xd2, 0x8e, 0xd2, 0x8e, 0xd2, 0x8e, 0xd2, 0x8e, 0xd2, 0x8e,
    0xd2, 0x8e, 0xd2, 0x8e, 0xd2, 0x8e, 0xd2, 0x8e, 0xd3, 0x8e, 0xd3, 0x8e, 0xd3, 0x8e, 0xd3, 0x8e, 0xd3, 0x8e, 0xd3,
    0x8e, 0xd3, 0x8e, 0xd3, 0x8e, 0xd3, 0x8e, 0xd4, 0x8e, 0xd4, 0x8e, 0xd4, 0x8e, 0xd4, 0x8e, 0xd4, 0x8e, 0xd4, 0x8e,
    0xd4, 0x8e, 0xd4, 0x8e, 0xd4, 0x8e, 0xd5, 0x8e, 0xd4, 0x8e, 0xd3, 0x8e, 0xd5, 0x8e, 0xd4, 0x8e, 0xd4, 0x8e, 0xd5,
    0x8e, 0xd4, 0x8e, 0xd4, 0x8e, 0xd4, 0x8e, 0xd5, 0x8e, 0xd5, 0x8e, 0x00, 0x00, 0x65, 0x40, 0x32, 0x00, 0x64, 0xc0,
    0x33, 0x80, 0x64, 0xc0, 0x34, 0x00, 0x64, 0xc0, 0x35, 0x80, 0x65, 0x40, 0x36, 0x80, 0x64, 0xc0, 0x37, 0x00, 0x64,
    0xc0, 0x38, 0x00, 0x64, 0xc0, 0x39, 0x80, 0x64, 0xc0, 0x3a, 0x80, 0x64, 0xc0, 0x3b, 0x00, 0x64, 0xc0, 0x3c, 0x80,
    0x64, 0xc0, 0x3d, 0x00, 0x64, 0xc0, 0x3e, 0x00, 0x64, 0xc0, 0x3f, 0x80, 0x64, 0xc0, 0x40, 0x00, 0x64, 0xc0, 0x41,
    0x80, 0x64, 0xc0, 0x42, 0x80, 0x64, 0xc0, 0x43, 0x00, 0x64, 0xc0, 0x44, 0x80, 0x64, 0xc0, 0x45, 0x00, 0x64, 0xc0,
    0x46, 0x00, 0x64, 0xc0, 0x47, 0x80, 0x64, 0xc0, 0x48, 0x80, 0x64, 0xc0, 0x49, 0x00, 0x64, 0xc0, 0x4a, 0x00, 0x64,
    0xc0, 0x4b, 0x80, 0x64, 0xc0, 0x4c, 0x00, 0x64, 0xc0, 0x4d, 0x80, 0x64, 0xc0, 0x4e, 0x80, 0x64, 0xc0, 0x4f, 0x00,
    0x64, 0xc0, 0x50, 0x80, 0x64, 0xc0, 0x51, 0x00, 0x64, 0xc0, 0x52, 0x00, 0x64, 0xc0, 0x53, 0x80, 0x64, 0xc0, 0x54,
    0x00, 0x64, 0xc0, 0x55, 0x80, 0x64, 0xc0, 0x56, 0x80, 0x64, 0xc0, 0x57, 0x00, 0x64, 0xc0, 0x58, 0x00, 0x64, 0xc0,
    0x59, 0x80, 0x64, 0xc0, 0x5a, 0x80, 0x64, 0xc0, 0x5b, 0x00, 0x64, 0xc0, 0x5c, 0x80, 0x64, 0xc0, 0x5d, 0x00, 0x64,
    0xc0, 0x5e, 0x00, 0x63, 0x40, 0x5f, 0x80, 0x63, 0x40, 0x60, 0x80, 0x64, 0xc0, 0x61, 0x00, 0x63, 0x40, 0x62, 0x00,
    0x63, 0x40, 0x63, 0x80, 0x63, 0x40, 0x64, 0x00, 0x63, 0x40, 0x65, 0x80, 0x63, 0x40, 0x66, 0x80, 0x63, 0x40, 0x67,
    0x00, 0x63, 0x40, 0x68, 0x00, 0x63, 0x40, 0x69, 0x80, 0x63, 0x40, 0x6a, 0x80, 0x63, 0x40, 0x6b, 0x00, 0x63, 0x40,
    0x6c, 0x80, 0x63, 0x40, 0x6d, 0x00, 0x63, 0x40, 0x6e, 0x00, 0x63, 0x40, 0x6f, 0x80, 0x63, 0x40, 0x70, 0x00, 0x63,
    0x40, 0x71, 0x80, 0x63, 0x40, 0x72, 0x80, 0x63, 0x40, 0x73, 0x00, 0x63, 0x40, 0x74, 0x80, 0x63, 0x40, 0x75, 0x00,
    0x63, 0x40, 0x76, 0x00, 0x63, 0x40, 0x77, 0x80, 0x63, 0x40, 0x78, 0x80, 0x63, 0x40, 0x79, 0x00, 0x63, 0x40, 0x7a,
    0x00, 0x63, 0x40, 0x7b, 0x80, 0x63, 0x40, 0x7c, 0x00, 0x63, 0x40, 0x7d, 0x80, 0x63, 0x40, 0x7e, 0x80, 0x63, 0x40,
    0x7f, 0x00};

// 1 ndef message composed with 1 record which mId_ is not null
std::vector<unsigned char> data4 = {0xd9, 0x01, 0x12, 0x01, 0x55, 0x74, 0x33, 0x61, 0x73, 0x63, 0x69, 0x69,
                                    0x2e, 0x39, 0x31, 0x31, 0x63, 0x68, 0x61, 0x2e, 0x63, 0x6f, 0x6d, 0x2f};

// chunck data
std::vector<unsigned char> data5 = {0xB1, 0x01, 0x0e, 0x54, 0x35, 0x55, 0x54, 0x46, 0x2d, 0x38, 0x31, 0x31,
                                    0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x56, 0x00, 0x0e, 0x35, 0x55, 0x54,
                                    0x46, 0x2d, 0x38, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31};

/**
 * @tc.number    : NFC_NdefMessage_API_0001
 * @tc.name      : API GetNdefMessage  test
 * @tc.desc      : Api test
 */
TEST(NdefMessage, GetNdefMessage_string_OK)
{
    auto msg0 = NdefMessage::GetNdefMessage(std::string(data0.begin(), data0.end()));
    EXPECT_FALSE(msg0);
    auto msg1 = NdefMessage::GetNdefMessage(std::string(data1.begin(), data1.end()));
    EXPECT_TRUE(msg1);
    auto records1 = msg1->GetNdefRecords();
    EXPECT_EQ(records1.size(), (unsigned int)1);

    auto msg2 = NdefMessage::GetNdefMessage(std::string(data2.begin(), data2.end()));
    EXPECT_TRUE(msg2);
    auto records2 = msg2->GetNdefRecords();
    EXPECT_EQ(records2.size(), (unsigned int)4);

    auto msg3 = NdefMessage::GetNdefMessage(std::string(data3.begin(), data3.end()));
    EXPECT_TRUE(msg3);
    auto records3 = msg3->GetNdefRecords();
    EXPECT_EQ(records3.size(), (unsigned int)2);

    auto msg4 = NdefMessage::GetNdefMessage(std::string(data3.begin(), data3.end() - 10));
    EXPECT_TRUE(msg4);
    auto records4 = msg4->GetNdefRecords();
    EXPECT_EQ(records4.size(), (unsigned int)2);

    auto msg5 = NdefMessage::GetNdefMessage(std::string(data4.begin(), data4.end()));
    EXPECT_TRUE(msg5);
    auto records5 = msg5->GetNdefRecords();
    EXPECT_EQ(records5.size(), (unsigned int)1);

    auto msg6 = NdefMessage::GetNdefMessage(std::string(data5.begin(), data5.end()));
    EXPECT_TRUE(msg6);
    auto records6 = msg6->GetNdefRecords();
    EXPECT_EQ(records5.size(), (unsigned int)1);
}

/**
 * @tc.number    : NFC_NdefMessage_API_0002
 * @tc.name      : API GetNdefMessage exception test
 * @tc.desc      : Api test
 */
TEST(NdefMessage, GetNdefMessage_string_invalid)
{
    // null string
    std::string d1;
    auto msg1 = NdefMessage::GetNdefMessage(d1);
    EXPECT_FALSE(msg1);

    // invalid string
    std::string d2{0x0};
    auto msg2 = NdefMessage::GetNdefMessage(d2);
    EXPECT_FALSE(msg2);

    // payload length invalid
    auto msg3 = NdefMessage::GetNdefMessage(std::string(data1.begin(), data1.end() - 3));
    EXPECT_FALSE(msg3);
}

/**
 * @tc.number    : NFC_NdefMessage_API_0003
 * @tc.name      : API GetTagRtdType test
 * @tc.desc      : Api test
 */
TEST(NdefMessage, GetTagRtdType)
{
    std::string rv;
    rv = NdefMessage::GetTagRtdType(NdefMessage::EmRtdType::RTD_INVALID);
    EXPECT_STREQ(rv.c_str(), "");
    rv = NdefMessage::GetTagRtdType(NdefMessage::EmRtdType::RTD_TEXT);
    EXPECT_STREQ(rv.c_str(), "T");
    rv = NdefMessage::GetTagRtdType(NdefMessage::EmRtdType::RTD_URI);
    EXPECT_STREQ(rv.c_str(), "U");
    rv = NdefMessage::GetTagRtdType(NdefMessage::EmRtdType::RTD_SMART_POSTER);
    EXPECT_STREQ(rv.c_str(), "Sp");
    rv = NdefMessage::GetTagRtdType(NdefMessage::EmRtdType::RTD_ALTERNATIVE_CARRIER);
    EXPECT_STREQ(rv.c_str(), "ac");
    rv = NdefMessage::GetTagRtdType(NdefMessage::EmRtdType::RTD_HANDOVER_CARRIER);
    EXPECT_STREQ(rv.c_str(), "Hc");
    rv = NdefMessage::GetTagRtdType(NdefMessage::EmRtdType::RTD_HANDOVER_REQUEST);
    EXPECT_STREQ(rv.c_str(), "Hr");
    rv = NdefMessage::GetTagRtdType(NdefMessage::EmRtdType::RTD_HANDOVER_SELECT);
    EXPECT_STREQ(rv.c_str(), "Hs");
    rv = NdefMessage::GetTagRtdType(NdefMessage::EmRtdType::RTD_OHOS_APP);
    EXPECT_STREQ(rv.c_str(), "ohos.com:pkg");
}

/**
 * @tc.number    : NFC_NdefMessage_API_0004
 * @tc.name      : API MakeUriRecord test
 * @tc.desc      : Api test
 */
TEST(NdefMessage, MakeUriRecord)
{
    auto rc1 = NdefMessage::MakeUriRecord("");
    EXPECT_FALSE(rc1);
    auto rc2 = NdefMessage::MakeUriRecord("http://");
    EXPECT_TRUE(rc2);
}

/**
 * @tc.number    : NFC_NdefMessage_API_0005
 * @tc.name      : API MakeTextRecord test
 * @tc.desc      : Api test
 */
TEST(NdefMessage, MakeTextRecord)
{
    auto rc1 = NdefMessage::MakeTextRecord("", "zh_CN");
    EXPECT_TRUE(rc1);
    auto rc2 = NdefMessage::MakeTextRecord("哈哈", "zh_CN");
    EXPECT_TRUE(rc2);
}

/**
 * @tc.number    : NFC_NdefMessage_API_0006
 * @tc.name      : API MakeMimeRecord test
 * @tc.desc      : Api test
 */
TEST(NdefMessage, MakeMimeRecord)
{
    auto rc1 = NdefMessage::MakeMimeRecord("", "");
    EXPECT_FALSE(rc1);
    auto rc2 = NdefMessage::MakeMimeRecord("", "zh_CN");
    EXPECT_FALSE(rc2);
    auto rc3 = NdefMessage::MakeMimeRecord("a/b", "zh_CN");
    EXPECT_TRUE(rc3);
}

/**
 * @tc.number    : NFC_NdefMessage_API_0007
 * @tc.name      : API MakeExternalRecord test
 * @tc.desc      : Api test
 */
TEST(NdefMessage, MakeExternalRecord)
{
    auto rc1 = NdefMessage::MakeExternalRecord("", "", "");
    EXPECT_FALSE(rc1);
    auto rc2 = NdefMessage::MakeExternalRecord("", "name", "");
    EXPECT_FALSE(rc2);
    auto rc3 = NdefMessage::MakeExternalRecord("domain", "name", "hello");
    EXPECT_TRUE(rc3);
    auto rc4 = NdefMessage::MakeExternalRecord("\r\n\t ", "name", "hello");
    EXPECT_FALSE(rc4);
    auto rc5 = NdefMessage::MakeExternalRecord("\r\n\t ", "\r\n\t  ", "hello");
    EXPECT_FALSE(rc5);

    auto rc6 = NdefMessage::MakeExternalRecord("domain\r\n\t ", "\r\n\t name", "hello");
    EXPECT_TRUE(rc6);
}

/**
 * @tc.number    : NFC_NdefMessage_API_0008
 * @tc.name      : API MessageToString test
 * @tc.desc      : Api test
 */
TEST(NdefMessage, MessageToString)
{
    auto msg1 = NdefMessage::GetNdefMessage(std::string(data1.begin(), data1.end()));
    EXPECT_TRUE(msg1);
    auto str1 = NdefMessage::MessageToString(msg1);
    EXPECT_FALSE(str1.empty());

    EXPECT_TRUE(NdefMessage::MessageToString(std::shared_ptr<NdefMessage>()).empty());
}

/**
 * @tc.number    : NFC_NdefMessage_API_0009
 * @tc.name      : API NdefRecordToString test
 * @tc.desc      : Api test
 */
TEST(NdefMessage, NdefRecordToString)
{
    std::string buffer;
    NdefMessage::NdefRecordToString(std::shared_ptr<NdefRecord>(), buffer, false, false);
    EXPECT_TRUE(buffer.empty());
}