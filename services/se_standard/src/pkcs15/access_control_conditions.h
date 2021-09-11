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
#ifndef ACCESS_CONTROL_CONDITIONS_H
#define ACCESS_CONTROL_CONDITIONS_H

#include <memory>
#include <string>
#include <vector>

namespace OHOS::se::security {
class AccessRuleCache;
class AidRefDo;
class AccessRuleFilesChannel;
/*
 * The conditions are expressed as a list of entries, each entry containing a SHA-1 hash of a certificate
 * identifying an authorized application issuer.
 * If this file is empty, then any Rule object pointing to this file is denying all device applications access to the
 * Secure Element applications pointed to by the Rule object.
 * If this file contains a condition without a certificate hash, then any Rule object pointing to this file is granting
 * any device application access to the Secure Element applications pointed to by the Rule object.
 */
class AccessControlConditions {
public:
    AccessControlConditions(std::weak_ptr<AccessRuleCache> accessRuleCache, std::shared_ptr<AidRefDo> aidRefDo);
    ~AccessControlConditions();
    void ReadDataFromPath(std::weak_ptr<AccessRuleFilesChannel> arfChannel, std::string path);
    void ReadData(std::string data);
    std::string GetData();
    void AnalyzeData(std::string data);

private:
    std::string data_;
    std::weak_ptr<AccessRuleCache> accessRuleCache_;
    std::shared_ptr<AidRefDo> aidRefDo_;
};
}  // namespace OHOS::se::security
#endif  // !ACCESS_CONTROL_CONDITIONS_H
