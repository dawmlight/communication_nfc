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
#ifndef REF_DO_H
#define REF_DO_H

#include <memory>

#include "ber_tlv.h"

namespace OHOS::se::security {
class AidRefDo;
class HashRefDo;
class PkgRefDo;
/*
 * Reference data object
 * The REF-DO contains an AID-REF-DO and a Hash-REF-DO which uniquely reference a specific set of
 * access rules assigned for a given SE application (which is identified by its AID) and a device application
 * (which is identified by the hash value of its certificate)
 */
class RefDo : public BerTlv {
public:
    static const auto REF_DO_TAG = 0xE1;
    RefDo(std::shared_ptr<AidRefDo> aidRefDo, std::shared_ptr<HashRefDo> hashRefDo);
    RefDo(std::shared_ptr<AidRefDo> aidRefDo, std::shared_ptr<HashRefDo> hashRefDo, std::shared_ptr<PkgRefDo> pkgRefDo);
    ~RefDo();
    bool operator<(const RefDo& refDo) const;
    std::shared_ptr<AidRefDo> GetAidRefDo();
    std::shared_ptr<HashRefDo> GetHashRefDo();
    std::shared_ptr<PkgRefDo> GetPkgRefDo();
    static std::shared_ptr<RefDo> BerTlvToRefDo(std::shared_ptr<BerTlv> berTlv);

private:
    std::shared_ptr<AidRefDo> aidRefDo_;
    std::shared_ptr<HashRefDo> hashRefDo_;
    std::shared_ptr<PkgRefDo> pkgRefDo_;
};
}  // namespace OHOS::se::security
#endif /* REF_DO_H */
