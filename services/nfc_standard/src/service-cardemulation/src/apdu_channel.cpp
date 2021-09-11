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

#include "apdu_channel.h"

#include <algorithm>
#include <cassert>
#include <cstdio>
#include <mutex>

#include "loghelper.h"
#ifdef USE_HILOG
DEFINE_NFC_LOG_LABEL("ApduEventHandler");
#endif
#include <cstring>

using namespace OHOS::AppExecFwk;
namespace OHOS {
namespace nfc::sdk::cardemulation {
void RemoteObjectDeathRecipient::OnRemoteDied(const wptr<IRemoteObject>& object)
{
    RemoveRecipient(object, true);
}
void RemoteObjectDeathRecipient::AddRecipient(const wptr<IRemoteObject>& object,
                                              const std::weak_ptr<OHOS::IRemoteObject::DeathRecipient>& recipient)
{
    if (object.GetRefPtr() == nullptr || recipient.expired()) {
        return;
    }
    std::lock_guard<std::mutex> lk(mu_);
    recipient_[recipient] = object;
}
void RemoteObjectDeathRecipient::RemoveRecipient(const wptr<IRemoteObject>& object)
{
    RemoveRecipient(object, false);
}

void RemoteObjectDeathRecipient::RemoveRecipient(const wptr<IRemoteObject>& object, bool notify)
{
    if (object.GetRefPtr() == nullptr) {
        return;
    }
    std::lock_guard<std::mutex> lk(mu_);
    auto it = std::find_if(recipient_.begin(), recipient_.end(), [&object](decltype(recipient_.begin())::reference r) {
        return (object == r.second);
    });
    if (it != recipient_.end()) {
        if (notify) {
            auto r = it->first.lock();
            if (r) {
                r->OnRemoteDied(object);
            }
        }
        recipient_.erase(it);
    }
}
ApduEventHandler::ApduEventHandler(const std::shared_ptr<OHOS::AppExecFwk::EventRunner>& runner,
                                   std::function<void(std::unique_ptr<Msg>)> handler)
    : OHOS::AppExecFwk::EventHandler(runner),
    handler_(std::move(handler))
{
}

void ApduEventHandler::ProcessEvent(const OHOS::AppExecFwk::InnerEvent::Pointer& event)
{
    if (!event || !handler_) {
        return;
    }
    auto msg = event->GetUniqueObject<Msg>();
    handler_(std::move(msg));
}

const std::u16string IApduChannel::Token = u"ohos.nfc.cardemulation.apduchannel";

ApduChannelProxy::ApduChannelProxy(const OHOS::sptr<OHOS::IRemoteObject>& ro) : OHOS::IRemoteProxy<IApduChannel>(ro)
{
}

ApduChannelProxy::~ApduChannelProxy()
{
}

void ApduChannelProxy::Send(std::unique_ptr<Msg> msg)
{
    using namespace OHOS;
    auto remote = Remote();
    if (!remote) {
        ErrorLog("remote object is nullptr!");
        return;
    }

    auto code = CODE_APDUCHANNEL_SEND;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);

    data.WriteInterfaceToken(IApduChannel::Token);
    data.WriteParcelable(msg.release());
    int rv = remote->SendRequest(code, data, reply, option);
    if (rv == 0 && reply.ReadInt32() == 0) {
        // succeeded
        return;
    }
    // failed
    ErrorLog("failed to send msg.");
}


static const std::string KEY_DATA = "data";
Msg::Msg(int id, int arg1, int arg2)
    : id_(id),
    arg1_(arg1),
    arg2_(arg2),
    data_(),
    source_(OHOS::sptr<OHOS::IRemoteObject>(nullptr))
{
}

Msg::Msg(const Msg& msg)
{
    Copy(msg);
}

Msg& Msg::operator=(const Msg& msg)
{
    if (this != &msg) {
        Copy(msg);
    }
    return *this;
}

Msg::Msg(Msg&& msg)
{
    Move(std::move(msg));
}

Msg& Msg::operator=(Msg&& msg)
{
    if (this != &msg) {
        Move(std::move(msg));
    }
    return *this;
}

bool Msg::operator==(const Msg& other) const
{
    DebugLog("Msg::operator==");
    if (this == &other) {
        return true;
    }
    bool dataEqs = (data_.size() == other.data_.size());
    if (dataEqs && !data_.empty()) {
        dataEqs = memcmp(&data_[0], &other.data_[0], data_.size() * sizeof(data_[0])) == 0;
    }
    bool sourceEqs = (source_ == other.source_);
    return dataEqs && id_ == other.id_ && arg1_ == other.arg1_ && arg2_ == other.arg2_ && sourceEqs;
}

Msg::~Msg()
{
    DebugLog("~Msg(). source_ : %p", source_.GetRefPtr());
}

int Msg::Id() const
{
    return id_;
}

int Msg::Arg1() const
{
    return arg1_;
}

int Msg::Arg2() const
{
    return arg2_;
}

std::vector<unsigned char> Msg::GetData() const
{
    return data_;
}

OHOS::sptr<ApduChannelProxy> Msg::GetSource() const
{
    DebugLog("Get Source_ pointer: %p\n", source_.GetRefPtr());
    if (!source_) {
        return OHOS::sptr<ApduChannelProxy>();
    }
    auto s = OHOS::sptr<ApduChannelProxy>(new ApduChannelProxy(source_));
    DebugLog("Get Source_ is %p, return: %p\n", source_.GetRefPtr(), s.GetRefPtr());
    return s;
}

bool Msg::EqualsReplyRemoteObject(const OHOS::sptr<OHOS::IRemoteObject>& ro) const
{
    return source_ == ro;
}

void Msg::SetId(int what)
{
    id_ = what;
}

void Msg::SetArg1(int arg)
{
    arg1_ = arg;
}

void Msg::SetArg2(int arg)
{
    arg2_ = arg;
}

void Msg::SetData(const std::vector<unsigned char>& data)
{
    data_ = data;
}

void Msg::SetSource(const OHOS::sptr<OHOS::IRemoteObject>& src)
{
    source_ = src;
    DebugLog("set source_ is %p\n", source_.GetRefPtr());
}

bool Msg::Marshalling(OHOS::Parcel& parcel) const
{
    DebugLog("Marshalling:\n\tid: %d, arg1: %d, arg2: %d\n\tsource_: %p", id_, arg1_, arg2_, source_.GetRefPtr());
    parcel.WriteInt32(id_);
    parcel.WriteInt32(arg1_);
    parcel.WriteInt32(arg2_);
    parcel.WriteUInt8Vector(data_);
    assert(source_);
    return parcel.WriteRemoteObject(source_);
}
bool Msg::ReadFromParcel(OHOS::Parcel& parcel)
{
    id_ = parcel.ReadInt32();
    arg1_ = parcel.ReadInt32();
    arg2_ = parcel.ReadInt32();
    parcel.ReadUInt8Vector(&data_);
    source_ = parcel.ReadObject<OHOS::IRemoteObject>();
    DebugLog("ReadFromParcel:\n\tid: %d, arg1: %d, arg2: %d\n\tsource_: %p", id_, arg1_, arg2_, source_.GetRefPtr());
    return true;
}
Msg* Msg::Unmarshalling(OHOS::Parcel& parcel) 
{
    auto msg = std::make_unique<Msg>(0, 0, 0);
    if (msg && msg->ReadFromParcel(parcel)) {
        return msg.release();
    }
    ErrorLog("Unmarshalling failed.");
    return nullptr;
}

void Msg::Copy(const Msg& msg)
{
    id_ = msg.Id();
    arg1_ = msg.Arg1();
    arg2_ = msg.Arg2();
    data_ = msg.GetData();
    source_ = msg.source_;
}

void Msg::Move(Msg&& msg)
{
    id_ = msg.id_;
    arg1_ = msg.arg1_;
    arg2_ = msg.arg2_;
    data_ = std::move(msg.data_);
    source_ = std::move(msg.source_);
}

ApduChannelStub::~ApduChannelStub()
{
    Stop();
}
void ApduChannelStub::Stop()
{
    if (runner_) {
        runner_->Stop();
        runner_ = nullptr;
    }
    handler_ = nullptr;
}
void ApduChannelStub::SetHandler(std::function<void(std::unique_ptr<Msg>)> handler)
{
    if (!runner_) {
        runner_ = OHOS::AppExecFwk::EventRunner::Create("channel_stub_runner");
    }
    handler_ = std::make_shared<ApduEventHandler>(runner_, handler);
    if (runner_) {
        DebugLog("ApduChannelStub::SetHandler: run");
        runner_->Run();
    }
}
void ApduChannelStub::Send(std::unique_ptr<Msg> msg) 
{
    if (msg && handler_) {
        handler_->SendEvent(InnerEvent::Get(0, std::move(msg)));
    }
}
int ApduChannelStub::OnRemoteRequest(uint32_t code,
                                     OHOS::MessageParcel& data,
                                     OHOS::MessageParcel& reply,
                                     OHOS::MessageOption& option)
{
    std::u16string token = data.ReadInterfaceToken();
    if (token != IApduChannel::Token) {
        return ERR_INVALID_TOKEN;
    }
    if (code == CODE_APDUCHANNEL_SEND) {
        auto msg = std::unique_ptr<Msg>(data.ReadParcelable<Msg>());
        Send(std::move(msg));
        return ERR_OK;
    }
    return ERR_UNSUPPORTED_CODE;
}
std::map<OHOS::IRemoteObject*, OHOS::sptr<OHOS::IRemoteObject>> RemoteObjectPool::pool_;
OHOS::sptr<RemoteObjectDeathRecipient> RemoteObjectPool::remoteObjectDeathRecipient_;
OHOS::sptr<OHOS::IRemoteObject> RemoteObjectPool::GetRemoteObject(const OHOS::sptr<OHOS::IRemoteObject>& ro)
{
    if (ro) {
        auto it = pool_.find(ro.GetRefPtr());
        if (it != pool_.end()) {
            DebugLog("found. pool size: %zu", pool_.size());
            return it->second;
        }
        RemoteObjectPool::InitDeathRecipient();
        ro->AddDeathRecipient(remoteObjectDeathRecipient_);
        pool_[ro.GetRefPtr()] = ro;
        DebugLog("not found. pool size: %zu", pool_.size());
        return ro;
    }
    DebugLog("ro is nullptr. pool size: %zu", pool_.size());
    return OHOS::sptr<OHOS::IRemoteObject>();
}
void RemoteObjectPool::AddRemoteDeathRecipient(const wptr<IRemoteObject>& object,
                                               const std::weak_ptr<OHOS::IRemoteObject::DeathRecipient>& recipient)
{
    if (object.GetRefPtr() == nullptr || recipient.expired()) {
        return;
    }
    RemoteObjectPool::InitDeathRecipient();
    DebugLog("AddRemoteDeathRecipient: %p", remoteObjectDeathRecipient_.GetRefPtr());
    object->AddDeathRecipient(remoteObjectDeathRecipient_);
    remoteObjectDeathRecipient_->AddRecipient(object, recipient);
}
void RemoteObjectPool::RemoveRemoteDeathRecipient(const wptr<IRemoteObject>& object,
                                                  const std::weak_ptr<OHOS::IRemoteObject::DeathRecipient>& recipient)
{
}
void RemoteObjectPool::InitDeathRecipient()
{
    if (!remoteObjectDeathRecipient_) {
        remoteObjectDeathRecipient_ = OHOS::sptr<RemoteObjectDeathRecipient>(new RemoteObjectDeathRecipient());
    }
}
}  // namespace nfc::sdk::cardemulation
}  // namespace OHOS
