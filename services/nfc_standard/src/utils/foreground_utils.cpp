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
#include "utils/foreground_utils.h"

#include "loghelper.h"

namespace OHOS{
namespace nfc{
ForegroundUtils::ForegroundUtils()
{
    mFrontUidPids_.clear();
    mBackCallbacks_.clear();
}

ForegroundUtils::~ForegroundUtils()
{
    std::lock_guard<std::mutex> guard(mMutex_);
    if (mFrontUidPids_.size() > 0) {
        mFrontUidPids_.clear();
    }
    if (mBackCallbacks_.size() > 0) {
        mBackCallbacks_.clear();
    }
}

ForegroundUtils* ForegroundUtils::GetInstance()
{
    static ForegroundUtils instance;
    return std::addressof(instance);
}

bool ForegroundUtils::RegisterUidToBackgroundCallback(std::weak_ptr<Callback> callback, int uid)
{
    std::lock_guard<std::mutex> guard(mMutex_);
    if (!IsInForegroundLocked(uid)) {
        return false;
    }
    // This uid is in the foreground; register callback for when it moves
    // into the background.
    auto range = mBackCallbacks_.equal_range(uid);
    for (auto iter = range.first; iter != range.second; ++iter) {
        if (iter->second.lock() == callback.lock()) {
            return false;
        }
    }
    mBackCallbacks_.insert(std::make_pair(uid, callback));
    return true;
}

bool ForegroundUtils::IsInForeground(int uid)
{
    std::lock_guard<std::mutex> guard(mMutex_);
    return IsInForegroundLocked(uid);
}

bool ForegroundUtils::IsInForegroundLocked(int uid)
{
    // Find PID by UID
    InfoLog("Is In Foreground Of UID (%d)", uid);
    // Checking UID
    return true;
}

bool ForegroundUtils::IsInForegroundForTest(int uid)
{
    std::lock_guard<std::mutex> guard(mMutex_);
    return mFrontUidPids_.count(uid) > 0;
}

void ForegroundUtils::OnForegroundActivitiesChanged(int pid, int uid, bool hasFrontActivities)
{
    {
        std::lock_guard<std::mutex> guard(mMutex_);
        if (hasFrontActivities) {
            mFrontUidPids_.insert(std::make_pair(uid, std::make_pair(pid, true)));
            return;
        }
        auto range = mFrontUidPids_.equal_range(uid);
        for (auto i = range.first; i != range.second;) {
            if (i->second.first == pid) {
                i = mFrontUidPids_.erase(i);
                continue;
            }
            ++i;
        }
    }

    // UID To Background
    if (mFrontUidPids_.count(uid) == 0) {
        HandleUidToBackground(uid);
    }
}

void ForegroundUtils::OnProcessDied(int pid, int uid)
{
    OnForegroundActivitiesChanged(pid, uid, false);
}

void ForegroundUtils::HandleUidToBackground(int uid)
{
    std::vector<std::weak_ptr<ForegroundUtils::Callback>> pendingCallbacks;
    {
        std::lock_guard<std::mutex> guard(mMutex_);
        auto range = mBackCallbacks_.equal_range(uid);
        for (auto iter = range.first; iter != range.second;) {
            pendingCallbacks.push_back(iter->second);
            iter = mBackCallbacks_.erase(iter);
        }
    }
    // Release lock for callbacks
    for (auto iter = pendingCallbacks.begin(); iter != pendingCallbacks.end(); ++iter) {
        if (iter->expired()) {
            continue;
        }
        (*iter).lock()->OnUidToBackground(uid);
    }
}
}  // namespace nfc
}  // namespace OHOS