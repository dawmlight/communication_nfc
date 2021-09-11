/*
  * Copyright (C) 2021 Huawei Device Co., Ltd.
  * Licensed under the Apache License, Version 2.0 (the "License");

 * * you may not use this file except in compliance with the License.
  * You may obtain a copy of the License at
  *

 * *     http://www.apache.org/licenses/LICENSE-2.0
  *
  * Unless required by applicable law or agreed to in writing,
 * software
  * distributed under the License is distributed on an "AS IS" BASIS,
  * WITHOUT WARRANTIES OR CONDITIONS
 * OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  *
 * limitations under the License.
  */

#include "card_emulation_agent_proxy.h"

#include "card_emulation_service_info_lite.h"
#include "element_name.h"
#include "loghelper.h"
#include "nfc_sdk_common.h"
namespace OHOS::nfc::sdk::cardemulation {
CardEmulationAgentProxy::CardEmulationAgentProxy(const OHOS::sptr<OHOS::IRemoteObject>& ro)
    : OHOS::IRemoteProxy<ICardEmulationAgent>(ro)
{
}

int CardEmulationAgentProxy::GetServices(int userId,
                                         const std::string& type,
                                         std::vector<std::unique_ptr<CardEmulationServiceInfoLite>>& result)
{
    auto ro = Remote();
    if (!ro) {
        return NFC_SDK_ERROR_NOT_INITIALIZED;
    }

    OHOS::MessageOption option;
    OHOS::MessageParcel data;
    OHOS::MessageParcel reply;
    WriteHeader(data);
    data.WriteInt32(userId);
    data.WriteString(type);
    auto rv = ro->SendRequest(CODE_GET_SERVICES_BY_TYPE, data, reply, option);
    if (rv != NFC_SUCCESS) {
        return NFC_SER_ERROR_DISCONNECT;
    }
    rv = CheckHeader(reply);
    if (rv != NFC_SUCCESS) {
        return rv;
    }

    auto count = reply.ReadInt16();
    for (int16_t i = 0; i < count; i++) {
        result.emplace_back(
            std::unique_ptr<CardEmulationServiceInfoLite>(reply.ReadParcelable<CardEmulationServiceInfoLite>()));
    }
    return NFC_SUCCESS;
}

int CardEmulationAgentProxy::GetAidsForService(int userId,
                                               const OHOS::AppExecFwk::ElementName& serviceName,
                                               const std::string& type,
                                               std::vector<std::string>& result)
{
    auto ro = Remote();
    if (!ro) {
        return NFC_SDK_ERROR_NOT_INITIALIZED;
    }
    OHOS::MessageOption option;
    OHOS::MessageParcel data;
    OHOS::MessageParcel reply;
    WriteHeader(data);
    // write request body
    data.WriteInt32(userId);
    data.WriteParcelable(&serviceName);
    data.WriteString(type);
    auto rv = ro->SendRequest(CODE_GET_AIDS_FOR_SERVICE, data, reply, option);
    if (rv != NFC_SUCCESS) {
        return NFC_SER_ERROR_DISCONNECT;
    }
    rv = CheckHeader(reply);
    if (rv != NFC_SUCCESS) {
        return rv;
    }
    reply.ReadStringVector(&result);
    return rv;
}

int CardEmulationAgentProxy::RegisterRemoteObject(int userId,
                                                  const OHOS::AppExecFwk::ElementName& serviceName,
                                                  const OHOS::sptr<OHOS::IRemoteObject>& apduChannel)
{
    auto ro = Remote();
    if (!ro) {
        DebugLog("RegisterRemoteObject: Remote() is nullptr");
        return NFC_SDK_ERROR_NOT_INITIALIZED;
    }

    OHOS::MessageOption option;
    OHOS::MessageParcel data;
    OHOS::MessageParcel reply;
    WriteHeader(data);
    data.WriteInt32(userId);
    data.WriteParcelable(&serviceName);
    data.WriteParcelable(apduChannel.GetRefPtr());
    auto rv = ro->SendRequest(CODE_REGISTER_REMOTE_OBJECT, data, reply, option);
    if (rv != NFC_SUCCESS) {
        return NFC_SER_ERROR_DISCONNECT;
    }
    return CheckHeader(reply);
}

int CardEmulationAgentProxy::AddAidsForService(int userId,
                                               const OHOS::AppExecFwk::ElementName& serviceName,
                                               std::vector<std::string> aids,
                                               std::string type)
{
    auto ro = Remote();
    if (!ro) {
        DebugLog("AddAidsForService: Remote() is nullptr");
        return NFC_SDK_ERROR_NOT_INITIALIZED;
    }
    DebugLog("write msg");
    OHOS::MessageOption option;
    OHOS::MessageParcel data;
    OHOS::MessageParcel reply;
    WriteHeader(data);
    // write request body
    data.WriteInt32(userId);
    DebugLog("write service name");
    data.WriteParcelable(&serviceName);
    data.WriteStringVector(aids);
    data.WriteString(type);
    DebugLog("write apdu channel");

    DebugLog("send request %d", CODE_ADD_AIDS_FOR_SERVICE);
    auto rv = ro->SendRequest(CODE_ADD_AIDS_FOR_SERVICE, data, reply, option);
    DebugLog("send request %d, return: %d", CODE_ADD_AIDS_FOR_SERVICE, rv);
    if (rv != NFC_SUCCESS) {
        return NFC_SER_ERROR_DISCONNECT;
    }
    return CheckHeader(reply);
}

int CardEmulationAgentProxy::RemoveAidsForService(int userId,
                                                  const OHOS::AppExecFwk::ElementName& serviceName,
                                                  std::string type)
{
    auto ro = Remote();
    if (!ro) {
        return NFC_SDK_ERROR_NOT_INITIALIZED;
    }
    OHOS::MessageOption option;
    OHOS::MessageParcel data;
    OHOS::MessageParcel reply;
    WriteHeader(data);
    // write request body
    data.WriteInt32(userId);
    data.WriteParcelable(&serviceName);
    data.WriteString(type);
    auto rv = ro->SendRequest(CODE_REMOVE_AIDS_FOR_SERVICE, data, reply, option);
    if (rv != NFC_SUCCESS) {
        return NFC_SER_ERROR_DISCONNECT;
    }
    return CheckHeader(reply);
}

int CardEmulationAgentProxy::RegisterOffHostService(int userId,
                                                    const OHOS::AppExecFwk::ElementName& serviceName,
                                                    std::string offHostSecureElement)
{
    auto ro = Remote();
    if (!ro) {
        return NFC_SDK_ERROR_NOT_INITIALIZED;
    }
    OHOS::MessageOption option;
    OHOS::MessageParcel data;
    OHOS::MessageParcel reply;
    WriteHeader(data);
    // write request body
    data.WriteInt32(userId);
    data.WriteParcelable(&serviceName);
    data.WriteString(offHostSecureElement);
    auto rv = ro->SendRequest(CODE_REGISTER_OFF_HOST_SERVICE, data, reply, option);
    if (rv != NFC_SUCCESS) {
        return NFC_SER_ERROR_DISCONNECT;
    }
    return CheckHeader(reply);
}

int CardEmulationAgentProxy::UnregisterOffHostService(int userId, const OHOS::AppExecFwk::ElementName& serviceName)
{
    auto ro = Remote();
    if (!ro) {
        return NFC_SDK_ERROR_NOT_INITIALIZED;
    }
    OHOS::MessageOption option;
    OHOS::MessageParcel data;
    OHOS::MessageParcel reply;
    WriteHeader(data);
    // write request body
    data.WriteInt32(userId);
    data.WriteParcelable(&serviceName);
    auto rv = ro->SendRequest(CODE_UNREGISTER_OFF_HOST_SERVICE, data, reply, option);
    if (rv != NFC_SUCCESS) {
        return NFC_SER_ERROR_DISCONNECT;
    }
    return CheckHeader(reply);
}

int CardEmulationAgentProxy::IsSupportedAidPrefixMode(bool& result)
{
    auto ro = Remote();
    if (!ro) {
        return NFC_SDK_ERROR_NOT_INITIALIZED;
    }
    OHOS::MessageOption option;
    OHOS::MessageParcel data;
    OHOS::MessageParcel reply;
    WriteHeader(data);
    // write request body
    auto rv = ro->SendRequest(CODE_IS_SUPPORTED_AID_PREFIX_MODE, data, reply, option);
    if (rv != NFC_SUCCESS) {
        return NFC_SER_ERROR_DISCONNECT;
    }

    rv = CheckHeader(reply);
    if (rv != NFC_SUCCESS) {
        return rv;
    }
    result = reply.ReadBool();
    return rv;
}

int CardEmulationAgentProxy::GetDescriptionForPreferredService(int userId, std::string type, std::string& result)
{
    auto ro = Remote();
    if (!ro) {
        return NFC_SDK_ERROR_NOT_INITIALIZED;
    }
    OHOS::MessageOption option;
    OHOS::MessageParcel data;
    OHOS::MessageParcel reply;
    WriteHeader(data);
    // write request body
    data.WriteInt32(userId);
    data.WriteString(type);
    auto rv = ro->SendRequest(CODE_GET_DESCRIPTION_FOR_PREFERRED_SERVICE, data, reply, option);
    if (rv != NFC_SUCCESS) {
        return NFC_SER_ERROR_DISCONNECT;
    }
    rv = CheckHeader(reply);
    if (rv != NFC_SUCCESS) {
        return rv;
    }
    result = reply.ReadString();
    return rv;
}

int CardEmulationAgentProxy::SetPreferredForegroundService(int userId, const OHOS::AppExecFwk::ElementName& serviceName)
{
    auto ro = Remote();
    if (!ro) {
        return NFC_SDK_ERROR_NOT_INITIALIZED;
    }
    OHOS::MessageOption option;
    OHOS::MessageParcel data;
    OHOS::MessageParcel reply;
    WriteHeader(data);
    // write request body
    data.WriteInt32(userId);
    data.WriteParcelable(&serviceName);
    auto rv = ro->SendRequest(CODE_SET_PREFERRED_FOREGROUND_SERVICE, data, reply, option);
    if (rv != NFC_SUCCESS) {
        return NFC_SER_ERROR_DISCONNECT;
    }
    return CheckHeader(reply);
}

int CardEmulationAgentProxy::UnsetPreferredForegroundService(int userId)
{
    auto ro = Remote();
    if (!ro) {
        return NFC_SDK_ERROR_NOT_INITIALIZED;
    }
    OHOS::MessageOption option;
    OHOS::MessageParcel data;
    OHOS::MessageParcel reply;
    WriteHeader(data);
    // write request body
    data.WriteInt32(userId);
    auto rv = ro->SendRequest(CODE_UNSET_PREFERRED_FOREGROUND_SERVICE, data, reply, option);
    if (rv != NFC_SUCCESS) {
        return NFC_SER_ERROR_DISCONNECT;
    }
    return CheckHeader(reply);
}

int CardEmulationAgentProxy::GetPrimaryServiceForType(int userId,
                                                      const std::string& type,
                                                      OHOS::AppExecFwk::ElementName& result)
{
    auto ro = Remote();
    if (!ro) {
        return NFC_SDK_ERROR_NOT_INITIALIZED;
    }
    OHOS::MessageOption option;
    OHOS::MessageParcel data;
    OHOS::MessageParcel reply;
    WriteHeader(data);
    // write request body
    data.WriteInt32(userId);
    data.WriteString(type);
    auto rv = ro->SendRequest(CODE_GET_PRIMARY_SERVICE_FOR_TYPE, data, reply, option);
    if (rv != NFC_SUCCESS) {
        return NFC_SER_ERROR_DISCONNECT;
    }
    rv = CheckHeader(reply);
    if (rv != NFC_SUCCESS) {
        return rv;
    }
    auto pResult = reply.ReadParcelable<OHOS::AppExecFwk::ElementName>();
    if (pResult) {
        result = *pResult;
    }
    return rv;
}

int CardEmulationAgentProxy::SetPrimaryServiceForType(int userId,
                                                      const OHOS::AppExecFwk::ElementName& serviceName,
                                                      const std::string& type)
{
    auto ro = Remote();
    if (!ro) {
        return NFC_SDK_ERROR_NOT_INITIALIZED;
    }
    OHOS::MessageOption option;
    OHOS::MessageParcel data;
    OHOS::MessageParcel reply;
    WriteHeader(data);
    // write request body
    data.WriteInt32(userId);
    data.WriteParcelable(&serviceName);
    data.WriteString(type);
    auto rv = ro->SendRequest(CODE_SET_PRIMARY_SERVICE_FOR_TYPE, data, reply, option);
    if (rv != NFC_SUCCESS) {
        return NFC_SER_ERROR_DISCONNECT;
    }
    return CheckHeader(reply);
}
void CardEmulationAgentProxy::WriteHeader(OHOS::MessageParcel& data) const
{
}
int CardEmulationAgentProxy::CheckHeader(OHOS::MessageParcel& reply) const
{
    auto statusCode = reply.ReadInt32();
    DebugLog("CardEmulationAgentProxy::CheckHeader, statusCode: %d", statusCode);
    if (statusCode != NFC_SUCCESS) {
        return NFC_SDK_ERROR_NFC_STATE_INVALID;
    }
    return NFC_SUCCESS;
}
}  // namespace OHOS::nfc::sdk::cardemulation
