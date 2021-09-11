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
#include "nfc_basic_proxy.h"

#include "loghelper.h"
#include "message_option.h"
#include "message_parcel.h"

using namespace std;

namespace OHOS {
namespace nfc {
int NfcBasicProxy::ProcessIntRes(int cmd, MessageParcel& data, MessageOption& option, int& result)
{
    MessageParcel reply;
    int res = remoteObj_->SendRequest(cmd, data, reply, option);
    if (res == ERR_NONE) {
        result = reply.ReadInt32();
        InfoLog("It is successful To send request(%d) with Res(%d).", cmd, res);
    } else {
        InfoLog("It is failed To send request(%d) with Res(%d).", cmd, res);
    }
    return res;
}

int NfcBasicProxy::ProcessBoolRes(int cmd, MessageParcel& data, MessageOption& option, bool& result)
{
    MessageParcel reply;
    int32_t res = remoteObj_->SendRequest(cmd, data, reply, option);
    if (res == ERR_NONE) {
        result = reply.ReadBool();
        InfoLog("It is successful To send request(%d) with Res(%d).", cmd, res);
    } else {
        InfoLog("It is failed To send request(%d) with Res(%d).", cmd, res);
    }
    return res;
}

int NfcBasicProxy::ProcessCommand(int cmd, MessageParcel& data, MessageOption& option)
{
    MessageParcel reply;
    return remoteObj_->SendRequest(cmd, data, reply, option);
}
}  // namespace nfc
}  // namespace OHOS
