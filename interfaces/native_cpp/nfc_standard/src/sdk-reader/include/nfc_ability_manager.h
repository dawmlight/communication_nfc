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
#ifndef NFC_ABILITY_MANAGER_H
#define NFC_ABILITY_MANAGER_H

#include <mutex>

#include "nfc_agent.h"
#include "ohos_application.h"

namespace OHOS {
class IRemoteObject;
namespace AppExecFwk {
class Ability;
}

using AppExecFwk::PacMap;
namespace nfc {
namespace sdk {
class AppCallbackProxy;
class NfcMap;
class NfcAbilityManager final : public OHOS::AppExecFwk::AbilityLifecycleCallbacks,
                                public AppCallbackStub,
                                public std::enable_shared_from_this<NfcAbilityManager> {
public:
    class NfcAbilityObject {
    public:
        explicit NfcAbilityObject(std::weak_ptr<Ability> ability, bool isActive);
        ~NfcAbilityObject() = default;
        void SetAbility(std::weak_ptr<Ability> ability);
        std::shared_ptr<Ability> GetAbility() const;
        void SetReaderCallback(std::weak_ptr<NfcAgent::TagCallback> callBack);
        std::shared_ptr<NfcAgent::TagCallback> GetReaderCallback() const;
        void SetTagTypeFilter(int tagTypeFilter);
        int GetTagTypeFilter() const;
        void SetIsActive(bool isActive);
        bool IsActive() const;
        void SetBinder(sptr<IRemoteObject> binder);
        sptr<IRemoteObject> GetBinder() const;
        void SetReaderModeExtra(std::weak_ptr<NfcMap> readerModeExtra);
        std::shared_ptr<NfcMap> GetReaderModeExtra() const;

    private:
        std::weak_ptr<Ability> mAbility_{};
        std::weak_ptr<NfcAgent::TagCallback> mReaderCallback_{};
        int mTagTypeFilter_{0};
        bool mIsActive_{false};
        sptr<IRemoteObject> mBinder_{};
        std::weak_ptr<NfcMap> mReaderModeExtra_{};
    };

    class NfcApplicationObject {
    public:
        explicit NfcApplicationObject(std::weak_ptr<OHOSApplication> app);
        ~NfcApplicationObject() = default;
        std::shared_ptr<OHOSApplication> GetApp();
        void RegistApp(std::weak_ptr<NfcAbilityManager> callback);
        void UnRegistApp(std::weak_ptr<NfcAbilityManager> callback);

    private:
        std::weak_ptr<OHOSApplication> mApp_{};
        size_t mRegistNum_{0};
    };

    explicit NfcAbilityManager();
    ~NfcAbilityManager();

    std::shared_ptr<NfcAbilityManager::NfcApplicationObject> FindNfcApplicationObject(
        std::weak_ptr<OHOSApplication> app);
    std::shared_ptr<NfcAbilityManager::NfcApplicationObject> FindAndRemoveNfcApplicationObject(
        std::weak_ptr<OHOSApplication> app);

    std::shared_ptr<NfcAbilityManager::NfcAbilityObject> FindNfcAbilityObject(std::weak_ptr<Ability> ability);
    std::shared_ptr<NfcAbilityManager::NfcAbilityObject> FindAndRemoveNfcAbilityObject(
        std::shared_ptr<Ability> ability);
    // Find the AbilityObject for the Ability，if not find, create the AbilityObject for it
    std::shared_ptr<NfcAbilityManager::NfcAbilityObject> GetNfcAbilityObject(std::weak_ptr<Ability> ability);
    void AddApp(std::weak_ptr<OHOSApplication> app);
    void RemoveApp(std::weak_ptr<OHOSApplication> app);
    std::shared_ptr<NfcAbilityManager::NfcAbilityObject> AddAbilityObject(std::weak_ptr<Ability> ability);
    void RemoveAbilityObject(std::shared_ptr<NfcAbilityObject> nfcAbilityObject);
    std::shared_ptr<NfcAbilityObject> FindResumeNfcAbilityObject();

    void DestroyAbilityObject(std::weak_ptr<Ability> ability);
    int ActiveNfcReaderMode(std::weak_ptr<Ability> ability,
                            std::weak_ptr<NfcAgent::TagCallback> callback,
                            int tagTypeFilter,
                            std::weak_ptr<NfcMap> extras);
    int DeactiveNfcReaderMode(std::weak_ptr<Ability> ability);

    void OnTagDiscovered(std::unique_ptr<Tag> tag) override;

    // override from AbilityLifecycleCallbacks
    void OnAbilityStart(const std::shared_ptr<Ability>& ability) override;
    void OnAbilityInactive(const std::shared_ptr<Ability>& ability) override;
    void OnAbilityBackground(const std::shared_ptr<Ability>& ability) override;
    void OnAbilityForeground(const std::shared_ptr<Ability>& ability) override;
    void OnAbilityActive(const std::shared_ptr<Ability>& ability) override;
    void OnAbilityStop(const std::shared_ptr<Ability>& ability) override;
    void OnAbilitySaveState(const OHOS::AppExecFwk::PacMap& outState) override;

private:
    std::vector<std::shared_ptr<NfcAbilityManager::NfcApplicationObject>> mNfcApplicationObject_{};
    std::vector<std::shared_ptr<NfcAbilityManager::NfcAbilityObject>> mNfcAbilityObjectList_{};
    std::mutex mMutex_{};
};
}  // namespace sdk
}  // namespace nfc
}  // namespace OHOS
#endif  // !NFC_ABILITY_MANAGER_H
