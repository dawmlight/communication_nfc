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
#ifndef AID_REF_DO_H
#define AID_REF_DO_H

#include <string>

#include "ber_tlv.h"

namespace OHOS::se::security {
/*
 * AID reference data objects
 * The AID-REF-DO shall be used to store and retrieve the corresponding access rules for an SE application
 * (which is identified by its AID) to and from the ARA.
 */
class AidRefDo : public BerTlv {
public:
    static const auto AID_REF_DO_TAG = 0x4F;
    static const auto AID_REF_DO_TAG_DEFAULT_APPLICATION = 0xC0;
    AidRefDo(int tag, const std::string& aid);
    ~AidRefDo();
    std::string GetAid();
    static std::shared_ptr<AidRefDo> BerTlvToAidRefDo(std::shared_ptr<BerTlv> berTlv);
    static std::shared_ptr<AidRefDo> AidToAidRefDo(const std::string& aid);

private:
    std::string aid_;
};
}  // namespace OHOS::se::security
#endif /* AID_REF_DO_H */
