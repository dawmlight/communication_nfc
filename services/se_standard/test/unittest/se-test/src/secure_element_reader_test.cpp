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
#include "secure_element_reader.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>

#include "context.h"
#include "isecure_element_mock.h"
#include "loghelper.h"
#include "se_common_exception.h"
#include "se_end_service.h"
#include "secure_element_service.h"
#include "secure_element_session.h"
#include "terminal.h"
#undef private

using namespace testing;
using namespace OHOS::se::omapi;
using namespace osal;
using OHOS::hardware::se::v1_0::ISecureElementMock;
namespace OHOS {
namespace se {
namespace test {
const std::string ESE_READER{"eSE1"};
class SecureElementReaderTest : public ::testing::Test {
public:
    virtual void SetUp() override
    {
        context_ = std::make_shared<Context>();
        LoadTerminalTable();
        InfoLog("==SetUp==");
        seServiceManager_ = std::make_shared<SeEndService>();
        seServiceManager_->Init(context_);
        seService_ = new SecureElementService(seServiceManager_, terminalTable_);
        reader_ = new SecureElementReader(seServiceManager_, terminalTable_[0].second);
    }

    virtual void TearDown() override
    {
        InfoLog("==TearDown==");
        context_ = std::shared_ptr<Context>();
        seService_ = nullptr;
        seServiceManager_ = nullptr;
        seMock_ = nullptr;
        terminalTable_.clear();
        reader_ = nullptr;
    }

    void LoadTerminalTable()
    {
        seMock_ = new ISecureElementMock();
        EXPECT_CALL(*seMock_, Initialize(_)).Times(AnyNumber());
        sptr<IRemoteObject> obj = new IPCObjectStub(u"ISecureElementMock");
        EXPECT_CALL(*seMock_, AsObject()).WillRepeatedly(Return(obj));

        std::shared_ptr<Terminal> terminal = std::make_shared<Terminal>(ESE_READER, context_);
        terminal->Initialize(seMock_);
        terminalTable_.push_back(std::make_pair(ESE_READER, terminal));
    }

public:
    sptr<SecureElementReader> reader_;
    sptr<SecureElementService> seService_;
    std::shared_ptr<SeEndService> seServiceManager_;
    sptr<ISecureElementMock> seMock_;
    std::shared_ptr<Context> context_;
    TerminalTable terminalTable_;
};

TEST_F(SecureElementReaderTest, IsSecureElementPresent_Test)
{
    EXPECT_CALL(*seMock_, IsCardPresent()).WillRepeatedly(Return(true));
    EXPECT_TRUE(reader_->IsSecureElementPresent());
    EXPECT_CALL(*seMock_, IsCardPresent()).WillRepeatedly(Return(false));
    EXPECT_FALSE(reader_->IsSecureElementPresent());
    EXPECT_FALSE(reader_->GetTerminal().expired());
}

TEST_F(SecureElementReaderTest, OpenSession_Test)
{
    EXPECT_CALL(*seMock_, IsCardPresent()).WillRepeatedly(Return(true));
    sptr<ISESession> session = reader_->OpenSession();
    EXPECT_TRUE(session != nullptr);
    // The SE Isn't Present.
    EXPECT_CALL(*seMock_, IsCardPresent()).WillRepeatedly(Return(false));
    try {
        session = reader_->OpenSession();
        EXPECT_TRUE(session == nullptr);
    } catch (const ServiceSpecificError& e) {
        EXPECT_TRUE(true);
    } catch (const std::exception&) {
        EXPECT_FALSE(true);
    }
}

TEST_F(SecureElementReaderTest, CloseSessions_Test)
{
    EXPECT_CALL(*seMock_, IsCardPresent()).WillRepeatedly(Return(true));
    sptr<ISESession> session = reader_->OpenSession();
    EXPECT_TRUE(session != nullptr);
    session = reader_->OpenSession();
    EXPECT_TRUE(session != nullptr);
    EXPECT_FALSE(session->IsClosed());

    // close sessions
    reader_->CloseSessions();
    EXPECT_TRUE(session->IsClosed());
}

TEST_F(SecureElementReaderTest, Reset_Test)
{
    EXPECT_CALL(*seMock_, Reset()).Times(1);
    // close sessions
    reader_->Reset();
}

TEST_F(SecureElementReaderTest, GetAtr_Test)
{
    try {
        std::string atr = reader_->GetAtr();
        EXPECT_TRUE(atr.empty());
    } catch (const std::exception&) {
        EXPECT_TRUE(false);
    }
}
}  // namespace test
}  // namespace se
}  // namespace OHOS