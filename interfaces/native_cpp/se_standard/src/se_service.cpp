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
#include "se_service.h"

#include <thread>

#include "connection_handler.h"
#include "intent.h"
#include "ipc_object_stub.h"
#include "isecure_element_reader.h"
#include "iservice_registry.h"
#include "loghelper.h"
#include "reader.h"
#include "secure_element_service_proxy.h"
#include "system_ability_connection_callback.h"
#include "system_ability_definition.h"

namespace OHOS {
namespace se {
namespace omapi {
std::string SEService::mUiccTerminal_ = "SIM";
void ProcessServiceConnected(std::weak_ptr<SEService::OnConnectedListener> listener)
{
    DebugLog("SaConnectionHandler::ProcessServiceConnected");
    if (listener.expired()) {
        return;
    }
    DebugLog("SaConnectionHandler::CBConnected");
    listener.lock()->CBConnected();
}

class SEService::SaConnectionHandler final : public ConnectionHandler {
public:
    explicit SaConnectionHandler(std::weak_ptr<SEService> seService) : seService_(seService) {}
    ~SaConnectionHandler()
    {
        seService_ = std::shared_ptr<SEService>();
    }
    void OnConnectedSystemAbility(const sptr<IRemoteObject>& connectionCallback)
    {
        DebugLog("SaConnectionHandler::OnConnectedSystemAbility");
        if (seService_.expired() || connectionCallback == nullptr) {
            return;
        }

        std::shared_ptr<SEService> seService = seService_.lock();
        seService->mSecureElementService_ = new SecureElementServiceProxy(connectionCallback);
        // start a thread to notify the front to connected to Se Service.
        if (seService->mConnListener_) {
            DebugLog("SaConnectionHandler::OnConnectedSystemAbility: thread");
            std::thread tConn(ProcessServiceConnected, seService->mConnListener_);
            tConn.detach();
        }
        DebugLog("SaConnectionHandler::OnConnectedSystemAbility End");
    }
    void OnDisConnectedSystemAbility(const std::u16string& name)
    {
        std::lock_guard<std::mutex> guard(seService_.lock()->mMutex_);
        seService_.lock()->mSecureElementService_ = nullptr;
    }

private:
    std::weak_ptr<SEService> seService_;
};

SEService::SEService() {}

SEService::~SEService()
{
    DebugLog("SEService::~SEService");
    mListener_ = nullptr;
    mConnListener_ = nullptr;
    mSecureElementService_ = nullptr;
    mSystemAbilityMgr_ = nullptr;
    mReaders_.clear();
}

bool SEService::LoadSecureElementService()
{
    //
    mSystemAbilityMgr_ = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (mSystemAbilityMgr_ == nullptr) {
        ErrorLog("SystemAbilityManager is unexist.");
        return false;
    }
    mConnectionHandler_ = std::make_shared<SaConnectionHandler>(shared_from_this());
    // SHALL Create SystemAbilityConnectionCallback and Connect System Ability
    // But heare can only get System Ability now
    sptr<IRemoteObject> obj = mSystemAbilityMgr_->GetSystemAbility(SE_MANAGER_SYS_ABILITY_ID);
    if (obj == nullptr) {
        DebugLog("To get the secure element service is failed!");
        return false;
    }
    mConnectionHandler_->OnConnectedSystemAbility(obj);
    return true;
}

bool SEService::InitSEService(std::shared_ptr<OnConnectedListener> listener)
{
    DebugLog("SEService::InitSEService.");
    if (!listener) {
        DebugLog("The Connected Listener can't be empty.");
        return false;
    }
    mConnListener_ = listener;
    mListener_ = new IPCObjectStub(u"se service listener");
    // load Secure Element Service
    return LoadSecureElementService();
}

bool SEService::IsConnected()
{
    if (mSecureElementService_) {
        return true;
    }
    return false;
}

std::vector<std::shared_ptr<Reader>> SEService::GetReaders()
{
    LoadReaders();
    std::vector<std::shared_ptr<Reader>> readerList;
    for (auto r : mReaders_) {
        if (r.first.empty()) {
            continue;
        }
        readerList.push_back(r.second);
    }
    return readerList;
}

std::shared_ptr<Reader> SEService::GetUiccReader(int slotNumber)
{
    if (slotNumber < 1) {
        return std::shared_ptr<Reader>();
    }

    LoadReaders();
    std::string readerName = mUiccTerminal_ + std::to_string(slotNumber);
    std::unordered_map<std::string, std::shared_ptr<Reader>>::const_iterator readerTmp = mReaders_.find(readerName);
    if (readerTmp == mReaders_.end()) {
        return std::shared_ptr<Reader>();
    }
    return readerTmp->second;
}
/**
 * @brief close the connection with SeEndService. And close the all sessions
 *
 */
void SEService::Shutdown()
{
    std::lock_guard<std::mutex> guard(mMutex_);
    if (!mSecureElementService_) {
        return;
    }

    for (auto r : mReaders_) {
        if (r.second) {
            r.second->CloseSessions();
        }
    }
    mReaders_.clear();
    // DisConnect Se End Service
    mSecureElementService_ = nullptr;
    mListener_ = nullptr;
}
/**
 * @brief Returns the version of the Open Mobile API Specification this implementation is based on.
 *
 * @return String containing the Open Mobile API version (e.g. “3.3” for Open Mobile API Specification version 3.3).
 */
std::string SEService::GetVersion() const
{
    return "3.3";
}

sptr<ISecureElementReader> SEService::GetReader(const std::string& readerName)
{
    if (!mSecureElementService_) {
        return nullptr;
    }
    return mSecureElementService_->GetReader(readerName, mListener_);
}

void SEService::LoadReaders()
{
    if (!mSecureElementService_) {
        return;
    }
    std::vector<std::string> readerNameList = mSecureElementService_->GetReaders();
    for (auto readerName : readerNameList) {
        std::unordered_map<std::string, std::shared_ptr<Reader>>::const_iterator readerTmp = mReaders_.find(readerName);
        // if the readerName hasn't the relational Reader, create the new Reader.
        if (readerTmp == mReaders_.end()) {
            sptr<ISecureElementReader> reader = GetReader(readerName);
            if (reader == nullptr) {
                InfoLog("Reader of (%s) is empty.", readerName.c_str());
                continue;
            }
            std::shared_ptr<Reader> newReader =
                std::shared_ptr<Reader>(new Reader(shared_from_this(), readerName, reader));
            mReaders_.insert(std::make_pair(readerName, newReader));
        }
    }
}
}  // namespace omapi
}  // namespace se
}  // namespace OHOS