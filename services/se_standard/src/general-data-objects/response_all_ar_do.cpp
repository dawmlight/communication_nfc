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
#include "response_all_ar_do.h"

#include "loghelper.h"
#include "ref_ar_do.h"
#include "se_common_exception.h"

namespace OHOS::se::security {
ResponseAllArDo::ResponseAllArDo(std::shared_ptr<BerTlv> berTlv, std::vector<RefArDo> refArDoArray)
    : BerTlv(berTlv->GetTag(), berTlv->GetLength(), berTlv->GetValue()), refArDoArray_(refArDoArray)
{
}

ResponseAllArDo::~ResponseAllArDo() {}

std::vector<RefArDo> ResponseAllArDo::GetRefArDoArray()
{
    return refArDoArray_;
}

std::shared_ptr<ResponseAllArDo> ResponseAllArDo::BerTlvToResponseAllArDo(std::shared_ptr<BerTlv> berTlv)
{
    int tag = berTlv->GetTag();
    int length = berTlv->GetLength();
    std::string value = berTlv->GetValue();
    if (tag != RESPONSE_ALL_AR_DO_TAG) {
        ErrorLog("Invalid tag in ResponseAllArDo");
        throw AccessControlError("Parsing data error");
    }
    if (length > (int)value.length()) {
        ErrorLog("Value length not enough in ResponseAllArDo");
        throw AccessControlError("Parsing data error");
    }

    std::vector<RefArDo> refArDoArray;
    int remainLength = length;
    std::string remainValue = value;
    while (remainLength > 0) {
        std::shared_ptr<BerTlv> bt = BerTlv::StrToBerTlv(remainValue);
        std::shared_ptr<RefArDo> rad = RefArDo::BerTlvToRefArDo(bt);
        if (rad) {
            refArDoArray.push_back(*rad);
        }
        remainValue = remainValue.substr((bt->GetData()).length());
        remainLength -= (bt->GetData()).length();
    }
    return std::make_shared<ResponseAllArDo>(berTlv, refArDoArray);
}
}  // namespace OHOS::se::security