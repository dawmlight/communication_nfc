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
#include "base_test.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <chrono>
#include <thread>

#include "access_rule_data_common.h"
#include "bundle_manager.h"
#include "context.h"
#include "ipc_object_stub.h"
#include "ipc_skeleton.h"
#include "isecure_element_mock.h"
#include "loghelper.h"
#include "osal_bundle_info.h"
#include "terminal.h"
#undef private

using OHOS::hardware::se::v1_0::ISecureElementCallback;
using OHOS::se::Terminal;
using osal::Context;
using namespace testing;
const std::string ESE_READER{"eSE1"};
const std::string SIM_READER{"SIM1"};
namespace OHOS {
namespace se {
namespace test {
sptr<ISecureElementMock> BaseTest::LoadSecureElement(bool state)
{
    sptr<ISecureElementMock> seMock{};
    seMock = new ISecureElementMock();
    EXPECT_CALL(*seMock, Initialize(_)).Times(AnyNumber());
    EXPECT_CALL(*seMock, CloseChannel(_)).Times(AnyNumber());
    EXPECT_CALL(*seMock, Transmit(_))
        .Times(AnyNumber())
        .WillOnce(Return(RESPONSE_REFERESH_TAG_D0))
        .WillOnce(Return(RESPONSE_ALL))
        .WillOnce(Return(RESPONSE_REFERESH_TAG_D0))
        .WillOnce(Return(RESPONSE_ALL))
        .WillRepeatedly(Return(RESPONSE_REFERESH_TAG_D0));
    EXPECT_CALL(*seMock, IsCardPresent()).WillRepeatedly(Return(true));
    sptr<IRemoteObject> obj = new IPCObjectStub(u"ISecureElementMock");
    EXPECT_CALL(*seMock, AsObject()).WillRepeatedly(Return(obj));
    // Initialize Call
    SetSeInitializeCall(state, seMock);
    return seMock;
}

void BaseTest::SetSeInitializeCall(bool status, sptr<ISecureElementMock> seMock)
{
    if (status) {
        ON_CALL(*seMock, Initialize(_)).WillByDefault([](const sptr<IRemoteObject>& obj) {
            if (obj == nullptr) {
                return;
            }
            sptr<ISecureElementCallback> callback = iface_cast<ISecureElementCallback>(obj);
            EXPECT_TRUE(callback != nullptr);
            callback->OnStateChange(true, "Initialize is success.");
        });
        return;
    }

    ON_CALL(*seMock, Initialize(_)).WillByDefault([](const sptr<IRemoteObject>& obj) {
        if (obj == nullptr) {
            return;
        }
        sptr<ISecureElementCallback> callback = iface_cast<ISecureElementCallback>(obj);
        EXPECT_TRUE(callback != nullptr);
        callback->OnStateChange(false, "Initialize is failed.");
    });
}

TerminalTable BaseTest::LoadTerminalTable(std::shared_ptr<Context> context, sptr<ISecureElementMock> seMock)
{
    TerminalTable terminalTable;
    std::unique_ptr<SELogicalRespData> respData{};
    // add eSE1
    respData = std::make_unique<SELogicalRespData>();
    respData->status = SEStatus::SUCCESS;
    respData->channelNumber = 19;
    respData->resp = {(char)0x90, 0x00};
    EXPECT_CALL(*seMock, OpenLogicalChannel(_, _)).WillOnce(Return(ByMove(std::move(respData))));
    std::shared_ptr<Terminal> terminal = std::make_shared<Terminal>(ESE_READER, context);
    try {
        terminal->Initialize(seMock);
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    } catch (const std::exception& e) {
        InfoLog("Terminal::Initialize(%s) exception: %s", terminal->GetName().c_str(), e.what());
        EXPECT_TRUE(false);
    }
    terminalTable.push_back(std::make_pair(ESE_READER, terminal));
    // add SIM1
    respData = std::make_unique<SELogicalRespData>();
    respData->status = SEStatus::SUCCESS;
    respData->channelNumber = 18;
    respData->resp = {(char)0x90, 0x00};
    EXPECT_CALL(*seMock, OpenLogicalChannel(_, _)).WillOnce(Return(ByMove(std::move(respData))));
    terminal = std::make_shared<Terminal>(SIM_READER, context);
    try {
        terminal->Initialize(seMock);
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    } catch (const std::exception& e) {
        InfoLog("Terminal::Initialize(%s) exception: %s", terminal->GetName().c_str(), e.what());
        EXPECT_TRUE(false);
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    terminalTable.push_back(std::make_pair(SIM_READER, terminal));
    return terminalTable;
}

void BaseTest::SetUpBundleInfo(std::string bundleName, std::shared_ptr<Context> context)
{
    std::vector<std::string> bundleNames;
    bundleNames.push_back(bundleName);
    std::shared_ptr<osal::BundleManager> bundleMng = context->GetBundleManager().lock();
    bundleMng->AddPermission("0987654321", bundleName);
    std::shared_ptr<osal::BundleInfo> bundleInfo = bundleMng->GetBundleInfo(bundleName, 0);
    bundleInfo->signatures_.push_back("1234567890abcdef");
    bundleMng->SetBundlesForUid(IPCSkeleton::GetCallingUid(), bundleNames);
}

void BaseTest::SetUpOpenLogicalChannel(sptr<ISecureElementMock>& seMock)
{
    std::unique_ptr<SELogicalRespData> respData{};
    respData = std::make_unique<SELogicalRespData>();
    respData->status = SEStatus::SUCCESS;
    respData->channelNumber = 3;
    respData->resp = {(char)0x90, 0x00};
    EXPECT_CALL(*seMock, OpenLogicalChannel(_, _)).WillOnce(Return(ByMove(std::move(respData))));
    EXPECT_CALL(*seMock, Transmit(_)).Times(AnyNumber()).WillRepeatedly(Return(RESPONSE_REFERESH_TAG_D0));
}

void BaseTest::SetUpOpenLogicalChannelWithPreData(sptr<ISecureElementMock>& seMock,
                                                  std::unique_ptr<SELogicalRespData>& preRespData)
{
    std::unique_ptr<SELogicalRespData> respData{};
    respData = std::make_unique<SELogicalRespData>();
    respData->status = SEStatus::SUCCESS;
    respData->channelNumber = 3;
    respData->resp = {(char)0x90, 0x00};
    EXPECT_CALL(*seMock, OpenLogicalChannel(_, _))
        .WillOnce(Return(ByMove(std::move(respData))))
        .WillOnce(Return(ByMove(std::move(preRespData))));
    EXPECT_CALL(*seMock, Transmit(_)).WillRepeatedly(Return(RESPONSE_REFERESH_TAG_D0));
}
}  // namespace test
}  // namespace se
}  // namespace OHOS