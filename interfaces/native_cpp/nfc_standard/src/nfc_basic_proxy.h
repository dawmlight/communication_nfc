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
#ifndef NFC_BASIC_PROXY_H
#define NFC_BASIC_PROXY_H

#include <memory>
#include <vector>

#include "iremote_object.h"

namespace OHOS {
class MessageParcel;
class MessageOption;

namespace nfc {
class NfcBasicProxy {
public:
    explicit NfcBasicProxy(const OHOS::sptr<OHOS::IRemoteObject>& obj) : remoteObj_(obj) {}
    virtual ~NfcBasicProxy() {}

    int ProcessIntRes(int cmd, OHOS::MessageParcel& data, OHOS::MessageOption& option, int& result);
    int ProcessBoolRes(int cmd, OHOS::MessageParcel& data, OHOS::MessageOption& option, bool& result);
    int ProcessCommand(int cmd, OHOS::MessageParcel& data, OHOS::MessageOption& option);

private:
    OHOS::sptr<OHOS::IRemoteObject> remoteObj_{};
};
}  // namespace nfc
}  // namespace OHOS
#endif  // !NFC_BASIC_PROXY_H