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
#ifndef _CARD_EMULATION_ABILITY_INFO_H_
#define _CARD_EMULATION_ABILITY_INFO_H_

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "nlohmann/json.hpp"

namespace OHOS::nfc::test {
struct AidSetData {
    std::string type_;
    std::string description_;
    std::vector<std::string> aids_;
};

struct CardEmulationAbilityInfo {
    std::string abilityName_;
    std::string abilityType_;
    std::string env_;
    std::string icon_;
    std::string label_;
    bool unlock_device_;
    std::vector<std::shared_ptr<AidSetData>> aidsets_;  // static
    std::vector<std::shared_ptr<AidSetData>> dynamicAidsets_;
};

struct DefaultInfo {
    std::string abilityName_;
    bool testWithRoutingTable_;
};

void from_json(const nlohmann::json& j, std::shared_ptr<AidSetData>& data);
void to_json(nlohmann::json& j, const std::shared_ptr<AidSetData>& data);
// get CardEmulationAbilityInfo from json , except dynamicAidsets_
void from_json(const nlohmann::json& j, std::shared_ptr<CardEmulationAbilityInfo>& data);
void LoadSdkDynamicInfo(const nlohmann::json& j, std::vector<std::shared_ptr<CardEmulationAbilityInfo>>& infos);
void to_json(nlohmann::json& j, const std::shared_ptr<CardEmulationAbilityInfo>& data);

std::vector<std::shared_ptr<CardEmulationAbilityInfo>> LoadAllAbilityInfosByUserId(int userId);
std::map<std::string, std::shared_ptr<CardEmulationAbilityInfo>> GetAbilityNameInfoMap(
    const std::vector<std::shared_ptr<CardEmulationAbilityInfo>>& infos);
std::vector<std::shared_ptr<CardEmulationAbilityInfo>> LoadStaticInfoFromFile(const std::string& path);

std::shared_ptr<DefaultInfo> LoadDefaultInfosByUserId(int userId);
}  // namespace OHOS::nfc::test

#endif
