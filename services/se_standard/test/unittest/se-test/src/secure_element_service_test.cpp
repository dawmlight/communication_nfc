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
#include "secure_element_service.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>

#include "context.h"
#include "ipc_object_stub.h"
#include "isecure_element_mock.h"
#include "isecure_element_reader.h"
#include "loghelper.h"
#include "se_common_exception.h"
#include "se_end_service.h"
#include "terminal.h"
#undef private

using namespace testing;
using namespace osal;
using OHOS::hardware::se::v1_0::ISecureElementMock;
namespace OHOS {
namespace se {
namespace test {
const std::string ESE_READER{"eSE1"};
class SecureElementServiceTest : public ::testing::Test {
public:
    virtual void SetUp() override
    {
        context_ = std::make_shared<Context>();
        LoadTerminalTable();
        InfoLog("==SetUp==");
        seServiceManager_ = std::make_shared<SeEndService>();
        seServiceManager_->Init(context_);

        seService_ = new SecureElementService(seServiceManager_, terminalTable_);
    }

    virtual void TearDown() override
    {
        InfoLog("==TearDown==");
        context_ = std::shared_ptr<Context>();
        seService_ = nullptr;
        seServiceManager_ = nullptr;

        terminalTable_.clear();
    }

    void LoadTerminalTable()
    {
        sptr<ISecureElementMock> seMock = new ISecureElementMock();
        EXPECT_CALL(*seMock, Initialize(_)).Times(AnyNumber());
        sptr<IRemoteObject> obj = new IPCObjectStub(u"ISecureElementMock");
        EXPECT_CALL(*seMock, AsObject()).WillRepeatedly(Return(obj));

        std::shared_ptr<Terminal> terminal = std::make_shared<Terminal>(ESE_READER, context_);
        terminal->Initialize(seMock);
        terminalTable_.push_back(std::make_pair(ESE_READER, terminal));
    }

public:
    sptr<SecureElementService> seService_;
    std::shared_ptr<SeEndService> seServiceManager_;
    std::shared_ptr<Context> context_;
    TerminalTable terminalTable_;
};

TEST_F(SecureElementServiceTest, GetReaders_Test)
{
    std::vector<std::string> readers = seService_->GetReaders();
    EXPECT_TRUE(readers.size() >= 1);
    std::vector<std::string>::iterator iter = std::find(readers.begin(), readers.end(), "eSE1");
    EXPECT_TRUE(iter != readers.end());
}

TEST_F(SecureElementServiceTest, GetReader_Test)
{
    sptr<ISEReader> reader = nullptr;
    std::string readerName{};
    sptr<IRemoteObject> listener = nullptr;
    // reader listener is empty.
    try {
        reader = seService_->GetReader(readerName, listener);
        EXPECT_TRUE(false);
    } catch (const IllegalArgumentError& e) {
        EXPECT_TRUE(true);
    } catch (const std::exception& e) {
        InfoLog("exception: %s", e.what());
        EXPECT_TRUE(false);
    }

    listener = new IPCObjectStub(u"ohos.se.test.SecureElementServiceTest");
    // reader name is empty.
    try {
        reader = seService_->GetReader(readerName, listener);
        EXPECT_TRUE(false);
    } catch (const IllegalArgumentError& e) {
        EXPECT_TRUE(true);
    } catch (const std::exception& e) {
        InfoLog("exception: %s", e.what());
        EXPECT_TRUE(false);
    }

    // reader name is SIM.
    readerName = "SIM";
    try {
        reader = seService_->GetReader(readerName, listener);
        EXPECT_TRUE(false);
    } catch (const IllegalArgumentError& e) {
        EXPECT_TRUE(true);
    } catch (const std::exception& e) {
        InfoLog("exception: %s", e.what());
        EXPECT_TRUE(false);
    }

    // reader name is eSE1.
    readerName = ESE_READER;
    try {
        reader = seService_->GetReader(readerName, listener);
        EXPECT_TRUE(reader);
    } catch (const std::exception& e) {
        InfoLog("exception: %s", e.what());
        EXPECT_TRUE(false);
    }
}

TEST_F(SecureElementServiceTest, IsNfcEventAllowed_Test)
{
    // aid length is less than 5
    std::string aid{0x00, 0x00, 0x00, 0x00};
    std::vector<std::string> packageNames{};
    std::vector<bool> result{};
    try {
        result = seService_->IsNfcEventAllowed(ESE_READER, aid, packageNames);
        EXPECT_TRUE(false);
    } catch (const IllegalArgumentError& e) {
        EXPECT_TRUE(true);
    } catch (const std::exception& e) {
        InfoLog("exception: %s", e.what());
        EXPECT_TRUE(false);
    }

    // aid length is bigger than 16
    aid = "1234567890abcdef1";
    try {
        result = seService_->IsNfcEventAllowed(ESE_READER, aid, packageNames);
        EXPECT_TRUE(false);
    } catch (const IllegalArgumentError& e) {
        EXPECT_TRUE(true);
    } catch (const std::exception& e) {
        InfoLog("exception: %s", e.what());
        EXPECT_TRUE(false);
    }

    // package names is empty
    aid = "";
    try {
        result = seService_->IsNfcEventAllowed(ESE_READER, aid, packageNames);
        EXPECT_TRUE(false);
    } catch (const IllegalArgumentError& e) {
        EXPECT_TRUE(true);
    } catch (const std::exception& e) {
        InfoLog("exception: %s", e.what());
        EXPECT_TRUE(false);
    }

    // package names is empty
    aid = std::string({0x00, 0x00, 0x00, 0x00, 0x00});
    packageNames.push_back("123");
    packageNames.push_back("234");
    packageNames.push_back("345");
    try {
        result = seService_->IsNfcEventAllowed(ESE_READER, aid, packageNames);
        EXPECT_TRUE(result.size() == 3);
        EXPECT_FALSE(result[0]);
        EXPECT_FALSE(result[1]);
        EXPECT_FALSE(result[2]);
    } catch (const std::exception& e) {
        InfoLog("exception: %s", e.what());
        EXPECT_TRUE(false);
    }
}
}  // namespace test
}  // namespace se
}  // namespace OHOS