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
#ifndef I_NFC_AGENT_SERVICE_H
#define I_NFC_AGENT_SERVICE_H

#include <memory>

#include "iremote_broker.h"

#ifdef _NFC_SERVICE_READER_
#include "itag_session.h"
#endif  // _NFC_SERVICE_READER_

namespace OHOS {
class IRemoteObject;
namespace AppExecFwk {
class PacMap;
}

namespace nfc {
namespace sdk {
class IAppCallback;
class ITagRemovedCallback;
class NfcMap;
namespace cardemulation {
class ICardEmulationAgent;
class INfcFCardEmulation;
}  // namespace cardemulation
}  // namespace sdk

enum {
    FLAG_RECEIVER_REGISTERED_ONLY_BEFORE_BOOT,
    FLAG_INCLUDE_STOPPED_PACKAGES,
    FLAG_ACTIVITY_NEW_TASK,
    FLAG_ACTIVITY_SINGLE_TOP,
    FLAG_ACTIVITY_CLEAR_TASK,
    FLAG_RECEIVER_FOREGROUND,
    FLAG_GRANT_READ_URI_PERMISSION,
    FLAG_DISMISS_KEYGUARD
};

class INfcAgentService : public OHOS::IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.nfc.INfcAgentService");

    virtual ~INfcAgentService() {}
    /**
     * @brief  Get the NFC state
     * @return The NFC State
     */
    virtual int GetState() = 0;
    /**
     * @brief Turn On NFC
     * @return true - turn on; the other
     */
    virtual bool TurnOn() = 0;
    /**
     * @brief Turn Off NFC
     * @param saveState If to save the NFC state
     * @return true - turn off; the other
     */
    virtual bool TurnOff(bool saveState) = 0;
    /**
     * @brief Pause NFC Polling
     * @param timeoutInMs the Pause duration
     */
    virtual void PausePolling(int timeoutInMs) = 0;
    /**
     * @brief Resume NFC Polling
     */
    virtual void ResumePolling() = 0;
    /**
     * @brief Turn on/off Secure NFC.
     * @param enable true - Turn on; false - Turn off
     * @return true - yes; false - no
     */
    virtual bool SetNfcSecure(bool enable) = 0;
    /**
     * @brief Checking If Device Supports Nfc Secure.
     * @return true - yes; false - no
     */
    virtual bool DeviceSupportsNfcSecure() = 0;
    /**
     * @brief Checking If Device Is Turn On Nfc Secure.
     * @return true - yes; false - no
     */
    virtual bool IsNfcSecureEnabled() = 0;
#ifdef _NFC_SERVICE_HCE_
    virtual OHOS::sptr<OHOS::nfc::sdk::cardemulation::ICardEmulationAgent> GetNfcCardEmulationInterface() = 0;
#ifdef _NFC_F_CE_
    virtual OHOS::sptr<OHOS::nfc::sdk::cardemulation::INfcFCardEmulation> GetNfcFCardEmulationInterface() = 0;
#endif
#endif  // _NFC_SERVICE_HCE_

#ifdef _NFC_SERVICE_READER_
    /**
     * @brief Setting the reader mode parameters.
     * @param readerObj the binder of reader app
     * @param appCallback the notify interface of the tag discovery event
     * @param flags the tecknolegy and policy
     * @param extras the extended parameters
     */
    virtual void SetRWMode(const sptr<IRemoteObject>& readerObj,
                           std::weak_ptr<nfc::sdk::IAppCallback> appCallback,
                           int flags,
                           std::weak_ptr<sdk::NfcMap> extras) = 0;
    /**
     * @brief Setting the ignore tag parameters
     * @param handle the ignore tag handle
     * @param debounceMs the ignore time
     * @param callback To invoke callback when the tag is ignored
     * @return true - success; false - failed
     */
    virtual bool SetIgnore(int handle, int debounceMs, std::weak_ptr<nfc::sdk::ITagRemovedCallback> callback) = 0;
    virtual void SetFrontDispatch() = 0;
    /**
     * @brief Getting the Tag Session Interface.
     * @return the Tag Session Interface
     */
    virtual OHOS::sptr<reader::ITagSession> GetNfcTagInterface() = 0;
#endif
private:
};

template <typename T> std::shared_ptr<T> ConvertPtrType(std::shared_ptr<void> data)
{
    if (!data) {
        return nullptr;
    }
    return std::static_pointer_cast<T>(data);
}
}  // namespace nfc
}  // namespace OHOS

#endif  // !I_NFC_AGENT_SERVICE_H
