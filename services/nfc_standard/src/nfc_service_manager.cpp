/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "nfc_service_manager.h"

#include <ctime>
#include <iostream>
#include <string>
#include <thread>

#include "context.h"
#include "loghelper.h"
#include "nfc_agent_service.h"
#include "nfc_service.h"
#include "system_ability_definition.h"

namespace OHOS {
namespace nfc {
const bool REGISTER_RESULT =
    SystemAbility::MakeAndRegisterAbility(DelayedSingleton<NfcServiceManager>::GetInstance().get());

NfcServiceManager::NfcServiceManager() : SystemAbility(NFC_MANAGER_SYS_ABILITY_ID + 10, true) {}

NfcServiceManager::~NfcServiceManager()
{
    if (nfcService_) {
        nfcService_ = nullptr;
    }
    if (context_) {
        context_ = nullptr;
    }
}

void NfcServiceManager::OnStart()
{
    if (state_ == ServiceRunningState::STATE_RUNNING) {
        InfoLog("NfcServiceManager has already started.");
        return;
    }

    if (!Init()) {
        InfoLog("failed to init NfcServiceManager");
        return;
    }
    state_ = ServiceRunningState::STATE_RUNNING;
    InfoLog("NfcServiceManager::OnStart start service success.");
}

bool NfcServiceManager::Init()
{
    InfoLog("NfcServiceManager::Init ready to init.");
    if (!registerToService_) {
        context_ = std::make_shared<osal::Context>();
        nfcService_ = std::make_shared<NfcService>(context_);
        nfcService_->Initialize();

        sptr<ISystemAbilityManager> systemAbilityMgr =
            SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
        if (systemAbilityMgr &&
            (systemAbilityMgr->AddSystemAbility(NFC_MANAGER_SYS_ABILITY_ID + 10, nfcService_->mNfcAgenter_) == 0)) {
        } else {
            InfoLog("NfcServiceManager::Init Add System Ability failed!");
            return false;
        }
        registerToService_ = true;
    }
    InfoLog("NfcServiceManager::Init init success.");
    return true;
}

void NfcServiceManager::OnStop()
{
    InfoLog("NfcServiceManager::OnStop ready to stop service.");
    state_ = ServiceRunningState::STATE_NOT_START;
    registerToService_ = false;
    InfoLog("NfcServiceManager::OnStop stop service success.");
}
}  // namespace nfc
}  // namespace OHOS
