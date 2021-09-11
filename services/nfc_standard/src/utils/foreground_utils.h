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
#ifndef FOREGROUND_UTILS_H
#define FOREGROUND_UTILS_H

#include <map>
#include <memory>
#include <mutex>
#include <vector>

namespace OHOS{
namespace nfc {
class ForegroundUtils {
public:
    class Callback {
    public:
        virtual ~Callback() {}
        virtual void OnUidToBackground(int uid) = 0;
    };
    static ForegroundUtils* GetInstance();
    // 显式禁止拷贝和赋值函数
    ForegroundUtils(const ForegroundUtils& manager) = delete;
    const ForegroundUtils& operator=(const ForegroundUtils& manager) = delete;

    bool RegisterUidToBackgroundCallback(std::weak_ptr<Callback> callback, int uid);
    bool IsInForeground(int uid);
    void OnForegroundActivitiesChanged(int pid, int uid, bool hasFrontActivities);
    void OnProcessDied(int pid, int uid);
    // GTest Interface
    bool IsInForegroundForTest(int uid);

private:
    ForegroundUtils();
    ~ForegroundUtils();
    bool IsInForegroundLocked(int uid);
    void HandleUidToBackground(int uid);

private:
    // 锁
    std::mutex mMutex_{};
    // UID-PID
    std::multimap<int, std::pair<int, bool>> mFrontUidPids_{};
    // UID-Callback
    std::multimap<int, std::weak_ptr<ForegroundUtils::Callback>> mBackCallbacks_{};
};

using UidPidMap = std::multimap<int, std::pair<int, bool>>;
using UidCallbackMap = std::multimap<int, std::weak_ptr<ForegroundUtils::Callback>>;
using UidCallbackPair = std::pair<UidCallbackMap::iterator, UidCallbackMap::iterator>;
}  // namespace nfc
}  // namespace OHOS

#endif  // !FOREGROUND_UTILS_H
