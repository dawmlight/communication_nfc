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

#include "nfc_service_proxy.h"

#include "card_emulation_agent_proxy.h"
#include "iapp_callback.h"
#include "loghelper.h"
#include "message_option.h"
#include "message_parcel.h"
#include "nfc_map.h"
#include "nfc_sdk_common.h"
#include "tag_session_proxy.h"

using namespace std;

namespace OHOS {
namespace nfc {
const std::string NFC_INTERFACE_TOKEN = "ohos.nfc.INfcService";
NfcServiceProxy ::~NfcServiceProxy() {}
int NfcServiceProxy::GetState()
{
    int state = 0;
    MessageParcel data;
    MessageOption option;
    int res = ProcessIntRes(COMMAND_GET_STATE, data, option, state);

    if (res != ERR_NONE) {
        InfoLog("It is failed To Get State with Res(%d).", res);
        return nfc::sdk::STATE_OFF;
    }
    return state;
}

bool NfcServiceProxy::TurnOn()
{
    DebugLog("NfcServiceProxy::TurnOn in.");
    bool result = false;
    MessageParcel data;
    MessageOption option(MessageOption::TF_ASYNC);
    int32_t res = ProcessBoolRes(COMMAND_TURN_ON, data, option, result);
    if (res != ERR_NONE) {
        DebugLog("NfcServiceProxy::TurnOn error.");
        return false;
    }
    return result;
}

bool NfcServiceProxy::TurnOff(bool saveState)
{
    DebugLog("NfcServiceProxy::TurnOff in.");
    bool result = false;
    MessageParcel data;
    data.WriteBool(saveState);
    MessageOption option(MessageOption::TF_ASYNC);
    int res = ProcessBoolRes(COMMAND_TURN_OFF, data, option, result);
    if (res != ERR_NONE) {
        DebugLog("NfcServiceProxy::TurnOff error.");
        return false;
    }
    return result;
}

void NfcServiceProxy::PausePolling(int timeoutInMs)
{
    MessageParcel data;
    data.WriteInt32(timeoutInMs);
    MessageOption option(MessageOption::TF_ASYNC);
    int res = ProcessCommand(COMMAND_PAUSE_POLLING, data, option);
    if (res != ERR_NONE) {
        InfoLog("It is failed To Pause Polling for (%d).", res);
        return;
    }
    return;
}

void NfcServiceProxy::ResumePolling()
{
    MessageParcel data;
    MessageOption option(MessageOption::TF_ASYNC);
    int res = ProcessCommand(COMMAND_RESUME_POLLING, data, option);
    if (res != ERR_NONE) {
        InfoLog("It is failed To Resume Polling for (%d).", res);
        return;
    }
    return;
}

bool NfcServiceProxy::SetNfcSecure(bool enable)
{
    bool result = false;
    MessageParcel data;
    data.WriteBool(enable);
    MessageOption option(MessageOption::TF_ASYNC);
    ProcessBoolRes(COMMAND_SET_NFC_SECURE, data, option, result);
    return result;
}

bool NfcServiceProxy::DeviceSupportsNfcSecure()
{
    bool result = false;
    MessageParcel data;
    MessageOption option(MessageOption::TF_SYNC);
    ProcessBoolRes(COMMAND_DEVICE_SUPPORT_SECURE, data, option, result);
    return result;
}

bool NfcServiceProxy::IsNfcSecureEnabled()
{
    bool result = false;
    MessageParcel data;
    MessageOption option(MessageOption::TF_SYNC);
    ProcessBoolRes(COMMAND_IS_SECURE_ENABLED, data, option, result);
    return result;
}
#ifdef _NFC_SERVICE_HCE_
OHOS::sptr<OHOS::nfc::sdk::cardemulation::ICardEmulationAgent> NfcServiceProxy::GetNfcCardEmulationInterface()
{
    using namespace OHOS::nfc::sdk::cardemulation;
    OHOS::sptr<sdk::cardemulation::ICardEmulationAgent> result;
    MessageParcel data;
    MessageOption option(MessageOption::TF_SYNC);
    MessageParcel reply;
    int res = Remote()->SendRequest(COMMAND_GET_CARDEMULATION_INTERFACE, data, reply, option);

    if (res == ERR_NONE) {
        result = new CardEmulationAgentProxy(reply.ReadRemoteObject());
    } else {
        ErrorLog("SendRequest: %d, return %d", COMMAND_GET_CARDEMULATION_INTERFACE, res);
    }
    return result;
}
#ifdef _NFC_F_CE_
OHOS::sptr<OHOS::nfc::sdk::cardemulation::INfcFCardEmulation> NfcServiceProxy::GetNfcFCardEmulationInterface()
{
    return new OHOS::nfc::sdk::cardemulation::INfcFCardEmulation();
}
#endif
#endif  // _NFC_SERVICE_HCE_
#ifdef _NFC_SERVICE_READER_
void NfcServiceProxy::SetRWMode(const sptr<IRemoteObject>& readerObj,
                                std::weak_ptr<nfc::sdk::IAppCallback> appCallback,
                                int flags,
                                std::weak_ptr<NfcMap> extras)
{
    MessageParcel data;
    data.WriteRemoteObject(readerObj);
    data.WriteRemoteObject(appCallback.lock()->AsObject());
    data.WriteInt32(flags);
    data.WriteParcelable(extras.lock().get());
    MessageOption option(MessageOption::TF_ASYNC);
    int res = ProcessCommand(COMMAND_SET_RW_MODE, data, option);
    if (res != ERR_NONE) {
        InfoLog("It is failed To Set Read/Write Mode for (%d).", res);
        return;
    }
    return;
}

bool NfcServiceProxy::SetIgnore(int handle, int debounceMs, std::weak_ptr<nfc::sdk::ITagRemovedCallback> callback)
{
    bool result = false;
    MessageParcel data;
    data.WriteInt32(handle);
    data.WriteInt32(debounceMs);
    data.WriteRemoteObject(callback.lock()->AsObject());
    MessageOption option(MessageOption::TF_ASYNC);
    int res = ProcessBoolRes(COMMAND_SET_IGNORE, data, option, result);
    if (res != ERR_NONE) {
        InfoLog("It is failed To Set Ignore for (%d).", res);
        return false;
    }
    return result;
}

void NfcServiceProxy::SetFrontDispatch()
{
    MessageParcel data;
    MessageOption option(MessageOption::TF_ASYNC);
    int res = ProcessCommand(COMMAND_SET_FRONT_DISPATCH, data, option);
    if (res != ERR_NONE) {
        InfoLog("It is failed To Set Front Dispatch for (%d).", res);
        return;
    }
    return;
}

OHOS::sptr<nfc::reader::ITagSession> NfcServiceProxy::GetNfcTagInterface()
{
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    MessageParcel data;
    int32_t res = Remote()->SendRequest(COMMAND_GET_TAG_INTERFACE, data, reply, option);
    if (res != ERR_NONE) {
        InfoLog("It is failed To Send Request Code (%d) for (%d).", COMMAND_GET_TAG_INTERFACE, res);
        // return std::shared_ptr<nfc::reader::ITagSession>();
    }
    sptr<OHOS::IRemoteObject> session = reply.ReadRemoteObject();
    int res1 = reply.ReadInt32();
    if (res1 != ERR_NONE) {
        InfoLog("It is failed To Send Request Code (%d) for res1 (%d).", COMMAND_GET_TAG_INTERFACE, res1);
        return OHOS::sptr<nfc::reader::ITagSession>();
    }
    tagSession_ = new nfc::reader::TagSessionProxy(session);
    return tagSession_;
}
#endif
}  // namespace nfc
}  // namespace OHOS
