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
#include "ref_ar_do.h"

#include "ar_do.h"
#include "loghelper.h"
#include "ref_do.h"
#include "se_common_exception.h"

namespace OHOS::se::security {
RefArDo::RefArDo(std::shared_ptr<BerTlv> berTlv, std::shared_ptr<RefDo> refDo, std::shared_ptr<ArDo> arDo)
    : BerTlv(berTlv->GetTag(), berTlv->GetLength(), berTlv->GetValue()), refDo_(refDo), arDo_(arDo)
{
}

RefArDo::~RefArDo() {}

std::shared_ptr<RefDo> RefArDo::GetRefDo()
{
    return refDo_;
}

std::shared_ptr<ArDo> RefArDo::GetArDo()
{
    return arDo_;
}

std::shared_ptr<RefArDo> RefArDo::BerTlvToRefArDo(std::shared_ptr<BerTlv> berTlv)
{
    int tag = berTlv->GetTag();
    int length = berTlv->GetLength();
    std::string value = berTlv->GetValue();
    if (tag != REF_AR_DO_TAG) {
        ErrorLog("Invalid tag in RefArDo");
        throw AccessControlError("Parsing data error");
    }
    if (length > (int)value.length()) {
        ErrorLog("Value length not enough in RefArDo");
        throw AccessControlError("Parsing data error");
    }

    std::shared_ptr<BerTlv> refTlv = BerTlv::StrToBerTlv(value);
    std::shared_ptr<RefDo> refDo = RefDo::BerTlvToRefDo(refTlv);

    std::string arTlv = value.substr((refDo->GetData()).length());
    std::shared_ptr<BerTlv> arData = BerTlv::StrToBerTlv(arTlv);
    std::shared_ptr<ArDo> arDo = ArDo::BerTlvToArDo(arData);

    return std::make_shared<RefArDo>(berTlv, refDo, arDo);
}
}  // namespace OHOS::se::security