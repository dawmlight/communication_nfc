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
#ifndef NFC_AR_DO_H
#define NFC_AR_DO_H

#include <memory>

#include "ber_tlv.h"

namespace OHOS::se::security {
/*
 * Nfc access rule data object
 * An NFC event data object defines an access rule for generating NFC events for a specific device application.
 * The NFC event access can be restricted by a rule based on an “event access is NEVER/ ALWAYS allowed”
 * policy
 */
class NfcArDo : public BerTlv {
public:
    static const auto NFC_AR_DO_TAG = 0xD1;
    static const auto NFC_ALWAYS = 0x01;
    static const auto NFC_NEVER = 0x00;
    NfcArDo(bool nfcAllowed);
    ~NfcArDo();
    bool IsNfcAllowed();
    static std::shared_ptr<NfcArDo> BerTlvToNfcArDo(std::shared_ptr<BerTlv> berTlv);

private:
    bool nfcAllowed_;
};
}  // namespace OHOS::se::security
#endif /* NFC_AR_DO_H */
