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
#include "der_parser.h"

#include "loghelper.h"
#include "se_common_exception.h"

namespace OHOS::se::security {
DerParser::DerParser(std::string data) : derData_(""), derSize_(0), derIndex_(0), tlvDataSize_(0)
{
    derData_ = data;
    derIndex_ = 0;
    derSize_ = 0;
    if (!data.empty()) {
        derSize_ = data.length();
        tlvDataSize_ = derSize_;
        if (derData_[derIndex_] == ASN1_PADDING) {
            tlvDataSize_ = 0;
            while (++derIndex_ < derSize_) {
                if (derData_[derIndex_] != ASN1_PADDING) {
                    throw AccessControlError("Incorrect file format");
                }
            }
        }
    }
}

DerParser::~DerParser() {}

bool DerParser::IsEnd()
{
    if (derIndex_ == derSize_) {
        return true;
    }
    if (derData_[derIndex_] == ASN1_PADDING) {
        while (++derIndex_ < derSize_) {
            if (derData_[derIndex_] != ASN1_PADDING) {
                throw AccessControlError("Incorrect file format");
            }
        }
        return true;
    }
    return false;
}

int DerParser::ParseTlv()
{
    int type = GetTlvType();
    tlvDataSize_ = GetTlvSize();
    return type;
}

int DerParser::ParseTlv(int type)
{
    int actualType = GetTlvType();
    if (actualType == type) {
        tlvDataSize_ = GetTlvSize();
    } else {
        DebugLog("Expect type: %d, actual type: %d", type, actualType);
        throw AccessControlError("Unexpected type");
    }
    return tlvDataSize_;
}

void DerParser::SkipTlvData()
{
    derIndex_ += tlvDataSize_;
}

std::string DerParser::GetTlvData()
{
    std::string data = derData_.substr(derIndex_, tlvDataSize_);
    derIndex_ += tlvDataSize_;
    return data;
}

std::vector<int> DerParser::TakeSnapshot()
{
    std::vector<int> snapshot;
    snapshot.push_back(derIndex_);
    snapshot.push_back(tlvDataSize_);
    return snapshot;
}

void DerParser::RestoreSnapshot(std::vector<int> snapshot)
{
    if (snapshot.size() != 2) {
        DebugLog("Invalid snapshot");
        throw AccessControlError("Invalid snapshot");
    }
    if (snapshot[0] < 0 || snapshot[0] > derSize_) {
        DebugLog("Out of index");
        throw AccessControlError("Out of index");
    }
    derIndex_ = snapshot[0];
    tlvDataSize_ = snapshot[1];
}

std::string DerParser::ParseOid()
{
    if (this->ParseTlv(ASN1_OID) <= 0) {
        DebugLog("Oid length is null");
        throw AccessControlError("Oid length is null");
    }

    int end = derIndex_ + tlvDataSize_;
    std::string oid;
    int subidentifier = ReadInt();
    if (subidentifier <= 79) {
        oid = std::to_string((subidentifier / 40)) + "." + std::to_string((subidentifier % 40));
    } else {
        oid = "2." + std::to_string(subidentifier - 80);
    }
    while (derIndex_ < end) {
        oid += "." + std::to_string(ReadInt());
    }
    DebugLog("Oid: %s", oid.c_str());
    return oid;
}

std::string DerParser::ParsePath()
{
    this->ParseTlv(ASN1_SEQUENCE);
    this->ParseTlv(ASN1_OCTET_STRING);
    return GetTlvData();
}

int DerParser::GetTlvType()
{
    if (IsEnd()) {
        DebugLog("End of buffer, cannot get tlv type");
        throw AccessControlError("Cannot get tlv type");
    }
    return derData_[derIndex_++];
}

int DerParser::GetTlvSize()
{
    if (IsEnd()) {
        DebugLog("End, cannot get tlv size");
        throw AccessControlError("Cannot get tlv size");
    }
    int tlvSize = (derData_[derIndex_++] & 0xFF);
    if (tlvSize > 128) {
        int size = tlvSize - 128;
        for (tlvSize = 0; size > 0; size--) {
            if (!IsEnd()) {
                tlvSize = (tlvSize << 8) + (derData_[derIndex_++] & 0xFF);
            } else {
                DebugLog("End, cannot get tlv size");
                throw AccessControlError("Cannot get tlv size");
            }
        }
    }
    if (derIndex_ + tlvSize > derSize_) {
        DebugLog("Data is not enough");
        throw AccessControlError("Data not enough");
    }
    return tlvSize;
}

int DerParser::ReadInt()
{
    int value = 0;
    do {
        value = (value << 7) + (derData_[derIndex_] & 0x7F);
    } while ((derData_[derIndex_++] & 0x80) != 0);
    return value;
}
}  // namespace OHOS::se::security