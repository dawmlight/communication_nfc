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

#ifndef NFC_SA_UNIT_TEST_H
#define NFC_SA_UNIT_TEST_H

#include <iostream>
#include <gtest/gtest.h>
#include "common_event_manager.h"
#include "nfc_manager_service.h"

namespace OHOS {
namespace Nfc {
class NfcSaUnitTest : public testing::Test {
public:
    void SetUp();
    void TearDown();
    int32_t SetCommonEventSubscriber(int32_t& state);

    sptr<NfcManagerService> nfcSaSvr_;
    std::shared_ptr<Notification::CommonEventSubscriber> subscriber_;
};
} // namespace Nfc
} // namespace OHOS

#endif