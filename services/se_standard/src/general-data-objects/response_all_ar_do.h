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
#ifndef RESPONSE_ALL_AR_DO_H
#define RESPONSE_ALL_AR_DO_H

#include <vector>

#include "ber_tlv.h"

namespace OHOS::se::security {
class RefArDo;
/*
 * The Response-ALL-AR-DO is mandatory for a GET DATA [All] request.
 * In response to a GET DATA [All] command, the ARA-M shall return all access rules stored in the Secure
 * Element in the response message Data field within a Response-ALL-AR-DO.
 */
class ResponseAllArDo : public BerTlv {
public:
    static const int RESPONSE_ALL_AR_DO_TAG = 0xFF40;
    ResponseAllArDo(std::shared_ptr<BerTlv> berTlv, std::vector<RefArDo> refArDoArray);
    ~ResponseAllArDo();
    std::vector<RefArDo> GetRefArDoArray();
    static std::shared_ptr<ResponseAllArDo> BerTlvToResponseAllArDo(std::shared_ptr<BerTlv> berTlv);

private:
    std::vector<RefArDo> refArDoArray_;
};
}  // namespace OHOS::se::security
#endif /* RESPONSE_ALL_AR_DO_H */
