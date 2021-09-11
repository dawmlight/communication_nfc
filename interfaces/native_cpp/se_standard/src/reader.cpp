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
#include "reader.h"

#include "isecure_element_reader.h"
#include "loghelper.h"
#include "se_service.h"
#include "session.h"

namespace OHOS {
namespace se {
namespace omapi {
Reader::Reader(std::weak_ptr<SEService> service, const std::string& name, sptr<ISecureElementReader> reader)
    : mName_(name), mService_(service), mReader_(reader)
{
}

Reader::~Reader() {}

std::string Reader::GetName() const
{
    return mName_;
}

std::shared_ptr<Session> Reader::OpenSession()
{
    if (mService_.expired() || !mService_.lock()->IsConnected() || !mReader_) {
        return std::shared_ptr<Session>();
    }
    sptr<ISecureElementSession> session = mReader_->OpenSession();
    if (!session) {
        return std::shared_ptr<Session>();
    }
    return std::shared_ptr<Session>(new Session(mService_, session, shared_from_this()));
}

bool Reader::IsSecureElementPresent()
{
    if (mService_.expired() || !mService_.lock()->IsConnected() || !mReader_) {
        return false;
    }

    return mReader_->IsSecureElementPresent();
}

std::weak_ptr<SEService> Reader::GetSEService() const
{
    return mService_;
}

void Reader::CloseSessions()
{
    if (mService_.expired() || !mService_.lock()->IsConnected() || !mReader_) {
        return;
    }

    mReader_->CloseSessions();
    return;
}

bool Reader::Reset()
{
    if (mService_.expired() || !mService_.lock()->IsConnected() || !mReader_) {
        return false;
    }

    return mReader_->Reset();
}
}  // namespace omapi
}  // namespace se
}  // namespace OHOS