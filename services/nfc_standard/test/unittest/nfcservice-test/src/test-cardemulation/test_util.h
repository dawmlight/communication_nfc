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

#ifndef SERVICE_CARD_EMULATION_TEST_UTIL_H
#define SERVICE_CARD_EMULATION_TEST_UTIL_H

#include <memory>
#include <string>

#include "card_emulation_service_info.h"
#include "element_name.h"
namespace OHOS::nfc::cardemulation {
namespace test {
class Util {
public:
    static void CardEmulationServiceInfoSetName(std::shared_ptr<CardEmulationServiceInfo>& info,
                                                std::string abilityName,
                                                std::string bundleName = "test",
                                                std::string deviceId = "dev0");
    static OHOS::AppExecFwk::ElementName CreateElementName(std::string abilityName,
                                                           std::string bundleName = "test",
                                                           std::string deviceId = "");

    static OHOS::AppExecFwk::ElementName CreateEmptyElementName();
};
}  // namespace test
}  // namespace OHOS::nfc::cardemulation

#endif  // !SERVICE_CARD_EMULATION_TEST_UTIL_H
