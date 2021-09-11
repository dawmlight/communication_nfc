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
#include "se_callback_stub.h"

#include "hilog/log.h"

namespace OHOS {
namespace hardware {
namespace se {
namespace v1_0 {
int SeCallbackStub::OnRemoteRequest(uint32_t code,         /* [in] */
                                    MessageParcel& data,   /* [in] */
                                    MessageParcel& reply,  /* [out] */
                                    MessageOption& option) /* [in] */
{
    HILOG_DEBUG(LOG_CORE, "OnRemoteRequest occur, code is %d", code);
    switch (code) {
        case COMMAND_STATE_CHANGE:
            return HandleOnStateChange(data, reply);
        default:
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
}

int SeCallbackStub::HandleOnStateChange(OHOS::MessageParcel& data, OHOS::MessageParcel& reply)
{
    bool state = data.ReadBool();
    std::string reason = data.ReadString();
    OnStateChange(state, reason);
    return ERR_NONE;
}
}  // namespace v1_0
}  // namespace se
}  // namespace hardware
}  // namespace OHOS