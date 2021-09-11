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

#include "installed_ceservice_getter.h"

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <fstream>

#include "bundle_mgr_proxy.h"
#include "card_emulation_def.h"
#include "card_emulation_util.h"
#include "element_name.h"
#include "ipc_skeleton.h"
#include "loghelper.h"
#include "nlohmann/json.hpp"
#include "refbase.h"
#include "system_ability_definition.h"

#ifdef USE_HILOG
DEFINE_NFC_LOG_LABEL("CeServiceGetter");
#endif

static const std::string PATH_SEPARATOR = "/";
static const std::string PLAIN_NAME_SEPARATOR = "\\.";
static const std::string KEY_DEFAULT_DEVICE_ID = "device_id";
static const std::string KEY_DEFAULT_BUNDLE_NAME = "bundle_name";
static const std::string KEY_DEFAULT_ABILITY_NAME = "default_ability_name";
static const std::string KEY_ABILITIES = "abilities";

namespace OHOS::nfc::cardemulation {
static const std::string JSON_FILE_TYPE_STATIC = "static";
static const std::string JSON_FILE_TYPE_DYNAMIC = "dynamic";
static const std::string JSON_FILE_TYPE_DEFAULT = "default";
#ifdef MOCK_FOR_TESTING
static void HandleException(const std::exception& err, bool throwAgain)
{
    ErrorLog("catch exception. err: %s", err.what());
    if (throwAgain) {
        throw err;
    }
}
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
    ;
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
        ErrorLog("fail to parse: %s. err: %s", filepath.c_str(), err.what());
    } catch (std::exception& err) {
        HandleException(err, false);
    }
    return rv;
}
static void JsonDumpToFile(nlohmann::json* j, const std::string& path)
{
}
nfc::cardemulation::CeServiceGetter::CeServiceGetter()
{
}

std::vector<std::shared_ptr<CardEmulationServiceInfo>> CeServiceGetter::GetInstalled(int userId)
{
    using namespace OHOS;

    std::vector<std::shared_ptr<CardEmulationServiceInfo>> rv;
    auto it = infos_.find(userId);
    if (it == infos_.end()) {
        // load static file
        std::string file = BuildUsersJsonPath(userId, JSON_FILE_TYPE_STATIC);
        rv = LoadFromFile(file);
        infos_[userId] = rv;
        return rv;
    }
    rv = it->second;

    return rv;
}
std::vector<std::shared_ptr<CardEmulationServiceInfo>> CeServiceGetter::GetDynamic(int userId)
{
    std::vector<std::shared_ptr<CardEmulationServiceInfo>> rv;
    auto it = infos_.find(userId);
    if (it == infos_.end()) {
        // load static file
        std::string file = BuildUsersJsonPath(userId, JSON_FILE_TYPE_DYNAMIC);
        rv = LoadFromFile(file);
        infos_[userId] = rv;
        return rv;
    }
    return it->second;
}
OHOS::AppExecFwk::ElementName CeServiceGetter::GetDefaultElementName(int userId)
{
    OHOS::AppExecFwk::ElementName rv;
    try {
        auto it = defaultNames_.find(userId);
        if (it == defaultNames_.end()) {
            std::string path = BuildUsersJsonPath(userId, JSON_FILE_TYPE_DEFAULT);
            nlohmann::json j = ParseFromFile(path);
            if (!j.is_object()) {
                return rv;
            }
            std::string abilityName;
            j.at(KEY_DEFAULT_ABILITY_NAME).get_to(abilityName);
            rv = OHOS::AppExecFwk::ElementName("", "", abilityName);
            defaultNames_[userId] = rv;
            return rv;
        }
        return it->second;

    } catch (std::exception& err) {
        HandleException(err, false);
    }
    return rv;
}
void CeServiceGetter::StoreDynamic(int userId, const std::vector<std::shared_ptr<CardEmulationServiceInfo>>& infos)
{
    std::string path = BuildUsersJsonPath(userId, JSON_FILE_TYPE_DYNAMIC);
    nlohmann::json j = {{KEY_ABILITIES, infos}};

    JsonDumpToFile(&j, BuildUsersJsonPath(userId, JSON_FILE_TYPE_DYNAMIC));
}
void CeServiceGetter::StoreDefault(int userId, std::string abilityName)
{
    nlohmann::json j = {{KEY_DEFAULT_ABILITY_NAME, abilityName}};
    JsonDumpToFile(&j, BuildUsersJsonPath(userId, JSON_FILE_TYPE_DEFAULT));
}
std::vector<std::shared_ptr<CardEmulationServiceInfo>> CeServiceGetter::LoadFromFile(const std::string& path)
{
    std::vector<std::shared_ptr<CardEmulationServiceInfo>> rv;
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
#endif
std::unique_ptr<OHOS::AppExecFwk::ElementName> StringToElementName(const std::string& str)
{
    return std::make_unique<OHOS::AppExecFwk::ElementName>("", "", str);
}
std::string ElementNameToString(const OHOS::AppExecFwk::ElementName& elementName)
{
    return elementName.GetAbilityName() + PLAIN_NAME_SEPARATOR + elementName.GetBundleName() + PLAIN_NAME_SEPARATOR +
           elementName.GetDeviceID();
}
}  // namespace OHOS::nfc::cardemulation
