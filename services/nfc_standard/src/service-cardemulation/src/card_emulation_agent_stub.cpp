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

#include "card_emulation_agent_stub.h"

#include <algorithm>

#include "card_emulation_error.h"
#include "card_emulation_service_info_lite.h"
#include "card_emulation_util.h"
#include "element_name.h"
#include "loghelper.h"

#ifdef USE_HILOG
DEFINE_NFC_LOG_LABEL("CardEmulationAgentStub");
#endif

using namespace OHOS::nfc::sdk::cardemulation;
namespace OHOS::nfc::cardemulation {
#define DECLARE_CARDEMULATION_ADAPTER_HANDLER(code, func) \
    handlers_[(code)] = [this](OHOS::MessageParcel& data, OHOS::MessageParcel& reply) { this->func(data, reply); };

int CardEmulationAgentStub::OnRemoteRequest(uint32_t code,
                                            OHOS::MessageParcel& data,
                                            OHOS::MessageParcel& reply,
                                            OHOS::MessageOption& option)
{
    DebugLog("CardEmulationAgentStub::OnRemoteRequest, code: %d", code);
    if (!CheckHeader(data)) {
        WriteHeader(reply, ERR_INVALID_REQUEST_HEADER);
        return ERR_OK;
    }
    auto it = handlers_.find(code);
    if (it == handlers_.end()) {
        WriteHeader(reply, ERR_UNSURPPORTED);
        return ERR_OK;
    }
    it->second(data, reply);
    return ERR_OK;
}

void CardEmulationAgentStub::Init()
{
    DECLARE_CARDEMULATION_ADAPTER_HANDLER(CODE_GET_AIDS_FOR_SERVICE, OnGetAidsForService);
    DECLARE_CARDEMULATION_ADAPTER_HANDLER(CODE_ADD_AIDS_FOR_SERVICE, OnAddAidsForService);
    DECLARE_CARDEMULATION_ADAPTER_HANDLER(CODE_REMOVE_AIDS_FOR_SERVICE, OnRemoveAidsForService);
    DECLARE_CARDEMULATION_ADAPTER_HANDLER(CODE_REGISTER_OFF_HOST_SERVICE, OnRegisterOffHostService);
    DECLARE_CARDEMULATION_ADAPTER_HANDLER(CODE_UNREGISTER_OFF_HOST_SERVICE, OnUnregisterOffHostService);
    DECLARE_CARDEMULATION_ADAPTER_HANDLER(CODE_IS_SUPPORTED_AID_PREFIX_MODE, OnIsSupportedAidPrefixMode);
    DECLARE_CARDEMULATION_ADAPTER_HANDLER(CODE_GET_DESCRIPTION_FOR_PREFERRED_SERVICE,
                                          OnGetDescriptionForPreferredService);
    DECLARE_CARDEMULATION_ADAPTER_HANDLER(CODE_SET_PREFERRED_FOREGROUND_SERVICE, OnSetPreferredForegroundService);
    DECLARE_CARDEMULATION_ADAPTER_HANDLER(CODE_UNSET_PREFERRED_FOREGROUND_SERVICE, OnUnsetPreferredForegroundService);
    DECLARE_CARDEMULATION_ADAPTER_HANDLER(CODE_GET_PRIMARY_SERVICE_FOR_TYPE, OnGetPrimaryServiceForType);
    DECLARE_CARDEMULATION_ADAPTER_HANDLER(CODE_SET_PRIMARY_SERVICE_FOR_TYPE, OnSetPrimaryServiceForType);
    DECLARE_CARDEMULATION_ADAPTER_HANDLER(CODE_GET_SERVICES_BY_TYPE, OnGetServices);
    DECLARE_CARDEMULATION_ADAPTER_HANDLER(CODE_REGISTER_REMOTE_OBJECT, OnRegisterRemoteObject);
}

static void ReadServiceName(OHOS::MessageParcel& data, OHOS::AppExecFwk::ElementName& elementName)
{
    auto p = data.ReadParcelable<OHOS::AppExecFwk::ElementName>();
    if (p) {
        elementName = *p;
    }
    return;
}
void CardEmulationAgentStub::OnGetServices(OHOS::MessageParcel& data, OHOS::MessageParcel& reply)
{
    using namespace OHOS::nfc::sdk::cardemulation;
    int userId = data.ReadInt32();
    const std::string& type = data.ReadString();
    std::vector<std::unique_ptr<CardEmulationServiceInfoLite>> result;
    auto rv = GetServices(userId, type, result);
    WriteHeader(reply, rv);
    reply.WriteInt16(static_cast<int16_t>(result.size()));
    std::for_each(result.begin(), result.end(), [&reply](decltype(result)::reference r) {
        reply.WriteParcelable(r.get());
    });
    DebugLog("GetServices(\n\tuserId : % d, type: %s, result&size: %zu) return: %d ",
             userId,
             type.c_str(),
             result.size(),
             rv);
}
void CardEmulationAgentStub::OnGetAidsForService(OHOS::MessageParcel& data, OHOS::MessageParcel& reply)
{
    int userId = data.ReadInt32();

    OHOS::AppExecFwk::ElementName elementName;
    ReadServiceName(data, elementName);
    const std::string& type = data.ReadString();
    std::vector<std::string> aids;
    DebugLog("GetAidsForService(\n\tuserId: %d,  \n\tabilityName: %s, \n\ttype: %s, \n\taids&: %s) ",
             userId,
             elementName.GetAbilityName().c_str(),
             type.c_str(),
             StringVectorToString(aids).c_str());
    auto rv = GetAidsForService(userId, elementName, type, aids);
    DebugLog("GetAidsForService return %d", rv);
    WriteHeader(reply, rv);
    reply.WriteStringVector(aids);
}
void CardEmulationAgentStub::OnRegisterRemoteObject(OHOS::MessageParcel& data, OHOS::MessageParcel& reply)
{
    int userId = data.ReadInt32();
    OHOS::AppExecFwk::ElementName elementName;
    ReadServiceName(data, elementName);
    OHOS::sptr<OHOS::IRemoteObject> ro(data.ReadParcelable<IRemoteObject>());
    auto rv = RegisterRemoteObject(userId, elementName, ro);

    DebugLog("RegisterRemoteObject(\n\tuserId: %d, \n\tabilityName: %s, \n\tremoteObject: %p) ",
             userId,
             elementName.GetAbilityName().c_str(),
             ro.GetRefPtr());
    WriteHeader(reply, rv);
}
void CardEmulationAgentStub::OnAddAidsForService(OHOS::MessageParcel& data, OHOS::MessageParcel& reply)
{
    int userId = data.ReadInt32();
    OHOS::AppExecFwk::ElementName elementName;
    ReadServiceName(data, elementName);
    std::vector<std::string> aids;
    data.ReadStringVector(&aids);
    std::string type = data.ReadString();
    OHOS::sptr<OHOS::IRemoteObject> ro(data.ReadParcelable<IRemoteObject>());
    DebugLog("AddAidsForService(\n\tuserId: %d, \n\tabilityName: %s, \n\taids: %s, \n\ttype: %s) ",
             userId,
             elementName.GetAbilityName().c_str(),
             StringVectorToString(aids).c_str(),
             type.c_str());
    auto rv = AddAidsForService(userId, elementName, std::move(aids), std::move(type));
    DebugLog("AddAidsForService return %d", rv);
    WriteHeader(reply, rv);
}
void CardEmulationAgentStub::OnRemoveAidsForService(OHOS::MessageParcel& data, OHOS::MessageParcel& reply)
{
    int userId = data.ReadInt32();
    OHOS::AppExecFwk::ElementName elementName;
    ReadServiceName(data, elementName);
    std::string type = data.ReadString();
    DebugLog("RemoveAidsForService(\n\tuserId: %d, \n\tabilityName: %s, \n\ttype: %s) ",
             userId,
             elementName.GetAbilityName().c_str(),
             type.c_str());
    auto rv = RemoveAidsForService(userId, elementName, std::move(type));
    DebugLog("RemoveAidsForService return %d", rv);

    WriteHeader(reply, rv);
}
void CardEmulationAgentStub::OnRegisterOffHostService(OHOS::MessageParcel& data, OHOS::MessageParcel& reply)
{
    int userId = data.ReadInt32();
    OHOS::AppExecFwk::ElementName elementName;
    ReadServiceName(data, elementName);
    std::string offHostSecureElement = data.ReadString();
    DebugLog("RegisterOffHostService(\n\tuserId: %d, \n\tabilityName: %s, \n\tEE: %s)",
             userId,
             elementName.GetAbilityName().c_str(),
             offHostSecureElement.c_str());
    auto rv = RegisterOffHostService(userId, elementName, std::move(offHostSecureElement));
    DebugLog("RegisterOffHostService return %d", rv);

    WriteHeader(reply, rv);
}
void CardEmulationAgentStub::OnUnregisterOffHostService(OHOS::MessageParcel& data, OHOS::MessageParcel& reply)
{
    int userId = data.ReadInt32();
    OHOS::AppExecFwk::ElementName elementName;
    ReadServiceName(data, elementName);
    auto rv = UnregisterOffHostService(userId, elementName);
    DebugLog("UnregisterOffHostService(\n\tuserId: %d, \n\tabilityName: %s ) return: %d",
             userId,
             elementName.GetAbilityName().c_str(),
             rv);

    WriteHeader(reply, rv);
}
void CardEmulationAgentStub::OnIsSupportedAidPrefixMode(OHOS::MessageParcel& data, OHOS::MessageParcel& reply)
{
    bool result;
    auto rv = IsSupportedAidPrefixMode(result);
    WriteHeader(reply, rv);
    DebugLog("IsSupportedAidPrefixMode(\n\result&: %d) return: %d", result, rv);

    reply.WriteBool(result);
}
void CardEmulationAgentStub::OnGetDescriptionForPreferredService(OHOS::MessageParcel& data, OHOS::MessageParcel& reply)
{
    int userId = data.ReadInt32();
    std::string type = data.ReadString();
    std::string desc;
    DebugLog("GetDescriptionForPreferredService(\n\tuserId: %d, \n\ttype: %s, \n\tdesc&: %s ) ",
             userId,
             type.c_str(),
             desc.c_str());
    auto rv = GetDescriptionForPreferredService(userId, std::move(type), desc);
    DebugLog("GetDescriptionForPreferredService return %d", rv);

    WriteHeader(reply, rv);

    reply.WriteString(desc);
}
void CardEmulationAgentStub::OnSetPreferredForegroundService(OHOS::MessageParcel& data, OHOS::MessageParcel& reply)
{
    int userId = data.ReadInt32();
    OHOS::AppExecFwk::ElementName elementName;
    ReadServiceName(data, elementName);
    auto rv = SetPreferredForegroundService(userId, elementName);
    DebugLog("SetPreferredForegroundService(\n\tuserId: %d, \n\tabilityName: %s ) return: %d",
             userId,
             elementName.GetAbilityName().c_str(),
             rv);

    WriteHeader(reply, rv);
}
void CardEmulationAgentStub::OnUnsetPreferredForegroundService(OHOS::MessageParcel& data, OHOS::MessageParcel& reply)
{
    int userId = data.ReadInt32();
    auto rv = UnsetPreferredForegroundService(userId);
    DebugLog("SetPreferredForegroundService(\n\tuserId: %d) return: %d", userId, rv);

    WriteHeader(reply, rv);
}
void CardEmulationAgentStub::OnGetPrimaryServiceForType(OHOS::MessageParcel& data, OHOS::MessageParcel& reply)
{
    int userId = data.ReadInt32();
    const std::string& type = data.ReadString();
    OHOS::AppExecFwk::ElementName result;
    auto rv = GetPrimaryServiceForType(userId, type, result);
    DebugLog("GetPrimaryServiceForType(\n\tuserId: %d, \n\type: %s, abilityName&: %s ) return: %d",
             userId,
             type.c_str(),
             result.GetAbilityName().c_str(),
             rv);

    WriteHeader(reply, rv);
    reply.WriteParcelable(&result);
}
void CardEmulationAgentStub::OnSetPrimaryServiceForType(OHOS::MessageParcel& data, OHOS::MessageParcel& reply)
{
    int userId = data.ReadInt32();
    OHOS::AppExecFwk::ElementName elementName;
    ReadServiceName(data, elementName);
    const std::string& type = data.ReadString();
    auto rv = SetPrimaryServiceForType(userId, elementName, type);
    DebugLog("SetPrimaryServiceForType(\n\tuserId: %d, \n\ttype: %s, abilityName&: %s ) return: %d",
             userId,
             elementName.GetAbilityName().c_str(),
             type.c_str(),
             rv);

    WriteHeader(reply, rv);
}
void CardEmulationAgentStub::WriteHeader(OHOS::MessageParcel& reply, int statusCode) const
{
    reply.WriteInt32(statusCode);
}
bool CardEmulationAgentStub::CheckHeader(OHOS::MessageParcel& data) const
{
    return true;
}
}  // namespace OHOS::nfc::cardemulation
