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
#define private public
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>
#include <string>
#include <vector>

#include "access_rule_cache.h"
#include "access_rule_data_common.h"
#include "access_rule_files_controller.h"
#include "base_test.h"
#include "channel_access_rule.h"
#include "context.h"
#include "isecure_element_mock.h"
#include "se_channel.h"
#include "terminal.h"
#undef private

using namespace OHOS::se::security;
using namespace OHOS::se;
using namespace OHOS::hardware::se::v1_0;
using namespace osal;
using namespace testing;

namespace OHOS::se::test {
class ArfTest : public ::testing::Test {
public:
    virtual void SetUp() override
    {
        std::cout << " ArfTest set up." << std::endl;
        accessRuleCache_ = std::make_shared<AccessRuleCache>();
        context_ = std::make_shared<Context>();
        secureElementMock_ = BaseTest::LoadSecureElement(true);
        EXPECT_CALL(*secureElementMock_, Initialize(_)).Times(AnyNumber());
        terminal_ = std::make_shared<Terminal>("ArfTest", context_);
        terminal_->Initialize(secureElementMock_);
        accessRuleFilesController_ = std::make_shared<AccessRuleFilesController>(accessRuleCache_, terminal_);
    }

    virtual void TearDown() override
    {
        std::cout << " ArfTest tear down." << std::endl;
    }

public:
    std::shared_ptr<AccessRuleCache> accessRuleCache_;
    std::shared_ptr<Context> context_;
    sptr<ISecureElementMock> secureElementMock_;
    std::shared_ptr<Terminal> terminal_;
    std::shared_ptr<AccessRuleFilesController> accessRuleFilesController_;
};

/**
 * @tc.number    : ARF_TEST_001
 * @tc.name      : Initialize_Test
 * @tc.desc      : Initialize
 */
TEST_F(ArfTest, Initialize_Test)
{
    try {
        std::string apduSuccess = {(char)0x90, 0x00};
        std::unique_ptr<OHOS::hardware::se::v1_0::ISecureElement::LogicalRespData> resData(
            std::make_unique<OHOS::hardware::se::v1_0::ISecureElement::LogicalRespData>());
        resData->status = SEStatus::SUCCESS;
        resData->channelNumber = 1;
        resData->resp = apduSuccess;
        std::unique_ptr<OHOS::hardware::se::v1_0::ISecureElement::LogicalRespData> resData2(
            std::make_unique<OHOS::hardware::se::v1_0::ISecureElement::LogicalRespData>());
        resData2->status = SEStatus::SUCCESS;
        resData2->channelNumber = 2;
        resData2->resp = apduSuccess;
        EXPECT_CALL(*secureElementMock_, OpenLogicalChannel(_, _))
            .WillOnce(Return(ByMove(std::move(resData))))
            .WillOnce(Return(ByMove(std::move(resData2))));
        EXPECT_CALL(*secureElementMock_, Transmit(_))
            .WillOnce(Return(EF_SIM_EF_TRANSPARENT_ODF + apduSuccess))
            .WillOnce(Return(ODF_DATA + apduSuccess))
            .WillOnce(Return(EF_SIM_EF_TRANSPARENT_DODF + apduSuccess))
            .WillOnce(Return(DODF_DATA + apduSuccess))
            .WillOnce(Return(EF_SIM_EF_TRANSPARENT_ACMF + apduSuccess))
            .WillOnce(Return(ACCESS_CONTROL_MAIN_DATA + apduSuccess))
            .WillOnce(Return(EF_SIM_EF_TRANSPARENT_ODF + apduSuccess))
            .WillOnce(Return(ODF_DATA + apduSuccess))
            .WillOnce(Return(EF_SIM_EF_TRANSPARENT_DODF + apduSuccess))
            .WillOnce(Return(DODF_DATA + apduSuccess))
            .WillOnce(Return(EF_SIM_EF_TRANSPARENT_ACRF + apduSuccess))
            .WillOnce(Return(ACCESS_CONTROL_RULES_DATA + apduSuccess))
            .WillOnce(Return(EF_SIM_EF_TRANSPARENT_ACCF + apduSuccess))
            .WillOnce(Return(ACCESS_CONTROL_CONDITIONS_DATA + apduSuccess));

        accessRuleFilesController_->Initialize();
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : ARF_TEST_002
 * @tc.name      : Initialize_TerminalExpired_Test
 * @tc.desc      : Initialize when termianla expired
 */
TEST_F(ArfTest, Initialize_TerminalExpired_Test)
{
    try {
        terminal_ = std::shared_ptr<Terminal>();
        accessRuleFilesController_->Initialize();
        EXPECT_TRUE(false);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(true);
    }
}
}  // namespace OHOS::se::test