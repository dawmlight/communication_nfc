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
#include "secure_element_session_stub.h"

#include "isecure_element_channel.h"
#include "loghelper.h"
#include "message_parcel.h"
#include "se_common_exception.h"

using namespace std;

namespace OHOS::se::omapi {
int SecureElemntSessionStub::OnRemoteRequest(uint32_t code,         /* [in] */
                                             MessageParcel& data,   /* [in] */
                                             MessageParcel& reply,  /* [out] */
                                             MessageOption& option) /* [in] */
{
    DebugLog("SecureElemntSessionStub::OnRemoteRequest occur, code is %d", code);
    switch (code) {
        case COMMAND_SESSION_GETATR:
            return HandleGetATR(data, reply);
        case COMMAND_SESSION_CLOSE:
            return HandleClose(data, reply);
        case COMMAND_SESSION_ISCLOSED:
            return HandleIsClosed(data, reply);
        case COMMAND_SESSION_CLOSE_CHANNELS:
            return HandleCloseChannels(data, reply);
        case COMMAND_SESSION_OPEN_BASIC_CHANNEL:
            return HandleOpenBasicChannel(data, reply);
        case COMMAND_SESSION_OPEN_LOGIC_CHANNEL:
            return HandleOpenLogicalChannel(data, reply);
        default:
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
}
int SecureElemntSessionStub::HandleGetATR(OHOS::MessageParcel& data, OHOS::MessageParcel& reply)
{
    std::string response = GetATR();
    reply.WriteString(response);
    return ERR_NONE;
}

int SecureElemntSessionStub::HandleClose(OHOS::MessageParcel& data, OHOS::MessageParcel& reply)
{
    Close();
    return ERR_NONE;
}

int SecureElemntSessionStub::HandleIsClosed(OHOS::MessageParcel& data, OHOS::MessageParcel& reply)
{
    bool res = IsClosed();
    reply.WriteBool(res);
    return ERR_NONE;
}

int SecureElemntSessionStub::HandleCloseChannels(OHOS::MessageParcel& data, OHOS::MessageParcel& reply)
{
    CloseChannels();
    return ERR_NONE;
}

int SecureElemntSessionStub::HandleOpenBasicChannel(OHOS::MessageParcel& data, OHOS::MessageParcel& reply)
{
    std::string aid = data.ReadString();
    int p2 = data.ReadInt32();

    int32_t state = ERR_NONE;
    sptr<IRemoteObject> iChannel = nullptr;
    try {
        sptr<ISecureElementChannel> channel = OpenBasicChannel(aid, p2);
        if (channel) {
            iChannel = channel->AsObject();
        }
    } catch (const IllegalStateError& e) {
        InfoLog("OpenBasicChannel is failed: %s", e.what());
        state = SE_SER_ERR_ILLEGAL_STATE;
    } catch (const UnsupportedOperationError& e) {
        InfoLog("OpenBasicChannel is failed: %s", e.what());
        state = SE_SER_ERR_UNSUPPORTED_OPERATION;
    } catch (const IllegalArgumentError& e) {
        InfoLog("OpenBasicChannel is failed: %s", e.what());
        state = SE_SER_ERR_ILLEGAL_PARAM;
    } catch (const ServiceSpecificError& e) {
        InfoLog("OpenBasicChannel is failed: %s", e.what());
        state = SE_SER_ERR_SECURITY;
    } catch (const NoSuchElementError& e) {
        InfoLog("OpenBasicChannel is failed: %s", e.what());
        state = SE_SER_ERR_NO_SUCH_ELEMENT;
    } catch (const AccessControlError& e) {
        InfoLog("OpenBasicChannel is failed: %s", e.what());
        state = SE_SER_ERR_ACCESS_CONTROL;
    }

    reply.WriteRemoteObject(iChannel);
    reply.WriteInt32(state);
    return ERR_NONE;
}

int SecureElemntSessionStub::HandleOpenLogicalChannel(OHOS::MessageParcel& data, OHOS::MessageParcel& reply)
{
    std::string aid = data.ReadString();
    int p2 = data.ReadInt32();

    int32_t state = ERR_NONE;
    sptr<IRemoteObject> iChannel = nullptr;
    try {
        sptr<ISecureElementChannel> channel = OpenLogicalChannel(aid, p2);
        if (channel) {
            iChannel = channel->AsObject();
        }
    } catch (const IllegalStateError& e) {
        InfoLog("OpenBasicChannel is failed: %s", e.what());
        state = SE_SER_ERR_ILLEGAL_STATE;
    } catch (const UnsupportedOperationError& e) {
        InfoLog("OpenBasicChannel is failed: %s", e.what());
        state = SE_SER_ERR_UNSUPPORTED_OPERATION;
    } catch (const IllegalArgumentError& e) {
        InfoLog("OpenBasicChannel is failed: %s", e.what());
        state = SE_SER_ERR_ILLEGAL_PARAM;
    } catch (const ServiceSpecificError& e) {
        InfoLog("OpenBasicChannel is failed: %s", e.what());
        state = SE_SER_ERR_SECURITY;
    } catch (const NoSuchElementError& e) {
        InfoLog("OpenBasicChannel is failed: %s", e.what());
        state = SE_SER_ERR_NO_SUCH_ELEMENT;
    } catch (const AccessControlError& e) {
        InfoLog("OpenBasicChannel is failed: %s", e.what());
        state = SE_SER_ERR_ACCESS_CONTROL;
    } catch (const SecurityError& e) {
        InfoLog("OpenBasicChannel is failed: %s", e.what());
        state = SE_SER_ERR_SECURITY;
    }

    reply.WriteRemoteObject(iChannel);
    reply.WriteInt32(state);
    return ERR_NONE;
}
}  // namespace OHOS::se::omapi