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

#ifndef NFC_CARD_EMULATION_ELEMENT_NAME_UTIL_H
#define NFC_CARD_EMULATION_ELEMENT_NAME_UTIL_H

#include "element_name.h"
#include "ipc_skeleton.h"
namespace OHOS::nfc::cardemulation {
class ElementNameUtil {
public:
    static OHOS::AppExecFwk::ElementName CreateElementName(const std::string& abilityName,
                                                           const std::string& bundleName,
                                                           const std::string& deviceId = "")
    {
        return OHOS::AppExecFwk::ElementName(deviceId.empty() ? OHOS::IPCSkeleton::GetLocalDeviceID() : deviceId,
                                             bundleName,
                                             abilityName);
    }
    static bool ElementNameEquals(const std::shared_ptr<OHOS::AppExecFwk::ElementName>& l,
                                  const OHOS::AppExecFwk::ElementName& r)
    {
        if (!l) {
            return false;
        }
        return (*l) == r;
    }
    static bool ElementNameEquals(const std::weak_ptr<OHOS::AppExecFwk::ElementName>& l,
                                  const OHOS::AppExecFwk::ElementName& r)
    {
        return ElementNameEquals(l.lock(), r);
    }
    static bool IsElementNameEmpty(const OHOS::AppExecFwk::ElementName& name)
    {
        static OHOS::AppExecFwk::ElementName sEmptyElementName{};
        return name == sEmptyElementName;
    }
    static bool IsElementNameEmpty(const std::shared_ptr<OHOS::AppExecFwk::ElementName>& name)
    {
        if (!name) {
            return true;
        }
        return IsElementNameEmpty(*name);
    }
};
}  // namespace OHOS::nfc::cardemulation
#endif
