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

#include "card_emulation_service.h"

#include <algorithm>
#include <cassert>
#include <vector>

#include "aid_routing_adapter.h"
#include "aid_routing_planner.h"
#include "aid_routing_policy_factory.h"
#include "card_emulation_def.h"
#include "card_emulation_error.h"
#include "card_emulation_event_handler.h"
#include "card_emulation_service_info_lite.h"
#include "card_emulation_service_info_manager.h"
#include "card_emulation_util.h"
#include "element_name.h"
#include "icard_emulation_device_host.h"
#include "loghelper.h"
#ifdef USE_HILOG
DEFINE_NFC_LOG_LABEL("CardEmulationService");
#endif

namespace OHOS::nfc::cardemulation {
CardEmulationService::CardEmulationService(std::shared_ptr<ICardEmulationDeviceHost> nci)
    : aidRoutingAdapter_(),
    aidRoutingPlanner_(),
    serviceInfoManager_(),
    eventHandler_(),
    inited_(false),
    ceDeviceHost_(nci)
{
    aidRoutingAdapter_ = std::make_shared<AidRoutingAdapter>(ceDeviceHost_);
}
CardEmulationService::~CardEmulationService() = default;

int nfc::cardemulation::CardEmulationService::Init()
{
    assert(aidRoutingAdapter_);
    auto uiccs = aidRoutingAdapter_->GetSupportedUiccs();
    auto ses = aidRoutingAdapter_->GetSupportedEses();

    std::vector<std::string> supportedExecEnv{NFC_EE_HOST};
    supportedExecEnv.insert(supportedExecEnv.end(), uiccs.begin(), uiccs.end());
    supportedExecEnv.insert(supportedExecEnv.end(), ses.begin(), ses.end());
    auto mode = aidRoutingAdapter_->GetAidRoutingMode();

    auto policy = RoutingPolicyFactory().CreateRoutingPolicy(supportedExecEnv, mode, CARDEMULATION_SERVICE_TYPE_SECURE);
    aidRoutingPlanner_ = std::make_shared<AidRoutingPlanner>(std::move(policy), aidRoutingAdapter_);

    serviceInfoManager_ =
        std::make_shared<CardEmulationServiceInfoManager>(aidRoutingPlanner_, std::make_shared<CeServiceGetter>());

    auto rv = serviceInfoManager_->Init();

    eventHandler_ = std::make_shared<CardEmulationEventHandler>(ceDeviceHost_, aidRoutingPlanner_);
    inited_ = true;
    return rv;
}

int nfc::cardemulation::CardEmulationService::Deinit()
{
    if (serviceInfoManager_) {
        serviceInfoManager_->Deinit();
    }
    return ERR_OK;
}

int CardEmulationService::OnUserSwitched(int userId)
{
    if (!IsInited()) {
        return ERR_CARD_EMULATION_SERVICE_NOT_INIT;
    }
    return serviceInfoManager_->OnUserSwitched(userId);
}
int CardEmulationService::RegisterRemoteObject(int userId,
                                               const OHOS::AppExecFwk::ElementName& elementName,
                                               const OHOS::sptr<OHOS::IRemoteObject>& apduChannel)
{
    if (!IsInited()) {
        return ERR_CARD_EMULATION_SERVICE_NOT_INIT;
    }

    return serviceInfoManager_->RegisterRemoteObject(userId, elementName, apduChannel);
}

int CardEmulationService::AddAidsForService(int userId,
                                            const OHOS::AppExecFwk::ElementName& elementName,
                                            const std::string& type,
                                            const std::vector<std::string>& aidSet)
{
    if (!IsInited()) {
        return ERR_CARD_EMULATION_SERVICE_NOT_INIT;
    }

    return serviceInfoManager_->AddAidSet(userId, elementName, type, aidSet);
}

int CardEmulationService::RemoveAidsForService(int userId,
                                               const OHOS::AppExecFwk::ElementName& elementName,
                                               const std::string& type)
{
    if (!IsInited()) {
        return ERR_CARD_EMULATION_SERVICE_NOT_INIT;
    }

    return serviceInfoManager_->RemoveAidSet(userId, elementName, type);
}

int CardEmulationService::MarkOffHostForService(int userId,
                                                const OHOS::AppExecFwk::ElementName& elementName,
                                                const std::string& secureElementName)
{
    if (!IsInited()) {
        return ERR_CARD_EMULATION_SERVICE_NOT_INIT;
    }

    std::string loc = aidRoutingAdapter_->ReviseSecureElementName(secureElementName);
    if (loc.empty()) {
        return ERR_INVALID_EXEUCTION_ENVIRONMENT;
    }

    return serviceInfoManager_->MarkOffHostCEService(userId, elementName, loc);
}

int CardEmulationService::UnmarkOffHostForService(int userId, const OHOS::AppExecFwk::ElementName& elementName)
{
    if (!IsInited()) {
        return ERR_CARD_EMULATION_SERVICE_NOT_INIT;
    }

    return serviceInfoManager_->UnmarkOffHostCEService(userId, elementName);
}

bool CardEmulationService::SupportsAidPrefix(void) const
{
    if (!IsInited()) {
        return false;
    }
    auto mode = aidRoutingAdapter_->GetAidRoutingMode();
    DebugLog("GetAidRoutingMode: %d", mode);
    return IsAidPrefixMode(mode);
}

int CardEmulationService::SetPreferredService(int userId, const OHOS::AppExecFwk::ElementName& elementName)
{
    if (!IsInited()) {
        return ERR_CARD_EMULATION_SERVICE_NOT_INIT;
    }
    return serviceInfoManager_->SetPreferredCEService(userId, elementName);
}

int CardEmulationService::UnsetPreferredService(int userId)
{
    if (!IsInited()) {
        return ERR_CARD_EMULATION_SERVICE_NOT_INIT;
    }
    return serviceInfoManager_->UnsetPreferredCEService(userId);
}

int CardEmulationService::OnHCEActivated()
{
    if (eventHandler_) {
        eventHandler_->OnHCEActivated();

        return ERR_OK;
    }
    return ERR_NULL_POINTER;
}

int CardEmulationService::OnHCEData(const unsigned char* data, size_t len)
{
    if (eventHandler_) {
        return eventHandler_->OnHCEData(data, len);
    }
    return ERR_NULL_POINTER;
}

int CardEmulationService::OnHCEDeactivated()
{
    if (eventHandler_) {
        eventHandler_->OnHCEDeactivated();
        return ERR_OK;
    }
    return ERR_NULL_POINTER;
}

int CardEmulationService::OnOffHostTransaction(void)
{
    if (eventHandler_) {
        eventHandler_->OnOffHostTransaction();
        return ERR_OK;
    }
    return ERR_NULL_POINTER;
}

bool CardEmulationService::IsDefaultServiceForAid(int userId,
                                                  const OHOS::AppExecFwk::ElementName& elementName,
                                                  const std::string& aid) const
{
    if (!IsInited()) {
        return IS_OK(ERR_CARD_EMULATION_SERVICE_NOT_INIT);
    }

    return serviceInfoManager_->IsDefaultCEServiceForAid(userId, elementName, aid);
}

bool CardEmulationService::IsDefaultServiceForType(int userId,
                                                   const OHOS::AppExecFwk::ElementName& elementName,
                                                   const std::string& aid) const
{
    if (!IsInited()) {
        return IS_OK(ERR_CARD_EMULATION_SERVICE_NOT_INIT);
    }

    return serviceInfoManager_->IsDefaultCEServiceForType(userId, elementName, aid);
}

std::vector<std::string> CardEmulationService::GetAidsForService(int userId,
                                                                 const OHOS::AppExecFwk::ElementName& elementName,
                                                                 const std::string& type) const
{
    if (!IsInited()) {
        return std::vector<std::string>();
    }

    return serviceInfoManager_->GetAidsForCEService(userId, elementName, type);
}

OHOS::AppExecFwk::ElementName nfc::cardemulation::CardEmulationService::GetPrimaryServiceForType(
    int userId,
    const std::string& type)
{
    if (!IsInited()) {
        return OHOS::AppExecFwk::ElementName();
    }

    return serviceInfoManager_->GetPrimaryServiceForType(userId, type);
}

std::string nfc::cardemulation::CardEmulationService::GetDescriptionForPreferredService(int userId, std::string type)
{
    if (!IsInited()) {
        return std::string();
    }

    auto info = serviceInfoManager_->GetUserPreferred(userId);
    if (info) {
        return info->GetIcon();
    }
    return std::string();
}

int CardEmulationService::SetPrimaryServiceForType(int userId,
                                                   const OHOS::AppExecFwk::ElementName& elementName,
                                                   const std::string& type)
{
    if (!IsInited()) {
        return ERR_CARD_EMULATION_SERVICE_NOT_INIT;
    }

    return serviceInfoManager_->SetDefaultCEServiceForType(userId, elementName, type);
}

void CardEmulationService::OnNfcEnabled()
{
    if (serviceInfoManager_) {
        serviceInfoManager_->Init();
    }
}
void CardEmulationService::OnNfcDisabled()
{
    if (serviceInfoManager_) {
        serviceInfoManager_->Deinit();
    }
}
void CardEmulationService::OnSecureNfcToggled()
{
}
std::vector<std::unique_ptr<OHOS::nfc::sdk::cardemulation::CardEmulationServiceInfoLite>>
CardEmulationService::GetServicesByType(int userId, const std::string& type)
{
    if (!IsInited()) {
        return std::vector<std::unique_ptr<OHOS::nfc::sdk::cardemulation::CardEmulationServiceInfoLite>>();
    }
    return serviceInfoManager_->GetServicesByType(userId, type);
}
bool nfc::cardemulation::CardEmulationService::IsInited() const
{
    return inited_;
}
}  // namespace OHOS::nfc::cardemulation
