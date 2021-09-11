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

#include <string>

#include "se_service_callback_skeleton.h"

#include "ipc_skeleton.h"
#include "nfc_log.h"

namespace OHOS {
namespace Nfc {
namespace Oma {
SecureElementCallbackProxy::SecureElementCallbackProxy(const sptr<IRemoteObject> &impl)
    : IRemoteProxy<ISecureElementCallback>(impl)
{
}

void SecureElementCallbackProxy::OnSeServiceConnected()
{
    HILOGI("SecureElementCallbackProxy::OnSeServiceConnected");
}

void SecureElementCallbackProxy::OnSeServiceDisconnected()
{
    HILOGI("SecureElementCallbackProxy::OnSeServiceDisconnected");
}

int SecureElementCallbackStub::OnRemoteRequest(uint32_t code, MessageParcel& data, MessageParcel& reply,
    MessageOption& option)
{
    int ret = -1;
    pid_t lastCallingPid = IPCSkeleton::GetCallingPid();
    uid_t lastCallinguid = IPCSkeleton::GetCallingUid();
    HILOGI("OnReceived cmd = %{public}d, pid= %{public}d, uid= %{public}d", code, lastCallingPid, lastCallinguid);
    switch (code) {
        case SE_SERVICE_CONNECTED_EVENT: {
            OnSeServiceConnected();
            break;
        }
        case SE_SERVICE_DISCONNECTED_EVENT: {
            OnSeServiceDisconnected();
            break;
        }
        default:
            ret = IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
    return ret;
}

sptr<IRemoteObject> SecureElementCallbackStub::AsObject()
{
    return this;
}

void SecureElementCallbackStub::OnSeServiceConnected()
{
    HILOGI("SecureElementCallbackStub::OnSeServiceConnected");
}

void SecureElementCallbackStub::OnSeServiceDisconnected()
{
    HILOGI("SecureElementCallbackStub::OnSeServiceDisconnected");
}
} // namespace Oma
} // namespace Nfc
} // namespace OHOS
