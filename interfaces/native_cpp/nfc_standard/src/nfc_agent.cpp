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
#include "nfc_agent.h"

#include "ability.h"
#include "element_name.h"
#include "host_card_emulation.h"
#include "icard_emulation_agent.h"
#include "infc_agent_service.h"
#include "ipc_skeleton.h"
#include "iremote_object.h"
#include "iservice_registry.h"
#include "itag_session.h"
#include "loghelper.h"
#include "nfc_ability_manager.h"
#include "nfc_map.h"
#include "nfc_sdk_common.h"
#include "nfc_service_proxy.h"
#include "offhost_card_emulation.h"
#include "system_ability.h"
#include "system_ability_definition.h"
#include "tag.h"

using namespace std;
using OHOS::nfc::INfcAgentService;
using OHOS::nfc::reader::ITagSession;
// using namespace OHOS::AppExecFwk;

namespace OHOS {
namespace nfc {
namespace sdk {
std::shared_ptr<OHOS::nfc::NfcServiceProxy> NfcAgent::nfcServiceProxy_;
std::weak_ptr<INfcAgentService> NfcAgent::mNfcAgentService_;
OHOS::sptr<ITagSession> NfcAgent::mTagSession_;
bool NfcAgent::mbInitialized_ = false;
bool NfcAgent::mbHasNfcCapability_ = false;
OHOS::sptr<cardemulation::ICardEmulationAgent> NfcAgent::mNfcCardEmulationService_{};
std::mutex NfcAgent::mMutex_;
std::map<std::weak_ptr<OHOSApplication>, std::weak_ptr<NfcAgent>, std::owner_less<std::weak_ptr<OHOSApplication>>>
    NfcAgent::mNfcAgents_;

NfcAgent::NfcAgent()
{
    mNfcAbilityManager_ = std::make_shared<NfcAbilityManager>();
    DebugLog("[NfcAgent::NfcAgent] new ability manager");
}

NfcAgent::~NfcAgent()
{
    DebugLog("destruct NFCAgent");
}
std::shared_ptr<NfcAgent> NfcAgent::GetNFCAgent(std::weak_ptr<OHOSApplication> application)
{
    InfoLog("NfcAgent::GetNFCAgent in.");
    if (application.expired()) {
        return std::shared_ptr<NfcAgent>();
    }
    return InitNFCAgent(application);
}

std::shared_ptr<NfcAgent> NfcAgent::InitNFCAgent(std::weak_ptr<OHOSApplication> application)
{
    DebugLog("NfcAgent::InitNFCAgent in.");
    std::lock_guard<std::mutex> guard(mMutex_);
    if (!mbInitialized_ || mNfcAgentService_.expired()) {
        // Init NFCService
        if (!HasNfcCapability() && !HasNfcHceCapability()) {
            DebugLog("NfcAgent::InitNFCAgent nfc not support, error.");
            return std::shared_ptr<NfcAgent>();
        }

        sptr<ISystemAbilityManager> systemAbilityMgr =
            SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
        OHOS::sptr<OHOS::IRemoteObject> obj = systemAbilityMgr->GetSystemAbility(NFC_MANAGER_SYS_ABILITY_ID + 10);
        if (!obj) {
            DebugLog("Nfc Service Is Unexist.");
            return std::shared_ptr<NfcAgent>();
        }
        nfcServiceProxy_ = std::make_shared<NfcServiceProxy>(obj);
        mNfcAgentService_ = nfcServiceProxy_;
        if (HasNfcCapability()) {
#ifdef _NFC_SERVICE_READER_
            mTagSession_ = mNfcAgentService_.lock()->GetNfcTagInterface();
            DebugLog("NfcAgent::InitNFCAgent GetNfcTagInterface out");
#endif
            if (!mTagSession_) {
                DebugLog("NfcAgent::InitNFCAgent tag session invalid.");
                return std::shared_ptr<NfcAgent>();
            }
        }
        if (HasNfcHceCapability()) {
            // Init CardEmulation
#ifdef _NFC_SERVICE_HCE_
            DebugLog("GetNfcCardEmulationInterface ...");
            if (!nfcServiceProxy_) {
                DebugLog("NfcAgent::InitNFCAgent nfcService invalid.");
                return std::shared_ptr<NfcAgent>();
            }

            mNfcCardEmulationService_ = nfcServiceProxy_->GetNfcCardEmulationInterface();
            DebugLog("GetNfcCardEmulationInterface: %p", mNfcCardEmulationService_.GetRefPtr());
            if (!mNfcCardEmulationService_) {
                DebugLog("NfcAgent::InitNFCAgent mNfcCardEmulationService_ invalid.");
                return std::shared_ptr<NfcAgent>();
            }
#endif
        }

        mbInitialized_ = true;
    }
    DebugLog("NfcAgent::InitNFCAgent new nfc agent.");

    auto nfcAgentTmp = mNfcAgents_.find(application);
    if (nfcAgentTmp == mNfcAgents_.end()) {
        std::shared_ptr<NfcAgent> nfcAgent = std::make_shared<NfcAgent>();
        mNfcAgents_.insert(make_pair(application, nfcAgent));

        DebugLog("NfcAgent::InitNFCAgent new a nfcagent for application.");
        return nfcAgent;
    } else {
        DebugLog("NfcAgent::InitNFCAgent find a nfcagent for application.");
        return nfcAgentTmp->second.lock();
    }
}
// Open NFC
int NfcAgent::ActiveNfc()
{
    DebugLog("NFCAgent::ActiveNfc in.");
    if (mNfcAgentService_.expired()) {
        return NfcErrorCode::NFC_SDK_ERROR_NOT_INITIALIZED;
    }
    return mNfcAgentService_.lock()->TurnOn();
}

// Close NFC
int NfcAgent::DeactiveNfc()
{
    DebugLog("NFCAgent::DeactiveNfc in.");
    if (mNfcAgentService_.expired()) {
        return NfcErrorCode::NFC_SDK_ERROR_NOT_INITIALIZED;
    }
    return mNfcAgentService_.lock()->TurnOff(true);
}

// get NFC state
int NfcAgent::GetNfcState()
{
    if (mNfcAgentService_.expired()) {
        return NfcErrorCode::NFC_SDK_ERROR_NOT_INITIALIZED;
    }
    return mNfcAgentService_.lock()->GetState();
}

int NfcAgent::ActiveNfcSecure()
{
    DebugLog("NFCAgent::ActiveNfcSecure in.");
    if (!HasNfcCapability()) {
        return NfcErrorCode::NFC_SDK_ERROR_DEVICE_NOT_SUPPORT_NFC;
    }
    if (mNfcAgentService_.expired()) {
        return NfcErrorCode::NFC_SDK_ERROR_NOT_INITIALIZED;
    }
    return mNfcAgentService_.lock()->SetNfcSecure(true);
}

int NfcAgent::DeactiveNfcSecure()
{
    DebugLog("NFCAgent::ActiveNfcSecure in.");
    if (!HasNfcCapability()) {
        return NfcErrorCode::NFC_SDK_ERROR_DEVICE_NOT_SUPPORT_NFC;
    }
    if (mNfcAgentService_.expired()) {
        return NfcErrorCode::NFC_SDK_ERROR_NOT_INITIALIZED;
    }
    return mNfcAgentService_.lock()->SetNfcSecure(false);
}

int NfcAgent::GetNfcSecureState()
{
    if (mNfcAgentService_.expired()) {
        return NfcErrorCode::NFC_SDK_ERROR_NOT_INITIALIZED;
    }
    return mNfcAgentService_.lock()->IsNfcSecureEnabled();
}

bool NfcAgent::HasSecureNfcCapability()
{
    if (mNfcAgentService_.expired()) {
        return false;
    }
    return mNfcAgentService_.lock()->DeviceSupportsNfcSecure();
}

void NfcAgent::PausePolling(int timeoutInMs)
{
    DebugLog("NFCAgent::PausePolling in.");
    if (mNfcAgentService_.expired()) {
        return;
    }
    mNfcAgentService_.lock()->PausePolling(timeoutInMs);
    return;
}

void NfcAgent::ResumePolling()
{
    if (mNfcAgentService_.expired()) {
        return;
    }
    mNfcAgentService_.lock()->ResumePolling();
}

bool NfcAgent::HasNfcCapability()
{
    // check package manager
    return true;
}

bool NfcAgent::HasNfcHceCapability()
{
    // check package manager
    return true;
}

OHOS::wptr<cardemulation::ICardEmulationAgent> NfcAgent::GetNfcCardEmulationService()
{
    return mNfcCardEmulationService_;
}

std::shared_ptr<cardemulation::IHostCardEmulation> NfcAgent::CreateHostCardEmulation(
    const OHOS::AppExecFwk::ElementName& name)
{
    using namespace OHOS::nfc::sdk::cardemulation;
    if (mNfcCardEmulationService_) {
        return std::make_shared<HostCardEmulation>(name, mNfcCardEmulationService_);
    }
    return nullptr;
}

std::shared_ptr<cardemulation::IOffHostCardEmulation> NfcAgent::CreateOffHostCardEmulation(
    const OHOS::AppExecFwk::ElementName& name)
{
    using namespace OHOS::nfc::sdk::cardemulation;
    if (mNfcCardEmulationService_) {
        return std::make_shared<OffHostCardEmulation>(name, mNfcCardEmulationService_);
    }
    return nullptr;
}

int NfcAgent::ActiveNfcReaderMode(std::weak_ptr<OHOS::Ability> ability,
                                  std::weak_ptr<NfcAgent::TagCallback> callback,
                                  int tagTypeFilter,
                                  std::weak_ptr<NfcMap> extras)
{
    DebugLog("NfcAgent::ActiveNfcReaderMode in");
    if (ability.expired() || callback.expired() || !mNfcAbilityManager_ || extras.expired()) {
        DebugLog("[NfcAgent::ActiveNfcReaderMode] param error！！");
        return NfcErrorCode::NFC_SDK_ERROR_INVALID_PARAM;
    }

    DebugLog("[NfcAgent::ActiveNfcReaderMode] Application！！");
    return mNfcAbilityManager_->ActiveNfcReaderMode(ability, callback, tagTypeFilter, extras);
}

int NfcAgent::DeactiveNfcReaderMode(std::weak_ptr<OHOS::Ability> ability)
{
    DebugLog("NfcAgent::DeactiveNfcReaderMode in");
    if (ability.expired() || !mNfcAbilityManager_) {
        DebugLog("[NfcAgent::DeactiveNfcReaderMode] ability is null");
        return NfcErrorCode::NFC_SDK_ERROR_INVALID_PARAM;
    }
    return mNfcAbilityManager_->DeactiveNfcReaderMode(ability);
}

bool nfc::sdk::NfcAgent::IgnoreTag(std::weak_ptr<Tag> tag,
                                   std::weak_ptr<NfcAgent::TagCallback> callback,
                                   int debounceMs)
{
    if (tag.expired() || mNfcAgentService_.expired()) {
        return false;
    }
    std::shared_ptr<ITagRemovedCallback> tagCallback = std::make_shared<TagRemovedCallback>(callback);
    return mNfcAgentService_.lock()->SetIgnore(tag.lock()->GetServiceHandle(), debounceMs, tagCallback);
}

void nfc::sdk::NfcAgent::TagRemovedCallback::OnTagRemoved()
{
    if (mCallback_.expired()) {
        return;
    }
    mCallback_.lock()->CBTagRemovedMsg();
    return;
}
}  // namespace sdk
}  // namespace nfc
}  // namespace OHOS