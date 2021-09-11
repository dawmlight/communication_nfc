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
#include "secure_element_reader.h"

#include "loghelper.h"
#include "se_common_exception.h"
#include "se_end_service.h"
#include "secure_element_service.h"
#include "secure_element_session.h"
#include "terminal.h"

namespace OHOS {
namespace se {
/**
 * @brief Check if a SE is present in this reader.
 * @return True if the SE is present, false otherwise.
 */
bool SecureElementReader::IsSecureElementPresent()
{
    if (mTerminal_.expired()) {
        return false;
    }
    return mTerminal_.lock()->IsSecureElementPresent();
}
/**
 * @brief Connects to a SE in this reader. This method prepares (initializes) the SE for communication before the
 * session object is returned (i.e. powers the SE by ICC ON if it is not already on).There might be multiple
 * sessions opened at the same time on the same reader. The system ensures the interleaving of APDUs between the
 * respective sessions. A Reader instance might not communicate with the SE during calls to openSession(), depending
 * on the implementation.
 * @return A Session object to be used to create channels. The creation of a Session object SHALL NOT depend upon
 * the availability of the basic or logical channels.
 */
sptr<ISESession> SecureElementReader::OpenSession()
{
    if (!IsSecureElementPresent()) {
        throw ServiceSpecificError(SE_SER_ERR_IO, "Secure Element is not present.");
    }
    std::lock_guard<std::recursive_mutex> lock(mLock_);
    DebugLog("SecureElementReader::OpenSession Begin");
    sptr<SecureElementSession> session = new SecureElementSession(mService_, this);
    mSessions_.push_back(session);
    DebugLog("SecureElementReader::OpenSession End");
    return session;
}
/**
 * @brief Close all the sessions opened on this reader. All the channels opened by all these sessions will be
 * closed.
 */
void SecureElementReader::CloseSessions()
{
    std::lock_guard<std::recursive_mutex> lock(mLock_);
    DebugLog("SecureElementReader::CloseSessions Begin");
    sptr<ISESession> session = nullptr;
    while (mSessions_.size() > 0) {
        try {
            session = *(mSessions_.begin());
            session->Close();
        } catch (const std::exception& ignore) {
        }
    }
    DebugLog("SecureElementReader::CloseSessions End");
}

/**
 * @brief reset the ese
 * @return true succecc; false otherwise
 */
bool SecureElementReader::Reset()
{
    if (mTerminal_.expired()) {
        return false;
    }
    return mTerminal_.lock()->Reset();
}

std::string SecureElementReader::GetAtr()
{
    if (mTerminal_.expired()) {
        return std::string();
    }
    return mTerminal_.lock()->GetAtr();
}

void SecureElementReader::RemoveSession(wptr<ISESession> session)
{
    if (session == nullptr || !session.promote()) {
        return;
    }

    std::lock_guard<std::recursive_mutex> lock(mLock_);
    DebugLog("SecureElementReader::RemoveSession Begin.");
    std::vector<sptr<ISESession>>::iterator iter = mSessions_.begin();
    while (iter != mSessions_.end()) {
        if ((*iter) == session) {
            mSessions_.erase(iter);
            break;
        }
        ++iter;
    }

    if (mSessions_.empty()) {
        mTerminal_.lock()->ResetDefaultAppState();
    }
    DebugLog("SecureElementReader::RemoveSession End.");
}

std::weak_ptr<Terminal> SecureElementReader::GetTerminal()
{
    return mTerminal_;
}

SecureElementReader::SecureElementReader(std::weak_ptr<SeEndService> service, std::weak_ptr<Terminal> terminal)
    : mService_(service), mTerminal_(terminal), mSessions_({})
{
}

SecureElementReader::~SecureElementReader()
{
    DebugLog("SecureElementReader::~SecureElementReader");
}
}  // namespace se
}  // namespace OHOS