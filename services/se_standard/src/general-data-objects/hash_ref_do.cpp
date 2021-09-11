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
#include "hash_ref_do.h"

#include "loghelper.h"
#include "se_common_exception.h"

namespace OHOS::se::security {
HashRefDo::HashRefDo() : BerTlv(HASH_REF_DO_TAG, 0, ""), hash_("") {}

HashRefDo::HashRefDo(const std::string& hash) : BerTlv(HASH_REF_DO_TAG, hash.length(), hash), hash_(hash) {}

HashRefDo::~HashRefDo() {}

std::string HashRefDo::GetHash()
{
    return hash_;
}

std::shared_ptr<HashRefDo> HashRefDo::BerTlvToHashRefDo(std::shared_ptr<BerTlv> berTlv)
{
    int tag = berTlv->GetTag();
    int length = berTlv->GetLength();
    std::string value = berTlv->GetValue();
    if (tag != HASH_REF_DO_TAG) {
        ErrorLog("Invalid tag in HashRefDo");
        throw AccessControlError("Parsing data error");
    }
    if (length != 0 && length != SHA1_LEN && length != SHA256_LEN) {
        ErrorLog("Invalid length in HashRefDo");
        throw AccessControlError("Parsing data error");
    }
    if (length > (int)value.length()) {
        ErrorLog("Value length not enough in HashRefDo");
        throw AccessControlError("Parsing data error");
    }
    return std::make_shared<HashRefDo>(value.substr(0, length));
}
}  // namespace OHOS::se::security