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

#include "card_emulation_service_info_manager.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <algorithm>
#include <iostream>

#include "aid_set.h"
#include "card_emulation_def.h"
#include "card_emulation_error.h"
#include "card_emulation_service_info.h"
#include "card_emulation_service_info_lite.h"
#include "element_name.h"
#include "iaid_routing_manager.h"
#include "installed_ceservice_getter.h"
#include "test_util.h"
namespace OHOS::nfc::cardemulation::test{
static constexpr int DEFAULT_USER_ID = 0;
static std::string elementAbilityName1 = "ability1";
static std::string elementAbilityName2 = "ability2";
static std::string elementAbilityName3 = "ability3";

using namespace OHOS::nfc::cardemulation;
using namespace OHOS::nfc::cardemulation::test;
using namespace testing;

class InstalledCEAppGetterMock : public InstalledCeServiceGetter {
public:
    MOCK_METHOD1(GetInstalled, std::vector<std::shared_ptr<CardEmulationServiceInfo>>(int));

    MOCK_METHOD1(GetDynamic, std::vector<std::shared_ptr<CardEmulationServiceInfo>>(int userId));
    MOCK_METHOD1(GetDefaultElementName, OHOS::AppExecFwk::ElementName(int userId));

#ifdef MOCK_FOR_TESTING

    MOCK_METHOD2(StoreDynamic, void(int userId, const std::vector<std::shared_ptr<CardEmulationServiceInfo>>&));
    MOCK_METHOD2(StoreDefault, void(int userId, std::string abilityName));
#endif
};
class ICEAppObserverMock : public IAidRoutingManager {
public:
    MOCK_METHOD1(GetCardEmulationServicesByAid, std::vector<ServiceInfoTypePair>(const std::string& aid));
    MOCK_METHOD3(OnCeServiceChanged,
                 int(std::vector<std::shared_ptr<CardEmulationServiceInfo>> const& apps,
                     const std::shared_ptr<CardEmulationServiceInfo>& primary,
                     const std::shared_ptr<CardEmulationServiceInfo>& preferred));
};

class CEAppTestObserver : public IAidRoutingManager {
public:
    MOCK_METHOD1(GetCardEmulationServicesByAid, std::vector<ServiceInfoTypePair>(const std::string& aid));
    CEAppTestObserver(int expectNotifedCount,
                      size_t appCount,
                      OHOS::AppExecFwk::ElementName primaryServiceName,
                      OHOS::AppExecFwk::ElementName preferredServiceName,
                      std::vector<std::string> elementNames)
        : expectNotifedCount_(expectNotifedCount),
        appCount_(appCount),
        primaryServiceName_(std::move(primaryServiceName)),
        preferredServiceName_(std::move(preferredServiceName)),
        elementNames_(std::move(elementNames))
    {
    }
    virtual ~CEAppTestObserver()
    {
        EXPECT_EQ(notifiedCount_, expectNotifedCount_);
    }
    int OnCeServiceChanged(std::vector<std::shared_ptr<CardEmulationServiceInfo>> const& apps,
                           const std::shared_ptr<CardEmulationServiceInfo>& primary,
                           const std::shared_ptr<CardEmulationServiceInfo>& preferred)
    {
        if (preferred) {
            EXPECT_EQ(preferredServiceName_, *(preferred->GetName()));
        }
        // 仅在最后一次通知时判断
        if (++notifiedCount_ != expectNotifedCount_) {
            return 0;
        }
        if (preferredServiceName_.GetAbilityName().empty()) {
            EXPECT_FALSE(preferred);
        } else {
            EXPECT_TRUE(preferred);
            if (preferred) {
                EXPECT_EQ(preferredServiceName_, *(preferred->GetName()));
            }
        }
        EXPECT_EQ(apps.size(), (appCount_));
        EXPECT_EQ(apps.size(), elementNames_.size());
        for (size_t i = 0; i < apps.size(); ++i) {
            EXPECT_LT(i, elementNames_.size());
            if (elementNames_.size() > i) {
                EXPECT_STREQ(apps[i]->GetName()->GetAbilityName().c_str(), elementNames_[i].c_str());
            }
        }
        return 0;
    }

private:
    int expectNotifedCount_;
    int notifiedCount_{0};
    size_t appCount_;
    OHOS::AppExecFwk::ElementName primaryServiceName_;
    OHOS::AppExecFwk::ElementName preferredServiceName_;
    std::vector<std::string> elementNames_;
};

/**
* @tc.number:
* @tc.name  :
* @tc.desc  :
*/
TEST(CardEmulationServiceInfoManager, Init)
{
    std::string typeNormal = CARDEMULATION_SERVICE_TYPE_NORMAL;
    std::string aidstr11 = "D2760000850101";
    std::string aidstr12 = "D2560000850101";
    auto aidset1 = AidSet::FromRawString({aidstr11, aidstr12});
    aidset1->SetType(typeNormal);

    std::string typeSecure = CARDEMULATION_SERVICE_TYPE_SECURE;
    std::string aidstr21 = "A010203040506070";
    std::string aidstr22 = "BBBB203040506070";
    auto aidset2 = AidSet::FromRawString({aidstr21, aidstr22});
    aidset2->SetType(typeSecure);

    std::shared_ptr<CardEmulationServiceInfo> info1 = std::make_shared<CardEmulationServiceInfo>(NFC_EE_HOST);
    info1->SetName(Util::CreateElementName(elementAbilityName1));
    info1->AddAidsetToStatic(std::move(aidset1));
    std::shared_ptr<CardEmulationServiceInfo> info2 = std::make_shared<CardEmulationServiceInfo>(NFC_EE_HOST);
    info2->AddAidsetToStatic(std::move(aidset2));

    auto defaultElementName = Util::CreateElementName(elementAbilityName2);
    info2->SetName(defaultElementName);

    auto installGetter = std::make_shared<InstalledCEAppGetterMock>();
    EXPECT_CALL(*installGetter, GetInstalled)
        .WillRepeatedly(Return(std::vector<std::shared_ptr<CardEmulationServiceInfo>>{info1, info2}));

    EXPECT_CALL(*installGetter, GetDefaultElementName(_)).WillOnce(Return(defaultElementName));
    EXPECT_CALL(*installGetter, GetDynamic(_))
        .WillOnce(Return(std::vector<std::shared_ptr<CardEmulationServiceInfo>>{}));
    auto ob = std::make_shared<CEAppTestObserver>(1,  // 通知次数. 切换用户, 添加/删除,设置优先服务都会通知
                                                  2,                               // app 数量
                                                  defaultElementName,              // 首要的服务的名称
                                                  Util::CreateEmptyElementName(),  // 优先的服务的名称
                                                  std::vector<std::string>{elementAbilityName1, elementAbilityName2});
    auto ceam = std::make_shared<CardEmulationServiceInfoManager>(ob, installGetter);
    ceam->Init();
}

/**
* @tc.number:
* @tc.name  :
* @tc.desc  :
*/
TEST(CardEmulationServiceInfoManager, AddAidSet)
{
    std::string typeNormal = CARDEMULATION_SERVICE_TYPE_NORMAL;
    std::string aidstr11 = "A010203040506070";
    std::string aidstr12 = "B010203040506070";
    std::vector<std::string> aidset1 = {aidstr11, aidstr12};

    std::string typeSecure = CARDEMULATION_SERVICE_TYPE_SECURE;
    std::string aidstr21 = "A0102030405060";
    std::string aidstr22 = "B0102030405060";
    std::vector<std::string> aidset2 = {aidstr21, aidstr22};

    std::string aidstr31 = "CCCC203040506070";
    std::string aidstr32 = "DDDD203040506070";
    std::vector<std::string> aidset3 = {aidstr31, aidstr32};

    auto ob = std::make_shared<CEAppTestObserver>(
        6,  
        3,  
        Util::CreateElementName(elementAbilityName2),  
        Util::CreateElementName(elementAbilityName3), 
        std::vector<std::string>{elementAbilityName1, elementAbilityName2, elementAbilityName3});
    auto getter = std::make_shared<InstalledCEAppGetterMock>();
    EXPECT_CALL(*getter, StoreDefault(0, elementAbilityName2)).Times(1);

    std::shared_ptr<CardEmulationServiceInfo> info1 = std::make_shared<CardEmulationServiceInfo>(NFC_EE_HOST);
    info1->SetName(Util::CreateElementName(elementAbilityName1));
    std::shared_ptr<CardEmulationServiceInfo> info2 = std::make_shared<CardEmulationServiceInfo>(NFC_EE_HOST);
    info2->SetName(Util::CreateElementName(elementAbilityName2));
    std::shared_ptr<CardEmulationServiceInfo> info3 = std::make_shared<CardEmulationServiceInfo>(NFC_EE_HOST);
    info3->SetName(Util::CreateElementName(elementAbilityName3));

    EXPECT_CALL(*getter, GetInstalled(_))
        .WillOnce(Return(std::vector<std::shared_ptr<CardEmulationServiceInfo>>{info1, info2, info3}));
    EXPECT_CALL(*getter, GetDynamic(_)).WillOnce(Return(std::vector<std::shared_ptr<CardEmulationServiceInfo>>{}));
    EXPECT_CALL(*getter, GetDefaultElementName(_)).WillOnce(Return(OHOS::AppExecFwk::ElementName()));
    auto ceam = std::make_shared<CardEmulationServiceInfoManager>(ob, getter);
    ceam->OnUserSwitched(DEFAULT_USER_ID);

    // invalid parameters
    // invalid userId
    EXPECT_EQ(ceam->AddAidSet(-1, Util::CreateEmptyElementName(), "", std::vector<std::string>()), ERR_INVALID_USERID);
    // invalid elementName
    EXPECT_EQ(ceam->AddAidSet(DEFAULT_USER_ID, Util::CreateEmptyElementName(), "", std::vector<std::string>()),
              ERR_INVALID_ELEMENT_NAME);
    // invalid type
    EXPECT_EQ(
        ceam->AddAidSet(DEFAULT_USER_ID, Util::CreateElementName("abc"), "invalid_type", std::vector<std::string>()),
        ERR_INVALID_SERVICE_TYPE);

    // invalid aid set
    EXPECT_EQ(ceam->AddAidSet(DEFAULT_USER_ID, Util::CreateElementName(elementAbilityName1), typeNormal, {}),
              ERR_AIDSET_IS_EMPTY);
    // !invalid parameters
    EXPECT_EQ(ceam->AddAidSet(DEFAULT_USER_ID, Util::CreateElementName(elementAbilityName1), typeNormal, aidset1),
              ERR_OK);

    EXPECT_EQ(ceam->AddAidSet(DEFAULT_USER_ID, Util::CreateElementName(elementAbilityName2), typeSecure, aidset2),
              ERR_OK);
    EXPECT_EQ(ceam->AddAidSet(DEFAULT_USER_ID, Util::CreateElementName(elementAbilityName3), typeNormal, aidset3),
              ERR_OK);

    EXPECT_EQ(
        ceam->SetDefaultCEServiceForType(DEFAULT_USER_ID, Util::CreateElementName(elementAbilityName2), typeSecure),
        ERR_OK);
    EXPECT_EQ(ceam->SetPreferredCEService(DEFAULT_USER_ID, Util::CreateElementName(elementAbilityName3)), ERR_OK);
    EXPECT_EQ(
        ceam->GetAidsForCEService(DEFAULT_USER_ID, Util::CreateElementName(elementAbilityName1), typeNormal).size(),
        aidset1.size());
}

/**
* @tc.number:
* @tc.name  :
* @tc.desc  :
*/
TEST(CardEmulationServiceInfoManager, RemoveAidSet)
{
    const std::string& typeNormal = CARDEMULATION_SERVICE_TYPE_NORMAL;
    auto aidstrStatic11 = "AAAA203040506070";
    std::vector<std::string> aidsetStatic1 = {aidstrStatic11};

    const std::string& typeSecure = CARDEMULATION_SERVICE_TYPE_SECURE;
    auto aidstrStatic21 = "BBBB2030405060";
    std::vector<std::string> aidsetStatic2 = {aidstrStatic21};

    auto aidstr11 = "A010203040506070";
    auto aidstr12 = "B010203040506070";
    std::vector<std::string> aidset1 = {aidstr11, aidstr12};

    auto aidstr21 = "A0102030405060";
    auto aidstr22 = "B0102030405060";
    std::vector<std::string> aidset2 = {aidstr21, aidstr22};

    auto ob = std::make_shared<CEAppTestObserver>(4, 
                                                  2,                                            
                                                  Util::CreateElementName(elementAbilityName2), 
                                                  Util::CreateEmptyElementName(),  
                                                  std::vector<std::string>{elementAbilityName1, elementAbilityName2});

    auto getter = std::make_shared<InstalledCEAppGetterMock>();
    // EXPECT_CALL(*getter, StoreDefault(0, elementAbilityName2)).Times(1);

    std::shared_ptr<CardEmulationServiceInfo> info1 = std::make_shared<CardEmulationServiceInfo>(NFC_EE_HOST);
    info1->SetName(Util::CreateElementName(elementAbilityName1));
    std::shared_ptr<CardEmulationServiceInfo> info2 = std::make_shared<CardEmulationServiceInfo>(NFC_EE_HOST);
    info2->SetName(Util::CreateElementName(elementAbilityName2));

    EXPECT_CALL(*getter, GetInstalled(_))
        .WillOnce(Return(std::vector<std::shared_ptr<CardEmulationServiceInfo>>{info1, info2}));
    EXPECT_CALL(*getter, GetDynamic(_)).WillOnce(Return(std::vector<std::shared_ptr<CardEmulationServiceInfo>>{}));
    EXPECT_CALL(*getter, GetDefaultElementName(_)).WillOnce(Return(OHOS::AppExecFwk::ElementName()));

    auto ceam = std::make_shared<CardEmulationServiceInfoManager>(ob, getter);

    ceam->OnUserSwitched(DEFAULT_USER_ID);
    EXPECT_EQ(ceam->AddAidSet(DEFAULT_USER_ID, Util::CreateElementName(elementAbilityName1), typeNormal, aidset1),
              ERR_OK);
    EXPECT_EQ(ceam->AddAidSet(DEFAULT_USER_ID, Util::CreateElementName(elementAbilityName2), typeSecure, aidset2),
              ERR_OK);
    EXPECT_EQ(ceam->SetPreferredCEService(DEFAULT_USER_ID, Util::CreateElementName(elementAbilityName2)),
              ERR_CONFILICTS_WITH_DEFAULT);
    EXPECT_EQ(ceam->RemoveAidSet(DEFAULT_USER_ID, Util::CreateElementName(elementAbilityName1), typeNormal), ERR_OK);

    EXPECT_TRUE(
        ceam->GetAidsForCEService(DEFAULT_USER_ID, Util::CreateElementName(elementAbilityName1), typeNormal).empty());
    EXPECT_TRUE(
        ceam->GetAidsForCEService(DEFAULT_USER_ID, Util::CreateElementName(elementAbilityName1), typeSecure).empty());
    EXPECT_TRUE(
        ceam->GetAidsForCEService(DEFAULT_USER_ID, Util::CreateElementName(elementAbilityName2), typeNormal).empty());
    EXPECT_TRUE(
        ceam->GetAidsForCEService(DEFAULT_USER_ID, Util::CreateElementName(elementAbilityName2), typeSecure).size() ==
        2u);
}

/**
* @tc.number:
* @tc.name  :
* @tc.desc  :
*/
TEST(CardEmulationServiceInfoManager, RemoveAidSet_withStatic)
{
    std::shared_ptr<InstalledCEAppGetterMock> installGetter;
    std::string typeNormal = CARDEMULATION_SERVICE_TYPE_NORMAL;
    std::string aidstrStatic11 = "AAAA0000850101";
    auto aidsetStatic1 = AidSet::FromRawString({aidstrStatic11});
    aidsetStatic1->SetType(typeNormal);

    std::string typeSecure = CARDEMULATION_SERVICE_TYPE_SECURE;
    std::string aidstrStatic21 = "BBBB203040506070";
    auto aidsetStatic2 = AidSet::FromRawString({aidstrStatic21});
    aidsetStatic2->SetType(typeSecure);

    std::shared_ptr<CardEmulationServiceInfo> info1 = std::make_shared<CardEmulationServiceInfo>(NFC_EE_HOST);
    info1->SetName(Util::CreateElementName(elementAbilityName1));
    info1->AddAidsetToStatic(std::move(aidsetStatic1));
    std::shared_ptr<CardEmulationServiceInfo> info2 = std::make_shared<CardEmulationServiceInfo>(NFC_EE_HOST);
    info2->AddAidsetToStatic(std::move(aidsetStatic2));

    auto defaultElementName = Util::CreateElementName(elementAbilityName2);
    info2->SetName(defaultElementName);

    auto elementName1 = *(info1->GetName());
    auto elementName2 = *(info2->GetName());
    installGetter = std::make_shared<InstalledCEAppGetterMock>();
    EXPECT_CALL(*installGetter, GetInstalled)
        .WillRepeatedly(Return(std::vector<std::shared_ptr<CardEmulationServiceInfo>>{info1, info2}));

    EXPECT_CALL(*installGetter, GetDefaultElementName(_)).WillOnce(Return(defaultElementName));
    EXPECT_CALL(*installGetter, GetDynamic(_))
        .WillOnce(Return(std::vector<std::shared_ptr<CardEmulationServiceInfo>>{}));
    EXPECT_CALL(*installGetter, StoreDynamic(_, _)).Times(3);
    std::string aidstr11 = "A010203040506070";
    std::string aidstr12 = "B010203040506070";
    std::vector<std::string> aidset1 = {aidstr11, aidstr12};

    std::string aidstr21 = "A0102030405060";
    std::string aidstr22 = "B0102030405060";
    std::vector<std::string> aidset2 = {aidstr21, aidstr22};

    auto ob = std::make_shared<CEAppTestObserver>(4,  
                                                  2,                              
                                                  elementName2,                    
                                                  Util::CreateEmptyElementName(),  
                                                  std::vector<std::string>{elementAbilityName1, elementAbilityName2});
    auto ceam = std::make_shared<CardEmulationServiceInfoManager>(ob, installGetter);

    ceam->OnUserSwitched(DEFAULT_USER_ID);
    EXPECT_EQ(ceam->AddAidSet(DEFAULT_USER_ID, elementName1, typeNormal, aidset1), ERR_OK);
    EXPECT_EQ(ceam->AddAidSet(DEFAULT_USER_ID, elementName2, typeSecure, aidset2), ERR_OK);
    EXPECT_EQ(ceam->SetPreferredCEService(DEFAULT_USER_ID, elementName2), ERR_CONFILICTS_WITH_DEFAULT);
    EXPECT_EQ(ceam->RemoveAidSet(DEFAULT_USER_ID, elementName1, typeNormal), ERR_OK);
    auto aids = ceam->GetAidsForCEService(DEFAULT_USER_ID, elementName1, typeNormal);
    EXPECT_EQ(aids.size(), 1u);
    EXPECT_STRCASEEQ(aids[0].c_str(), aidstrStatic11.c_str());
}

/**
* @tc.number:
* @tc.name  :
* @tc.desc  :
*/
TEST(CardEmulationServiceInfoManager, MarkOffHostService)
{
    const std::string& typeNormal = CARDEMULATION_SERVICE_TYPE_NORMAL;
    auto aidstr11 = "A010203040506070";
    auto aidstr12 = "B010203040506070";
    auto aidset1 = std::vector<std::string>({aidstr11, aidstr12});
    std::shared_ptr<CardEmulationServiceInfo> info1 = std::make_shared<CardEmulationServiceInfo>(NFC_EE_HOST);
    info1->SetName(Util::CreateElementName(elementAbilityName1));
    info1->AddAidsetToStatic(AidSet::FromRawString(aidset1));

    const std::string& typeSecure = CARDEMULATION_SERVICE_TYPE_SECURE;
    auto aidstr21 = "A0102030405060";
    auto aidstr22 = "B0102030405060";
    auto aidset2 = std::vector<std::string>({aidstr21, aidstr22});
    std::shared_ptr<CardEmulationServiceInfo> info2 = std::make_shared<CardEmulationServiceInfo>(NFC_EE_ESE);
    info2->SetName(Util::CreateElementName(elementAbilityName2));
    info2->AddAidsetToStatic(AidSet::FromRawString(aidset2));

    std::string aidstr31 = "CCCC203040506070";
    std::string aidstr32 = "DDDD203040506070";
    auto aidset3 = std::vector<std::string>({aidstr31, aidstr32});
    std::shared_ptr<CardEmulationServiceInfo> info3 = std::make_shared<CardEmulationServiceInfo>(NFC_EE_UICC_1);
    info3->SetName(Util::CreateElementName(elementAbilityName3));
    info3->AddAidsetToStatic(AidSet::FromRawString(aidset3));

    auto installGetter = std::make_shared<InstalledCEAppGetterMock>();
    EXPECT_CALL(*installGetter, GetInstalled)
        .WillRepeatedly(Return(std::vector<std::shared_ptr<CardEmulationServiceInfo>>{info1, info2, info3}));
    EXPECT_CALL(*installGetter, GetDefaultElementName(_)).WillOnce(Return(*(info2->GetName())));
    EXPECT_CALL(*installGetter, GetDynamic(_))
        .WillOnce(Return(std::vector<std::shared_ptr<CardEmulationServiceInfo>>{}));
    auto ob = std::make_shared<CEAppTestObserver>(
        5,  
        3,  
        Util::CreateElementName(elementAbilityName2),  
        Util::CreateEmptyElementName(),                
        std::vector<std::string>{elementAbilityName1, elementAbilityName2, elementAbilityName3});
    auto ceam = std::make_shared<CardEmulationServiceInfoManager>(ob, installGetter);
    ceam->OnUserSwitched(DEFAULT_USER_ID);  // notify

    EXPECT_EQ(ceam->AddAidSet(DEFAULT_USER_ID, Util::CreateElementName(elementAbilityName1), typeNormal, aidset1),
              ERR_OK);  // notify
    EXPECT_EQ(ceam->AddAidSet(DEFAULT_USER_ID, Util::CreateElementName(elementAbilityName2), typeSecure, aidset2),
              ERR_OK);  // notify
    EXPECT_EQ(ceam->SetPreferredCEService(DEFAULT_USER_ID, Util::CreateElementName(elementAbilityName2)),
              ERR_CONFILICTS_WITH_DEFAULT);
    EXPECT_EQ(ceam->MarkOffHostCEService(DEFAULT_USER_ID, *(info1->GetName()), NFC_EE_ESE),
              ERR_HOST_NO_MODIFICATION_ALLOWED);
    EXPECT_EQ(ceam->MarkOffHostCEService(DEFAULT_USER_ID, *(info2->GetName()), NFC_EE_ESE_1), ERR_OK);  // notify
    EXPECT_STREQ(info2->GetExecutionEnvironment().c_str(), NFC_EE_ESE_1.c_str());
    EXPECT_EQ(ceam->MarkOffHostCEService(DEFAULT_USER_ID, *(info3->GetName()), NFC_EE_UICC), ERR_OK);  // notify
    EXPECT_STREQ(info3->GetExecutionEnvironment().c_str(), NFC_EE_UICC.c_str());
}

/**
* @tc.number:
* @tc.name  :
* @tc.desc  :
*/
TEST(CardEmulationServiceInfoManager, OnUserSwitched)
{
    auto aidstr11 = "A010203040506070";
    auto aidstr12 = "B010203040506070";
    auto aidset1 = std::vector<std::string>({aidstr11, aidstr12});
    std::shared_ptr<CardEmulationServiceInfo> info1 = std::make_shared<CardEmulationServiceInfo>(NFC_EE_HOST);
    info1->SetName(Util::CreateElementName(elementAbilityName1));
    info1->AddAidsetToStatic(AidSet::FromRawString(aidset1));

    auto observer = std::make_shared<ICEAppObserverMock>();
    EXPECT_CALL(*observer, OnCeServiceChanged(_, _, _)).WillOnce(Return(0));
    auto getter = std::make_shared<InstalledCEAppGetterMock>();
    EXPECT_CALL(*getter, GetInstalled(_))
        .WillOnce(Return(std::vector<std::shared_ptr<CardEmulationServiceInfo>>{info1}));
    EXPECT_CALL(*getter, GetDynamic(_)).WillOnce(Return(std::vector<std::shared_ptr<CardEmulationServiceInfo>>()));
    EXPECT_CALL(*getter, GetDefaultElementName(_)).WillOnce(Return(*(info1->GetName())));
    EXPECT_CALL(*getter, StoreDynamic(_, _)).Times(1);

    CardEmulationServiceInfoManager cesm(observer, getter);
    cesm.Init();
    cesm.Deinit();
}
/**
* @tc.number:
* @tc.name  :
* @tc.desc  :
*/
TEST(CardEmulationServiceInfoManager, NoInit)
{
    {
        CardEmulationServiceInfoManager cesm({}, {});
        EXPECT_EQ(cesm.OnUserSwitched(0), ERR_CESERVICE_GETTER_IS_NULL);
        EXPECT_EQ(cesm.MarkOffHostCEService(-1, {}, {}), ERR_INVALID_USERID);
        EXPECT_EQ(cesm.MarkOffHostCEService(0, {}, {}), ERR_INVALID_ELEMENT_NAME);
        EXPECT_EQ(cesm.MarkOffHostCEService(0, Util::CreateElementName("name"), {}), ERR_NOT_FOUND_CESERVICE_INFO);
    }
    {
        auto observer = std::make_shared<ICEAppObserverMock>();
        EXPECT_CALL(*observer, OnCeServiceChanged(_, _, _)).WillOnce(Return(0));
        auto getter = std::make_shared<InstalledCEAppGetterMock>();
        EXPECT_CALL(*getter, GetInstalled(_))
            .WillOnce(Return(std::vector<std::shared_ptr<CardEmulationServiceInfo>>()));
        EXPECT_CALL(*getter, GetDynamic(_)).WillOnce(Return(std::vector<std::shared_ptr<CardEmulationServiceInfo>>()));
        EXPECT_CALL(*getter, GetDefaultElementName(_)).WillOnce(Return(OHOS::AppExecFwk::ElementName()));

        CardEmulationServiceInfoManager cesm(observer, getter);
        EXPECT_EQ(cesm.OnUserSwitched(0), ERR_OK);
        EXPECT_EQ(cesm.SetDefaultCEServiceForType(0, {}, {}), ERR_INVALID_ELEMENT_NAME);
        EXPECT_FALSE(cesm.IsDefaultCEServiceForAid(0, {}, {}));
        EXPECT_FALSE(cesm.IsDefaultCEServiceForType(0, {}, {}));
        EXPECT_FALSE(cesm.GetUserPreferred(1));
        EXPECT_TRUE(cesm.GetAidsForCEService(0, {}, {}).empty());
        EXPECT_TRUE(cesm.GetPrimaryServiceForType(0, {}) == OHOS::AppExecFwk::ElementName());
    }
}
}
