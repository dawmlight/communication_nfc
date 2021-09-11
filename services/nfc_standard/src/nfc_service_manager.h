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

#ifndef OHOS_NFC_SERVICE_MANAGER_H
#define OHOS_NFC_SERVICE_MANAGER_H
#include <memory>
#include <mutex>
#include <cstring>
#include <map>
#include <set>

#include "singleton.h"
#include "nocopyable.h"
#include "iremote_object.h"
#include "system_ability.h"
#include "iservice_registry.h"

namespace osal{
class Context;
}

namespace OHOS {
namespace nfc {
enum ServiceRunningState {
    STATE_NOT_START,
    STATE_RUNNING
};
class NfcService;
class NfcAgentService;

class NfcServiceManager : public SystemAbility {
    DECLARE_DELAYED_SINGLETON(NfcServiceManager)
    DECLARE_SYSTEM_ABILITY(NfcServiceManager); // necessary
public:
    DISALLOW_COPY_AND_MOVE(NfcServiceManager);
    
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

    std::shared_ptr<osal::Context> context_;
    std::shared_ptr<NfcService> nfcService_;
    sptr<NfcAgentService> nfcAgentService_;
};
} // namespace nfc
} // namespace OHOS
#endif // OHOS_NFC_SERVICE_MANAGER_H