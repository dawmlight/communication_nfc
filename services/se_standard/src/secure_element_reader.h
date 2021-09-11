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
#ifndef SECURE_ELEMENT_READER_H
#define SECURE_ELEMENT_READER_H

#include <memory>
#include <mutex>
#include <vector>

#include "isecure_element_session.h"
#include "secure_element_reader_stub.h"

using ISESession = OHOS::se::omapi::ISecureElementSession;

namespace OHOS {
namespace se {
class Terminal;
class SeEndService;

class SecureElementReader final : public omapi::SecureElemntReaderStub {
public:
    ~SecureElementReader();
    void RemoveSession(wptr<ISESession> session);
    std::weak_ptr<Terminal> GetTerminal();
    std::string GetAtr();
    /**
     * @brief Check if a SE is present in this reader.
     * @return True if the SE is present, false otherwise.
     */
    bool IsSecureElementPresent() override;
    /**
     * @brief Connects to a SE in this reader. This method prepares (initializes) the SE for communication before the
     * session object is returned (i.e. powers the SE by ICC ON if it is not already on).There might be multiple
     * sessions opened at the same time on the same reader. The system ensures the interleaving of APDUs between the
     * respective sessions. A Reader instance might not communicate with the SE during calls to openSession(), depending
     * on the implementation.
     * @return A Session object to be used to create channels. The creation of a Session object SHALL NOT depend upon
     * the availability of the basic or logical channels.
     */
    sptr<ISESession> OpenSession() override;
    /**
     * @brief Close all the sessions opened on this reader. All the channels opened by all these sessions will be
     * closed.
     */
    void CloseSessions() override;
    /**
     * @brief reset the ese
     * @return true succecc; false otherwise
     */
    bool Reset() override;

private:
    explicit SecureElementReader(std::weak_ptr<SeEndService> mService, std::weak_ptr<Terminal> terminal);

private:
    // secure element service
    std::weak_ptr<SeEndService> mService_{};
    std::weak_ptr<Terminal> mTerminal_{};
    // the session list
    std::vector<sptr<ISESession>> mSessions_{};
    std::recursive_mutex mLock_{};

    friend class SecureElementService;
};
}  // namespace se
}  // namespace OHOS
#endif  // !SECURE_ELEMENT_READER_H
