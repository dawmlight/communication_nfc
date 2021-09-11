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
#include "access_control_main.h"

#include "access_rule_cache.h"
#include "access_rule_files_channel.h"
#include "der_parser.h"
#include "ef.h"
#include "loghelper.h"
#include "se_common_exception.h"

namespace OHOS::se::security {
AccessControlMain::AccessControlMain(std::weak_ptr<AccessRuleFilesChannel> arfChannel,
                                     std::weak_ptr<AccessRuleCache> accessRuleCache,
                                     std::string path)
    : arfChannel_(arfChannel), accessRuleCache_(accessRuleCache), acmfPath_(path)
{
}

AccessControlMain::~AccessControlMain() {}

std::string AccessControlMain::ReadDataFromPath()
{
    DebugLog("ACMF read data from path");
    EF ef = EF(arfChannel_);
    if (ef.SelectFilePath(acmfPath_) == EF::APDU_SUCCESS) {
        return AnalyzeData(ef.ReadData(0, -1));
    } else {
        DebugLog("ACMF not found");
        throw AccessControlError("ACMF not found");
    }
    return "";
}

std::string AccessControlMain::AnalyzeData(std::string data)
{
    DebugLog("ACMF analyze data");
    DerParser der = DerParser(data);
    der.ParseTlv(DerParser::ASN1_SEQUENCE);
    if (der.ParseTlv(DerParser::ASN1_OCTET_STRING) != 8) {
        DebugLog("Refresh tag length error");
        throw AccessControlError("Refresh tag length error");
    }
    std::string refreshTag = der.GetTlvData();
    std::shared_ptr<AccessRuleCache> accessRuleCache = accessRuleCache_.lock();
    if (!accessRuleCache) {
        DebugLog("AccessRuleCache has expired");
        throw AccessControlError("AccessRuleCache has expired");
    }
    if (!accessRuleCache->CompareRefreshTag(refreshTag)) {
        DebugLog("Refresh tag has changed");
        accessRuleCache->SetRefreshTag(refreshTag);
        // ACRF path
        return der.ParsePath();
    }
    DebugLog("Refresh tag has not changed");
    return "";
}
}  // namespace OHOS::se::security