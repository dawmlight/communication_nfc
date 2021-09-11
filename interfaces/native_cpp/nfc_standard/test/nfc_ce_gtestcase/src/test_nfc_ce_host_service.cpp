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
#include "test_nfc_ce_host_service.h"

#include "loghelper.h"

namespace OHOS::nfc::test {
TestNfcCEHostService::TestNfcCEHostService() {}
std::vector<unsigned char> TestNfcCEHostService::HandleApdu(const std::vector<unsigned char>& apdu)
{
    return apduExpectionCB_(apdu);
}
void TestNfcCEHostService::OnDeactivated(int reason)
{
    DebugLog("reason: %d\n", reason);
}

void TestNfcCEHostService::SetApduExpection(std::function<std::vector<unsigned char>(std::vector<unsigned char>)> f)
{
    apduExpectionCB_ = std::move(f);
}

bool TestNfcCEHostService::IsApduBeginWith(const std::vector<unsigned char>& apduCmd,
                                           const std::vector<unsigned char>& prefix)
{
    if (apduCmd.size() < prefix.size()) {
        return false;
    }
    return std::equal(apduCmd.cbegin(), apduCmd.cbegin() + prefix.size(), prefix.cbegin(), prefix.cend());
}
}  // namespace OHOS::nfc::test
