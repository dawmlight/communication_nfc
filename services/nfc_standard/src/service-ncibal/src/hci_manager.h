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
#ifndef HCI_MANAGER_H
#define HCI_MANAGER_H

#include <memory>
#include <string>

#include "nfa_hci_api.h"

namespace OHOS {
namespace nfc {
namespace ncibal {
static const auto APP_NAME = "NfcNci";
static const int HCI_TRANSACTION_DATA_LENGTH_MIN = 3;
static const int HCI_TRANSACTION_AID_LENGTH_POS = 1;
static const int HCI_TRANSACTION_AID_POS = 2;
static const int HCI_TRANSACTION_AID_BERTLV_HEAD_LENGTH = 3;
static const int BERTLV_LENGTH_LIMITED = 0x80;
static const int BERTLV_LENGTH_ONE = 0x81;
static const int BERTLV_LENGTH_TWO = 0x82;
static const int BERTLV_LENGTH_THREE = 0x83;
static const int BERTLV_LENGTH_FOUR = 0x84;

class INfcNci;
class HciManager final {
public:
    static HciManager& GetInstance();
    void Initialize();
    void SetNfcNciImpl(std::shared_ptr<INfcNci> nfcNciImpl);
    static uint8_t GetConnectedNfceeId(uint8_t nfceeId);

private:
    HciManager();
    ~HciManager();
    static void NfcHciCallback(tNFA_HCI_EVT event, tNFA_HCI_EVT_DATA* eventData);
    static std::string GetDataFromBerTlv(std::string& berTlv);

    static uint8_t mEsePipe_;
    static uint8_t mSimPipe_;
    static std::shared_ptr<INfcNci> mNfcNciImpl_;
};
}  // namespace ncibal
}  // namespace nfc
}  // namespace OHOS
#endif /* HCI_MANAGER_H */