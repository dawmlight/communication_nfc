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
#include "ber_tlv.h"

#include "loghelper.h"
#include "se_common_exception.h"

namespace OHOS::se::security {
BerTlv::BerTlv(int tag, int length, std::string value, std::string data)
    : tag_(tag), length_(length), value_(value), data_(data)
{
}

BerTlv::BerTlv(int tag, int length, std::string value) : tag_(tag), length_(length), value_(value), data_("")
{
    std::string data = "";
    // tag
    if (tag > 0xFF) {
        data += (char)(((tag & 0x0000FF00) >> 8) & 0xFF);
        data += (char)((tag & 0x000000FF) & 0xFF);
    } else {
        data += (char)((tag & 0x00FF) & 0xFF);
    }
    // length
    if (length > 0xFFFF) {
        data += 0x83;
        data += (char)(((length & 0x00FF0000) >> 16) & 0xFF);
        data += (char)(((length & 0x0000FF00) >> 8) & 0xFF);
        data += (char)((length & 0x000000FF) & 0xFF);
    } else if (length > 0xFF) {
        data += 0x82;
        data += (char)(((length & 0x0000FF00) >> 8) & 0xFF);
        data += (char)((length & 0x000000FF) & 0xFF);
    } else if (length > 0x7F) {
        data += 0x81;
        data += (char)((length & 0x000000FF) & 0xFF);
    } else {
        data += (char)((length & 0x000000FF) & 0xFF);
    }
    // value
    data += value;
    data_ = data;
}

BerTlv::~BerTlv() {}

int BerTlv::GetTag()
{
    return tag_;
}

int BerTlv::GetLength()
{
    return length_;
}

std::string BerTlv::GetValue()
{
    return value_;
}

std::string BerTlv::GetData()
{
    return data_;
}

std::shared_ptr<BerTlv> BerTlv::StrToBerTlv(std::string& data)
{
    if (data.empty()) {
        ErrorLog("Data is empty");
        throw AccessControlError("Parsing data error");
    }

    int curIndex = 0;
    int temp = data[curIndex++] & 0xff;
    int tag = 0;
    if ((temp & 0x1f) == 0x1f) {
        if (curIndex >= (int)data.length()) {
            ErrorLog("Data length error");
            throw AccessControlError("Parsing data error");
        }
        tag = temp << 8 | (data[curIndex++] & 0xff);
    } else {
        tag = temp;
    }

    if (curIndex >= (int)data.length()) {
        ErrorLog("Data length error");
        throw AccessControlError("Parsing data error");
    }
    temp = data[curIndex++] & 0xff;
    int length = 0;
    if (temp < 0x80) {
        length = temp;
    } else if (temp == 0x81) {
        length = data[curIndex++] & 0xff;
        if (length < 0x80) {
            ErrorLog("Length error");
            throw AccessControlError("Parsing data error");
        }
    } else if (temp == 0x82) {
        if (curIndex + 1 >= (int)data.length()) {
            ErrorLog("Data length error");
            throw AccessControlError("Parsing data error");
        }
        length = ((data[curIndex] & 0xff) << 8) | (data[curIndex + 1] & 0xff);
        curIndex += 2;
        if (length < 0x100) {
            ErrorLog("Length error");
            throw AccessControlError("Parsing data error");
        }
    } else if (temp == 0x83) {
        if (curIndex + 2 >= (int)data.length()) {
            ErrorLog("Data length error");
            throw AccessControlError("Parsing data error");
        }
        length = ((data[curIndex] & 0xff) << 16) | ((data[curIndex + 1] & 0xff) << 8) | (data[curIndex + 2] & 0xff);
        curIndex += 3;
        if (length < 0x10000) {
            ErrorLog("Length error");
            throw AccessControlError("Parsing data error");
        }
    } else {
        ErrorLog("Unsupported length");
        throw AccessControlError("Parsing data error");
    }

    std::string value;
    if (length == 0) {
        value = "";
    } else if (curIndex + length > (int)data.length()) {
        value = data.substr(curIndex);
    } else {
        value = data.substr(curIndex, length);
    }
    return std::make_shared<BerTlv>(tag, length, value, data.substr(0, curIndex + length));
}

void BerTlv::Print()
{
    printf("BerTlv::Tag : %d\n", tag_);
    printf("BerTlv::Length : %d\n", length_);
    printf("BerTlv::Value : ");
    for (size_t i = 0; i < value_.length(); i++) {
        printf("0x%02X ", value_[i]);
    }
    printf("\n");
}
}  // namespace OHOS::se::security