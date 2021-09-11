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

#ifndef INFC_AGENT_SERVICE_MOCK_H
#define INFC_AGENT_SERVICE_MOCK_H

#include <gmock/gmock.h>

#include "infc_agent_service.h"
#include "iremote_object.h"

namespace OHOS {
namespace nfc {
namespace reader {
class ITagSession;
}

namespace sdk {
class NfcAgent;
class IAppCallback;

class INfcAgentServiceMock : public OHOS::IRemoteStub<INfcAgentService> {
public:
    INfcAgentServiceMock()
    {
    }
    MOCK_METHOD0(GetState, int());
    MOCK_METHOD0(TurnOn, bool());
    MOCK_METHOD1(TurnOff, bool(bool saveState));
    MOCK_METHOD1(PausePolling, void(int timeoutInMs));
    MOCK_METHOD0(ResumePolling, void());
    MOCK_METHOD1(SetNfcSecure, bool(bool enable));
    MOCK_METHOD0(DeviceSupportsNfcSecure, bool());
    MOCK_METHOD0(IsNfcSecureEnabled, bool());
    MOCK_METHOD4(SetRWMode,
                 void(const sptr<IRemoteObject>& readerObj,
                      std::weak_ptr<OHOS::nfc::sdk::IAppCallback> appCallback,
                      int flags,
                      std::weak_ptr<sdk::NfcMap> extras));
    MOCK_METHOD3(SetIgnore,
                 bool(int handle, int debounceMs, std::weak_ptr<OHOS::nfc::sdk::ITagRemovedCallback> callback));
    MOCK_METHOD0(SetFrontDispatch, void());
    MOCK_METHOD0(GetNfcTagInterface, OHOS::sptr<reader::ITagSession>());
#ifdef _NFC_SERVICE_HCE_
    MOCK_METHOD0(GetNfcCardEmulationInterface, OHOS::sptr<cardemulation::ICardEmulationAgent>());
#ifdef _NFC_F_CE_
    MOCK_METHOD0(GetNfcFCardEmulationInterface, OHOS::sptr<OHOS::nfc::sdk::cardemulation::INfcFCardEmulation>());
#endif
#endif

    MOCK_METHOD0(AsObject, sptr<IRemoteObject>());
};
}  // namespace sdk
}  // namespace nfc
}  // namespace OHOS
#endif  // !INFC_AGENT_SERVICE_MOCK_H
