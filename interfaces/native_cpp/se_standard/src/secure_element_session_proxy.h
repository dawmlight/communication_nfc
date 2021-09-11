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
#ifndef SECURE_ELEMENT_SESSION_PROXY_H
#define SECURE_ELEMENT_SESSION_PROXY_H

#include <memory>

#include "errors.h"
#include "iremote_proxy.h"
#include "isecure_element_session.h"

namespace OHOS {
namespace se {
namespace omapi {
class SecureElemntSessionProxy final : public OHOS::IRemoteProxy<ISecureElementSession> {
public:
    explicit SecureElemntSessionProxy(const OHOS::sptr<OHOS::IRemoteObject>& remote)
        : OHOS::IRemoteProxy<ISecureElementSession>(remote)
    {
    }
    ~SecureElemntSessionProxy() {}

    std::string GetATR() override;
    void Close() override;
    bool IsClosed() override;
    void CloseChannels() override;
    sptr<ISecureElementChannel> OpenBasicChannel(const std::string& aid, int p2) override;
    sptr<ISecureElementChannel> OpenLogicalChannel(const std::string& aid, int p2) override;

private:
    static constexpr int COMMAND_SESSION_START_ID = 300;
    static constexpr int COMMAND_SESSION_CLOSE = COMMAND_SESSION_START_ID + 0;
    static constexpr int COMMAND_SESSION_ISCLOSED = COMMAND_SESSION_START_ID + 1;
    static constexpr int COMMAND_SESSION_GETATR = COMMAND_SESSION_START_ID + 2;
    static constexpr int COMMAND_SESSION_CLOSE_CHANNELS = COMMAND_SESSION_START_ID + 3;
    static constexpr int COMMAND_SESSION_OPEN_BASIC_CHANNEL = COMMAND_SESSION_START_ID + 4;
    static constexpr int COMMAND_SESSION_OPEN_LOGIC_CHANNEL = COMMAND_SESSION_START_ID + 5;
};
}  // namespace omapi
}  // namespace se
}  // namespace OHOS
#endif  // !SECURE_ELEMENT_SESSION_PROXY_H