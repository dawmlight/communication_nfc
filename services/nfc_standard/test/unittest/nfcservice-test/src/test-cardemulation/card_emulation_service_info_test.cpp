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

#include "card_emulation_service_info.h"

#include <gtest/gtest.h>

#include <fstream>
#include <iostream>
#include <sstream>

#include "aid_set.h"
#include "card_emulation_def.h"
#include "card_emulation_error.h"
#include "card_emulation_service_info_lite.h"
#include "element_name.h"
#include "test_util.h"
using namespace OHOS::nfc::cardemulation;
using namespace OHOS::nfc::cardemulation::test;
namespace OHOS::nfc::cardemulation::test{
static const std::string STR_JSON_ability1 = R"({

      "ability_name": "ability1",
      "ability_type": "host_card_emulation",
      "aid_sets": [
        {
          "aids": ["D2760000850101", "D2560000850101"],
          "description": "ndef",
          "type": "normal"
        }
      ],
      "environment": "host",
      "icon": "$icon",
      "label": "label",
      "unlock_device": true
    })";
static const std::string STR_JSON_ability2 = R"({
        "ability_name" : "ability2",
      "ability_type": "offhost_card_emulation",
        "aid_sets" :
            [ {"aids" : [ "A010203040506070", "BBBB203040506070" ], "description" : "", "type" : "secure"} ],
        "environment" : "eSE1",
        "icon" : "$icon",
        "label" : "label",
        "unlock_device" : true
    })";
/**
* @tc.number:
* @tc.name  :
* @tc.desc  :
*/
TEST(CardEmulationServiceInfo, ALessB)
{
    std::shared_ptr<AidSet> aidset0;
    EXPECT_FALSE(CardEmulationServiceInfo::ALessB(aidset0, aidset0));

    auto aidstr1 = "A010203040506070";
    auto aidset1 = std::shared_ptr<AidSet>(AidSet::FromRawString({aidstr1}));
    aidset1->SetType(CARDEMULATION_SERVICE_TYPE_NORMAL);
    // compare with self
    EXPECT_FALSE(CardEmulationServiceInfo::ALessB(aidset1, aidset1));
    // compare with nullptr
    EXPECT_FALSE(CardEmulationServiceInfo::ALessB(aidset0, aidset1));
    EXPECT_FALSE(CardEmulationServiceInfo::ALessB(aidset1, aidset0));

    auto aidstr2 = "A010203040506070";
    auto aidset2 = std::shared_ptr<AidSet>(AidSet::FromRawString({aidstr2}));
    aidset2->SetType(CARDEMULATION_SERVICE_TYPE_SECURE);
    //
    EXPECT_FALSE(CardEmulationServiceInfo::ALessB(aidset2, aidset2));
    EXPECT_FALSE(CardEmulationServiceInfo::ALessB(aidset1, aidset2));
    EXPECT_TRUE(CardEmulationServiceInfo::ALessB(aidset2, aidset1));
    // compare with nullptr
    EXPECT_FALSE(CardEmulationServiceInfo::ALessB(aidset0, aidset2));
    EXPECT_FALSE(CardEmulationServiceInfo::ALessB(aidset2, aidset0));

    auto aidstr3 = "A010203040506070";
    auto aidset3 = std::shared_ptr<AidSet>(AidSet::FromRawString({aidstr3}));
    aidset3->SetType(CARDEMULATION_SERVICE_TYPE_NORMAL);
    EXPECT_FALSE(CardEmulationServiceInfo::ALessB(aidset1, aidset3));
    EXPECT_FALSE(CardEmulationServiceInfo::ALessB(aidset3, aidset1));

    auto aidstr4 = "A010203040506070";
    auto aidset4 = std::shared_ptr<AidSet>(AidSet::FromRawString({aidstr4}));
    aidset4->SetType(CARDEMULATION_SERVICE_TYPE_SECURE);

    EXPECT_TRUE(CardEmulationServiceInfo::ALessB(aidset2, aidset4));
    EXPECT_TRUE(CardEmulationServiceInfo::ALessB(aidset4, aidset2));
}
/**
* @tc.number:
* @tc.name  :
* @tc.desc  :
*/
TEST(CardEmulationServiceInfo, AddAidsetToStatic)
{
    auto ceinfo = std::make_shared<CardEmulationServiceInfo>(NFC_EE_HOST);

    auto aidstr1 = "A010203040506070";
    auto aidset1 = AidSet::FromRawString({aidstr1});
    aidset1->SetType(CARDEMULATION_SERVICE_TYPE_NORMAL);
    auto paidset1 = aidset1.get();

    EXPECT_TRUE(ceinfo->AddAidsetToStatic(std::move(aidset1)));
    EXPECT_FALSE(ceinfo->AddAidsetToStatic(std::move(aidset1)));
    // no aids

    EXPECT_FALSE(ceinfo->AddAidsetToStatic(std::unique_ptr<AidSet>()));
    // no type
    auto aidset2 = AidSet::FromRawString({aidstr1});
    EXPECT_FALSE(ceinfo->AddAidsetToStatic(std::move(aidset2)));

    std::map<std::string, bool> found;
    ceinfo->Visit([&paidset1, &found](std::string const& type, std::shared_ptr<AidSet> aidset) {
        found[type] = (paidset1 == aidset.get());
    });
    EXPECT_TRUE(found[paidset1->GetType()]);
}

/**
* @tc.number:
* @tc.name  :
* @tc.desc  :
*/
TEST(CardEmulationServiceInfo, AddAidset)
{
    auto ceinfo0 = std::make_shared<CardEmulationServiceInfo>(NFC_EE_HOST, Util::CreateElementName("ce0"), false);
    EXPECT_FALSE(ceinfo0->MustUnlock());

    auto ceinfo = std::make_shared<CardEmulationServiceInfo>(NFC_EE_HOST);
    EXPECT_FALSE(ceinfo->AddAidset(nullptr));
    EXPECT_FALSE(ceinfo->AddAidset(std::unique_ptr<AidSet>()));
    EXPECT_EQ(ceinfo->SetExecutionEnvironment(NFC_EE_HOST), ERR_HOST_NO_MODIFICATION_ALLOWED);
    EXPECT_TRUE(ceinfo->MustUnlock());
    EXPECT_TRUE(ceinfo->GetName());

    auto aidstr1 = "A010203040506070";
    auto aidset1 = AidSet::FromRawString({aidstr1});
    aidset1->SetType(CARDEMULATION_SERVICE_TYPE_NORMAL);
    auto paidset1 = aidset1.get();

    EXPECT_TRUE(ceinfo->AddAidset(std::move(aidset1)));

    std::map<std::string, bool> found;
    ceinfo->Visit([&paidset1, &found](std::string const& type, std::shared_ptr<AidSet> aidset) {
        found[type] = (paidset1 == aidset.get());
    });

    EXPECT_TRUE(ceinfo->HasType(CARDEMULATION_SERVICE_TYPE_NORMAL));
    EXPECT_TRUE(found[paidset1->GetType()]);
}

/**
* @tc.number:
* @tc.name  :
* @tc.desc  :
*/
TEST(CardEmulationServiceInfo, AddAidset_override_static)
{
    auto ceinfo0 = std::make_shared<CardEmulationServiceInfo>(NFC_EE_HOST, Util::CreateElementName("ce0"), false);
    EXPECT_FALSE(ceinfo0->MustUnlock());

    auto ceinfo = std::make_shared<CardEmulationServiceInfo>(NFC_EE_HOST);
    EXPECT_FALSE(ceinfo->AddAidset(nullptr));
    EXPECT_FALSE(ceinfo->AddAidset(std::unique_ptr<AidSet>()));
    EXPECT_EQ(ceinfo->SetExecutionEnvironment(NFC_EE_HOST), ERR_HOST_NO_MODIFICATION_ALLOWED);
    EXPECT_TRUE(ceinfo->MustUnlock());
    EXPECT_TRUE(ceinfo->GetName());

    auto aidstrStatic1 = "AAAA203040506070";
    auto aidsetStatic1 = AidSet::FromRawString({aidstrStatic1});
    aidsetStatic1->SetType(CARDEMULATION_SERVICE_TYPE_NORMAL);
    EXPECT_TRUE(ceinfo->AddAidsetToStatic(std::move(aidsetStatic1)));

    auto aidstr1 = "A010203040506070";
    auto aidset1 = AidSet::FromRawString({aidstr1});
    aidset1->SetType(CARDEMULATION_SERVICE_TYPE_NORMAL);

    EXPECT_TRUE(ceinfo->AddAidset(std::move(aidset1)));

    bool bFoundAidsetStatic1{false};
    bool bFoundAidset1{false};
    ceinfo->Visit([&](std::string const& type, std::shared_ptr<AidSet> aidset) {
        if (aidset->HasAidString(AidString(aidstrStatic1))) {
            bFoundAidsetStatic1 = true;
        }
        if (aidset->HasAidString(AidString(aidstr1))) {
            bFoundAidset1 = true;
        }
    });

    EXPECT_FALSE(ceinfo->HasType(CARDEMULATION_SERVICE_TYPE_SECURE));
    EXPECT_TRUE(ceinfo->HasType(CARDEMULATION_SERVICE_TYPE_NORMAL));
    EXPECT_FALSE(bFoundAidsetStatic1);
    EXPECT_TRUE(bFoundAidset1);

    // remove
    bFoundAidsetStatic1 = false;
    bFoundAidset1 = false;
    EXPECT_TRUE(ceinfo->RemoveAidset(CARDEMULATION_SERVICE_TYPE_NORMAL));
    ceinfo->Visit([&](std::string const& type, std::shared_ptr<AidSet> aidset) {
        if (aidset->HasAidString(AidString(aidstrStatic1))) {
            bFoundAidsetStatic1 = true;
        }
        if (aidset->HasAidString(AidString(aidstr1))) {
            bFoundAidset1 = true;
        }
    });

    EXPECT_TRUE(ceinfo->HasType(CARDEMULATION_SERVICE_TYPE_NORMAL));
    EXPECT_FALSE(ceinfo->HasType(CARDEMULATION_SERVICE_TYPE_SECURE));
    EXPECT_TRUE(bFoundAidsetStatic1);
    EXPECT_FALSE(bFoundAidset1);
}

/**
* @tc.number:
* @tc.name  :
* @tc.desc  :
*/
TEST(CardEmulationServiceInfo, AddAidset_override_same_type)
{
    auto ceinfo0 = std::make_shared<CardEmulationServiceInfo>(NFC_EE_HOST, Util::CreateElementName("ce0"), false);
    EXPECT_FALSE(ceinfo0->MustUnlock());

    auto ceinfo = std::make_shared<CardEmulationServiceInfo>(NFC_EE_HOST);
    EXPECT_FALSE(ceinfo->AddAidset(nullptr));
    EXPECT_FALSE(ceinfo->AddAidset(std::unique_ptr<AidSet>()));
    EXPECT_EQ(ceinfo->SetExecutionEnvironment(NFC_EE_HOST), ERR_HOST_NO_MODIFICATION_ALLOWED);
    EXPECT_TRUE(ceinfo->MustUnlock());
    EXPECT_TRUE(ceinfo->GetName());

    auto aidstr0 = "AAAA203040506070";
    auto aidset0 = AidSet::FromRawString({aidstr0});
    aidset0->SetType(CARDEMULATION_SERVICE_TYPE_SECURE);

    EXPECT_TRUE(ceinfo->AddAidset(std::move(aidset0)));

    auto aidstr1 = "A010203040506070";
    auto aidset1 = AidSet::FromRawString({aidstr1});
    aidset1->SetType(CARDEMULATION_SERVICE_TYPE_NORMAL);

    EXPECT_TRUE(ceinfo->AddAidset(std::move(aidset1)));

    bool bFoundAidset0{false};
    bool bFoundAidset1{false};
    ceinfo->Visit([&](std::string const& type, std::shared_ptr<AidSet> aidset) {
        if (aidset->HasAidString(AidString(aidstr0))) {
            bFoundAidset0 = true;
        }
        if (aidset->HasAidString(AidString(aidstr1))) {
            bFoundAidset1 = true;
        }
    });
    EXPECT_TRUE(bFoundAidset1);
    EXPECT_TRUE(bFoundAidset0);

    EXPECT_TRUE(ceinfo->HasType(CARDEMULATION_SERVICE_TYPE_SECURE));
    EXPECT_TRUE(ceinfo->HasType(CARDEMULATION_SERVICE_TYPE_NORMAL));

    // override - secure
    bool bFoundAidset2{false};
    auto aidstr2 = "BBBB203040506070";
    auto aidset2 = AidSet::FromRawString({aidstr2});
    aidset2->SetType(CARDEMULATION_SERVICE_TYPE_SECURE);
    // 2 override 0
    EXPECT_TRUE(ceinfo->AddAidset(std::move(aidset2)));

    EXPECT_TRUE(ceinfo->HasType(CARDEMULATION_SERVICE_TYPE_SECURE));
    EXPECT_TRUE(ceinfo->HasType(CARDEMULATION_SERVICE_TYPE_NORMAL));

    bFoundAidset0 = false;
    bFoundAidset1 = false;
    bFoundAidset2 = false;
    ceinfo->Visit([&](std::string const& type, std::shared_ptr<AidSet> aidset) {
        if (aidset->HasAidString(AidString(aidstr0))) {
            bFoundAidset0 = true;
        }
        if (aidset->HasAidString(AidString(aidstr1))) {
            bFoundAidset1 = true;
        }
        if (aidset->HasAidString(AidString(aidstr2))) {
            bFoundAidset2 = true;
        }
    });
    EXPECT_FALSE(bFoundAidset0);
    EXPECT_TRUE(bFoundAidset1);
    EXPECT_TRUE(bFoundAidset2);

    auto aidstr3 = "CCCC203040506070";
    auto aidset3 = AidSet::FromRawString({aidstr3});
    aidset3->SetType(CARDEMULATION_SERVICE_TYPE_NORMAL);
    // override - normal. 3 override 1
    EXPECT_TRUE(ceinfo->AddAidset(std::move(aidset3)));

    bFoundAidset0 = false;
    bFoundAidset1 = false;
    bFoundAidset2 = false;
    bool bFoundAidset3 = false;

    ceinfo->Visit([&](std::string const& type, std::shared_ptr<AidSet> aidset) {
        if (aidset->HasAidString(AidString(aidstr0))) {
            bFoundAidset0 = true;
        }
        if (aidset->HasAidString(AidString(aidstr1))) {
            bFoundAidset1 = true;
        }
        if (aidset->HasAidString(AidString(aidstr2))) {
            bFoundAidset2 = true;
        }
        if (aidset->HasAidString(AidString(aidstr3))) {
            bFoundAidset3 = true;
        }
    });
    EXPECT_FALSE(bFoundAidset0);
    EXPECT_FALSE(bFoundAidset1);
    EXPECT_TRUE(bFoundAidset2);
    EXPECT_TRUE(bFoundAidset3);
}

/**
* @tc.number:
* @tc.name  :
* @tc.desc  :
*/
TEST(CardEmulationServiceInfo, RemoveAidset)
{
    auto ceinfo = std::make_shared<CardEmulationServiceInfo>(NFC_EE_HOST);
    auto aidstr1 = "A010203040506070";
    auto aidset1 = AidSet::FromRawString({aidstr1});
    auto type1 = "aidset1";
    aidset1->SetType(type1);
    EXPECT_TRUE(ceinfo->AddAidset(std::move(aidset1)));
    EXPECT_TRUE(ceinfo->RemoveAidset(type1));
    EXPECT_FALSE(ceinfo->RemoveAidset(type1));
}
/**
* @tc.number:
* @tc.name  :
* @tc.desc  :
*/
TEST(CardEmulationServiceInfo, HasType)
{
    auto ceinfo = std::make_shared<CardEmulationServiceInfo>(NFC_EE_HOST);
    EXPECT_FALSE(ceinfo->HasType(CARDEMULATION_SERVICE_TYPE_NORMAL));
    EXPECT_FALSE(ceinfo->HasType(CARDEMULATION_SERVICE_TYPE_SECURE));

    // load static
    nlohmann::json j = nlohmann::json::parse(STR_JSON_ability2);
    EXPECT_TRUE(j.is_object());
    ceinfo->FromJson(j);
    EXPECT_TRUE(ceinfo->HasType(CARDEMULATION_SERVICE_TYPE_SECURE));

    auto aidstr1 = "A010203040506070";
    auto aidset1 = AidSet::FromRawString({aidstr1});
    aidset1->SetType(CARDEMULATION_SERVICE_TYPE_NORMAL);
    ceinfo->AddAidset(std::move(aidset1));
    EXPECT_TRUE(ceinfo->HasType(CARDEMULATION_SERVICE_TYPE_NORMAL));
}
/**
* @tc.number:
* @tc.name  :
* @tc.desc  :
*/
TEST(CardEmulationServiceInfo, SortAidsets)
{
    auto ceinfo = std::make_shared<CardEmulationServiceInfo>(NFC_EE_HOST);

    auto sorted = ceinfo->SortAidsets(CardEmulationServiceInfo::ALessB);
    EXPECT_TRUE(sorted.empty());

    auto aidstr1 = "A010203040506071";
    auto aidset1 = (AidSet::FromRawString({aidstr1}));
    aidset1->SetType(CARDEMULATION_SERVICE_TYPE_NORMAL);
    AidSet* paidset1 = aidset1.get();

    EXPECT_TRUE(ceinfo->AddAidset(std::move(aidset1)));
    sorted = ceinfo->SortAidsets(CardEmulationServiceInfo::ALessB);
    EXPECT_TRUE(!sorted.empty());

    EXPECT_TRUE(sorted.front().get() == paidset1);

    auto aidstr2 = "A010203040506072";
    auto aidset2 = (AidSet::FromRawString({aidstr2}));
    aidset2->SetType(CARDEMULATION_SERVICE_TYPE_SECURE);
    AidSet* paidset2 = aidset2.get();

    EXPECT_TRUE(ceinfo->AddAidset(std::move(aidset2)));

    sorted = ceinfo->SortAidsets(CardEmulationServiceInfo::ALessB);
    EXPECT_TRUE(!sorted.empty());
    EXPECT_TRUE(sorted.front().get() == paidset2);
    EXPECT_TRUE(sorted.back().get() == paidset1);

    auto aidstr3 = "A010203040506073";
    auto aidset3 = (AidSet::FromRawString({aidstr3}));
    aidset3->SetType(CARDEMULATION_SERVICE_TYPE_NORMAL);
    AidSet* paidset3 = aidset3.get();

    EXPECT_TRUE(ceinfo->AddAidset(std::move(aidset3)));
    sorted = ceinfo->SortAidsets(CardEmulationServiceInfo::ALessB);
    EXPECT_TRUE(!sorted.empty());
    EXPECT_TRUE(sorted.front().get() == paidset2);
    EXPECT_TRUE(sorted.back().get() == paidset3);

    auto aidstr4 = "A010203040506074";
    auto aidset4 = (AidSet::FromRawString({aidstr4}));
    aidset4->SetType(CARDEMULATION_SERVICE_TYPE_SECURE);
    AidSet* paidset4 = aidset4.get();

    EXPECT_TRUE(ceinfo->AddAidset(std::move(aidset4)));
    sorted = ceinfo->SortAidsets(CardEmulationServiceInfo::ALessB);
    EXPECT_TRUE(!sorted.empty());
    EXPECT_TRUE(sorted.front().get() == paidset4);
    EXPECT_TRUE(sorted.back().get() == paidset3);
}
/**
* @tc.number:
* @tc.name  :
* @tc.desc  :
*/
TEST(CardEmulationServiceInfo, GetAidsetByType)
{
    nlohmann::json j = nlohmann::json::parse(STR_JSON_ability1);
    EXPECT_TRUE(j.is_object());
    auto ceinfo = std::make_shared<CardEmulationServiceInfo>(NFC_EE_ESE_1);
    EXPECT_EQ(ceinfo->GetAidsetByType(CARDEMULATION_SERVICE_TYPE_NORMAL), nullptr);
    EXPECT_EQ(ceinfo->GetAidsetByType(CARDEMULATION_SERVICE_TYPE_SECURE), nullptr);
    ceinfo->FromJson(j);

    auto aidsetNormal = ceinfo->GetAidsetByType(CARDEMULATION_SERVICE_TYPE_NORMAL);
    ASSERT_TRUE(aidsetNormal);
    EXPECT_EQ(aidsetNormal->Size(), 2u);

    auto aidsetSecure = ceinfo->GetAidsetByType(CARDEMULATION_SERVICE_TYPE_SECURE);
    EXPECT_FALSE(aidsetSecure);
    auto aidstr1 = "A010203040506070";
    auto aidstr2 = "BBBB203040506070";
    auto aidset1 = AidSet::FromRawString({aidstr1, aidstr2});
    aidset1->SetType(CARDEMULATION_SERVICE_TYPE_SECURE);
    ceinfo->AddAidset(std::move(aidset1));
    aidsetSecure = ceinfo->GetAidsetByType(CARDEMULATION_SERVICE_TYPE_SECURE);
    ASSERT_TRUE(aidsetSecure);
    EXPECT_EQ(aidsetSecure->Size(), 2u);
}
/**
* @tc.number:
* @tc.name  :
* @tc.desc  :
*/
TEST(CardEmulationServiceInfo, ToJson)
{
    auto ceinfo = std::make_shared<CardEmulationServiceInfo>(NFC_EE_HOST);
    ceinfo->SetIcon("$icon");
    ceinfo->SetLabel("label");
    OHOS::AppExecFwk::ElementName en;
    en.SetAbilityName("ability1");
    en.SetDeviceID("device");
    en.SetBundleName("bundle");
    ceinfo->SetName(en);
    auto aidstr1 = "A010203040506070";
    auto aidstr2 = "BBBB203040506070";
    auto aidset1 = AidSet::FromRawString({aidstr1, aidstr2});
    aidset1->SetType(CARDEMULATION_SERVICE_TYPE_SECURE);
    EXPECT_TRUE(ceinfo->AddAidset(std::move(aidset1)));
    nlohmann::json j;
    ceinfo->ToJson(j);

    ASSERT_TRUE(j.is_object());
    EXPECT_TRUE(j["ability_name"] == "ability1");
    EXPECT_TRUE(j["environment"] == "host");
    EXPECT_TRUE(j["unlock_device"] == true);
    EXPECT_TRUE(j.at("/aid_sets/0/aids/0"_json_pointer) == "A010203040506070");
    EXPECT_TRUE(j.at("/aid_sets/0/aids/1"_json_pointer) == "BBBB203040506070");
}
/**
* @tc.number:
* @tc.name  :
* @tc.desc  :
*/
TEST(CardEmulationServiceInfo, FromJson)
{
    nlohmann::json j = nlohmann::json::parse(STR_JSON_ability2);
    EXPECT_TRUE(j.is_object());
    auto ceinfo = std::make_shared<CardEmulationServiceInfo>(NFC_EE_ESE_1);
    ceinfo->FromJson(j);
    EXPECT_STREQ(ceinfo->GetLabel().c_str(), "label");
    EXPECT_STREQ(ceinfo->GetIcon().c_str(), "$icon");

    std::vector<std::shared_ptr<AidSet>> aidsets;

    ceinfo->Visit([&aidsets](const std::string& type, std::shared_ptr<AidSet> aidset) {
        aidsets.emplace_back(std::move(aidset));
    });
    EXPECT_EQ(aidsets.size(), 1u);
    auto aids = aidsets[0]->GetAllAidRawString();
    auto aidstr1 = "A010203040506070";
    auto aidstr2 = "BBBB203040506070";
    EXPECT_EQ(aids.size(), 2u);
    EXPECT_STRCASEEQ(aids[0].c_str(), aidstr1);
    EXPECT_STRCASEEQ(aids[1].c_str(), aidstr2);

    // array
    try {
        nlohmann::json jarray =
            nlohmann::json::parse(R"({"abilities":[)" + STR_JSON_ability1 + "," + STR_JSON_ability2 + R"(]})");
        EXPECT_TRUE(jarray.contains("abilities"));

        std::vector<std::shared_ptr<CardEmulationServiceInfo>> infoArray;
        jarray.at("abilities").get_to(infoArray);
        EXPECT_EQ(infoArray.size(), 2u);
    } catch (std::exception& e) {
        printf("catch exception: %s\n", e.what());
    }
}
/**
* @tc.number:
* @tc.name  :
* @tc.desc  :
*/
TEST(CardEmulationServiceInfo, SetExecutionEnvironment)
{
    {
        auto ceinfo0 = std::make_shared<CardEmulationServiceInfo>(NFC_EE_HOST, Util::CreateElementName("ce0"), false);
        EXPECT_EQ(ceinfo0->SetExecutionEnvironment(NFC_EE_HOST), ERR_HOST_NO_MODIFICATION_ALLOWED);
    }

    {
        auto ceinfo0 = std::make_shared<CardEmulationServiceInfo>(NFC_EE_ESE, Util::CreateElementName("ce0"), false);
        EXPECT_STREQ(ceinfo0->GetExecutionEnvironment().c_str(), NFC_EE_ESE.c_str());
        EXPECT_EQ(ceinfo0->SetExecutionEnvironment(NFC_EE_ESE_1), ERR_OK);
        EXPECT_STREQ(ceinfo0->GetExecutionEnvironment().c_str(), NFC_EE_ESE_1.c_str());
        EXPECT_EQ(ceinfo0->SetExecutionEnvironment(NFC_EE_UICC), ERR_OK);
        EXPECT_EQ(ceinfo0->SetExecutionEnvironment(NFC_EE_HOST), ERR_HOST_NO_MODIFICATION_ALLOWED);
        EXPECT_STREQ(ceinfo0->GetExecutionEnvironment().c_str(), NFC_EE_UICC.c_str());
    }
    {
        nlohmann::json j = nlohmann::json::parse(STR_JSON_ability2);
        EXPECT_TRUE(j.is_object());
        auto ceinfo = std::make_shared<CardEmulationServiceInfo>("");
        ceinfo->FromJson(j);
        EXPECT_STREQ(ceinfo->GetExecutionEnvironment().c_str(), NFC_EE_ESE_1.c_str());
        EXPECT_EQ(ceinfo->SetExecutionEnvironment(NFC_EE_UICC), ERR_OK);
        EXPECT_STREQ(ceinfo->GetExecutionEnvironment().c_str(), NFC_EE_UICC.c_str());
    }
}
}