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
#ifndef ACCESS_CONTROL_MAIN_H
#define ACCESS_CONTROL_MAIN_H

#include <memory>
#include <string>

namespace OHOS::se::security {
class AccessRuleCache;
class AccessRuleFilesChannel;
/*
 * The AccessControlMainFile object is related to the Secure Element that contains it
 * The AccessControlMainFile object contains the refresh tag and the path to the rules (i.e. the path to the
 * Access Control Rules File); additional fields may be added in future versions of this specification
 */
class AccessControlMain {
public:
    AccessControlMain(std::weak_ptr<AccessRuleFilesChannel> arfChannel,
                      std::weak_ptr<AccessRuleCache> accessRuleCache,
                      std::string path);
    ~AccessControlMain();
    std::string ReadDataFromPath();
    std::string AnalyzeData(std::string data);

private:
    std::weak_ptr<AccessRuleFilesChannel> arfChannel_;
    std::weak_ptr<AccessRuleCache> accessRuleCache_;
    std::string acmfPath_;
};
}  // namespace OHOS::se::security
#endif  // !ACCESS_CONTROL_MAIN_H
