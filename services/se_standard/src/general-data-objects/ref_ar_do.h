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
#ifndef REF_AR_DO_H
#define REF_AR_DO_H

#include <memory>

#include "ber_tlv.h"

namespace OHOS::se::security {
class RefDo;
class ArDo;
/*
 * Reference access rule data object
 * The REF-AR-DO contains an access rule data object and its corresponding references for the SE application
 * (AID reference) and device application (hash reference)
 */
class RefArDo : public BerTlv {
public:
    static const auto REF_AR_DO_TAG = 0xE2;
    RefArDo(std::shared_ptr<BerTlv> berTlv, std::shared_ptr<RefDo> refDo, std::shared_ptr<ArDo> arDo);
    ~RefArDo();
    std::shared_ptr<RefDo> GetRefDo();
    std::shared_ptr<ArDo> GetArDo();
    static std::shared_ptr<RefArDo> BerTlvToRefArDo(std::shared_ptr<BerTlv> berTlv);

private:
    std::shared_ptr<RefDo> refDo_;
    std::shared_ptr<ArDo> arDo_;
};
}  // namespace OHOS::se::security
#endif /* REF_AR_DO_H */
