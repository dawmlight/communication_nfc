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
#ifndef ODF_H
#define ODF_H

#include <memory>
#include <string>
#include <vector>

namespace OHOS::se::security {
class AccessRuleFilesChannel;
/*
 *  Reference DODF
 */
static const std::string ODF_PATH = {(char)0x50, (char)0x31};
class ODF {
public:
    explicit ODF(std::weak_ptr<AccessRuleFilesChannel> arfChannel);
    ~ODF();
    std::string ReadDataFromPath(std::string path);
    std::string AnalyzeData(std::string data);

private:
    std::weak_ptr<AccessRuleFilesChannel> arfChannel_;
    std::string cdfPath_;
};
}  // namespace OHOS::se::security
#endif  // !ODF_H
