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
#include "general-data-objects/response_refresh_tag_do.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>
#include <string>
#include <vector>

#include "../access_rule_data_common.h"

using namespace OHOS::se::security;

namespace OHOS::se::test {
class ResponseRefreshTagDoTest : public ::testing::Test {
public:
    virtual void SetUp() override
    {
        std::cout << " ResponseRefreshTagDoTest set up." << std::endl;
    }

    virtual void TearDown() override
    {
        std::cout << " ResponseRefreshTagDoTest tear down." << std::endl;
    }

public:
};

/**
 * @tc.number    : RESPONSE_REFRESH_TAG_DO_TEST_001
 * @tc.name      : Response_Refresh_Tag_Test
 * @tc.desc      : Generate response-refresh-tag-do
 */
TEST_F(ResponseRefreshTagDoTest, Response_Refresh_Tag_Test)
{
    try {
        std::string data = RESPONSE_REFRESH_TAG_DO;
        std::shared_ptr<BerTlv> bt = BerTlv::StrToBerTlv(data);
        std::shared_ptr<ResponseRefreshTagDo> responseRefreshTagDo =
            ResponseRefreshTagDo::BerTlvToResponseRefreshTagDo(bt);
        EXPECT_EQ(RESPONSE_REFRESH_TAG, responseRefreshTagDo->GetRefreshTag());
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(false);
    }
}

/**
 * @tc.number    : RESPONSE_REFRESH_TAG_DO_TEST_002
 * @tc.name      : Illegal_Tag_Test
 * @tc.desc      : Generate response-refresh-tag-do with illegal tag
 */
TEST_F(ResponseRefreshTagDoTest, Illegal_Tag_Test)
{
    try {
        std::string data = RESPONSE_REFRESH_TAG_DO;
        data[0] = 0x00;
        std::shared_ptr<BerTlv> bt = BerTlv::StrToBerTlv(data);
        std::shared_ptr<ResponseRefreshTagDo> responseRefreshTagDo =
            ResponseRefreshTagDo::BerTlvToResponseRefreshTagDo(bt);
        EXPECT_TRUE(false);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(true);
    }
}

/**
 * @tc.number    : RESPONSE_REFRESH_TAG_DO_TEST_003
 * @tc.name      : Data_Not_Enough_Test
 * @tc.desc      : Generate response-refresh-tag-do with not enough value
 */
TEST_F(ResponseRefreshTagDoTest, Data_Not_Enough_Test)
{
    try {
        std::string data = RESPONSE_REFRESH_TAG_DO;
        data = data.substr(0, data.length() - 1);
        std::shared_ptr<BerTlv> bt = BerTlv::StrToBerTlv(data);
        std::shared_ptr<ResponseRefreshTagDo> responseRefreshTagDo =
            ResponseRefreshTagDo::BerTlvToResponseRefreshTagDo(bt);
        EXPECT_TRUE(false);
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
        EXPECT_TRUE(true);
    }
}
}  // namespace OHOS::se::test