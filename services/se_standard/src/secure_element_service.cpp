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
#include "secure_element_service.h"

#include "bundle_manager.h"
#include "loghelper.h"
#include "se_common_exception.h"
#include "se_end_service.h"
#include "secure_element_reader.h"
#include "terminal.h"

namespace OHOS {
namespace se {
/**
 * @brief The Notification recipient, when the terminal of calling service is died.
 */
class SecureElementService::SeReaderDeathReceiver : public IRemoteObject::DeathRecipient {
public:
    explicit SeReaderDeathReceiver(sptr<SecureElementReader> seReader,
                                   sptr<IRemoteObject> listener,
                                   wptr<SecureElementService> seService)
        : seReader_(seReader), mListener_(listener), seService_(seService)
    {
    }

    void OnRemoteDied(const wptr<IRemoteObject>& object) override
    {
        if (object == nullptr) {
            return;
        }
        InfoLog("SecureElementService::SeReaderDeathReceiver::OnRemoteDied: %p", object.GetRefPtr());
        if (object == mListener_) {
            InfoLog("SecureElementService::SeReaderDeathReceiver::OnRemoteDied: Object == Listener");
        }
        mListener_->RemoveDeathRecipient(this);
        seReader_->CloseSessions();
        seReader_ = nullptr;

        seService_->RemoveDeathReceiver(this);
    }

private:
    DISALLOW_COPY_AND_MOVE(SeReaderDeathReceiver);

private:
    sptr<SecureElementReader> seReader_{};
    sptr<IRemoteObject> mListener_{};
    wptr<SecureElementService> seService_{};
};
/**
 * @brief Get the reader names that provides this session.
 * @return The all reader names
 */
std::vector<std::string> SecureElementService::GetReaders()
{
    std::vector<std::string> readers{};
    for (IdTerminalPair terminalPair : mTerminals_) {
        readers.push_back(terminalPair.first);
    }
    return readers;
}
/**
 * @brief Get the reader that provides this session.
 * @param readerName the reader name
 * @param listener The front terminal(such as ability, etc.) listener. The service willl regist the death recipient to
 * the listener used to receive the OnRemoteDied handle, when the front terminal is died.
 * @return The Reader object. If the reader object isn't found, throw IllegalArgumentError
 */
sptr<ISEReader> SecureElementService::GetReader(std::string const& readerName, const sptr<IRemoteObject>& listener)
{
    if (listener == nullptr) {
        throw IllegalArgumentError("listener must not be null");
    }

    std::weak_ptr<Terminal> terminal = GetTerminal(readerName);
    std::lock_guard<std::mutex> lock(mtx_);
    sptr<SecureElementReader> reader = new SecureElementReader(mSEService_, terminal);
    sptr<IRemoteObject::DeathRecipient> deathReceiver = new SeReaderDeathReceiver(reader, listener, this);
    listener->AddDeathRecipient(deathReceiver);
    deathReceivers_.push_back(deathReceiver);
    return reader;
}
/**
 * @brief Checks if the application defined by the bundle name is authorized
 * to receive the NFC transaction events for the defined AID.
 * @param reader the reader name
 * @param aid the SE application ID
 * @param bundleNames about the bundle names
 * @return the true/false list
 */
std::vector<bool> SecureElementService::IsNfcEventAllowed(const std::string& reader,
                                                          const std::string& cAid,
                                                          std::vector<std::string> bundleNames)
{
    std::string aid = cAid;
    if (aid.empty()) {
        aid = std::string{0x00, 0x00, 0x00, 0x00, 0x00};
    }
    if (aid.length() < 5 || aid.length() > 16) {
        throw IllegalArgumentError("AID out of range");
    }
    if (bundleNames.empty()) {
        throw IllegalArgumentError("bundle names not specified");
    }
    std::weak_ptr<Terminal> terminal = GetTerminal(reader);
    return terminal.lock()->IsNfcEventAllowed(mSEService_.lock()->GetBundleManager(), aid, bundleNames);
}

/**
 * @brief Retrieve the special terminal of the SE proxy.
 * @param reader the special name
 * @return Returns the terminal instance. If the terminal isn't found, throw IllegalArgumentError.
 */
std::weak_ptr<Terminal> SecureElementService::GetTerminal(const std::string& reader)
{
    if (reader.empty()) {
        throw IllegalArgumentError("reader must not be null");
    }
    std::string name = reader;
    if (name.compare("SIM") == 0) {
        name = "SIM1";
    }

    std::weak_ptr<Terminal> terminal{};
    for (IdTerminalPair terminalPair : mTerminals_) {
        if (terminalPair.first.compare(name) == 0) {
            terminal = terminalPair.second;
            break;
        }
    }
    if (terminal.expired()) {
        throw IllegalArgumentError("Reader: " + name + " doesn't exist");
    }
    return terminal;
}

void SecureElementService::RemoveDeathReceiver(sptr<IRemoteObject::DeathRecipient> recipient)
{
    if (recipient == nullptr) {
        return;
    }

    std::lock_guard<std::mutex> lock(mtx_);
    std::vector<sptr<IRemoteObject::DeathRecipient>>::iterator iter = deathReceivers_.begin();
    while (iter != deathReceivers_.end()) {
        if (recipient == (*iter)) {
            DebugLog("DeathReceiver is ereased.");
            deathReceivers_.erase(iter);
            return;
        }
        ++iter;
    }
}

SecureElementService::SecureElementService(std::weak_ptr<SeEndService> service, std::vector<IdTerminalPair>& table)
    : mSEService_(service), mTerminals_(table)
{
}

SecureElementService::~SecureElementService() {}
}  // namespace se
}  // namespace OHOS