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

#include "nfc_sa_unit_test.h"
#include <iostream>
#include <gtest/gtest.h>
#include <cstring>
#include "ipc_skeleton.h"
#include "nfc_adapter_utils.h"
#include "string_ex.h"

using namespace std;
using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Nfc {
namespace TestSaName {
const int SLEEP_SEC = 3;

/* NFC state */
const int NFC_STATE_OFF = 1;
const int NFC_STATE_ON = 3;

/* NFC tech */
const int TECH_NFCA = 1;
const int TECH_ISODEP = 3;
const int TECH_NDEF = 6;

/* const for NFC function */
const int SA_FUNC_SUCC = 0;
const int NO_ERROR = 0;
const int SYS_UID = 1000;
const std::string PKG_NXP = "com.nxp.nfc.INxpNfcAdapter";

const std::string STATE_CHANGED = "usual.event.nfc.action.ADAPTER_STATE_CHANGED";
const std::string FIELD_ON = "usual.event.nfc.action.RF_FIELD_ON_DETECTED";
const std::string FIELD_OFF = "usual.event.nfc.action.RF_FIELD_OFF_DETECTED";

const std::string EXTRA_NFC_STATE = "extra_nfc_state";

const std::u16string NFC_CONTROLLER_TOKEN = u"ohos.nfc.INfcController";
const std::u16string SECURE_ELEMENT_TOKEN = u"ohos.nfc.oma.ISecureElement";
const std::u16string NFC_TAG_TOKEN = u"ohos.nfc.ITagInfo";
}

class NfcSaUnitTestSubscriber final : public Notification::CommonEventSubscriber {
public:
    std::string expectAction_;
    std::string expectParam_;
    int32_t& result_;
public:
    explicit NfcSaUnitTestSubscriber(const sptr<Notification::CommonEventSubscribeInfo>& info,
        const std::string& expectAction,
        const std::string& expectParam,
        int32_t& result)
        : CommonEventSubscriber(info),
        expectAction_(expectAction),
        expectParam_(expectParam),
        result_(result) {}
    ~NfcSaUnitTestSubscriber() override = default;

    void OnReceive(const sptr<Notification::CommonEventData>& event) override
    {
        GTEST_LOG_(INFO) << "NfcSaUnitTestSubscriber OnReceive";
        if (!event) {
            GTEST_LOG_(INFO) << "NfcSaUnitTestSubscriber receive invalid event!";
            return;
        }
        const auto intent = event->GetIntent();
        if (!intent) {
            GTEST_LOG_(INFO) << "NfcSaUnitTestSubscriber receive invalid intent!";
            return;
        }
        std::string action = intent->GetAction();
        GTEST_LOG_(INFO) << "NfcSaUnitTestSubscriber receive receive action: " << action.c_str();

        if (expectAction_.compare(action) == 0) {
            result_ = intent->GetIntParam(expectParam_, -1);
            GTEST_LOG_(INFO) << "NfcSaUnitTestSubscriber notification result: " << result_;
        }
    }
};

int32_t NfcSaUnitTest::SetCommonEventSubscriber(int32_t& state)
{
    GTEST_LOG_(INFO) << "NfcSaUnitTest SetCommonEventSubscriber";
    sptr<AAFwk::Skills> skill = new AAFwk::Skills();
    skill->AddAction(TestSaName::STATE_CHANGED);
    skill->AddAction(TestSaName::FIELD_ON);
    skill->AddAction(TestSaName::FIELD_OFF);
    sptr<Notification::CommonEventSubscribeInfo> info = new Notification::CommonEventSubscribeInfo();
    info->SetSkills(skill);
    subscriber_ = std::make_shared<NfcSaUnitTestSubscriber>(info,
        TestSaName::STATE_CHANGED, TestSaName::EXTRA_NFC_STATE, state);
    const auto result = Notification::CommonEventManager::GetInstance().SubscribeCommonEvent(subscriber_);
    return result;
}

void WriteInterfaceToken(const std::string& nfcInterface, MessageParcel& data)
{
    data.WriteInt32(1); // set 1: strictPolicy
    data.WriteInt32(1); // set 1: workSource
    data.WriteString16(Str8ToStr16(nfcInterface)); // Interface
}

/* write byte array to MessageParcel */
void WriteByteArray(MessageParcel &data, uint8_t val[], int32_t len)
{
    data.WriteInt32(static_cast<uint32_t>(len));
    data.WriteBuffer(val, len * sizeof(*val));
}

void NfcSaUnitTest::SetUp(void)
{
    GTEST_LOG_(INFO) << "SetUp";
    /**
     * @tc.setup: create the instance of NFC
     */
    nfcSaSvr_ = NfcManagerService::GetInstance();
}

void NfcSaUnitTest::TearDown(void)
{
    GTEST_LOG_(INFO) << "TearDown";
    /**
     * @tc.teardown: destroy the instance of NFC
     */
    nfcSaSvr_ = nullptr;
}

/**
 * @tc.name: SetNfcEnabled001
 * @tc.desc: Verify if set NFC Enabled successfully.
 * @tc.type: FUNC
 * @tc.require:AR000CSJ1A
 * @tc.author: zhangxiuping
 */
HWTEST_F(NfcSaUnitTest, SetNfcEnabled001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "NfcSaUnitTest setNfcEnabled001 enabled";
    /**
     * @tc.steps: step1. close NFC, set NFC enabled
     * @tc.expected: step1. return true, NFC is Enabled.
     */
    MessageParcel data;
    MessageParcel reply;
    data.WriteString16(TestSaName::NFC_CONTROLLER_TOKEN);
    data.WriteInt32(1);
    int32_t ret = nfcSaSvr_->SetNfcEnabled(data, reply);
    EXPECT_EQ(1, ret); // 1: return success
    sleep(TestSaName::SLEEP_SEC); // time for waiting process
    MessageParcel data1;
    MessageParcel reply1;
    data1.WriteString16(TestSaName::NFC_CONTROLLER_TOKEN);
    EXPECT_EQ(TestSaName::NFC_STATE_ON, nfcSaSvr_->GetNfcState(data1, reply1));
}

/**
 * @tc.name: GetNfcState001
 * @tc.desc: Verify if get NfcState correctly.
 * @tc.type: FUNC
 * @tc.require:AR000CSJ3D
 * @tc.author: zhangxiuping
 */
HWTEST_F(NfcSaUnitTest, GetNfcState001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "NfcSaUnitTest NfcStateQuery_002 enabled";
    /**
     * @tc.steps: step1. open NFC(opened by last test), get the NFC state
     * @tc.expected: step1. return 3 (TestSaName::NFC_STATE_ON)
     */
    MessageParcel data;
    MessageParcel reply;
    data.WriteString16(TestSaName::NFC_CONTROLLER_TOKEN);
    EXPECT_EQ(TestSaName::NFC_STATE_ON, nfcSaSvr_->GetNfcState(data, reply)); // TestSaName::NFC_STATE_ON is 3
}

/**
 * @tc.name: SetNfcEnabled002
 * @tc.desc: Verify if set NFC Enabled successfully.
 * @tc.type: FUNC
 * @tc.require:AR000CSJ1A
 * @tc.author: zhangxiuping
 */
HWTEST_F(NfcSaUnitTest, SetNfcEnabled002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "NfcSaUnitTest setNfcEnabled002 disabled";
    /**
     * @tc.steps: step1. open NFC, set NFC disabled
     * @tc.expected: step1. return true, NFC is Disabled.
     */
    MessageParcel data;
    MessageParcel reply;
    data.WriteString16(TestSaName::NFC_CONTROLLER_TOKEN);
    data.WriteInt32(0);
    bool ret = nfcSaSvr_->SetNfcEnabled(data, reply);  // disabled NFC
    EXPECT_EQ(true, ret);
    sleep(TestSaName::SLEEP_SEC); // time for waiting process
    MessageParcel data1;
    MessageParcel reply1;
    data1.WriteString16(TestSaName::NFC_CONTROLLER_TOKEN);
    EXPECT_EQ(TestSaName::NFC_STATE_OFF, nfcSaSvr_->GetNfcState(data1, reply1));
}

/**
 * @tc.name: IsNfcAvailable001
 * @tc.desc: Verify if NFC Available.
 * @tc.type: FUNC
 * @tc.require:AR000CSJ3D
 * @tc.author: zhangxiuping
 */
HWTEST_F(NfcSaUnitTest, IsNfcAvailable001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "NfcSaUnitTest IsNfcAvailable001";
    /**
     * @tc.steps: step1. close NFC (closed by last test), query if NFC has capability
     * @tc.expected: step1. return true
     */
    MessageParcel data;
    MessageParcel reply;
    data.WriteString16(TestSaName::NFC_CONTROLLER_TOKEN);
    EXPECT_TRUE(nfcSaSvr_->IsNfcAvailable(data, reply));
}

/**
 * @tc.name: ConnectTag001
 * @tc.desc: Verify funcs about connect nfcA tag and query connect state.
 * @tc.type: FUNC
 * @tc.require:AR000CSJ7F
 * @tc.author: maolongxia
 */
HWTEST_F(NfcSaUnitTest, ConnectTag001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "NfcSaUnitTest ConnectTag001 NFC_A";
    /**
     * @tc.steps: step1. open NFC, query if NFC is open
     * @tc.expected: step1. return true
     */
    MessageParcel data;
    MessageParcel reply;
    data.WriteString16(TestSaName::NFC_CONTROLLER_TOKEN);
    data.WriteInt32(1);
    bool ret = nfcSaSvr_->SetNfcEnabled(data, reply);
    EXPECT_EQ(true, ret);
    sleep(TestSaName::SLEEP_SEC); // time for waiting process
    MessageParcel data1;
    MessageParcel reply1;
    data1.WriteString16(TestSaName::NFC_CONTROLLER_TOKEN);
    EXPECT_EQ(TestSaName::NFC_STATE_ON, nfcSaSvr_->GetNfcState(data1, reply1));
    /**
     * @tc.steps: step2. connect nfcA tag, query if tag is connected
     * @tc.expected: step2. return connect unsuccess, tag is unconnect.
     */
    MessageParcel data2;
    MessageParcel reply2;
    data2.WriteString16(TestSaName::NFC_TAG_TOKEN);
    EXPECT_FALSE(nfcSaSvr_->ConnectTag(data2, reply2));
    MessageParcel data3;
    MessageParcel reply3;
    data3.WriteString16(TestSaName::NFC_TAG_TOKEN);
    EXPECT_FALSE(nfcSaSvr_->IsTagConnected(data3, reply3));
    /**
     * @tc.steps: step3. reconnect nfcA tag, query if tag is connected
     * @tc.expected: step3. return reconnect unsuccess, tag is unconnect.
     */
    MessageParcel data4;
    MessageParcel reply4;
    data4.WriteString16(TestSaName::NFC_TAG_TOKEN);
    EXPECT_FALSE(nfcSaSvr_->ReconnectTag(data4, reply4));
    MessageParcel data5;
    MessageParcel reply5;
    data5.WriteString16(TestSaName::NFC_TAG_TOKEN);
    EXPECT_FALSE(nfcSaSvr_->IsTagConnected(data5, reply5));
}

/**
 * @tc.name: ConnectTag002
 * @tc.desc: Verify funcs about connect IsoDep tag and query connect state.
 * @tc.type: FUNC
 * @tc.require:AR000CSJ6B
 * @tc.author: maolongxia
 */
HWTEST_F(NfcSaUnitTest, ConnectTag002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "NfcSaUnitTest ConnectTag001 ISO_DEP";
    /**
     * @tc.steps: step1. open NFC, query if NFC is open
     * @tc.expected: step1. return true
     */
    MessageParcel data;
    MessageParcel reply;
    data.WriteString16(TestSaName::NFC_CONTROLLER_TOKEN);
    data.WriteInt32(1);
    bool ret = nfcSaSvr_->SetNfcEnabled(data, reply);
    EXPECT_EQ(true, ret);
    sleep(TestSaName::SLEEP_SEC); // time for waiting process
    MessageParcel data1;
    MessageParcel reply1;
    data1.WriteString16(TestSaName::NFC_CONTROLLER_TOKEN);
    EXPECT_EQ(TestSaName::NFC_STATE_ON, nfcSaSvr_->GetNfcState(data1, reply1));
    /**
     * @tc.steps: step2. connect IsoDep tag, query if tag is connected
     * @tc.expected: step2. return connect unsuccess, tag is unconnect.
     */
    MessageParcel data2;
    MessageParcel reply2;
    data2.WriteString16(TestSaName::NFC_TAG_TOKEN);
    EXPECT_FALSE(nfcSaSvr_->ConnectTag(data2, reply2));
    MessageParcel data3;
    MessageParcel reply3;
    data3.WriteString16(TestSaName::NFC_TAG_TOKEN);
    EXPECT_FALSE(nfcSaSvr_->IsTagConnected(data3, reply3));
    /**
     * @tc.steps: step3. reconnect IsoDep tag, query if tag is connected
     * @tc.expected: step3. return reconnect unsuccess, tag is unconnect.
     */
    MessageParcel data4;
    MessageParcel reply4;
    data4.WriteString16(TestSaName::NFC_TAG_TOKEN);
    EXPECT_FALSE(nfcSaSvr_->ReconnectTag(data4, reply4));
    MessageParcel data5;
    MessageParcel reply5;
    data5.WriteString16(TestSaName::NFC_TAG_TOKEN);
    EXPECT_FALSE(nfcSaSvr_->IsTagConnected(data5, reply5));
}

/**
 * @tc.name: ConnectTag003
 * @tc.desc: Verify funcs about connect Ndef tag and query connect state.
 * @tc.type: FUNC
 * @tc.require: AR000DOUON
 * @tc.author: liujiawei
 */
HWTEST_F(NfcSaUnitTest, ConnectTag003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "NfcSaUnitTest ConnectTag001 NDEF";
    /**
     * @tc.steps: step1. open NFC, query if NFC is open
     * @tc.expected: step1. return true
     */
    MessageParcel data;
    MessageParcel reply;
    data.WriteString16(TestSaName::NFC_CONTROLLER_TOKEN);
    data.WriteInt32(1);
    bool ret = nfcSaSvr_->SetNfcEnabled(data, reply);
    EXPECT_EQ(true, ret);
    sleep(TestSaName::SLEEP_SEC); // time for waiting process
    MessageParcel data1;
    MessageParcel reply1;
    data1.WriteString16(TestSaName::NFC_CONTROLLER_TOKEN);
    EXPECT_EQ(TestSaName::NFC_STATE_ON, nfcSaSvr_->GetNfcState(data1, reply1));
    /**
     * @tc.steps: step2. connect Ndef tag, query if tag is connected
     * @tc.expected: step2. return connect unsuccess, tag is unconnect.
     */
    MessageParcel data2;
    MessageParcel reply2;
    data2.WriteString16(TestSaName::NFC_TAG_TOKEN);
    data2.WriteInt32(TestSaName::TECH_NDEF);
    nfcSaSvr_->ConnectTag(data2, reply2);
    MessageParcel data3;
    MessageParcel reply3;
    data3.WriteString16(TestSaName::NFC_TAG_TOKEN);
    nfcSaSvr_->IsTagConnected(data3, reply3);

    MessageParcel data4;
    MessageParcel reply4;
    nfcSaSvr_->IsNdefTag(data4, reply4);
    MessageParcel reply5;
    nfcSaSvr_->NdefRead(data4, reply5);
    MessageParcel reply6;
    nfcSaSvr_->NdefWrite(data4, reply6);
    MessageParcel reply7;
    nfcSaSvr_->NdefSetReadOnly(data4, reply7);
    MessageParcel reply8;
    nfcSaSvr_->CanSetReadOnly(data4, reply8);
}

/**
 * @tc.name: SendDataTimeout001
 * @tc.desc: Verify funcs about setting sendDataTimeout of nfcA tag
 * @tc.type: FUNC
 * @tc.require:AR000CSJ7F
 * @tc.author: maolongxia
 */
HWTEST_F(NfcSaUnitTest, SendDataTimeout001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "NfcSaUnitTest SendDataTimeout001 NFC_A";
    /**
     * @tc.steps: step1. open NFC, query if NFC is open
     * @tc.expected: step1. return true
     */
    MessageParcel data;
    MessageParcel reply;
    data.WriteString16(TestSaName::NFC_CONTROLLER_TOKEN);
    data.WriteInt32(1);
    bool ret = nfcSaSvr_->SetNfcEnabled(data, reply);
    EXPECT_EQ(true, ret);
    sleep(TestSaName::SLEEP_SEC); // time for waiting process
    MessageParcel data1;
    MessageParcel reply1;
    data1.WriteString16(TestSaName::NFC_CONTROLLER_TOKEN);
    EXPECT_EQ(TestSaName::NFC_STATE_ON, nfcSaSvr_->GetNfcState(data1, reply1));
    /**
     * @tc.steps: step2. get the default sendDataTimeout of nfcA tag, verify default sendDataTimeout > 0
     * @tc.expected: step2. return true.
     */
    MessageParcel data2;
    MessageParcel reply2;
    data2.WriteString16(TestSaName::NFC_TAG_TOKEN);
    data2.WriteInt32(TestSaName::TECH_NFCA);
    int defaultTimeout = nfcSaSvr_->GetSendDataTimeout(data2, reply2);
    EXPECT_LT(0, defaultTimeout);
    /**
     * @tc.steps: step3. set sendDataTimeout of nfcA tag to a newTimeout, verify if timeout is equal to newTimeout
     * @tc.expected: step3. return true.
     */
    int newTimeout = 5000;
    MessageParcel data3;
    MessageParcel reply3;
    data3.WriteString16(TestSaName::NFC_TAG_TOKEN);
    data3.WriteInt32(TestSaName::TECH_NFCA);
    data3.WriteInt32(newTimeout);
    EXPECT_TRUE(nfcSaSvr_->SetSendDataTimeout(data3, reply3));
    MessageParcel data4;
    MessageParcel reply4;
    data4.WriteString16(TestSaName::NFC_TAG_TOKEN);
    data4.WriteInt32(TestSaName::TECH_NFCA);
    EXPECT_EQ(newTimeout, nfcSaSvr_->GetSendDataTimeout(data4, reply4));
    /**
     * @tc.steps: step4. reset sendDataTimeouts, verify if timeout is equal to default timeout
     * @tc.expected: step4. return true.
     */
    MessageParcel data5;
    MessageParcel reply5;
    data5.WriteString16(TestSaName::NFC_TAG_TOKEN);
    nfcSaSvr_->ResetTimeouts(data5, reply5);
    MessageParcel data6;
    MessageParcel reply6;
    data6.WriteString16(TestSaName::NFC_TAG_TOKEN);
    data6.WriteInt32(TestSaName::TECH_NFCA);
    EXPECT_EQ(defaultTimeout, nfcSaSvr_->GetSendDataTimeout(data6, reply6));
}

/**
 * @tc.name: SendDataTimeout002
 * @tc.desc: Verify funcs about setting sendDataTimeout of IsoDep tag
 * @tc.type: FUNC
 * @tc.require:AR000CSJ6B
 * @tc.author: maolongxia
 */
HWTEST_F(NfcSaUnitTest, SendDataTimeout002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "NfcSaUnitTest SendDataTimeout001 ISO_DEP";
    /**
     * @tc.steps: step1. open NFC, query if NFC is open
     * @tc.expected: step1. return true
     */
    MessageParcel data;
    MessageParcel reply;
    data.WriteString16(TestSaName::NFC_CONTROLLER_TOKEN);
    data.WriteInt32(1);
    bool ret = nfcSaSvr_->SetNfcEnabled(data, reply);
    EXPECT_EQ(true, ret);
    sleep(TestSaName::SLEEP_SEC); // time for waiting process
    MessageParcel data1;
    MessageParcel reply1;
    data1.WriteString16(TestSaName::NFC_CONTROLLER_TOKEN);
    EXPECT_EQ(TestSaName::NFC_STATE_ON, nfcSaSvr_->GetNfcState(data1, reply1));
    /**
     * @tc.steps: step2. get the default sendDataTimeout of IsoDep tag, verify default sendDataTimeout > 0
     * @tc.expected: step2. return true.
     */
    MessageParcel data2;
    MessageParcel reply2;
    data2.WriteString16(TestSaName::NFC_TAG_TOKEN);
    data2.WriteInt32(TestSaName::TECH_ISODEP);
    int defaultTimeout = nfcSaSvr_->GetSendDataTimeout(data2, reply2);
    EXPECT_LT(0, defaultTimeout);
    /**
     * @tc.steps: step3. set sendDataTimeout of nfcA tag to a newTimeout, verify if timeout is equal to newTimeout
     * @tc.expected: step3. return true.
     */
    MessageParcel data3;
    MessageParcel reply3;
    data3.WriteString16(TestSaName::NFC_TAG_TOKEN);
    data3.WriteInt32(TestSaName::TECH_ISODEP);
    int newTimeout = 5000;
    data3.WriteInt32(newTimeout);
    EXPECT_TRUE(nfcSaSvr_->SetSendDataTimeout(data3, reply3));
    MessageParcel data4;
    MessageParcel reply4;
    data4.WriteString16(TestSaName::NFC_TAG_TOKEN);
    data4.WriteInt32(TestSaName::TECH_ISODEP);
    EXPECT_EQ(newTimeout, nfcSaSvr_->GetSendDataTimeout(data4, reply4));
    /**
     * @tc.steps: step4. reset sendDataTimeouts, verify if timeout is equal to default timeout
     * @tc.expected: step4. return true.
     */
    MessageParcel data5;
    MessageParcel reply5;
    data5.WriteString16(TestSaName::NFC_TAG_TOKEN);
    nfcSaSvr_->ResetTimeouts(data5, reply5);
    MessageParcel data6;
    MessageParcel reply6;
    data6.WriteString16(TestSaName::NFC_TAG_TOKEN);
    data6.WriteInt32(TestSaName::TECH_ISODEP);
    EXPECT_EQ(defaultTimeout, nfcSaSvr_->GetSendDataTimeout(data6, reply6));
}

/**
 * @tc.name: GetMaxSendLength001
 * @tc.desc: Verify if get maxSendLength of nfcA tag successfully.
 * @tc.type: FUNC
 * @tc.require:AR000CSJ7F
 * @tc.author: maolongxia
 */
HWTEST_F(NfcSaUnitTest, GetMaxSendLength001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "NfcSaUnitTest GetMaxSendLength001";
    /**
     * @tc.steps: step1. get maxSendLength of nfcA tag, verify if it's greater than 0
     * @tc.expected: step1. return true
     */
    MessageParcel data;
    MessageParcel reply;
    data.WriteString16(TestSaName::NFC_TAG_TOKEN);
    data.WriteInt32(TestSaName::TECH_NFCA);
    int length = nfcSaSvr_->GetMaxSendLength(data, reply);
    GTEST_LOG_(INFO) << "max send length of nfca is " << length;
    EXPECT_LT(0, length);
}

/**
 * @tc.name: GetMaxSendLength002
 * @tc.desc: Verify if get maxSendLength of IsoDep tag successfully.
 * @tc.type: FUNC
 * @tc.require:AR000CSJ6B
 * @tc.author: maolongxia
 */
HWTEST_F(NfcSaUnitTest, GetMaxSendLength002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "NfcSaUnitTest GetMaxSendLength001";
    /**
     * @tc.steps: step1. get maxSendLength of IsoDep tag, verify if it's greater than 0
     * @tc.expected: step1. return true
     */
    MessageParcel data;
    MessageParcel reply;
    data.WriteString16(TestSaName::NFC_TAG_TOKEN);
    data.WriteInt32(TestSaName::TECH_ISODEP);
    int length = nfcSaSvr_->GetMaxSendLength(data, reply);
    GTEST_LOG_(INFO) << "max send length of isodep is " << length;
    EXPECT_LT(0, length);
}

/**
 * @tc.name: BindSeService001
 * @tc.desc: Verify if can bind SE Service
 * @tc.type: FUNC
 * @tc.require:AR000CSJO9
 * @tc.author: liuzhaoshen
 */
HWTEST_F(NfcSaUnitTest, BindSeService001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "NfcSaUnitTest BindSeService001";
    /**
     * @tc.steps: step1. get SE service
     * @tc.expected: step1. the string array of each secure element name
     */
    MessageParcel data;
    MessageParcel reply;
    WriteInterfaceToken(TestSaName::PKG_NXP, data);
    int32_t rps = nfcSaSvr_->BindSeService(data, reply);
    EXPECT_TRUE(rps == TestSaName::SA_FUNC_SUCC);
    int errorCode = reply.ReadInt32();
    GTEST_LOG_(INFO) << "error code = " << errorCode;
    EXPECT_TRUE(errorCode == TestSaName::NO_ERROR);
    GTEST_LOG_(INFO) << "get bind Se service success!";
}

/**
 * @tc.name: GetSecureElements001
 * @tc.desc: Verify if can get secure elements
 * @tc.type: FUNC
 * @tc.require:AR000CSJO9
 * @tc.author: liuzhaoshen
 */
HWTEST_F(NfcSaUnitTest, GetSecureElements001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "NfcSaUnitTest GetSecureElements001";
    /**
     * @tc.steps: step1. get all secure elements
     * @tc.expected: step1. the string array of each secure element name
     */
    MessageParcel data;
    MessageParcel reply;
    WriteInterfaceToken(StringReplaceReturnEight("ohos.se.omapi.ISecureElementService"), data);
    int32_t rps = nfcSaSvr_->GetSecureElements(data, reply);
    if (rps != TestSaName::SA_FUNC_SUCC) {
        sleep(TestSaName::SLEEP_SEC + 2); // time for service connected.
        rps = nfcSaSvr_->GetSecureElements(data, reply);
    }
    EXPECT_TRUE(rps == TestSaName::SA_FUNC_SUCC);
    GTEST_LOG_(INFO) << "Get Secure Elements success!";
    int seNum = reply.ReadInt32();
    GTEST_LOG_(INFO) << "Secure Element number = " << seNum;
    EXPECT_TRUE(seNum > 0);
    std::string seName;
    for (int i = 1; i <= seNum; i++) {
        seName = Str16ToStr8(reply.ReadString16());
        GTEST_LOG_(INFO) << "SecureElement" << i << ": " << seName;
        EXPECT_TRUE(seName.length() > 0);
    }
}

/**
 * @tc.name: IsSecureElementPresent001
 * @tc.desc: Verify if card present in specific secure element
 * @tc.type: FUNC
 * @tc.require:AR000CSJP7
 * @tc.author: liuzhaoshen
 */
HWTEST_F(NfcSaUnitTest, IsSecureElementPresent001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "NfcSaUnitTest IsSecureElementPresent001";
    /**
     * @tc.steps: step1. set eSE name to verify card present
     * @tc.expected: step1. the return value is true (if card present in that SE)
     */
    std::string seName = "SIM1";
    MessageParcel data;
    MessageParcel reply;
    data.WriteString16(TestSaName::SECURE_ELEMENT_TOKEN);
    data.WriteString16(Str8ToStr16(seName));
    GTEST_LOG_(INFO) << "getuid = " << getuid();
    setuid(TestSaName::SYS_UID);
    GTEST_LOG_(INFO) << "getuid = " << getuid();
    int32_t rps = nfcSaSvr_->IsSecureElementPresent(data, reply);
    if (rps != TestSaName::SA_FUNC_SUCC) {
        sleep(TestSaName::SLEEP_SEC + 2); // time for service connected.
        data.WriteString16(TestSaName::SECURE_ELEMENT_TOKEN);
        rps = nfcSaSvr_->IsSecureElementPresent(data, reply);
    }
    GTEST_LOG_(INFO) << "rps = " << rps;
    EXPECT_TRUE(rps == TestSaName::SA_FUNC_SUCC);
}

/**
 * @tc.name: IsListenModeEnabled001
 * @tc.desc: Verify if Listen Mode open
 * @tc.type: FUNC
 * @tc.require: AR000CSJRA
 * @tc.author: wutong
 */
HWTEST_F(NfcSaUnitTest, IsListenModeEnabled001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "NfcSaUnitTest IsListenModeEnabled001";
    /**
     * @tc.steps: step1. query if cardemulation enabled
     * @tc.expected: step1. the return value is true or false (if get result)
     */
    MessageParcel data;
    MessageParcel reply;
    WriteInterfaceToken("com.nxp.nfc.INxpNfcAdapter", data);
    GTEST_LOG_(INFO) << "getuid = " << getuid();
    setuid(TestSaName::SYS_UID);
    GTEST_LOG_(INFO) << "getuid = " << getuid();
    int32_t rps = nfcSaSvr_->IsListenModeEnabled(data, reply);
    GTEST_LOG_(INFO) << "rps = " << rps;
    EXPECT_TRUE(rps == TestSaName::SA_FUNC_SUCC);
    bool result = (reply.ReadBool());
    GTEST_LOG_(INFO) << " result =  " << result;
    EXPECT_TRUE(result == true || result == false);
}

/**
 * @tc.name: SetListenMode001
 * @tc.desc: Verify if set Listen Mode successfully
 * @tc.type: FUNC
 * @tc.require: AR000CSJRA
 * @tc.author: wutong
 */
HWTEST_F(NfcSaUnitTest, SetListenMode001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "NfcSaUnitTest SetListenMode001";
    /**
     * @tc.steps: step1. disable mode A and B, and query if cardemulation enabled
     * @tc.expected: step1. the return value is 0 (if get result)
     */
    int mode = 0x0; // mCardEmulationService.DISABLE_MODE_A_B
    int override = 1;
    MessageParcel data;
    MessageParcel reply;
    WriteInterfaceToken("com.nxp.nfc.INxpNfcAdapter", data);
    data.WriteInt32(mode);
    data.WriteInt32(override);
    GTEST_LOG_(INFO) << "getuid = " << getuid();
    setuid(TestSaName::SYS_UID);
    GTEST_LOG_(INFO) << "getuid = " << getuid();
    int32_t rps = nfcSaSvr_->SetListenMode(data, reply);
    GTEST_LOG_(INFO) << "rps = " << rps;
    EXPECT_TRUE(rps == TestSaName::SA_FUNC_SUCC);
    int result = reply.ReadInt32();
    GTEST_LOG_(INFO) << " result =  " << result;
    EXPECT_TRUE(result == 0);
}

/**
 * @tc.name: SetListenMode002
 * @tc.desc: Verify if set Listen Mode successfully and get result
 * @tc.type: FUNC
 * @tc.require: AR000CSJRA
 * @tc.author: wutong
 */
HWTEST_F(NfcSaUnitTest, SetListenMode002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "NfcSaUnitTest SetListenMode002";
    /**
     * @tc.steps: step1. disable mode A and B, and query if cardemulation enabled
     * @tc.expected: step1. the return value is false (if get result)
     */
    int mode = 0x03; // mCardEmulationService.ENABLE_MODE_ALL
    int override = 1;
    MessageParcel data;
    MessageParcel reply;
    WriteInterfaceToken("com.nxp.nfc.INxpNfcAdapter", data);
    data.WriteInt32(mode);
    data.WriteInt32(override);
    GTEST_LOG_(INFO) << "getuid = " << getuid();
    setuid(TestSaName::SYS_UID);
    GTEST_LOG_(INFO) << "getuid = " << getuid();
    int32_t rps = nfcSaSvr_->SetListenMode(data, reply);
    GTEST_LOG_(INFO) << "rps = " << rps;
    EXPECT_TRUE(rps == TestSaName::SA_FUNC_SUCC);
    int result = reply.ReadInt32();
    GTEST_LOG_(INFO) << " result =  " << result;
    EXPECT_TRUE(result == 0);

    MessageParcel data1;
    MessageParcel reply1;
    WriteInterfaceToken("com.nxp.nfc.INxpNfcAdapter", data1);
    int32_t rps1 = nfcSaSvr_->IsListenModeEnabled(data1, reply1);
    GTEST_LOG_(INFO) << "rps1 = " << rps1;
    EXPECT_TRUE(rps1 == TestSaName::SA_FUNC_SUCC);
    bool result1 = (reply1.ReadBool());
    GTEST_LOG_(INFO) << " result1 =  " << result1;
    EXPECT_TRUE(result1 == true);
}

/**
 * @tc.name: SetListenMode003
 * @tc.desc: Verify if set Listen Mode successfully and get result
 * @tc.type: FUNC
 * @tc.require: AR000CSJRA
 * @tc.author: wutong
 */
HWTEST_F(NfcSaUnitTest, SetListenMode003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "NfcSaUnitTest SetListenMode003";
    /**
     * @tc.steps: step1. enable mode A and B and F, and query if cardemulation enabled
     * @tc.expected: step1. the return value is true (if get result)
     */
    int mode = 0x0; // mCardEmulationService.DISABLE_MODE_A_B
    int override = 1;
    MessageParcel data;
    MessageParcel reply;
    WriteInterfaceToken("com.nxp.nfc.INxpNfcAdapter", data);
    data.WriteInt32(mode);
    data.WriteInt32(override);
    GTEST_LOG_(INFO) << "getuid = " << getuid();
    setuid(TestSaName::SYS_UID);
    GTEST_LOG_(INFO) << "getuid = " << getuid();
    int32_t rps = nfcSaSvr_->SetListenMode(data, reply);
    GTEST_LOG_(INFO) << "rps = " << rps;
    EXPECT_TRUE(rps == TestSaName::SA_FUNC_SUCC);
    int result = reply.ReadInt32();
    GTEST_LOG_(INFO) << " result =  " << result;
    EXPECT_TRUE(result == 0);

    MessageParcel data1;
    MessageParcel reply1;
    WriteInterfaceToken("com.nxp.nfc.INxpNfcAdapter", data1);
    int32_t rps1 = nfcSaSvr_->IsListenModeEnabled(data1, reply1);
    GTEST_LOG_(INFO) << "rps1 = " << rps1;
    EXPECT_TRUE(rps1 == TestSaName::SA_FUNC_SUCC);
    bool result1 = (reply1.ReadBool());
    GTEST_LOG_(INFO) << " result1 =  " << result1;
    EXPECT_TRUE(result1 == false);
}

/**
 * @tc.name: GetNfcInfo001
 * @tc.desc: get NFC info, eSE type
 * @tc.type: FUNC
 * @tc.require: AR000CSJRA
 * @tc.author: wutong
 */
HWTEST_F(NfcSaUnitTest, GetNfcInfo001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "NfcSaUnitTest GetNfcInfo001";
    /**
     * @tc.steps: step1. query if get NFC info, eSE type successfully
     * @tc.expected: step1. the return value is string and it is more than 1 (if get result)
     */
    std::string infoType = "ese_type";
    MessageParcel data;
    MessageParcel reply;
    WriteInterfaceToken("com.nxp.nfc.INxpNfcAdapter", data);
    data.WriteString16(Str8ToStr16(infoType));
    GTEST_LOG_(INFO) << "getuid = " << getuid();
    setuid(TestSaName::SYS_UID);
    GTEST_LOG_(INFO) << "getuid = " << getuid();
    int32_t rps = nfcSaSvr_->GetNfcInfo(data, reply);
    GTEST_LOG_(INFO) << "rps = " << rps;
    EXPECT_TRUE(rps == TestSaName::SA_FUNC_SUCC);
    std::string result(Str16ToStr8(reply.ReadString16()).c_str());
    GTEST_LOG_(INFO) << " result =  " << result;
    EXPECT_TRUE(result.length() > 0);
}

/**
 * @tc.name: SetRfConfig001
 * @tc.desc: get NFC info, eSE type
 * @tc.type: FUNC
 * @tc.require: AR000CSJRA
 * @tc.author: wutong
 */
HWTEST_F(NfcSaUnitTest, SetRfConfig001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "NfcSaUnitTest SetRfConfig001";
    /**
     * @tc.steps: step1. query if set the RF configs
     * @tc.expected: step1. the return value is 0 (if get result)
     */
    std::string configs = "HW_RF_CONF_BLK={20,02,0C,01,A0,3A,08,0E,01,0E,01,0E,01,0E,01}";
    std::string pkg = "com.ohos.wallet";
    MessageParcel data;
    MessageParcel reply;
    WriteInterfaceToken("com.nxp.nfc.INxpNfcAdapter", data);
    data.WriteString16(Str8ToStr16(configs));
    data.WriteString16(Str8ToStr16(pkg));
    GTEST_LOG_(INFO) << "getuid = " << getuid();
    setuid(TestSaName::SYS_UID);
    GTEST_LOG_(INFO) << "getuid = " << getuid();
    int32_t rps = nfcSaSvr_->SetRfConfig(data, reply);
    GTEST_LOG_(INFO) << "rps = " << rps;
    EXPECT_TRUE(rps == TestSaName::SA_FUNC_SUCC);
    int result = reply.ReadInt32();
    GTEST_LOG_(INFO) << " result =  " << result;
    EXPECT_TRUE(result == 0 || result == 0xFF); // 0xFF: NFCC is busy.
}

/**
 * @tc.name: GetInfoFromConfig001
 * @tc.desc: get ese info from config file
 * @tc.type: FUNC
 * @tc.require: AR000CSJQF
 * @tc.author: maolongxia
 */
HWTEST_F(NfcSaUnitTest, GetInfoFromConfig001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "NfcSaUnitTest GetInfoFromConfig001";
    /**
     * @tc.steps: step1. query if set the RF configs
     * @tc.expected: step1. the return value is 0 (if get result)
     */
    int32_t feature = 2; // eSE
    MessageParcel data;
    MessageParcel reply;
    WriteInterfaceToken("com.nxp.nfc.INxpNfcAdapter", data);
    data.WriteInt32(feature);
    int32_t rps = nfcSaSvr_->GetInfoFromConfig(data, reply);
    GTEST_LOG_(INFO) << "rps = " << rps;
    EXPECT_TRUE(rps == TestSaName::SA_FUNC_SUCC);
    int result = reply.ReadInt32();
    GTEST_LOG_(INFO) << " result =  " << result;
    EXPECT_TRUE(result >= 0);
}

/**
 * @tc.name: NotificationNfcState001
 * @tc.desc: test the notification of NFC state changed.
 * @tc.type: FUNC
 * @tc.require: SR000CTD64 AR000CTD65
 * @tc.author: zhangxiuping
 */
HWTEST_F(NfcSaUnitTest, NotificationNfcState001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "NfcSaUnitTest NotificationNfcState001";
    /**
     * @tc.steps: step1. query if set the RF configs
     * @tc.expected: step1. the return value is 0 (if get result)
     */
    int32_t stateResult = -1;
    int32_t setRet = SetCommonEventSubscriber(stateResult);
    GTEST_LOG_(INFO) << "SetCommonEventSubscriber setRet = " << setRet;

    MessageParcel data;
    MessageParcel reply;
    data.WriteString16(TestSaName::NFC_CONTROLLER_TOKEN);
    int32_t defState = nfcSaSvr_->GetNfcState(data, reply);
    bool defIsOn = defState == TestSaName::NFC_STATE_ON;
    GTEST_LOG_(INFO) << "NotificationNfcState001 isOn = " << defIsOn;

    MessageParcel data1;
    MessageParcel reply1;
    data1.WriteString16(TestSaName::NFC_CONTROLLER_TOKEN);
    if (!defIsOn == true) {
        data1.WriteInt32(1);
    } else {
        data1.WriteInt32(0);
    }
    int32_t ret = nfcSaSvr_->SetNfcEnabled(data1, reply1);
    sleep(TestSaName::SLEEP_SEC); // time for waiting process

    // check the notification received.
    GTEST_LOG_(INFO) << "NotificationNfcState001 stateResult = " << stateResult;

    MessageParcel data2;
    MessageParcel reply2;
    data2.WriteString16(TestSaName::NFC_CONTROLLER_TOKEN);
    if (defIsOn == true) {
        data2.WriteInt32(1);
    } else {
        data2.WriteInt32(0);
    }
    ret = nfcSaSvr_->SetNfcEnabled(data2, reply2);
    sleep(TestSaName::SLEEP_SEC); // time for waiting process
}
} // namespace Nfc
} // namespace OHOS
