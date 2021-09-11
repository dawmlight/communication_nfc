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
#include "access_rule_files_channel.h"

#include "channel_access_rule.h"
#include "loghelper.h"
#include "se_channel.h"
#include "se_common_exception.h"

namespace OHOS::se::security {
AccessRuleFilesChannel::AccessRuleFilesChannel() : channel_(std::shared_ptr<SeChannel>()) {}

AccessRuleFilesChannel::~AccessRuleFilesChannel() {}

void AccessRuleFilesChannel::SetChannel(std::shared_ptr<OHOS::se::SeChannel> channel)
{
    channel_ = channel;
}

std::string AccessRuleFilesChannel::Transmit(std::string cmd)
{
    if (channel_) {
        return channel_->Transmit(cmd);
    }
    throw SecureElementError("Channel has expired");
}

void AccessRuleFilesChannel::Close()
{
    if (channel_) {
        channel_->Close();
    }
}
}  // namespace OHOS::se::security