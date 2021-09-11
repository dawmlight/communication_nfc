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

#include <gtest/gtest.h>

#include <algorithm>
#include <string>

#include "aid_routing_adapter.h"
#include "aid_routing_planner.h"
#include "aid_routing_policy_factory.h"
#include "aid_routing_table.h"
#include "aid_set.h"
#include "card_emulation_def.h"
#include "card_emulation_device_host_mock.h"
#include "card_emulation_error.h"
#include "card_emulation_service_info.h"
#include "card_emulation_util.h"
#include "test_util.h"

using namespace OHOS::nfc::cardemulation;
using namespace OHOS::nfc::cardemulation::test;
using namespace testing;
namespace OHOS::nfc::cardemulation::test{
static const std::string kSecureType = "secure";
static const std::string kNormalType = "other";
static const std::vector<std::string> kSupportedLocations = {"host", "eSE1"};

/**
* @tc.number:
* @tc.name  :
* @tc.desc  :
*/
TEST(IT_AidRoutingPlanner, plan)
{
    std::string aid1("F0102030405060");
    std::string aid2("A010203040506070");

    std::string aid3("F0102030405060");
    std::string aid4(aid3 + "*");
    auto as1 = AidSet::FromRawString({aid1});
    as1->SetType(kNormalType);
    auto as2 = AidSet::FromRawString({aid2});
    as2->SetType(kSecureType);

    auto as3 = AidSet::FromRawString({aid4});
    as3->SetType(kNormalType);

    std::shared_ptr<CardEmulationServiceInfo> info1 = std::make_shared<CardEmulationServiceInfo>("host");
    info1->SetName(Util::CreateElementName("info1"));
    info1->AddAidset(std::move(as1));
    std::shared_ptr<CardEmulationServiceInfo> info2 = std::make_shared<CardEmulationServiceInfo>("eSE1");
    info2->SetName(Util::CreateElementName("info2"));
    info2->AddAidset(std::move(as2));

    std::shared_ptr<CardEmulationServiceInfo> info3 = std::make_shared<CardEmulationServiceInfo>("host");
    info3->SetName(Util::CreateElementName("info3"));
    info3->AddAidset(std::move(as3));
    auto dh = std::make_shared<DeviceHostCEMock>();
    size_t capacity = 512;
    EXPECT_CALL(*dh, GetAidRoutingTableSize()).WillRepeatedly(testing::Return(capacity));
    EXPECT_CALL(*dh, GetRemainRoutingTableSize()).WillRepeatedly(testing::Return(capacity - 4));
    EXPECT_CALL(*dh, ClearRouting()).WillRepeatedly(testing::Return(0));
    EXPECT_CALL(*dh, AddAidRouting(_, _, _)).WillRepeatedly(testing::Return(true));
    EXPECT_CALL(*dh, CommitRouting()).WillRepeatedly(testing::Return(false));
    EXPECT_CALL(*dh, GetDefaultRoute()).WillRepeatedly(testing::Return(0));
    EXPECT_CALL(*dh, GetDefaultOffHostRoute()).WillRepeatedly(testing::Return(0));
    EXPECT_CALL(*dh, GetOffHostUiccRoute()).WillRepeatedly(testing::Return(std::vector<int>{0}));
    EXPECT_CALL(*dh, GetOffHostEseRoute()).WillRepeatedly(testing::Return(std::vector<int>{0}));
    EXPECT_CALL(*dh, GetAidMatchingMode()).WillRepeatedly(testing::Return(AID_ROUTING_MODE_MASK_PREFIX));

    auto controller = std::make_shared<AidRoutingAdapter>(dh);
    /*std::vector<RoutingEntry>{{HexStrToBytes(aid2), 1, 0}, {HexStrToBytes(aid3), 1, 1}}, 512;*/
    auto mode = controller->GetAidRoutingMode();
    auto policy = RoutingPolicyFactory().CreateRoutingPolicy(kSupportedLocations, mode, kSecureType);
    AidRoutingPlanner planner(std::move(policy), controller);
    EXPECT_EQ(ERR_COMMIT_ROUTING_FAILED, planner.OnCeServiceChanged({info1, info2, info3}, info2, info3));
}
}