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
#ifndef NFC_AGENT_SERVICE_H
#define NFC_AGENT_SERVICE_H

#include <memory>
#include <mutex>

#include "nfc_service_stub.h"

namespace osal {
class Context;
}  // namespace osal
namespace OHOS {
namespace nfc::sdk {
class IAppCallback;
class ITagRemovedCallback;
}  // namespace nfc::sdk

namespace nfc::reader {
#ifdef _NFC_SERVICE_READER_
class TagDispatcher;
class ITagSession;
#endif
}  // namespace nfc::reader

namespace nfc::sdk {
class NfcMap;
}  // namespace nfc::sdk

namespace nfc {
class NfcService;

class NfcAgentService final : public NfcServiceStub {
public:
    NfcAgentService(std::weak_ptr<NfcService> nfcService, std::weak_ptr<osal::Context> context);
    ~NfcAgentService();

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
                   std::weak_ptr<sdk::NfcMap> extras) override;
    bool SetIgnore(int handle, int debounceMs, std::weak_ptr<nfc::sdk::ITagRemovedCallback> removeCallback) override;
    void SetFrontDispatch() override;
    OHOS::sptr<nfc::reader::ITagSession> GetNfcTagInterface() override;
#endif  // _NFC_SERVICE_READER_
private:
    std::weak_ptr<NfcService> mNfcService_;
#ifdef _NFC_SERVICE_READER_
    std::weak_ptr<nfc::reader::TagDispatcher> mTagDispatcher_{};
#endif  // _NFC_SERVICE_READER_

    std::mutex mtx_{};
};
}  // namespace nfc
}  // namespace OHOS
#endif  // !NFC_AGENTER_H
