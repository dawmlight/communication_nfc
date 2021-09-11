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
#ifndef RESPONSE_REFRESHTAG_DO_H
#define RESPONSE_REFRESHTAG_DO_H

#include "ber_tlv.h"

namespace OHOS::se::security {
/*
 * The Response-RefreshTag-DO is mandatory for a GET DATA [Refresh tag] request.
 * The GET DATA [Refresh tag] command shall return a Response-RefreshTag-DO containing a refresh tag
 * that indicates whether changes have occurred in the access control data.
 */
class ResponseRefreshTagDo : public BerTlv {
public:
    static const int RESPONSE_REFRESH_TAG_DO_TAG = 0xDF20;
    ResponseRefreshTagDo(std::shared_ptr<BerTlv> berTlv, const std::string& freshTag);
    ~ResponseRefreshTagDo();
    std::string GetRefreshTag();
    static std::shared_ptr<ResponseRefreshTagDo> BerTlvToResponseRefreshTagDo(std::shared_ptr<BerTlv> berTlv);

private:
    std::string refreshTag_;
};
}  // namespace OHOS::se::security
#endif /* RESPONSE_REFRESHTAG_DO_H */
