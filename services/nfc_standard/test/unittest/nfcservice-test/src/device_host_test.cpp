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
#include "device_host.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "device_host.h"
#include "nci_bal_manager.h"
#include "nfc_nci_mock.h"
#include "nfc_target.h"
#include "tag_end_point.h"

using namespace OHOS::nfc::ncibal;

class DeviceHostTestListenerDemo : public OHOS::nfc::ncibal::IDeviceHost::IDeviceHostListener {
public:
    DeviceHostTestListenerDemo() : OHOS::nfc::ncibal::DeviceHost::IDeviceHostListener() {}
    virtual ~DeviceHostTestListenerDemo() = default;
    virtual void OnRemoteFieldActivated(){};
    virtual void OnRemoteFieldDeactivated(){};
    virtual void OnHostCardEmulationActivated(int technology){};
    virtual void OnHostCardEmulationDeactivated(int technology){};
    virtual void OnHostCardEmulationDataReceived(int technology, std::string& data){};
    virtual void OnTagDiscovered(std::shared_ptr<ITagEndPoint> tagEndPoint){};
    virtual void OnOffHostTransactionEvent(std::string& aid, std::string& data, std::string& seName){};
    virtual void OnEeUpdate(){};
};

class DeviceHostTest : public ::testing::Test {
public:
    virtual void SetUp() override
    {
        std::cout << " SetUpTestCase." << std::endl;
        std::shared_ptr<DeviceHostTestListenerDemo> listener =
            std::make_shared<DeviceHostTestListenerDemo>(DeviceHostTestListenerDemo());
        deviceHost_ = std::make_shared<OHOS::nfc::ncibal::DeviceHost>(listener);
        nfcNciMock_ = std::make_shared<NfcNciMock>();
        deviceHost_->SetNfcNciImpl(nfcNciMock_);
        EXPECT_TRUE(deviceHost_->Initialize());
    }

    virtual void TearDown() override
    {
        std::cout << " TearDownTestCase." << std::endl;
    }

public:
    std::shared_ptr<DeviceHost> deviceHost_{};
    std::shared_ptr<NfcNciMock> nfcNciMock_{};
};

/**
 * @tc.number    : NFC_DEVICE_HOST_API_0001
 * @tc.name      : Initialize_Test
 * @tc.desc      : DeviceHost Initialize
 */
TEST_F(DeviceHostTest, Initialize_Test)
{
    EXPECT_TRUE(deviceHost_->Deinitialize());

    EXPECT_TRUE(deviceHost_->Initialize());
}

/**
 * @tc.number    : NFC_DEVICE_HOST_API_0002
 * @tc.name      : Initialize_Failed_Test
 * @tc.desc      : DeviceHost Initialize NFA_Enable return NFA_STATUS_FAILED
 */
TEST_F(DeviceHostTest, Initialize_Failed_Test)
{
    EXPECT_TRUE(deviceHost_->Deinitialize());

    nfcNciMock_->SetEnableScene(1);
    EXPECT_FALSE(deviceHost_->Initialize());
}

/**
 * @tc.number    : NFC_DEVICE_HOST_API_0003
 * @tc.name      : Initialize_Failed2_Test
 * @tc.desc      : DeviceHost Initialize NFA_DM_ENABLE_EVT event status NFA_STATUS_FAILED
 */
TEST_F(DeviceHostTest, Initialize_Failed2_Test)
{
    EXPECT_TRUE(deviceHost_->Deinitialize());

    nfcNciMock_->SetEnableScene(2);
    EXPECT_FALSE(deviceHost_->Initialize());
}

/**
 * @tc.number    : NFC_DEVICE_HOST_API_0004
 * @tc.name      : Deinitialize_Test
 * @tc.desc      : DeviceHost Deinitialize
 */
TEST_F(DeviceHostTest, Deinitialize_Test)
{
    EXPECT_TRUE(deviceHost_->Deinitialize());
}

/**
 * @tc.number    : NFC_DEVICE_HOST_API_0005
 * @tc.name      : Deinitialize_Failed_Test
 * @tc.desc      : DeviceHost Deinitialize NFA_Disable return NFA_STATUS_FAILED
 */
TEST_F(DeviceHostTest, Deinitialize_Failed_Test)
{
    nfcNciMock_->SetDisableScene(1);
    EXPECT_FALSE(deviceHost_->Deinitialize());
}

/**
 * @tc.number    : NFC_DEVICE_HOST_API_0006
 * @tc.name      : EnableDiscovery_NoTech_Test
 * @tc.desc      : DeviceHost EnableDiscovery enable reader mode with no tech
 */
TEST_F(DeviceHostTest, EnableDiscovery_NoTech_Test)
{
    deviceHost_->EnableDiscovery(0, true, true, false);
}

/**
 * @tc.number    : NFC_DEVICE_HOST_API_0007
 * @tc.name      : EnableDiscovery_Test
 * @tc.desc      : DeviceHost EnableDiscovery enable reader mode with tech
 */
TEST_F(DeviceHostTest, EnableDiscovery_Test)
{
    deviceHost_->EnableDiscovery(0x01, true, true, false);
}

/**
 * @tc.number    : NFC_DEVICE_HOST_API_0008
 * @tc.name      : EnableDiscovery_DefaultTech_Test
 * @tc.desc      : DeviceHost EnableDiscovery enable reader mode with default tech
 */
TEST_F(DeviceHostTest, EnableDiscovery_DefaultTech_Test)
{
    deviceHost_->EnableDiscovery(-1, true, true, false);
}

/**
 * @tc.number    : NFC_DEVICE_HOST_API_0009
 * @tc.name      : EnableDiscovery_NfcOff_Test
 * @tc.desc      : DeviceHost EnableDiscovery when NFC off
 */
TEST_F(DeviceHostTest, EnableDiscovery_NfcOff_Test)
{
    EXPECT_TRUE(deviceHost_->Deinitialize());
    deviceHost_->EnableDiscovery(-1, true, true, false);
}

/**
 * @tc.number    : NFC_DEVICE_HOST_API_0010
 * @tc.name      : EnableDiscovery_Repeat_Test
 * @tc.desc      : DeviceHost EnableDiscovery repeat
 */
TEST_F(DeviceHostTest, EnableDiscovery_Repeat_Test)
{
    deviceHost_->EnableDiscovery(-1, true, true, false);
    deviceHost_->EnableDiscovery(-1, true, true, false);
}

/**
 * @tc.number    : NFC_DEVICE_HOST_API_0011
 * @tc.name      : EnableDiscovery_DisableReadMode_Test
 * @tc.desc      : DeviceHost EnableDiscovery disable reader mode after once enable
 */
TEST_F(DeviceHostTest, EnableDiscovery_DisableReadMode_Test)
{
    deviceHost_->EnableDiscovery(-1, true, true, false);
    deviceHost_->EnableDiscovery(-1, false, true, false);
}

/**
 * @tc.number    : NFC_DEVICE_HOST_API_0012
 * @tc.name      : EnableDiscovery_DisableRoutingToHost_Test
 * @tc.desc      : DeviceHost EnableDiscovery disable routing to host
 */
TEST_F(DeviceHostTest, EnableDiscovery_DisableRoutingToHost_Test)
{
    deviceHost_->EnableDiscovery(-1, false, false, false);
}

/**
 * @tc.number    : NFC_DEVICE_HOST_API_0013
 * @tc.name      : EnableDiscovery_DisableRoutingToHost_Test
 * @tc.desc      : DeviceHost EnableDiscovery SetDefaultRouteFail
 */
TEST_F(DeviceHostTest, EnableDiscovery_EnableRoutingToHost_SetDefaultRouteFail_Test)
{
    nfcNciMock_->SetSetDefaultProtoRoutingScene(1);
    nfcNciMock_->SetSetDefaultTechRoutingScene(1);
    deviceHost_->EnableDiscovery(-1, false, true, false);
}

/**
 * @tc.number    : NFC_DEVICE_HOST_API_0014
 * @tc.name      : EnableDiscovery_DisableRoutingToHost_Test
 * @tc.desc      : DeviceHost EnableDiscovery ClearDefaultRouteFail
 */
TEST_F(DeviceHostTest, EnableDiscovery_DisableRoutingToHost_ClearDefaultRouteFail_Test)
{
    nfcNciMock_->SetClearDefaultProtoRoutingScene(1);
    nfcNciMock_->SetClearDefaultTechRoutingScene(1);
    deviceHost_->EnableDiscovery(-1, false, false, false);
}

/**
 * @tc.number    : NFC_DEVICE_HOST_API_0015
 * @tc.name      : DisableDiscovery_Test
 * @tc.desc      : DeviceHost DisableDiscovery
 */
TEST_F(DeviceHostTest, DisableDiscovery_Test)
{
    deviceHost_->DisableDiscovery();
}

/**
 * @tc.number    : NFC_DEVICE_HOST_API_0016
 * @tc.name      : DisableDiscovery2_Test
 * @tc.desc      : DeviceHost DisableDiscovery after EnableDiscovery
 */
TEST_F(DeviceHostTest, DisableDiscovery_After_Enable_Test)
{
    deviceHost_->EnableDiscovery(-1, true, true, true);
    deviceHost_->DisableDiscovery();
}

/**
 * @tc.number    : NFC_DEVICE_HOST_API_0017
 * @tc.name      : SendRawFrame_Test
 * @tc.desc      : DeviceHost SendRawFrame
 */
TEST_F(DeviceHostTest, SendRawFrame_Test)
{
    std::string rawData = "rawData";
    EXPECT_TRUE(deviceHost_->SendRawFrame(rawData));
}

/**
 * @tc.number    : NFC_DEVICE_HOST_API_0018
 * @tc.name      : SetScreenStatus_Test
 * @tc.desc      : DeviceHost SetScreenStatus
 */
TEST_F(DeviceHostTest, SetScreenStatus_Test)
{
    EXPECT_TRUE(deviceHost_->SetScreenStatus(NFA_SCREEN_STATE_ON_UNLOCKED));
}

/**
 * @tc.number    : NFC_DEVICE_HOST_API_0019
 * @tc.name      : SetScreenStatus_Repeat_Test
 * @tc.desc      : DeviceHost SetScreenStatus repeat
 */
TEST_F(DeviceHostTest, SetScreenStatus_Repeat_Test)
{
    EXPECT_TRUE(deviceHost_->SetScreenStatus(NFA_SCREEN_STATE_ON_UNLOCKED));
    EXPECT_TRUE(deviceHost_->SetScreenStatus(NFA_SCREEN_STATE_ON_UNLOCKED));
}

/**
 * @tc.number    : NFC_DEVICE_HOST_API_0020
 * @tc.name      : SetScreenStatus_NfcOff_Test
 * @tc.desc      : DeviceHost SetScreenStatus when NFC off
 */
TEST_F(DeviceHostTest, SetScreenStatus_NfcOff_Test)
{
    // set when nfc off
    EXPECT_TRUE(deviceHost_->Deinitialize());
    EXPECT_TRUE(deviceHost_->SetScreenStatus(NFA_SCREEN_STATE_ON_UNLOCKED));
}

/**
 * @tc.number    : NFC_DEVICE_HOST_API_0021
 * @tc.name      : SetScreenStatus_OffLocked_Test
 * @tc.desc      : DeviceHost SetScreenStatus
 */
TEST_F(DeviceHostTest, SetScreenStatus_OffLocked_Test)
{
    EXPECT_TRUE(deviceHost_->SetScreenStatus(NFA_SCREEN_STATE_ON_UNLOCKED));
    EXPECT_TRUE(deviceHost_->SetScreenStatus(NFA_SCREEN_STATE_OFF_LOCKED));
}

/**
 * @tc.number    : NFC_DEVICE_HOST_API_0022
 * @tc.name      : SetScreenStatus_OnLocked_Test
 * @tc.desc      : DeviceHost SetScreenStatus
 */
TEST_F(DeviceHostTest, SetScreenStatus_OnLocked_Test)
{
    EXPECT_TRUE(deviceHost_->SetScreenStatus(NFA_SCREEN_STATE_ON_UNLOCKED));
    EXPECT_TRUE(deviceHost_->SetScreenStatus(NFA_SCREEN_STATE_ON_LOCKED));
}

/**
 * @tc.number    : NFC_DEVICE_HOST_API_0023
 * @tc.name      : SetScreenStatus_OnUnlocked_Test
 * @tc.desc      : DeviceHost SetScreenStatus
 */
TEST_F(DeviceHostTest, SetScreenStatus_OnUnlocked_Test)
{
    EXPECT_TRUE(deviceHost_->SetScreenStatus(NFA_SCREEN_STATE_ON_LOCKED));
    EXPECT_TRUE(deviceHost_->SetScreenStatus(NFA_SCREEN_STATE_ON_UNLOCKED));
}

/**
 * @tc.number    : NFC_DEVICE_HOST_API_0024
 * @tc.name      : SetScreenStatus_OnLocked_Fail_Test
 * @tc.desc      : DeviceHost SetScreenStatus
 */
TEST_F(DeviceHostTest, SetScreenStatus_OnLocked_Fail_Test)
{
    EXPECT_TRUE(deviceHost_->SetScreenStatus(NFA_SCREEN_STATE_ON_UNLOCKED));
    nfcNciMock_->SetSetPowerStatusScene(1);
    EXPECT_TRUE(deviceHost_->SetScreenStatus(NFA_SCREEN_STATE_ON_LOCKED));
}

/**
 * @tc.number    : NFC_DEVICE_HOST_API_0025
 * @tc.name      : SetScreenStatus_OnUnlocked_Fail_Test
 * @tc.desc      : DeviceHost SetScreenStatus
 */
TEST_F(DeviceHostTest, SetScreenStatus_OnUnlocked_Fail_Test)
{
    EXPECT_TRUE(deviceHost_->SetScreenStatus(NFA_SCREEN_STATE_ON_LOCKED));
    nfcNciMock_->SetSetPowerStatusScene(1);
    EXPECT_TRUE(deviceHost_->SetScreenStatus(NFA_SCREEN_STATE_ON_UNLOCKED));
}

/**
 * @tc.number    : NFC_DEVICE_HOST_API_0026
 * @tc.name      : GetNciVersion_Test
 * @tc.desc      : DeviceHost GetNciVersion
 */
TEST_F(DeviceHostTest, GetNciVersion_Test)
{
    EXPECT_EQ(deviceHost_->GetNciVersion(), NCI_VERSION_2_0);
}

/**
 * @tc.number    : NFC_DEVICE_HOST_API_0027
 * @tc.name      : SetSecureNfc_Test
 * @tc.desc      : DeviceHost SetSecureNfc
 */
TEST_F(DeviceHostTest, SetSecureNfc_Test)
{
    bool secure = true;
    EXPECT_TRUE(deviceHost_->SetSecureNfc(secure));
}

/**
 * @tc.number    : NFC_DEVICE_HOST_API_0028
 * @tc.name      : GetIsoDepMaxTransceiveLength_Test
 * @tc.desc      : DeviceHost GetIsoDepMaxTransceiveLength
 */
TEST_F(DeviceHostTest, GetIsoDepMaxTransceiveLength_Test)
{
    // NciBalManager::ISODEP_MAX_TRANCEIVE_LENGTH
    EXPECT_EQ(deviceHost_->GetIsoDepMaxTransceiveLength(), 261);
}

/**
 * @tc.number    : NFC_DEVICE_HOST_API_0029
 * @tc.name      : RegisterT3tIdentifier_Test
 * @tc.desc      : DeviceHost RegisterT3tIdentifier
 */
TEST_F(DeviceHostTest, RegisterT3tIdentifier_Test)
{
    std::string lfT3tIdentifier = "123";
    EXPECT_FALSE(deviceHost_->RegisterT3tIdentifier(lfT3tIdentifier));
}

/**
 * @tc.number    : NFC_DEVICE_HOST_API_0030
 * @tc.name      : DeregisterT3tIdentifier_Test
 * @tc.desc      : DeviceHost DeregisterT3tIdentifier
 */
TEST_F(DeviceHostTest, DeregisterT3tIdentifier_Test)
{
    std::string lfT3tIdentifier = "123";
    deviceHost_->DeregisterT3tIdentifier(lfT3tIdentifier);
}

/**
 * @tc.number    : NFC_DEVICE_HOST_API_0031
 * @tc.name      : ClearT3tIdentifiersCache_Test
 * @tc.desc      : DeviceHost ClearT3tIdentifiersCache
 */
TEST_F(DeviceHostTest, ClearT3tIdentifiersCache_Test)
{
    deviceHost_->ClearT3tIdentifiersCache();
}

/**
 * @tc.number    : NFC_DEVICE_HOST_API_0032
 * @tc.name      : GetLfT3tMax_Test
 * @tc.desc      : DeviceHost GetLfT3tMax
 */
TEST_F(DeviceHostTest, GetLfT3tMax_Test)
{
    EXPECT_EQ(deviceHost_->GetLfT3tMax(), 0);
}

/**
 * @tc.number    : NFC_DEVICE_HOST_API_0033
 * @tc.name      : GetLastError_Test
 * @tc.desc      : DeviceHost GetLastError
 */
TEST_F(DeviceHostTest, GetLastError_Test)
{
    EXPECT_EQ(deviceHost_->GetLastError(), 0);
}

/**
 * @tc.number    : NFC_DEVICE_HOST_API_0034
 * @tc.name      : Abort_Test
 * @tc.desc      : DeviceHost Abort
 */
TEST_F(DeviceHostTest, Abort_Test)
{
    deviceHost_->Abort();
}

/**
 * @tc.number    : NFC_DEVICE_HOST_API_0035
 * @tc.name      : CheckFirmware_Test
 * @tc.desc      : DeviceHost CheckFirmware
 */
TEST_F(DeviceHostTest, CheckFirmware_Test)
{
    EXPECT_TRUE(deviceHost_->CheckFirmware());
}

/**
 * @tc.number    : NFC_DEVICE_HOST_API_0036
 * @tc.name      : Dump_Test
 * @tc.desc      : DeviceHost Dump
 */
TEST_F(DeviceHostTest, Dump_Test)
{
    int fd = 0;
    deviceHost_->Dump(fd);
}

/**
 * @tc.number    : NFC_DEVICE_HOST_API_0037
 * @tc.name      : FactoryReset_Test
 * @tc.desc      : DeviceHost FactoryReset
 */
TEST_F(DeviceHostTest, FactoryReset_Test)
{
    deviceHost_->FactoryReset();
}

/**
 * @tc.number    : NFC_DEVICE_HOST_API_0038
 * @tc.name      : Shutdown_Test
 * @tc.desc      : DeviceHost Shutdown
 */
TEST_F(DeviceHostTest, Shutdown_Test)
{
    deviceHost_->Shutdown();
}

/**
 * @tc.number    : NFC_DEVICE_HOST_API_0039
 * @tc.name      : AddAidRouting_Test
 * @tc.desc      : DeviceHost AddAidRouting
 */
TEST_F(DeviceHostTest, AddAidRouting_Test)
{
    std::string aid = "";
    int route = 0;
    int aidInfo = 0;
    EXPECT_TRUE(deviceHost_->AddAidRouting(aid, route, aidInfo));
}

/**
 * @tc.number    : NFC_DEVICE_HOST_API_0040
 * @tc.name      : AddAidRouting_Test
 * @tc.desc      : DeviceHost AddAidRouting
 */
TEST_F(DeviceHostTest, AddAidRouting_Fail_Test)
{
    std::string aid = "";
    int route = 0;
    int aidInfo = 0;
    nfcNciMock_->SetAddAidScene(1);
    EXPECT_FALSE(deviceHost_->AddAidRouting(aid, route, aidInfo));
}

/**
 * @tc.number    : NFC_DEVICE_HOST_API_0041
 * @tc.name      : RemoveAidRouting_Test
 * @tc.desc      : DeviceHost RemoveAidRouting
 */
TEST_F(DeviceHostTest, RemoveAidRouting_Test)
{
    std::string aid = "";
    EXPECT_TRUE(deviceHost_->RemoveAidRouting(aid));
}

/**
 * @tc.number    : NFC_DEVICE_HOST_API_0042
 * @tc.name      : RemoveAidRouting_Test
 * @tc.desc      : DeviceHost RemoveAidRouting
 */
TEST_F(DeviceHostTest, RemoveAidRouting__Fail_Test)
{
    std::string aid = "";
    nfcNciMock_->SetRemoveAidScene(1);
    EXPECT_FALSE(deviceHost_->RemoveAidRouting(aid));
}

/**
 * @tc.number    : NFC_DEVICE_HOST_API_0043
 * @tc.name      : CommitRouting_Test
 * @tc.desc      : DeviceHost CommitRouting
 */
TEST_F(DeviceHostTest, CommitRouting_Test)
{
    EXPECT_TRUE(deviceHost_->CommitRouting());
}

/**
 * @tc.number    : NFC_DEVICE_HOST_API_0044
 * @tc.name      : GetAidRoutingTableSize_Test
 * @tc.desc      : DeviceHost GetAidRoutingTableSize
 */
TEST_F(DeviceHostTest, GetAidRoutingTableSize_Test)
{
    EXPECT_EQ(deviceHost_->GetAidRoutingTableSize(), 100);
}

/**
 * @tc.number    : NFC_DEVICE_HOST_API_0045
 * @tc.name      : GetDefaultRoute_Test
 * @tc.desc      : DeviceHost GetDefaultRoute
 */
TEST_F(DeviceHostTest, GetDefaultRoute_Test)
{
    EXPECT_GE(deviceHost_->GetDefaultRoute(), 0);
}

/**
 * @tc.number    : NFC_DEVICE_HOST_API_0046
 * @tc.name      : GetDefaultOffHostRoute_Test
 * @tc.desc      : DeviceHost GetDefaultOffHostRoute
 */
TEST_F(DeviceHostTest, GetDefaultOffHostRoute_Test)
{
    EXPECT_GE(deviceHost_->GetDefaultOffHostRoute(), 0);
}

/**
 * @tc.number    : NFC_DEVICE_HOST_API_0047
 * @tc.name      : GetOffHostUiccRoute_Test
 * @tc.desc      : DeviceHost GetOffHostUiccRoute
 */
TEST_F(DeviceHostTest, GetOffHostUiccRoute_Test)
{
    EXPECT_GE(deviceHost_->GetOffHostUiccRoute().size(), 0U);
}

/**
 * @tc.number    : NFC_DEVICE_HOST_API_0048
 * @tc.name      : GetOffHostEseRoute_Test
 * @tc.desc      : DeviceHost GetOffHostEseRoute
 */
TEST_F(DeviceHostTest, GetOffHostEseRoute_Test)
{
    EXPECT_GE(deviceHost_->GetOffHostEseRoute().size(), 0U);
}

/**
 * @tc.number    : NFC_DEVICE_HOST_API_0049
 * @tc.name      : GetAidMatchingMode_Test
 * @tc.desc      : DeviceHost GetAidMatchingMode
 */
TEST_F(DeviceHostTest, GetAidMatchingMode_Test)
{
    EXPECT_GE(deviceHost_->GetAidMatchingMode(), 0);
}

/**
 * @tc.number    : NFC_DEVICE_HOST_API_0050
 * @tc.name      : GetDefaultIsoDepRouteDestination_Test
 * @tc.desc      : DeviceHost GetDefaultIsoDepRouteDestination
 */
TEST_F(DeviceHostTest, GetDefaultIsoDepRouteDestination_Test)
{
    EXPECT_GE(deviceHost_->GetDefaultIsoDepRouteDestination(), 0);
}

/**
 * @tc.number    : NFC_DEVICE_HOST_API_0051
 * @tc.name      : CanMakeReadOnly_Test
 * @tc.desc      : DeviceHost CanMakeReadOnly
 */
TEST_F(DeviceHostTest, CanMakeReadOnly_Test)
{
    int ndefType = NDEF_TYPE1_TAG;
    EXPECT_TRUE(deviceHost_->CanMakeReadOnly(ndefType));
    ndefType = NDEF_TYPE2_TAG;
    EXPECT_TRUE(deviceHost_->CanMakeReadOnly(ndefType));
    ndefType = NDEF_TYPE3_TAG;
    EXPECT_FALSE(deviceHost_->CanMakeReadOnly(ndefType));
    ndefType = NDEF_TYPE4_TAG;
    EXPECT_FALSE(deviceHost_->CanMakeReadOnly(ndefType));
    ndefType = NDEF_MIFARE_CLASSIC_TAG;
    EXPECT_FALSE(deviceHost_->CanMakeReadOnly(ndefType));
    ndefType = NDEF_UNKNOWN_TYPE;
    EXPECT_FALSE(deviceHost_->CanMakeReadOnly(ndefType));
}

/**
 * @tc.number    : NFC_DEVICE_HOST_API_0052
 * @tc.name      : GetExtendedLengthApdusSupported_Test
 * @tc.desc      : DeviceHost GetExtendedLengthApdusSupported
 */
TEST_F(DeviceHostTest, GetExtendedLengthApdusSupported_Test)
{
    EXPECT_FALSE(deviceHost_->GetExtendedLengthApdusSupported());
}

/**
 * @tc.number    : NFC_DEVICE_HOST_API_0053
 * @tc.name      : RemoteFieldActivated_Test
 * @tc.desc      : DeviceHost RemoteFieldActivated
 */
TEST_F(DeviceHostTest, RemoteFieldActivated_Test)
{
    deviceHost_->RemoteFieldActivated();
}

/**
 * @tc.number    : NFC_DEVICE_HOST_API_0054
 * @tc.name      : RemoteFieldDeactivated_Test
 * @tc.desc      : DeviceHost RemoteFieldDeactivated
 */
TEST_F(DeviceHostTest, RemoteFieldDeactivated_Test)
{
    deviceHost_->RemoteFieldDeactivated();
}

/**
 * @tc.number    : NFC_DEVICE_HOST_API_0055
 * @tc.name      : HostCardEmulationActivated_Test
 * @tc.desc      : DeviceHost HostCardEmulationActivated
 */
TEST_F(DeviceHostTest, HostCardEmulationActivated_Test)
{
    int technology = NFA_TECHNOLOGY_MASK_A;
    deviceHost_->HostCardEmulationActivated(technology);
}

/**
 * @tc.number    : NFC_DEVICE_HOST_API_0056
 * @tc.name      : HostCardEmulationDeactivated_Test
 * @tc.desc      : DeviceHost HostCardEmulationDeactivated
 */
TEST_F(DeviceHostTest, HostCardEmulationDeactivated_Test)
{
    int technology = NFA_TECHNOLOGY_MASK_A;
    deviceHost_->HostCardEmulationDeactivated(technology);
}

/**
 * @tc.number    : NFC_DEVICE_HOST_API_0057
 * @tc.name      : HostCardEmulationDataReceived_Test
 * @tc.desc      : DeviceHost HostCardEmulationDataReceived
 */
TEST_F(DeviceHostTest, HostCardEmulationDataReceived_Test)
{
    int technology = NFA_TECHNOLOGY_MASK_A;
    std::string data = "";
    deviceHost_->HostCardEmulationDataReceived(technology, data);
}

/**
 * @tc.number    : NFC_DEVICE_HOST_API_0058
 * @tc.name      : TagDiscovered_Test
 * @tc.desc      : DeviceHost TagDiscovered
 */
TEST_F(DeviceHostTest, TagDiscovered_Test)
{
    std::shared_ptr<ITagEndPoint> tagEndPoint;
    deviceHost_->TagDiscovered(tagEndPoint);
}

/**
 * @tc.number    : NFC_DEVICE_HOST_API_0059
 * @tc.name      : OffHostTransactionEvent_Test
 * @tc.desc      : DeviceHost OffHostTransactionEvent
 */
TEST_F(DeviceHostTest, OffHostTransactionEvent_Test)
{
    std::string aid = "";
    std::string data = "";
    std::string seName = "";
    deviceHost_->OffHostTransactionEvent(aid, data, seName);
}

/**
 * @tc.number    : NFC_DEVICE_HOST_API_0060
 * @tc.name      : EeUpdate_Test
 * @tc.desc      : DeviceHost EeUpdate
 */
TEST_F(DeviceHostTest, EeUpdate_Test)
{
    deviceHost_->EeUpdate();
}

/**
 * @tc.number    : NFC_DEVICE_HOST_API_0061
 * @tc.name      : TimeOutEvent_Test
 * @tc.desc      : DeviceHost NfcDeviceManagementCallback NFA_DM_NFCC_TIMEOUT_EVT
 */
TEST_F(DeviceHostTest, TimeOutEvent_Test)
{
    tNFA_DM_CBACK_DATA dmEventData;
    NfcNciMock::NfcDeviceManagementCallback(NFA_DM_NFCC_TIMEOUT_EVT, &dmEventData);
}

/**
 * @tc.number    : NFC_DEVICE_HOST_API_0062
 * @tc.name      : RfFieldOnEvent_Test
 * @tc.desc      : DeviceHost NfcDeviceManagementCallback NFA_DM_RF_FIELD_EVT NFA_DM_RF_FIELD_ON
 */
TEST_F(DeviceHostTest, RfFieldOnEvent_Test)
{
    tNFA_DM_CBACK_DATA dmEventData;
    dmEventData.rf_field.status = NFA_STATUS_OK;
    dmEventData.rf_field.rf_field_status = NFA_DM_RF_FIELD_ON;
    NfcNciMock::NfcDeviceManagementCallback(NFA_DM_RF_FIELD_EVT, &dmEventData);
}

/**
 * @tc.number    : NFC_DEVICE_HOST_API_0063
 * @tc.name      : RfFieldOffEvent_Test
 * @tc.desc      : DeviceHost NfcDeviceManagementCallback NFA_DM_RF_FIELD_EVT NFA_DM_RF_FIELD_OFF
 */
TEST_F(DeviceHostTest, RfFieldOffEvent_Test)
{
    tNFA_DM_CBACK_DATA dmEventData;
    dmEventData.rf_field.status = NFA_STATUS_OK;
    dmEventData.rf_field.rf_field_status = NFA_DM_RF_FIELD_OFF;
    NfcNciMock::NfcDeviceManagementCallback(NFA_DM_RF_FIELD_EVT, &dmEventData);
}