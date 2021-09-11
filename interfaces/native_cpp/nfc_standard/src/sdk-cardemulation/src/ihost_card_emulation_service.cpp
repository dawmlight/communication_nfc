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

#include "ihost_card_emulation_service.h"

#include <cassert>
#include <functional>

#include "apdu_channel.h"
#include "loghelper.h"

namespace OHOS::nfc::sdk::cardemulation {
IHostCardEmulationService::IHostCardEmulationService() : IHostCardEmulationService(OHOS::AppExecFwk::ElementName())
{
}
IHostCardEmulationService::IHostCardEmulationService(const OHOS::AppExecFwk::ElementName& name)
    : name_(name),
    channel_(),
    serviceChannel_()
{
}
void IHostCardEmulationService::CreateMessenger()
{
    if (!channel_) {
        DebugLog("creating channel_");
        channel_ = OHOS::sptr<ApduChannelStub>(new ApduChannelStub());
        DebugLog("creating channel_ done.");
        channel_->SetHandler([this](std::unique_ptr<Msg> msg) {
            if (!msg) {
                ErrorLog("msg is nullptr!");
                return;
            }
            DebugLog("Id: %d, arg1: %d, arg2: %d, data size: %d\n",
                     msg->Id(),
                     msg->Arg1(),
                     msg->Arg2(),
                     msg->GetData().size());
            switch (msg->Id()) {
                case COMMAND_APDU:  // APDU 命令
                {
                    serviceChannel_ = msg->GetSource();
                    if (!serviceChannel_) {
                        ErrorLog("service channel is nullptr.");
                        return;
                    }
                    auto resp = std::make_unique<Msg>(RESPONSE_APDU, 0, 0);
                    resp->SetData(HandleApdu(msg->GetData()));
                    resp->SetSource(RemoteObjectPool::GetRemoteObject(channel_->AsObject()));
                    DebugLog("CMD_APDU service channel: %p, client channel: %p",
                             serviceChannel_.GetRefPtr(),
                             channel_.GetRefPtr());
                    serviceChannel_->Send(std::move(resp));
                    break;
                }
                case RESPONSE_APDU:  // APDU 响应
                {
                    // 处理 IHostCardEmulationService 主动发送的响应
                    if (serviceChannel_) {
                        DebugLog("RESP_APDU service channel: %p, client channel: %p",
                                 serviceChannel_.GetRefPtr(),
                                 channel_.GetRefPtr());
                        msg->SetSource(RemoteObjectPool::GetRemoteObject(channel_->AsObject()));
                        serviceChannel_->Send(std::move(msg));
                    }
                    break;
                }
                case DEACTIVATED:  // 去激活
                {
                    OnDeactivated(msg->Arg1());
                    break;
                }
                case UNHANDLED:  // 未处理
                {
                    // 处理 IHostCardEmulationService 主动发送通知
                    if (serviceChannel_) {
                        DebugLog("UNHANDLED service channel: %p, client channel: %p",
                                 serviceChannel_.GetRefPtr(),
                                 channel_.GetRefPtr());
                        msg->SetSource(RemoteObjectPool::GetRemoteObject(channel_->AsObject()));
                        serviceChannel_->Send(std::move(msg));
                    }
                    break;
                }
                default:
                    break;
            }
        });
        DebugLog("CreateMessenger: set handler done.");
        DebugLog("channel: %p", channel_ ? channel_.GetRefPtr() : nullptr);
    }
}
void IHostCardEmulationService::SendApduResponse(std::vector<unsigned char> data)
{
    // IHostCardEmulationService 主动发送响应
    auto resp = std::make_unique<Msg>(RESPONSE_APDU, 0, 0);
    resp->SetData(std::move(data));
    if (channel_) {
        resp->SetSource(RemoteObjectPool::GetRemoteObject(channel_->AsObject()));
        channel_->Send(std::move(resp));
    }
}
void IHostCardEmulationService::NotifyUnhandled()
{
    // IHostCardEmulationService 主动发送通知
    auto resp = std::make_unique<Msg>(UNHANDLED, 0, 0);
    if (channel_) {
        resp->SetSource(RemoteObjectPool::GetRemoteObject(channel_->AsObject()));
        channel_->Send(std::move(resp));
    }
}
OHOS::sptr<IRemoteObject> IHostCardEmulationService::OnConnect(const OHOS::AAFwk::Want& want)
{
    CreateMessenger();
    if (channel_) {
        return RemoteObjectPool::GetRemoteObject(channel_->AsObject());
    }
    return OHOS::sptr<IRemoteObject>();
}

IHostCardEmulationService::~IHostCardEmulationService()
{
    try {
        if (channel_) {
            channel_->Stop();
        }
    } catch (...) {
    }
};
}  // namespace OHOS::nfc::sdk::cardemulation
