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
#include "nfc_ability_manager.h"

#include "ability.h"
#include "ability_lifecycle_executor.h"
#include "infc_agent_service.h"
#include "ipc_object_stub.h"
#include "iremote_object.h"
#include "loghelper.h"
#include "nfc_map.h"
#include "nfc_sdk_common.h"
#include "ohos_application.h"
#include "pac_map.h"
#include "tag.h"

using namespace std;
using OHOS::AppExecFwk::AbilityLifecycleExecutor;
using OHOS::AppExecFwk::PacMap;
using OHOS::nfc::reader::ITagSession;
namespace OHOS {
namespace nfc {
namespace sdk {
NfcAbilityManager::NfcAbilityManager() {}

NfcAbilityManager::~NfcAbilityManager()
{
    DebugLog("Destruct NfcAbilityManager");
}

NfcAbilityManager::NfcAbilityObject::NfcAbilityObject(std::weak_ptr<Ability> ability, bool isActive)
    : mAbility_(ability),
      mReaderCallback_(std::shared_ptr<NfcAgent::TagCallback>()),
      mTagTypeFilter_(0),
      mIsActive_(isActive),
      mBinder_(new IPCObjectStub(u"nfc ability binder")),
      mReaderModeExtra_(std::shared_ptr<NfcMap>())
{
}

void NfcAbilityManager::NfcAbilityObject::SetAbility(std::weak_ptr<Ability> ability)
{
    mAbility_ = ability;
}

std::shared_ptr<Ability> NfcAbilityManager::NfcAbilityObject::GetAbility() const
{
    if (mAbility_.expired()) {
        return std::shared_ptr<Ability>();
    }
    return mAbility_.lock();
}

void NfcAbilityManager::NfcAbilityObject::SetReaderCallback(std::weak_ptr<NfcAgent::TagCallback> callBack)
{
    mReaderCallback_ = callBack;
}

std::shared_ptr<NfcAgent::TagCallback> NfcAbilityManager::NfcAbilityObject::GetReaderCallback() const
{
    if (mReaderCallback_.expired()) {
        return std::shared_ptr<NfcAgent::TagCallback>();
    }
    return mReaderCallback_.lock();
}

void NfcAbilityManager::NfcAbilityObject::SetTagTypeFilter(int tagTypeFilter)
{
    mTagTypeFilter_ = tagTypeFilter;
}

int NfcAbilityManager::NfcAbilityObject::GetTagTypeFilter() const
{
    return mTagTypeFilter_;
}

void NfcAbilityManager::NfcAbilityObject::SetIsActive(bool isActive)
{
    mIsActive_ = isActive;
}

bool NfcAbilityManager::NfcAbilityObject::IsActive() const
{
    return mIsActive_;
}

void NfcAbilityManager::NfcAbilityObject::SetBinder(sptr<IRemoteObject> binder)
{
    mBinder_ = binder;
}

sptr<IRemoteObject> NfcAbilityManager::NfcAbilityObject::GetBinder() const
{
    return mBinder_;
}

void NfcAbilityManager::NfcAbilityObject::SetReaderModeExtra(std::weak_ptr<NfcMap> readerModeExtra)
{
    mReaderModeExtra_ = readerModeExtra;
}

std::shared_ptr<NfcMap> NfcAbilityManager::NfcAbilityObject::GetReaderModeExtra() const
{
    if (mReaderModeExtra_.expired()) {
        return std::shared_ptr<NfcMap>();
    }
    return mReaderModeExtra_.lock();
}

NfcAbilityManager::NfcApplicationObject::NfcApplicationObject(std::weak_ptr<OHOSApplication> app)
    : mApp_(app), mRegistNum_(0)
{
}

std::shared_ptr<OHOSApplication> NfcAbilityManager::NfcApplicationObject::GetApp()
{
    if (mApp_.expired()) {
        return std::shared_ptr<OHOSApplication>();
    }
    return mApp_.lock();
}

void NfcAbilityManager::NfcApplicationObject::RegistApp(std::weak_ptr<NfcAbilityManager> callback)
{
    mRegistNum_++;
    if ((mRegistNum_ == 1) && !mApp_.expired()) {
        mApp_.lock()->RegisterAbilityLifecycleCallbacks(callback.lock());
    }
}

void NfcAbilityManager::NfcApplicationObject::UnRegistApp(std::weak_ptr<NfcAbilityManager> callback)
{
    mRegistNum_--;
    if ((mRegistNum_ == 0) && !mApp_.expired()) {
        mApp_.lock()->UnregisterAbilityLifecycleCallbacks(callback.lock());
    }
}
std::shared_ptr<NfcAbilityManager::NfcApplicationObject> NfcAbilityManager::FindNfcApplicationObject(
    std::weak_ptr<OHOSApplication> app)
{
    if (app.expired()) {
        return std::shared_ptr<NfcAbilityManager::NfcApplicationObject>();
    }

    std::vector<std::shared_ptr<NfcAbilityManager::NfcApplicationObject>>::iterator iter;
    for (iter = mNfcApplicationObject_.begin(); iter != mNfcApplicationObject_.end(); ++iter) {
        if (!(*iter)) {
            continue;
        }

        if ((*iter)->GetApp() == app.lock()) {
            return (*iter);
        }
    }

    return std::shared_ptr<NfcAbilityManager::NfcApplicationObject>();
}

std::shared_ptr<NfcAbilityManager::NfcApplicationObject> NfcAbilityManager::FindAndRemoveNfcApplicationObject(
    std::weak_ptr<OHOSApplication> app)
{
    if (app.expired()) {
        return std::shared_ptr<NfcAbilityManager::NfcApplicationObject>();
    }

    int index = 0;
    for (auto& object : mNfcApplicationObject_) {
        if (object->GetApp() == app.lock()) {
            mNfcApplicationObject_.erase(mNfcApplicationObject_.begin() + index);
            return object;
        }
        ++index;
    }
    return std::shared_ptr<NfcAbilityManager::NfcApplicationObject>();
}

std::shared_ptr<NfcAbilityManager::NfcAbilityObject> NfcAbilityManager::FindNfcAbilityObject(
    std::weak_ptr<Ability> ability)
{
    if (ability.expired()) {
        return std::shared_ptr<NfcAbilityManager::NfcAbilityObject>();
    }

    std::vector<std::shared_ptr<NfcAbilityManager::NfcAbilityObject>>::iterator iter;
    for (iter = mNfcAbilityObjectList_.begin(); iter != mNfcAbilityObjectList_.end(); ++iter) {
        if (!(*iter)) {
            continue;
        }

        if ((*iter)->GetAbility() == ability.lock()) {
            return (*iter);
        }
    }

    return std::shared_ptr<NfcAbilityManager::NfcAbilityObject>();
}

std::shared_ptr<NfcAbilityManager::NfcAbilityObject> NfcAbilityManager::FindAndRemoveNfcAbilityObject(
    std::shared_ptr<Ability> ability)
{
    if (!ability) {
        DebugLog("Ability Is Empty.");
        return std::shared_ptr<NfcAbilityManager::NfcAbilityObject>();
    }
    DebugLog("Ability (%p) Is Exist.", (void*)ability.get());
    int index = 0;
    for (auto& object : mNfcAbilityObjectList_) {
        DebugLog("Ability Object (%p) Of List (%d).", (void*)object->GetAbility().get(), index);
        if (object->GetAbility().get() == ability.get()) {
            DebugLog("Find And Remove Ability Object.");
            mNfcAbilityObjectList_.erase(mNfcAbilityObjectList_.begin() + index);
            object->SetBinder(nullptr);
            return object;
        }
        ++index;
    }
    DebugLog("Ability Object Isnot found.");
    return std::shared_ptr<NfcAbilityManager::NfcAbilityObject>();
}

std::shared_ptr<NfcAbilityManager::NfcAbilityObject> NfcAbilityManager::GetNfcAbilityObject(
    std::weak_ptr<Ability> ability)
{
    if (ability.expired()) {
        return std::unique_ptr<NfcAbilityManager::NfcAbilityObject>();
    }
    std::shared_ptr<NfcAbilityObject> nfcAbilityObject = FindNfcAbilityObject(ability);
    if (!nfcAbilityObject) {
        // new a ability object
        nfcAbilityObject = AddAbilityObject(ability);
        AddApp(ability.lock()->GetApplication());
    }
    return nfcAbilityObject;
}

void NfcAbilityManager::AddApp(std::weak_ptr<OHOSApplication> app)
{
    if (app.expired()) {
        return;
    }
    std::shared_ptr<NfcApplicationObject> nfcApplicationObject = FindNfcApplicationObject(app);

    if (!nfcApplicationObject) {
        nfcApplicationObject = std::make_shared<NfcApplicationObject>(app);
        mNfcApplicationObject_.push_back(nfcApplicationObject);
    }
    nfcApplicationObject->RegistApp(shared_from_this());
    return;
}

void NfcAbilityManager::RemoveApp(std::weak_ptr<OHOSApplication> app)
{
    std::shared_ptr<NfcApplicationObject> nfcApplicationObject = FindAndRemoveNfcApplicationObject(app);

    if (nfcApplicationObject) {
        DebugLog("[NfcAbilityManager::RemoveApp] UnRegistApp!");
        nfcApplicationObject->UnRegistApp(shared_from_this());
    } else {
        DebugLog("[NfcAbilityManager::RemoveApp] no find app!");
    }
    return;
}

std::shared_ptr<NfcAbilityManager::NfcAbilityObject> NfcAbilityManager::AddAbilityObject(std::weak_ptr<Ability> ability)
{
    if (ability.expired()) {
        return std::shared_ptr<NfcAbilityManager::NfcAbilityObject>();
    }

    bool bActive = (ability.lock()->GetState() == AbilityLifecycleExecutor::LifecycleState::ACTIVE) ? true : false;
    std::shared_ptr<NfcAbilityObject> nfcAbilityObject = std::make_shared<NfcAbilityObject>(ability, bActive);

    if (nfcAbilityObject) {
        mNfcAbilityObjectList_.push_back(nfcAbilityObject);
    }
    return nfcAbilityObject;
}

void NfcAbilityManager::RemoveAbilityObject(std::shared_ptr<NfcAbilityObject> nfcAbilityObject)
{
    for (size_t i = 0; i < mNfcAbilityObjectList_.size(); i++) {
        if (mNfcAbilityObjectList_.at(i) == nfcAbilityObject) {
            mNfcAbilityObjectList_.erase(mNfcAbilityObjectList_.begin() + i);
            break;
        }
    }
}

void NfcAbilityManager::DestroyAbilityObject(std::weak_ptr<Ability> ability)
{
    if (ability.expired()) {
        DebugLog("[NfcAbilityManager::DestroyAbilityObject] ability is null!");
        return;
    }
    std::lock_guard<std::mutex> guard(mMutex_);
    std::shared_ptr<NfcAbilityObject> abilityObject = FindAndRemoveNfcAbilityObject(ability.lock());
    if (!abilityObject) {
        DebugLog(
            "[NfcAbilityManager::DestroyAbilityObject] nfcAbilityObject is "
            "exist!");
        return;
    }
    std::weak_ptr<OHOSApplication> app = ability.lock()->GetApplication();
    if (!app.expired()) {
        RemoveApp(app);
    }
    // RemoveAbilityObject(abilityObject);
    abilityObject = nullptr;
}

std::shared_ptr<NfcAbilityManager::NfcAbilityObject> NfcAbilityManager::FindResumeNfcAbilityObject()
{
    std::vector<std::shared_ptr<NfcAbilityObject>>::iterator iter;
    for (iter = mNfcAbilityObjectList_.begin(); iter != mNfcAbilityObjectList_.end(); ++iter) {
        if ((*iter)->IsActive()) {
            return (*iter);
        }
    }
    return std::shared_ptr<NfcAbilityManager::NfcAbilityObject>();
}

int NfcAbilityManager::ActiveNfcReaderMode(std::weak_ptr<Ability> ability,
                                           std::weak_ptr<NfcAgent::TagCallback> callback,
                                           int tagTypeFilter,
                                           std::weak_ptr<NfcMap> extras)
{
    DebugLog("NfcAbilityManager::ActiveNfcReaderMode in.");
    if (ability.expired() || callback.expired()) {
        DebugLog("NfcAbilityManager::ActiveNfcReaderMode ability or callback expired.");
        return NfcErrorCode::NFC_SDK_ERROR_INVALID_PARAM;
    }
    bool bActive = false;
    sptr<IRemoteObject> readerObj;
    {
        std::lock_guard<std::mutex> guard(mMutex_);
        std::shared_ptr<NfcAbilityObject> abilityObject = GetNfcAbilityObject(ability);
        abilityObject->SetReaderCallback(callback);
        abilityObject->SetTagTypeFilter(tagTypeFilter);
        abilityObject->SetReaderModeExtra(extras);

        readerObj = abilityObject->GetBinder();
        bActive = abilityObject->IsActive();
    }

    if (bActive) {
        DebugLog("NfcAbilityManager::ActiveNfcReaderMode set reader mode .");
        NfcAgent::mNfcAgentService_.lock()->SetRWMode(readerObj, shared_from_this(), tagTypeFilter, extras);
    }
    DebugLog("NfcAbilityManager::ActiveNfcReaderMode set reader mode end .");
    return NfcErrorCode::NFC_SUCCESS;
}

int NfcAbilityManager::DeactiveNfcReaderMode(std::weak_ptr<Ability> ability)
{
    bool bActive = false;
    sptr<IRemoteObject> readerObj;
    {
        std::lock_guard<std::mutex> guard(mMutex_);
        std::shared_ptr<NfcAbilityObject> abilityObject = FindNfcAbilityObject(ability);
        if (!abilityObject) {
            DebugLog("NfcAbilityManager::DeactiveNfcReaderMode can not find abilityObject for ability.");
            return NfcErrorCode::NFC_SUCCESS;
        }
        abilityObject->SetReaderCallback(std::shared_ptr<NfcAgent::TagCallback>());
        abilityObject->SetTagTypeFilter(0);
        abilityObject->SetReaderModeExtra(std::shared_ptr<NfcMap>());

        bActive = abilityObject->IsActive();
        readerObj = abilityObject->GetBinder();
    }

    if (bActive) {
        DebugLog("NfcAbilityManager::DeactiveNfcReaderMode deactive reader mode .");
        NfcAgent::mNfcAgentService_.lock()->SetRWMode(
            readerObj, std::shared_ptr<IAppCallback>(), 0, std::shared_ptr<NfcMap>());
    }
    DebugLog("NfcAbilityManager::DeactiveNfcReaderMode deactive reader mode end .");
    return NfcErrorCode::NFC_SUCCESS;
}

void NfcAbilityManager::OnTagDiscovered(std::unique_ptr<Tag> tag)
{
    InfoLog("NfcAbilityManager::OnTagDiscovered tagtmp tech len.%d", tag->GetTagTechnologyList().size());
    if (!tag) {
        DebugLog("[NfcAbilityManager::OnTagDiscovered] receive tagObj is null!");
        return;
    }

    std::lock_guard<std::mutex> guard(mMutex_);
    std::shared_ptr<NfcAbilityObject> activeObject = FindResumeNfcAbilityObject();
    if (!activeObject) {
        DebugLog("[NfcAbilityManager::OnTagDiscovered] no activity ability!");
        return;
    }

    std::shared_ptr<NfcAgent::TagCallback> callback = activeObject->GetReaderCallback();
    if (!callback) {
        DebugLog("[NfcAbilityManager::OnTagDiscovered] ability callback is null!");
        return;
    }

    callback->CBTagDiscoverCommond(std::move(tag));
    return;
}

void NfcAbilityManager::OnAbilityStart(const std::shared_ptr<Ability>& ability) {}

void NfcAbilityManager::OnAbilityActive(const std::shared_ptr<Ability>& ability)
{
    DebugLog("NfcAbilityManager::OnAbilityActive in");
    if (!ability || NfcAgent::mNfcAgentService_.expired()) {
        DebugLog("[NfcAbilityManager::OnAbilityActive] activity or mNfcAgentService_ is null");
        return;
    }

    int tagTypeFilter = 0;
    sptr<IRemoteObject> readerObj;
    std::shared_ptr<NfcMap> readerModeExtra;
    {
        std::lock_guard<std::mutex> guard(mMutex_);
        std::shared_ptr<NfcAbilityManager::NfcAbilityObject> abilityObj = FindNfcAbilityObject(ability);
        if (!abilityObj) {
            DebugLog("NfcAbilityManager::OnAbilityActive can find ability .");
            return;
        }

        abilityObj->SetIsActive(true);

        tagTypeFilter = abilityObj->GetTagTypeFilter();
        readerObj = abilityObj->GetBinder();
        readerModeExtra = abilityObj->GetReaderModeExtra();
    }

    if (tagTypeFilter) {
        DebugLog("NfcAbilityManager::OnAbilityActive set reader mode .");
        NfcAgent::mNfcAgentService_.lock()->SetRWMode(readerObj, shared_from_this(), tagTypeFilter, readerModeExtra);
    }
    return;
}

void NfcAbilityManager::OnAbilityInactive(const std::shared_ptr<Ability>& ability)
{
    DebugLog("NfcAbilityManager::OnAbilityInactive in");
    if (!ability) {
        DebugLog("[NfcAbilityManager::OnAbilityInactive] ability is null");
        return;
    }

    bool readmodeFlag = false;
    sptr<IRemoteObject> readerObj;
    {
        std::lock_guard<std::mutex> guard(mMutex_);
        std::shared_ptr<NfcAbilityManager::NfcAbilityObject> abilityObj = FindNfcAbilityObject(ability);
        if (!abilityObj) {
            DebugLog("NfcAbilityManager::OnAbilityInactive can find ability .");
            return;
        }
        abilityObj->SetIsActive(false);
        int tagTypeFilter = abilityObj->GetTagTypeFilter();

        readerObj = abilityObj->GetBinder();
        readmodeFlag = (tagTypeFilter != 0);
    }

    if (readmodeFlag && !NfcAgent::mNfcAgentService_.expired()) {
        DebugLog("NfcAbilityManager::OnAbilityInactive set reader mode .");
        NfcAgent::mNfcAgentService_.lock()->SetRWMode(
            readerObj, std::shared_ptr<IAppCallback>(), 0, std::shared_ptr<NfcMap>());
        return;
    }
}

void NfcAbilityManager::OnAbilityStop(const std::shared_ptr<Ability>& ability)
{
    DebugLog("NfcAbilityManager::OnAbilityStop in");
    if (!ability) {
        DebugLog("[NfcAbilityManager::OnAbilityStop] ability is null");
        return;
    }
    DestroyAbilityObject(ability);
}

void NfcAbilityManager::OnAbilityBackground(const std::shared_ptr<Ability>& ability) {}
void NfcAbilityManager::OnAbilityForeground(const std::shared_ptr<Ability>& ability) {}
void NfcAbilityManager::OnAbilitySaveState(const OHOS::AppExecFwk::PacMap& outState) {}
}  // namespace sdk
}  // namespace nfc
}  // namespace OHOS