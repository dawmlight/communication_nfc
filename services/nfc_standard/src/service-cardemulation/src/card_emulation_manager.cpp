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

#include "card_emulation_manager.h"

#include <algorithm>
#include <cassert>
#include <cstdio>

#include "card_emulation_agent.h"
#include "card_emulation_def.h"
#include "card_emulation_device_host.h"
#include "card_emulation_error.h"
#include "card_emulation_service.h"
#include "card_emulation_util.h"
#include "loghelper.h"
#ifdef USE_HILOG
DEFINE_NFC_LOG_LABEL("CardEmulationManager");
#endif

namespace OHOS::nfc::cardemulation {
CardEmulationManager::CardEmulationManager(std::shared_ptr<nfc::NfcService> nfcService)
    : ceService_(),
    userSwitchHandlers_(),
    ceAgent_()

{
    ceService_ = std::make_shared<CardEmulationService>(std::make_shared<CardEmulationDeviceHost>(nfcService));
    userSwitchHandlers_.push_back(ceService_);
};
CardEmulationManager::~CardEmulationManager() = default;

int CardEmulationManager::Init(void)
{
    ceService_->Init();
    if (!ceAgent_) {
        ceAgent_ = OHOS::sptr<CardEmulationAgent>(new CardEmulationAgent(ceService_));
        ceAgent_->Init();
    }
    return ERR_OK;
}

void CardEmulationManager::Deinit(void)
{
    DebugLog("Deinit");
    ceService_->Deinit();
}

int CardEmulationManager::OnUserSwitched(int userId)
{
    DebugLog("OnUserSwitched");
    std::for_each(userSwitchHandlers_.begin(),
                  userSwitchHandlers_.end(),
                  [userId](decltype(userSwitchHandlers_)::reference r) { r->OnUserSwitched(userId); });
    return ERR_OK;
}

OHOS::sptr<nfc::sdk::cardemulation::ICardEmulationAgent>
nfc::cardemulation::CardEmulationManager::GetCardEmulationAgent()
{
    DebugLog("ceAgent_: %p", ceAgent_.GetRefPtr());
    return ceAgent_;
}
void CardEmulationManager::OnHostCardEmulationActivated(int technology)
{
    if (technology == TECH_HCE_APDU) {
        ceService_->OnHCEActivated();
    }
}

void CardEmulationManager::OnHostCardEmulationData(int technology, const std::string& data)
{
    if (technology == TECH_HCE_APDU) {
        auto bytes = StringToBytes(data);
#ifdef USE_HILOG
        DebugLog("OnHostCardEmulationData:\n\t%{public}s\n", BytesToHexStr(bytes).c_str());

#else
        DebugLog("OnHostCardEmulationData:\n\t%s\n", BytesToHexStr(bytes).c_str());
#endif

        if (bytes.empty()) {
            return;
        }
        ceService_->OnHCEData(&bytes[0], bytes.size());
    }
}

void CardEmulationManager::OnHostCardEmulationDeactivated(int technology)
{
    if (technology == TECH_HCE_APDU) {
        ceService_->OnHCEDeactivated();
    }
}

void CardEmulationManager::OnOffHostTransaction(void)
{
    ceService_->OnOffHostTransaction();
}
void nfc::cardemulation::CardEmulationManager::OnNfcEnabled()
{
    ceService_->OnNfcEnabled();
}

void nfc::cardemulation::CardEmulationManager::OnNfcDisabled()
{
    ceService_->OnNfcDisabled();
}

void nfc::cardemulation::CardEmulationManager::OnSecureNfcToggled()
{
    ceService_->OnSecureNfcToggled();
}
}  // namespace OHOS::nfc::cardemulation
