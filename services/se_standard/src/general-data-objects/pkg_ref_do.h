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
#ifndef PKG_REF_DO_H
#define PKG_REF_DO_H

#include <string>

#include "ber_tlv.h"

namespace OHOS::se::security {
/*
 * Package reference data object
 * PKG-REF-DO (CA) is the full package name string defined in manifest, ASCII encoded, max length 127 bytes.
 */
class PkgRefDo : public BerTlv {
public:
    static const auto PKG_REF_DO_TAG = 0xCA;
    static const auto PKG_MAX_LEN = 128;
    PkgRefDo();
    PkgRefDo(const std::string& name);
    ~PkgRefDo();
    std::string GetName();
    static std::shared_ptr<PkgRefDo> BerTlvToPkgRefDo(std::shared_ptr<BerTlv> berTlv);

private:
    std::string name_;
};
}  // namespace OHOS::se::security
#endif /* PKG_REF_DO_H */
