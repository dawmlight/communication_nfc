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
#include "access_control_enforcer.h"

#include "access_rule_application_controller.h"
#include "access_rule_cache.h"
#include "access_rule_files_controller.h"
#include "bundle_manager.h"
#include "channel_access_rule.h"
#include "loghelper.h"
#include "openssl/sha.h"
#include "osal_bundle_info.h"
#include "se_channel.h"
#include "se_common_exception.h"
#include "terminal.h"

namespace OHOS::se::security {
AccessControlEnforcer::AccessControlEnforcer(std::weak_ptr<Terminal> terminal)
    : terminal_(terminal),
      bundleManager_(std::shared_ptr<osal::BundleManager>()),
      accessRuleApplicationController_(std::shared_ptr<AccessRuleApplicationController>()),
      accessRuleFilesController_(std::shared_ptr<AccessRuleFilesController>()),
      accessRuleCache_(std::make_shared<AccessRuleCache>()),
      noRuleFound_(false),
      hasAra_(true),
      hasArf_(true),
      rulesRead_(false)
{
}

AccessControlEnforcer::~AccessControlEnforcer() {}

std::string AccessControlEnforcer::GetDefaultAccessControlAid()
{
    if (accessRuleApplicationController_) {
        return accessRuleApplicationController_->GetAccessControlAid();
    }
    return accessRuleApplicationController_->GetAraMAid();
}

std::weak_ptr<osal::BundleManager> AccessControlEnforcer::GetBundleManager()
{
    return bundleManager_;
}

void AccessControlEnforcer::SetBundleManager(std::weak_ptr<osal::BundleManager> bundleManager)
{
    bundleManager_ = bundleManager;
}

std::weak_ptr<Terminal> AccessControlEnforcer::GetTerminal()
{
    return terminal_;
}

std::weak_ptr<AccessRuleCache> AccessControlEnforcer::GetAccessRuleCache()
{
    return accessRuleCache_;
}

void AccessControlEnforcer::Reset()
{
    DebugLog("Reset access control enforcer");
    std::lock_guard<std::mutex> lock(mutex_);
    accessRuleCache_->SetRefreshTag("");
    accessRuleCache_->ClearAccessRules();
    accessRuleApplicationController_ = std::shared_ptr<AccessRuleApplicationController>();
    accessRuleFilesController_ = std::shared_ptr<AccessRuleFilesController>();
}

void AccessControlEnforcer::Initialize()
{
    DebugLog("AccessControlEnforcer::Initialize");
    std::lock_guard<std::mutex> lock(mutex_);
    hasAra_ = true;
    hasArf_ = true;
    noRuleFound_ = false;
    /*
     * Access Rule Application
     * When a device application attempts to access an SE application, the Access Control enforcer shall request
     * the pertinent rules from the ARA-M. The ARA-M shall provide the appropriate rules, whether they are stored
     * on the ARA-M, an ARA-C, or the ARF
     */
    if (hasAra_ && !accessRuleApplicationController_) {
        DebugLog("Get ARA");
        accessRuleApplicationController_ =
            std::make_shared<AccessRuleApplicationController>(accessRuleCache_, terminal_);
    }
    if (hasAra_ && accessRuleApplicationController_) {
        DebugLog("Init ARA");
        try {
            accessRuleApplicationController_->Initialize();
            hasArf_ = false;
        } catch (const NoSuchElementError& e) {
            ErrorLog("No ARA applet exist");
            hasAra_ = false;
            noRuleFound_ = true;
        } catch (const std::runtime_error& error) {
            hasAra_ = false;
        }
    }
    /*
     * Access Rule Files
     * For the UICC, the following fallback shall be implemented: If the ARA-M is not present, the Access Control
     * enforcer shall retrieve the access rules from the Access Rule Files (ARF)
     */
    if (hasArf_ && !accessRuleFilesController_) {
        DebugLog("Get ARF");
        accessRuleFilesController_ = std::make_shared<AccessRuleFilesController>(accessRuleCache_, terminal_);
    }
    if (hasArf_ && accessRuleFilesController_) {
        DebugLog("Init ARF");
        try {
            accessRuleFilesController_->Initialize();
        } catch (const NoSuchElementError& e) {
            ErrorLog("No ARF applet exist");
            hasArf_ = false;
            noRuleFound_ = true;
        } catch (const std::runtime_error& error) {
            hasArf_ = false;
        }
    }
    if (!hasAra_ && !hasArf_) {
        DebugLog("Neither ARA nor ARF, deny all access");
        rulesRead_ = false;
    }
    rulesRead_ = true;
}

bool AccessControlEnforcer::IsNoRuleFound()
{
    return noRuleFound_;
}

void AccessControlEnforcer::CheckCommand(std::weak_ptr<OHOS::se::SeChannel> channel, const std::string& command)
{
    DebugLog("AccessControlEnforcer::CheckCommand");
    std::shared_ptr<SeChannel> sChannel = channel.lock();
    if (!sChannel) {
        DebugLog("Channel has expired");
        throw AccessControlError("Channel has expired");
    }
    std::shared_ptr<ChannelAccessRule> channelAccessRule = sChannel->GetChannelAccess().lock();
    if (!channelAccessRule) {
        DebugLog("ChannelAccessRule has expired");
        throw AccessControlError("ChannelAccessRule has expired");
    }

    if (channelAccessRule->GetAccessRule() != ChannelAccessRule::ACCESSRULE::ALWAYS) {
        DebugLog("Access not allowed");
        throw AccessControlError("Access not allowed");
    }
    if (channelAccessRule->HasApduFilter()) {
        std::vector<std::string> apduFilters = channelAccessRule->GetApduFilters();
        if (apduFilters.size() == 0) {
            DebugLog("Access rule not match");
            throw AccessControlError("Access rule not match");
        }
        for (std::string apduFilter : apduFilters) {
            if (apduFilter.length() == 8) {
                if ((((command[0] & 0xFF) & (apduFilter[4] & 0xFF)) == apduFilter[0]) &&
                    (((command[1] & 0xFF) & (apduFilter[5] & 0xFF)) == apduFilter[1]) &&
                    (((command[2] & 0xFF) & (apduFilter[6] & 0xFF)) == apduFilter[2]) &&
                    (((command[3] & 0xFF) & (apduFilter[7] & 0xFF)) == apduFilter[3])) {
                    DebugLog("Command matched apdu filter");
                    return;
                }
            }
        }
        throw AccessControlError("Access rule not match");
    }
    if (channelAccessRule->GetApduAccessRule() != ChannelAccessRule::ACCESSRULE::ALWAYS) {
        DebugLog("Apdu access not allowed");
        throw AccessControlError("Apdu access not allowed");
    }
    DebugLog("Access allowed");
    return;
}

std::shared_ptr<ChannelAccessRule> AccessControlEnforcer::EstablishChannelAccessRuleForBundle(
    const std::string& aid, const std::string& bundleName, bool checkRefreshTag)
{
    DebugLog("AccessControlEnforcer::EstablishChannelAccessRuleForBundle");
    std::lock_guard<std::mutex> lock(mutex_);
    std::shared_ptr<ChannelAccessRule> channelAccessRule = std::shared_ptr<ChannelAccessRule>();
    if (hasAra_ || hasArf_) {
        if (bundleName.empty()) {
            DebugLog("Bundle is null");
            throw AccessControlError("Bundle is null");
        }
        std::vector<std::string> appCertHashes = this->GetHashesFromBundle(bundleName);
        if (checkRefreshTag) {
            this->CheckRefreshTag();
        }
        channelAccessRule = this->GetAccessRule(aid, appCertHashes);
    }
    if (!channelAccessRule) {
        DebugLog("No apdu access allowed");
        channelAccessRule = std::make_shared<ChannelAccessRule>();
        channelAccessRule->SetAccessRule(ChannelAccessRule::ACCESSRULE::NEVER, "No apdu access allowed");
        channelAccessRule->SetApduAccessRule(ChannelAccessRule::ACCESSRULE::NEVER);
        channelAccessRule->SetNFCEventAccessRule(ChannelAccessRule::ACCESSRULE::NEVER);
    }
    channelAccessRule->SetBundleName(bundleName);
    return channelAccessRule;
}

std::shared_ptr<ChannelAccessRule> AccessControlEnforcer::GetAccessRule(const std::string& aid,
                                                                        std::vector<std::string> appCertHashes)
{
    DebugLog("AccessControlEnforcer::GetAccessRule");
    ChannelAccessRule channelAccessRule;
    if (rulesRead_) {
        channelAccessRule = accessRuleCache_->GetAccessRule(aid, appCertHashes);
    } else {
        channelAccessRule = ChannelAccessRule();
        channelAccessRule.SetAccessRule(ChannelAccessRule::ACCESSRULE::NEVER, "No access rule found");
        channelAccessRule.SetApduAccessRule(ChannelAccessRule::ACCESSRULE::NEVER);
        channelAccessRule.SetNFCEventAccessRule(ChannelAccessRule::ACCESSRULE::NEVER);
    }
    return std::make_shared<ChannelAccessRule>(channelAccessRule);
}

std::vector<bool> AccessControlEnforcer::IsNfcEventAllowed(const std::string& aid,
                                                           const std::vector<std::string>& bundleNames)
{
    DebugLog("AccessControlEnforcer::IsNfcEventAllowed");
    std::vector<bool> nfcEventAllowed;
    if (hasAra_ || hasArf_) {
        for (std::string bundleName : bundleNames) {
            std::vector<std::string> appCertHashes = this->GetHashesFromBundle(bundleName);
            std::shared_ptr<ChannelAccessRule> channelAccess = GetAccessRule(aid, appCertHashes);
            nfcEventAllowed.push_back(
                (channelAccess->GetNFCEventAccessRule() == ChannelAccessRule::ACCESSRULE::ALWAYS));
        }
    } else {
        for (std::string bundleName : bundleNames) {
            nfcEventAllowed.push_back(false);
        }
    }
    return nfcEventAllowed;
}

bool AccessControlEnforcer::CheckCarrierPrivilege(std::weak_ptr<osal::BundleInfo> bInfo, bool checkRefreshTag)
{
    DebugLog("AccessControlEnforcer::CheckCarrierPrivilege");
    std::lock_guard<std::mutex> lock(mutex_);
    if (!hasAra_ && !hasArf_) {
        return false;
    }
    if (checkRefreshTag) {
        this->CheckRefreshTag();
    }
    if (rulesRead_) {
        return false;
    }
    std::string bundleName;
    {
        std::shared_ptr<osal::BundleInfo> bundleInfo = bInfo.lock();
        if (!bundleInfo) {
            DebugLog("Bundle info has expired");
            throw AccessControlError("Bundle info has expired");
        }
        bundleName = bundleInfo->mBundleName_;
    }
    std::vector<std::string> hashes = this->GetHashesFromBundle(bundleName);
    if (hashes.size() == 0) {
        return false;
    }
    return accessRuleCache_->CheckCarrierPrivilege(bundleName, hashes);
}

std::vector<std::string> AccessControlEnforcer::GetHashesFromBundle(const std::string& bundleName)
{
    DebugLog("AccessControlEnforcer::GetHashesFromBundle");
    if (bundleName.empty()) {
        DebugLog("Bundle name is empty");
        throw AccessControlError("Bundle name is null");
    }
    std::shared_ptr<osal::BundleManager> bundleManager = bundleManager_.lock();
    if (!bundleManager) {
        DebugLog("Bundle manager has expired");
        throw AccessControlError("Bundle magager has expired");
    }
    std::shared_ptr<osal::BundleInfo> bundleInfo =
        bundleManager->GetBundleInfo(bundleName, osal::BundleManager::GET_SIGNATURES);
    if (!bundleInfo) {
        DebugLog("Bundle info is empty");
        throw AccessControlError("Bundle info does not exist");
    }
    std::vector<std::string> hashes;
    for (std::string signature : bundleInfo->signatures_) {
        // OpenSSL
        // SHA-1
        unsigned char hash1[SHA_DIGEST_LENGTH] = "";
        SHA_CTX ctx1;
        SHA1_Init(&ctx1);
        SHA1_Update(&ctx1, signature.c_str(), signature.size());
        SHA1_Final(hash1, &ctx1);
        hashes.push_back(UnsignedCharArrayToString(hash1, SHA_DIGEST_LENGTH));
        // SHA-256
        unsigned char hash256[SHA256_DIGEST_LENGTH] = "";
        SHA256_CTX ctx256;
        SHA256_Init(&ctx256);
        SHA256_Update(&ctx256, signature.c_str(), signature.size());
        SHA256_Final(hash256, &ctx256);
        hashes.push_back(UnsignedCharArrayToString(hash256, SHA256_DIGEST_LENGTH));
    }
    return hashes;
}

void AccessControlEnforcer::CheckRefreshTag()
{
    DebugLog("AccessControlEnforcer::CheckRefreshTag");
    if (hasAra_ && accessRuleApplicationController_) {
        accessRuleApplicationController_->Initialize();
        hasArf_ = false;
    } else if (hasArf_ && accessRuleFilesController_) {
        accessRuleFilesController_->Initialize();
    }
};

std::string AccessControlEnforcer::UnsignedCharArrayToString(const unsigned char* charArray, int length)
{
    std::string result = "";
    for (int i = 0; i < length; i++) {
        result += charArray[i];
    }
    return result;
}
}  // namespace OHOS::se::security