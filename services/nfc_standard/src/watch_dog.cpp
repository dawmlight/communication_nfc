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
#include "watch_dog.h"

#include <chrono>

#include "idevice_host.h"
#include "loghelper.h"

using namespace std;
namespace OHOS{
namespace nfc {
WatchDog::WatchDog(const std::string& threadName, int timeout, std::weak_ptr<IDeviceHost> deviceHost)
    : mThreadName_(threadName), mTimeout_(timeout), mCanceled_(false), mThread_(nullptr), mDeviceHost_(deviceHost)
{
}

WatchDog::~WatchDog()
{
    if (mThread_ && mThread_->joinable()) {
        mCV_.notify_one();
        mThread_->join();
    }
}

void WatchDog::MainLoop()
{
    std::unique_lock<mutex> lock(mtx_);
    mCV_.wait_for(lock, chrono::milliseconds(mTimeout_), [this] { return mCanceled_; });
    if (mCanceled_) {
        return;
    }
    // If Routing Wake Lock is held, Routing Wake Lock release. Watchdog triggered, release lock before aborting.
    InfoLog("Watchdog triggered, aborting.");
    mDeviceHost_.lock()->Abort();
}

void WatchDog::Run()
{
    mThread_ = std::make_unique<std::thread>(&WatchDog::MainLoop, this);
}

void WatchDog::Cancel()
{
    std::unique_lock<mutex> lock(mtx_);
    mCanceled_ = true;
    mCV_.notify_one();
}
}  // namespace nfc
}  // namespace OHOS