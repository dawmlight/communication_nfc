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

#ifndef SE_SERVICE_CALLBACK_SKELETON_H
#define SE_SERVICE_CALLBACK_SKELETON_H

#include "iremote_broker.h"
#include "iremote_object.h"
#include "iremote_proxy.h"
#include "iremote_stub.h"

namespace OHOS {
namespace Nfc {
namespace Oma {
class ISecureElementCallback : public IRemoteBroker {
public:
    enum {
        SE_SERVICE_CONNECTED_EVENT = 1,
        SE_SERVICE_DISCONNECTED_EVENT = 2,
    };

    virtual void OnSeServiceConnected() = 0;
    virtual void OnSeServiceDisconnected() = 0;

public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.ipc.SecureElementCallback");
};

class SecureElementCallbackProxy : public IRemoteProxy<ISecureElementCallback> {
public:
    explicit SecureElementCallbackProxy(const sptr<IRemoteObject> &impl);
    ~SecureElementCallbackProxy() = default;
    void OnSeServiceConnected() override;
    void OnSeServiceDisconnected() override;
};

class SecureElementCallbackStub : public IRemoteStub<ISecureElementCallback> {
public:
    virtual int OnRemoteRequest(uint32_t code,
        MessageParcel& data, MessageParcel& reply, MessageOption& option) override;
    sptr<IRemoteObject> AsObject() override;
    void OnSeServiceConnected() override;
    void OnSeServiceDisconnected() override;
};
} // namespace Oma
} // namespace Nfc
} // namespace OHOS
#endif // SE_SERVICE_CALLBACK_SKELETON_H
