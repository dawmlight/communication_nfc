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
#ifndef ISECURE_ELEMENT_SERVICE_H
#define ISECURE_ELEMENT_SERVICE_H

#include <memory>
#include <string>
#include <vector>

#include "iremote_broker.h"

namespace OHOS::se::omapi {
class ISecureElementReader;
class ISecureElementService : public OHOS::IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.nfc.se.ISecureElementService");
    virtual ~ISecureElementService() {}
    /**
     * @brief Get the list of available SE reader names. There must be no duplicated names in the returned list.
     * @return The all reader names
     */
    virtual std::vector<std::string> GetReaders() = 0;
    /**
     * @brief Get the reader instance by the name.
     * @param readerName the reader name
     * @param listener The front terminal(such as ability, etc.) listener. The service willl regist the death recipient
     * to the listener used to receive the OnRemoteDied handle, when the front terminal is died.
     * @return The Reader object.
     */
    virtual sptr<ISecureElementReader> GetReader(const std::string& readerName,
                                                 const OHOS::sptr<IRemoteObject>& listener) = 0;
    /**
     * @brief Checks if the application defined by the bundle name is authorized
     * to receive the NFC transaction events for the defined AID.
     * @param reader the reader name
     * @param aid the SE application ID
     * @param bundleNames about the bundle names
     * @return the true/false list
     */
    virtual std::vector<bool> IsNfcEventAllowed(const std::string& readerName,
                                                const std::string& aid,
                                                std::vector<std::string> bundleNames) = 0;
};
}  // namespace OHOS::se::omapi
#endif  // ISECURE_ELEMENT_SERVICE_H
