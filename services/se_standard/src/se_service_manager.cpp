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
#include "se_service_manager.h"

#include <string>

#include "context.h"
#include "loghelper.h"
#include "se_end_service.h"
#include "system_ability_definition.h"

namespace OHOS {
namespace se {
const bool REGISTER_RESULT =
    SystemAbility::MakeAndRegisterAbility(DelayedSingleton<SeServiceManager>::GetInstance().get());

SeServiceManager::SeServiceManager() : SystemAbility(SE_MANAGER_SYS_ABILITY_ID, true) {}

SeServiceManager::~SeServiceManager()
{
    seService_ = std::shared_ptr<SeEndService>();
}

void SeServiceManager::OnStart()
{
    if (state_ == ServiceRunningState::STATE_RUNNING) {
        InfoLog("SeServiceManager has already started.");
        return;
    }

    if (!Init()) {
        ErrorLog("failed to init SeServiceManager");
        return;
    }
    state_ = ServiceRunningState::STATE_RUNNING;
    DebugLog("SeServiceManager::OnStart start service success.");
}

void SeServiceManager::OnStop()
{
    DebugLog("SeServiceManager::OnDestroy");
    if (seService_) {
        seService_ = std::shared_ptr<SeEndService>();
    }
}

bool SeServiceManager::Init()
{
    DebugLog("1=SeServiceManager::Init ready to init.");
    if (!registerToService_) {
        std::shared_ptr<osal::Context> context = std::make_shared<osal::Context>();
        seService_ = std::make_shared<SeEndService>();
        if (!seService_->Init(context)) {
            return false;
        }

        sptr<ISystemAbilityManager> systemAbilityMgr =
            SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
        if (systemAbilityMgr &&
            (systemAbilityMgr->AddSystemAbility(SE_MANAGER_SYS_ABILITY_ID, seService_->mSecureElementService_) == 0)) {
        } else {
            InfoLog("SeServiceManager::Init Add System Ability failed!");
            return false;
        }
        registerToService_ = true;
    }
    DebugLog("SeServiceManager::Init init success.");
    return true;
}
}  // namespace se
}  // namespace OHOS