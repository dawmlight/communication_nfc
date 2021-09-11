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

#ifndef NFC_SERVICE_CARD_EMULATION_ABILITY_CONNECTION_STUB_H
#define NFC_SERVICE_CARD_EMULATION_ABILITY_CONNECTION_STUB_H

#include <functional>
#include <memory>

#include "ability_connect_callback_interface.h"
#include "iremote_stub.h"

namespace OHOS {
namespace nfc::cardemulation {
using FuncOnServiceConnect = std::function<void(const OHOS::AppExecFwk::ElementName& element,
                                                const OHOS::sptr<OHOS::IRemoteObject>& remoteObject)>;

using FuncOnServiceDisconnect = std::function<void(const AppExecFwk::ElementName& element)>;

class AbilityConnectionStub : public OHOS::IRemoteStub<OHOS::AAFwk::IAbilityConnection> {
public:
    AbilityConnectionStub(FuncOnServiceConnect onConnect, FuncOnServiceDisconnect onDisconnect);
    void OnAbilityConnectDone(const OHOS::AppExecFwk::ElementName& element,
                              const OHOS::sptr<OHOS::IRemoteObject>& remoteObject,
                              int resultCode) override;
    void OnAbilityDisconnectDone(const AppExecFwk::ElementName& element, int resultCode) override;

private:
    FuncOnServiceConnect connectCallback_;
    FuncOnServiceDisconnect disconnectCallback_;
};
}  // namespace nfc::cardemulation
}  // namespace OHOS

#endif  // !NFC_SERVICE_CARD_EMULATION_ABILITY_CONNECTION_STUB_H
