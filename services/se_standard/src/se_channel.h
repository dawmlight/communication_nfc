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
#ifndef SE_SERVICE_CHANNEL_H
#define SE_SERVICE_CHANNEL_H

#include <memory>
#include <mutex>

#include "iremote_object.h"
#include "secure_element_channel.h"
#include "secure_element_session.h"

namespace OHOS::se::security {
class ChannelAccessRule;
}

using SESChannelAccess = OHOS::se::security::ChannelAccessRule;
namespace OHOS {
namespace se {
class Terminal;

class SeChannel final : public std::enable_shared_from_this<SeChannel> {
public:
    void Initialize(std::weak_ptr<SESChannelAccess> channelAccess);

    void Close();
    std::string Transmit(std::string command);
    std::weak_ptr<SESChannelAccess> GetChannelAccess();
    void SetChannelAccess(std::weak_ptr<SESChannelAccess> channelAccess);
    bool HasSelectedAid();
    int GetChannelNumber();
    std::string GetSelectResponse();
    bool IsBasicChannel();
    bool IsClosed();

private:
    explicit SeChannel(std::weak_ptr<Terminal> terminal,
                       int channelNumber,
                       const std::string& selectResponse,
                       const std::string& aid);

    bool SelectNext();
    void SetCallingPid(int pid);
    void CheckCommand(std::string command);
    char SetChannelToClassByte(char cla, int channelNumber);
    void SetUpRelationship(sptr<ISEChannel> iChannel, wptr<SecureElementSession> iSession);

private:
    int mChannelNumber_;
    std::mutex mLock_{};
    bool mIsClosed_{false};
    std::weak_ptr<Terminal> mTerminal_{};
    std::string mSelectResponse_{};
    std::shared_ptr<SESChannelAccess> mChannelAccess_{};
    sptr<ISEChannel> iChannel_{};
    wptr<SecureElementSession> seSession_{};
    int mCallingPid{0};
    std::string mAid_{};

    friend class SecureElementChannel;
    friend class SecureElementSession;
    friend class Terminal;
};
}  // namespace se
}  // namespace OHOS
#endif  // !SE_SERVICE_CHANNEL_H