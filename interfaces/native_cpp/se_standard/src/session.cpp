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
#include "session.h"

#include "channel.h"
#include "isecure_element_channel.h"
#include "isecure_element_session.h"
#include "loghelper.h"
#include "reader.h"
#include "se_service.h"

namespace OHOS {
namespace se {
namespace omapi {
std::string Session::mTag_ = "OMAPI.Session";
Session::Session(std::weak_ptr<SEService> service, sptr<ISecureElementSession> session, std::weak_ptr<Reader> reader)
    : mSession_(session), mService_(service), mReader_(reader)
{
}

Session::~Session() {}

std::weak_ptr<Reader> Session::GetReader() const
{
    return mReader_;
}

std::string Session::GetATR() const
{
    if (mService_.expired() || !mService_.lock()->IsConnected() || !mSession_) {
        return std::string();
    }
    return mSession_->GetATR();
}

void Session::Close()
{
    if (mService_.expired() || !mService_.lock()->IsConnected() || !mSession_) {
        return;
    }

    std::lock_guard<std::mutex> guard(mMutex_);
    mSession_->Close();
    return;
}

bool Session::IsClosed()
{
    if (mService_.expired() || !mService_.lock()->IsConnected() || !mSession_) {
        return false;
    }
    return mSession_->IsClosed();
}

void Session::CloseChannels()
{
    if (mService_.expired() || !mService_.lock()->IsConnected() || !mSession_) {
        return;
    }

    std::lock_guard<std::mutex> guard(mMutex_);
    mSession_->CloseChannels();
    return;
}

std::shared_ptr<Channel> Session::OpenBasicChannel(const std::string& aid, int p2)
{
    if (mService_.expired() || !mService_.lock()->IsConnected() || !mSession_) {
        return std::shared_ptr<Channel>();
    }

    std::lock_guard<std::mutex> guard(mMutex_);
    sptr<ISecureElementChannel> channel = mSession_->OpenBasicChannel(aid, p2);
    if (!channel) {
        return std::shared_ptr<Channel>();
    }
    return std::shared_ptr<Channel>(new Channel(mService_, shared_from_this(), channel));
}

std::shared_ptr<Channel> Session::OpenBasicChannel(const std::string& aid)
{
    return OpenBasicChannel(aid, 0x00);
}

std::shared_ptr<Channel> Session::OpenLogicalChannel(const std::string& aid, int p2)
{
    if (mService_.expired() || !mService_.lock()->IsConnected() || !mSession_) {
        return std::shared_ptr<Channel>();
    }

    std::lock_guard<std::mutex> guard(mMutex_);
    sptr<ISecureElementChannel> channel = mSession_->OpenLogicalChannel(aid, p2);
    if (!channel) {
        return std::shared_ptr<Channel>();
    }
    return std::shared_ptr<Channel>(new Channel(mService_, shared_from_this(), channel));
}

std::shared_ptr<Channel> Session::OpenLogicalChannel(const std::string& aid)
{
    return OpenLogicalChannel(aid, 0x00);
}
}  // namespace omapi
}  // namespace se
}  // namespace OHOS