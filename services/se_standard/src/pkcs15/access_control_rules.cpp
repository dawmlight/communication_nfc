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
#include "access_control_rules.h"

#include "access_control_conditions.h"
#include "access_rule_cache.h"
#include "access_rule_files_channel.h"
#include "der_parser.h"
#include "ef.h"
#include "general-data-objects/aid_ref_do.h"
#include "loghelper.h"
#include "se_common_exception.h"

namespace OHOS::se::security {
AccessControlRules::AccessControlRules(std::weak_ptr<AccessRuleFilesChannel> arfChannel,
                                       std::weak_ptr<AccessRuleCache> accessRuleCache)
    : arfChannel_(arfChannel), accessRuleCache_(accessRuleCache), accfCache_({})
{
}

AccessControlRules::~AccessControlRules() {}

void AccessControlRules::ReadDataFromPath(std::string path)
{
    DebugLog("ACRF read data from path");
    accfCache_.clear();
    EF ef = EF(arfChannel_);
    if (ef.SelectFilePath(path) == EF::APDU_SUCCESS) {
        AnalyzeData(ef.ReadData(0, -1));
    } else {
        DebugLog("ACRF not found");
        throw AccessControlError("ACRF not found");
    }
}

void AccessControlRules::AnalyzeData(std::string data)
{
    DebugLog("ACRF analyze data");
    std::string aid;
    int tag = 0;
    DerParser der = DerParser(data);
    while (!der.IsEnd()) {
        der.ParseTlv(DerParser::ASN1_SEQUENCE);
        switch (der.ParseTlv()) {
            case 0xA0: {
                der.ParseTlv(DerParser::ASN1_OCTET_STRING);
                aid = der.GetTlvData();
                tag = AidRefDo::AID_REF_DO_TAG;
            } break;

            case 0x81: {
                aid = "";
                tag = AidRefDo::AID_REF_DO_TAG_DEFAULT_APPLICATION;
            } break;

            case 0x82: {
                aid = "";
                tag = AidRefDo::AID_REF_DO_TAG;
            } break;

            default: {
                DebugLog("Parser ACRF error");
                throw AccessControlError("Parser ACRF error");
            } break;
        }
        std::string path = der.ParsePath();
        if (!path.empty()) {
            std::shared_ptr<AidRefDo> aidRefDo = std::make_shared<AidRefDo>(tag, aid);
            AccessControlConditions accf = AccessControlConditions(accessRuleCache_, aidRefDo);
            auto iter = accfCache_.find(path);
            if (iter != accfCache_.end()) {
                accf.ReadData(iter->second);
            } else {
                accf.ReadDataFromPath(arfChannel_, path);
                if (!(accf.GetData().empty())) {
                    accfCache_.insert(std::pair<std::string, std::string>(path, accf.GetData()));
                }
            }
        }
    }
}
}  // namespace OHOS::se::security