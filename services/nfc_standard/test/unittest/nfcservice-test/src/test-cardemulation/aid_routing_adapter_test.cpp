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

#include "aid_routing_adapter.h"

#include <gtest/gtest.h>

#include <cstdio>
#include <memory>

#include "card_emulation_def.h"
#include "card_emulation_device_host_mock.h"
#include "card_emulation_error.h"

using namespace OHOS::nfc::cardemulation;
namespace OHOS::nfc::cardemulation::test{
class AidRoutingControllerTest : public ::testing::Test {
public:
    AidRoutingControllerTest();

protected:
    void SetUp() override
    {
        EXPECT_CALL(*dh_, GetAidRoutingTableSize()).WillRepeatedly(testing::Return(1024));
        EXPECT_CALL(*dh_, GetDefaultRoute()).WillRepeatedly(testing::Return(0));
        EXPECT_CALL(*dh_, GetOffHostUiccRoute()).WillRepeatedly(testing::Return(std::vector<int>{1}));
        EXPECT_CALL(*dh_, GetOffHostEseRoute()).WillRepeatedly(testing::Return(std::vector<int>{2}));
        EXPECT_CALL(*dh_, GetDefaultOffHostRoute()).WillRepeatedly(testing::Return(3));
        EXPECT_CALL(*dh_, GetAidMatchingMode()).WillRepeatedly(testing::Return(AID_ROUTING_MODE_MASK_PREFIX));

        EXPECT_CALL(*dh_, AddAidRouting(testing::_, testing::_, testing::_)).WillRepeatedly(testing::Return(true));
        EXPECT_CALL(*dh_, RemoveAidRouting(testing::_)).WillRepeatedly(testing::Return(true));
        EXPECT_CALL(*dh_, CommitRouting()).WillRepeatedly(testing::Return(true));
        EXPECT_CALL(*dh_, SendData(testing::_)).WillRepeatedly(testing::Return(true));
        arc_ = std::make_shared<AidRoutingAdapter>(dh_);
    }
    void TearDown() override
    {
        arc_.reset();
    }

protected:
    std::shared_ptr<AidRoutingAdapter> arc_{};
    std::shared_ptr<DeviceHostCEMock> dh_;
};
AidRoutingControllerTest::AidRoutingControllerTest()
{
    dh_ = std::make_shared<DeviceHostCEMock>();
}
/**
 * @tc.number:
 * @tc.name  :
 * @tc.desc  :
 */
TEST(AidRoutingAdapter, NotInit)
{
    auto arc = std::make_shared<AidRoutingAdapter>(std::shared_ptr<DeviceHostCEMock>());
    EXPECT_EQ(arc->GetDefaultRoute(), 0);
    EXPECT_EQ(arc->GetDefaultOffHostRoute(), 0);
    EXPECT_EQ(arc->LocationStringToInt(""), 0);
    EXPECT_EQ(arc->LocationStringToInt(NFC_EE_HOST), 0);
    EXPECT_EQ(arc->LocationStringToInt(NFC_EE_ESE), 0);
    EXPECT_EQ(arc->LocationStringToInt(NFC_EE_UICC), 0);
    EXPECT_EQ(arc->LocationStringToInt(NFC_EE_ESE + "1"), 0);
    EXPECT_EQ(arc->LocationStringToInt(NFC_EE_UICC + "2"), 0);
}

/**
 * @tc.number:
 * @tc.name  :
 * @tc.desc  :
 */
TEST_F(AidRoutingControllerTest, GetDefaultRoute)
{
    EXPECT_EQ(arc_->GetDefaultRoute(), 0);
}
/**
 * @tc.number:
 * @tc.name  :
 * @tc.desc  :
 */
TEST_F(AidRoutingControllerTest, GetDefaultOffHostRoute)
{
    EXPECT_EQ(arc_->GetDefaultOffHostRoute(), 3);
}

/**
 * @tc.number:
 * @tc.name  :
 * @tc.desc  :
 */
TEST_F(AidRoutingControllerTest, GetAidRoutingMode)
{
    // [0,2] 区间
    EXPECT_GE(arc_->GetAidRoutingMode(), 0);
    EXPECT_LE(arc_->GetAidRoutingMode(), 2);
}

/**
 * @tc.number:
 * @tc.name  :
 * @tc.desc  :
 */
TEST_F(AidRoutingControllerTest, GetAidRoutingTableCapacity)
{
    // (0,?) 区间
    EXPECT_GT(arc_->GetAidRoutingTableCapacity(), 0);
    EXPECT_LE(arc_->GetAidRoutingTableCapacity(), 1024);
}
/**
 * @tc.number:
 * @tc.name  :
 * @tc.desc  :
 */
TEST(AidRoutingAdapter, AddAidRoutingEntry_Failed)
{
    auto arc = std::make_shared<AidRoutingAdapter>(std::shared_ptr<DeviceHostCEMock>());
    // aid is empty
    EXPECT_EQ(arc->AddAidRoutingEntry(std::vector<unsigned char>{}, 0, 0), ERR_NOT_INIT_NCI);
    // 0< aid size < 5 or aid size > 16
    EXPECT_EQ(arc->AddAidRoutingEntry(std::vector<unsigned char>{0x01}, 0, 0), ERR_AID_ILLIGLE_LENGTH);
    EXPECT_EQ(arc->AddAidRoutingEntry(std::vector<unsigned char>(17, 0x01), 0, 0), ERR_AID_ILLIGLE_LENGTH);
    EXPECT_EQ(arc->AddAidRoutingEntry(std::vector<unsigned char>({0xa0, 0x01, 0x2, 0x3, 0x4, 0x5}), 0, 0),
              ERR_NOT_INIT_NCI);
}

/**
 * @tc.number:
 * @tc.name  :
 * @tc.desc  :
 */
TEST(AidRoutingAdapter, RemoveAidRoutingEntry_Failed)
{
    auto arc = std::make_shared<AidRoutingAdapter>(std::shared_ptr<DeviceHostCEMock>());
    EXPECT_EQ(arc->RemoveAidRoutingEntry(std::vector<unsigned char>()), ERR_AID_ILLIGLE_LENGTH);
    EXPECT_EQ(arc->RemoveAidRoutingEntry(std::vector<unsigned char>({0xa0, 0x01, 0x2, 0x3, 0x4, 0x5})),
              ERR_NOT_INIT_NCI);
}

/**
 * @tc.number:
 * @tc.name  :
 * @tc.desc  :
 */
TEST_F(AidRoutingControllerTest, AddAidRoutingEntry_OK)
{
    EXPECT_EQ(arc_->AddAidRoutingEntry(std::vector<unsigned char>({0xa0, 0x01, 0x2, 0x3, 0x4, 0x5}), 0, 0), ERR_OK);
}

/**
 * @tc.number:
 * @tc.name  :
 * @tc.desc  :
 */
TEST_F(AidRoutingControllerTest, RemoveAidRoutingEntry_OK)
{
    EXPECT_EQ(arc_->RemoveAidRoutingEntry(std::vector<unsigned char>({0xa0, 0x01, 0x2, 0x3, 0x4, 0x5})), ERR_OK);
}

/**
 * @tc.number:
 * @tc.name  :
 * @tc.desc  :
 */
TEST_F(AidRoutingControllerTest, CommitAidRouting_OK)
{
    EXPECT_EQ(arc_->CommitAidRouting(), ERR_OK);
}
/**
 * @tc.number:
 * @tc.name  :
 * @tc.desc  :
 */
TEST_F(AidRoutingControllerTest, LocationStringToInt)
{
    EXPECT_CALL(*dh_, GetDefaultOffHostRoute()).WillRepeatedly(testing::Return(3));
    EXPECT_EQ(arc_->GetDefaultRoute(), 0);
    EXPECT_EQ(arc_->GetDefaultOffHostRoute(), 3);
    EXPECT_EQ(arc_->LocationStringToInt(""), 0);
    EXPECT_EQ(arc_->LocationStringToInt(NFC_EE_HOST), 0);

    EXPECT_EQ(arc_->LocationStringToInt(NFC_EE_UICC), 1);
    EXPECT_EQ(arc_->LocationStringToInt(NFC_EE_UICC + "1"), 1);
    EXPECT_EQ(arc_->LocationStringToInt(NFC_EE_UICC + "2"), 0);
    EXPECT_EQ(arc_->LocationStringToInt(NFC_EE_ESE), 2);
    EXPECT_EQ(arc_->LocationStringToInt(NFC_EE_ESE + "1"), 2);
    EXPECT_EQ(arc_->LocationStringToInt(NFC_EE_ESE + "2"), 0);
}
/**
 * @tc.number:
 * @tc.name  :
 * @tc.desc  :
 */
TEST_F(AidRoutingControllerTest, IsSurpported)
{
    EXPECT_CALL(*dh_, GetDefaultOffHostRoute()).WillRepeatedly(testing::Return(3));
    EXPECT_CALL(*dh_, GetOffHostUiccRoute)
        .WillOnce(testing::Return(std::vector<int>{
            0x81,
        }));
    EXPECT_CALL(*dh_, GetOffHostEseRoute)
        .WillOnce(testing::Return(std::vector<int>{
            0x82,
        }));
    EXPECT_EQ(arc_->GetDefaultRoute(), 0);
    EXPECT_EQ(arc_->GetDefaultOffHostRoute(), 3);
    EXPECT_EQ(arc_->IsSurpported(""), false);
    EXPECT_EQ(arc_->IsSurpported(NFC_EE_HOST), true);

    EXPECT_EQ(arc_->IsSurpported(NFC_EE_UICC), true);
    EXPECT_EQ(arc_->IsSurpported(NFC_EE_UICC + "1"), true);
    EXPECT_EQ(arc_->IsSurpported(NFC_EE_UICC + "2"), false);
    EXPECT_EQ(arc_->IsSurpported(NFC_EE_ESE), true);
    EXPECT_EQ(arc_->IsSurpported(NFC_EE_ESE + "1"), true);
    EXPECT_EQ(arc_->IsSurpported(NFC_EE_ESE + "2"), false);
}

/**
 * @tc.number:
 * @tc.name  :
 * @tc.desc  :
 */
TEST_F(AidRoutingControllerTest, ReviseSecureElementName)
{
    EXPECT_STREQ(arc_->ReviseSecureElementName(NFC_EE_HOST).c_str(), NFC_EE_HOST.c_str());
    EXPECT_STREQ(arc_->ReviseSecureElementName(NFC_EE_ESE).c_str(), NFC_EE_ESE.c_str());
}
}