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
#include "app_callback_stub.h"

#include "loghelper.h"
#include "message_parcel.h"
#include "tag.h"

using namespace std;

namespace OHOS {
namespace nfc {
namespace sdk {
int AppCallbackStub::OnRemoteRequest(uint32_t code,         /* [in] */
                                     MessageParcel& data,   /* [in] */
                                     MessageParcel& reply,  /* [out] */
                                     MessageOption& option) /* [in] */
{
    DebugLog("OnRemoteRequest occur, code is %d", code);
    switch (code) {
        case COMMAND_TAG_DISCOVERED:
            return HandleTagDiscovered(data, reply);
        default:
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
}
int AppCallbackStub::HandleTagDiscovered(MessageParcel& data, MessageParcel& reply)
{
    DebugLog("AppCallbackStub::HandleTagDiscovered in");
    Tag* tagtmp = data.ReadParcelable<Tag>();
    InfoLog("AppCallbackStub::HandleTagDiscovered tagtmp tech len.%d", tagtmp->GetTagTechnologyList().size());
    std::unique_ptr<Tag> tag = std::unique_ptr<Tag>(tagtmp);
    InfoLog("AppCallbackStub::HandleTagDiscovered tech len.%d", tag->GetTagTechnologyList().size());

    OnTagDiscovered(std::move(tag));
    return ERR_NONE;
}

int TagRemovedCallbackStub::OnRemoteRequest(uint32_t code,         /* [in] */
                                            MessageParcel& data,   /* [in] */
                                            MessageParcel& reply,  /* [out] */
                                            MessageOption& option) /* [in] */
{
    DebugLog("OnRemoteRequest occur, code is %d", code);
    switch (code) {
        case COMMAND_TAG_REMOVED:
            return HandleTagRemoved(data, reply);
        default:
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
}
int TagRemovedCallbackStub::HandleTagRemoved(MessageParcel& data, MessageParcel& reply)
{
    OnTagRemoved();
    return ERR_NONE;
}
}  // namespace sdk
}  // namespace nfc
}  // namespace OHOS