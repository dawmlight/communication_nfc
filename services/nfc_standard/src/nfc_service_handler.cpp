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
#include <string>

#include "intent.h"
#include "loghelper.h"
#include "nfc_service.h"
#ifdef _NFC_SERVICE_READER_
#include "tag_dispatcher.h"
#endif

#ifdef _NFC_SERVICE_HCE_
#include "card_emulation_manager.h"
#include "want.h"
#endif
#include "infc_agent_service.h"
#include "itag_end_point.h"
#include "nfc_discovery_params.h"
#include "nfc_service_define.h"
#include "nfc_service_handler.h"

using namespace std;
using namespace osal;

namespace OHOS {
using AppExecFwk::EventHandler;
using AppExecFwk::EventRunner;
namespace nfc {
using ncibal::ITagEndPoint;
NfcServiceHandler::NfcServiceHandler(const std::shared_ptr<EventRunner>& runner, std::weak_ptr<NfcService> servcie)
    : EventHandler(runner), mNfcService_(servcie), mDeviceHost_(servcie.lock()->mDeviceHost_)
{
}

NfcServiceHandler::~NfcServiceHandler() {}

void NfcServiceHandler::Intialize(std::weak_ptr<reader::TagDispatcher> tagDispatcher)
{
    mTagDispatcher_ = tagDispatcher;
}

void NfcServiceHandler::ProcessEvent(const AppExecFwk::InnerEvent::Pointer& event)
{
    std::shared_ptr<nfc::NfcService> nfcService = mNfcService_.lock();
    std::shared_ptr<nfc::reader::TagDispatcher> tagDispatcher = mTagDispatcher_.lock();
    std::shared_ptr<nfc::ncibal::IDeviceHost> deviceHost = mDeviceHost_.lock();

    uint32_t what = event->GetInnerEventId();
    DebugLog("NFC Service Handler receive a Message of %d", what);
    switch (what) {
#ifdef _NFC_SERVICE_HCE_
        case MSG_ROUTE_AID: {
            shared_ptr<RouteAidsEntry> entry = event->GetSharedObject<RouteAidsEntry>();
            deviceHost->AddAidRouting(entry->aid_, entry->route_, entry->aidInfo_);
            break;
        }
        case MSG_UNROUTE_AID: {
            shared_ptr<string> aid = event->GetSharedObject<string>();
            deviceHost->RemoveAidRouting(*aid);
            break;
        }
        case MSG_REGISTER_T3T_IDENTIFIER: {
            DebugLog("message to register LF_T3T_IDENTIFIER");
            deviceHost->DisableDiscovery();

            shared_ptr<string> t3tIdentifier = event->GetSharedObject<string>();
            deviceHost->RegisterT3tIdentifier(*t3tIdentifier);

            auto params = nfcService->GetDiscoveryParameters(nfcService->mScreenState_);
            bool shouldRestart = nfcService->mCurrentDiscoveryParams_->ShouldEnableDiscovery();
            deviceHost->EnableDiscovery(params->GetTechMask(),
                                        params->ShouldEnableReaderMode(),
                                        params->ShouldEnableHostRouting(),
                                        shouldRestart);
            break;
        }
        case MSG_DEREGISTER_T3T_IDENTIFIER: {
            DebugLog("message to deregister LF_T3T_IDENTIFIER");
            deviceHost->DisableDiscovery();

            shared_ptr<string> t3tIdentifier = event->GetSharedObject<string>();
            deviceHost->DeregisterT3tIdentifier(*t3tIdentifier);

            auto params = nfcService->GetDiscoveryParameters(nfcService->mScreenState_);
            bool shouldRestart = nfcService->mCurrentDiscoveryParams_->ShouldEnableDiscovery();
            deviceHost->EnableDiscovery(params->GetTechMask(),
                                        params->ShouldEnableReaderMode(),
                                        params->ShouldEnableHostRouting(),
                                        shouldRestart);
            break;
        }
        case MSG_COMMIT_ROUTING: {
            bool commit = false;
            {
                lock_guard<mutex> lock(nfcService->mtx_);
                if (nfcService->mCurrentDiscoveryParams_->ShouldEnableDiscovery()) {
                    commit = true;
                } else {
                    DebugLog(
                        "Not committing routing because discovery is "
                        "disabled.");
                }
            }
            if (commit) {
                deviceHost->CommitRouting();
            }
            break;
        }
        case MSG_RF_FIELD_ACTIVATED: {
            auto fieldOnIntent = std::make_shared<OHOS::AAFwk::Want>();
            fieldOnIntent->SetAction(NfcServiceHandler::ACTION_RF_FIELD_ON_DETECTED);
            nfcService->SendNfcEeAccessProtectedBroadcast(fieldOnIntent);
            break;
        }
        case MSG_RF_FIELD_DEACTIVATED: {
            auto fieldOnIntent = std::make_shared<OHOS::AAFwk::Want>();
            fieldOnIntent->SetAction(ACTION_RF_FIELD_OFF_DETECTED);
            nfcService->SendNfcEeAccessProtectedBroadcast(fieldOnIntent);
            break;
        }
        case MSG_TRANSACTION_EVENT: {
            std::shared_ptr<OffHostTransactionData> data = event->GetSharedObject<OffHostTransactionData>();
            auto cem = mCardEmulationManager_.lock();
            if (cem) {
                cem->OnOffHostTransaction();
            }
            nfcService->SendOffHostTransactionEvent(data->aid_, data->data_, data->seName_);
            break;
        }
        case MSG_PREFERRED_PAYMENT_CHANGED: {
            auto preferredPaymentChangedWant = std::make_shared<OHOS::AAFwk::Want>();
            preferredPaymentChangedWant->SetAction(ACTION_PREFERRED_PAYMENT_CHANGED);
            preferredPaymentChangedWant->SetParam(EXTRA_PREFERRED_PAYMENT_CHANGED_REASON, (long)event->GetParam());
            nfcService->SendPreferredPaymentChangedEvent(preferredPaymentChangedWant);
            break;
        }
#endif
        case MSG_EVENT_RESUME_POLLING:
            nfcService->ResumeDiscovering();
            break;
        case MSG_NDEF_TAG:
#ifdef _NFC_SERVICE_READER_
            tagDispatcher->DispatchTag(event->GetSharedObject<ITagEndPoint>());
            break;
        case MSG_TAG_DEBOUNCE:
            tagDispatcher->ResetIgnoreTag();
#endif
            break;
        case MSG_APPLY_SCREEN_STATE: {
            nfcService->ApplyScreenState(event->GetParam());
            break;
        }
        case MSG_TOAST_DEBOUNCE_EVENT:
#ifdef _NFC_SERVICE_READER_
            reader::TagDispatcher::DebounceMsgProcess();
#endif
            break;
        default:
            ErrorLog("Unknown message (%d) received", what);
            break;
    }
}
}  // namespace nfc
}  // namespace OHOS