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
#ifndef WATCH_DOG_THREAD_H
#define WATCH_DOG_THREAD_H

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <string>
#include <thread>

namespace OHOS{
namespace nfc::ncibal {
class IDeviceHost;
}  // namespace nfc::ncibal
using nfc::ncibal::IDeviceHost;

namespace nfc {
class WatchDog final {
public:
    WatchDog(const std::string& threadName, int timeout, std::weak_ptr<IDeviceHost> deviceHost);
    ~WatchDog();
    void Cancel();
    void Run();

private:
    void MainLoop();

private:
    std::mutex mtx_{};
    std::condition_variable mCV_{};
    std::string mThreadName_{""};
    int mTimeout_{0};
    volatile bool mCanceled_{false};
    std::unique_ptr<std::thread> mThread_{};

    std::weak_ptr<IDeviceHost> mDeviceHost_;
};
}  // namespace nfc
}  // namespace OHOS
#endif  // !WATCH_DOG_THREAD_H
