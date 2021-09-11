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

#include "ability_connection_stub.h"

namespace OHOS::nfc::cardemulation {
AbilityConnectionStub::AbilityConnectionStub(FuncOnServiceConnect onConnect, FuncOnServiceDisconnect onDisconnect)
    : connectCallback_(std::move(onConnect)),
    disconnectCallback_(std::move(onDisconnect))
{
}
void AbilityConnectionStub::OnAbilityConnectDone(const OHOS::AppExecFwk::ElementName& element,
                                                 const OHOS::sptr<OHOS::IRemoteObject>& remoteObject,
                                                 int resultCode)
{
    if (connectCallback_) {
        connectCallback_(element, remoteObject);
    }
}
void AbilityConnectionStub::OnAbilityDisconnectDone(const AppExecFwk::ElementName& element, int resultCode)
{
    if (disconnectCallback_) {
        disconnectCallback_(element);
    }
}
}  // namespace OHOS::nfc::cardemulation
