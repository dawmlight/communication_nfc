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

#include "card_emulation_device_host.h"

#include <gtest/gtest.h>

#include "card_emulation_error.h"

using namespace OHOS::nfc::cardemulation;
namespace OHOS::nfc::cardemulation::test{
/**
* @tc.number:
* @tc.name  :
* @tc.desc  :
*/
TEST(CardEmulationDeviceHost, NoInit)
{
    CardEmulationDeviceHost dh({});
    EXPECT_FALSE(dh.AddAidRouting({}, 0, 0));
    EXPECT_FALSE(dh.RemoveAidRouting({}));
    EXPECT_FALSE(dh.CommitRouting());
    EXPECT_EQ(dh.GetAidRoutingTableSize(), 0);
    EXPECT_EQ(dh.GetDefaultOffHostRoute(), 0);
    EXPECT_EQ(dh.GetOffHostUiccRoute().size(), 0u);
    EXPECT_EQ(dh.GetOffHostEseRoute().size(), 0u);
    EXPECT_EQ(dh.GetAidMatchingMode(), 0);
    EXPECT_EQ(dh.GetDefaultIsoDepRouteDestination(), 0);
    EXPECT_EQ(dh.GetExtendedLengthApdusSupported(), false);
    EXPECT_EQ(dh.SendData({}), false);
}
}