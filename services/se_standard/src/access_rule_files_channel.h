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
#ifndef ACCESS_RULE_FILES_CHANNEL_H
#define ACCESS_RULE_FILES_CHANNEL_H

#include <memory>
#include <string>

namespace OHOS::se {
class SeChannel;
}  // namespace OHOS::se

namespace OHOS::se::security {
class AccessRuleFilesChannel {
public:
    AccessRuleFilesChannel();
    ~AccessRuleFilesChannel();
    void SetChannel(std::shared_ptr<OHOS::se::SeChannel> channel);
    std::string Transmit(std::string cmd);
    void Close();

private:
    std::shared_ptr<OHOS::se::SeChannel> channel_;
};
}  // namespace OHOS::se::security
#endif /* ACCESS_RULE_FILES_CHANNEL_H */