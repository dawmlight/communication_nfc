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

#ifndef CARDEMULATION_AGENT_STUB_H
#define CARDEMULATION_AGENT_STUB_H
#include <functional>
#include <map>

#include "icard_emulation_agent.h"
#include "iremote_stub.h"

namespace OHOS::nfc::cardemulation {
class CardEmulationAgentStub : public OHOS::IRemoteStub<OHOS::nfc::sdk::cardemulation::ICardEmulationAgent> {
public:
    ~CardEmulationAgentStub() override = default;

    int OnRemoteRequest(uint32_t code,
                        OHOS::MessageParcel& data,
                        OHOS::MessageParcel& reply,
                        OHOS::MessageOption& option) override;

    void Init();

private:
    void OnGetServices(OHOS::MessageParcel& data, OHOS::MessageParcel& reply);
    void OnGetAidsForService(OHOS::MessageParcel& data, OHOS::MessageParcel& reply);
    void OnRegisterRemoteObject(OHOS::MessageParcel& data, OHOS::MessageParcel& repl);
    void OnAddAidsForService(OHOS::MessageParcel& data, OHOS::MessageParcel& reply);
    void OnRemoveAidsForService(OHOS::MessageParcel& data, OHOS::MessageParcel& reply);
    void OnRegisterOffHostService(OHOS::MessageParcel& data, OHOS::MessageParcel& reply);
    void OnUnregisterOffHostService(OHOS::MessageParcel& data, OHOS::MessageParcel& reply);
    void OnIsSupportedAidPrefixMode(OHOS::MessageParcel& data, OHOS::MessageParcel& reply);
    void OnGetDescriptionForPreferredService(OHOS::MessageParcel& data, OHOS::MessageParcel& reply);
    void OnSetPreferredForegroundService(OHOS::MessageParcel& data, OHOS::MessageParcel& reply);
    void OnUnsetPreferredForegroundService(OHOS::MessageParcel& data, OHOS::MessageParcel& reply);
    void OnGetPrimaryServiceForType(OHOS::MessageParcel& data, OHOS::MessageParcel& reply);
    void OnSetPrimaryServiceForType(OHOS::MessageParcel& data, OHOS::MessageParcel& reply);

protected:
    void WriteHeader(OHOS::MessageParcel& reply, int statusCode) const;
    bool CheckHeader(OHOS::MessageParcel& data) const;

private:
    std::unique_ptr<ICardEmulationAgent> impl_;
    using handler_t = std::function<void(OHOS::MessageParcel& data, OHOS::MessageParcel& reply)>;
    std::unordered_map<uint32_t, handler_t> handlers_;
};
}  // namespace OHOS::nfc::cardemulation
#endif  // !CARDEMULATION_AGENT_STUB_H
