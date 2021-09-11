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
#include "ef.h"

#include "access_rule_files_channel.h"
#include "der_parser.h"
#include "loghelper.h"
#include "se_common_exception.h"

namespace OHOS::se::security {
EF::EF(std::weak_ptr<AccessRuleFilesChannel> arfChannel)
    : fileType_(FILE_UNKNOWN),
      fileStructure_(FILE_UNKNOWN),
      fileSize_(0),
      fileRecordSize_(0),
      fileNbRecords_(0),
      fileId_(FILE_UNKNOWN),
      arfChannel_(arfChannel)
{
}

EF::~EF() {}

int EF::SelectFilePath(std::string path)
{
    DebugLog("Select File");
    if (path.empty() || path.length() == 0 || ((path.length() % 2) != 0)) {
        DebugLog("Incorrect path");
        throw SecureElementError("Incorrect path");
    }

    fileType_ = FILE_UNKNOWN;
    fileStructure_ = FILE_UNKNOWN;
    fileSize_ = 0;
    fileRecordSize_ = 0;
    fileNbRecords_ = 0;

    std::string res;
    // select file command
    std::string cmd = {0x00, (char)0xA4, 0x00, 0x04, 0x02, 0x00, 0x00};
    std::shared_ptr<AccessRuleFilesChannel> arfChannel = arfChannel_.lock();
    if (!arfChannel) {
        DebugLog("ArfChannel has expired");
        throw SecureElementError("ArfChannel has expired");
    }
    for (size_t i = 0; i < path.length(); i += 2) {
        fileId_ = (((path[i] & 0xFF) << 8) | (path[i + 1] & 0xFF));
        cmd[5] = path[i] & 0xFF;
        cmd[6] = path[i + 1] & 0xFF;
        res = arfChannel->Transmit(cmd);
        int sw1 = res[res.length() - 2] & 0xFF;
        int sw2 = res[res.length() - 1] & 0xFF;
        if (sw1 != 0x62 && sw1 != 0x63 && sw1 != 0x90 && sw1 != 0x91) {
            return ((sw1 << 8) | sw2);
        }
    }
    ParseFile(res);
    return APDU_SUCCESS;
}

int EF::GetFileId()
{
    return fileId_;
}

std::string EF::ReadData(int offset, int len)
{
    DebugLog("Read data");
    if (fileSize_ == 0) {
        return "";
    }
    if (fileType_ != FILE_TYPE_EF) {
        DebugLog("Incorrect file type");
        throw SecureElementError("Incorrect file type");
    }
    if (fileStructure_ != FILE_STRUCTURE_TRANSPARENT) {
        DebugLog("Incorrect file structure");
        throw SecureElementError("Incorrect file structure");
    }
    if (len == -1) {
        len = fileSize_;
    }

    std::string result = "";
    int length = 0;
    std::string cmd = {0x00, (char)0xB0, 0x00, 0x00, 0x00};
    std::shared_ptr<AccessRuleFilesChannel> arfChannel = arfChannel_.lock();
    if (!arfChannel) {
        DebugLog("ArfChannel has expired");
        throw SecureElementError("ArfChannel has expired");
    }
    while (len != 0) {
        if (len < 253) {
            length = len;
        } else {
            length = 253;
        }
        cmd[2] = (char)((offset >> 8) & 0xFF);
        cmd[3] = (char)(offset & 0xFF);
        cmd[4] = (char)(length & 0xFF);
        std::string res = arfChannel->Transmit(cmd);
        result += res.substr(0, length);
        len -= length;
        offset += length;
    }
    return result;
}

std::string EF::ReadRecord(int record)
{
    DebugLog("Read record");
    if (fileType_ != FILE_TYPE_EF) {
        DebugLog("Incorrect file type");
        throw SecureElementError("Incorrect file type");
    }
    if (fileStructure_ != FILE_STRUCTURE_LINEAR_FIXED) {
        DebugLog("Incorrect file structure");
        throw SecureElementError("Incorrect file structure");
    }
    if ((record < 0) || (record > fileNbRecords_)) {
        DebugLog("Incorrect record");
        throw SecureElementError("Incorrect record");
    }

    std::string cmd = {0x00, (char)0xB2, (char)record, 0x04, (char)fileRecordSize_};
    std::shared_ptr<AccessRuleFilesChannel> arfChannel = arfChannel_.lock();
    if (!arfChannel) {
        DebugLog("ArfChannel has expired");
        throw SecureElementError("ArfChannel has expired");
    }
    std::string res = arfChannel->Transmit(cmd);
    return res.substr(0, fileRecordSize_);
}

int EF::GetFileNbRecords()
{
    if (fileNbRecords_ < 0) {
        DebugLog("Incorrect file type");
        throw SecureElementError("Incorrect file type");
    }
    return fileNbRecords_;
}

void EF::ParseFile(std::string data)
{
    DebugLog("Parse File");
    if (data.empty() || data.length() == 0) {
        DebugLog("Data is null");
        return;
    }
    if (data[0] == 0x62) {
        this->ParseUSIMFile(data);
    } else {
        this->ParseSIMFile(data);
    }
}

void EF::ParseSIMFile(std::string data)
{
    DebugLog("Parse SIM File");
    if (data.empty() || data.length() < 15) {
        DebugLog("Data length error");
        throw SecureElementError("Data length error");
    }
    if ((data[6] & 0xFF) == 0x04) {
        fileType_ = FILE_TYPE_EF;
    } else {
        fileType_ = FILE_UNKNOWN;
    }
    if ((data[13] & 0xFF) == 0x00) {
        fileStructure_ = FILE_STRUCTURE_TRANSPARENT;
    } else if ((data[13] & 0xFF) == 0x01) {
        fileStructure_ = FILE_STRUCTURE_LINEAR_FIXED;
    } else {
        fileStructure_ = FILE_UNKNOWN;
    }
    fileSize_ = ((data[2] & 0xFF) << 8 | (data[3] & 0xFF));

    if (fileType_ == FILE_TYPE_EF && fileStructure_ != FILE_STRUCTURE_TRANSPARENT) {
        fileRecordSize_ = data[14] & 0xFF;
        fileNbRecords_ = (fileSize_ / fileRecordSize_);
    }
}

void EF::ParseUSIMFile(std::string data)
{
    DebugLog("Parse USIM File");
    DerParser der = DerParser(data);
    der.ParseTlv(DerParser::ASN1_FCP);
    while (!der.IsEnd()) {
        switch (der.ParseTlv()) {
            case 0x80: {
                DebugLog("Get file size");
                std::string tlv = der.GetTlvData();
                if (!tlv.empty() && (tlv.length() >= 2)) {
                    fileSize_ = ((tlv[0] & 0xFF) << 8) | (tlv[1] & 0xFF);
                }
            } break;

            case 0x82: {
                DebugLog("Get file structure");
                std::string tlv = der.GetTlvData();
                if (!tlv.empty() && (tlv.length() >= 2)) {
                    if ((tlv[0] & 0x07) == 0x01) {
                        fileStructure_ = FILE_STRUCTURE_TRANSPARENT;
                    } else if ((tlv[0] & 0x07) == 0x02) {
                        fileStructure_ = FILE_STRUCTURE_LINEAR_FIXED;
                    } else {
                        fileStructure_ = FILE_UNKNOWN;
                    }

                    if ((tlv[0] & 0x38) == 0x38) {
                        fileType_ = FILE_UNKNOWN;
                    } else {
                        fileType_ = FILE_TYPE_EF;
                    }
                    if (tlv.length() == 5) {
                        fileRecordSize_ = tlv[3] & 0xFF;
                        fileNbRecords_ = tlv[4] & 0xFF;
                    }
                }
            } break;

            default: {
                der.SkipTlvData();
            } break;
        }
    }
}
}  // namespace OHOS::se::security