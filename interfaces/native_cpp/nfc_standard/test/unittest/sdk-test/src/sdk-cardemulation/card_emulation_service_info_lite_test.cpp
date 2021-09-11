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

#include "card_emulation_service_info_lite.h"

#include <gtest/gtest.h>

#include "element_name.h"

using namespace OHOS::AppExecFwk;
using namespace OHOS::nfc::sdk::cardemulation;
namespace OHOS::nfc::sdk::cardemulation::test {
/**
* @tc.number:
* @tc.name  :
* @tc.desc  :
*/
TEST(CardEmulationServiceInfoLite, CopyConstructor)
{
    ElementName en1("dev1", "bundle1", "ability1");
    std::string label1 = "label1";
    std::string desc1 = "desc1";
    std::string ee1 = "host";
    CardEmulationServiceInfoLite info1(en1, label1, desc1, ee1);

    CardEmulationServiceInfoLite info2(info1);

    EXPECT_EQ(info1.GetName(), info2.GetName());
    EXPECT_EQ(info1.GetLabel(), info2.GetLabel());
    EXPECT_EQ(info1.GetExecutionEnvironment(), info2.GetExecutionEnvironment());

    EXPECT_EQ(info1.GetName(), en1);
    EXPECT_EQ(info1.GetLabel(), label1);
    EXPECT_EQ(info1.GetExecutionEnvironment(), ee1);
}
/**
* @tc.number:
* @tc.name  :
* @tc.desc  :
*/
TEST(CardEmulationServiceInfoLite, MoveConstructor)
{
    ElementName en1("dev1", "bundle1", "ability1");
    std::string label1 = "label1";
    std::string desc1 = "desc1";
    std::string ee1 = "host";
    CardEmulationServiceInfoLite info1(en1, label1, desc1, ee1);

    CardEmulationServiceInfoLite info2(std::move(info1));

    EXPECT_EQ(info1.GetName(), ElementName());
    EXPECT_EQ(info1.GetLabel(), "");
    EXPECT_EQ(info1.GetExecutionEnvironment(), "");

    EXPECT_EQ(info2.GetName(), en1);
    EXPECT_EQ(info2.GetLabel(), label1);
    EXPECT_EQ(info2.GetExecutionEnvironment(), ee1);
}
/**
* @tc.number:
* @tc.name  :
* @tc.desc  :
*/
TEST(CardEmulationServiceInfoLite, CopyAssign)
{
    ElementName en1("dev1", "bundle1", "ability1");
    std::string label1 = "label1";
    std::string desc1 = "desc1";
    std::string ee1 = "host";
    CardEmulationServiceInfoLite info1(en1, label1, desc1, ee1);

    CardEmulationServiceInfoLite info2;
    info2 = info1;

    EXPECT_EQ(info1.GetName(), info2.GetName());
    EXPECT_EQ(info1.GetLabel(), info2.GetLabel());
    EXPECT_EQ(info1.GetExecutionEnvironment(), info2.GetExecutionEnvironment());

    EXPECT_EQ(info1.GetName(), en1);
    EXPECT_EQ(info1.GetLabel(), label1);
    EXPECT_EQ(info1.GetExecutionEnvironment(), ee1);
}
/**
* @tc.number:
* @tc.name  :
* @tc.desc  :
*/
TEST(CardEmulationServiceInfoLite, MoveAssign)
{
    ElementName en1("dev1", "bundle1", "ability1");
    std::string label1 = "label1";
    std::string desc1 = "desc1";
    std::string ee1 = "host";
    CardEmulationServiceInfoLite info1(en1, label1, desc1, ee1);

    CardEmulationServiceInfoLite info2;

    info2 = std::move(info1);

    EXPECT_EQ(info1.GetName(), ElementName());
    EXPECT_EQ(info1.GetLabel(), "");
    EXPECT_EQ(info1.GetExecutionEnvironment(), "");

    EXPECT_EQ(info2.GetName(), en1);
    EXPECT_EQ(info2.GetLabel(), label1);
    EXPECT_EQ(info2.GetExecutionEnvironment(), ee1);
}
/**
* @tc.number:
* @tc.name  :
* @tc.desc  :
*/
TEST(CardEmulationServiceInfoLite, Equals)
{
    ElementName en1("dev1", "bundle1", "ability1");
    std::string label1 = "label1";
    std::string desc1 = "desc1";
    std::string ee1 = "host";
    CardEmulationServiceInfoLite info1(en1, label1, desc1, ee1);

    CardEmulationServiceInfoLite info2(info1);
    EXPECT_EQ(info1, info2);

    CardEmulationServiceInfoLite info3(en1, "", "", "");
    EXPECT_EQ(info1, info3);

    ElementName en2("dev2", "bundle2", "ability2");
    CardEmulationServiceInfoLite info4(en2, label1, desc1, ee1);
    EXPECT_NE(info4, info3);
}
} 