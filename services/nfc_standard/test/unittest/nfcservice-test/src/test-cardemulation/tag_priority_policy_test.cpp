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

#include "tag_priority_policy.h"

#include <gtest/gtest.h>

#include "aid_set.h"
#include "card_emulation_def.h"
#include "card_emulation_service_info.h"
#include "test_util.h"

using namespace OHOS::nfc::cardemulation;
using namespace OHOS::nfc::cardemulation::test;
namespace OHOS::nfc::cardemulation::test{
static std::string kSecureType = CARDEMULATION_SERVICE_TYPE_SECURE;
static std::string kNormalType = CARDEMULATION_SERVICE_TYPE_NORMAL;
static const std::vector<std::string> kSupportedLocations = {"host", "eSE1"};

/**
* @tc.number:
* @tc.name  :
* @tc.desc  :
*/
TEST(TagPriorityPolicy, plan_case1)
{
    // conflict with as2 type
    auto as1 = AidSet::FromRawString({"A0102030405060*"});
    as1->SetType(kSecureType);

    auto app1 = std::make_shared<CardEmulationServiceInfo>("host");
    app1->SetName(Util::CreateElementName("app1"));
    app1->AddAidset(std::move(as1));

    auto app2 = std::make_shared<CardEmulationServiceInfo>("eSE1");
    app2->SetName(Util::CreateElementName("app2"));
    auto as2 = AidSet::FromRawString({"F0102030405001", "F0102030405002"});
    as2->SetType(kSecureType);
    app2->AddAidset(std::move(as2));

    // not surpport EE
    auto app3 = std::make_shared<CardEmulationServiceInfo>("UICC");
    app3->SetName(Util::CreateElementName("app3"));
    auto as3 = AidSet::FromRawString({"F0102030405060*"});
    as3->SetType(kNormalType);
    app3->AddAidset(std::move(as3));

    TagPriorityPolicy policy(kSupportedLocations, AID_ROUTING_MODE_MASK_PREFIX, kSecureType);
    auto entries = policy.PlanRoutingTable({app1, app2, app3}, app2, app3);
    ASSERT_EQ(entries.size(), 1u);
    EXPECT_EQ(entries[0]->GetOwner().lock(), app2);
}

/**
* @tc.number:
* @tc.name  :
* @tc.desc  :
*/
TEST(TagPriorityPolicy, plan_case2)
{
    auto createAppInfo = [](std::string location) { return std::make_shared<CardEmulationServiceInfo>(location); };

    auto as1 = AidSet::FromRawString({"A010203040506070"});
    as1->SetType(kNormalType);

    auto app1 = createAppInfo("host");
    app1->SetName(Util::CreateElementName("app1"));
    app1->AddAidset(std::move(as1));

    auto app2 = createAppInfo("eSE1");
    app2->SetName(Util::CreateElementName("app2"));
    auto as2 = AidSet::FromRawString({"F0102030405001", "F0102030405002"});
    as2->SetType(kSecureType);
    app2->AddAidset(std::move(as2));

    auto app3 = createAppInfo("UICC");
    app3->SetName(Util::CreateElementName("app3"));
    auto as3 = AidSet::FromRawString({"F0102030405060*"});
    as3->SetType(kNormalType);
    app3->AddAidset(std::move(as3));

    TagPriorityPolicy policy(kSupportedLocations, AID_ROUTING_MODE_MASK_PREFIX, kSecureType);
    auto entries = policy.PlanRoutingTable({app1, app2, app3}, app2, app3);
    ASSERT_EQ(entries.size(), 2u);
    EXPECT_EQ(entries[0]->GetOwner().lock(), app2);
    EXPECT_EQ(entries[1]->GetOwner().lock(), app1);
}

/**
* @tc.number:
* @tc.name  :
* @tc.desc  :
*/
TEST(TagPriorityPolicy, plan_case3)
{
    // aid conflicts with primary
    auto as1 = AidSet::FromRawString({"F01020304050*"});
    as1->SetType(kNormalType);

    auto app1 = std::make_shared<CardEmulationServiceInfo>("host");
    app1->SetName(Util::CreateElementName("app1"));
    app1->AddAidset(std::move(as1));

    auto app2 = std::make_shared<CardEmulationServiceInfo>("host");
    app2->SetName(Util::CreateElementName("app2"));
    auto as2 = AidSet::FromRawString({"F0102030405001", "F0102030405002"});
    as2->SetType(kSecureType);
    app2->AddAidset(std::move(as2));

    auto app3 = std::make_shared<CardEmulationServiceInfo>("host");
    app3->SetName(Util::CreateElementName("app3"));
    auto as3 = AidSet::FromRawString({"F0102030405060*"});
    as3->SetType(kNormalType);
    app3->AddAidset(std::move(as3));

    // type conflicts with primary
    auto app4 = std::make_shared<CardEmulationServiceInfo>("host");
    app4->SetName(Util::CreateElementName("app4"));
    auto as4 = AidSet::FromRawString({"F010203040506070"});
    as4->SetType(kSecureType);
    app4->AddAidset(std::move(as4));

    TagPriorityPolicy policy(kSupportedLocations, AID_ROUTING_MODE_MASK_PREFIX, kSecureType);
    auto entries = policy.PlanRoutingTable({app1, app2, app3, app4}, app2, app3);
    ASSERT_EQ(entries.size(), 2u);
    EXPECT_EQ(entries[0]->GetOwner().lock(), app2);
    EXPECT_EQ(entries[1]->GetOwner().lock(), app3);
}

/**
* @tc.number:
* @tc.name  :
* @tc.desc  :
*/
TEST(TagPriorityPolicy, plan_case4)
{
    auto as1 = AidSet::FromRawString({"AA1020304050*"});
    as1->SetType(kNormalType);

    auto app1 = std::make_shared<CardEmulationServiceInfo>("host");
    app1->SetName(Util::CreateElementName("app1"));
    app1->AddAidset(std::move(as1));

    auto app2 = std::make_shared<CardEmulationServiceInfo>("host");
    app2->SetName(Util::CreateElementName("app2"));
    auto as2 = AidSet::FromRawString({"F0102030405001", "F0102030405002*"});
    as2->SetType(kSecureType);
    app2->AddAidset(std::move(as2));

    auto app3 = std::make_shared<CardEmulationServiceInfo>("host");
    app3->SetName(Util::CreateElementName("app3"));
    auto as3 = AidSet::FromRawString({"F0102030405060*"});
    as3->SetType(kNormalType);
    app3->AddAidset(std::move(as3));

    // conflict with as2
    auto app4 = std::make_shared<CardEmulationServiceInfo>("host");
    app4->SetName(Util::CreateElementName("app4"));
    auto as4 = AidSet::FromRawString({"F0102030405002"});
    as4->SetType(kNormalType);
    app4->AddAidset(std::move(as4));

    TagPriorityPolicy policy(kSupportedLocations, AID_ROUTING_MODE_MASK_PREFIX, kSecureType);
    auto entries = policy.PlanRoutingTable({app1, app2, app3, app4}, app2, app3);
    ASSERT_EQ(entries.size(), 3u);
    EXPECT_EQ(entries[0]->GetOwner().lock(), app2);
    EXPECT_EQ(entries[1]->GetOwner().lock(), app3);
    EXPECT_EQ(entries[2]->GetOwner().lock(), app1);
}

/**
* @tc.number:
* @tc.name  :
* @tc.desc  :
*/
TEST(TagPriorityPolicy, plan_primary_is_null)
{
    auto as1 = AidSet::FromRawString({"AA1020304050*"});
    as1->SetType(kNormalType);

    auto app1 = std::make_shared<CardEmulationServiceInfo>("host");
    app1->SetName(Util::CreateElementName("app1"));
    app1->AddAidset(std::move(as1));

    auto app2 = std::make_shared<CardEmulationServiceInfo>("host");
    app2->SetName(Util::CreateElementName("app2"));
    auto as2 = AidSet::FromRawString({"F0102030405060", "F0102030405002*"});
    as2->SetType(kNormalType);
    app2->AddAidset(std::move(as2));

    auto app3 = std::make_shared<CardEmulationServiceInfo>("host");
    app3->SetName(Util::CreateElementName("app3"));
    auto as3 = AidSet::FromRawString({"F01020304050*"});
    as3->SetType(kNormalType);
    app3->AddAidset(std::move(as3));

    // conflict with as2
    auto app4 = std::make_shared<CardEmulationServiceInfo>("host");
    app4->SetName(Util::CreateElementName("app4"));
    auto as4 = AidSet::FromRawString({"F0102030405002"});
    as4->SetType(kNormalType);
    app4->AddAidset(std::move(as4));

    TagPriorityPolicy policy(kSupportedLocations, AID_ROUTING_MODE_MASK_PREFIX, kSecureType);
    auto entries = policy.PlanRoutingTable({app1, app2, app3, app4}, nullptr, app2);
    ASSERT_EQ(entries.size(), 4u);
    EXPECT_EQ(entries[0]->GetOwner().lock(), app2);
    EXPECT_EQ(entries[1]->GetOwner().lock(), app1);
    EXPECT_EQ(entries[2]->GetOwner().lock(), app3);
    EXPECT_EQ(entries[3]->GetOwner().lock(), app4);
}
}
