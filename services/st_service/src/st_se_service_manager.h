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
#ifndef OHOS_ST_SE_SERVICE_MANAGER_H
#define OHOS_ST_SE_SERVICE_MANAGER_H
#include <memory>

#include "iremote_object.h"
#include "iservice_registry.h"
#include "nocopyable.h"
#include "singleton.h"
#include "system_ability.h"

namespace OHOS {
namespace hardware {
namespace se {
namespace v1_0 {
enum ServiceRunningState { STATE_NOT_START, STATE_RUNNING };
class StSeService;

class StSeServiceManager : public SystemAbility {
    DECLARE_DELAYED_SINGLETON(StSeServiceManager)
    DECLARE_SYSTEM_ABILITY(StSeServiceManager);  // necessary
public:
    DISALLOW_COPY_AND_MOVE(StSeServiceManager);

    /* Nfc open or close operations */
    void OnStart() override;
    void OnStop() override;

    /* query service state */
    ServiceRunningState QueryServiceState() const
    {
        return state_;
    }

private:
    bool Init();
    bool registerToService_ = false;
    ServiceRunningState state_ = ServiceRunningState::STATE_NOT_START;

    sptr<StSeService> stSeService_;
};
}  // namespace v1_0
}  // namespace se
}  // namespace hardware
}  // namespace OHOS
#endif  // OHOS_NFC_SERVICE_MANAGER_H