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

#ifndef NFC_IEXECUTION_ENVIRONMENT_MANAGER_H
#define NFC_IEXECUTION_ENVIRONMENT_MANAGER_H

namespace OHOS::nfc::cardemulation {
class IExecutionEnvironmentManager {
public:
    virtual ~IExecutionEnvironmentManager() = default;

    virtual int GetDefaultRoute() = 0;
    virtual int GetDefaultOffHostRoute() = 0;
    virtual int LocationStringToInt(const std::string& loc) const = 0;
    virtual std::string ReviseSecureElementName(const std::string& loc) const = 0;
    virtual bool IsSurpported(const std::string& loc) const = 0;
};
}  // namespace OHOS::nfc::cardemulation
#endif  // !NFC_IEXECUTION_ENVIRONMENT_MANAGER_H
