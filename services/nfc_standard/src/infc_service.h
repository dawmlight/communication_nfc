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
#ifndef I_NFC_SERVICE_H
#define I_NFC_SERVICE_H

#include <memory>
#include <string>

#include "refbase.h"

namespace OHOS {
namespace AppExecFwk {
class EventHandler;
}
namespace nfc::reader {
class ITagSession;
class TagDispatcher;
}  // namespace nfc::reader

namespace nfc::ncibal {
class IDeviceHost;
}

namespace nfc {
static const std::string PREF = "NfcServicePrefs";

enum NfcServiceEvent {
    MSG_NDEF_TAG = 0,
    MSG_ROUTE_AID,
    MSG_UNROUTE_AID,
    MSG_COMMIT_ROUTING,
    MSG_RF_FIELD_ACTIVATED,
    MSG_RF_FIELD_DEACTIVATED,  // 5
    MSG_RESUME_POLLING,
    MSG_REGISTER_T3T_IDENTIFIER,
    MSG_DEREGISTER_T3T_IDENTIFIER,
    MSG_TAG_DEBOUNCE,
    MSG_APPLY_SCREEN_STATE,  // 10
    MSG_TRANSACTION_EVENT,
    MSG_PREFERRED_PAYMENT_CHANGED,
    MSG_EVENT_RESUME_POLLING,
    MSG_TOAST_DEBOUNCE_EVENT
};

struct OffHostTransactionData {
    std::string aid_{""};
    std::string data_{""};
    std::string seName_{""};
};

class INfcService {
public:
    virtual ~INfcService() {}

    virtual bool IsNfcEnabled() = 0;
    virtual void ApplyDiscovering(bool) = 0;
    virtual void PauseDiscovering(int timeoutInMs) = 0;

    virtual bool IsScreenUnlock() = 0;
    virtual bool IsInForeground(int uid) = 0;

    virtual void PowerActivity() = 0;
    virtual void PlaySound(int) = 0;

    virtual std::weak_ptr<ncibal::IDeviceHost> GetDeviceHost() = 0;
#ifdef _NFC_SERVICE_READER_
    virtual OHOS::sptr<reader::ITagSession> GetTagSession() = 0;
    virtual std::weak_ptr<reader::TagDispatcher> GetTagDispatcher() = 0;
    virtual std::weak_ptr<AppExecFwk::EventHandler> GetMessageHandler() = 0;
#endif
private:
};
}  // namespace nfc
}  // namespace OHOS
#endif  // !I_NFC_SERVICE_H
