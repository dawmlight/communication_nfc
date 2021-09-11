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
#ifndef TAG_DISPATCH_H
#define TAG_DISPATCH_H

#include <map>
#include <memory>
#include <mutex>
#include <string>

#include "iremote_object.h"

namespace osal {
class Resources;
class Context;
class SharedPreferences;
}  // namespace osal

namespace OHOS {
namespace AppExecFwk {
class EventHandler;
}  // namespace AppExecFwk

namespace nfc::ncibal {
class ITagEndPoint;
}  // namespace nfc::ncibal
using nfc::ncibal::ITagEndPoint;

class IRemoteObject;
namespace nfc {
class INfcService;

namespace sdk {
class Tag;
class IAppCallback;
class ITagRemovedCallback;
class NfcMap;
}  // namespace sdk

namespace reader {
struct ReaderModeParams {
    int mFlags_{0};
    std::shared_ptr<nfc::sdk::IAppCallback> mCallback_{};
    int mPresenceCheckDelay_{0};
};

struct DispatchInfo {
    static std::string ndefUri_;
    static std::string mimeType_;
};

class TagDispatcher final {
public:
    TagDispatcher(std::weak_ptr<osal::Context> context, std::weak_ptr<nfc::INfcService> nfcService);
    ~TagDispatcher();
    TagDispatcher(const TagDispatcher&) = delete;
    TagDispatcher& operator=(const TagDispatcher&) = delete;
    int DispatchTag(std::shared_ptr<ITagEndPoint> tag);
    void ResetIgnoreTag();
    int InitializeReaderParams();
    void ResetTagParams();
    static void DebounceMsgProcess();
    void MaybeDisconnectTarget();
    bool IsTagPresent();
    bool Ignore(int handle, int debounceMs, std::weak_ptr<nfc::sdk::ITagRemovedCallback> callback);
    void SetRWMode(const sptr<IRemoteObject>& readerObj,
                   std::weak_ptr<sdk::IAppCallback> appCallback,
                   int flags,
                   std::weak_ptr<sdk::NfcMap> extras);
    void SetFrontDispatch(std::shared_ptr<void>) {}
    std::weak_ptr<ReaderModeParams> GetReaderDatas();
    void BinderDied();
    void ResumeAppSwitches();
    std::weak_ptr<ITagEndPoint> FindObject(int key);

protected:
    void TagDisconnectedCallback(int handle);
    int DispatchTagEndpoint(std::shared_ptr<ITagEndPoint> tagEndpoint, std::weak_ptr<ReaderModeParams> readerParams);
    int DispatchTagOnBackground(std::unique_ptr<sdk::Tag> tag);
    void StopPresenceChecking();
    std::shared_ptr<ITagEndPoint> FindAndRemoveObject(int handle);
    void RegisterTagObject(std::shared_ptr<ITagEndPoint> tag);
    void UnregisterObject(int handle);

private:
    std::weak_ptr<osal::Context> mContext_ = {};
    std::weak_ptr<osal::SharedPreferences> mPrefs_{};
    std::weak_ptr<osal::Resources> mResources_{};
    std::weak_ptr<nfc::INfcService> mNfcService_{};
    std::weak_ptr<AppExecFwk::EventHandler> mHandler_{};
    // Lock
    std::mutex mMtx_{};
    std::map<int, std::shared_ptr<ITagEndPoint>> mObjectMap_{};
    // Dispatch
    static int mDispatchFailedCounts_;
    static int mDispatchFailedMax_;
    int mNfcMessageEnabled_{0};
    // Is debounce processing supported
    static bool sToastDebounce_;
    // Debounce delay time
    static int sToastDebounceTimeMs;
    // Tag Debounce
    std::string mTagDebounceUid_;
    int mTagDebounceMs_{0};
    int mTagDebounceNativeHandle_;
    std::shared_ptr<sdk::ITagRemovedCallback> mTagRemovedHandler_{};
    //
    std::string mLastReadNdefMessage_;
    // front dispatch
    std::shared_ptr<void> mOverrideIntent_;
    // Reader mode
    std::shared_ptr<ReaderModeParams> mReaderParams_{};
    // Binder Death Receiver
    OHOS::sptr<IRemoteObject::DeathRecipient> receiver_{};
    // message
    static constexpr const auto PREF_NFC_MESSAGE = "nfc_message";
    static constexpr const bool NFC_MESSAGE_DEFAULT = true;
    // the nfc technology flags
    static constexpr const int FLAG_READER_CHECK_SKIP_NDEF = 0x80;
    static constexpr const int FLAG_READER_NO_PLATFORM_SOUNDS = 0x100;
    static constexpr const auto EXTRA_READER_PRESENCE_CHECK_DELAY = "presence";

    static constexpr const int INVALID_NATIVE_HANDLE = -1;
    static constexpr const int DEFAULT_PRESENCE_CHECK_DELAY = 125;

    static constexpr const int MAX_TOAST_DEBOUNCE_TIME = 10000;
};
}  // namespace reader
}  // namespace nfc
}  // namespace OHOS
#endif  // !TAG_DISPATCH_H
