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
#ifndef SECURE_ELEMENT_CHANNEL_PROXY_H
#define SECURE_ELEMENT_CHANNEL_PROXY_H

#include <memory>

#include "errors.h"
#include "iremote_proxy.h"
#include "isecure_element_channel.h"

namespace OHOS {
namespace se {
namespace omapi {
class SecureElemntChannelProxy final : public OHOS::IRemoteProxy<ISecureElementChannel> {
public:
    explicit SecureElemntChannelProxy(const OHOS::sptr<OHOS::IRemoteObject>& remote)
        : OHOS::IRemoteProxy<ISecureElementChannel>(remote)
    {
    }
    ~SecureElemntChannelProxy() {}

    void Close() override;
    bool IsClosed() override;
    bool IsBasicChannel() override;
    std::string GetSelectResponse() override;
    std::string Transmit(const std::string& command) override;
    bool SelectNext() override;

private:
    static constexpr int COMMAND_CHANNEL_START_ID = 400;
    static constexpr int COMMAND_CHANNEL_CLOSE = COMMAND_CHANNEL_START_ID + 0;
    static constexpr int COMMAND_CHANNEL_ISCLOSED = COMMAND_CHANNEL_START_ID + 1;
    static constexpr int COMMAND_CHANNEL_ISBASICCHANNEL = COMMAND_CHANNEL_START_ID + 2;
    static constexpr int COMMAND_CHANNEL_GET_SELECT_RESPONSE = COMMAND_CHANNEL_START_ID + 3;
    static constexpr int COMMAND_CHANNEL_TRANSMIT = COMMAND_CHANNEL_START_ID + 4;
    static constexpr int COMMAND_CHANNEL_SELECT_NEXT = COMMAND_CHANNEL_START_ID + 5;
};
}  // namespace omapi
}  // namespace se
}  // namespace OHOS
#endif  // !SECURE_ELEMENT_CHANNEL_PROXY_H