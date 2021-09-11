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
#include "card_emulation_ability_info.h"

#include <cstdio>
#include <fstream>

#include "loghelper.h"

#define LogD(format, ...)                \
    {                                    \
        printf((format), ##__VA_ARGS__); \
    }
#define JSON_GET_TO(j, key, value)   \
    {                                \
        if (j.contains(key)) {       \
            j.at(key).get_to(value); \
        }                            \
    }
namespace OHOS::nfc::test {
static const std::string INFO_KEY_ABILITIES = "abilities";
static const std::string INFO_KEY_AID_SET_DESCRIPTION = "description";
static const std::string INFO_KEY_AID_SET_TYPE = "type";
static const std::string INFO_KEY_AID_SET_AIDS = "aids";
static const std::string INFO_KEY_ABILITY_NAME = "ability_name";
static const std::string INFO_KEY_ABILITY_TYPE = "ability_type";
static const std::string INFO_ABILITY_TYPE_HOST_CARD_EMULATION = "host_card_emulation";
static const std::string INFO_ABILITY_TYPE_OFFHOST_CARD_EMULATION = "offhost_card_emulation";
static const std::string INFO_KEY_AIDSETS = "aid_sets";
static const std::string INFO_KEY_DYNAMIC_AIDSETS = "dynamic_aid_sets";
static const std::string INFO_KEY_LABEL = "label";
static const std::string INFO_KEY_ICON = "icon";
static const std::string INFO_KEY_UNLOCK_DEVICE = "unlock_device";
static const std::string INFO_KEY_ENVIRONMENT = "environment";
static const std::string TEST_FLAG = "test_with_routing_table_flag";

void from_json(const nlohmann::json& j, std::shared_ptr<AidSetData>& data)
{
    if (!j.is_object()) {
        LogD("json is not an object.");
        return;
    }
    data = std::make_shared<AidSetData>();
    try {
        JSON_GET_TO(j, INFO_KEY_AID_SET_DESCRIPTION, data->description_);
        JSON_GET_TO(j, INFO_KEY_AID_SET_TYPE, data->type_);
        JSON_GET_TO(j, INFO_KEY_AID_SET_AIDS, data->aids_);
    } catch (const std::exception& e) {
        LogD("catch exception: %s\n", e.what());
    }
}
void to_json(nlohmann::json& j, const std::shared_ptr<AidSetData>& data)
{
    if (!data) {
        LogD("aidset data is nullptr .");
    }
    j = {{INFO_KEY_AID_SET_DESCRIPTION, data->description_},
         {INFO_KEY_AID_SET_TYPE, data->type_},
         {INFO_KEY_AID_SET_AIDS, data->aids_}};
}

void from_json(const nlohmann::json& j, std::shared_ptr<CardEmulationAbilityInfo>& data)
{
    if (!j.is_object()) {
        LogD("json is not an object.");
        return;
    }
    data = std::make_shared<CardEmulationAbilityInfo>();
    JSON_GET_TO(j, INFO_KEY_ABILITY_NAME, data->abilityName_);
    JSON_GET_TO(j, INFO_KEY_ABILITY_TYPE, data->abilityType_);
    JSON_GET_TO(j, INFO_KEY_ENVIRONMENT, data->env_);
    JSON_GET_TO(j, INFO_KEY_ICON, data->icon_);
    JSON_GET_TO(j, INFO_KEY_LABEL, data->label_);
    JSON_GET_TO(j, INFO_KEY_AIDSETS, data->aidsets_);
}

void LoadSdkDynamicInfo(const nlohmann::json& j, std::vector<std::shared_ptr<CardEmulationAbilityInfo>>& infos)
{
    if (!j.is_object()) {
        LogD("json is not an object.");
        return;
    }
    try {
        auto jAbilities = j.at(INFO_KEY_ABILITIES);
        if (!jAbilities.is_array()) {
            LogD("json abilities is not an array.");
            return;
        }
        auto d = jAbilities.dump();
        LogD("jAbilities \t%s\n", d.c_str());
        std::for_each(infos.begin(),
                      infos.end(),
                      [&jAbilities](std::vector<std::shared_ptr<CardEmulationAbilityInfo>>::reference r) {
                          std::for_each(jAbilities.begin(), jAbilities.end(), [&r](auto a) {
                              if (a.contains(r->abilityName_)) {
                                  a.at(r->abilityName_).at(INFO_KEY_AIDSETS).get_to(r->dynamicAidsets_);

                                  return;
                              }
                          });
                      });
    } catch (const std::exception& err) {
        LogD("catch exception: %s\n", err.what());
    }
}
void to_json(nlohmann::json& j, const std::shared_ptr<CardEmulationAbilityInfo>& data)
{
    if (!data) {
        LogD("info data is nullptr .");
        return;
    }
    j = {{INFO_KEY_ABILITY_NAME, data->abilityName_},
         {INFO_KEY_ABILITY_TYPE, data->abilityType_},
         {INFO_KEY_ENVIRONMENT, data->env_},
         {INFO_KEY_ICON, data->icon_},
         {INFO_KEY_LABEL, data->label_},
         {INFO_KEY_AIDSETS, data->aidsets_},
         {INFO_KEY_DYNAMIC_AIDSETS, data->dynamicAidsets_}};
}

static const std::string PATH_SEPARATOR = "/";
static const std::string JSON_FILE_TYPE_STATIC = "static";
static const std::string JSON_FILE_TYPE_DYNAMIC = "sdk_dynamic";
static const std::string JSON_FILE_TYPE_DEFAULT = "default";

static std::string GetJsonPathFromEnv()
{
    static const char* NFC_CARD_EMULATION_INFO_PATH = "NFC_CARD_EMULATION_INFO_PATH";
    std::string result = "./users";  // default

    char* path = std::getenv(NFC_CARD_EMULATION_INFO_PATH);
    if (path) {
        result = path;
    }
    return result;
}

static std::string BuildUsersJsonPath(int userId, const std::string& type /*static, dynamic, default*/)
{
    return GetJsonPathFromEnv() + PATH_SEPARATOR + std::to_string(userId) + "_" + type + ".json";
}

static void HandleException(const std::exception& err, bool throwAgain)
{
    LogD("catch exception. err: %s", err.what());
    if (throwAgain) {
        throw err;
    }
}

static nlohmann::json ParseFromFile(const std::string& filepath)
{
    nlohmann::json rv;
    try {
        std::ifstream f(filepath);
        if (!f.is_open()) {
            return rv;
        }
        rv = nlohmann::json::parse(f);
    } catch (nlohmann::json::parse_error& err) {
    } catch (std::exception& err) {
        HandleException(err, false);
    }
    return rv;
}
std::vector<std::shared_ptr<CardEmulationAbilityInfo>> LoadAllAbilityInfosByUserId(int userId)
{
    DebugLog("LoadAllAbilityInfosByUserId in");
    // load static info
    std::string path = BuildUsersJsonPath(userId, JSON_FILE_TYPE_STATIC);
    auto infos = LoadStaticInfoFromFile(path);
    std::string dynamicPath = BuildUsersJsonPath(userId, JSON_FILE_TYPE_DYNAMIC);

    auto j = ParseFromFile(dynamicPath);
    LoadSdkDynamicInfo(j, infos);
    DebugLog("LoadAllAbilityInfosByUserId path.%s dynamicPath.%s", path.c_str(), dynamicPath.c_str());
    return infos;
}

std::map<std::string, std::shared_ptr<CardEmulationAbilityInfo>> GetAbilityNameInfoMap(
    const std::vector<std::shared_ptr<CardEmulationAbilityInfo>>& infos)
{
    std::map<std::string, std::shared_ptr<CardEmulationAbilityInfo>> ret;
    std::for_each(infos.begin(), infos.end(), [&ret](auto r) { ret[r->abilityName_] = r; });
    return ret;
}
std::vector<std::shared_ptr<CardEmulationAbilityInfo>> LoadStaticInfoFromFile(const std::string& path)
{
    std::vector<std::shared_ptr<CardEmulationAbilityInfo>> rv;
    try {
        nlohmann::json j = ParseFromFile(path);
        if (!j.is_object()) {
            return rv;
        }
        j.at("abilities").get_to(rv);
    } catch (std::exception& err) {
        HandleException(err, false);
    }
    return rv;
}

std::shared_ptr<DefaultInfo> LoadDefaultInfosByUserId(int userId)
{
    std::string defaultPath = BuildUsersJsonPath(userId, JSON_FILE_TYPE_DEFAULT);
    DebugLog("LoadDefaultInfosByUserId defaultPath.%s", defaultPath.c_str());
    auto j = ParseFromFile(defaultPath);

    std::shared_ptr<DefaultInfo> data = std::make_shared<DefaultInfo>();
    std::string testFlag;
    JSON_GET_TO(j, INFO_KEY_ABILITY_NAME, data->abilityName_);
    JSON_GET_TO(j, TEST_FLAG, testFlag);
    data->testWithRoutingTable_ = std::atoi(testFlag.c_str());

    DebugLog("LoadDefaultInfosByUserId ability name.%s testflag.%d",
             data->abilityName_.c_str(),
             data->testWithRoutingTable_);
    return data;
}
}  // namespace OHOS::nfc::test
