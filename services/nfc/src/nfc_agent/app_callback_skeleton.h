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

#ifndef COMMUNICATION_APP_CALLBACK_SKELETON_H
#define COMMUNICATION_APP_CALLBACK_SKELETON_H

#include "iremote_broker.h"
#include "iremote_object.h"
#include "iremote_proxy.h"
#include "iremote_stub.h"

namespace OHOS {
namespace Nfc {
class IAppCallback : public IRemoteBroker {
public:
    enum {
        CREATE_BEAM_SHARE_DATA_EVENT = 1,
        ON_NDEF_PUSH_COMPLETE_EVENT = 2,
        ON_TAG_DISCOVERED_EVENT = 3,
    };

    virtual void CreateBeamShareData() = 0;
    virtual void OnNdefPushComplete() = 0;
    virtual void OnTagDiscovered() = 0;

public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.ipc.AppCallback");
};

class AppCallbackProxy : public IRemoteProxy<IAppCallback> {
public:
    explicit AppCallbackProxy(const sptr<IRemoteObject> &impl);
    ~AppCallbackProxy() = default;
    void CreateBeamShareData() override;
    void OnNdefPushComplete() override;
    void OnTagDiscovered() override;
};

class AppCallbackStub : public IRemoteStub<IAppCallback> {
public:
    virtual int OnRemoteRequest(uint32_t code,
        MessageParcel& data, MessageParcel& reply, MessageOption& option) override;
    sptr<IRemoteObject> AsObject() override;
    void CreateBeamShareData() override;
    void OnNdefPushComplete() override;
    void OnTagDiscovered() override;
};
} // namespace Nfc
} // namespace OHOS
#endif // COMMUNICATION_APP_CALLBACK_SKELETON_H
