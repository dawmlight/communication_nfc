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

#ifndef AR_DO_H
#define AR_DO_H

#include <memory>

#include "ber_tlv.h"

namespace OHOS::se::security {
class ApduArDo;
class NfcArDo;
/*
 * Access rule data object
 * The AR-DO contains one or two access rules of type APDU or NFC
 */
class ArDo : public BerTlv {
public:
    static const auto AR_DO_TAG = 0xE3;
    ArDo(std::shared_ptr<BerTlv> berTlv, std::shared_ptr<ApduArDo> apduArDo, std::shared_ptr<NfcArDo> nfcArDo);
    ~ArDo();
    std::shared_ptr<ApduArDo> GetApduArDo();
    std::shared_ptr<NfcArDo> GetNfcArDo();
    static std::shared_ptr<ArDo> BerTlvToArDo(std::shared_ptr<BerTlv> berTlv);

private:
    std::shared_ptr<ApduArDo> apduArDo_;
    std::shared_ptr<NfcArDo> nfcArDo_;
};
}  // namespace OHOS::se::security
#endif /* AR_DO_H */
