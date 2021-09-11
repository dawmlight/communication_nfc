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
#ifndef SYNCHRONIZE_EVENT_H
#define SYNCHRONIZE_EVENT_H
#include <pthread.h>

namespace OHOS{
namespace nfc {

static const int MILLISECOND_PER_SECOND = 1000;
static const int NANOSECOND_PER_MILLISECOND = 1000000;
static const int NANOSECOND_PER_SECOND = 1000000000;

class SynchronizeEvent {
public:
    SynchronizeEvent();
    ~SynchronizeEvent();

    void Start();
    void End();
    void Wait();
    bool Wait(long ms);
    void NotifyOne();

private:
    pthread_mutex_t mLock;
    pthread_cond_t mCond;
};

class SynchronizeGuard {
public:
    explicit SynchronizeGuard(SynchronizeEvent& event) : mSyncEvent(event)
    {
        event.Start();
    };

    ~SynchronizeGuard()
    {
        mSyncEvent.End();
    };

private:
    SynchronizeEvent& mSyncEvent;
};
}  // namespace nfc
}  // namespace OHOS
#endif /* SYNCHRONIZE_EVENT_H */