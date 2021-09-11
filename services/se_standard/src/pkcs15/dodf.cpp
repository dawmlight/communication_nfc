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
#include "dodf.h"

#include "access_rule_files_channel.h"
#include "der_parser.h"
#include "ef.h"
#include "loghelper.h"
#include "se_common_exception.h"

namespace OHOS::se::security {
DODF::DODF(std::weak_ptr<AccessRuleFilesChannel> arfChannel) : arfChannel_(arfChannel) {}

DODF::~DODF() {}

std::string DODF::ReadDataFromPath(std::string path)
{
    DebugLog("DODF read data from path");
    EF ef = EF(arfChannel_);
    if (ef.SelectFilePath(path) == EF::APDU_SUCCESS) {
        return AnalyzeData(ef.ReadData(0, -1));
    } else {
        DebugLog("DODF not found");
        throw AccessControlError("DODF not found");
    }
    return "";
}

std::string DODF::AnalyzeData(std::string data)
{
    DebugLog("DODF analyze data");
    DerParser der = DerParser(data);
    while (!der.IsEnd()) {
        if (der.ParseTlv() == 0xA1) {
            der.ParseTlv(DerParser::ASN1_SEQUENCE);
            der.SkipTlvData();
            der.ParseTlv(DerParser::ASN1_SEQUENCE);
            der.SkipTlvData();

            int tag = der.ParseTlv();
            if (tag == 0xA0) {
                der.SkipTlvData();
                tag = der.ParseTlv();
            }
            if (tag == 0xA1) {
                der.ParseTlv(DerParser::ASN1_SEQUENCE);
                std::vector<int> snapshot = der.TakeSnapshot();
                if (der.ParseOid() == AC_OID) {
                    return der.ParsePath();
                } else {
                    der.RestoreSnapshot(snapshot);
                    der.SkipTlvData();
                }
            } else {
                DebugLog("Need oid tag");
                throw AccessControlError("Need oid tag");
            }
        } else {
            der.SkipTlvData();
        }
    }
    return "";
}
}  // namespace OHOS::se::security