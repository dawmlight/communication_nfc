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
#ifndef ISECURE_ELEMENT_READER_H
#define ISECURE_ELEMENT_READER_H

#include <iostream>
#include <memory>

#include "iremote_broker.h"

namespace OHOS::se::omapi {
class ISecureElementSession;

class ISecureElementReader : public OHOS::IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.nfc.se.ISecureElementReader");
    virtual ~ISecureElementReader() {}
    /**
     * @brief Check if a SE is present in this reader.
     * @return True if the SE is present, false otherwise.
     */
    virtual bool IsSecureElementPresent() = 0;
    /**
     * @brief Connects to a SE in this reader. This method prepares (initializes) the SE for communication before the
     * session object is returned (i.e. powers the SE by ICC ON if it is not already on).There might be multiple
     * sessions opened at the same time on the same reader. The system ensures the interleaving of APDUs between the
     * respective sessions. A Reader instance might not communicate with the SE during calls to openSession(), depending
     * on the implementation.
     * @return A Session object to be used to create channels. The creation of a Session object SHALL NOT depend upon
     * the availability of the basic or logical channels.
     */
    virtual sptr<ISecureElementSession> OpenSession() = 0;
    /**
     * @brief Close all the sessions opened on this reader. All the channels opened by all these sessions will be
     * closed.
     */
    virtual void CloseSessions() = 0;
    /**
     * @brief reset the ese
     * @return true succecc; false otherwise
     */
    virtual bool Reset() = 0;
};
}  // namespace OHOS::se::omapi
#endif  // ISECURE_ELEMENT_READER_H
