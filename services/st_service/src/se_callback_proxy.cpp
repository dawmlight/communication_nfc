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
#include "se_callback_proxy.h"

#include "hilog/log.h"
#include "message_option.h"
#include "message_parcel.h"

namespace OHOS {
namespace hardware {
namespace se {
namespace v1_0 {
void SeCallbackProxy::OnStateChange(bool state, std::string reason)
{
    MessageParcel data, reply;
    MessageOption option(MessageOption::TF_ASYNC);
    data.WriteBool(state);
    data.WriteString(reason);
    int res = Remote()->SendRequest(COMMAND_STATE_CHANGE, data, reply, option);
    if (res != ERR_NONE) {
        HILOG_INFO(LOG_CORE,
                   "[SeCallbackProxy::OnStateChange] send request(%d) with Res(%d) fail.",
                   COMMAND_STATE_CHANGE,
                   res);
    }
}
}  // namespace v1_0
}  // namespace se
}  // namespace hardware
}  // namespace OHOS