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

#include "app_callback_skeleton.h"

#include <string>

#include "ipc_skeleton.h"
#include "nfc_log.h"

namespace OHOS {
namespace Nfc {
AppCallbackProxy::AppCallbackProxy(const sptr<IRemoteObject> &impl)
    : IRemoteProxy<IAppCallback>(impl)
{
}

void AppCallbackProxy::CreateBeamShareData()
{
    HILOGI("AppCallbackProxy::CreateBeamShareData");
}

void AppCallbackProxy::OnNdefPushComplete()
{
    HILOGI("AppCallbackProxy::OnNdefPushComplete");
}

void AppCallbackProxy::OnTagDiscovered()
{
    HILOGI("AppCallbackProxy::OnTagDiscovered");
}

int AppCallbackStub::OnRemoteRequest(uint32_t code, MessageParcel& data, MessageParcel& reply,
    MessageOption& option)
{
    int ret = -1;
    pid_t lastCallingPid = IPCSkeleton::GetCallingPid();
    uid_t lastCallinguid = IPCSkeleton::GetCallingUid();
    HILOGI("OnReceived cmd = %{public}d, pid= %{public}d, uid= %{public}d", code, lastCallingPid, lastCallinguid);
    switch (code) {
        case CREATE_BEAM_SHARE_DATA_EVENT: {
            CreateBeamShareData();
            break;
        }
        case ON_NDEF_PUSH_COMPLETE_EVENT: {
            OnNdefPushComplete();
            break;
        }
        case ON_TAG_DISCOVERED_EVENT: {
            OnTagDiscovered();
            break;
        }
        default:
            ret = IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
    return ret;
}

sptr<IRemoteObject> AppCallbackStub::AsObject()
{
    return this;
}

void AppCallbackStub::CreateBeamShareData()
{
    HILOGI("AppCallbackStub::CreateBeamShareData");
}

void AppCallbackStub::OnNdefPushComplete()
{
    HILOGI("AppCallbackStub::OnNdefPushComplete");
}

void AppCallbackStub::OnTagDiscovered()
{
    HILOGI("AppCallbackStub::OnTagDiscovered");
}
} // namespace Nfc
} // namespace OHOS
