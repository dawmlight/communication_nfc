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
#include "response_refresh_tag_do.h"

#include "loghelper.h"
#include "se_common_exception.h"

namespace OHOS::se::security {
ResponseRefreshTagDo::ResponseRefreshTagDo(std::shared_ptr<BerTlv> berTlv, const std::string& refreshTag)
    : BerTlv(berTlv->GetTag(), berTlv->GetLength(), berTlv->GetValue()), refreshTag_(refreshTag)
{
}

ResponseRefreshTagDo::~ResponseRefreshTagDo() {}

std::string ResponseRefreshTagDo::GetRefreshTag()
{
    return refreshTag_;
}

std::shared_ptr<ResponseRefreshTagDo> ResponseRefreshTagDo::BerTlvToResponseRefreshTagDo(std::shared_ptr<BerTlv> berTlv)
{
    int tag = berTlv->GetTag();
    int length = berTlv->GetLength();
    std::string value = berTlv->GetValue();
    if (tag != RESPONSE_REFRESH_TAG_DO_TAG) {
        ErrorLog("Invalid tag in ResponseRefreshTagDo");
        throw AccessControlError("Parsing data error");
    }
    if (length > (int)value.length()) {
        ErrorLog("Value length not enough in ResponseRefreshTagDo");
        throw AccessControlError("Parsing data error");
    }

    std::string refreshTag = value.substr(0, length);
    return std::make_shared<ResponseRefreshTagDo>(berTlv, refreshTag);
}
}  // namespace OHOS::se::security