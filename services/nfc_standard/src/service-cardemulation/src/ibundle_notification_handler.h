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

#ifndef CARD_EMULATION_IBUNDLE_NOTIFICATION_HANDLER_H
#define CARD_EMULATION_IBUNDLE_NOTIFICATION_HANDLER_H

#include <string>
namespace OHOS {
namespace nfc::cardemulation {
class IBundleNotificationHandler {
public:
    virtual ~IBundleNotificationHandler() = default;

    virtual void OnInstall(const std::string& bundleName) = 0;
    virtual void OnUninstall(const std::string& bundleName) = 0;
};
}  // namespace nfc::cardemulation
}  // namespace OHOS

#endif  // !CARD_EMULATION_IBUNDLE_NOTIFICATION_HANDLER_H
