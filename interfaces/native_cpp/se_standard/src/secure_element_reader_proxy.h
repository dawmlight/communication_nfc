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
#ifndef SECURE_ELEMENT_READER_PROXY_H
#define SECURE_ELEMENT_READER_PROXY_H

#include <memory>

#include "errors.h"
#include "iremote_proxy.h"
#include "isecure_element_reader.h"

namespace OHOS {
namespace se {
namespace omapi {
class ISecureElementSession;
class SecureElemntReaderProxy final : public OHOS::IRemoteProxy<ISecureElementReader> {
public:
    explicit SecureElemntReaderProxy(const OHOS::sptr<OHOS::IRemoteObject>& remote)
        : OHOS::IRemoteProxy<ISecureElementReader>(remote)
    {
    }
    ~SecureElemntReaderProxy() {}

    bool IsSecureElementPresent() override;
    sptr<ISecureElementSession> OpenSession() override;
    void CloseSessions() override;
    bool Reset() override;

private:
    static constexpr int COMMAND_READER_START_ID = 200;
    static constexpr int COMMAND_READER_OPEN_SESSION = COMMAND_READER_START_ID + 0;
    static constexpr int COMMAND_READER_CLOSE_SESSIONS = COMMAND_READER_START_ID + 1;
    static constexpr int COMMAND_READER_RESET = COMMAND_READER_START_ID + 2;
    static constexpr int COMMAND_READER_IS_SE_PRESENT = COMMAND_READER_START_ID + 3;
};
}  // namespace omapi
}  // namespace se
}  // namespace OHOS
#endif  // !SECURE_ELEMENT_READER_PROXY_H