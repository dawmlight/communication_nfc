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
#include "dir.h"

#include "access_rule_files_channel.h"
#include "der_parser.h"
#include "ef.h"
#include "loghelper.h"
#include "se_common_exception.h"

namespace OHOS::se::security {
DIR::DIR(std::weak_ptr<AccessRuleFilesChannel> arfChannel) : arfChannel_(arfChannel) {}

DIR::~DIR() {}

std::string DIR::ReadDataFromAid(std::string aid)
{
    DebugLog("DIR read data from aid");
    std::string odfPath = "";
    int index = 1;
    EF ef = EF(arfChannel_);
    if (ef.SelectFilePath(DIR_PATH) != EF::APDU_SUCCESS) {
        DebugLog("DIR not found");
        throw AccessControlError("DIR not found");
    }
    while (1 <= ef.GetFileNbRecords()) {
        std::string data = ef.ReadRecord(index++);
        odfPath = AnalyzeData(data, aid);
        if (!odfPath.empty()) {
            break;
        }
    }
    return odfPath;
}
std::string DIR::AnalyzeData(std::string data, std::string aid)
{
    DebugLog("DIR analyze data");
    DerParser der = DerParser(data);
    der.ParseTlv(DerParser::ASN1_APPL_TEMPLATE);
    der.ParseTlv(DerParser::ASN1_APPL_IDENTIFIER);
    if (aid != der.GetTlvData()) {
        DebugLog("Record for another aid");
        return "";
    }
    int tag = der.ParseTlv();
    if (tag == DerParser::ASN1_APPL_LABEL) {
        der.GetTlvData();
        der.ParseTlv(DerParser::ASN1_APPL_PATH);
    } else if (tag != DerParser::ASN1_APPL_PATH) {
        DebugLog("Need application path tag");
        throw AccessControlError("Need application path tag");
    }
    return der.GetTlvData();
}
}  // namespace OHOS::se::security