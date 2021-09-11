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
#include "aid_ref_do.h"

#include "loghelper.h"
#include "se_common_exception.h"

namespace OHOS::se::security {
AidRefDo::AidRefDo(int tag, const std::string& aid) : BerTlv(tag, aid.length(), aid), aid_(aid) {}

AidRefDo::~AidRefDo() {}

std::string AidRefDo::GetAid()
{
    return aid_;
}

std::shared_ptr<AidRefDo> AidRefDo::BerTlvToAidRefDo(std::shared_ptr<BerTlv> berTlv)
{
    int tag = berTlv->GetTag();
    int length = berTlv->GetLength();
    std::string value = berTlv->GetValue();
    if (tag == AID_REF_DO_TAG_DEFAULT_APPLICATION) {
        if (length != 0) {
            ErrorLog("Invalid length in AidRefDo");
            throw AccessControlError("Parsing data error");
        }
        return std::make_shared<AidRefDo>(tag, value);
    } else if (tag == AID_REF_DO_TAG) {
        if (length != 0 && (length < 5 || length > 16)) {
            ErrorLog("Invalid length in AidRefDo");
            throw AccessControlError("Parsing data error");
        }
        if (length > (int)value.length()) {
            ErrorLog("Value length not enough in AidRefDo");
            throw AccessControlError("Parsing data error");
        }
        return std::make_shared<AidRefDo>(tag, value.substr(0, length));
    } else {
        ErrorLog("Invalid tag in AidRefDo");
        throw AccessControlError("Parsing data error");
    }
}

std::shared_ptr<AidRefDo> AidRefDo::AidToAidRefDo(const std::string& aid)
{
    int tag = AID_REF_DO_TAG;
    if (aid.empty()) {
        tag = AID_REF_DO_TAG_DEFAULT_APPLICATION;
    } else {
        tag = AID_REF_DO_TAG;
    }
    return std::make_shared<AidRefDo>(tag, aid);
}
}  // namespace OHOS::se::security