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

#ifndef APDU_AR_DO_H
#define APDU_AR_DO_H

#include <vector>

#include "ber_tlv.h"

namespace OHOS::se::security {
/*
 * APDU access rule data object
 * An APDU access rule data object defines an access rule for APDU access. The APDU access can either be
 * restricted by a general rule based on an “access is NEVER/ALWAYS allowed” policy or by a specific rule
 * based on APDU filters which defines the range of allowed APDUs more precisely.
 */
class ApduArDo : public BerTlv {
public:
    static const auto APDU_AR_DO_TAG = 0xD0;
    static const auto APDU_ALWAYS = 0x01;
    static const auto APDU_NEVER = 0x00;
    ApduArDo(std::shared_ptr<BerTlv> berTlv, bool apduAllowed, std::vector<std::string> apduFilters);
    ~ApduArDo();
    bool IsApduAllowed();
    std::vector<std::string> GetApduFilters();
    static std::shared_ptr<ApduArDo> BerTlvToApduArDo(std::shared_ptr<BerTlv> berTlv);

private:
    bool apduAllowed_;
    std::vector<std::string> apduFilters_;
};
}  // namespace OHOS::se::security
#endif /* APDU_AR_DO_H */
