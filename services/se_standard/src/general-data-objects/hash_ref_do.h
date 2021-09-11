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
#ifndef HASH_REF_DO_H
#define HASH_REF_DO_H

#include <string>

#include "ber_tlv.h"

namespace OHOS::se::security {
/*
 * Hash reference data objects
 * The Hash-REF-DO shall be used to store and retrieve the corresponding access rules for a device
 * application (which is identified by the hash value of its certificate) to and from the ARA
 */
class HashRefDo : public BerTlv {
public:
    static const auto HASH_REF_DO_TAG = 0xC1;
    static const auto SHA1_LEN = 20;
    static const auto SHA256_LEN = 32;
    HashRefDo();
    HashRefDo(const std::string& hash);
    ~HashRefDo();
    std::string GetHash();
    static std::shared_ptr<HashRefDo> BerTlvToHashRefDo(std::shared_ptr<BerTlv> berTlv);

private:
    std::string hash_;
};
}  // namespace OHOS::se::security
#endif /* HASH_REF_DO_H */
