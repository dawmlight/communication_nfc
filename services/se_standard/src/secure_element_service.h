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
#ifndef SECURE_ELEMENT_SERVICE_H
#define SECURE_ELEMENT_SERVICE_H

#include <mutex>

#include "secure_element_service_stub.h"

namespace OHOS::se::omapi {
class ISecureElementReader;
}
using ISEReader = OHOS::se::omapi::ISecureElementReader;

namespace OHOS {
namespace se {
class SeEndService;
class Terminal;
using IdTerminalPair = std::pair<std::string, std::shared_ptr<Terminal>>;

class SecureElementService final : public omapi::SecureElementServiceStub {
    // the Receiver bind to the front Death.
    class SeReaderDeathReceiver;

public:
    ~SecureElementService() override;
    /**
     * @brief Get the reader names that provides this session.
     * @return The all reader names
     */
    std::vector<std::string> GetReaders() override;
    /**
     * @brief Get the reader that provides this session.
     * @param readerName the reader name
     * @param listener The front terminal(such as ability, etc.) listener. The service willl regist the death recipient
     * to the listener used to receive the OnRemoteDied handle, when the front terminal is died.
     * @return The Reader object.
     */
    sptr<ISEReader> GetReader(std::string const& readerName, const sptr<IRemoteObject>& listener) override;
    /**
     * @brief Checks if the application defined by the package name is authorized
     * to receive the NFC transaction events for the defined AID.
     * @param reader the reader name
     * @param aid the SE application ID
     * @param packageNames about the package names
     * @return the true/false list
     */
    std::vector<bool> IsNfcEventAllowed(const std::string& readerName,
                                        const std::string& cAid,
                                        std::vector<std::string> packageNames) override;

private:
    explicit SecureElementService(std::weak_ptr<SeEndService> service, std::vector<IdTerminalPair>& table);

    /**
     * @brief Retrieve the special terminal of the SE proxy.
     * @param reader the special name
     * @return Returns the terminal instance. If the terminal isn't found, throw IllegalArgumentError.
     */
    std::weak_ptr<Terminal> GetTerminal(const std::string& reader);
    void RemoveDeathReceiver(sptr<IRemoteObject::DeathRecipient> recipient);

private:
    std::weak_ptr<SeEndService> mSEService_;
    std::vector<IdTerminalPair>& mTerminals_;
    std::vector<sptr<IRemoteObject::DeathRecipient>> deathReceivers_;
    std::mutex mtx_{};

    friend class SecureElementReader;
    friend class SeEndService;
};
}  // namespace se
}  // namespace OHOS
#endif  // !SECURE_ELEMENT_SERVICE_H
