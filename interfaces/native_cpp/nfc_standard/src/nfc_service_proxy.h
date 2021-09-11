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
#ifndef NFC_SERVICE_PROXY_H
#define NFC_SERVICE_PROXY_H

#include <memory>

#include "infc_agent_service.h"
#include "iremote_proxy.h"
#include "nfc_basic_proxy.h"

namespace OHOS {
namespace nfc {
namespace reader {
class ITagSession;
}  // namespace reader

namespace sdk {
class IAppCallback;
class ITagRemovedCallback;
class NfcMap;
}  // namespace sdk

using sdk::NfcMap;
class NfcServiceProxy final : public OHOS::IRemoteProxy<INfcAgentService>, public NfcBasicProxy {
public:
    explicit NfcServiceProxy(const OHOS::sptr<OHOS::IRemoteObject>& remote)
        : OHOS::IRemoteProxy<INfcAgentService>(remote), NfcBasicProxy(remote)
    {
    }
    ~NfcServiceProxy() override;

    int GetState() override;
    bool TurnOn() override;
    bool TurnOff(bool saveState) override;
    void PausePolling(int timeoutInMs) override;
    void ResumePolling() override;
    bool SetNfcSecure(bool enable) override;
    bool DeviceSupportsNfcSecure() override;
    bool IsNfcSecureEnabled() override;
#ifdef _NFC_SERVICE_HCE_
    OHOS::sptr<OHOS::nfc::sdk::cardemulation::ICardEmulationAgent> GetNfcCardEmulationInterface() override;
#ifdef _NFC_F_CE_
    OHOS::sptr<OHOS::nfc::sdk::cardemulation::INfcFCardEmulation> GetNfcFCardEmulationInterface() override;
#endif
#endif  // _NFC_SERVICE_HCE_

#ifdef _NFC_SERVICE_READER_
    void SetRWMode(const sptr<IRemoteObject>& readerObj,
                   std::weak_ptr<nfc::sdk::IAppCallback> appCallback,
                   int flags,
                   std::weak_ptr<NfcMap> extras) override;
    bool SetIgnore(int handle, int debounceMs, std::weak_ptr<nfc::sdk::ITagRemovedCallback> callback) override;
    void SetFrontDispatch() override;
    OHOS::sptr<reader::ITagSession> GetNfcTagInterface() override;
#endif  // _NFC_SERVICE_READER_

private:
    OHOS::sptr<nfc::reader::ITagSession> tagSession_{};

    static constexpr int UID_ID = 100;
    static constexpr int COMMAND_GET_STATE = UID_ID + 0;
    static constexpr int COMMAND_TURN_ON = UID_ID + 1;
    static constexpr int COMMAND_TURN_OFF = UID_ID + 2;
    static constexpr int COMMAND_PAUSE_POLLING = UID_ID + 3;
    static constexpr int COMMAND_RESUME_POLLING = UID_ID + 4;
    static constexpr int COMMAND_SET_NFC_SECURE = UID_ID + 5;
    static constexpr int COMMAND_DEVICE_SUPPORT_SECURE = UID_ID + 6;
    static constexpr int COMMAND_IS_SECURE_ENABLED = UID_ID + 7;
    static constexpr int COMMAND_SET_RW_MODE = UID_ID + 8;
    static constexpr int COMMAND_SET_IGNORE = UID_ID + 9;
    static constexpr int COMMAND_SET_FRONT_DISPATCH = UID_ID + 10;
    static constexpr int COMMAND_GET_TAG_INTERFACE = UID_ID + 11;
    static constexpr int COMMAND_GET_CARDEMULATION_INTERFACE = UID_ID + 12;
};
}  // namespace nfc
}  // namespace OHOS
#endif  // !NFC_SERVICE_PROXY_H