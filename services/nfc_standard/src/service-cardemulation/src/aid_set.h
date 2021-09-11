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

#ifndef AID_SET_H
#define AID_SET_H

#include <functional>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include "aid_string.h"
#include "nlohmann/json.hpp"

namespace OHOS::AppExecFwk {
class ElementName;
}
namespace OHOS::nfc::cardemulation {
class CardEmulationServiceInfo;
using aidset_t = std::set<std::shared_ptr<AidString>, AidStringSPtrCompare>;

class AidSet final {
public:
    static AidSet From(const std::vector<std::string>& aids);
    static AidSet From(const std::vector<AidString>& aids);
    static std::unique_ptr<AidSet> FromRawString(const std::vector<std::string>& aids);
    static std::unique_ptr<AidSet> FromAidString(const std::vector<AidString>& aids);
    AidSet();

    explicit AidSet(std::string tp);
    ~AidSet();
    AidSet(const AidSet& rh);
    AidSet& operator=(const AidSet& rh);
    AidSet(AidSet&& rh);
    AidSet& operator=(AidSet&& rh);

    std::string GetType() const;
    void SetType(std::string tp);
    std::string GetDescription() const;
    void SetDescription(std::string description);
    std::shared_ptr<OHOS::AppExecFwk::ElementName> GetOwnerName() const;
    std::weak_ptr<CardEmulationServiceInfo> GetOwner() const;
    void SetOwner(const std::shared_ptr<CardEmulationServiceInfo>& owner);
    std::string GetExecutionEnvironment() const;
    bool Empty() const;
    size_t Size() const;
    bool HasAidString(const AidString& aid);
    void Visit(std::function<void(const std::string&, const AidString&)> visitor) const;
    std::vector<std::shared_ptr<AidString>> GetAll() const;
    std::vector<std::string> GetAllAidRawString(bool fillSpace = false) const;
#ifdef DEBUG

    std::string ToString() const;
#endif  // DEBUG

    bool AddAidString(AidString aid);
    void RemoveAidString(const AidString& aid);
    void Clear();

    bool Equals(const AidSet& other) const;
    bool operator==(const AidSet& other) const;
    bool ConflictWith(const AidSet& other, bool typeEnabled) const;

    void swap(AidSet& other) noexcept;

    void FromJson(const nlohmann::json& j);
    void ToJson(nlohmann::json& j) const;

private:
    aidset_t::const_iterator Find(const AidString& aid) const;
    bool AidsEqual(const aidset_t& aids) const;
    void Copy(const AidSet& rh);
    void Move(AidSet&& other);

private:
    std::string type_;
    std::string description_;
    std::weak_ptr<CardEmulationServiceInfo> owner_;
    aidset_t aidset_;
};
void from_json(const nlohmann::json& j, std::shared_ptr<AidSet>& info);
void to_json(nlohmann::json& j, const std::shared_ptr<AidSet>& info);
void from_json(const nlohmann::json& j, AidSet& info);
void to_json(nlohmann::json& j, const AidSet& info);
}  // namespace OHOS::nfc::cardemulation
#endif  // !AID_SET_H
