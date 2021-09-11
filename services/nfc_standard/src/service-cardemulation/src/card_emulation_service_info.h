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

#ifndef CARD_EMULATION_SERVICE_INFO_H
#define CARD_EMULATION_SERVICE_INFO_H

#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

#include "iremote_object.h"
#include "nlohmann/json.hpp"
namespace OHOS::AppExecFwk {
class ElementName;
}
namespace OHOS::nfc {
namespace sdk::cardemulation {
class IApduChannel;
class ApduChannelProxy;
class CardEmulationServiceInfoLite;
}  // namespace sdk::cardemulation
namespace cardemulation {
class AidSet;
class CardEmulationServiceInfo : public std::enable_shared_from_this<CardEmulationServiceInfo>,
                                 public OHOS::IRemoteObject::DeathRecipient {
public:
    // a<b means a has higher priority than b
    static bool ALessB(const std::shared_ptr<AidSet>& a, const std::shared_ptr<AidSet>& b);
    static std::unique_ptr<OHOS::nfc::sdk::cardemulation::CardEmulationServiceInfoLite> GetLiteInfo(
        const CardEmulationServiceInfo& info);
    explicit CardEmulationServiceInfo(std::string location);
    CardEmulationServiceInfo(std::string location, const OHOS::AppExecFwk::ElementName& name);
    CardEmulationServiceInfo(std::string location, const OHOS::AppExecFwk::ElementName& name, bool mustUnlock);
    ~CardEmulationServiceInfo();
    void Visit(std::function<void(const std::string&, std::shared_ptr<AidSet>)> visitor) const;
    bool AddAidsetToStatic(std::unique_ptr<AidSet> aidset);
    bool AddAidset(std::unique_ptr<AidSet> aidset);
    bool RemoveAidset(const std::string& type);
    void SetName(const OHOS::AppExecFwk::ElementName& name);

    int SetExecutionEnvironment(std::string location);
    std::string GetExecutionEnvironment() const;
    std::string GetLabel() const;
    void SetLabel(std::string label);
    std::string GetIcon() const;
    void SetIcon(std::string icon);

    std::shared_ptr<OHOS::AppExecFwk::ElementName> GetName() const;
    bool MustUnlock() const;
    void SetMustUnlock(bool must);

    bool HasType(const std::string& type);

    void SetApduChannel(const OHOS::sptr<OHOS::IRemoteObject>& ro);
    OHOS::sptr<OHOS::nfc::sdk::cardemulation::ApduChannelProxy> GetApduChannel() const;
    std::vector<std::shared_ptr<AidSet>> SortAidsets(
        std::function<bool(std::shared_ptr<AidSet>, std::shared_ptr<AidSet>)> less);

    std::shared_ptr<AidSet> GetAidsetByType(const std::string& type);
    CardEmulationServiceInfo(CardEmulationServiceInfo const&) = delete;

    CardEmulationServiceInfo& operator=(CardEmulationServiceInfo const&) = delete;
    CardEmulationServiceInfo(CardEmulationServiceInfo&&) = delete;
    CardEmulationServiceInfo& operator=(CardEmulationServiceInfo&&) = delete;

    void FromJson(const nlohmann::json& jsonObject);
    void ToJson(nlohmann::json& j) const;

    std::string PrintAllAids();
    void OnRemoteDied(const OHOS::wptr<IRemoteObject>& object) override;

private:
    // type: aidset
    using Aidsets_t = std::map<std::string, std::shared_ptr<AidSet>>;
    Aidsets_t Merge() const;
    std::vector<std::shared_ptr<AidSet>> MergeToVector() const;

private:
    Aidsets_t staticAidsets_;
    Aidsets_t dynamicAidsets_;
    std::mutex mu_;
    // from config
    std::string rawExecEnv_;
    // from set api
    std::string execEnv_;
    std::string label_;
    std::string icon_;

    std::shared_ptr<OHOS::AppExecFwk::ElementName> elementName_{};

    bool mustUnlock_;
    OHOS::sptr<OHOS::nfc::sdk::cardemulation::ApduChannelProxy> apduChannel_;
};
void from_json(const nlohmann::json& jsonObject, CardEmulationServiceInfo& info);
void to_json(nlohmann::json& j, const CardEmulationServiceInfo& info);
void from_json(const nlohmann::json& jsonObject, std::shared_ptr<CardEmulationServiceInfo>& info);
void to_json(nlohmann::json& j, const std::shared_ptr<CardEmulationServiceInfo>& info);
}  // namespace cardemulation
}  // namespace OHOS::nfc
#endif  // CARD_EMULATION_SERVICE_INFO_H
