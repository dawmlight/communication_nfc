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
#include "app_callback_proxy.h"
#include "intent.h"
#include "iremote_object.h"
#include "loghelper.h"
#include "nfc_service_define.h"

#ifdef _NFC_SERVICE_READER_
#include "itag_session.h"
#include "tag_dispatcher.h"
#endif

#ifdef _NFC_SERVICE_HCE_
#include "icard_emulation_agent.h"
#endif

#include "nfc_agent_service.h"
#include "nfc_service.h"

namespace OHOS {
namespace nfc {
int NfcAgentService::GetState()
{
    return mNfcService_.lock()->GetState();
}

bool NfcAgentService::TurnOn()
{
    mNfcService_.lock()->StartEnableDisableNfcTask(TASK_ENABLE, true);
    return true;
}

bool NfcAgentService::TurnOff(bool saveState)
{
    mNfcService_.lock()->StartEnableDisableNfcTask(TASK_DISABLE, saveState);
    return true;
}

void NfcAgentService::PausePolling(int timeoutInMs)
{
    mNfcService_.lock()->PauseDiscovering(timeoutInMs);
}

void NfcAgentService::ResumePolling()
{
    mNfcService_.lock()->ResumeDiscovering();
}

bool NfcAgentService::DeviceSupportsNfcSecure()
{
    return mNfcService_.lock()->DeviceSupportsNfcSecure();
}

bool NfcAgentService::IsNfcSecureEnabled()
{
    return mNfcService_.lock()->IsNfcSecureEnabled();
}

bool NfcAgentService::SetNfcSecure(bool enable)
{
    return mNfcService_.lock()->SetNfcSecure(enable);
}

#ifdef _NFC_SERVICE_HCE_
OHOS::sptr<OHOS::nfc::sdk::cardemulation::ICardEmulationAgent> NfcAgentService::GetNfcCardEmulationInterface()
{
    auto s = mNfcService_.lock();
    if (s) {
        DebugLog("mNfcService_: %p, user_count: %ld", s.get(), s.use_count());
        return s->GetNfcCardEmulationInterface();
    }
    return {};
}
#ifdef _NFC_F_CE_
std::shared_ptr<OHOS::nfc::sdk::cardemulation::INfcFCardEmulation> NfcAgentService::GetNfcFCardEmulationInterface()
{
    auto s = mNfcService_.lock();
    if (s) {
        return s->GetNfcFCardEmulationInterface();
    }
    return {};
}
#endif
#endif  // _NFC_SERVICE_HCE_

#ifdef _NFC_SERVICE_READER_
void NfcAgentService::SetRWMode(const sptr<IRemoteObject>& readerObj,
                                std::weak_ptr<nfc::sdk::IAppCallback> appCallback,
                                int flags,
                                std::weak_ptr<sdk::NfcMap> extras)
{
    mTagDispatcher_.lock()->SetRWMode(readerObj, appCallback, flags, extras);
}

bool NfcAgentService::SetIgnore(int handle, int debounceMs, std::weak_ptr<nfc::sdk::ITagRemovedCallback> removeCallback)
{
    return mTagDispatcher_.lock()->Ignore(handle, debounceMs, removeCallback);
}

void NfcAgentService::SetFrontDispatch()
{
    mTagDispatcher_.lock()->SetFrontDispatch(nullptr);
}

OHOS::sptr<reader::ITagSession> NfcAgentService::GetNfcTagInterface()
{
    return mNfcService_.lock()->GetTagSession();
}
#endif

NfcAgentService::NfcAgentService(std::weak_ptr<NfcService> nfcService, std::weak_ptr<osal::Context> context)
    : NfcServiceStub(context), mNfcService_(nfcService)
{
    mTagDispatcher_ = mNfcService_.lock()->mTagDispatcher_;
}

NfcAgentService::~NfcAgentService() {}
}  // namespace nfc
}  // namespace OHOS