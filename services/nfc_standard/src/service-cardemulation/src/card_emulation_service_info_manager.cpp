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

#include "card_emulation_service_info_manager.h"

#include <assert.h>

#include <algorithm>

#include "aid_set.h"
#include "bundle_notification_stub.h"
#include "card_emulation_def.h"
#include "card_emulation_error.h"
#include "card_emulation_service_info.h"
#include "card_emulation_service_info_lite.h"
#include "card_emulation_util.h"
#include "element_name_util.h"
#include "iaid_routing_manager.h"
#include "ipc_skeleton.h"
#include "langinfo.h"
#include "loghelper.h"
#include "setting_changed_event_subscriber.h"

#ifdef USE_HILOG
DEFINE_NFC_LOG_LABEL("CardEmulationServiceInfoManager");
#endif

static constexpr int INVALID_USER_ID = -1;
using namespace OHOS::nfc::sdk::cardemulation;
namespace OHOS::nfc::cardemulation {
static std::shared_ptr<CardEmulationServiceInfo> FindCEServiceInfoByName(
    const OHOS::AppExecFwk::ElementName& serviceName,
    std::vector<std::shared_ptr<CardEmulationServiceInfo>>& infos)
{
    auto pos =
        std::find_if(std::begin(infos), std::end(infos), [&serviceName](decltype(std::begin(infos))::reference r) {
            if (!r) {
                return false;
            }
            auto name = r->GetName();
            if (name) {
                return *name == serviceName;
            }
            return false;
        });
    if (pos == infos.end()) {
        return std::shared_ptr<CardEmulationServiceInfo>();
    }
    return *pos;
}

CardEmulationServiceInfoManager::CardEmulationServiceInfoManager(
    std::shared_ptr<IAidRoutingManager> ob,
    std::shared_ptr<InstalledCeServiceGetter> ceserviceinfo_getter)
    : userCEServices_(),
    userPreferredCEServices_(),
    userDefaultCEService_(),
    mu_(),
    routingManager_(ob),
    serviceinfoGetter_(ceserviceinfo_getter),
    lastUserId_(INVALID_USER_ID)
{
}

CardEmulationServiceInfoManager::~CardEmulationServiceInfoManager()
{
}
int CardEmulationServiceInfoManager::Init()
{
    SettingChangedEventSubscriber::Subscribe([this](const OHOS::AppExecFwk::ElementName& name) {
        SetDefaultCEServiceForType(GetCurrentUserId(), name, CARDEMULATION_SERVICE_TYPE_SECURE);
    });
#ifdef MOCK_FOR_TESTING

    auto stub = std::make_shared<BundleNotificationStub>(
        [this](bool installing, const std::string& bundleName) { OnServiceUpdated(GetCurrentUserId()); });
    BundleChangedEventSubscriber::Subscribe(stub);
#endif
    return OnServiceUpdated(GetCurrentUserId());
}
int CardEmulationServiceInfoManager::Deinit()
{
    SettingChangedEventSubscriber::Subscribe([](const OHOS::AppExecFwk::ElementName&) {});

    return StoreDynamicCEServiceInfo(GetCurrentUserId());
}
std::vector<std::unique_ptr<CardEmulationServiceInfoLite>> CardEmulationServiceInfoManager::GetServicesByType(
    int userId,
    const std::string& type)
{
    std::vector<std::unique_ptr<CardEmulationServiceInfoLite>> rv;
    if (userId != GetCurrentUserId()) {
        return rv;
    }
    if (!IsValidType(type)) {
        return rv;
    }
    std::lock_guard<std::mutex> locker(mu_);
    auto it = userCEServices_.find(userId);
    if (it == userCEServices_.end()) {
        return rv;
    }

    std::for_each(it->second.begin(),
                  it->second.end(),
                  [&rv, &type](std::vector<std::shared_ptr<CardEmulationServiceInfo>>::reference r) {
                      if (r && r->HasType(type)) {
                          rv.emplace_back(CardEmulationServiceInfo::GetLiteInfo(*r));
                      }
                  });

    return rv;
}

int CardEmulationServiceInfoManager::RegisterRemoteObject(int userId,
                                                          const OHOS::AppExecFwk::ElementName& elementName,
                                                          const OHOS::sptr<OHOS::IRemoteObject>& apduChannel)
{
    DebugLog("RegisterRemoteObject, %d , GetCurrentUserId: %d, channel: %p\n",
             userId,
             GetCurrentUserId(),
             apduChannel.GetRefPtr());

    if (userId != GetCurrentUserId()) {
        return ERR_INVALID_USERID;
    }
    if (ElementNameUtil::IsElementNameEmpty(elementName)) {
        return ERR_INVALID_ELEMENT_NAME;
    }
    std::lock_guard<std::mutex> locker(mu_);
    if (!IsCEServiceInstalled(userId, elementName)) {
        return ERR_NOT_FOUND_CESERVICE_INFO;
    }

    auto serviceInfo = GetOrCreateAppInfo(userId, elementName);
    assert(serviceInfo);
    serviceInfo->SetApduChannel(apduChannel);
    return ERR_OK;
}
int CardEmulationServiceInfoManager::AddAidSet(int userId,
                                               const OHOS::AppExecFwk::ElementName& elementName,
                                               const std::string& type,
                                               const std::vector<std::string>& aidSet)
{
    DebugLog("AddAidSet, %d , GetCurrentUserId: %d, type: %s\n", userId, GetCurrentUserId(), type.c_str());
    if (userId != GetCurrentUserId()) {
        return ERR_INVALID_USERID;
    }
    if (ElementNameUtil::IsElementNameEmpty(elementName)) {
        return ERR_INVALID_ELEMENT_NAME;
    }
    if (!IsValidType(type)) {
        DebugLog("ERR_INVALID_SERVICE_TYPE: %s\n", type.c_str());
        return ERR_INVALID_SERVICE_TYPE;
    }
    auto aids = AidSet::FromRawString(aidSet);
    if (!aids || aids->Empty()) {
        return ERR_AIDSET_IS_EMPTY;
    }
    std::lock_guard<std::mutex> locker(mu_);
    if (!IsCEServiceInstalled(userId, elementName)) {
        return ERR_NOT_FOUND_CESERVICE_INFO;
    }

    auto serviceInfo = GetOrCreateAppInfo(userId, elementName);
    assert(serviceInfo);

    aids->SetOwner(serviceInfo);
    aids->SetType(type);
    if (serviceInfo->AddAidset(std::move(aids))) {
        StoreDynamicCEServiceInfoWithoutLock(GetCurrentUserId());
        NotifyCEServiceUpdated(userId);
        return ERR_OK;
    }
    return ERR_AIDSET_IS_EMPTY;
}

int CardEmulationServiceInfoManager::RemoveAidSet(int userId,
                                                  const OHOS::AppExecFwk::ElementName& elementName,
                                                  const std::string& type)
{
    if (userId != GetCurrentUserId()) {
        return ERR_INVALID_USERID;
    }
    if (ElementNameUtil::IsElementNameEmpty(elementName)) {
        return ERR_INVALID_ELEMENT_NAME;
    }
    if (!IsValidType(type)) {
        return ERR_INVALID_SERVICE_TYPE;
    }

    std::lock_guard<std::mutex> locker(mu_);
    std::shared_ptr<CardEmulationServiceInfo> result;
    auto rv = FetchServiceInfoByName(result, userId, elementName);
    if (IS_NOK(rv) || !result) {
        return ERR_NOT_FOUND_CESERVICE_INFO;
    }

    result->RemoveAidset(type);
    StoreDynamicCEServiceInfoWithoutLock(GetCurrentUserId());

    NotifyCEServiceUpdated(userId);
    return ERR_OK;
}

int CardEmulationServiceInfoManager::MarkOffHostCEService(int userId,
                                                          const OHOS::AppExecFwk::ElementName& elementName,
                                                          const std::string& secureElementName)
{
    DebugLog("MarkOffHostCEService, ability name: %s, set env: %s",
             elementName.GetAbilityName().c_str(),
             secureElementName.c_str());
    if (userId != GetCurrentUserId()) {
        return ERR_INVALID_USERID;
    }
    if (ElementNameUtil::IsElementNameEmpty(elementName)) {
        return ERR_INVALID_ELEMENT_NAME;
    }
    std::lock_guard<std::mutex> locker(mu_);
    std::shared_ptr<CardEmulationServiceInfo> result;
    auto rv = FetchServiceInfoByName(result, userId, elementName);
    if (IS_NOK(rv) || !result) {
        return ERR_NOT_FOUND_CESERVICE_INFO;
    }
    rv = result->SetExecutionEnvironment(secureElementName);
    if (IS_NOK(rv)) {
        return rv;
    }
    NotifyCEServiceUpdated(userId);

    return rv;
}

int CardEmulationServiceInfoManager::UnmarkOffHostCEService(int userId,
                                                            const OHOS::AppExecFwk::ElementName& elementName)
{
    return MarkOffHostCEService(userId, elementName, "");
}

int CardEmulationServiceInfoManager::SetPreferredCEService(int userId, const OHOS::AppExecFwk::ElementName& elementName)
{
    if (userId != GetCurrentUserId()) {
        return ERR_INVALID_USERID;
    }
    if (ElementNameUtil::IsElementNameEmpty(elementName)) {
        return ERR_INVALID_ELEMENT_NAME;
    }
    std::shared_ptr<CardEmulationServiceInfo> abilityInfo;
    {
        std::lock_guard<std::mutex> locker(mu_);
        auto rv = FetchServiceInfoByName(abilityInfo, userId, elementName);
        if (IS_NOK(rv) || !abilityInfo) {
            return rv;
        }
    }
    // found
    if (abilityInfo->HasType(CARDEMULATION_SERVICE_TYPE_SECURE)) {
        // confilicts with secure service
        return ERR_CONFILICTS_WITH_DEFAULT;
    }
    if (ConfilictsWithDefault(userId, abilityInfo)) {
        return ERR_CONFILICTS_WITH_DEFAULT;
    }
    std::lock_guard<std::mutex> locker(mu_);
    userPreferredCEServices_[userId] = abilityInfo;
    NotifyCEServiceUpdated(userId);
    return ERR_OK;
}

int CardEmulationServiceInfoManager::UnsetPreferredCEService(int userId)
{
    if (userId != GetCurrentUserId()) {
        return ERR_INVALID_USERID;
    }
    std::lock_guard<std::mutex> locker(mu_);
    userPreferredCEServices_[userId] = std::weak_ptr<CardEmulationServiceInfo>();
    NotifyCEServiceUpdated(userId);
    return ERR_OK;
}

int CardEmulationServiceInfoManager::OnUserSwitched(int userId)
{
    if (lastUserId_ != INVALID_USER_ID) {
        StoreDynamicCEServiceInfo(lastUserId_);
        lastUserId_ = userId;
    }

    return OnServiceUpdated(userId);
}

int CardEmulationServiceInfoManager::OnServiceUpdated(int userId)
{
    std::lock_guard<std::mutex> locker(mu_);
    int rv = ParseCEAbilityInfo(userId);
    if (IS_OK(rv)) {
        NotifyCEServiceUpdated(userId);
    }
    return rv;
}

std::shared_ptr<CardEmulationServiceInfo> CardEmulationServiceInfoManager::GetUserPreferred(int userId)
{
    if (userId != GetCurrentUserId()) {
        return std::shared_ptr<CardEmulationServiceInfo>();
    }
    std::lock_guard<std::mutex> locker(mu_);

    return GetUserPreferredWithoutLock(userId);
}

bool CardEmulationServiceInfoManager::IsDefaultCEServiceForAid(int userId,
                                                               const OHOS::AppExecFwk::ElementName& elementName,
                                                               const std::string& aid)
{
    if (userId != GetCurrentUserId()) {
        return false;
    }
    if (ElementNameUtil::IsElementNameEmpty(elementName)) {
        return false;
    }
    if (!routingManager_) {
        return false;
    }
    std::vector<ServiceInfoTypePair> pair = routingManager_->GetCardEmulationServicesByAid(aid);
    if (pair.empty()) {
        return false;
    }
    auto info = pair.front().first.lock();
    if (!info) {
        return false;
    }
    return *(info->GetName()) == elementName;
}

bool CardEmulationServiceInfoManager::IsDefaultCEServiceForType(int userId,
                                                                const OHOS::AppExecFwk::ElementName& elementName,
                                                                const std::string& type)
{
    if (userId != GetCurrentUserId()) {
        return false;
    }
    if (!IsSecureType(type)) {
        return false;
    }
    if (ElementNameUtil::IsElementNameEmpty(elementName)) {
        return false;
    }
    std::lock_guard<std::mutex> locker(mu_);
    auto it = userDefaultCEService_.find(userId);
    if (it == userDefaultCEService_.end()) {
        return false;
    }
    auto abilityInfo = it->second.lock();
    if (!abilityInfo) {
        return false;
    }
    return ElementNameUtil::ElementNameEquals(abilityInfo->GetName(), elementName);
}

std::vector<std::string> CardEmulationServiceInfoManager::GetAidsForCEService(
    int userId,
    const OHOS::AppExecFwk::ElementName& elementName,
    const std::string& type)
{
    std::vector<std::string> rv;
    if (userId != GetCurrentUserId()) {
        return rv;
    }
    if (ElementNameUtil::IsElementNameEmpty(elementName)) {
        return rv;
    }
    if (!IsValidType(type)) {
        return rv;
    }

    std::lock_guard<std::mutex> locker(mu_);
    auto it = userCEServices_.find(userId);
    if (it == userCEServices_.end()) {
        return rv;
    }
    std::shared_ptr<CardEmulationServiceInfo> abilityInfo;

    auto r = FetchServiceInfoByName(abilityInfo, userId, elementName);
    if (IS_NOK(r)) {
        return rv;
    }
    abilityInfo->Visit([&type, &rv](const std::string& tagName, std::shared_ptr<AidSet> aidset) {
        if (type == tagName) {
            rv = aidset->GetAllAidRawString();
        }
    });
    return rv;
}

OHOS::AppExecFwk::ElementName CardEmulationServiceInfoManager::GetPrimaryServiceForType(int userId,
                                                                                        const std::string& type)
{
    if (userId != GetCurrentUserId() || IsNormalType(type)) {
        return OHOS::AppExecFwk::ElementName();
    }
    std::shared_ptr<CardEmulationServiceInfo> abilityInfo;
    {
        std::lock_guard<std::mutex> locker(mu_);
        auto it = userDefaultCEService_.find(userId);
        if (it == userDefaultCEService_.end()) {
            return OHOS::AppExecFwk::ElementName();
        }

        abilityInfo = it->second.lock();
    }
    if (!abilityInfo) {
        return OHOS::AppExecFwk::ElementName();
    }
    auto name = abilityInfo->GetName();
    if (!name) {
        return OHOS::AppExecFwk::ElementName();
    }
    return *name;
}

int CardEmulationServiceInfoManager::SetDefaultCEServiceForType(int userId,
                                                                const OHOS::AppExecFwk::ElementName& elementName,
                                                                const std::string& type)
{
    if (userId != GetCurrentUserId()) {
        return ERR_INVALID_USERID;
    }
    if (ElementNameUtil::IsElementNameEmpty(elementName)) {
        return ERR_INVALID_ELEMENT_NAME;
    }
    if (!IsSecureType(type)) {
        return ERR_INVALID_SERVICE_TYPE;
    }

    std::shared_ptr<CardEmulationServiceInfo> abilityInfo;
    {
        //  elementName exists
        std::lock_guard<std::mutex> locker(mu_);
        auto it = userCEServices_.find(userId);
        if (it == userCEServices_.end()) {
            return ERR_NOT_FOUND_CESERVICE_INFO;
        }

        auto r = FetchServiceInfoByName(abilityInfo, userId, elementName);
        if (IS_NOK(r)) {
            return r;
        }
    }
    StoreDefaultCEServiceForType(userId, elementName, type);
    std::lock_guard<std::mutex> locker(mu_);
    if (abilityInfo) {
        DebugLog("SetDefaultCEServiceForType, ability name: %s", abilityInfo->GetName()->GetAbilityName().c_str());
        userDefaultCEService_[userId] = abilityInfo;
        NotifyCEServiceUpdated(userId);
    }
    return ERR_OK;
}

std::shared_ptr<CardEmulationServiceInfo> CardEmulationServiceInfoManager::GetOrCreateAppInfo(
    int userId,
    const OHOS::AppExecFwk::ElementName& elementName)
{
    if (ElementNameUtil::IsElementNameEmpty(elementName)) {
        return std::shared_ptr<CardEmulationServiceInfo>();
    }

    auto it = userCEServices_.find(userId);
    if (it == userCEServices_.end()) {
        std::vector<std::shared_ptr<CardEmulationServiceInfo>> appinfos;
        auto info = std::make_shared<CardEmulationServiceInfo>(NFC_EE_HOST);
        info->SetName(elementName);
        appinfos.push_back(info);
        userCEServices_[userId] = std::move(appinfos);

        return info;
    } else {
        auto found = FindCEServiceInfoByName(elementName, it->second);
        if (!found) {
            auto info = std::make_shared<CardEmulationServiceInfo>(NFC_EE_HOST);
            info->SetName(elementName);
            it->second.push_back(info);

            return info;
        }

        return found;
    }
}

void CardEmulationServiceInfoManager::NotifyCEServiceUpdated(int userId)
{
    if (routingManager_) {
        auto it = userCEServices_.find(userId);
        if (it == std::end(userCEServices_)) {
            routingManager_->OnCeServiceChanged(std::vector<std::shared_ptr<CardEmulationServiceInfo>>(),
                                                nullptr,
                                                nullptr);
            return;
        }

        auto d = GetUserDefaultWithoutLock(userId);
        auto p = GetUserPreferredWithoutLock(userId);

        routingManager_->OnCeServiceChanged(it->second, d, p);
    }
}
bool CardEmulationServiceInfoManager::IsCEServiceInstalled(int userId, const OHOS::AppExecFwk::ElementName& elementName)
{
    if (ElementNameUtil::IsElementNameEmpty(elementName)) {
        return false;
    }

    std::shared_ptr<CardEmulationServiceInfo> result;
    auto rv = FetchServiceInfoByName(result, userId, elementName);
    return IS_OK(rv);
}

int CardEmulationServiceInfoManager::ParseDefaultCEServiceForType(int userId)
{
    if (!serviceinfoGetter_) {
        return ERR_CESERVICE_GETTER_IS_NULL;
    }

    auto name = serviceinfoGetter_->GetDefaultElementName(userId);

    if (name == OHOS::AppExecFwk::ElementName()) {
        return ERR_OK;
    }
    std::shared_ptr<CardEmulationServiceInfo> result;
    auto rv = FetchServiceInfoByName(result, userId, name);
    if (IS_NOK(rv)) {
        DebugLog("failed to fetch info, name: %s, err: %d", name.GetAbilityName().c_str(), rv);
        return rv;
    }
    DebugLog("fetch info succeeded, name: %s", result->GetName()->GetAbilityName().c_str());

    userDefaultCEService_[userId] = std::move(result);
    return ERR_OK;
}

int CardEmulationServiceInfoManager::StoreDefaultCEServiceForType(int userId,
                                                                  const OHOS::AppExecFwk::ElementName& elementName,
                                                                  const std::string& type)
{
#ifdef MOCK_FOR_TESTING
    if (ElementNameUtil::IsElementNameEmpty(elementName)) {
        return ERR_INVALID_ELEMENT_NAME;
    }

    if (!serviceinfoGetter_) {
        return ERR_CESERVICE_GETTER_IS_NULL;
    }

    serviceinfoGetter_->StoreDefault(userId, elementName.GetAbilityName());
#endif
    return ERR_OK;
}

int CardEmulationServiceInfoManager::ParseCEAbilityInfo(int userId)
{
    DebugLog("parse CE ability info, userId: %d", userId);
    if (!serviceinfoGetter_) {
        return ERR_CESERVICE_GETTER_IS_NULL;
    }
    // clear
    userCEServices_[userId] = {};

    auto infos = serviceinfoGetter_->GetInstalled(userId);
    DebugLog("get installed. size: %u\n", infos.size());
    auto dynamicInfos = serviceinfoGetter_->GetDynamic(userId);
    DebugLog("get installed. dynamicInfos size: %u\n", dynamicInfos.size());
    for (auto& i : infos) {
        if (IS_OK(GetDynamicCEServiceInfo(i, dynamicInfos))) {
            userCEServices_[userId].emplace_back(std::shared_ptr<CardEmulationServiceInfo>(i));
        }
    }
    userCEServices_[userId] = infos;
    DebugLog("user: %d has %zu infos", userId, userCEServices_[userId].size());
    ParseDefaultCEServiceForType(userId);
    return ERR_OK;
}

std::shared_ptr<CardEmulationServiceInfo> CardEmulationServiceInfoManager::GetUserDefaultWithoutLock(int userId)
{
    auto posDefault = userDefaultCEService_.find(userId);
    if (posDefault == std::end(userDefaultCEService_)) {
        return std::shared_ptr<CardEmulationServiceInfo>();
    }
    return posDefault->second.lock();
}

int CardEmulationServiceInfoManager::GetDynamicCEServiceInfo(
    std::shared_ptr<CardEmulationServiceInfo>& info,
    const std::vector<std::shared_ptr<CardEmulationServiceInfo>>& dynamicInfos)
{
    auto it = std::find_if(dynamicInfos.cbegin(),
                           dynamicInfos.cend(),
                           [&info](std::vector<std::shared_ptr<CardEmulationServiceInfo>>::const_reference cr) {
                               if (info && cr) {
                                   printf("name1: %s, name2: %s\n",
                                          info->GetName()->GetAbilityName().c_str(),
                                          cr->GetName()->GetAbilityName().c_str());

                                   return info->GetName() == cr->GetName();
                               }
                               return false;
                           });
    if (it == dynamicInfos.cend()) {
        return ERR_NOT_FOUND_CESERVICE_INFO;
    }
    info = *it;
    return ERR_OK;
}

int CardEmulationServiceInfoManager::StoreDynamicCEServiceInfo(int userId)
{
    std::lock_guard<std::mutex> locker(mu_);
    return StoreDynamicCEServiceInfoWithoutLock(userId);
}
int CardEmulationServiceInfoManager::StoreDynamicCEServiceInfoWithoutLock(int userId)
{
    auto it = userCEServices_.find(userId);
    if (it == userCEServices_.end()) {
        return ERR_NOT_FOUND_USER_INFO;
    }
    serviceinfoGetter_->StoreDynamic(userId, it->second);
    return ERR_OK;
}
std::shared_ptr<CardEmulationServiceInfo> CardEmulationServiceInfoManager::GetUserPreferredWithoutLock(int userId)
{
    auto posPreferred = userPreferredCEServices_.find(userId);
    if (posPreferred == std::end(userPreferredCEServices_)) {
        return std::shared_ptr<CardEmulationServiceInfo>();
    }
    return posPreferred->second.lock();
}
int CardEmulationServiceInfoManager::GetCurrentUserId() const
{
    return IPCSkeleton::GetCallingUid();
}

int CardEmulationServiceInfoManager::FetchServiceInfosByType(
    std::vector<std::shared_ptr<CardEmulationServiceInfo>>& result,
    int userId,
    const std::string& type)
{
    return 0;
}

int CardEmulationServiceInfoManager::FetchServiceInfoByName(std::shared_ptr<CardEmulationServiceInfo>& result,
                                                            int userId,
                                                            const OHOS::AppExecFwk::ElementName& elementName)
{
    if (ElementNameUtil::IsElementNameEmpty(elementName)) {
        return ERR_INVALID_ELEMENT_NAME;
    }

    auto it = userCEServices_.find(userId);
    if (it == userCEServices_.end()) {
        return ERR_NOT_FOUND_USER_INFO;
    }

    result = FindCEServiceInfoByName(elementName, it->second);
    if (!result) {
        return ERR_NOT_FOUND_CESERVICE_INFO;
    }
    return ERR_OK;
}

bool CardEmulationServiceInfoManager::ConfilictsWithDefault(
    int userId,
    const std::shared_ptr<CardEmulationServiceInfo>& abilityInfo)
{
    std::lock_guard<std::mutex> locker(mu_);
    auto it = userDefaultCEService_.find(userId);
    if (it == userDefaultCEService_.end()) {
        return false;
    }
    auto defaultService = it->second.lock();
    if (!defaultService) {
        return false;
    }
    auto aidsets0 = defaultService->GetAidsetByType(CARDEMULATION_SERVICE_TYPE_SECURE);
    if (!aidsets0) {
        return false;
    }
    auto aidsets1 = abilityInfo->SortAidsets(CardEmulationServiceInfo::ALessB);

    for (auto& b : aidsets1) {
        if (b->ConflictWith(*aidsets0, false)) {
            return true;
        }
    }

    return false;
}
}  // namespace OHOS::nfc::cardemulation
