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

#include "aid_routing_planner.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <algorithm>
#include <string>

#include "aid_routing_adapter.h"
#include "aid_routing_policy_factory.h"
#include "aid_routing_table.h"
#include "aid_set.h"
#include "card_emulation_def.h"
#include "card_emulation_device_host_mock.h"
#include "card_emulation_error.h"
#include "card_emulation_service_info.h"
#include "card_emulation_util.h"
#include "element_name.h"
#include "test_util.h"

using namespace testing;
using namespace OHOS::nfc::cardemulation;
using namespace OHOS::nfc::cardemulation::test;
namespace OHOS::nfc::cardemulation::test{
static const std::string kSecureType = CARDEMULATION_SERVICE_TYPE_SECURE;
static const std::string kNormalType = CARDEMULATION_SERVICE_TYPE_NORMAL;
static const std::vector<std::string> kSupportedLocations = {"host", "eSE1"};

class AidRoutingAdapterStub : public AidRoutingAdapter {
public:
    AidRoutingAdapterStub(const std::vector<RoutingEntry>& entries,
                          size_t capacity,
                          std::shared_ptr<ICardEmulationDeviceHost> dh);
    int AddAidRoutingEntry(const std::vector<unsigned char>& aid, int target, int aidType) override;
    virtual int CommitAidRouting();

private:
private:
    const std::vector<RoutingEntry> entries_;
    std::vector<RoutingEntry> actualEntries_{};
    size_t reserved_;
};

AidRoutingAdapterStub::AidRoutingAdapterStub(const std::vector<RoutingEntry>& entries,
                                             size_t capacity,
                                             std::shared_ptr<ICardEmulationDeviceHost> dh)
  : AidRoutingAdapter(dh),
    entries_(entries),
    reserved_(capacity)
{
}

int AidRoutingAdapterStub::AddAidRoutingEntry(const std::vector<unsigned char>& aid, int target, int aidType)
{
    auto re = RoutingEntry();
    re.aid_ = aid;
    re.route_ = target;
    re.aidPattern_ = aidType;
    actualEntries_.emplace_back(re);
    reserved_ -= aid.size() + AID_HEAD_LENGTH;

    return reserved_;
}

int AidRoutingAdapterStub::CommitAidRouting()
{
    EXPECT_EQ(entries_.size(), actualEntries_.size());
    for (auto& entry : entries_) {
        auto pos = std::find_if(std::begin(actualEntries_),
                                std::end(actualEntries_),
                                [&entry](decltype(actualEntries_)::reference r) {
                                    return std::equal(std::begin(entry.aid_), std::end(entry.aid_), std::begin(r.aid_));
                                });
        EXPECT_TRUE(pos != std::end(actualEntries_));
        if (pos != std::end(actualEntries_)) {
            EXPECT_EQ(entry.route_, pos->route_);
            EXPECT_EQ(entry.aidPattern_, pos->aidPattern_);
        }
    }
    return 0;
}
/**
 * @tc.number:
 * @tc.name  :
 * @tc.desc  :
 */
TEST(AidRoutingPlanner, plan_Failed)
{
    AidRoutingPlanner planner0({}, {});
    EXPECT_EQ(planner0.OnCeServiceChanged({}, nullptr, nullptr), ERR_NULL_POINTER);

    auto dh = std::make_shared<DeviceHostCEMock>();
    EXPECT_CALL(*dh, GetAidRoutingTableSize()).WillRepeatedly(testing::Return(0));
    EXPECT_CALL(*dh, GetDefaultRoute()).WillRepeatedly(testing::Return(0));
    EXPECT_CALL(*dh, GetDefaultOffHostRoute())
        .WillRepeatedly(testing::Return(2));  // GetOffHostEseRoute, GetOffHostUiccRoute 列表中的一个值.
    //  GetOffHostEseRoute, GetOffHostUiccRoute  和 kSupportedLocations 一致.
    // 比如: kSupportedLocations 仅支持 host, eSE 时, GetOffHostUiccRoute返回空列表
    EXPECT_CALL(*dh, GetOffHostUiccRoute()).WillRepeatedly(testing::Return(std::vector<int>{}));
    EXPECT_CALL(*dh, GetOffHostEseRoute()).WillRepeatedly(testing::Return(std::vector<int>{2}));
    EXPECT_CALL(*dh, GetAidMatchingMode()).WillRepeatedly(testing::Return(AID_ROUTING_MODE_MASK_PREFIX));
    auto controller = std::make_shared<AidRoutingAdapterStub>(std::vector<RoutingEntry>{}, 0, dh);
    auto mode = controller->GetAidRoutingMode();
    auto policy = RoutingPolicyFactory().CreateRoutingPolicy(kSupportedLocations, mode, kSecureType);
    AidRoutingPlanner planner(std::move(policy), controller);

    planner.Init();

    EXPECT_EQ(planner.OnCeServiceChanged({}, nullptr, nullptr), ERR_ROUTING_TABLE_NOT_ENOUGH_CAPACITY);
}

/**
 * @tc.number:
 * @tc.name  :
 * @tc.desc  :
 */
TEST(AidRoutingPlanner, plan)
{
    std::string aid1("F0102030405060");
    std::string aid2("A010203040506070");

    std::string aid3("B0102030405060");
    std::string aid4(aid3 + "*");
    auto as1 = AidSet::FromRawString({aid1});
    as1->SetType(kSecureType);
    auto as2 = AidSet::FromRawString({aid2});
    as2->SetType(kNormalType);

    auto as3 = AidSet::FromRawString({aid4});
    as3->SetType(kSecureType);

    std::shared_ptr<CardEmulationServiceInfo> info1 = std::make_shared<CardEmulationServiceInfo>("host");
    Util::CardEmulationServiceInfoSetName(info1, "info1");
    info1->AddAidset(std::move(as1));
    std::shared_ptr<CardEmulationServiceInfo> info2 = std::make_shared<CardEmulationServiceInfo>("eSE1");
    Util::CardEmulationServiceInfoSetName(info2, "info2");
    info2->AddAidset(std::move(as2));
    info2->AddAidset(std::move(as3));
    auto dh = std::make_shared<DeviceHostCEMock>();
    EXPECT_CALL(*dh, GetAidRoutingTableSize()).WillRepeatedly(testing::Return(512));
    EXPECT_CALL(*dh, GetDefaultRoute()).WillRepeatedly(testing::Return(0));

    // GetOffHostEseRoute, GetOffHostUiccRoute 列表中的一个值.
    //  GetOffHostEseRoute, GetOffHostUiccRoute  和 kSupportedLocations 一致.
    // 比如: kSupportedLocations 仅支持 host, eSE 时, GetOffHostUiccRoute返回空列表
    EXPECT_CALL(*dh, GetDefaultOffHostRoute()).WillRepeatedly(testing::Return(0));
    // 不支持 SIM
    EXPECT_CALL(*dh, GetOffHostUiccRoute()).WillRepeatedly(testing::Return(std::vector<int>{}));
    // 支持一个 eSE, 对应 route: 2
    EXPECT_CALL(*dh, GetOffHostEseRoute()).WillRepeatedly(testing::Return(std::vector<int>{2}));
    EXPECT_CALL(*dh, GetAidMatchingMode()).WillRepeatedly(testing::Return(AID_ROUTING_MODE_MASK_PREFIX));
    EXPECT_CALL(*dh, AddAidRouting(_, _, _)).WillRepeatedly(Return(true));
    EXPECT_CALL(*dh, CommitRouting()).WillRepeatedly(testing::Return(true));
    auto controller = std::make_shared<AidRoutingAdapterStub>(
        std::vector<RoutingEntry>{{HexStrToBytes(aid1), 0, AidTypeToInt(AidType::EXACT)},
                                  {HexStrToBytes(aid2), 2, AidTypeToInt(AidType::EXACT)},
                                  {HexStrToBytes(aid3), 2, AidTypeToInt(AidType::PREFIX)}},
        512,
        dh);
    auto mode = controller->GetAidRoutingMode();
    auto policy = RoutingPolicyFactory().CreateRoutingPolicy(kSupportedLocations, mode);
    AidRoutingPlanner planner(std::move(policy), controller);
    planner.OnCeServiceChanged({info1, info2}, info2, nullptr);
}

/**
 * @tc.number:
 * @tc.name  :
 * @tc.desc  :
 */
TEST(AidRoutingPlanner, plan_overflow_0)
{
    std::string aid1("F0102030405060");
    std::string aid2("A010203040506070");
    std::string aid3("B0102030405060*");
    auto as1 = AidSet::FromRawString({aid1, aid2, aid3});
    as1->SetType(kSecureType);

    std::shared_ptr<CardEmulationServiceInfo> info1 = std::make_shared<CardEmulationServiceInfo>(NFC_EE_ESE);
    Util::CardEmulationServiceInfoSetName(info1, "info1");
    info1->AddAidset(std::move(as1));

    auto dh = std::make_shared<DeviceHostCEMock>();
    EXPECT_CALL(*dh, GetAidRoutingTableSize()).WillRepeatedly(testing::Return(16));
    EXPECT_CALL(*dh, GetDefaultRoute()).WillRepeatedly(testing::Return(0));

    // GetOffHostEseRoute, GetOffHostUiccRoute 列表中的一个值.
    //  GetOffHostEseRoute, GetOffHostUiccRoute  和 kSupportedLocations 一致.
    // 比如: kSupportedLocations 仅支持 host, eSE 时, GetOffHostUiccRoute返回空列表
    EXPECT_CALL(*dh, GetDefaultOffHostRoute()).WillRepeatedly(testing::Return(0));
    // 不支持 SIM
    EXPECT_CALL(*dh, GetOffHostUiccRoute()).WillRepeatedly(testing::Return(std::vector<int>{}));
    // 支持一个 eSE, 对应 route: 2
    EXPECT_CALL(*dh, GetOffHostEseRoute()).WillRepeatedly(testing::Return(std::vector<int>{2}));
    EXPECT_CALL(*dh, GetAidMatchingMode()).WillRepeatedly(testing::Return(AID_ROUTING_MODE_MASK_PREFIX));
    EXPECT_CALL(*dh, AddAidRouting(_, _, _)).WillRepeatedly(Return(true));
    EXPECT_CALL(*dh, CommitRouting()).WillRepeatedly(testing::Return(true));
    auto controller = std::make_shared<AidRoutingAdapterStub>(std::vector<RoutingEntry>{}, 16, dh);
    auto mode = controller->GetAidRoutingMode();
    auto policy = RoutingPolicyFactory().CreateRoutingPolicy(kSupportedLocations, mode);
    AidRoutingPlanner planner(std::move(policy), controller);
    planner.OnCeServiceChanged({info1}, info1, nullptr);
}

/**
 * @tc.number:
 * @tc.name  :
 * @tc.desc  :
 */
TEST(AidRoutingPlanner, plan_overflow_1)
{
    std::string aid1("F0102030405060");
    std::string aid2("A010203040506070");
    std::string aid3("B01020304050607080");
    auto as1 = AidSet::FromRawString({aid1});
    as1->SetType(kSecureType);
    auto as2 = AidSet::FromRawString({aid2, aid3});
    as2->SetType(kNormalType);

    std::shared_ptr<CardEmulationServiceInfo> info1 = std::make_shared<CardEmulationServiceInfo>(NFC_EE_ESE);
    Util::CardEmulationServiceInfoSetName(info1, "info1");
    info1->AddAidset(std::move(as1));
    std::shared_ptr<CardEmulationServiceInfo> info2 = std::make_shared<CardEmulationServiceInfo>(NFC_EE_HOST);
    Util::CardEmulationServiceInfoSetName(info2, "info2");
    info2->AddAidset(std::move(as2));

    auto dh = std::make_shared<DeviceHostCEMock>();
    EXPECT_CALL(*dh, GetAidRoutingTableSize()).WillRepeatedly(testing::Return(16));
    EXPECT_CALL(*dh, GetDefaultRoute()).WillRepeatedly(testing::Return(0));

    // GetOffHostEseRoute, GetOffHostUiccRoute 列表中的一个值.
    //  GetOffHostEseRoute, GetOffHostUiccRoute  和 kSupportedLocations 一致.
    // e.g.: kSupportedLocations only surpport host, eSE 时, GetOffHostUiccRoute return empty
    EXPECT_CALL(*dh, GetDefaultOffHostRoute()).WillRepeatedly(testing::Return(0));
    // not surpported: SIM
    EXPECT_CALL(*dh, GetOffHostUiccRoute()).WillRepeatedly(testing::Return(std::vector<int>{}));
    // eSE, route: 2
    EXPECT_CALL(*dh, GetOffHostEseRoute()).WillRepeatedly(testing::Return(std::vector<int>{2}));
    EXPECT_CALL(*dh, GetAidMatchingMode()).WillRepeatedly(testing::Return(AID_ROUTING_MODE_MASK_PREFIX));
    EXPECT_CALL(*dh, AddAidRouting(_, _, _)).WillRepeatedly(Return(true));
    EXPECT_CALL(*dh, CommitRouting()).WillRepeatedly(testing::Return(true));
    auto controller = std::make_shared<AidRoutingAdapterStub>(
        std::vector<RoutingEntry>{{HexStrToBytes(aid1), 2, AidTypeToInt(AidType::EXACT)}},
        16,
        dh);
    auto mode = controller->GetAidRoutingMode();
    auto policy = RoutingPolicyFactory().CreateRoutingPolicy(kSupportedLocations, mode);
    AidRoutingPlanner planner(std::move(policy), controller);
    planner.OnCeServiceChanged({info1, info2}, info1, nullptr);
}
}