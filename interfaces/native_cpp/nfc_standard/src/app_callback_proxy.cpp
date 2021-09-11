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

#include "app_callback_proxy.h"

#include "loghelper.h"
#include "message_option.h"
#include "message_parcel.h"
#include "tag.h"

using namespace std;

namespace OHOS {
namespace nfc {
namespace sdk {
void AppCallbackProxy::OnTagDiscovered(std::unique_ptr<Tag> tag)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);
    Tag* pTag = tag.get();
    if (pTag) {
        InfoLog("AppCallbackProxy::OnTagDiscovered");
    } else {
        InfoLog("AppCallbackProxy::OnTagDiscovered: Tag Is Empty.");
    }
    data.WriteParcelable(tag.get());
    Remote()->SendRequest(COMMAND_TAG_DISCOVERED, data, reply, option);
}

void TagRemovedCallbackProxy::OnTagRemoved()
{
    MessageParcel data, reply;
    MessageOption option(MessageOption::TF_ASYNC);
    Remote()->SendRequest(COMMAND_TAG_REMOVED, data, reply, option);
}
}  // namespace sdk
}  // namespace nfc
}  // namespace OHOS