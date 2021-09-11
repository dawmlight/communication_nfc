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

#ifndef INSTALLED_CESERVICE_GETTER_H
#define INSTALLED_CESERVICE_GETTER_H
#include <map>
#include <string>

#include "card_emulation_service_info.h"
namespace OHOS::AppExecFwk {
class ElementName;
}
namespace OHOS::nfc::cardemulation {
std::unique_ptr<OHOS::AppExecFwk::ElementName> StringToElementName(const std::string& str);
std::string ElementNameToString(const OHOS::AppExecFwk::ElementName& elementName);
class InstalledCeServiceGetter {
public:
    virtual ~InstalledCeServiceGetter() = default;
    virtual std::vector<std::shared_ptr<CardEmulationServiceInfo>> GetInstalled(int userId) = 0;
    virtual std::vector<std::shared_ptr<CardEmulationServiceInfo>> GetDynamic(int userId) = 0;
    virtual OHOS::AppExecFwk::ElementName GetDefaultElementName(int userId) = 0;

    virtual void StoreDynamic(int userId, const std::vector<std::shared_ptr<CardEmulationServiceInfo>>&) = 0;
    virtual void StoreDefault(int userId, std::string abilityName) = 0;
};

#ifdef MOCK_FOR_TESTING
class CeServiceGetter : public InstalledCeServiceGetter {
public:
    CeServiceGetter();
    std::vector<std::shared_ptr<CardEmulationServiceInfo>> GetInstalled(int userId) override;
    std::vector<std::shared_ptr<CardEmulationServiceInfo>> GetDynamic(int userId) override;
    OHOS::AppExecFwk::ElementName GetDefaultElementName(int userId) override;
    void StoreDynamic(int userId, const std::vector<std::shared_ptr<CardEmulationServiceInfo>>& infos) override;
    void StoreDefault(int userId, std::string abilityName) override;

private:
    std::vector<std::shared_ptr<CardEmulationServiceInfo>> LoadFromFile(const std::string& path);

private:
    std::map<int, std::vector<std::shared_ptr<CardEmulationServiceInfo>>> infos_{};
    std::map<int, std::vector<std::shared_ptr<CardEmulationServiceInfo>>> dynamicInfos_{};
    std::map<int, OHOS::AppExecFwk::ElementName> defaultNames_{};
};

#endif
}  // namespace OHOS::nfc::cardemulation
#endif  // !INSTALLED_CESERVICE_GETTER_H
