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
#ifndef DODF_H
#define DODF_H

#include <memory>
#include <string>
#include <vector>

namespace OHOS::se::security {
class AccessRuleFilesChannel;
/*
 * Data Object Directory File, reference Acceess Rule Main File
 */
static const std::string AC_OID = "1.2.840.114283.200.1.1";
class DODF {
public:
    explicit DODF(std::weak_ptr<AccessRuleFilesChannel> arfChannel);
    ~DODF();
    std::string ReadDataFromPath(std::string path);
    std::string AnalyzeData(std::string data);

private:
    std::weak_ptr<AccessRuleFilesChannel> arfChannel_;
};
}  // namespace OHOS::se::security
#endif  // !DODF_H
