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
#ifndef DIR_H
#define DIR_H

#include <memory>
#include <string>
#include <vector>

namespace OHOS::se::security {
class AccessRuleFilesChannel;
/*
 * Application Directory, reference DF PKCS-15
 */
static const std::string DIR_PATH = {(char)0x3F, 0x00, 0x2F, 0x00};
class DIR {
public:
    explicit DIR(std::weak_ptr<AccessRuleFilesChannel> arfChannel);
    ~DIR();
    std::string ReadDataFromAid(std::string aid);
    std::string AnalyzeData(std::string data, std::string aid);

private:
    std::weak_ptr<AccessRuleFilesChannel> arfChannel_;
};
}  // namespace OHOS::se::security
#endif  // !DIR_H
