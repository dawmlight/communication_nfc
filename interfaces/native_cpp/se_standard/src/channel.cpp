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
#include "channel.h"

#include "isecure_element_channel.h"
#include "loghelper.h"
#include "se_service.h"
#include "session.h"

using namespace std;

namespace OHOS {
namespace se {
namespace omapi {
Channel::Channel(std::weak_ptr<SEService> service, std::weak_ptr<Session> session, sptr<ISecureElementChannel> channel)
    : mSession_(session), mChannel_(channel), mService_(service)
{
}

Channel::~Channel() {}

void Channel::Close()
{
    if (IsClosed()) {
        return;
    }

    std::lock_guard<std::mutex> guard(mMutex_);
    mChannel_->Close();
}

bool Channel::IsClosed()
{
    if (mService_.expired() || !mChannel_ || !mService_.lock()->IsConnected()) {
        return false;
    }
    return mChannel_->IsClosed();
}

bool Channel::IsBasicChannel()
{
    if (mService_.expired() || !mChannel_ || !mService_.lock()->IsConnected()) {
        return false;
    }
    return mChannel_->IsBasicChannel();
}

std::string Channel::GetSelectResponse()
{
    if (mService_.expired() || !mChannel_ || !mService_.lock()->IsConnected()) {
        return std::string();
    }
    return mChannel_->GetSelectResponse();
}

std::string Channel::Transmit(const std::string& command)
{
    if (mService_.expired() || !mChannel_ || !mService_.lock()->IsConnected()) {
        return std::string();
    }
    return mChannel_->Transmit(command);
}

bool Channel::SelectNext()
{
    if (mService_.expired() || !mChannel_ || !mService_.lock()->IsConnected()) {
        return false;
    }
    return mChannel_->SelectNext();
}

std::weak_ptr<Session> Channel::GetSession()
{
    return mSession_;
}
}  // namespace omapi
}  // namespace se
}  // namespace OHOS