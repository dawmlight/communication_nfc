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
#include "access_rule_files_controller.h"

#include "access_rule_cache.h"
#include "access_rule_files_channel.h"
#include "channel_access_rule.h"
#include "loghelper.h"
#include "pkcs15/access_control_main.h"
#include "pkcs15/access_control_rules.h"
#include "pkcs15/dir.h"
#include "pkcs15/dodf.h"
#include "pkcs15/odf.h"
#include "se_channel.h"
#include "se_common_exception.h"
#include "terminal.h"

namespace OHOS::se::security {
AccessRuleFilesController::AccessRuleFilesController(std::weak_ptr<AccessRuleCache> accessRuleCache,
                                                     std::weak_ptr<OHOS::se::Terminal> terminal)
    : accessRuleCache_(accessRuleCache),
      terminal_(terminal),
      arfChannel_(std::make_shared<AccessRuleFilesChannel>()),
      accessControlMain_(std::shared_ptr<AccessControlMain>()),
      accessControlRule_(std::shared_ptr<AccessControlRules>()),
      accessControlMainPath_(""),
      pkcs15Path_(""),
      acmfFound_(true)
{
}

AccessRuleFilesController::~AccessRuleFilesController() {}

void AccessRuleFilesController::Initialize()
{
    DebugLog("AccessRuleFilesController Initialize");
    this->GetAcmf();
    if (accessControlMain_) {
        this->UpdateAccessControlRules();
    } else {
        DebugLog("Get access control main file fail");
        throw AccessControlError("Access rule file init fail");
    }
    if (arfChannel_) {
        arfChannel_->Close();
    }
}

void AccessRuleFilesController::GetAcmf()
{
    DebugLog("AccessRuleFilesController GetAcmfPath");
    /*
     * Step 1: the device sends a SELECT_BY_NAME command with PKCS#15 AID (A0 00 00 00 63 50 4B 43 53
     * 2D 31 35). If the select is successful, the device can start reading PKCS#15 files (ODF, DODF…)
     * Step 2: if the previous select fails, the device sends SELECT commands to select the MF and the EF DIR,
     * and then reads the EF DIR in order to locate an entry with the PKCS#15 AID. If a matching entry is found,
     * the device must select the PKCS#15 DF path, and then it can start reading PKCS#15 files (ODF, DODF…)
     */
    std::vector<std::string> aidArray;
    aidArray.push_back(PKCS15_AID);
    aidArray.push_back(GPAC_AID);
    aidArray.push_back(EMPTY_AID);
    for (std::string aid : aidArray) {
        bool selectResult = this->SelectAccessControlRules(aid);
        if (selectResult == false) {
            continue;
        }

        std::string accessControlMainPath = "";
        if (accessControlMainPath_.empty()) {
            ODF odf = ODF(arfChannel_);
            std::string dodfPath = odf.ReadDataFromPath(pkcs15Path_);
            DODF dodf = DODF(arfChannel_);
            accessControlMainPath = dodf.ReadDataFromPath(dodfPath);
            accessControlMainPath_ = accessControlMainPath;
        } else {
            if (pkcs15Path_.empty()) {
                accessControlMainPath = accessControlMainPath_;
            } else {
                accessControlMainPath = pkcs15Path_ + accessControlMainPath_;
            }
        }
        accessControlMain_ = std::make_shared<AccessControlMain>(arfChannel_, accessRuleCache_, accessControlMainPath);
        break;
    }
}

bool AccessRuleFilesController::SelectAccessControlRules(std::string aid)
{
    DebugLog("AccessRuleFilesController SelectAccessRules");
    std::shared_ptr<Terminal> terminal = terminal_.lock();
    if (!terminal) {
        DebugLog("Terminal has expired");
        throw AccessControlError("Terminal has expired");
    }
    std::shared_ptr<OHOS::se::SeChannel> channel = terminal->OpenLogicalChannelForAccessControl(aid);
    if (!channel) {
        ErrorLog("Channel open failed");
        return false;
    }
    std::shared_ptr<ChannelAccessRule> channelAccess = std::make_shared<ChannelAccessRule>();
    channelAccess->SetAccessRule(ChannelAccessRule::ACCESSRULE::ALWAYS, "");
    channelAccess->SetApduAccessRule(ChannelAccessRule::ACCESSRULE::ALWAYS);
    channel->SetChannelAccess(channelAccess);
    arfChannel_->SetChannel(channel);

    if (aid.empty()) {
        if (pkcs15Path_.empty()) {
            accessControlMainPath_ = "";
            DIR dir = DIR(arfChannel_);
            pkcs15Path_ = dir.ReadDataFromAid(PKCS15_AID);
            if (pkcs15Path_.empty()) {
                DebugLog("Select PKCSC#15 fail");
                return false;
            }
        }
    } else {
        if (!pkcs15Path_.empty()) {
            accessControlMainPath_ = "";
        }
        pkcs15Path_ = "";
    }
    return true;
}

bool AccessRuleFilesController::UpdateAccessControlRules()
{
    DebugLog("AccessRuleFilesController UpdateAccessControlRules");
    std::shared_ptr<AccessRuleCache> accessRuleCache = accessRuleCache_.lock();
    if (!accessRuleCache) {
        DebugLog("AccessRuleCache has expired");
        throw AccessControlError("AccessRuleCache has expired");
    }

    if (!acmfFound_) {
        accessRuleCache->SetRefreshTag("");
        accessRuleCache->ClearAccessRules();
        accessControlMainPath_ = "";
        if (arfChannel_) {
            arfChannel_->Close();
        }
        this->GetAcmf();
    }

    std::string accessControlRulesPath;
    try {
        accessControlRulesPath = accessControlMain_->ReadDataFromPath();
        acmfFound_ = true;
    } catch (std::exception& e) {
        DebugLog("ACMF not found");
        acmfFound_ = false;
        accessRuleCache->SetRefreshTag("");
        accessRuleCache->ClearAccessRules();
        throw e;
    }

    if (!accessControlRulesPath.empty()) {
        DebugLog("Need update access rules");
        if (!accessControlRule_) {
            accessControlRule_ = std::make_shared<AccessControlRules>(arfChannel_, accessRuleCache);
        }
        accessRuleCache->ClearAccessRules();
        accessControlMainPath_ = "";
        if (arfChannel_) {
            arfChannel_->Close();
        }
        this->GetAcmf();
        try {
            accessControlRule_->ReadDataFromPath(accessControlRulesPath);
        } catch (std::exception& e) {
            DebugLog("Error, clear access rule cache");
            accessRuleCache->SetRefreshTag("");
            accessRuleCache->ClearAccessRules();
            throw e;
        }
        return true;
    } else {
        DebugLog("Refresh tag not changed");
        return false;
    }
}
}  // namespace OHOS::se::security