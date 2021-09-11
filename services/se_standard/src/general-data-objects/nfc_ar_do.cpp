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
#include "nfc_ar_do.h"

#include "loghelper.h"
#include "se_common_exception.h"

namespace OHOS::se::security {
NfcArDo::NfcArDo(bool nfcAllowed)
    : BerTlv(NFC_AR_DO_TAG, 1, std::string(1, (char)(nfcAllowed ? NFC_ALWAYS : NFC_NEVER))), nfcAllowed_(nfcAllowed)
{
}

NfcArDo::~NfcArDo() {}

bool NfcArDo::IsNfcAllowed()
{
    return nfcAllowed_;
}

std::shared_ptr<NfcArDo> NfcArDo::BerTlvToNfcArDo(std::shared_ptr<BerTlv> berTlv)
{
    int tag = berTlv->GetTag();
    int length = berTlv->GetLength();
    std::string value = berTlv->GetValue();
    if (tag != NFC_AR_DO_TAG) {
        DebugLog("Invalid tag in NfcArDo");
        return std::shared_ptr<NfcArDo
        >();
    }
    if (length != 1) {
        ErrorLog("Invalid length in NfcArDo");
        throw AccessControlError("Parsing data error");
    }
    if (length > (int)value.length()) {
        ErrorLog("Value length not enough in NfcArDo");
        throw AccessControlError("Parsing data error");
    }
    bool nfcAllowed = (value[0] == NFC_ALWAYS);
    return std::make_shared<NfcArDo>(nfcAllowed);
}
}  // namespace OHOS::se::security