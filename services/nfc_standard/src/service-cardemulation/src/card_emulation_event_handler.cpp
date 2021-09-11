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

#include "card_emulation_event_handler.h"

#include <functional>
#include <iostream>
#include <sstream>

#include "ability_connection_stub.h"
#include "ability_manager_client.h"
#include "apdu_channel.h"
#include "card_emulation_def.h"
#include "card_emulation_error.h"
#include "card_emulation_service_info_manager.h"
#include "card_emulation_util.h"
#include "errors.h"
#include "ipc_skeleton.h"
#include "iservice_registry.h"
#include "loghelper.h"
#include "string_ex.h"

#ifdef USE_HILOG
DEFINE_NFC_LOG_LABEL("CardEmulationEventHandler");
#endif
using namespace OHOS::AppExecFwk;
using namespace OHOS::AAFwk;
using namespace OHOS::nfc::sdk::cardemulation;
namespace OHOS::nfc::cardemulation {

enum EN_DEACTIVATION_REASON {
    LINK_LOSS,  
    DESELECTED  
};
enum EN_MSG_HCE {
    COMMAND_APDU,   
    RESPONSE_APDU,  
    DEACTIVATED,    
    UNHANDLED       
};

static const std::vector<unsigned char> UNABLE_TO_HANDLE_AID = {0x6A, 0x82};
static const std::vector<unsigned char> UNEXPECTED_APDU = {0x6F, 0x00};

CardEmulationEventHandler::CardEmulationEventHandler(std::shared_ptr<ICardEmulationDeviceHost> ceDeviceHost,
                                                     std::shared_ptr<IAidRoutingManager> aidRoutingManager)
    : aidRoutingManager_(aidRoutingManager),
    state_(EventHandlerState::IDLE),
    ceDeviceHost_(ceDeviceHost)
{
}

CardEmulationEventHandler::~CardEmulationEventHandler() = default;

void CardEmulationEventHandler::OnHCEActivated(void)
{
    std::lock_guard<std::mutex> lock(mu_);
    CloseTapAgainDialog();
    state_ = EventHandlerState::WAIT_FOR_SELECT_AID;
}

int CardEmulationEventHandler::OnHCEData(const unsigned char* data, size_t len)
{
    auto aid = FindSelectAid(data, len);
#ifdef USE_HILOG

    DebugLog("FindSelectAid: %{public}s", aid.c_str());
    InfoLog("OnHCEData:\n\t0x%{public}s\n\t state: %{public}d", BytesToHexStr(data, len).c_str(), StateToInt(state_));
#else
    DebugLog("FindSelectAid: %s", aid.c_str());
    InfoLog("OnHCEData:\n\t0x%s\n\t state: %d", BytesToHexStr(data, len).c_str(), StateToInt(state_));
#endif

    std::lock_guard<std::mutex> lock(mu_);
    if (state_ == EventHandlerState::IDLE) {
        SendDataToReader(UNEXPECTED_APDU);
        return ERR_DROP_HCE_EVENT_DATA;
    }
    if (state_ == EventHandlerState::WAIT_FOR_DEACTIVE) {
        SendDataToReader(UNEXPECTED_APDU);
        return ERR_DROP_HCE_EVENT_DATA;
    }
    std::string resolvedAidsetType;
    std::shared_ptr<CardEmulationServiceInfo> resolvedServiceInfo;
    if (!aid.empty()) {
        auto ceServices = aidRoutingManager_->GetCardEmulationServicesByAid(aid);
#ifdef USE_HILOG

        DebugLog("ceServiceNames.size(): %{public}zu", ceServices.size());
#else
        DebugLog("ceServiceNames.size(): %zu", ceServices.size());
#endif

        if (ceServices.empty()) {
            // not found service
            SendDataToReader(UNABLE_TO_HANDLE_AID);
            return ERR_DROP_HCE_EVENT_DATA;
        } else {
            // ceServices.size() > 0
            resolvedServiceInfo = ceServices.front().first.lock();
            resolvedAidsetType = ceServices.front().second;
            DebugLog("resolved service: %p", resolvedServiceInfo.get());
            if (!resolvedServiceInfo) {
                SendDataToReader(UNABLE_TO_HANDLE_AID);
                return ERR_DROP_HCE_EVENT_DATA;
            }
        }
        lastSelectAid_ = aid;
    }
    switch (state_) {
        case EventHandlerState::WAIT_FOR_SELECT_AID:
            if (aid.empty()) {
            } else {
                HandleDataInWaitForSelectState(resolvedServiceInfo, resolvedAidsetType, data, len);
                return ERR_OK;
            }
            break;
        case EventHandlerState::WAIT_FOR_CONNECT_ABILITY:
            // drop data
            DebugLog("dropping apdu. reason: waiting for connect ability .");
            break;
        case EventHandlerState::TRANSFERRING:
            if (!aid.empty()) {
                HandleDataInWaitForSelectState(resolvedServiceInfo, resolvedAidsetType, data, len);
                return ERR_OK;
            } else if (IsActiveServiceValid()) {
                DebugLog("aid is empty, send data to active service. state: TRANSFERRING");
                SendDataToCeService(GetActiveService(), data, len);
                return ERR_OK;
            } else {
                DebugLog("dropping apdu. reason: service no longer active.");
            }
            break;
        default:

            break;
    }

    SendDataToReader(UNEXPECTED_APDU);
    return ERR_DROP_HCE_EVENT_DATA;
}

void CardEmulationEventHandler::OnHCEDeactivated(void)
{
    std::lock_guard<std::mutex> lock(mu_);
    NotifyActiveCEServiceDeactive(EN_DEACTIVATION_REASON::LINK_LOSS);

    state_ = EventHandlerState::IDLE;
}

void CardEmulationEventHandler::OnOffHostTransaction(void)
{
    std::lock_guard<std::mutex> lock(mu_);
    if (state_ == EventHandlerState::TRANSFERRING) {
        NotifyActiveCEServiceDeactive(EN_DEACTIVATION_REASON::DESELECTED);
    }
    state_ = EventHandlerState::WAIT_FOR_SELECT_AID;
    CloseTapAgainDialog();
}
void CardEmulationEventHandler::OnRemoteDied(const wptr<IRemoteObject>& object)
{
    std::lock_guard<std::mutex> lock(mu_);

    if (preferredService_ == object) {
        DebugLog("normal service died");
        preferredService_.clear();
        preferredServiceName_ = ElementName();
    } else if (primaryService_ == object) {
        DebugLog("secure service died");
        primaryService_.clear();
        primaryServiceName_ = ElementName();
    }
}
void CardEmulationEventHandler::SendDataToCeService(const OHOS::sptr<sdk::cardemulation::IApduChannel>& service,
                                                    const unsigned char* data,
                                                    size_t len)
{
    if (activeHCEService_ != service) {
        NotifyActiveCEServiceDeactive(EN_DEACTIVATION_REASON::DESELECTED);
        activeHCEService_ = service;
    }
    auto msg = std::make_unique<Msg>(EN_MSG_HCE::COMMAND_APDU, 0, 0);
    msg->SetData(std::vector<unsigned char>(data, data + len));
    if (!apduChannel_) {
        apduChannel_ = OHOS::sptr<ApduChannelStub>(new ApduChannelStub());
        apduChannel_->SetHandler([this](std::unique_ptr<Msg> msg1) { HandleApduResponse(std::move(msg1)); });
    }
    msg->SetSource(RemoteObjectPool::GetRemoteObject(apduChannel_->AsObject()));
#ifdef USE_HILOG

    DebugLog("SendDataToCeService:\n\tId %{public}d, Arg1: %{public}d, Arg2: %{public}d\n\t=>Apdu: %{public}s",
             msg->Id(),
             msg->Arg1(),
             msg->Arg2(),
             OHOS::nfc::cardemulation::BytesToHexStr(msg->GetData()).c_str());
#else
    DebugLog("SendDataToCeService:\n\tId %d, Arg1: %d, Arg2: %d\n\t=>Apdu: %s",
             msg->Id(),
             msg->Arg1(),
             msg->Arg2(),
             OHOS::nfc::cardemulation::BytesToHexStr(msg->GetData()).c_str());
#endif
    auto activeService = GetActiveService();
    if (activeService) {
        activeService->Send(std::move(msg));
        return;
    }
    ErrorLog("active service no longer valid.");
}

OHOS::sptr<sdk::cardemulation::IApduChannel> CardEmulationEventHandler::ConnectAbility(
    int userId,
    const std::shared_ptr<OHOS::AppExecFwk::ElementName>& elementName)
{
    auto rv = HandleConnectAbility(userId, elementName);
    if (rv) {
        return rv;
    }
    return HandleDisconnectAbility(userId, elementName);
}

OHOS::sptr<sdk::cardemulation::IApduChannel> CardEmulationEventHandler::HandleConnectAbility(
    int userId,
    const std::shared_ptr<OHOS::AppExecFwk::ElementName>& elementName)
{
    if (primaryServiceName_ == *elementName) {
        return primaryService_;
    }
    if (preferredServiceName_ == *elementName) {
        return preferredService_;
    }
    return OHOS::sptr<sdk::cardemulation::IApduChannel>();
}

static std::shared_ptr<AbilityManagerClient> GetAbilityManagerClient()
{
    auto amc = AbilityManagerClient::GetInstance();
    if (amc) {
        auto err = amc->Connect();
        if (FAILED(err)) {
            ErrorLog("fail to connect ability manager!");
        }
    }
    return amc;
}
OHOS::sptr<sdk::cardemulation::IApduChannel> CardEmulationEventHandler::HandleDisconnectAbility(
    int userId,
    const std::shared_ptr<OHOS::AppExecFwk::ElementName>& elementName)
{
    if (elementName) {
        auto amc = GetAbilityManagerClient();
        if (!amc) {
            ErrorLog("AbilityManagerClient::GetInstance() return nullptr!");
            return OHOS::sptr<sdk::cardemulation::IApduChannel>();
        }
        CreateNormalServiceConnection();
        Want want;
        want.SetElement(*elementName);
        auto err = amc->ConnectAbility(want, normalConnectionObserver_, OHOS::sptr<OHOS::IRemoteObject>());
        if (FAILED(err)) {
            ErrorLog("fail to connect ability. URI: %s ", elementName->GetURI().c_str());
        } else {
            connectionState_ = ConnectionState::Connecting;
        }
    }
    return OHOS::sptr<sdk::cardemulation::IApduChannel>();
}

void CardEmulationEventHandler::DisconnectPrimaryService()
{
    primaryService_.clear();
    primaryServiceName_ = ElementName();
}

void CardEmulationEventHandler::DisconnectNormalService()
{
    preferredService_.clear();
    preferredServiceName_ = ElementName();
}

void CardEmulationEventHandler::CreateSecureServiceConnection()
{
    if (!secureConnectionObserver_) {
        auto amc = GetAbilityManagerClient();
        if (!amc) {
            ErrorLog("AbilityManagerClient::GetInstance() return nullptr!");
            return;
        }

        secureConnectionObserver_ = OHOS::sptr<AbilityConnectionStub>(new AbilityConnectionStub(
            [this](const OHOS::AppExecFwk::ElementName& element, const OHOS::sptr<OHOS::IRemoteObject>& remoteObject) {
                std::lock_guard<std::mutex> lock(mu_);

                if (!(element == expectedPrimaryServiceName_)) {
                    return;
                }
                primaryService_ = OHOS::sptr<ApduChannelProxy>(new ApduChannelProxy(remoteObject));
                primaryServiceName_ = element;
            },
            [this](const AppExecFwk::ElementName& element) {
                std::lock_guard<std::mutex> lock(mu_);
                if (element == primaryServiceName_) {
                    DisconnectPrimaryService();
                }
            }));
    }
}

void CardEmulationEventHandler::CreateNormalServiceConnection()
{
    if (!normalConnectionObserver_) {
        normalConnectionObserver_ = OHOS::sptr<AbilityConnectionStub>(new AbilityConnectionStub(
            [this](const OHOS::AppExecFwk::ElementName& element, const OHOS::sptr<OHOS::IRemoteObject>& remoteObject) {
                std::lock_guard<std::mutex> lock(mu_);
                preferredService_ = OHOS::sptr<ApduChannelProxy>(new ApduChannelProxy(remoteObject));
                preferredServiceName_ = element;
                if (!selectApdu_.empty()) {
                    state_ = EventHandlerState::TRANSFERRING;
                    SendDataToCeService(preferredService_, &selectApdu_[0], selectApdu_.size());
                }
            },
            [this](const AppExecFwk::ElementName& element) {
                std::lock_guard<std::mutex> lock(mu_);
                if (element == preferredServiceName_) {
                    DisconnectNormalService();
                }
            }));
    }
}

int CardEmulationEventHandler::StateToInt(EventHandlerState state)
{
    return static_cast<int>(state);
}

void CardEmulationEventHandler::CloseTapAgainDialog()
{
}

void CardEmulationEventHandler::NotifyActiveCEServiceDeactive(int reason)
{
    auto service = GetActiveService();
    if (!service) {
        return;
    }
    auto msg = std::make_unique<Msg>(EN_MSG_HCE::DEACTIVATED, 0, 0);
    msg->SetArg1(reason);
    service->Send(std::move(msg));
}
void CardEmulationEventHandler::LaunchTapAgainDialog()
{
}

void CardEmulationEventHandler::LaunchSelectorDialog(
    const std::vector<ServiceInfoTypePair>& services,
    const std::shared_ptr<OHOS::AppExecFwk::ElementName>& unhandledServiceName)
{
}

void CardEmulationEventHandler::HandleDataInWaitForSelectState(
    const std::shared_ptr<CardEmulationServiceInfo>& serviceInfo,
    const std::string& aidsetType,
    const unsigned char* data,
    size_t len)
{
    OHOS::sptr<IApduChannel> channel = serviceInfo ? serviceInfo->GetApduChannel() : nullptr;
    if (channel) {
        // directly connect
#ifdef USE_HILOG

        DebugLog("directly connect, type: %{public}s", aidsetType.c_str());
#else
        DebugLog("directly connect, type: %s", aidsetType.c_str());
#endif

    } else {
#ifdef USE_HILOG

        DebugLog("connect ability, type: %{public}s", aidsetType.c_str());
#else
        DebugLog("connect ability, type: %s", aidsetType.c_str());
#endif
        channel = ConnectAbility(GetCurrentUserId(), serviceInfo->GetName());
    }
    if (channel) {
        state_ = EventHandlerState::TRANSFERRING;

        activeHCEService_ = channel;
        SendDataToCeService(channel, data, len);
#ifdef USE_HILOG

        DebugLog("state: %{public}d, send apdu: %{public}s",
                 static_cast<int>(state_),
                 BytesToHexStr(std::vector<unsigned char>(data, data + len)).c_str());

#else
        DebugLog("state: %d, send apdu: %s",
                 static_cast<int>(state_),
                 BytesToHexStr(std::vector<unsigned char>(data, data + len)).c_str());

#endif
    } else {
        selectApdu_ = std::vector<unsigned char>(data, data + len);
        state_ = EventHandlerState::WAIT_FOR_CONNECT_ABILITY;

        DebugLog("channel is nullptr. wait for connect ability......");
        SendDataToReader(UNABLE_TO_HANDLE_AID);
    }
}

int CardEmulationEventHandler::GetCurrentUserId() const
{
    return IPCSkeleton::GetCallingUid();
}
void CardEmulationEventHandler::SendDataToReader(std::vector<unsigned char> data)
{
    auto dh = ceDeviceHost_.lock();
    if (dh) {
#ifdef USE_HILOG

        DebugLog("send data to device host,data: \n\t<=Resp: 0x%{public}s", BytesToHexStr(data).c_str());
#else
        DebugLog("send data to device host,data: \n\t<=Resp: 0x%s", BytesToHexStr(data).c_str());
#endif

        dh->SendData(std::move(data));
    }
}
void CardEmulationEventHandler::HandleApduResponse(std::unique_ptr<sdk::cardemulation::Msg> msg)
{
    {
        std::lock_guard<std::mutex> lock(mu_);
        auto service = GetActiveService();
        if (service) {
            if (!msg->EqualsReplyRemoteObject(RemoteObjectPool::GetRemoteObject(service->AsObject()))) {
                DebugLog("drop reponse. reason: msg source service does not equal active service.");
                SendDataToReader(UNABLE_TO_HANDLE_AID);

                return;
            }
        }
    }
    switch (msg->Id()) {
        case EN_MSG_HCE::RESPONSE_APDU: {
            auto data = msg->GetData();
            EventHandlerState state;
            {
                std::lock_guard<std::mutex> lock(mu_);
                state = state_;
            }
            if (state == EventHandlerState::TRANSFERRING) {
                SendDataToReader(std::move(data));
                return;
            } else {
#ifdef USE_HILOG

                DebugLog("dropping  response. reason: wrong state [%{public}d]", static_cast<int>(state));
#else
                DebugLog("dropping  response. reason: wrong state [%d]", static_cast<int>(state));
#endif
            }

            break;
        }
        case EN_MSG_HCE::UNHANDLED: {
            break;
        }
        default:
            break;
    }
    SendDataToReader(UNABLE_TO_HANDLE_AID);
}
bool CardEmulationEventHandler::IsActiveServiceValid()
{
    auto service = activeHCEService_.promote();
    return service.GetRefPtr() != nullptr;
}
OHOS::sptr<sdk::cardemulation::IApduChannel> CardEmulationEventHandler::GetActiveService()
{
    return activeHCEService_.promote();
}
}  // namespace OHOS::nfc::cardemulation
