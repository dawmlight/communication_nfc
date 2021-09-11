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
#ifndef READER_H
#define READER_H

#include <iostream>
#include <memory>
#include <mutex>

#include "isecure_element_reader.h"

namespace OHOS::se::omapi {
class SEService;
class Session;
class Reader : public std::enable_shared_from_this<Reader> {
public:
    ~Reader();
    /**
     * @brief Get the name of this reader.
     *
     * @return std::string the Reader Name
     */
    std::string GetName() const;
    /**
     * @brief Connects to a SE in this reader. This method prepares (initializes) the SE for communication before the
     * session object is returned (i.e. powers the SE by ICC ON if it is not already on).There might be multiple
     * sessions opened at the same time on the same reader. The system ensures the interleaving of APDUs between the
     * respective sessions. A Reader instance might not communicate with the SE during calls to openSession(), depending
     * on the implementation.
     * @return A Session object to be used to create channels. The creation of a Session object SHALL NOT depend upon
     * the availability of the basic or logical channels.
     */
    std::shared_ptr<Session> OpenSession();
    /**
     * @brief Check if a SE is present in this reader.
     * @return True if the SE is present, false otherwise.
     */
    bool IsSecureElementPresent();
    /**
     * @brief Get the SE Service this reader is bound to.
     *
     * @return std::weak_ptr<SEService> The SEService object.
     */
    std::weak_ptr<SEService> GetSEService() const;
    /**
     * @brief Close all the sessions opened on this reader. All the channels opened by all these sessions will be
     * closed.
     */
    void CloseSessions();
    /**
     * @brief reset the ese
     * @return true succecc; false otherwise
     */
    bool Reset();

private:
    Reader(std::weak_ptr<SEService> service, const std::string& name, sptr<ISecureElementReader> reader);

private:
    std::string mName_;
    std::weak_ptr<SEService> mService_;
    sptr<ISecureElementReader> mReader_;
    std::mutex mMutex_;

    friend class SEService;
};
}  // namespace OHOS::se::omapi
#endif  // READER_H
