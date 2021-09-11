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
#include "apdu_ar_do.h"

#include "loghelper.h"
#include "se_common_exception.h"

namespace OHOS::se::security {
ApduArDo::ApduArDo(std::shared_ptr<BerTlv> berTlv, bool apduAllowed, std::vector<std::string> apduFilters)
    : BerTlv(berTlv->GetTag(), berTlv->GetLength(), berTlv->GetValue()),
      apduAllowed_(apduAllowed),
      apduFilters_(apduFilters)
{
}

ApduArDo::~ApduArDo() {}

bool ApduArDo::IsApduAllowed()
{
    return apduAllowed_;
}

std::vector<std::string> ApduArDo::GetApduFilters()
{
    return apduFilters_;
}

std::shared_ptr<ApduArDo> ApduArDo::BerTlvToApduArDo(std::shared_ptr<BerTlv> berTlv)
{
    int tag = berTlv->GetTag();
    int length = berTlv->GetLength();
    std::string value = berTlv->GetValue();
    if (tag != APDU_AR_DO_TAG) {
        DebugLog("Invalid tag in ApduArDo");
        return std::shared_ptr<ApduArDo>();
    }
    if (length > (int)value.length()) {
        ErrorLog("Value length not enough in ApduArDo");
        throw AccessControlError("Parsing data error");
    }

    bool apduAllowed = false;
    std::vector<std::string> apduFilters;
    if (length == 1) {
        if (APDU_ALWAYS == value[0]) {
            apduAllowed = true;
        } else if (APDU_NEVER == value[0]) {
            apduAllowed = false;
        } else {
            ErrorLog("Invalid value in ApduArDo");
            throw AccessControlError("Parsing data error");
        }
    } else if (length % 8 == 0 && length != 0) {
        apduAllowed = true;
        for (size_t i = 0; i < value.length(); i += 8) {
            apduFilters.push_back(value.substr(i, 8));
        }
    } else {
        ErrorLog("Invalid length in ApduArDo");
        throw AccessControlError("Parsing data error");
    }
    return std::make_shared<ApduArDo>(berTlv, apduAllowed, apduFilters);
}
}  // namespace OHOS::se::security