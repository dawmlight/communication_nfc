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
#ifndef TEST_NFC_CE_HOST_SERVICE_H
#define TEST_NFC_CE_HOST_SERVICE_H

#include <memory>
#include <string>
#include <vector>

#include "ihost_card_emulation_service.h"
#include "loghelper.h"

namespace OHOS {
namespace nfc {
namespace test {
enum EN_NDEF_EMU_STATE { Ready, NdefAppSelected, CCSelected, NdefSelected };
class TestNfcCEHostService : public sdk::cardemulation::IHostCardEmulationService {
public:
    TestNfcCEHostService();
    ~TestNfcCEHostService() {}
    std::vector<unsigned char> HandleApdu(const std::vector<unsigned char>&) override;
    void OnDeactivated(int reason) override;

    void SetApduExpection(std::function<std::vector<unsigned char>(std::vector<unsigned char>)>);

    static bool IsApduBeginWith(const std::vector<unsigned char>& apduCmd, const std::vector<unsigned char>& prefix);

private:
    std::vector<unsigned char> apduData_;
    // EN_NDEF_EMU_STATE state_{EN_NDEF_EMU_STATE::Ready};
    std::function<std::vector<unsigned char>(std::vector<unsigned char>)> apduExpectionCB_;
};
}  // namespace test
}  // namespace nfc
}  // namespace OHOS
#endif /* TEST_NFC_CE_HOST_SERVICE_H */
