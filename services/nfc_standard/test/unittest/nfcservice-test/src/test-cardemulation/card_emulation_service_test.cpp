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

#include "card_emulation_service.h"

#include <gtest/gtest.h>

#include "aid_routing_adapter.h"
#include "aid_routing_planner.h"
#include "card_emulation_def.h"
#include "card_emulation_error.h"
#include "card_emulation_event_handler.h"
#include "card_emulation_service_info_manager.h"
#include "card_emulation_util.h"
#include "icard_emulation_device_host.h"
#include "test_util.h"

using namespace OHOS::nfc::cardemulation;
using namespace OHOS::nfc::cardemulation::test;
namespace OHOS::nfc::cardemulation::test{
/**
* @tc.number:
* @tc.name  :
* @tc.desc  :
*/
TEST(CardEmulationService, Init_Failed)
{
    CardEmulationService cs({});
    cs.OnUserSwitched(0);

    EXPECT_EQ(cs.OnUserSwitched(0), ERR_CARD_EMULATION_SERVICE_NOT_INIT);

    EXPECT_EQ(cs.RegisterRemoteObject(0, Util::CreateEmptyElementName(), nullptr), ERR_CARD_EMULATION_SERVICE_NOT_INIT);
    EXPECT_EQ(cs.AddAidsForService(0, Util::CreateEmptyElementName(), "", {}), ERR_CARD_EMULATION_SERVICE_NOT_INIT);
    EXPECT_EQ(cs.RemoveAidsForService(0, {}, {}), ERR_CARD_EMULATION_SERVICE_NOT_INIT);

    EXPECT_EQ(cs.MarkOffHostForService(0, {}, {}), ERR_CARD_EMULATION_SERVICE_NOT_INIT);
    EXPECT_EQ(cs.UnmarkOffHostForService(0, {}), ERR_CARD_EMULATION_SERVICE_NOT_INIT);
    EXPECT_EQ(cs.SupportsAidPrefix(), false);
    EXPECT_EQ(cs.SetPreferredService(0, {}), ERR_CARD_EMULATION_SERVICE_NOT_INIT);
    EXPECT_EQ(cs.UnsetPreferredService(0), ERR_CARD_EMULATION_SERVICE_NOT_INIT);
    EXPECT_EQ(cs.IsDefaultServiceForAid(0, {}, {}), false);
    EXPECT_EQ(cs.IsDefaultServiceForType(0, {}, {}), false);
    EXPECT_EQ(cs.GetAidsForService(0, {}, {}).empty(), true);
    EXPECT_EQ(cs.SetPrimaryServiceForType(0, {}, {}), ERR_CARD_EMULATION_SERVICE_NOT_INIT);

    EXPECT_EQ(cs.OnHCEActivated(), ERR_NULL_POINTER);
    EXPECT_EQ(cs.OnHCEData(nullptr, 0), ERR_NULL_POINTER);
    EXPECT_EQ(cs.OnHCEDeactivated(), ERR_NULL_POINTER);
    EXPECT_EQ(cs.OnOffHostTransaction(), ERR_NULL_POINTER);
    cs.Deinit();
}
}