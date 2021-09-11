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
#include "se_end_service.h"

#include <string>

#include "bundle_manager.h"
#include "context.h"
#include "loghelper.h"
#include "se_common_exception.h"
#include "secure_element_service.h"
#include "terminal.h"

using osal::Context;
namespace OHOS {
namespace se {
SeEndService::~SeEndService()
{
    DebugLog("SeEndService::OnDestroy");
    DestructionTerminals();
    if (mSecureElementService_) {
        mSecureElementService_ = nullptr;
    }
}

bool SeEndService::Init(std::shared_ptr<Context> context)
{
    InfoLog("2=SeEndService::Init ready to init.");
    context_ = context;
    // Establish Terminals
    EstablishTerminals();
    if (mSecureElementService_ == nullptr) {
        // Create Secure Element Service
        mSecureElementService_ = new SecureElementService(shared_from_this(), mTerminals_);
    }
    InfoLog("SeEndService::Init success.");
    return true;
}

std::weak_ptr<osal::BundleManager> SeEndService::GetBundleManager()
{
    return context_->GetBundleManager();
}

std::string SeEndService::GetBundleNameFromCallingUid(int uid)
{
    std::weak_ptr<osal::BundleManager> bundleManager = context_->GetBundleManager();
    if (!bundleManager.expired()) {
        std::vector<std::string> bundleNames = bundleManager.lock()->GetBundlesForUid(uid);
        if (!bundleNames.empty()) {
            return bundleNames[0];
        }
    }
    throw AccessControlError("BundleName can not be determined");
}

/**
 * @brief Destruction All Terminals
 */
void SeEndService::DestructionTerminals()
{
    for (IdTerminalPair terminal : mTerminals_) {
        terminal.second->CloseChannels();
        terminal.second->Close();
    }
}

/**
 * @brief Establish All SE Hal Terminals
 */
void SeEndService::EstablishTerminals()
{
    AddTerminal(ESE_TERMINAL);
    AddTerminal(UICC_TERMINAL);
}

/**
 * @brief Establish the special the SE Terminal
 * @param terminalName the terminal name
 */
void SeEndService::AddTerminal(const std::string& terminalName)
{
    int index{1};
    std::string name{};
    DebugLog("AddTerminal Of %s", terminalName.c_str());
    try {
        do {
            name = terminalName + std::to_string(index);
            std::shared_ptr<Terminal> terminal = std::make_shared<Terminal>(name, context_);

            // Only retry on fail for the first terminal of each type.
            terminal->Initialize();
            mTerminals_.push_back(std::make_pair(name, terminal));
        } while (++index > 0);
    } catch (const NoSuchElementError& e) {
        InfoLog("No HAL implementation for %s", name.c_str());
    } catch (const std::exception& e) {
        ErrorLog("Error in getService() for %s", name.c_str());
    }
}
}  // namespace se
}  // namespace OHOS