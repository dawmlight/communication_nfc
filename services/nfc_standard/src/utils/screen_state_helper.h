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
#ifndef SCREEN_STATE_HELPER_H
#define SCREEN_STATE_HELPER_H

#include <memory>
#include <string>

namespace osal {
class Context;
class PowerManager;
class KeyguardManager;
}  // namespace osal

namespace OHOS {
namespace nfc {
enum ScreenState {
    SCREEN_STATE_UNKNOWN = 0x00,
    SCREEN_STATE_OFF_UNLOCKED = 0x01,
    SCREEN_STATE_OFF_LOCKED = 0x02,
    SCREEN_STATE_ON_LOCKED = 0x04,
    SCREEN_STATE_ON_UNLOCKED = 0x08,
    // Polling mask
    SCREEN_POLLING_TAG_MASK = 0x10,
    SCREEN_POLLING_READER_MASK = 0x40
};

class ScreenStateHelper {
public:
    explicit ScreenStateHelper(std::weak_ptr<osal::Context> ctx);
    ~ScreenStateHelper();

    int CheckScreenState() const;

private:
    std::weak_ptr<osal::PowerManager> mPowerManager_{};
    std::weak_ptr<osal::KeyguardManager> mKeyguardManager_{};
};
}  // namespace nfc
}  // namespace OHOS

#endif  // !SCREEN_STATE_HELPER_H