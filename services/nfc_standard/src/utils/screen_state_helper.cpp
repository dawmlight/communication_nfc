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
#include "utils/screen_state_helper.h"

#include "context.h"
#include "key_guard_manager.h"
#include "power_manager.h"

using namespace osal;
namespace OHOS{
namespace nfc{
ScreenStateHelper::ScreenStateHelper(std::weak_ptr<Context> ctx)
{
    mPowerManager_ = ctx.lock()->GetPowerManagerService();
    mKeyguardManager_ = ctx.lock()->GetKeyguardManagerService();
}

ScreenStateHelper::~ScreenStateHelper() {}

int ScreenStateHelper::CheckScreenState() const
{
    if (!mPowerManager_.lock()->IsScreenOn()) {
        if (mKeyguardManager_.lock()->IsKeyguardLocked()) {
            return SCREEN_STATE_OFF_LOCKED;
        }
        return SCREEN_STATE_OFF_UNLOCKED;
    }
    if (mKeyguardManager_.lock()->IsKeyguardLocked()) {
        return SCREEN_STATE_ON_LOCKED;
    }
    return SCREEN_STATE_ON_UNLOCKED;
}
}  // namespace nfc
}  // namespace OHOS