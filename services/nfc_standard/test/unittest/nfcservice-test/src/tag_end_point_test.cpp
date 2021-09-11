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
#include "tag_end_point.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <thread>

#include "device_host.h"
#include "nci_bal_tag.h"
#include "nfc_api.h"
#include "nfc_nci_mock.h"
#include "rw_int.h"
#include "test-ncibal/device_host_listener_mock.h"

using namespace OHOS::nfc::ncibal;

class TagEndPointTestListenerDemo : public OHOS::nfc::ncibal::IDeviceHost::IDeviceHostListener {
public:
    TagEndPointTestListenerDemo() : OHOS::nfc::ncibal::DeviceHost::IDeviceHostListener() {}
    virtual ~TagEndPointTestListenerDemo() = default;
    virtual void OnRemoteFieldActivated(){};
    virtual void OnRemoteFieldDeactivated(){};
    virtual void OnHostCardEmulationActivated(int technology){};
    virtual void OnHostCardEmulationDeactivated(int technology){};
    virtual void OnHostCardEmulationDataReceived(int technology, std::string& data){};
    virtual void OnTagDiscovered(std::shared_ptr<ITagEndPoint> tagEndPoint){};
    virtual void OnOffHostTransactionEvent(std::string& aid, std::string& data, std::string& seName){};
    virtual void OnEeUpdate(){};
};

void TagDisconnectedCallbackDemo(int handle) {}

class TagEndPointTest : public ::testing::Test {
public:
    virtual void SetUp() override
    {
        std::cout << " SetUpTestCase." << std::endl;
        std::shared_ptr<TagEndPointTestListenerDemo> listener =
            std::make_shared<TagEndPointTestListenerDemo>(TagEndPointTestListenerDemo());
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

std::shared_ptr<TagEndPoint> GetT1TTag()
{
    // Discover T1T Tag
    tNFA_CONN_EVT_DATA connEventData;
    connEventData.activated.activate_ntf.protocol = NFC_PROTOCOL_T1T;
    connEventData.activated.activate_ntf.rf_tech_param.mode = NCI_DISCOVERY_TYPE_POLL_A;
    connEventData.activated.activate_ntf.intf_param.type = NFC_INTERFACE_FRAME;
    connEventData.activated.activate_ntf.rf_disc_id = 1;
    // uid
    connEventData.activated.activate_ntf.rf_tech_param.param.pa.nfcid1_len = 4;
    std::string uid = {0x01, 0x02, 0x03, 0x04};
    connEventData.activated.activate_ntf.rf_tech_param.param.pa.nfcid1[0] = uid[0];
    connEventData.activated.activate_ntf.rf_tech_param.param.pa.nfcid1[1] = uid[1];
    connEventData.activated.activate_ntf.rf_tech_param.param.pa.nfcid1[2] = uid[2];
    connEventData.activated.activate_ntf.rf_tech_param.param.pa.nfcid1[3] = uid[3];            
    // poll(ATQA)
    std::string atqa = {0x11, 0x12};
    connEventData.activated.activate_ntf.rf_tech_param.param.pa.sens_res[0] = atqa[0];
    connEventData.activated.activate_ntf.rf_tech_param.param.pa.sens_res[1] = atqa[1];
    // act(SAK)
    connEventData.activated.activate_ntf.rf_tech_param.param.pa.sel_rsp = 0x08;
    connEventData.activated.params.t1t.hr[0] = RW_T1T_IS_TOPAZ512;
    NfcNciMock::NfcConnectionCallback(NFA_ACTIVATED_EVT, &connEventData);

    connEventData.activated.params.t1t.hr[0] = RW_T1T_IS_TOPAZ96;
    NfcNciMock::NfcConnectionCallback(NFA_ACTIVATED_EVT, &connEventData);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    std::shared_ptr<TagEndPoint> tagEndPoint;
    std::string poll = {0x11, 0x12};
    std::string act = {0x08};
    std::vector<int> techList;
    techList.push_back(TARGET_TYPE_ISO14443_3A);
    std::vector<int> techHandles;
    techHandles.push_back(1);
    std::vector<int> techLibNfcTypes;
    techLibNfcTypes.push_back(NFA_PROTOCOL_T1T);
    std::vector<std::string> techPollBytes;
    techPollBytes.push_back(poll);
    std::vector<std::string> techActBytes;
    techActBytes.push_back(act);
    tagEndPoint =
        std::make_shared<TagEndPoint>(techList, techHandles, techLibNfcTypes, uid, techPollBytes, techActBytes);
    tagEndPoint->ReadNdef();
    tagEndPoint->GetTechList();
    tagEndPoint->GetTechExtras();
    return tagEndPoint;
}

std::shared_ptr<TagEndPoint> GetT2TTag()
{
    // Discover T2T Tag
    tNFA_CONN_EVT_DATA connEventData;
    connEventData.activated.activate_ntf.protocol = NFC_PROTOCOL_T2T;
    connEventData.activated.params.t1t.hr[0] = RW_T1T_IS_TOPAZ96;
    connEventData.activated.activate_ntf.rf_tech_param.mode = NCI_DISCOVERY_TYPE_POLL_A;
    connEventData.activated.activate_ntf.intf_param.type = NFC_INTERFACE_FRAME;
    connEventData.activated.activate_ntf.rf_disc_id = 1;
    // uid
    connEventData.activated.activate_ntf.rf_tech_param.param.pa.nfcid1_len = 4;
    std::string uid = {0x01, 0x02, 0x03, 0x04};
    connEventData.activated.activate_ntf.rf_tech_param.param.pa.nfcid1[0] = uid[0];
    connEventData.activated.activate_ntf.rf_tech_param.param.pa.nfcid1[1] = uid[1];
    connEventData.activated.activate_ntf.rf_tech_param.param.pa.nfcid1[2] = uid[2];
    connEventData.activated.activate_ntf.rf_tech_param.param.pa.nfcid1[3] = uid[3];
    // poll(ATQA)
    std::string atqa = {0x11, 0x12};
    connEventData.activated.activate_ntf.rf_tech_param.param.pa.sens_res[0] = atqa[0];
    connEventData.activated.activate_ntf.rf_tech_param.param.pa.sens_res[1] = atqa[1];
    // act(SAK)
    connEventData.activated.activate_ntf.rf_tech_param.param.pa.sel_rsp = 0x08;
    NfcNciMock::NfcConnectionCallback(NFA_ACTIVATED_EVT, &connEventData);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    std::shared_ptr<TagEndPoint> tagEndPoint;
    std::string poll = {0x11, 0x12};
    std::string act = {0x08};
    std::vector<int> techList;
    techList.push_back(TARGET_TYPE_ISO14443_3A);
    std::vector<int> techHandles;
    techHandles.push_back(1);
    std::vector<int> techLibNfcTypes;
    techLibNfcTypes.push_back(NFA_PROTOCOL_T2T);
    std::vector<std::string> techPollBytes;
    techPollBytes.push_back(poll);
    std::vector<std::string> techActBytes;
    techActBytes.push_back(act);
    tagEndPoint =
        std::make_shared<TagEndPoint>(techList, techHandles, techLibNfcTypes, uid, techPollBytes, techActBytes);
    tagEndPoint->ReadNdef();
    tagEndPoint->GetTechList();
    tagEndPoint->GetTechExtras();
    return tagEndPoint;
}

std::shared_ptr<TagEndPoint> GetMifareUltralightTag()
{
    // Discover MifareUltralight Tag
    tNFA_CONN_EVT_DATA connEventData;
    connEventData.activated.activate_ntf.protocol = NFC_PROTOCOL_T2T;
    connEventData.activated.params.t1t.hr[0] = RW_T1T_IS_TOPAZ96;
    connEventData.activated.activate_ntf.rf_tech_param.mode = NCI_DISCOVERY_TYPE_POLL_A;
    connEventData.activated.activate_ntf.intf_param.type = NFC_INTERFACE_FRAME;
    connEventData.activated.activate_ntf.rf_disc_id = 1;
    // uid
    connEventData.activated.activate_ntf.rf_tech_param.param.pa.nfcid1_len = 4;
    std::string uid = {0x04, 0x02, 0x03, 0x04};
    connEventData.activated.activate_ntf.rf_tech_param.param.pa.nfcid1[0] = uid[0];
    connEventData.activated.activate_ntf.rf_tech_param.param.pa.nfcid1[1] = uid[1];
    connEventData.activated.activate_ntf.rf_tech_param.param.pa.nfcid1[2] = uid[2];
    connEventData.activated.activate_ntf.rf_tech_param.param.pa.nfcid1[3] = uid[3];
    // poll(ATQA)
    std::string atqa = {0x11, 0x12};
    connEventData.activated.activate_ntf.rf_tech_param.param.pa.sens_res[0] = atqa[0];
    connEventData.activated.activate_ntf.rf_tech_param.param.pa.sens_res[1] = atqa[1];
    // Mifare Ultralight
    connEventData.activated.activate_ntf.rf_tech_param.param.pa.nfcid1[0] = 0x04;
    connEventData.activated.activate_ntf.rf_tech_param.param.pa.sel_rsp = 0x00;
    NfcNciMock::NfcConnectionCallback(NFA_ACTIVATED_EVT, &connEventData);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    std::shared_ptr<TagEndPoint> tagEndPoint;
    std::string poll = {0x11, 0x12};
    std::string act = {0x00};
    std::vector<int> techList;
    techList.push_back(TARGET_TYPE_MIFARE_UL);
    std::vector<int> techHandles;
    techHandles.push_back(1);
    std::vector<int> techLibNfcTypes;
    techLibNfcTypes.push_back(NFA_PROTOCOL_T2T);
    std::vector<std::string> techPollBytes;
    techPollBytes.push_back(poll);
    std::vector<std::string> techActBytes;
    techActBytes.push_back(act);
    tagEndPoint =
        std::make_shared<TagEndPoint>(techList, techHandles, techLibNfcTypes, uid, techPollBytes, techActBytes);
    tagEndPoint->ReadNdef();
    tagEndPoint->GetTechList();
    tagEndPoint->GetTechExtras();
    return tagEndPoint;
}

std::shared_ptr<TagEndPoint> GetT3TTag()
{
    // Discover T3T Tag
    tNFA_CONN_EVT_DATA connEventData;
    connEventData.activated.activate_ntf.protocol = NFC_PROTOCOL_T3T;
    connEventData.activated.activate_ntf.rf_tech_param.mode = NCI_DISCOVERY_TYPE_POLL_F;
    connEventData.activated.activate_ntf.intf_param.type = NFC_INTERFACE_FRAME;
    connEventData.activated.activate_ntf.rf_disc_id = 1;
    // uid
    std::string uid = {0x01, 0x02, 0x03, 0x04, 0x11, 0x12, 0x13, 0x14};
    connEventData.activated.activate_ntf.rf_tech_param.param.pf.nfcid2[0] = uid[0];
    connEventData.activated.activate_ntf.rf_tech_param.param.pf.nfcid2[1] = uid[1];
    connEventData.activated.activate_ntf.rf_tech_param.param.pf.nfcid2[2] = uid[2];
    connEventData.activated.activate_ntf.rf_tech_param.param.pf.nfcid2[3] = uid[3];
    connEventData.activated.activate_ntf.rf_tech_param.param.pf.nfcid2[4] = uid[4];
    connEventData.activated.activate_ntf.rf_tech_param.param.pf.nfcid2[5] = uid[5];
    connEventData.activated.activate_ntf.rf_tech_param.param.pf.nfcid2[6] = uid[6];
    connEventData.activated.activate_ntf.rf_tech_param.param.pf.nfcid2[7] = uid[7];
    // poll
    std::string poll = {0x01, 0x02, 0x03, 0x04, 0x11, 0x12, 0x13, 0x14};
    connEventData.activated.activate_ntf.rf_tech_param.param.pf.sensf_res[0] = poll[0];
    connEventData.activated.activate_ntf.rf_tech_param.param.pf.sensf_res[1] = poll[1];
    connEventData.activated.activate_ntf.rf_tech_param.param.pf.sensf_res[2] = poll[2];
    connEventData.activated.activate_ntf.rf_tech_param.param.pf.sensf_res[3] = poll[3];
    connEventData.activated.activate_ntf.rf_tech_param.param.pf.sensf_res[4] = poll[4];
    connEventData.activated.activate_ntf.rf_tech_param.param.pf.sensf_res[5] = poll[5];
    connEventData.activated.activate_ntf.rf_tech_param.param.pf.sensf_res[6] = poll[6];
    connEventData.activated.activate_ntf.rf_tech_param.param.pf.sensf_res[7] = poll[7];
    unsigned short us = 1;
    connEventData.activated.params.t3t.p_system_codes = &us;
    NfcNciMock::NfcConnectionCallback(NFA_ACTIVATED_EVT, &connEventData);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    std::shared_ptr<TagEndPoint> tagEndPoint;
    std::string act;
    std::vector<int> techList;
    techList.push_back(TARGET_TYPE_FELICA);
    std::vector<int> techHandles;
    techHandles.push_back(1);
    std::vector<int> techLibNfcTypes;
    techLibNfcTypes.push_back(NFA_PROTOCOL_T3T);
    std::vector<std::string> techPollBytes;
    techPollBytes.push_back(poll);
    std::vector<std::string> techActBytes;
    techActBytes.push_back(act);
    tagEndPoint =
        std::make_shared<TagEndPoint>(techList, techHandles, techLibNfcTypes, uid, techPollBytes, techActBytes);
    tagEndPoint->ReadNdef();
    tagEndPoint->GetTechList();
    tagEndPoint->GetTechExtras();
    return tagEndPoint;
}

std::shared_ptr<TagEndPoint> GetT4TTag()
{
    // Discover T4T Tag(TsoDep)
    tNFA_CONN_EVT_DATA connEventData;
    connEventData.activated.activate_ntf.protocol = NFC_PROTOCOL_ISO_DEP;
    connEventData.activated.activate_ntf.intf_param.type = NFC_INTERFACE_ISO_DEP;
    connEventData.activated.activate_ntf.rf_disc_id = 1;

    // Not A OR B
    connEventData.activated.activate_ntf.rf_tech_param.mode = 0x55;
    NfcNciMock::NfcConnectionCallback(NFA_ACTIVATED_EVT, &connEventData);

    connEventData.activated.activate_ntf.rf_tech_param.param.pb.sensb_res_len = 6;
    std::string atq = {0x01, 0x02, 0x03, 0x04, 0x01, 0x02};
    connEventData.activated.activate_ntf.rf_tech_param.param.pb.sensb_res[0] = atq[0];
    connEventData.activated.activate_ntf.rf_tech_param.param.pb.sensb_res[1] = atq[1];
    connEventData.activated.activate_ntf.rf_tech_param.param.pb.sensb_res[2] = atq[2];
    connEventData.activated.activate_ntf.rf_tech_param.param.pb.sensb_res[3] = atq[3];    
    connEventData.activated.activate_ntf.rf_tech_param.param.pb.sensb_res[4] = atq[4];
    connEventData.activated.activate_ntf.rf_tech_param.param.pb.sensb_res[5] = atq[5];
    NfcNciMock::NfcConnectionCallback(NFA_ACTIVATED_EVT, &connEventData);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    std::shared_ptr<TagEndPoint> tagEndPoint;
    std::string uid = {0x01, 0x02, 0x03, 0x04, 0x01, 0x02, 0x03, 0x04};
    std::string poll = {0x01, 0x02, 0x03, 0x04, 0x01, 0x02};
    std::string act = {0x21, 0x22, 0x23, 0x20};
    std::vector<int> techList;
    techList.push_back(TARGET_TYPE_ISO14443_4);
    std::vector<int> techHandles;
    techHandles.push_back(1);
    std::vector<int> techLibNfcTypes;
    techLibNfcTypes.push_back(NFA_PROTOCOL_ISO_DEP);
    std::vector<std::string> techPollBytes;
    techPollBytes.push_back(poll);
    std::vector<std::string> techActBytes;
    techActBytes.push_back(act);
    tagEndPoint =
        std::make_shared<TagEndPoint>(techList, techHandles, techLibNfcTypes, uid, techPollBytes, techActBytes);
    tagEndPoint->ReadNdef();
    tagEndPoint->GetTechList();
    tagEndPoint->GetTechExtras();
    return tagEndPoint;
}

std::shared_ptr<TagEndPoint> GetT4TATag()
{
    // Discover T4T Tag(A)
    tNFA_CONN_EVT_DATA connEventData;
    connEventData.activated.activate_ntf.protocol = NFC_PROTOCOL_ISO_DEP;
    connEventData.activated.activate_ntf.intf_param.type = NFC_INTERFACE_ISO_DEP;
    connEventData.activated.activate_ntf.rf_disc_id = 1;
    connEventData.activated.activate_ntf.rf_tech_param.mode = NCI_DISCOVERY_TYPE_POLL_A;
    // uid
    connEventData.activated.activate_ntf.rf_tech_param.param.pa.nfcid1_len = 4;
    std::string uid = {0x01, 0x02, 0x03, 0x04};
    connEventData.activated.activate_ntf.rf_tech_param.param.pa.nfcid1[0] = uid[0];
    connEventData.activated.activate_ntf.rf_tech_param.param.pa.nfcid1[1] = uid[1];
    connEventData.activated.activate_ntf.rf_tech_param.param.pa.nfcid1[2] = uid[2];
    connEventData.activated.activate_ntf.rf_tech_param.param.pa.nfcid1[3] = uid[3];    
    // poll(ATQA)
    std::string atqa = {0x11, 0x12};
    connEventData.activated.activate_ntf.rf_tech_param.param.pa.sens_res[0] = atqa[0];
    connEventData.activated.activate_ntf.rf_tech_param.param.pa.sens_res[1] = atqa[1];
    // act(historical bytes)
    connEventData.activated.activate_ntf.intf_param.intf_param.pa_iso.his_byte_len = 3;
    std::string hb = {0x11, 0x12, 0x13};
    connEventData.activated.activate_ntf.intf_param.intf_param.pa_iso.his_byte[0] = hb[0];
    connEventData.activated.activate_ntf.intf_param.intf_param.pa_iso.his_byte[1] = hb[1];
    connEventData.activated.activate_ntf.intf_param.intf_param.pa_iso.his_byte[2] = hb[2];
    NfcNciMock::NfcConnectionCallback(NFA_ACTIVATED_EVT, &connEventData);

    connEventData.activated.activate_ntf.rf_tech_param.param.pb.sensb_res_len = 6;
    std::string atq = {0x01, 0x02, 0x03, 0x04, 0x01, 0x02};
    connEventData.activated.activate_ntf.rf_tech_param.param.pb.sensb_res[0] = atq[0];
    connEventData.activated.activate_ntf.rf_tech_param.param.pb.sensb_res[1] = atq[1];
    connEventData.activated.activate_ntf.rf_tech_param.param.pb.sensb_res[2] = atq[2];
    connEventData.activated.activate_ntf.rf_tech_param.param.pb.sensb_res[3] = atq[3];    
    connEventData.activated.activate_ntf.rf_tech_param.param.pb.sensb_res[4] = atq[4];
    connEventData.activated.activate_ntf.rf_tech_param.param.pb.sensb_res[5] = atq[5];
    NfcNciMock::NfcConnectionCallback(NFA_ACTIVATED_EVT, &connEventData);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    std::shared_ptr<TagEndPoint> tagEndPoint;
    std::string poll = {0x01, 0x02, 0x03, 0x04, 0x01, 0x02};
    std::string act = {0x21, 0x22, 0x23, 0x20};
    std::vector<int> techList;
    techList.push_back(TARGET_TYPE_ISO14443_3A);
    techList.push_back(TARGET_TYPE_ISO14443_4);
    std::vector<int> techHandles;
    techHandles.push_back(1);
    techHandles.push_back(1);
    std::vector<int> techLibNfcTypes;
    techLibNfcTypes.push_back(NFA_PROTOCOL_ISO_DEP);
    techLibNfcTypes.push_back(NFA_PROTOCOL_ISO_DEP);
    std::vector<std::string> techPollBytes;
    techPollBytes.push_back(poll);
    techPollBytes.push_back(poll);
    std::vector<std::string> techActBytes;
    techActBytes.push_back(act);
    techActBytes.push_back(act);
    tagEndPoint =
        std::make_shared<TagEndPoint>(techList, techHandles, techLibNfcTypes, uid, techPollBytes, techActBytes);
    tagEndPoint->ReadNdef();
    tagEndPoint->GetTechList();
    tagEndPoint->GetTechExtras();
    return tagEndPoint;
}

std::shared_ptr<TagEndPoint> GetT4TBTag()
{
    // Discover T4T Tag(B)
    tNFA_CONN_EVT_DATA connEventData;
    connEventData.activated.activate_ntf.protocol = NFC_PROTOCOL_ISO_DEP;
    connEventData.activated.activate_ntf.intf_param.type = NFC_INTERFACE_ISO_DEP;
    connEventData.activated.activate_ntf.rf_disc_id = 1;
    connEventData.activated.activate_ntf.rf_tech_param.mode = NCI_DISCOVERY_TYPE_POLL_B;
    // uid
    std::string uidb = {0x01, 0x02, 0x03, 0x04};
    connEventData.activated.activate_ntf.rf_tech_param.param.pb.nfcid0[0] = uidb[0];
    connEventData.activated.activate_ntf.rf_tech_param.param.pb.nfcid0[1] = uidb[1];
    connEventData.activated.activate_ntf.rf_tech_param.param.pb.nfcid0[2] = uidb[2];
    connEventData.activated.activate_ntf.rf_tech_param.param.pb.nfcid0[3] = uidb[3];    
    // act(Higher layer Info)
    connEventData.activated.activate_ntf.intf_param.intf_param.pb_iso.hi_info_len = 4;
    std::string hli = {0x21, 0x22, 0x23, 0x20};
    connEventData.activated.activate_ntf.intf_param.intf_param.pb_iso.hi_info[0] = hli[0];
    connEventData.activated.activate_ntf.intf_param.intf_param.pb_iso.hi_info[1] = hli[1];
    connEventData.activated.activate_ntf.intf_param.intf_param.pb_iso.hi_info[2] = hli[2];
    connEventData.activated.activate_ntf.intf_param.intf_param.pb_iso.hi_info[3] = hli[3];    
    // poll
    connEventData.activated.activate_ntf.rf_tech_param.param.pb.sensb_res_len = 2;
    NfcNciMock::NfcConnectionCallback(NFA_ACTIVATED_EVT, &connEventData);

    connEventData.activated.activate_ntf.rf_tech_param.param.pb.sensb_res_len = 6;
    std::string atq = {0x01, 0x02, 0x03, 0x04, 0x01, 0x02};
    connEventData.activated.activate_ntf.rf_tech_param.param.pb.sensb_res[0] = atq[0];
    connEventData.activated.activate_ntf.rf_tech_param.param.pb.sensb_res[1] = atq[1];
    connEventData.activated.activate_ntf.rf_tech_param.param.pb.sensb_res[2] = atq[2];
    connEventData.activated.activate_ntf.rf_tech_param.param.pb.sensb_res[3] = atq[3];    
    connEventData.activated.activate_ntf.rf_tech_param.param.pb.sensb_res[4] = atq[4];
    connEventData.activated.activate_ntf.rf_tech_param.param.pb.sensb_res[5] = atq[5];
    NfcNciMock::NfcConnectionCallback(NFA_ACTIVATED_EVT, &connEventData);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    std::shared_ptr<TagEndPoint> tagEndPoint;
    std::string poll = {0x01, 0x02, 0x03, 0x04, 0x01, 0x02};
    std::string act = {0x21, 0x22, 0x23, 0x20};
    std::vector<int> techList;
    techList.push_back(TARGET_TYPE_ISO14443_3B);
    techList.push_back(TARGET_TYPE_ISO14443_4);
    std::vector<int> techHandles;
    techHandles.push_back(1);
    techHandles.push_back(1);
    std::vector<int> techLibNfcTypes;
    techLibNfcTypes.push_back(NFA_PROTOCOL_ISO_DEP);
    techLibNfcTypes.push_back(NFA_PROTOCOL_ISO_DEP);
    std::vector<std::string> techPollBytes;
    techPollBytes.push_back(poll);
    techPollBytes.push_back(poll);
    std::vector<std::string> techActBytes;
    techActBytes.push_back(act);
    techActBytes.push_back(act);
    tagEndPoint =
        std::make_shared<TagEndPoint>(techList, techHandles, techLibNfcTypes, uidb, techPollBytes, techActBytes);
    tagEndPoint->ReadNdef();
    tagEndPoint->GetTechList();
    tagEndPoint->GetTechExtras();
    return tagEndPoint;
}

std::shared_ptr<TagEndPoint> GetT5TTag()
{
    // Discover T5T Tag
    tNFA_CONN_EVT_DATA connEventData;
    connEventData.activated.activate_ntf.protocol = NFA_PROTOCOL_T5T;
    connEventData.activated.activate_ntf.intf_param.type = NFC_INTERFACE_FRAME;
    connEventData.activated.activate_ntf.rf_disc_id = 1;
    connEventData.activated.activate_ntf.rf_tech_param.mode = NCI_DISCOVERY_TYPE_LISTEN_ISO15693;
    // uid
    std::string uid = {0x01, 0x02, 0x03, 0x04, 0x01, 0x02, 0x03, 0x04};
    connEventData.activated.params.i93.uid[0] = uid[0];
    connEventData.activated.params.i93.uid[1] = uid[1];
    connEventData.activated.params.i93.uid[2] = uid[2];
    connEventData.activated.params.i93.uid[3] = uid[3];    
    connEventData.activated.params.i93.uid[4] = uid[4];
    connEventData.activated.params.i93.uid[5] = uid[5];    
    connEventData.activated.params.i93.uid[6] = uid[6];
    connEventData.activated.params.i93.uid[7] = uid[7];
    // poll & act
    connEventData.activated.params.i93.afi = 0x06;
    connEventData.activated.params.i93.dsfid = 0x08;
    NfcNciMock::NfcConnectionCallback(NFA_ACTIVATED_EVT, &connEventData);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    std::shared_ptr<TagEndPoint> tagEndPoint;
    std::string poll = {0x06, 0x08};
    std::string act = {0x06, 0x08};
    std::vector<int> techList;
    techList.push_back(TARGET_TYPE_V);
    std::vector<int> techHandles;
    techHandles.push_back(1);
    std::vector<int> techLibNfcTypes;
    techLibNfcTypes.push_back(NFA_PROTOCOL_T5T);
    std::vector<std::string> techPollBytes;
    techPollBytes.push_back(poll);
    std::vector<std::string> techActBytes;
    techActBytes.push_back(act);
    tagEndPoint =
        std::make_shared<TagEndPoint>(techList, techHandles, techLibNfcTypes, uid, techPollBytes, techActBytes);
    tagEndPoint->ReadNdef();
    tagEndPoint->GetTechList();
    tagEndPoint->GetTechExtras();
    return tagEndPoint;
}

std::shared_ptr<TagEndPoint> GetMifareClassicTag()
{
    // Discover MifareClassic Tag
    tNFA_CONN_EVT_DATA connEventData;
    connEventData.activated.activate_ntf.protocol = NFC_PROTOCOL_MIFARE;
    connEventData.activated.activate_ntf.intf_param.type = NFC_INTERFACE_MIFARE;
    connEventData.activated.activate_ntf.rf_disc_id = 1;
    // uid
    connEventData.activated.activate_ntf.rf_tech_param.param.pa.nfcid1_len = 4;
    std::string uid = {0x01, 0x02, 0x03, 0x04};
    connEventData.activated.activate_ntf.rf_tech_param.param.pa.nfcid1[0] = uid[0];
    connEventData.activated.activate_ntf.rf_tech_param.param.pa.nfcid1[1] = uid[1];
    connEventData.activated.activate_ntf.rf_tech_param.param.pa.nfcid1[2] = uid[2];
    connEventData.activated.activate_ntf.rf_tech_param.param.pa.nfcid1[3] = uid[3];    
    // poll(ATQA)
    std::string atqa = {0x11, 0x12};
    connEventData.activated.activate_ntf.rf_tech_param.param.pa.sens_res[0] = atqa[0];
    connEventData.activated.activate_ntf.rf_tech_param.param.pa.sens_res[1] = atqa[1];
    // act(SAK)
    connEventData.activated.activate_ntf.rf_tech_param.param.pa.sel_rsp = 0x08;
    NfcNciMock::NfcConnectionCallback(NFA_ACTIVATED_EVT, &connEventData);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    std::shared_ptr<TagEndPoint> tagEndPoint;
    std::string poll = {0x11, 0x12};
    std::string act = {0x08};
    std::vector<int> techList;
    techList.push_back(TARGET_TYPE_MIFARE_CLASSIC);
    std::vector<int> techHandles;
    techHandles.push_back(1);
    std::vector<int> techLibNfcTypes;
    techLibNfcTypes.push_back(NFC_PROTOCOL_MIFARE);
    std::vector<std::string> techPollBytes;
    techPollBytes.push_back(poll);
    std::vector<std::string> techActBytes;
    techActBytes.push_back(act);
    tagEndPoint =
        std::make_shared<TagEndPoint>(techList, techHandles, techLibNfcTypes, uid, techPollBytes, techActBytes);
    tagEndPoint->ReadNdef();
    tagEndPoint->GetTechList();
    tagEndPoint->GetTechExtras();
    return tagEndPoint;
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0001
 * @tc.name      : Discover_T1T_Tag_Test
 * @tc.desc      : TagEndPoint discover T1T tag test
 */
TEST_F(TagEndPointTest, Discover_T1T_Tag_Test)
{
    GetT1TTag();
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0002
 * @tc.name      : Discover_T2T_Tag_Test
 * @tc.desc      : TagEndPoint discover T2T tag
 */
TEST_F(TagEndPointTest, Discover_T2T_Tag_Test)
{
    GetT2TTag();
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0003
 * @tc.name      : Discover_MifareUltralight_Tag_Test
 * @tc.desc      : TagEndPoint discover MifareUltralight tag
 */
TEST_F(TagEndPointTest, Discover_MifareUltralight_Tag_Test)
{
    GetMifareUltralightTag();
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0004
 * @tc.name      : Discover_T3T_Tag_Test
 * @tc.desc      : TagEndPoint discover T3T tag
 */
TEST_F(TagEndPointTest, Discover_T3T_Tag_Test)
{
    GetT3TTag();
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0005
 * @tc.name      : Discover_T4T_Tag_Test
 * @tc.desc      : TagEndPoint discover T4T(IsoDep) tag
 */
TEST_F(TagEndPointTest, Discover_T4T_Tag_Test)
{
    GetT4TTag();
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0006
 * @tc.name      : Discover_ISO14443_3A_Tag_Test
 * @tc.desc      : TagEndPoint discover T4T(A) tag
 */
TEST_F(TagEndPointTest, Discover_ISO14443_3A_Tag_Test)
{
    GetT4TATag();
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0007
 * @tc.name      : Discover_ISO14443_3B_Tag_Test
 * @tc.desc      : TagEndPoint discover T4T(B) tag
 */
TEST_F(TagEndPointTest, Discover_ISO14443_3B_Tag_Test)
{
    GetT4TBTag();
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0008
 * @tc.name      : Discover_T5T_Tag_Test
 * @tc.desc      : TagEndPoint discover T5T tag
 */
TEST_F(TagEndPointTest, Discover_T5T_Tag_Test)
{
    GetT5TTag();
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0009
 * @tc.name      : Discover_MifareClassic_Tag_Test
 * @tc.desc      : TagEndPoint discover MifareClassic tag
 */
TEST_F(TagEndPointTest, Discover_MifareClassic_Tag_Test)
{
    GetMifareClassicTag();
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0010
 * @tc.name      : Discover_UnknownTag_Test
 * @tc.desc      : TagEndPoint discover unknown tag
 */
TEST_F(TagEndPointTest, Discover_UnknownTag_Test)
{
    tNFA_CONN_EVT_DATA connEventData;
    connEventData.activated.activate_ntf.protocol = NFC_INTERFACE_FRAME;
    connEventData.activated.activate_ntf.intf_param.type = 0x55;
    connEventData.activated.activate_ntf.rf_disc_id = 1;
    connEventData.activated.activate_ntf.rf_tech_param.mode = 0xFF;
    NfcNciMock::NfcConnectionCallback(NFA_ACTIVATED_EVT, &connEventData);
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0011
 * @tc.name      : Discover_P2P_Test
 * @tc.desc      : TagEndPoint discover NfcDep
 */
TEST_F(TagEndPointTest, Discover_P2P_Test)
{
    tNFA_CONN_EVT_DATA connEventData;
    connEventData.activated.activate_ntf.protocol = NCI_PROTOCOL_NFC_DEP;
    NfcNciMock::NfcConnectionCallback(NFA_ACTIVATED_EVT, &connEventData);
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0012
 * @tc.name      : Connect_T1T_Test
 * @tc.desc      : TagEndPoint Connect T1T tag
 */
TEST_F(TagEndPointTest, Connect_T1T_Test)
{
    EXPECT_TRUE(GetT1TTag()->Connect(EnumNcibalTagTechnology::ENUM_NFC_ISODEP_TECH));
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0013
 * @tc.name      : Connect_T2T_Test
 * @tc.desc      : TagEndPoint Connect T2T tag
 */
TEST_F(TagEndPointTest, Connect_T2T_Test)
{
    EXPECT_TRUE(GetT2TTag()->Connect(EnumNcibalTagTechnology::ENUM_NFC_ISODEP_TECH));
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0014
 * @tc.name      : Connect_NDEF_Test
 * @tc.desc      : TagEndPoint Connect NDEF tag
 */
TEST_F(TagEndPointTest, Connect_NDEF_Test)
{
    EXPECT_TRUE(GetT2TTag()->Connect(EnumNcibalTagTechnology::ENUM_NFC_NDEF_TECH));
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0015
 * @tc.name      : Connect_MifareUltralight_Test
 * @tc.desc      : TagEndPoint Connect MifareUltralight tag
 */
TEST_F(TagEndPointTest, Connect_MifareUltralight_Test)
{
    EXPECT_TRUE(GetMifareUltralightTag()->Connect(EnumNcibalTagTechnology::ENUM_NFC_MIFARE_ULTRALIGHT_TECH));
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0016
 * @tc.name      : Connect_T3T_Test
 * @tc.desc      : TagEndPoint Connect T3T tag
 */
TEST_F(TagEndPointTest, Connect_T3T_Test)
{
    EXPECT_TRUE(GetT3TTag()->Connect(EnumNcibalTagTechnology::ENUM_NFC_FELICA_TECH));
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0017
 * @tc.name      : Connect_T4T_Test
 * @tc.desc      : TagEndPoint Connect T4T tag
 */
TEST_F(TagEndPointTest, Connect_T4T_Test)
{
    EXPECT_TRUE(GetT4TTag()->Connect(EnumNcibalTagTechnology::ENUM_NFC_ISODEP_TECH));
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0018
 * @tc.name      : Connect_T5T_Test
 * @tc.desc      : TagEndPoint Connect T5T tag
 */
TEST_F(TagEndPointTest, Connect_T5T_Test)
{
    EXPECT_TRUE(GetT5TTag()->Connect(EnumNcibalTagTechnology::ENUM_NFC_ISO15693_TECH));
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0019
 * @tc.name      : Connect_MifareClassic_Test
 * @tc.desc      : TagEndPoint Connect MifareClassic tag
 */
TEST_F(TagEndPointTest, Connect_MifareClassic_Test)
{
    EXPECT_TRUE(GetMifareClassicTag()->Connect(EnumNcibalTagTechnology::ENUM_NFC_MIFARE_CLASSIC_TECH));
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0020
 * @tc.name      : Connect_Nfc_Off_Test
 * @tc.desc      : TagEndPoint Connect tag when nfc off
 */
TEST_F(TagEndPointTest, Connect_Nfc_Off_Test)
{
    std::shared_ptr<TagEndPoint> mTagEndPoint = GetT2TTag();
    EXPECT_TRUE(deviceHost_->Deinitialize());
    EXPECT_FALSE(mTagEndPoint->Connect(EnumNcibalTagTechnology::ENUM_NFC_ISODEP_TECH));
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0021
 * @tc.name      : Connect_Diff_Tech_Test
 * @tc.desc      : TagEndPoint Connect tag with diff tech
 */
TEST_F(TagEndPointTest, Connect_Diff_Tech_Test)
{
    EXPECT_FALSE(GetT2TTag()->Connect(EnumNcibalTagTechnology::ENUM_NFC_MIFARE_CLASSIC_TECH));
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0022
 * @tc.name      : Connect_Failed_Test
 * @tc.desc      : TagEndPoint Connect tag NFA_Select return NFA_STATUS_FAILED
 */
TEST_F(TagEndPointTest, Connect_Failed_Test)
{
    std::shared_ptr<TagEndPoint> tagEndPoint = GetT2TTag();
    nfcNciMock_->SetSelectScene(1);
    EXPECT_FALSE(tagEndPoint->Connect(EnumNcibalTagTechnology::ENUM_NFC_ISODEP_TECH));
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0023
 * @tc.name      : Connect_Timeout_Test
 * @tc.desc      : TagEndPoint Connect tag NFA_Select with no NFA_SELECT_RESULT_EVT
 */
TEST_F(TagEndPointTest, Connect_Timeout_Test)
{
    std::shared_ptr<TagEndPoint> tagEndPoint = GetT2TTag();
    nfcNciMock_->SetSelectScene(2);
    EXPECT_FALSE(tagEndPoint->Connect(EnumNcibalTagTechnology::ENUM_NFC_ISODEP_TECH));
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0024
 * @tc.name      : Disconnect_T1T_Test
 * @tc.desc      : TagEndPoint Disconnect T1T tag
 */
TEST_F(TagEndPointTest, Disconnect_T1T_Test)
{
    EXPECT_TRUE(GetT1TTag()->Disconnect());
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0025
 * @tc.name      : Disconnect_T2T_Test
 * @tc.desc      : TagEndPoint Disconnect T2T tag
 */
TEST_F(TagEndPointTest, Disconnect_T2T_Test)
{
    EXPECT_TRUE(GetT2TTag()->Disconnect());
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0026
 * @tc.name      : Disconnect_MifareUltralight_Test
 * @tc.desc      : TagEndPoint Disconnect MifareUltralight tag
 */
TEST_F(TagEndPointTest, Disconnect_MifareUltralight_Test)
{
    EXPECT_TRUE(GetMifareUltralightTag()->Disconnect());
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0027
 * @tc.name      : Disconnect_T3T_Test
 * @tc.desc      : TagEndPoint Disconnect T3T tag
 */
TEST_F(TagEndPointTest, Disconnect_T3T_Test)
{
    EXPECT_TRUE(GetT3TTag()->Disconnect());
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0028
 * @tc.name      : Disconnect_T4T_Test
 * @tc.desc      : TagEndPoint Disconnect T4T tag
 */
TEST_F(TagEndPointTest, Disconnect_T4T_Test)
{
    EXPECT_TRUE(GetT4TTag()->Disconnect());
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0029
 * @tc.name      : Disconnect_T5T_Test
 * @tc.desc      : TagEndPoint Disconnect T5T tag
 */
TEST_F(TagEndPointTest, Disconnect_T5T_Test)
{
    EXPECT_TRUE(GetT5TTag()->Disconnect());
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0030
 * @tc.name      : Disconnect_MifareClassic_Test
 * @tc.desc      : TagEndPoint Disconnect MifareClassic tag test
 */
TEST_F(TagEndPointTest, Disconnect_MifareClassic_Test)
{
    EXPECT_TRUE(GetMifareClassicTag()->Disconnect());
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0031
 * @tc.name      : Disconnect_Nfc_Off_Test
 * @tc.desc      : TagEndPoint Disconnect when nfc off
 */
TEST_F(TagEndPointTest, Disconnect_Nfc_Off_Test)
{
    std::shared_ptr<TagEndPoint> tagEndPoint = GetT2TTag();
    EXPECT_TRUE(deviceHost_->Deinitialize());
    EXPECT_TRUE(tagEndPoint->Disconnect());
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0032
 * @tc.name      : Disconnect_Failed_Test
 * @tc.desc      : TagEndPoint Disconnect NFA_Deactivate return NFA_STATUS_FAILED
 */
TEST_F(TagEndPointTest, Disconnect_Failed_Test)
{
    std::shared_ptr<TagEndPoint> tagEndPoint = GetT2TTag();
    EXPECT_TRUE(tagEndPoint->Connect(EnumNcibalTagTechnology::ENUM_NFC_ISODEP_TECH));
    nfcNciMock_->SetDeactivateScene(1);
    EXPECT_FALSE(tagEndPoint->Disconnect());
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0033
 * @tc.name      : Reconnect_Test
 * @tc.desc      : TagEndPoint Reconnect with diff tag
 */
TEST_F(TagEndPointTest, Reconnect_Test)
{
    std::shared_ptr<TagEndPoint> tagEndPoint2 = GetMifareClassicTag();
    EXPECT_TRUE(tagEndPoint2->Connect(EnumNcibalTagTechnology::ENUM_NFC_MIFARE_CLASSIC_TECH));
    std::shared_ptr<TagEndPoint> tagEndPoint = GetT2TTag();
    EXPECT_TRUE(tagEndPoint->Reconnect());
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0034
 * @tc.name      : Reconnect_T1T_Test
 * @tc.desc      : TagEndPoint Reconnect T1T tag
 */
TEST_F(TagEndPointTest, Reconnect_T1T_Test)
{
    std::shared_ptr<TagEndPoint> tagEndPoint = GetT1TTag();
    EXPECT_TRUE(tagEndPoint->Connect(EnumNcibalTagTechnology::ENUM_NFC_ISODEP_TECH));
    EXPECT_TRUE(tagEndPoint->Reconnect());
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0035
 * @tc.name      : Reconnect_T2T_Test
 * @tc.desc      : TagEndPoint Reconnect T2T tag
 */
TEST_F(TagEndPointTest, Reconnect_T2T_Test)
{
    std::shared_ptr<TagEndPoint> tagEndPoint = GetT2TTag();
    EXPECT_TRUE(tagEndPoint->Connect(EnumNcibalTagTechnology::ENUM_NFC_ISODEP_TECH));
    EXPECT_TRUE(tagEndPoint->Reconnect());
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0036
 * @tc.name      : Reconnect_MifareUltralight_Test
 * @tc.desc      : TagEndPoint Reconnect MifareUltralight tag
 */
TEST_F(TagEndPointTest, Reconnect_MifareUltralight_Test)
{
    std::shared_ptr<TagEndPoint> tagEndPoint = GetMifareUltralightTag();
    EXPECT_TRUE(tagEndPoint->Connect(EnumNcibalTagTechnology::ENUM_NFC_MIFARE_ULTRALIGHT_TECH));
    EXPECT_TRUE(tagEndPoint->Reconnect());
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0037
 * @tc.name      : Reconnect_T3T_Test
 * @tc.desc      : TagEndPoint Reconnect T3T tag
 */
TEST_F(TagEndPointTest, Reconnect_T3T_Test)
{
    std::shared_ptr<TagEndPoint> tagEndPoint = GetT3TTag();
    EXPECT_TRUE(tagEndPoint->Connect(EnumNcibalTagTechnology::ENUM_NFC_FELICA_TECH));
    EXPECT_TRUE(tagEndPoint->Reconnect());
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0038
 * @tc.name      : Reconnect_T4T_Test
 * @tc.desc      : TagEndPoint Reconnect T4T tag
 */
TEST_F(TagEndPointTest, Reconnect_T4T_Test)
{
    std::shared_ptr<TagEndPoint> tagEndPoint = GetT4TTag();
    EXPECT_TRUE(tagEndPoint->Connect(EnumNcibalTagTechnology::ENUM_NFC_ISODEP_TECH));
    EXPECT_TRUE(tagEndPoint->Reconnect());
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0039
 * @tc.name      : Reconnect_T5T_Test
 * @tc.desc      : TagEndPoint Reconnect T5T tag
 */
TEST_F(TagEndPointTest, Reconnect_T5T_Test)
{
    std::shared_ptr<TagEndPoint> tagEndPoint = GetT5TTag();
    EXPECT_TRUE(tagEndPoint->Connect(EnumNcibalTagTechnology::ENUM_NFC_ISO15693_TECH));
    EXPECT_TRUE(tagEndPoint->Reconnect());
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0040
 * @tc.name      : Reconnect_MifareClassic_Test
 * @tc.desc      : TagEndPoint Reconnect MifareClassic tag
 */
TEST_F(TagEndPointTest, Reconnect_MifareClassic_Test)
{
    std::shared_ptr<TagEndPoint> tagEndPoint = GetMifareClassicTag();
    EXPECT_TRUE(tagEndPoint->Connect(EnumNcibalTagTechnology::ENUM_NFC_MIFARE_CLASSIC_TECH));
    EXPECT_TRUE(tagEndPoint->Reconnect());
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0041
 * @tc.name      : Transceive_T1T_Test
 * @tc.desc      : TagEndPoint Transceive T1T tag
 */
TEST_F(TagEndPointTest, Transceive_T1T_Test)
{
    std::string request = "req";
    std::string response;
    EXPECT_EQ(GetT1TTag()->Transceive(request, response), NFA_STATUS_OK);
    EXPECT_NE(response, "");
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0042
 * @tc.name      : Transceive_T2T_Test
 * @tc.desc      : TagEndPoint Transceive T2T tag
 */
TEST_F(TagEndPointTest, Transceive_T2T_Test)
{
    std::string request = "req";
    std::string response;
    EXPECT_EQ(GetT2TTag()->Transceive(request, response), NFA_STATUS_OK);
    EXPECT_NE(response, "");
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0043
 * @tc.name      : Transceive_MifareUltralight_Test
 * @tc.desc      : TagEndPoint Transceive MifareUltralight tag
 */
TEST_F(TagEndPointTest, Transceive_MifareUltralight_Test)
{
    std::string request = "req";
    std::string response;
    EXPECT_EQ(GetMifareUltralightTag()->Transceive(request, response), NFA_STATUS_OK);
    EXPECT_NE(response, "");
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0044
 * @tc.name      : Transceive_T3T_Test
 * @tc.desc      : TagEndPoint Transceive T3T tag
 */
TEST_F(TagEndPointTest, Transceive_T3T_Test)
{
    std::string request = "req";
    std::string response;
    EXPECT_EQ(GetT3TTag()->Transceive(request, response), NFA_STATUS_OK);
    EXPECT_NE(response, "");
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0045
 * @tc.name      : Transceive_T4T_Test
 * @tc.desc      : TagEndPoint Transceive T4T tag
 */
TEST_F(TagEndPointTest, Transceive_T4T_Test)
{
    std::string request = "req";
    std::string response;
    EXPECT_EQ(GetT4TTag()->Transceive(request, response), NFA_STATUS_OK);
    EXPECT_NE(response, "");
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0046
 * @tc.name      : Transceive_T5T_Test
 * @tc.desc      : TagEndPoint Transceive T5T tag
 */
TEST_F(TagEndPointTest, Transceive_T5T_Test)
{
    std::string request = "req";
    std::string response;
    EXPECT_EQ(GetT5TTag()->Transceive(request, response), NFA_STATUS_OK);
    EXPECT_NE(response, "");
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0047
 * @tc.name      : Transceive_MifareClassic_Test
 * @tc.desc      : TagEndPoint Transceive MifareClassic tag
 */
TEST_F(TagEndPointTest, Transceive_MifareClassic_Test)
{
    std::string request = "req";
    std::string response;
    // nfcNciMock->SetExtnsGetCallBackFlagScene(1);
    EXPECT_EQ(GetMifareClassicTag()->Transceive(request, response), NFA_STATUS_OK);
    EXPECT_NE(response, "");
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0048
 * @tc.name      : Transceive_Nfc_Off_Test
 * @tc.desc      : TagEndPoint Transceive when nfc off
 */
TEST_F(TagEndPointTest, Transceive_Nfc_Off_Test)
{
    std::string request = "req";
    std::string response;
    std::shared_ptr<TagEndPoint> tagEndPoint = GetT2TTag();
    EXPECT_TRUE(deviceHost_->Deinitialize());
    EXPECT_NE(tagEndPoint->Transceive(request, response), NFA_STATUS_OK);
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0049
 * @tc.name      : Transceive_Failed_Test
 * @tc.desc      : TagEndPoint Transceive NFA_SendRawFrame return NFA_STATUS_FAILED
 */
TEST_F(TagEndPointTest, Transceive_Failed_Test)
{
    std::string request = "req";
    std::string response;
    nfcNciMock_->SetSendRawFrameScene(1);
    EXPECT_NE(GetT2TTag()->Transceive(request, response), NFA_STATUS_OK);
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0050
 * @tc.name      : Transceive_Timeout_Test
 * @tc.desc      : TagEndPoint Transceive NFA_SendRawFrame with no NFA_DATA_EVT
 */
TEST_F(TagEndPointTest, Transceive_Timeout_Test)
{
    std::string request = "req";
    std::string response;
    nfcNciMock_->SetSendRawFrameScene(2);
    EXPECT_NE(GetT2TTag()->Transceive(request, response), NFA_STATUS_OK);
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0051
 * @tc.name      : PresenceCheck_T1T_Test
 * @tc.desc      : TagEndPoint PresenceCheck T1T tag
 */
TEST_F(TagEndPointTest, PresenceCheck_T1T_Test)
{
    EXPECT_TRUE(GetT1TTag()->PresenceCheck());
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0052
 * @tc.name      : PresenceCheck_T2T_Test
 * @tc.desc      : TagEndPoint PresenceCheck T2T tag
 */
TEST_F(TagEndPointTest, PresenceCheck_T2T_Test)
{
    EXPECT_TRUE(GetT2TTag()->PresenceCheck());
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0053
 * @tc.name      : PresenceCheck_MifareUltralight_Test
 * @tc.desc      : TagEndPoint PresenceCheck MifareUltralight tag
 */
TEST_F(TagEndPointTest, PresenceCheck_MifareUltralight_Test)
{
    EXPECT_TRUE(GetMifareUltralightTag()->PresenceCheck());
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0054
 * @tc.name      : PresenceCheck_T3T_Test
 * @tc.desc      : TagEndPoint PresenceCheck T3T tag
 */
TEST_F(TagEndPointTest, PresenceCheck_T3T_Test)
{
    EXPECT_TRUE(GetT3TTag()->PresenceCheck());
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0055
 * @tc.name      : PresenceCheck_T4T_Test
 * @tc.desc      : TagEndPoint PresenceCheck T4T tag
 */
TEST_F(TagEndPointTest, PresenceCheck_T4T_Test)
{
    EXPECT_TRUE(GetT4TTag()->PresenceCheck());
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0056
 * @tc.name      : PresenceCheck_T5T_Test
 * @tc.desc      : TagEndPoint PresenceCheck T5T tag
 */
TEST_F(TagEndPointTest, PresenceCheck_T5T_Test)
{
    EXPECT_TRUE(GetT5TTag()->PresenceCheck());
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0057
 * @tc.name      : PresenceCheck_MifareClassic_Test
 * @tc.desc      : TagEndPoint PresenceCheck MifareClassic tag
 */
TEST_F(TagEndPointTest, PresenceCheck_MifareClassic_Test)
{
    EXPECT_TRUE(GetMifareClassicTag()->PresenceCheck());
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0058
 * @tc.name      : PresenceCheck_Nfc_Off_Test
 * @tc.desc      : TagEndPoint PresenceCheck when nfc off
 */
TEST_F(TagEndPointTest, PresenceCheck_Nfc_Off_Test)
{
    std::shared_ptr<TagEndPoint> tagEndPoint = GetT2TTag();
    EXPECT_TRUE(deviceHost_->Deinitialize());
    EXPECT_FALSE(tagEndPoint->PresenceCheck());
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0059
 * @tc.name      : PresenceCheck_Failed_Test
 * @tc.desc      : TagEndPoint PresenceCheck NFA_PRESENCE_CHECK_EVT event status NFA_STATUS_FAILED
 */
TEST_F(TagEndPointTest, PresenceCheck_Failed_Test)
{
    nfcNciMock_->SetRwPresenceCheckScene(1);
    EXPECT_FALSE(GetT2TTag()->PresenceCheck());
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0060
 * @tc.name      : PresenceCheck_Timeout_Test
 * @tc.desc      : TagEndPoint PresenceCheck with no NFA_PRESENCE_CHECK_EVT
 */
TEST_F(TagEndPointTest, PresenceCheck_Timeout_Test)
{
    nfcNciMock_->SetRwPresenceCheckScene(2);
    EXPECT_FALSE(GetT2TTag()->PresenceCheck());
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0061
 * @tc.name      : IsPresent_Test
 * @tc.desc      : TagEndPoint IsPresent
 */
TEST_F(TagEndPointTest, IsPresent_Test)
{
    EXPECT_TRUE(GetT2TTag()->IsPresent());
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0062
 * @tc.name      : StopPresenceChecking_Test
 * @tc.desc      : TagEndPoint StopPresenceChecking
 */
TEST_F(TagEndPointTest, StopPresenceChecking_Test)
{
    std::shared_ptr<TagEndPoint> tagEndPoint = GetT2TTag();
    tagEndPoint->StartPresenceChecking(0, nullptr);
    tagEndPoint->StopPresenceChecking();
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0063
 * @tc.name      : PresenceChecking_Failed_Test
 * @tc.desc      : TagEndPoint PresenceChecking NFA_PRESENCE_CHECK_EVT event status failed
 */
TEST_F(TagEndPointTest, PresenceChecking_Failed_Test)
{
    std::shared_ptr<TagEndPoint> tagEndPoint = GetT2TTag();
    nfcNciMock_->SetRwPresenceCheckScene(1);
    tagEndPoint->StartPresenceChecking(100, &TagDisconnectedCallbackDemo);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0064
 * @tc.name      : GetTechList_T1T_Test
 * @tc.desc      : TagEndPoint GetTechList T1T tag
 */
TEST_F(TagEndPointTest, GetTechList_T1T_Test)
{
    EXPECT_NE(0, (int)GetT1TTag()->GetTechList().size());
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0065
 * @tc.name      : GetTechList_T2T_Test
 * @tc.desc      : TagEndPoint GetTechList T2T tag
 */
TEST_F(TagEndPointTest, GetTechList_T2T_Test)
{
    EXPECT_NE(0, (int)GetT2TTag()->GetTechList().size());
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0066
 * @tc.name      : GetTechList_MifareUltralight_Test
 * @tc.desc      : TagEndPoint GetTechList MifareUltralight tag
 */
TEST_F(TagEndPointTest, GetTechList_MifareUltralight_Test)
{
    EXPECT_NE(0, (int)GetMifareUltralightTag()->GetTechList().size());
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0067
 * @tc.name      : GetTechList_T3T_Test
 * @tc.desc      : TagEndPoint GetTechList T3T tag
 */
TEST_F(TagEndPointTest, GetTechList_T3T_Test)
{
    EXPECT_NE(0, (int)GetT3TTag()->GetTechList().size());
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0068
 * @tc.name      : GetTechList_T4T_Test
 * @tc.desc      : TagEndPoint GetTechList T4T tag
 */
TEST_F(TagEndPointTest, GetTechList_T4T_Test)
{
    EXPECT_NE(0, (int)GetT4TTag()->GetTechList().size());
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0069
 * @tc.name      : GetTechList_T5T_Test
 * @tc.desc      : TagEndPoint GetTechList T5T tag
 */
TEST_F(TagEndPointTest, GetTechList_T5T_Test)
{
    EXPECT_NE(0, (int)GetT5TTag()->GetTechList().size());
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0070
 * @tc.name      : GetTechList_MifareClassic_Test
 * @tc.desc      : TagEndPoint GetTechList MifareClassic tag
 */
TEST_F(TagEndPointTest, GetTechList_MifareClassic_Test)
{
    EXPECT_NE(0, (int)GetMifareClassicTag()->GetTechList().size());
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0071
 * @tc.name      : RemoveTechnology_Test
 * @tc.desc      : TagEndPoint RemoveTechnology
 */
TEST_F(TagEndPointTest, RemoveTechnology_Test)
{
    int technology = -1;
    GetT2TTag()->RemoveTechnology(technology);
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0072
 * @tc.name      : GetUid_Test
 * @tc.desc      : TagEndPoint GetUid
 */
TEST_F(TagEndPointTest, GetUid_Test)
{
    EXPECT_NE(GetT2TTag()->GetUid(), "");
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0073
 * @tc.name      : GetTechExtras_T1T_Test
 * @tc.desc      : TagEndPoint GetTechExtras T1T tag
 */
TEST_F(TagEndPointTest, GetTechExtras_T1T_Test)
{
    GetT1TTag()->GetTechExtras();
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0074
 * @tc.name      : GetTechExtras_T2T_Test
 * @tc.desc      : TagEndPoint GetTechExtras T2T tag
 */
TEST_F(TagEndPointTest, GetTechExtras_T2T_Test)
{
    GetT2TTag()->GetTechExtras();
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0075
 * @tc.name      : GetTechExtras_MifareUltralight_Test
 * @tc.desc      : TagEndPoint GetTechExtras MifareUltralight tag
 */
TEST_F(TagEndPointTest, GetTechExtras_MifareUltralight_Test)
{
    GetMifareUltralightTag()->GetTechExtras();
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0076
 * @tc.name      : GetTechExtras_T3T_Test
 * @tc.desc      : TagEndPoint GetTechExtras T3T tag
 */
TEST_F(TagEndPointTest, GetTechExtras_T3T_Test)
{
    GetT3TTag()->GetTechExtras();
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0077
 * @tc.name      : GetTechExtras_T4T_Test
 * @tc.desc      : TagEndPoint GetTechExtras T4T tag
 */
TEST_F(TagEndPointTest, GetTechExtras_T4T_Test)
{
    GetT4TTag()->GetTechExtras();
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0078
 * @tc.name      : GetTechExtras_T5T_Test
 * @tc.desc      : TagEndPoint GetTechExtras T5T tag
 */
TEST_F(TagEndPointTest, GetTechExtras_T5T_Test)
{
    GetT5TTag()->GetTechExtras();
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0079
 * @tc.name      : GetTechExtras_MifareClassic_Test
 * @tc.desc      : TagEndPoint GetTechExtras MifareClassic tag
 */
TEST_F(TagEndPointTest, GetTechExtras_MifareClassic_Test)
{
    GetMifareClassicTag()->GetTechExtras();
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0080
 * @tc.name      : GetHandle_Test
 * @tc.desc      : TagEndPoint GetHandle
 */
TEST_F(TagEndPointTest, GetHandle_Test)
{
    EXPECT_EQ(GetT2TTag()->GetHandle(), 1);
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0081
 * @tc.name      : MakeReadOnly_T1T_Test
 * @tc.desc      : TagEndPoint MakeReadOnly T1T tag
 */
TEST_F(TagEndPointTest, MakeReadOnly_T1T_Test)
{
    EXPECT_TRUE(GetT1TTag()->MakeReadOnly());
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0082
 * @tc.name      : MakeReadOnly_T2T_Test
 * @tc.desc      : TagEndPoint MakeReadOnly T2T tag
 */
TEST_F(TagEndPointTest, MakeReadOnly_T2T_Test)
{
    EXPECT_TRUE(GetT2TTag()->MakeReadOnly());
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0083
 * @tc.name      : MakeReadOnly_MifareUltralight_Test
 * @tc.desc      : TagEndPoint MakeReadOnly MifareUltralight tag
 */
TEST_F(TagEndPointTest, MakeReadOnly_MifareUltralight_Test)
{
    EXPECT_TRUE(GetMifareUltralightTag()->MakeReadOnly());
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0084
 * @tc.name      : MakeReadOnly_T3T_Test
 * @tc.desc      : TagEndPoint MakeReadOnly T3T tag
 */
TEST_F(TagEndPointTest, MakeReadOnly_T3T_Test)
{
    EXPECT_TRUE(GetT3TTag()->MakeReadOnly());
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0085
 * @tc.name      : MakeReadOnly_T4T_Test
 * @tc.desc      : TagEndPoint MakeReadOnly T4T tag
 */
TEST_F(TagEndPointTest, MakeReadOnly_T4T_Test)
{
    EXPECT_TRUE(GetT4TTag()->MakeReadOnly());
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0086
 * @tc.name      : MakeReadOnly_T5T_Test
 * @tc.desc      : TagEndPoint MakeReadOnly T5T tag
 */
TEST_F(TagEndPointTest, MakeReadOnly_T5T_Test)
{
    EXPECT_TRUE(GetT5TTag()->MakeReadOnly());
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0087
 * @tc.name      : MakeReadOnly_MifareClassic_Test
 * @tc.desc      : TagEndPoint MakeReadOnly MifareClassic tag
 */
TEST_F(TagEndPointTest, MakeReadOnly_MifareClassic_Test)
{
    EXPECT_TRUE(GetMifareClassicTag()->MakeReadOnly());
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0088
 * @tc.name      : MakeReadOnly_SoftLock_Test
 * @tc.desc      : TagEndPoint MakeReadOnly NFA_RwSetTagReadOnly return rejected
 */
TEST_F(TagEndPointTest, MakeReadOnly_SoftLock_Test)
{
    nfcNciMock_->SetRwSetTagReadOnlyScene(1);
    EXPECT_TRUE(GetT2TTag()->MakeReadOnly());
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0089
 * @tc.name      : MakeReadOnly_Failed_Test
 * @tc.desc      : TagEndPoint MakeReadOnly NFA_RwSetTagReadOnly return NFA_STATUS_FAILED
 */
TEST_F(TagEndPointTest, MakeReadOnly_Failed_Test)
{
    nfcNciMock_->SetRwSetTagReadOnlyScene(2);
    EXPECT_FALSE(GetT2TTag()->MakeReadOnly());
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0090
 * @tc.name      : ReadNdef_T1T_Test
 * @tc.desc      : TagEndPoint ReadNdef T1T tag
 */
TEST_F(TagEndPointTest, ReadNdef_T1T_Test)
{
    EXPECT_NE(GetT1TTag()->ReadNdef(), "");
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0091
 * @tc.name      : ReadNdef_T2T_Test
 * @tc.desc      : TagEndPoint ReadNdef T2T tag
 */
TEST_F(TagEndPointTest, ReadNdef_T2T_Test)
{
    EXPECT_NE(GetT2TTag()->ReadNdef(), "");
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0092
 * @tc.name      : ReadNdef_MifareUltralight_Test
 * @tc.desc      : TagEndPoint ReadNdef MifareUltralight tag
 */
TEST_F(TagEndPointTest, ReadNdef_MifareUltralight_Test)
{
    EXPECT_NE(GetMifareUltralightTag()->ReadNdef(), "");
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0093
 * @tc.name      : ReadNdef_T3T_Test
 * @tc.desc      : TagEndPoint ReadNdef T3T tag
 */
TEST_F(TagEndPointTest, ReadNdef_T3T_Test)
{
    EXPECT_NE(GetT3TTag()->ReadNdef(), "");
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0094
 * @tc.name      : ReadNdef_T4T_Test
 * @tc.desc      : TagEndPoint ReadNdef T4T tag
 */
TEST_F(TagEndPointTest, ReadNdef_T4T_Test)
{
    EXPECT_NE(GetT4TTag()->ReadNdef(), "");
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0095
 * @tc.name      : ReadNdef_T5T_Test
 * @tc.desc      : TagEndPoint ReadNdef T5T tag
 */
TEST_F(TagEndPointTest, ReadNdef_T5T_Test)
{
    EXPECT_NE(GetT5TTag()->ReadNdef(), "");
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0096
 * @tc.name      : ReadNdef_MifareClassic_Test
 * @tc.desc      : TagEndPoint ReadNdef MifareClassic tag
 */
TEST_F(TagEndPointTest, ReadNdef_MifareClassic_Test)
{
    EXPECT_NE(GetMifareClassicTag()->ReadNdef(), "");
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0097
 * @tc.name      : ReadNdef_Nfc_Off_Test
 * @tc.desc      : TagEndPoint ReadNdef when nfc off
 */
TEST_F(TagEndPointTest, ReadNdef_Nfc_Off_Test)
{
    std::shared_ptr<TagEndPoint> tagEndPoint = GetT2TTag();
    EXPECT_TRUE(deviceHost_->Deinitialize());
    EXPECT_EQ(tagEndPoint->ReadNdef(), "");
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0098
 * @tc.name      : ReadNdef_Failed2_Test
 * @tc.desc      : TagEndPoint ReadNdef NFA_READ_CPLT_EVT event status NFA_STATUS_FAILED
 */
TEST_F(TagEndPointTest, ReadNdef_Failed_Test)
{
    std::shared_ptr<TagEndPoint> tagEndPoint = GetT2TTag();
    nfcNciMock_->SetRwReadNdefScene(1);
    EXPECT_EQ(tagEndPoint->ReadNdef(), "");
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0099
 * @tc.name      : WriteNdef_T1T_Test
 * @tc.desc      : TagEndPoint WriteNdef T1T tag
 */
TEST_F(TagEndPointTest, WriteNdef_T1T_Test)
{
    std::string data = "w123";
    EXPECT_TRUE(GetT1TTag()->WriteNdef(data));
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0100
 * @tc.name      : WriteNdef_T2T_Test
 * @tc.desc      : TagEndPoint WriteNdef T2T tag
 */
TEST_F(TagEndPointTest, WriteNdef_T2T_Test)
{
    std::string data = "w123";
    EXPECT_TRUE(GetT2TTag()->WriteNdef(data));
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0101
 * @tc.name      : TagEndPoint WriteNdef MifareUltralight tag test
 * @tc.desc      : [C- SOFTWARE -0200]
 */
TEST_F(TagEndPointTest, WriteNdef_MifareUltralight_Test)
{
    std::string data = "w123";
    EXPECT_TRUE(GetMifareUltralightTag()->WriteNdef(data));
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0102
 * @tc.name      : WriteNdef_T3T_Test
 * @tc.desc      : TagEndPoint WriteNdef T3T tag
 */
TEST_F(TagEndPointTest, WriteNdef_T3T_Test)
{
    std::string data = "w123";
    EXPECT_TRUE(GetT3TTag()->WriteNdef(data));
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0103
 * @tc.name      : WriteNdef_T4T_Test
 * @tc.desc      : TagEndPoint WriteNdef T4T tag
 */
TEST_F(TagEndPointTest, WriteNdef_T4T_Test)
{
    std::string data = "w123";
    EXPECT_TRUE(GetT4TTag()->WriteNdef(data));
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0104
 * @tc.name      : WriteNdef_T5T_Test
 * @tc.desc      : TagEndPoint WriteNdef T5T tag
 */
TEST_F(TagEndPointTest, WriteNdef_T5T_Test)
{
    std::string data = "w123";
    EXPECT_TRUE(GetT5TTag()->WriteNdef(data));
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0105
 * @tc.name      : WriteNdef_MifareClassic_Test
 * @tc.desc      : TagEndPoint WriteNdef MifareClassic tag
 */
TEST_F(TagEndPointTest, WriteNdef_MifareClassic_Test)
{
    std::string data = "w123";
    EXPECT_TRUE(GetMifareClassicTag()->WriteNdef(data));
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0106
 * @tc.name      : WriteEmptyNdef_T1T_Test
 * @tc.desc      : TagEndPoint WriteNdef with empty data T1T tag
 */
TEST_F(TagEndPointTest, WriteEmptyNdef_T1T_Test)
{
    std::string data = "";
    EXPECT_TRUE(GetT1TTag()->WriteNdef(data));
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0107
 * @tc.name      : WriteEmptyNdef_T2T_Test
 * @tc.desc      : TagEndPoint WriteNdef with empty data T2T tag
 */
TEST_F(TagEndPointTest, WriteEmptyNdef_T2T_Test)
{
    std::string data = "";
    EXPECT_TRUE(GetT2TTag()->WriteNdef(data));
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0108
 * @tc.name      : WriteEmptyNdef_MifareUltralight_Test
 * @tc.desc      : TagEndPoint WriteNdef with empty data MifareUltralight tag
 */
TEST_F(TagEndPointTest, WriteEmptyNdef_MifareUltralight_Test)
{
    std::string data = "";
    EXPECT_TRUE(GetMifareUltralightTag()->WriteNdef(data));
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0109
 * @tc.name      : WriteEmptyNdef_T3T_Test
 * @tc.desc      : TagEndPoint WriteNdef with empty data T3T tag
 */
TEST_F(TagEndPointTest, WriteEmptyNdef_T3T_Test)
{
    std::string data = "";
    EXPECT_TRUE(GetT3TTag()->WriteNdef(data));
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0110
 * @tc.name      : WriteEmptyNdef_T4T_Test
 * @tc.desc      : TagEndPoint WriteNdef with empty data T4T tag
 */
TEST_F(TagEndPointTest, WriteEmptyNdef_T4T_Test)
{
    std::string data = "";
    EXPECT_TRUE(GetT4TTag()->WriteNdef(data));
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0111
 * @tc.name      : WriteEmptyNdef_T5T_Test
 * @tc.desc      : TagEndPoint WriteNdef with empty data T5T tag
 */
TEST_F(TagEndPointTest, WriteEmptyNdef_T5T_Test)
{
    std::string data = "";
    EXPECT_TRUE(GetT5TTag()->WriteNdef(data));
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0112
 * @tc.name      : WriteEmptyNdef_MifareClassic_Test
 * @tc.desc      : TagEndPoint WriteNdef with empty data MifareClassic tag
 */
TEST_F(TagEndPointTest, WriteEmptyNdef_MifareClassic_Test)
{
    std::string data = "";
    EXPECT_TRUE(GetMifareClassicTag()->WriteNdef(data));
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0113
 * @tc.name      : WriteNdef_Nfc_Off_Test
 * @tc.desc      : TagEndPoint WriteNdef when nfc off
 */
TEST_F(TagEndPointTest, WriteNdef_Nfc_Off_Test)
{
    std::string data = "wf123";
    std::shared_ptr<TagEndPoint> tagEndPoint = GetT2TTag();
    EXPECT_TRUE(deviceHost_->Deinitialize());
    EXPECT_FALSE(tagEndPoint->WriteNdef(data));
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0114
 * @tc.name      : WriteNdef_Failed_Test
 * @tc.desc      : TagEndPoint WriteNdef NFA_RwWriteNDef return NFA_STATUS_FAILED
 */
TEST_F(TagEndPointTest, WriteNdef_Failed_Test)
{
    std::string data = "wf123";
    nfcNciMock_->SetRwWriteNdefScene(1);
    EXPECT_FALSE(GetT2TTag()->WriteNdef(data));
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0115
 * @tc.name      : WriteNdef_Event_Failed_Test
 * @tc.desc      : TagEndPoint WriteNdef NFA_WRITE_CPLT_EVT event status NFA_STATUS_FAILED
 */
TEST_F(TagEndPointTest, WriteNdef_Event_Failed_Test)
{
    std::string data = "wf123";
    nfcNciMock_->SetRwWriteNdefScene(2);
    EXPECT_FALSE(GetT2TTag()->WriteNdef(data));
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0116
 * @tc.name      : FormatNdef_T1T_Test
 * @tc.desc      : TagEndPoint FormatNdef T1T tag
 */
TEST_F(TagEndPointTest, FormatNdef_T1T_Test)
{
    std::string key = "key123";
    EXPECT_TRUE(GetT1TTag()->FormatNdef(key));
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0117
 * @tc.name      : FormatNdef_T2T_Test
 * @tc.desc      : TagEndPoint FormatNdef T2T tag
 */
TEST_F(TagEndPointTest, FormatNdef_T2T_Test)
{
    std::string key = "key123";
    EXPECT_TRUE(GetT2TTag()->FormatNdef(key));
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0118
 * @tc.name      : FormatNdef_MifareUltralight_Test
 * @tc.desc      : TagEndPoint FormatNdef MifareUltralight tag
 */
TEST_F(TagEndPointTest, FormatNdef_MifareUltralight_Test)
{
    std::string key = "key123";
    EXPECT_TRUE(GetMifareUltralightTag()->FormatNdef(key));
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0119
 * @tc.name      : FormatNdef_T3T_Test
 * @tc.desc      : TagEndPoint FormatNdef T3T tag
 */
TEST_F(TagEndPointTest, FormatNdef_T3T_Test)
{
    std::string key = "key123";
    EXPECT_TRUE(GetT3TTag()->FormatNdef(key));
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0120
 * @tc.name      : FormatNdef_T4T_Test
 * @tc.desc      : TagEndPoint FormatNdef T4T tag
 */
TEST_F(TagEndPointTest, FormatNdef_T4T_Test)
{
    std::string key = "key123";
    EXPECT_TRUE(GetT4TTag()->FormatNdef(key));
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0121
 * @tc.name      : FormatNdef_T5T_Test
 * @tc.desc      : TagEndPoint FormatNdef T5T tag
 */
TEST_F(TagEndPointTest, FormatNdef_T5T_Test)
{
    std::string key = "key123";
    EXPECT_TRUE(GetT5TTag()->FormatNdef(key));
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0122
 * @tc.name      : FormatNdef_MifareClassic_Test
 * @tc.desc      : TagEndPoint FormatNdef MifareClassic tag
 */
TEST_F(TagEndPointTest, FormatNdef_MifareClassic_Test)
{
    std::string key = "key123";
    EXPECT_TRUE(GetMifareClassicTag()->FormatNdef(key));
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0123
 * @tc.name      : FormatNdef_Empty_Key_Failed_Test
 * @tc.desc      : TagEndPoint FormatNdef with empty key
 */
TEST_F(TagEndPointTest, FormatNdef_Empty_Key_Failed_Test)
{
    std::string key = "";
    EXPECT_FALSE(GetT2TTag()->FormatNdef(key));
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0124
 * @tc.name      : FormatNdef_Nfc_Off_Test
 * @tc.desc      : TagEndPoint FormatNdef when nfc off
 */
TEST_F(TagEndPointTest, FormatNdef_Nfc_Off_Test)
{
    std::string key = "keyyyy";
    std::shared_ptr<TagEndPoint> tagEndPoint = GetT2TTag();
    EXPECT_TRUE(deviceHost_->Deinitialize());
    EXPECT_FALSE(tagEndPoint->FormatNdef(key));
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0125
 * @tc.name      : FormatNdef_Failed_Test
 * @tc.desc      : TagEndPoint FormatNdef NFA_RwFormatTag return NFA_STATUS_FAILED
 */
TEST_F(TagEndPointTest, FormatNdef_Failed_Test)
{
    std::string key = "keyyyy";
    nfcNciMock_->SetRwFormatTagScene(1);
    EXPECT_FALSE(GetT2TTag()->FormatNdef(key));
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0126
 * @tc.name      : FormatNdef_Event_Failed_Test
 * @tc.desc      : TagEndPoint FormatNdef NFA_FORMAT_CPLT_EVT event status NFA_STATUS_FAILED
 */
TEST_F(TagEndPointTest, FormatNdef_Event_Failed_Test)
{
    std::string key = "keyyyy";
    nfcNciMock_->SetRwFormatTagScene(2);
    EXPECT_FALSE(GetT2TTag()->FormatNdef(key));
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0127
 * @tc.name      : IsNdefFormatable_Test
 * @tc.desc      : TagEndPoint IsNdefFormatable
 */
TEST_F(TagEndPointTest, IsNdefFormatable_Test)
{
    EXPECT_FALSE(GetT2TTag()->IsNdefFormatable());
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0128
 * @tc.name      : IsNdefFormatable_After_Format_Test
 * @tc.desc      : TagEndPoint IsNdefFormatable after format
 */
TEST_F(TagEndPointTest, IsNdefFormatable_After_Format_Test)
{
    std::string key = "key123";
    std::shared_ptr<TagEndPoint> tagEndPoint = GetT2TTag();
    EXPECT_TRUE(tagEndPoint->FormatNdef(key));
    EXPECT_TRUE(tagEndPoint->IsNdefFormatable());
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0129
 * @tc.name      : CheckNdef_T1T_Test
 * @tc.desc      : TagEndPoint CheckNdef T1T tag
 */
TEST_F(TagEndPointTest, CheckNdef_T1T_Test)
{
    std::vector<int> ndefInfo;
    EXPECT_TRUE(GetT1TTag()->CheckNdef(ndefInfo));
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0130
 * @tc.name      : CheckNdef_T2T_Test
 * @tc.desc      : TagEndPoint CheckNdef T2T tag
 */
TEST_F(TagEndPointTest, CheckNdef_T2T_Test)
{
    std::vector<int> ndefInfo;
    EXPECT_TRUE(GetT2TTag()->CheckNdef(ndefInfo));
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0131
 * @tc.name      : CheckNdef_MifareUltralight_Test
 * @tc.desc      : TagEndPoint CheckNdef MifareUltralight tag
 */
TEST_F(TagEndPointTest, CheckNdef_MifareUltralight_Test)
{
    std::vector<int> ndefInfo;
    EXPECT_TRUE(GetMifareUltralightTag()->CheckNdef(ndefInfo));
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0132
 * @tc.name      : CheckNdef_T3T_Test
 * @tc.desc      : TagEndPoint CheckNdef T3T tag
 */
TEST_F(TagEndPointTest, CheckNdef_T3T_Test)
{
    std::vector<int> ndefInfo;
    EXPECT_TRUE(GetT3TTag()->CheckNdef(ndefInfo));
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0133
 * @tc.name      : CheckNdef_T4T_Test
 * @tc.desc      : TagEndPoint CheckNdef T4T tag
 */
TEST_F(TagEndPointTest, CheckNdef_T4T_Test)
{
    std::vector<int> ndefInfo;
    EXPECT_TRUE(GetT4TTag()->CheckNdef(ndefInfo));
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0134
 * @tc.name      : CheckNdef_T5T_Test
 * @tc.desc      : TagEndPoint CheckNdef T5T tag
 */
TEST_F(TagEndPointTest, CheckNdef_T5T_Test)
{
    std::vector<int> ndefInfo;
    EXPECT_TRUE(GetT5TTag()->CheckNdef(ndefInfo));
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0135
 * @tc.name      : CheckNdef_MifareClassic_Test
 * @tc.desc      : TagEndPoint CheckNdef MifareClassic tag
 */
TEST_F(TagEndPointTest, CheckNdef_MifareClassic_Test)
{
    std::vector<int> ndefInfo;
    EXPECT_TRUE(GetMifareClassicTag()->CheckNdef(ndefInfo));
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0136
 * @tc.name      : CheckNdef_Flags_Test
 * @tc.desc      : TagEndPoint CheckNdef with diff flags
 */
TEST_F(TagEndPointTest, CheckNdef_Flags_Test)
{
    std::vector<int> ndefInfo;
    std::shared_ptr<TagEndPoint> tagEndPoint = GetT2TTag();
    nfcNciMock_->SetRwDetectNdefScene(1);
    EXPECT_TRUE(tagEndPoint->CheckNdef(ndefInfo));
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0137
 * @tc.name      : CheckNdef_Status_Failed_Test
 * @tc.desc      : TagEndPoint CheckNdef with NFA_STATUS_FAILED status
 */
TEST_F(TagEndPointTest, CheckNdef_Status_Failed_Test)
{
    std::vector<int> ndefInfo;
    std::shared_ptr<TagEndPoint> tagEndPoint = GetT2TTag();
    nfcNciMock_->SetRwDetectNdefScene(2);
    EXPECT_TRUE(tagEndPoint->CheckNdef(ndefInfo));
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0138
 * @tc.name      : CheckNdef_Status_Timeout_Test
 * @tc.desc      : TagEndPoint CheckNdef with NFA_STATUS_TIMEOUT status
 */
TEST_F(TagEndPointTest, CheckNdef_Status_Timeout_Test)
{
    std::vector<int> ndefInfo;
    std::shared_ptr<TagEndPoint> tagEndPoint = GetT2TTag();
    nfcNciMock_->SetRwDetectNdefScene(3);
    EXPECT_FALSE(tagEndPoint->CheckNdef(ndefInfo));
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0139
 * @tc.name      : CheckNdef_Nfc_Off_Test
 * @tc.desc      : TagEndPoint CheckNdef when nfc off
 */
TEST_F(TagEndPointTest, CheckNdef_Nfc_Off_Test)
{
    std::vector<int> ndefInfo;
    std::shared_ptr<TagEndPoint> tagEndPoint = GetT2TTag();
    EXPECT_TRUE(deviceHost_->Deinitialize());
    EXPECT_FALSE(tagEndPoint->CheckNdef(ndefInfo));
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0140
 * @tc.name      : CheckNdef_Failed_Test
 * @tc.desc      : TagEndPoint CheckNdef NFA_RwDetectNDef return NFA_STATUS_FAILED
 */
TEST_F(TagEndPointTest, CheckNdef_Failed_Test)
{
    std::vector<int> ndefInfo;
    std::shared_ptr<TagEndPoint> tagEndPoint = GetT2TTag();
    nfcNciMock_->SetRwDetectNdefScene(4);
    EXPECT_FALSE(tagEndPoint->CheckNdef(ndefInfo));
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0141
 * @tc.name      : CheckNdef_Timeout_Test
 * @tc.desc      : TagEndPoint CheckNdef NFA_RwDetectNDef return NFA_STATUS_OK but no NFA_NDEF_DETECT_EVT
 */
TEST_F(TagEndPointTest, CheckNdef_Timeout_Test)
{
    std::vector<int> ndefInfo;
    std::shared_ptr<TagEndPoint> tagEndPoint = GetT2TTag();
    nfcNciMock_->SetRwDetectNdefScene(5);
    EXPECT_FALSE(tagEndPoint->CheckNdef(ndefInfo));
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0142
 * @tc.name      : GetConnectedTechnology_Test
 * @tc.desc      : TagEndPoint GetConnectedTechnolog
 */
TEST_F(TagEndPointTest, GetConnectedTechnology_Test)
{
    EXPECT_EQ(GetT2TTag()->GetConnectedTechnology(), 0);
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0143
 * @tc.name      : Ndef_Formattable_T1T_Test
 * @tc.desc      : TagEndPoint discover T1T tag not supported ndef 
 */
TEST_F(TagEndPointTest, Ndef_Formattable_T1T_Test)
{
    std::vector<int> ndefInfo;
    nfcNciMock_->SetRwDetectNdefScene(4);
    std::shared_ptr<TagEndPoint> tagEndPoint = GetT1TTag();
    EXPECT_NE(0, (int)tagEndPoint->GetTechList().size());
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0144
 * @tc.name      : Ndef_Formattable_T2T_Test
 * @tc.desc      : TagEndPoint discover T2T tag not supported ndef 
 */
TEST_F(TagEndPointTest, Ndef_Formattable_T2T_Test)
{
    std::vector<int> ndefInfo;
    nfcNciMock_->SetRwDetectNdefScene(4);
    std::shared_ptr<TagEndPoint> tagEndPoint = GetT2TTag();
    EXPECT_NE(0, (int)tagEndPoint->GetTechList().size());
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0145
 * @tc.name      : Ndef_Formattable_MifareUltralight_Test
 * @tc.desc      : TagEndPoint discover MifareUltralight tag not supported ndef 
 */
TEST_F(TagEndPointTest, Ndef_Formattable_MifareUltralight_Test)
{
    std::vector<int> ndefInfo;
    nfcNciMock_->SetRwDetectNdefScene(4);
    std::shared_ptr<TagEndPoint> tagEndPoint = GetMifareUltralightTag();
    EXPECT_NE(0, (int)tagEndPoint->GetTechList().size());
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0146
 * @tc.name      : Ndef_Formattable_T3T_Test
 * @tc.desc      : TagEndPoint discover T3T tag not supported ndef 
 */
TEST_F(TagEndPointTest, Ndef_Formattable_T3T_Test)
{
    std::vector<int> ndefInfo;
    nfcNciMock_->SetRwDetectNdefScene(4);
    std::shared_ptr<TagEndPoint> tagEndPoint = GetT3TTag();
    EXPECT_NE(0, (int)tagEndPoint->GetTechList().size());
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0147
 * @tc.name      : Ndef_Formattable_T4T_Test
 * @tc.desc      : TagEndPoint discover T4T tag not supported ndef 
 */
TEST_F(TagEndPointTest, Ndef_Formattable_T4T_Test)
{
    std::vector<int> ndefInfo;
    nfcNciMock_->SetRwDetectNdefScene(4);
    std::shared_ptr<TagEndPoint> tagEndPoint = GetT4TTag();
    EXPECT_NE(0, (int)tagEndPoint->GetTechList().size());
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0148
 * @tc.name      : Ndef_Formattable_T5T_Test
 * @tc.desc      : TagEndPoint discover T5T tag not supported ndef 
 */
TEST_F(TagEndPointTest, Ndef_Formattable_T5T_Test)
{
    std::vector<int> ndefInfo;
    nfcNciMock_->SetRwDetectNdefScene(4);
    std::shared_ptr<TagEndPoint> tagEndPoint = GetT5TTag();
    EXPECT_NE(0, (int)tagEndPoint->GetTechList().size());
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0149
 * @tc.name      : Ndef_Formattable_MifareClassic_Test
 * @tc.desc      : TagEndPoint discover MifareClassic tag not supported ndef 
 */
TEST_F(TagEndPointTest, Ndef_Formattable_MifareClassic_Test)
{
    std::vector<int> ndefInfo;
    nfcNciMock_->SetRwDetectNdefScene(4);
    std::shared_ptr<TagEndPoint> tagEndPoint = GetMifareClassicTag();
    EXPECT_NE(0, (int)tagEndPoint->GetTechList().size());
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0150
 * @tc.name      : DiscResult_IsoDep_Test
 * @tc.desc      : TagEndPoint NFA_DISC_RESULT_EVT with NFA_PROTOCOL_ISO_DEP
 */
TEST_F(TagEndPointTest, DiscResult_IsoDep_Test)
{
    tNFA_CONN_EVT_DATA connEventData;
    connEventData.disc_result.status = NFA_STATUS_OK;
    connEventData.disc_result.discovery_ntf.rf_disc_id = 2;
    connEventData.disc_result.discovery_ntf.protocol = NFA_PROTOCOL_ISO_DEP;
    NfcNciMock::NfcConnectionCallback(NFA_DISC_RESULT_EVT, &connEventData);
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0151
 * @tc.name      : DiscResult_Mifare_Test
 * @tc.desc      : TagEndPoint NFA_DISC_RESULT_EVT with NFC_PROTOCOL_MIFARE
 */
TEST_F(TagEndPointTest, DiscResult_Mifare_Test)
{
    tNFA_CONN_EVT_DATA connEventData;
    connEventData.disc_result.status = NFA_STATUS_OK;
    connEventData.disc_result.discovery_ntf.rf_disc_id = 2;
    connEventData.disc_result.discovery_ntf.protocol = NFC_PROTOCOL_MIFARE;
    NfcNciMock::NfcConnectionCallback(NFA_DISC_RESULT_EVT, &connEventData);
}

/**
 * @tc.number    : NFC_TAG_END_POINT_API_0152
 * @tc.name      : DiscResult_T2T_Test3
 * @tc.desc      : TagEndPoint NFA_DISC_RESULT_EVT with NFC_PROTOCOL_T2T
 */
TEST_F(TagEndPointTest, DiscResult_T2T_Test3)
{
    tNFA_CONN_EVT_DATA connEventData;
    connEventData.disc_result.status = NFA_STATUS_OK;
    connEventData.disc_result.discovery_ntf.rf_disc_id = 2;
    connEventData.disc_result.discovery_ntf.protocol = NFC_PROTOCOL_T2T;
    NfcNciMock::NfcConnectionCallback(NFA_DISC_RESULT_EVT, &connEventData);
}