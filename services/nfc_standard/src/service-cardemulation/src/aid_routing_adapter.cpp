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

#include "aid_routing_adapter.h"

#include <algorithm>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <sstream>
#include <thread>

#include "card_emulation_def.h"
#include "card_emulation_error.h"
#include "card_emulation_util.h"
#include "loghelper.h"

#ifdef USE_HILOG
DEFINE_NFC_LOG_LABEL("AidRoutingAdapter");
#endif

namespace OHOS::nfc::cardemulation {
static const std::vector<std::string> LOCATION_PREFIXES{NFC_EE_ESE, NFC_EE_UICC};
AidRoutingAdapter::AidRoutingAdapter(const std::shared_ptr<ICardEmulationDeviceHost>& nci)
    : defaultRoute_(0),
    defaultOffHostRoute_(0),
    routingMode_(AID_ROUTING_MODE_MASK_PREFIX),
    routingTableCapacity_(0),
    inited_(false),
    nci_(nci)
{
}

AidRoutingAdapter::~AidRoutingAdapter()
{
}

int AidRoutingAdapter::GetDefaultRoute()
{
    Init();
    return defaultRoute_;
}
int AidRoutingAdapter::GetDefaultOffHostRoute()
{
    Init();
    return defaultOffHostRoute_;
}
int AidRoutingAdapter::GetAidRoutingMode()
{
    Init();
    return routingMode_;
}
int AidRoutingAdapter::GetAidRoutingTableCapacity()
{
    Init();
    return routingTableCapacity_;
}
int AidRoutingAdapter::GetRemainRoutingTableSize()
{
    auto nci = nci_.lock();
    if (nci) {
        return nci->GetRemainRoutingTableSize();
    }
    return 0;
}
bool AidRoutingAdapter::ClearRoutingTable()
{
    auto nci = nci_.lock();
    if (nci) {
        return nci->ClearRouting();
    }
    return false;
}
int AidRoutingAdapter::AddAidRoutingEntry(const std::vector<unsigned char>& aid, int target, int aidType)
{
    if ((aid.size() < AID_MIN_LENGTH_IN_BYTES && !aid.empty()) || aid.size() > AID_MAX_LENGTH_IN_BYTES) {
        return ERR_AID_ILLIGLE_LENGTH;
    }
    auto nci = nci_.lock();
    if (nci) {
        DebugLog("add aid to nci. aid(HEXSTR): %s, env: %d, type: %d", BytesToHexStr(aid).c_str(), target, aidType);

        std::string t(aid.begin(), aid.end());
        return nci->AddAidRouting(t, target, aidType) ? ERR_OK : ERR_ADD_ROUTING_AID_FAILED;
    }
    return ERR_NOT_INIT_NCI;
}
int AidRoutingAdapter::RemoveAidRoutingEntry(const std::vector<unsigned char>& aid)
{
    if (aid.size() < AID_MIN_LENGTH_IN_BYTES || aid.size() > AID_MAX_LENGTH_IN_BYTES) {
        return ERR_AID_ILLIGLE_LENGTH;
    }
    auto nci = nci_.lock();
    if (nci) {
        std::string t(aid.begin(), aid.end());
        return nci->RemoveAidRouting(t) ? ERR_OK : ERR_REMOVE_ROUTING_AID_FAILED;
    }
    return ERR_NOT_INIT_NCI;
}
int AidRoutingAdapter::CommitAidRouting()
{
    auto nci = nci_.lock();
    if (nci) {
        int rv = nci->CommitRouting() ? ERR_OK : ERR_COMMIT_ROUTING_FAILED;
        DebugLog("nci CommitRouting return: %d", rv);

        return rv;
    }
    return ERR_NOT_INIT_NCI;
}

int AidRoutingAdapter::LocationStringToInt(const std::string& loc) const
{
    return ValidLocationStringToInt(loc, defaultRoute_);
}

bool AidRoutingAdapter::IsDefaultRoute(const std::string& location)
{
    return LocationStringToInt(location) == defaultRoute_;
}
std::string AidRoutingAdapter::ReviseSecureElementName(const std::string& loc) const
{
    // User definitions are no longer modified
    return loc;
}

bool AidRoutingAdapter::IsSurpported(const std::string& loc) const
{
    if (loc.empty()) {
        return false;
    }
    constexpr int invalidId = -1;
    int id = ValidLocationStringToInt(loc, invalidId);
    return (id != invalidId);
}

std::vector<std::string> AidRoutingAdapter::GetSupportedEses()
{
    Init();
    std::vector<std::string> rv;
    for (size_t i = 0; i < eSes_.size(); i++) {
        rv.emplace_back(NFC_EE_ESE + std::to_string(i + 1));
    }
    return rv;
}

std::vector<std::string> AidRoutingAdapter::GetSupportedUiccs()
{
    Init();
    std::vector<std::string> rv;
    for (size_t i = 0; i < uiccs_.size(); i++) {
        rv.emplace_back(NFC_EE_UICC + std::to_string(i + 1));
    }

    return rv;
}

void nfc::cardemulation::AidRoutingAdapter::Init()
{
    if (inited_) {
        return;
    }
    auto nci = nci_.lock();
    if (nci) {
        defaultRoute_ = nci->GetDefaultRoute();
        defaultOffHostRoute_ = nci->GetDefaultOffHostRoute();
        routingMode_ = nci->GetAidMatchingMode();
        routingTableCapacity_ = nci->GetAidRoutingTableSize();
        uiccs_ = nci->GetOffHostUiccRoute();
        eSes_ = nci->GetOffHostEseRoute();
        inited_ = true;
    }
}
int AidRoutingAdapter::ValidLocationStringToInt(const std::string& loc, int defaultId) const
{
    if (StringEqual(loc, NFC_EE_HOST, true)) {
        return 0;  // host id: 0
    }
    // eSE, UICC(SIM) id from nci

    // return default_id when failed.
    auto transformFunc = [defaultId](const std::string& src, const std::string& prefix, const std::vector<int>& ids) {
        if (src.empty()) {
            return defaultId;
        }
        auto si = src.substr(prefix.size());
        size_t n = 1;
        if (si.empty()) {
            // eSE == eSE1 -> 1
            n = 1;
        } else {
            // eSEn -> n
            n = static_cast<size_t>(std::stoi(si));
        }
        if (n > 0u && n <= ids.size()) {
            return ids.at(n - 1);
        }
        return defaultId;
    };
    if (StringStartsWith(loc, NFC_EE_ESE, true)) {
        return transformFunc(loc, NFC_EE_ESE, eSes_);
    }
    if (StringStartsWith(loc, NFC_EE_UICC, true)) {
        return transformFunc(loc, NFC_EE_UICC, uiccs_);
    }
    return defaultId;
}
}  // namespace OHOS::nfc::cardemulation
