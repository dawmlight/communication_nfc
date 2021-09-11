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
#include "access_rule_application_controller.h"

#include "access_rule_cache.h"
#include "channel_access_rule.h"
#include "context.h"
#include "general-data-objects/ar_do.h"
#include "general-data-objects/ref_ar_do.h"
#include "general-data-objects/ref_do.h"
#include "general-data-objects/response_all_ar_do.h"
#include "general-data-objects/response_refresh_tag_do.h"
#include "loghelper.h"
#include "resources.h"
#include "se_channel.h"
#include "se_common_exception.h"
#include "terminal.h"

namespace OHOS::se::security {
AccessRuleApplicationController::AccessRuleApplicationController(std::weak_ptr<AccessRuleCache> accessRuleCache,
                                                                 std::weak_ptr<Terminal> terminal)
    : accessRuleCache_(accessRuleCache), terminal_(terminal), accessControlAid_(ARA_M_AID)
{
}

AccessRuleApplicationController::~AccessRuleApplicationController() {}

void AccessRuleApplicationController::Initialize()
{
    InfoLog("AccessRuleApplicationController::Initialize");
    std::shared_ptr<Terminal> terminal = terminal_.lock();
    if (!terminal) {
        ErrorLog("Terminal has expired");
        throw AccessControlError("Terminal has expired");
    }
    std::shared_ptr<AccessRuleCache> accessRuleCache = accessRuleCache_.lock();
    if (!accessRuleCache) {
        ErrorLog("AccessRuleCache has expired");
        throw AccessControlError("AccessRuleCache has expired");
    }

    std::shared_ptr<OHOS::se::SeChannel> channel = std::shared_ptr<OHOS::se::SeChannel>();
    // Get aid list from resource
    std::vector<std::string> aidList;
    if (terminal->GetName().substr(0, 3) == "eSE") {
        std::shared_ptr<osal::Context> context = (terminal->GetContext()).lock();
        std::shared_ptr<osal::Resources> resources = context->GetResources().lock();
        // ese config ara aid
        std::string* pStr = resources->GetStringArray(osal::R::config_ara_aid_list_ese);
        aidList.assign(pStr, pStr + (resources->GetInt(osal::R::config_ara_aid_list_ese_length)));
    }
    for (std::string aid : aidList) {
        try {
            channel = terminal->OpenLogicalChannelForAccessControl(aid);
            if (!channel) {
                ErrorLog("Could not open channel");
                throw MissingResourceError("Could not open channel");
            }
            accessControlAid_ = aid;
            break;
        } catch (const NoSuchElementError& e) {
            ErrorLog("Applet %s is not accessible", aid.c_str());
            continue;
        }
    }
    if (!channel) {
        channel = (terminal->OpenLogicalChannelForAccessControl(ARA_M_AID));
        if (!channel) {
            ErrorLog("Could not open channel");
            throw MissingResourceError("Could not open channel");
        }
    }
    try {
        std::shared_ptr<ChannelAccessRule> channelAccessRule = std::make_shared<ChannelAccessRule>();
        channelAccessRule->SetAccessRule(ChannelAccessRule::ACCESSRULE::ALWAYS, "");
        channelAccessRule->SetApduAccessRule(ChannelAccessRule::ACCESSRULE::ALWAYS);
        channel->SetChannelAccess(channelAccessRule);
        // Retrieve a refresh tag indicating whether any access rules have been updated.
        std::string refreshTag = this->GetRefreshTag(channel);
        if (accessRuleCache->CompareRefreshTag(refreshTag)) {
            InfoLog("RefreshTag has not changed");
            channel->Close();
            return;
        }
        // update refresh tag, re-read all access rules
        InfoLog("RefreshTag has changed");
        accessRuleCache->ClearAccessRules();
        std::vector<RefArDo> refArDoArray = this->GetAllAccessRules(channel);
        for (size_t i = 0; i < refArDoArray.size(); i++) {
            accessRuleCache->AddAccessRule((refArDoArray.at(i)).GetRefDo(), (refArDoArray.at(i)).GetArDo());
        }
        accessRuleCache->SetRefreshTag(refreshTag);
        channel->Close();
    } catch (const std::runtime_error& error) {
        channel->Close();
        throw error;
    }
}

std::string AccessRuleApplicationController::GetAraMAid()
{
    return ARA_M_AID;
}

std::string AccessRuleApplicationController::GetAccessControlAid()
{
    return accessControlAid_;
}

std::string AccessRuleApplicationController::GetRefreshTag(std::shared_ptr<OHOS::se::SeChannel> channel)
{
    InfoLog("AccessRuleApplicationController::GetRefreshTag");
    std::string responseRefreshTag = this->GetDataRefreshTag(channel);
    std::shared_ptr<BerTlv> bt = BerTlv::StrToBerTlv(responseRefreshTag);
    std::shared_ptr<ResponseRefreshTagDo> rrtd = ResponseRefreshTagDo::BerTlvToResponseRefreshTagDo(bt);
    return rrtd->GetRefreshTag();
}

std::vector<RefArDo> AccessRuleApplicationController::GetAllAccessRules(std::shared_ptr<OHOS::se::SeChannel> channel)
{
    InfoLog("AccessRuleApplicationController::GetAllAccessRules");
    // GET DATA [All]: Fetches the first bytes of the Response-ALL-AR-DO
    std::string responseAll = this->GetDataAll(channel);
    std::shared_ptr<BerTlv> bt = BerTlv::StrToBerTlv(responseAll);
    int valueLength = bt->GetValue().length();
    while (valueLength < bt->GetLength()) {
        // GET DATA [Next]: Fetches the next (succeeding) bytes of the Response-ALL-AR-DO
        std::string responseNext = this->GetDataNext(channel);
        responseAll += responseNext;
        valueLength += responseNext.length();
    }
    bt = BerTlv::StrToBerTlv(responseAll);
    std::shared_ptr<ResponseAllArDo> raad = ResponseAllArDo::BerTlvToResponseAllArDo(bt);
    return raad->GetRefArDoArray();
}

std::string AccessRuleApplicationController::GetDataRefreshTag(std::shared_ptr<OHOS::se::SeChannel> channel)
{
    DebugLog("GET DATA [Refresh Tag]");
    std::string response = channel->Transmit(GET_DATA_REFRESH_TAG);
    if (this->ResponseIsSuccess(response)) {
        return response.substr(0, response.length() - 2);
    }
    ErrorLog("GET DATA [Refresh Tag] not success");
    throw AccessControlError("GET DATA [Refresh Tag] not success");
}

std::string AccessRuleApplicationController::GetDataAll(std::shared_ptr<OHOS::se::SeChannel> channel)
{
    DebugLog("GET DATA [All]");
    std::string response = channel->Transmit(GET_DATA_ALL);
    if (this->ResponseIsSuccess(response)) {
        return response.substr(0, response.length() - 2);
    }
    ErrorLog("GET DATA [All] not success");
    throw AccessControlError("GET DATA [All] not success");
}

std::string AccessRuleApplicationController::GetDataNext(std::shared_ptr<OHOS::se::SeChannel> channel)
{
    DebugLog("GET DATA [Next]");
    std::string response = channel->Transmit(GET_DATA_NEXT);
    if (this->ResponseIsSuccess(response)) {
        return response.substr(0, response.length() - 2);
    }
    ErrorLog("GET DATA [Next] not success");
    throw AccessControlError("GET DATA [Next] not success");
}

bool AccessRuleApplicationController::ResponseIsSuccess(std::string response)
{
    if (response.size() < 2) {
        return false;
    }
    int sw1 = response.at(response.length() - 2);
    int sw2 = response.at(response.length() - 1);
    int status = (sw1 << 8) | sw2;
    if (status == 0x9000) {
        DebugLog("Response is success");
        return true;
    } else {
        DebugLog("Reponse is not success, status=0x%04X", status);
        return false;
    }
}
}  // namespace OHOS::se::security