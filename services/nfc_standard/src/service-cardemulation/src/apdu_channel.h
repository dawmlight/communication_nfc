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

#ifndef APDU_CHANNEL_H
#define APDU_CHANNEL_H

#include <atomic>
#include <condition_variable>
#include <cstdio>
#include <functional>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>

#include "event_handler.h"
#include "iremote_broker.h"
#include "iremote_proxy.h"
#include "iremote_stub.h"
#include "pac_map.h"

namespace OHOS {
namespace AAFwk {
}
namespace AppExecFwk {
class EventRunner;
class PacMap;
}  // namespace AppExecFwk

namespace nfc::sdk::cardemulation {
class ApduEventHandler;
class ApduChannelProxy;
class Msg;
class RemoteObjectDeathRecipient;
class IApduChannel : public OHOS::IRemoteBroker {
public:
    static const std::u16string Token;
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.nfc.osal.apdu_channel");
    virtual ~IApduChannel()
    {
    }
    virtual void Send(std::unique_ptr<Msg> msg) = 0;
};

class Msg final : public OHOS::Parcelable {
public:
    Msg(int id, int arg1, int arg2);
    Msg(const Msg& msg);
    Msg& operator=(const Msg& msg);
    Msg(Msg&& msg);
    Msg& operator=(Msg&& msg);
    bool operator==(const Msg& other) const;
    ~Msg();
    int Id() const;
    int Arg1() const;
    int Arg2() const;
    std::vector<unsigned char> GetData() const;
    OHOS::sptr<ApduChannelProxy> GetSource() const;
    bool EqualsReplyRemoteObject(const OHOS::sptr<OHOS::IRemoteObject>& ro) const;

    void SetId(int id);
    void SetArg1(int arg);
    void SetArg2(int arg);
    void SetData(const std::vector<unsigned char>& data);
    void SetSource(const OHOS::sptr<OHOS::IRemoteObject>& source);

    bool Marshalling(OHOS::Parcel& parcel) const override;
    bool ReadFromParcel(OHOS::Parcel& parcel);
    static Msg* Unmarshalling(OHOS::Parcel& parcel);

private:
    void Copy(const Msg& msg);
    void Move(Msg&& msg);

private:
    int id_;
    int arg1_;
    int arg2_;

    std::vector<unsigned char> data_;
    OHOS::sptr<OHOS::IRemoteObject> source_;
};

class ApduEventHandler : public OHOS::AppExecFwk::EventHandler {
public:
    ApduEventHandler(const std::shared_ptr<OHOS::AppExecFwk::EventRunner>& runner,
                     std::function<void(std::unique_ptr<Msg>)> handler);

protected:
    void ProcessEvent(const OHOS::AppExecFwk::InnerEvent::Pointer& event) override;

private:
    std::function<void(std::unique_ptr<Msg>)> handler_;
};
// rpc request code
constexpr int CODE_APDUCHANNEL_SEND = 1;
// rpc response error
constexpr int ERR_INVALID_TOKEN = 1;
constexpr int ERR_UNSUPPORTED_CODE = 2;
class ApduChannelProxy final : public OHOS::IRemoteProxy<IApduChannel> {
public:
    explicit ApduChannelProxy(const OHOS::sptr<OHOS::IRemoteObject>& ro);
    ~ApduChannelProxy() override;
    void Send(std::unique_ptr<Msg> msg) override;

    ApduChannelProxy(const ApduChannelProxy& o) = delete;
    ApduChannelProxy& operator=(const ApduChannelProxy& o) = delete;
private:
    std::unique_ptr<ApduEventHandler> handler_;
};

class ApduChannelStub final : public OHOS::IRemoteStub<IApduChannel> {
public:
    ~ApduChannelStub();
    void Stop();
    void SetHandler(std::function<void(std::unique_ptr<Msg>)> handler);
    void Send(std::unique_ptr<Msg> msg) override;
    int OnRemoteRequest(uint32_t code,
                        OHOS::MessageParcel& data,
                        OHOS::MessageParcel& reply,
                        OHOS::MessageOption& option) override;

private:
    std::shared_ptr<ApduEventHandler> handler_;
    std::shared_ptr<OHOS::AppExecFwk::EventRunner> runner_;
};

class RemoteObjectDeathRecipient : public OHOS::IRemoteObject::DeathRecipient {
public:
    RemoteObjectDeathRecipient() = default;
    ~RemoteObjectDeathRecipient() = default;
    void OnRemoteDied(const wptr<IRemoteObject>& object) override;
    void AddRecipient(const wptr<IRemoteObject>& object,
                      const std::weak_ptr<OHOS::IRemoteObject::DeathRecipient>& recipient);
    void RemoveRecipient(const wptr<IRemoteObject>& object);

private:
    void RemoveRecipient(const wptr<IRemoteObject>& object, bool notify);

private:
    std::mutex mu_;
    std::map<std::weak_ptr<OHOS::IRemoteObject::DeathRecipient>,
             wptr<IRemoteObject>,
             std::owner_less<std::weak_ptr<OHOS::IRemoteObject::DeathRecipient>>>
        recipient_;
};

class RemoteObjectPool final {
public:
    static OHOS::sptr<OHOS::IRemoteObject> GetRemoteObject(const OHOS::sptr<OHOS::IRemoteObject>& ro);
    static void AddRemoteDeathRecipient(const wptr<IRemoteObject>& object,
                                        const std::weak_ptr<OHOS::IRemoteObject::DeathRecipient>& recipient);
    static void RemoveRemoteDeathRecipient(const wptr<IRemoteObject>& object,
                                           const std::weak_ptr<OHOS::IRemoteObject::DeathRecipient>& recipient);

private:
    static void InitDeathRecipient();

private:
    static std::map<OHOS::IRemoteObject*, OHOS::sptr<OHOS::IRemoteObject>> pool_;
    static OHOS::sptr<RemoteObjectDeathRecipient> remoteObjectDeathRecipient_;
};

}  // namespace nfc::sdk::cardemulation
}  // namespace OHOS
#endif  // !APDU_CHANNEL_H
