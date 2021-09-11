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

#include "nfc_manager_service.h"

#include <ctime>
#include <iostream>
#include <string>
#include <thread>

#include "card_emulation/card_emulation.h"
#include "common_event_manager.h"
#include "ipc_skeleton.h"
#include "nfc_adapter_utils.h"
#include "nfc_agent/nfc_agent.h"
#include "nfc_error.h"
#include "nfc_log.h"
#include "nfc_utils.h"
#include "secure_element/secure_element.h"
#include "string_ex.h"
#include "system_ability_definition.h"
#include "tag/tag_session.h"

using namespace OHOS::Notification;
namespace OHOS {
namespace Nfc {
const std::string PKG_ADAPTER = "ohos.nfc.INfcAdapter";

/* const for broadcast */
const std::string CONVERT_JAR_NAME = "/system/framework/zframework.z.jar";
const std::string NFC_CONVERT_CLASS_NAME =  "ohos.nfc.NfcEventsConverter";
const std::list<std::string> NFC_CONVERT_ACTIONS = {
    StringReplaceReturnEight("ohos.nfc.action.ADAPTER_STATE_CHANGED"),
    StringReplaceReturnEight("com.ohos.nfc_extras.action.RF_FIELD_ON_DETECTED"),
    StringReplaceReturnEight("com.ohos.nfc_extras.action.RF_FIELD_OFF_DETECTED"),
};

const int REGISTER_TIME_INTERVAL = 5000;
const int MAX_CE_REGISTER_NUM = 30;

std::mutex NfcManagerService::g_instanceLock;
sptr<NfcManagerService> NfcManagerService::g_nfcManagerService;

const bool REGISTER_RESULT = SystemAbility::MakeAndRegisterAbility(
    NfcManagerService::GetInstance().GetRefPtr());

NfcManagerService::NfcManagerService() : SystemAbility(NFC_MANAGER_SYS_ABILITY_ID, true)
{
}

NfcManagerService::~NfcManagerService()
{
}

sptr<NfcManagerService> NfcManagerService::GetInstance()
{
    if (!g_nfcManagerService) {
        std::lock_guard<std::mutex> lock(g_instanceLock);
        if (g_nfcManagerService == nullptr) {
            sptr<NfcManagerService> tmp = new NfcManagerService;
            g_nfcManagerService = tmp;
        }
    }
    return g_nfcManagerService;
}

void NfcManagerService::OnStart()
{
    if (state_ == ServiceRunningState::STATE_RUNNING) {
        HILOGI("NfcManagerService has already started.");
        return;
    }
    if (!Init()) {
        HILOGE("failed to init NfcManagerService");
        return;
    }
    state_ = ServiceRunningState::STATE_RUNNING;
    HILOGI("NfcManagerService::OnStart start service success.");
}

bool NfcManagerService::Init()
{
    HILOGI("NfcManagerService::Init ready to init.");
    if (!registerToService_) {
        bool ret = Publish(NfcManagerService::GetInstance());
        if (!ret) {
            HILOGE("NfcManagerService::Init Publish failed!");
            return false;
        }
        registerToService_ = true;
    }
    HILOGI("NfcManagerService::Init init success.");

    std::thread t(std::bind(&NfcManagerService::RegisterNfcConvertActions, NfcManagerService::GetInstance()));
    t.detach();
    return true;
}

void NfcManagerService::RegisterNfcConvertActions()
{
    int tryNum = 0;
    HILOGI("NfcManagerService::RegisterNfcConvertActions.");
    sptr<CommonEventConvertInfo> nfcConvertInfo = new CommonEventConvertInfo();
    if (nfcConvertInfo == nullptr) {
        HILOGE("Unable to malloc CommonEventConvertInfo");
        return;
    }

    while (tryNum++ < MAX_CE_REGISTER_NUM) {
        HILOGI("RegisterCE tryNum = %{public}d", tryNum);
        nfcConvertInfo->SetJarName(CONVERT_JAR_NAME);
        nfcConvertInfo->SetClassName(NFC_CONVERT_CLASS_NAME);
        nfcConvertInfo->SetActions(NFC_CONVERT_ACTIONS);
        ErrCode errCode = CommonEventManager::GetInstance().RegisterConvertAction(nfcConvertInfo);
        if (FAILED(errCode)) {
            HILOGE("Failed to register notification, errCode: %{public}d", errCode);
            std::this_thread::sleep_for(std::chrono::milliseconds(REGISTER_TIME_INTERVAL));
            continue;
        } else {
            HILOGI("Succeed to register notification, ec: %{public}d", errCode);
            return;
        }
    }
}

void NfcManagerService::OnStop()
{
    HILOGI("NfcManagerService::OnStop ready to stop service.");
    state_ = ServiceRunningState::STATE_NOT_START;
    registerToService_ = false;
    HILOGI("NfcManagerService::OnStop stop service success.");
}

sptr<IRemoteObject> NfcManagerService::GetRemoteNfcService()
{
    if (g_remoteNfcService == nullptr) {
        sptr<IServiceRegistry> registry = ServiceRegistry::GetInstance();
        if (registry == nullptr) {
            return nullptr;
        }
        g_remoteNfcService = registry->GetService(u"nfc");
        if (g_remoteNfcService == nullptr) {
            HILOGE("NfcManagerService::GetRemoteNfcService remoteNfcService_ == nullptr");
        }
    }
    return g_remoteNfcService;
}

int32_t NfcManagerService::SetNfcEnabled(MessageParcel& data, MessageParcel& reply)
{
    return Nfc::NfcAgent::GetInstance().SetNfcEnabled(data, reply);
}

int32_t NfcManagerService::GetNfcState(MessageParcel& data, MessageParcel& reply)
{
    return Nfc::NfcAgent::GetInstance().GetNfcState(data, reply);
}

bool NfcManagerService::IsNfcAvailable(MessageParcel& data, MessageParcel& reply)
{
    return Nfc::NfcAgent::GetInstance().IsNfcAvailable(data, reply) == Nfc::ERR_NONE;
}

void NfcManagerService::RegisterForegroundDispatch(MessageParcel& data, MessageParcel& reply)
{
    Nfc::NfcAgent::GetInstance().RegisterForegroundDispatch(data, reply);
}

bool NfcManagerService::SetReaderMode(MessageParcel& data, MessageParcel& reply)
{
    return Nfc::NfcAgent::GetInstance().SetReaderMode(data, reply) == Nfc::ERR_NONE;
}

bool NfcManagerService::ConnectTag(MessageParcel& data, MessageParcel& reply)
{
    return Tag::TagSession::GetInstance().ConnectTag(data, reply) == Nfc::ERR_NONE;
}

bool NfcManagerService::ReconnectTag(MessageParcel& data, MessageParcel& reply)
{
    return Tag::TagSession::GetInstance().ReconnectTag(data, reply) == Nfc::ERR_NONE;
}

void NfcManagerService::ResetTimeouts(MessageParcel& data, MessageParcel& reply)
{
    Tag::TagSession::GetInstance().ResetTimeouts(data, reply);
}

bool NfcManagerService::IsTagConnected(MessageParcel& data, MessageParcel& reply)
{
    return Tag::TagSession::GetInstance().IsTagConnected(data, reply) == Nfc::ERR_NONE;
}

bool NfcManagerService::SetSendDataTimeout(MessageParcel& data, MessageParcel& reply)
{
    return Tag::TagSession::GetInstance().SetSendDataTimeout(data, reply) == Nfc::ERR_NONE;
}

int32_t NfcManagerService::GetSendDataTimeout(MessageParcel& data, MessageParcel& reply)
{
    return Tag::TagSession::GetInstance().GetSendDataTimeout(data, reply);
}

int32_t NfcManagerService::GetMaxSendLength(MessageParcel& data, MessageParcel& reply)
{
    return Tag::TagSession::GetInstance().GetMaxSendLength(data, reply);
}

int32_t NfcManagerService::SendData(MessageParcel& data, MessageParcel& reply)
{
    return Tag::TagSession::GetInstance().SendData(data, reply);
}

/* NFC Tag operations 2.0 */
int32_t NfcManagerService::IsNdefTag(MessageParcel& data, MessageParcel& reply)
{
    return Tag::TagSession::GetInstance().IsNdefTag(data, reply);
}

int32_t NfcManagerService::NdefRead(MessageParcel& data, MessageParcel& reply)
{
    return Tag::TagSession::GetInstance().NdefRead(data, reply);
}

int32_t NfcManagerService::NdefWrite(MessageParcel& data, MessageParcel& reply)
{
    return Tag::TagSession::GetInstance().NdefWrite(data, reply);
}

int32_t NfcManagerService::NdefSetReadOnly(MessageParcel& data, MessageParcel& reply)
{
    return Tag::TagSession::GetInstance().NdefSetReadOnly(data, reply);
}

int32_t NfcManagerService::CanSetReadOnly(MessageParcel& data, MessageParcel& reply)
{
    return Tag::TagSession::GetInstance().CanSetReadOnly(data, reply);
}

/* bind Secure Element Service in case it is not auto started */
int32_t NfcManagerService::BindSeService(MessageParcel& data, MessageParcel& reply)
{
    return Oma::SecureElement::GetInstance().BindSeService(data, reply);
}

int32_t NfcManagerService::IsSeServiceConnected(MessageParcel& data, MessageParcel& reply)
{
    return Oma::SecureElement::GetInstance().IsSeServiceConnected(data, reply);
}

/* get all Secure Element names in string */
int32_t NfcManagerService::GetSecureElements(MessageParcel& data, MessageParcel& reply)
{
    return Oma::SecureElement::GetInstance().GetSecureElements(data, reply);
}

/* get SE reader */
sptr<IRemoteObject> NfcManagerService::GetSecureElement(std::u16string seName)
{
    return Oma::SecureElement::GetInstance().GetSecureElement(seName);
}

/* verify if a card present in specific Secure element */
int32_t NfcManagerService::IsSecureElementPresent(MessageParcel& data, MessageParcel& reply)
{
    return Oma::SecureElement::GetInstance().IsSecureElementPresent(data, reply);
}

/* Reader Get Session */
int32_t NfcManagerService::OpenSession(MessageParcel& data, MessageParcel& reply)
{
    return Oma::SecureElement::GetInstance().OpenSession(data, reply);
}

int32_t NfcManagerService::GetAtr(MessageParcel& data, MessageParcel& reply)
{
    return Oma::SecureElement::GetInstance().GetAtr(data, reply);
}

/* Reader Close Remote Sessions */
int32_t NfcManagerService::CloseSeSessions(MessageParcel& data, MessageParcel& reply)
{
    return Oma::SecureElement::GetInstance().CloseSeSessions(data, reply);
}

/* open basic or logical channel */
int32_t NfcManagerService::OpenChannel(MessageParcel& data, MessageParcel& reply)
{
    return Oma::SecureElement::GetInstance().OpenChannel(data, reply);
}

int32_t NfcManagerService::CloseSessionChannels(MessageParcel& data, MessageParcel& reply)
{
    return Oma::SecureElement::GetInstance().CloseSessionChannels(data, reply);
}

int32_t NfcManagerService::GetSelectResponse(MessageParcel& data, MessageParcel& reply)
{
    return Oma::SecureElement::GetInstance().GetSelectResponse(data, reply);
}

int32_t NfcManagerService::TransmitApdu(MessageParcel& data, MessageParcel& reply)
{
    return Oma::SecureElement::GetInstance().TransmitApdu(data, reply);
}

int32_t NfcManagerService::CloseChannel(MessageParcel& data, MessageParcel& reply)
{
    return Oma::SecureElement::GetInstance().CloseChannel(data, reply);
}

int32_t NfcManagerService::IsChannelClosed(MessageParcel& data, MessageParcel& reply)
{
    return Oma::SecureElement::GetInstance().IsChannelClosed(data, reply);
}

int32_t NfcManagerService::SetListenMode(MessageParcel& data, MessageParcel& reply)
{
    return Card::CardEmulation::GetInstance().SetListenMode(data, reply);
}

int32_t NfcManagerService::IsListenModeEnabled(MessageParcel& data, MessageParcel& reply)
{
    return Card::CardEmulation::GetInstance().IsListenModeEnabled(data, reply);
}

int32_t NfcManagerService::GetNfcInfo(MessageParcel& data, MessageParcel& reply)
{
    return Card::CardEmulation::GetInstance().GetNfcInfo(data, reply);
}

int32_t NfcManagerService::SetRfConfig(MessageParcel& data, MessageParcel& reply)
{
    return Card::CardEmulation::GetInstance().SetRfConfig(data, reply);
}

/* get information from config file */
int32_t NfcManagerService::GetInfoFromConfig(MessageParcel& data, MessageParcel& reply)
{
    return Card::CardEmulation::GetInstance().GetInfoFromConfig(data, reply);
}

/* set foreground preferred */
int32_t NfcManagerService::SetForegroundPreferred(MessageParcel& data, MessageParcel& reply)
{
    return Card::CardEmulation::GetInstance().SetForegroundPreferred(data, reply);
}

/* unset foreground preferred */
int32_t NfcManagerService::UnsetForegroundPreferred(MessageParcel& data, MessageParcel& reply)
{
    return Card::CardEmulation::GetInstance().UnsetForegroundPreferred(data, reply);
}

int32_t NfcManagerService::IsDefaultForType(MessageParcel& data, MessageParcel& reply)
{
    return Card::CardEmulation::GetInstance().IsDefaultForType(data, reply);
}

int32_t NfcManagerService::IsDefaultForAid(MessageParcel& data, MessageParcel& reply)
{
    return Card::CardEmulation::GetInstance().IsDefaultForAid(data, reply);
}

int32_t NfcManagerService::RegisterAids(MessageParcel& data, MessageParcel& reply)
{
    return Card::CardEmulation::GetInstance().RegisterAids(data, reply);
}

int32_t NfcManagerService::RemoveAids(MessageParcel& data, MessageParcel& reply)
{
    return Card::CardEmulation::GetInstance().RemoveAids(data, reply);
}

int32_t NfcManagerService::GetAids(MessageParcel& data, MessageParcel& reply)
{
    return Card::CardEmulation::GetInstance().GetAids(data, reply);
}
} // namespace Nfc
} // namespace OHOS
