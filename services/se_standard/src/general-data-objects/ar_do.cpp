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
#include "ar_do.h"

#include "apdu_ar_do.h"
#include "loghelper.h"
#include "nfc_ar_do.h"
#include "se_common_exception.h"

namespace OHOS::se::security {
ArDo::ArDo(std::shared_ptr<BerTlv> berTlv, std::shared_ptr<ApduArDo> apduArDo, std::shared_ptr<NfcArDo> nfcArDo)
    : BerTlv(berTlv->GetTag(), berTlv->GetLength(), berTlv->GetValue()), apduArDo_(apduArDo), nfcArDo_(nfcArDo)
{
}

ArDo::~ArDo() {}

std::shared_ptr<ApduArDo> ArDo::GetApduArDo()
{
    return apduArDo_;
}

std::shared_ptr<NfcArDo> ArDo::GetNfcArDo()
{
    return nfcArDo_;
}

std::shared_ptr<ArDo> ArDo::BerTlvToArDo(std::shared_ptr<BerTlv> berTlv)
{
    int tag = berTlv->GetTag();
    int length = berTlv->GetLength();
    std::string value = berTlv->GetValue();
    if (tag != AR_DO_TAG) {
        ErrorLog("Invalid tag in ArDo");
        throw AccessControlError("Parsing data error");
    }
    if (length > (int)value.length()) {
        ErrorLog("Value length not enough in ArDo");
        throw AccessControlError("Parsing data error");
    }

    std::shared_ptr<BerTlv> tlv = BerTlv::StrToBerTlv(value);
    std::shared_ptr<ApduArDo> apduArDo;
    std::shared_ptr<NfcArDo> nfcArDo;
    apduArDo = ApduArDo::BerTlvToApduArDo(tlv);
    if (apduArDo) {
        std::string tlvStr = value.substr((apduArDo->GetData()).length());
        if (tlvStr.empty()) {
            tlv = std::shared_ptr<BerTlv>();
        } else {
            tlv = BerTlv::StrToBerTlv(tlvStr);
        }
    }
    if (tlv) {
        nfcArDo = NfcArDo::BerTlvToNfcArDo(tlv);
    }
    // if (nfcArDo) {
    //	std::string tlvStr = value.substr(nfcArDo->GetTotalLength());
    //	tlv = BerTlv::StrToBerTlv(tlvStr);
    //}

    if (apduArDo || nfcArDo) {
        return std::make_shared<ArDo>(berTlv, apduArDo, nfcArDo);
    } else {
        ErrorLog("No valid data in ar-do");
        throw AccessControlError("Parsing data error");
    }
}
}  // namespace OHOS::se::security