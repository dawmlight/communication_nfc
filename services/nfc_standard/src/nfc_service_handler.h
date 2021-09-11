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
#ifndef NFC_SERVICE_HANDLER_H
#define NFC_SERVICE_HANDLER_H

#include "event_handler.h"

namespace OHOS {
namespace nfc::ncibal {
class IDeviceHost;
}  // namespace nfc::ncibal
using nfc::ncibal::IDeviceHost;

namespace nfc {
namespace cardemulation {
class CardEmulationManager;
}
namespace reader {
class TagDispatcher;
}

class NfcService;

class NfcServiceHandler final : public AppExecFwk::EventHandler {
public:
    explicit NfcServiceHandler(const std::shared_ptr<AppExecFwk::EventRunner>& runner,
                               std::weak_ptr<NfcService> servcie);
    ~NfcServiceHandler();
    NfcServiceHandler& operator=(const NfcServiceHandler&) = delete;
    NfcServiceHandler(const NfcServiceHandler&) = delete;

    void Intialize(std::weak_ptr<reader::TagDispatcher> tagDispatcher);
    void ProcessEvent(const AppExecFwk::InnerEvent::Pointer& event) override;

private:
    // RF field events as defined in NFC extras
    const std::string ACTION_RF_FIELD_ON_DETECTED = "com.hos.nfc_extras.action.RF_FIELD_ON_DETECTED";
    const std::string ACTION_RF_FIELD_OFF_DETECTED = "com.hos.nfc_extras.action.RF_FIELD_OFF_DETECTED";
    // ====== adapter Begin============
    const std::string ACTION_PREFERRED_PAYMENT_CHANGED{"hos.nfc.action.PREFERRED_PAYMENT_CHANGED"};
    const std::string EXTRA_PREFERRED_PAYMENT_CHANGED_REASON{"hos.nfc.extra.PREFERRED_PAYMENT_CHANGED_REASON"};
    std::weak_ptr<NfcService> mNfcService_{};
    std::weak_ptr<IDeviceHost> mDeviceHost_{};
    std::weak_ptr<reader::TagDispatcher> mTagDispatcher_{};
#ifdef _NFC_SERVICE_HCE_
    std::weak_ptr<nfc::cardemulation::CardEmulationManager> mCardEmulationManager_{};
#endif  // _NFC_SERVICE_HCE_
};
}  // namespace nfc
}  // namespace OHOS
#endif  // !NFC_SERVICE_HANDLER_H
