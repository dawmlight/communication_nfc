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
#include "odf.h"

#include "access_rule_files_channel.h"
#include "der_parser.h"
#include "ef.h"
#include "loghelper.h"
#include "se_common_exception.h"

namespace OHOS::se::security {
ODF::ODF(std::weak_ptr<AccessRuleFilesChannel> arfChannel) : arfChannel_(arfChannel), cdfPath_("") {}

ODF::~ODF() {}

std::string ODF::ReadDataFromPath(std::string path)
{
    DebugLog("ODF read data from path");
    std::string sPath = path + ODF_PATH;
    EF ef = EF(arfChannel_);
    if (ef.SelectFilePath(sPath) == EF::APDU_SUCCESS) {
        return AnalyzeData(ef.ReadData(0, -1));
    } else {
        DebugLog("ODF not found");
        throw AccessControlError("ODF not found");
    }
    return "";
}

std::string ODF::AnalyzeData(std::string data)
{
    DebugLog("ODF analyze data");
    DerParser derCdf = DerParser(data);
    while (!derCdf.IsEnd()) {
        if (derCdf.ParseTlv() == 0xA5) {
            DebugLog("Found cdf path");
            cdfPath_ = derCdf.ParsePath();
        } else {
            derCdf.SkipTlvData();
        }
    }

    DerParser derDodf = DerParser(data);
    while (!derDodf.IsEnd()) {
        if (derDodf.ParseTlv() == 0xA7) {
            return derDodf.ParsePath();
        } else {
            derDodf.SkipTlvData();
        }
    }
    return "";
}
}  // namespace OHOS::se::security