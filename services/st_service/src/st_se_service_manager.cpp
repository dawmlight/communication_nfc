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
#include "st_se_service_manager.h"

#include <string>
#include <thread>

#include "loghelper.h"
#include "st_se_service.h"
#include "system_ability_definition.h"

namespace OHOS {
namespace hardware {
namespace se {
namespace v1_0 {
const bool REGISTER_RESULT =
    SystemAbility::MakeAndRegisterAbility(DelayedSingleton<StSeServiceManager>::GetInstance().get());

StSeServiceManager::StSeServiceManager() : SystemAbility(ST_ESE1_SYS_ABILITY_ID, true) {}

StSeServiceManager::~StSeServiceManager()
{
    stSeService_ = nullptr;
}

void StSeServiceManager::OnStart()
{
    if (state_ == ServiceRunningState::STATE_RUNNING) {
        InfoLog("StSeServiceManager has already started.");
        return;
    }

    if (!Init()) {
        ErrorLog("failed to init StSeServiceManager");
        return;
    }
    state_ = ServiceRunningState::STATE_RUNNING;
    DebugLog("StSeServiceManager::OnStart start service success.");
}

bool StSeServiceManager::Init()
{
    DebugLog("StSeServiceManager::Init ready to init.");
    if (!registerToService_) {
        stSeService_ = new StSeService();

        sptr<ISystemAbilityManager> systemAbilityMgr =
            SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
        if (systemAbilityMgr && (systemAbilityMgr->AddSystemAbility(ST_ESE1_SYS_ABILITY_ID, stSeService_) == 0)) {
        } else {
            InfoLog("StSeServiceManager::Init Add System Ability failed!");
            return false;
        }
        registerToService_ = true;
    }
    DebugLog("StSeServiceManager::Init init success.");
    return true;
}

void StSeServiceManager::OnStop()
{
    DebugLog("StSeServiceManager::OnStop ready to stop service.");
    state_ = ServiceRunningState::STATE_NOT_START;
    registerToService_ = false;
    DebugLog("StSeServiceManager::OnStop stop service success.");
}
}  // namespace v1_0
}  // namespace se
}  // namespace hardware
}  // namespace OHOS
