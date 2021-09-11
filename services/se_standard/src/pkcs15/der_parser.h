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
#ifndef DER_PARSER_H
#define DER_PARSER_H

#include <string>
#include <vector>

namespace OHOS::se::security {
/*
 * Distinguished Encoding Rules Parser
 */
class DerParser {
public:
    // ASN.1 syntax
    static const auto ASN1_OCTET_STRING = 0x04;
    static const auto ASN1_OID = 0x06;
    static const auto ASN1_SEQUENCE = 0x30;
    static const auto ASN1_APPL_IDENTIFIER = 0x4F;
    static const auto ASN1_APPL_LABEL = 0x50;
    static const auto ASN1_APPL_PATH = 0x51;
    static const auto ASN1_APPL_TEMPLATE = 0x61;
    static const auto ASN1_FCP = 0x62;
    static const auto ASN1_PADDING = 0xFF;
    DerParser(std::string data);
    ~DerParser();
    bool IsEnd();
    int ParseTlv();
    int ParseTlv(int type);
    void SkipTlvData();
    std::string GetTlvData();
    std::vector<int> TakeSnapshot();
    void RestoreSnapshot(std::vector<int> snapshot);
    std::string ParseOid();
    std::string ParsePath();
    int GetTlvType();
    int GetTlvSize();

private:
    std::string derData_;
    int derSize_;
    int derIndex_;
    int tlvDataSize_;
    int ReadInt();
};
}  // namespace OHOS::se::security
#endif  // !DER_PARSER_H
