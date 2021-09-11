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
#include "tag_dispatcher.h"

#include <functional>

#include "context.h"
#include "event_handler.h"
#include "iapp_callback.h"
#include "infc_agent_service.h"
#include "ipc_skeleton.h"
#include "iremote_object.h"
#include "itag_end_point.h"
#include "loghelper.h"
#include "ndef_message.h"
#include "nfc_map.h"
#include "nfc_service.h"
#include "nfc_service_define.h"
#include "refbase.h"
#include "resources.h"
#include "shared_preferences.h"
#include "tag.h"
#include "tag_session.h"
#include "utils/common_utils.h"
#include "want.h"


using namespace std;
using namespace osal;

namespace OHOS {
using AAFwk::Want;
using ObjectMapIter = std::map<int, std::shared_ptr<ITagEndPoint>>::iterator;
using FunCallback = std::function<void(int)>;

/**
 * @brief The IRemoteObject Death Receiver Implement
 */
class BinderDeathReceiver : public IRemoteObject::DeathRecipient {
public:
    explicit BinderDeathReceiver(std::weak_ptr<nfc::reader::TagDispatcher> dispatcher) : mDispatcher_(dispatcher) {}
    ~BinderDeathReceiver() {}
    void OnRemoteDied(const wptr<IRemoteObject>& object) override
    {
        InfoLog("BinderDeathReceiver::OnRemoteDied");
        if (!mDispatcher_.expired()) {
            mDispatcher_.lock()->BinderDied();
        }
    }

private:
    std::weak_ptr<nfc::reader::TagDispatcher> mDispatcher_;
};

namespace nfc {
namespace reader {
enum DispatchResult { DISPATCH_SUCCESS = 1, DISPATCH_FAIL, DISPATCH_UNLOCK };

// Is debounce processing supported
bool TagDispatcher::sToastDebounce_ = false;
// Debounce delay time
int TagDispatcher::sToastDebounceTimeMs = 3000;
int TagDispatcher::mDispatchFailedCounts_;
int TagDispatcher::mDispatchFailedMax_;
/**
 * @brief When The End-point tag is leave, DeviceHost is called this function
 * @param handle the End-point tag handle
 */
void TagDispatcher::TagDisconnectedCallback(int handle)
{
    UnregisterObject(handle);
    mNfcService_.lock()->ApplyDiscovering(false);
}
/**
 * @brief Reset the Reader Mode Parameters when the associate Reader App is Died.
 */
void TagDispatcher::BinderDied()
{
    InfoLog("IRometeObject Died");

    std::lock_guard<std::mutex> lock(mMtx_);
    if (!mReaderParams_) {
        return;
    }
    mReaderParams_ = std::shared_ptr<ReaderModeParams>();
    mNfcService_.lock()->ApplyDiscovering(false);
}
/**
 * @brief Dispatch the Discovered Tag.
 * @param tag the Discovered End-point Tag
 * @return the Dispatched result
 */
int TagDispatcher::DispatchTag(std::shared_ptr<ITagEndPoint> tag)
{
    DebugLog("Tag Detected, notifying applications");
    if (!tag) {
        InfoLog("TagEndPoint Is Unexist.");
        return 0;
    }
    // The Binding Callback
    static ITagEndPoint::TagDisconnectedCallBack callback =
        bind(&TagDispatcher::TagDisconnectedCallback, this, std::placeholders::_1);
    // Reset the screen activity time
    mNfcService_.lock()->PowerActivity();
    std::shared_ptr<ReaderModeParams> readerParams;
    int presenceCheckDelay = DEFAULT_PRESENCE_CHECK_DELAY;
    std::string debounceTagUid;
    int debounceTagMs{0};
    {
        std::lock_guard<std::mutex> lock(mMtx_);
        debounceTagUid = mTagDebounceUid_;
        debounceTagMs = mTagDebounceMs_;

        readerParams = mReaderParams_;
    }
    if (readerParams) {
        presenceCheckDelay = readerParams->mPresenceCheckDelay_;
        if ((readerParams->mFlags_ & FLAG_READER_CHECK_SKIP_NDEF) != 0) {
            DebugLog("Skipping NDEF detection in reader mode");
            tag->StartPresenceChecking(presenceCheckDelay, callback);
            return DispatchTagEndpoint(tag, readerParams);
        }
    }
    std::string ndefMsg = tag->ReadNdef();
    std::weak_ptr<sdk::NdefMessage> ndefMessage = sdk::NdefMessage::GetNdefMessage(ndefMsg);
    if (ndefMessage.expired()) {
        // First try to see if this was a bad tag read
        if (!tag->Reconnect()) {
            tag->Disconnect();
            if (mNfcService_.lock()->IsScreenUnlock() && !sToastDebounce_) {
#if _TOAST_
                Toast.makeText(mContext, R.string.tag_read_error, Toast.LENGTH_SHORT).show();
#endif
                sToastDebounce_ = true;
                int64_t newTime = CommonUtils::GetCurrentMilliSecondsTime();
                mHandler_.lock()->SendTimingEvent(MSG_TOAST_DEBOUNCE_EVENT, newTime + sToastDebounceTimeMs);
            }
            return 0;
        }
    }
    if (!debounceTagUid.empty()) {
        // If we're debouncing and the UID or the NDEF message of the tag match,
        // don't dispatch but drop it.
        if (debounceTagUid.compare(tag->GetUid()) == 0 ||
            (!ndefMessage.expired() && ndefMsg.compare(mLastReadNdefMessage_) == 0)) {
            mHandler_.lock()->RemoveEvent(MSG_TAG_DEBOUNCE);
            int64_t newTime = CommonUtils::GetCurrentMilliSecondsTime();
            mHandler_.lock()->SendTimingEvent(MSG_TAG_DEBOUNCE, newTime + debounceTagMs);
            tag->Disconnect();
            return 0;
        } else {
            ResetIgnoreTag();
        }
    }
    mLastReadNdefMessage_ = ndefMsg;
    tag->StartPresenceChecking(presenceCheckDelay, callback);
    return DispatchTagEndpoint(tag, readerParams);
}
/**
 * @brief Foreground Dispatch
 * @param tagEndpoint the End-point Tag
 * @param readerParams the reader mode parameters
 */
int TagDispatcher::DispatchTagEndpoint(std::shared_ptr<ITagEndPoint> tagEndpoint,
                                       std::weak_ptr<ReaderModeParams> readerParams)
{
    // Get the tag session instance
    string uid = tagEndpoint->GetUid();
    std::unique_ptr<sdk::Tag> tag = std::make_unique<sdk::Tag>(uid,
                                                               tagEndpoint->GetTechList(),
                                                               tagEndpoint->GetTechExtras(),
                                                               tagEndpoint->GetHandle(),
                                                               mNfcService_.lock()->GetTagSession());
    RegisterTagObject(tagEndpoint);
    if (!readerParams.expired()) {
        if ((readerParams.lock()->mFlags_ & FLAG_READER_NO_PLATFORM_SOUNDS) == 0) {
            mNfcService_.lock()->PlaySound(SOUND_END);
        }
        if (readerParams.lock()->mCallback_) {
            readerParams.lock()->mCallback_->OnTagDiscovered(std::move(tag));
            return DISPATCH_SUCCESS;
        }
    }
    int dispatchResult = DispatchTagOnBackground(std::move(tag));
    if (dispatchResult == DISPATCH_FAIL) {
        DebugLog("Tag dispatch failed");
        UnregisterObject(tagEndpoint->GetHandle());

        int pollDelay = mResources_.lock()->GetInt(R::unknown_tag_polling_delay);
        if (pollDelay >= 0) {
            tagEndpoint->StopPresenceChecking();
            mNfcService_.lock()->PauseDiscovering(pollDelay);
        }
        if (mNfcService_.lock()->IsScreenUnlock() && mResources_.lock()->GetBool(R::enable_notify_dispatch_failed)) {
            if (!sToastDebounce_) {
#if _TOAST_  // Prompt window will be poped up on the view
                Toast.makeText(mContext, R.string.tag_dispatch_failed, Toast.LENGTH_SHORT).show();
#endif
                sToastDebounce_ = true;
                int64_t newTime = CommonUtils::GetCurrentMilliSecondsTime();
                mHandler_.lock()->SendTimingEvent(MSG_TOAST_DEBOUNCE_EVENT, newTime + sToastDebounceTimeMs);
            }
            mNfcService_.lock()->PlaySound(SOUND_ERROR);
        }
        if (mNfcMessageEnabled_ && mDispatchFailedCounts_++ > mDispatchFailedMax_) {
            Want dialogWant;
            // Failure tips
            mContext_.lock()->StartAbility(dialogWant);
            mPrefs_.lock()->PutBool(PREF_NFC_MESSAGE, false);
            mNfcMessageEnabled_ = false;
            mDispatchFailedCounts_ = 0;
            DebugLog("Tag dispatch failed notification");
        }
    } else if (DISPATCH_SUCCESS == dispatchResult) {
        mDispatchFailedCounts_ = 0;
        mNfcService_.lock()->PlaySound(SOUND_END);
    }
    return dispatchResult;
}
/**
 * @brief The Background Dispatch
 * @param  tag the End-point tag
 * @return the dispatch result
 */
int TagDispatcher::DispatchTagOnBackground(std::unique_ptr<sdk::Tag> tag)
{
    InfoLog("Tag (%s) Detected And Background Dispatch.", tag->GetTagId().c_str());
    return DISPATCH_FAIL;
}
/**
 * @brief Reset the ignore tag parameters
 */
void TagDispatcher::ResetIgnoreTag()
{
    // Didn't see the tag again, tag is gone
    std::weak_ptr<sdk::ITagRemovedCallback> tagRemovedHandler;
    {
        std::lock_guard<std::mutex> lock(mMtx_);
        mTagDebounceUid_ = "";
        tagRemovedHandler = mTagRemovedHandler_;
        mTagDebounceNativeHandle_ = INVALID_NATIVE_HANDLE;
    }
    if (!tagRemovedHandler.expired()) {
        tagRemovedHandler.lock()->OnTagRemoved();
    }
}
/**
 * @brief Initialize the Dispatch Parameters
 * @return the result
 */
int TagDispatcher::InitializeReaderParams()
{
    sToastDebounceTimeMs = mResources_.lock()->GetInt(R::toast_debounce_time_ms);
    if (sToastDebounceTimeMs > MAX_TOAST_DEBOUNCE_TIME) {
        sToastDebounceTimeMs = MAX_TOAST_DEBOUNCE_TIME;
    }

    mDispatchFailedCounts_ = 0;
    if (mResources_.lock()->GetBool(R::enable_nfc_blocking_alert) &&
        mPrefs_.lock()->GetBool(PREF_NFC_MESSAGE, NFC_MESSAGE_DEFAULT)) {
        mNfcMessageEnabled_ = true;
        mDispatchFailedMax_ = mResources_.lock()->GetInt(R::nfc_blocking_count);
    } else {
        mNfcMessageEnabled_ = false;
    }
    return 0;
}
/**
 * @brief Reset the tag parameters
 */
void TagDispatcher::ResetTagParams()
{
    lock_guard<mutex> lock(mMtx_);
    mObjectMap_.clear();
    sToastDebounce_ = false;
}
/**
 * @brief Debounce Message Process
 */
void TagDispatcher::DebounceMsgProcess()
{
    sToastDebounce_ = false;
}
/**
 * @brief Setting the ignore tag parameters
 * @param handle the ignore tag handle
 * @param debounceMs the ignore time
 * @param callback To invoke callback when the tag is ignored
 * @return true - success; false - failed
 */
bool TagDispatcher::Ignore(int handle, int debounceMs, std::weak_ptr<sdk::ITagRemovedCallback> callback)
{
    if (debounceMs <= 0 && mTagDebounceNativeHandle_ != INVALID_NATIVE_HANDLE && handle == mTagDebounceNativeHandle_) {
        // Remove any previous messages and immediately debounce.
        mHandler_.lock()->RemoveEvent(MSG_TAG_DEBOUNCE);
        mHandler_.lock()->SendEvent(MSG_TAG_DEBOUNCE);
        return true;
    }

    std::shared_ptr<ITagEndPoint> tag = FindAndRemoveObject(handle);
    if (!tag) {
        return false;
    }
    {
        // Store UID and params
        std::lock_guard<std::mutex> guard(mMtx_);

        mTagDebounceMs_ = debounceMs;
        mTagDebounceNativeHandle_ = handle;
        mTagDebounceUid_ = tag->GetUid();
        mTagRemovedHandler_ = callback.lock();
    }

    /*
     * Disconnect from this tag; this should resume the normal
     * polling loop (and enter listen mode for a while), before
     * we pick up any tags again.
     */
    tag->Disconnect();
    int64_t newTime = CommonUtils::GetCurrentMilliSecondsTime();
    mHandler_.lock()->SendTimingEvent(MSG_TAG_DEBOUNCE, newTime + debounceMs);
    return true;
}
/**
 * @brief To get the Reader Mode Parameters
 * @return the Reader Mode Parameters
 */
std::weak_ptr<ReaderModeParams> TagDispatcher::GetReaderDatas()
{
    return mReaderParams_;
}
/**
 * @brief Setting the Reader Mode Parameters
 * @param readerObj the reader obj
 * @param appCallback the dispatch interface
 * @param flags the flags parameters
 * @param extras the extend parameters
 */
void TagDispatcher::SetRWMode(const sptr<IRemoteObject>& readerObj,
                              std::weak_ptr<sdk::IAppCallback> appCallback,
                              int flags,
                              std::weak_ptr<sdk::NfcMap> extras)
{
    // The callingUid Must be SYSTEM_UID.
    int callingUid = IPCSkeleton::GetCallingUid();
    if (!mNfcService_.lock()->IsInForeground(callingUid)) {
        ErrorLog("setReaderMode: Caller is not in foreground and is not system process.");
        return;
    }
    if (!mNfcService_.lock()->IsNfcEnabled()) {
        ErrorLog("setReaderMode() called while NFC is not enabled.");
        return;
    }
    if (!readerObj) {
        ErrorLog("ReaderModeDatas Is Empty. Or IRemoteObject Is Unexist.");
        return;
    }

    {
        std::lock_guard<std::mutex> guard(mMtx_);
        if (flags != 0) {
            mReaderParams_ = std::make_shared<ReaderModeParams>();
            mReaderParams_->mCallback_ = appCallback.lock();
            mReaderParams_->mFlags_ = flags;
            if (extras.expired()) {
                mReaderParams_->mPresenceCheckDelay_ = DEFAULT_PRESENCE_CHECK_DELAY;
            } else {
                std::weak_ptr<long> checkDelay = extras.lock()->GetLong(EXTRA_READER_PRESENCE_CHECK_DELAY);
                if (checkDelay.expired()) {
                    mReaderParams_->mPresenceCheckDelay_ = DEFAULT_PRESENCE_CHECK_DELAY;
                } else {
                    mReaderParams_->mPresenceCheckDelay_ = static_cast<int>(*(checkDelay.lock()));
                }
            }
            if (!receiver_) {
                receiver_ = new BinderDeathReceiver(mNfcService_.lock()->GetTagDispatcher());
            }
            DebugLog("readerObj::AddDeathRecipient.");
            readerObj->AddDeathRecipient(receiver_);
        } else {
            mReaderParams_ = nullptr;
            StopPresenceChecking();
            readerObj->RemoveDeathRecipient(receiver_);
        }
    }
    mNfcService_.lock()->ApplyDiscovering(false);
}
/**
 * @brief Checking if there is a Tag currently
 * @return true - yes; false - no
 */
bool TagDispatcher::IsTagPresent()
{
    ObjectMapIter iter = mObjectMap_.begin();
    for (; iter != mObjectMap_.end(); ++iter) {
        return iter->second->IsPresent();
    }
    return false;
}
/**
 * @brief Stop Tag presence checking
 */
void TagDispatcher::StopPresenceChecking()
{
    ObjectMapIter iter = mObjectMap_.begin();
    for (; iter != mObjectMap_.end(); ++iter) {
        iter->second->StopPresenceChecking();
    }
}
/**
 * @brief Disconnect and clear all Tags
 */
void TagDispatcher::MaybeDisconnectTarget()
{
    lock_guard<mutex> lock(mMtx_);
    ObjectMapIter iter = mObjectMap_.begin();
    for (; iter != mObjectMap_.end(); ++iter) {
        iter->second->Disconnect();
    }
    mObjectMap_.clear();
}
/**
 * @brief Finde the End-point tag by the tag handle
 * @param key the tag handle
 * @return the End-point tag
 */
std::weak_ptr<ITagEndPoint> TagDispatcher::FindObject(int key)
{
    lock_guard<mutex> lock(mMtx_);
    ObjectMapIter device = mObjectMap_.find(key);
    if (device == mObjectMap_.end()) {
        WarnLog("Handle not found");
        return std::shared_ptr<ITagEndPoint>();
    }
    return device->second;
}
/**
 * @brief Find and remove the End-point tag by handle.
 */
std::shared_ptr<ITagEndPoint> TagDispatcher::FindAndRemoveObject(int handle)
{
    lock_guard<mutex> lock(mMtx_);
    ObjectMapIter device = mObjectMap_.find(handle);
    std::shared_ptr<ITagEndPoint> temp = nullptr;
    if (device == mObjectMap_.end()) {
        WarnLog("Handle not found");
    } else {
        temp = device->second;
        mObjectMap_.erase(handle);
    }
    return temp;
}
/**
 * @brief Register the End-point Tag Object
 * @param tag the End-point Tag
 */
void TagDispatcher::RegisterTagObject(std::shared_ptr<ITagEndPoint> tag)
{
    lock_guard<mutex> lock(mMtx_);
    mObjectMap_.insert(make_pair(tag->GetHandle(), tag));
}
/**
 * @brief Unregister the End-point Tag Object
 * @param handle the End-point Tag handle
 */
void TagDispatcher::UnregisterObject(int handle)
{
    lock_guard<mutex> lock(mMtx_);
    mObjectMap_.erase(handle);
}

void TagDispatcher::ResumeAppSwitches()
{
    DebugLog("ResumeAppSwitches");
}

TagDispatcher::TagDispatcher(std::weak_ptr<osal::Context> context, std::weak_ptr<nfc::INfcService> nfcService)
    : mContext_(context),
      mNfcService_(nfcService),
      mTagDebounceUid_(""),
      mTagDebounceNativeHandle_(INVALID_NATIVE_HANDLE),
      mLastReadNdefMessage_(""),
      mOverrideIntent_(nullptr),
      mReaderParams_(nullptr),
      receiver_(nullptr)
{
    mPrefs_ = mContext_.lock()->GetSharedPreferences(PREF);
    mResources_ = mContext_.lock()->GetResources();
    mHandler_ = mNfcService_.lock()->GetMessageHandler();
}

TagDispatcher::~TagDispatcher()
{
    std::lock_guard<std::mutex> guard(mMtx_);
    mOverrideIntent_ = nullptr;
    mReaderParams_ = nullptr;
}
}  // namespace reader
}  // namespace nfc
}  // namespace OHOS