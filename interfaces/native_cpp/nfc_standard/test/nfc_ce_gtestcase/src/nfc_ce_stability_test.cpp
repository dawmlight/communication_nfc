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
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <fstream>
#include <iomanip>
#include <memory>
#include <sstream>
#include <thread>
#include <vector>

#include "card_emulation.h"
#include "card_emulation_service_info_lite.h"
#include "element_name.h"
#include "ihost_card_emulation.h"
#include "ihost_card_emulation_service.h"
#include "ioffhost_card_emulation.h"
#include "ioffhost_card_emulation_service.h"
#include "loghelper.h"
#include "nfc_agent.h"
#include "test_nfc_ce_app.h"
#include "test_nfc_ce_host_service.h"

using namespace testing;
using namespace OHOS::nfc::sdk;
using namespace OHOS::nfc::sdk::cardemulation;
using namespace OHOS::AppExecFwk;

namespace OHOS::nfc::test {
class NfcCeStabilityTest : public testing::Test {
public:
    virtual void SetUp() override
    {
        std::cout << " SetUp NfcCeStabilityTest." << std::endl;
        loopTimes_ = 1000;
        std::string appName = "AppName";
        std::shared_ptr<OHOSApplication> application = TestNfcCEApp::GenAppliaction(appName);
        nfcAgent_ = NfcAgent::GetNFCAgent(application);
        ASSERT_TRUE(nfcAgent_);
    }

    virtual void TearDown() override
    {
        std::cout << " TearDown NfcCeStabilityTest." << std::endl;
        outFile_.close();
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    std::string GetCurrentTime();
    void PrintToFile(std::string functionName, std::string startTime, std::string endTime);
    static std::vector<unsigned char> TestApduCommandHandler(std::vector<unsigned char> adpu);
    static void TestDeactiveEventHandler(int reason);

public:
    int loopTimes_;
    std::shared_ptr<NfcAgent> nfcAgent_;
    std::ofstream outFile_;
};

std::string NfcCeStabilityTest::GetCurrentTime()
{
    struct timespec time;
    clock_gettime(CLOCK_REALTIME, &time);
    std::stringstream ss;
    // microseconds
    ss << std::setw(6) << std::setfill('0') << time.tv_nsec / 1000;
    return std::to_string(time.tv_sec) + ss.str();
}

void NfcCeStabilityTest::PrintToFile(std::string functionName, std::string startTime, std::string endTime)
{
    std::string consumeTime = std::to_string(atoll(endTime.c_str()) - atoll(startTime.c_str()));
    outFile_ << functionName << "," << startTime << "," << endTime << "," << consumeTime << std::endl;
}

std::vector<unsigned char> NfcCeStabilityTest::TestApduCommandHandler(std::vector<unsigned char> adpu)
{
    InfoLog("NfcCeStabilityTest::TestApduCommandHandler");
    std::vector<unsigned char> response = {0x90, 0x00};
    return response;
}

void NfcCeStabilityTest::TestDeactiveEventHandler(int reason)
{
    InfoLog("NfcCeStabilityTest::TestDeactiveEventHandler, reason{%d}", reason);
}

TEST_F(NfcCeStabilityTest, Card_Emulation_Service_Test)
{
    time_t t = time(NULL);
    char ch[64] = {0};
    strftime(ch, sizeof(ch) - 1, "%Y-%m-%d-%H-%M-%S", localtime(&t));
    std::string fileName = "Nfc-Ce-Stability-Test-Card-Emulation-Service-" + std::string(ch) + ".csv";
    outFile_.open(fileName, std::ios::out);
    outFile_ << "API Name"
             << ","
             << "start time stamp(microseconds)"
             << ","
             << "end time stamp(microseconds) "
             << ","
             << "time consumption(microseconds)" << std::endl;

    std::string startTime;
    std::string endTime;
    for (int i = 0; i < loopTimes_; i++) {
        InfoLog("===Nfc-Ce-Stability-Test-Card-Emulation-Service number: %d===", (i + 1));

        int userId = 0;
        ElementName serviceName("DeviceId", "BundleName", "AbilityName");
        std::string type = "secure";
        OHOS::sptr<OHOS::IRemoteObject> apduChannel;
        std::vector<std::string> aids = {"A000000000112233445566778899AABB"};
        std::string offHostSecureElement = "OffHostSecureElement";

        startTime = GetCurrentTime();
        std::shared_ptr<CardEmulation> cardEmulation = CardEmulation::GetInstance(nfcAgent_);
        endTime = GetCurrentTime();
        PrintToFile("CardEmulation::GetInstance()", startTime, endTime);

        startTime = GetCurrentTime();
        cardEmulation->GetServices(type);
        endTime = GetCurrentTime();
        PrintToFile("CardEmulation::GetServices()", startTime, endTime);

        startTime = GetCurrentTime();
        cardEmulation->GetAidsForService(serviceName, type);
        endTime = GetCurrentTime();
        PrintToFile("CardEmulation::GetAidsForService()", startTime, endTime);

        startTime = GetCurrentTime();
        cardEmulation->RegisterRemoteObject(userId, serviceName, apduChannel);
        endTime = GetCurrentTime();
        PrintToFile("CardEmulation::RegisterRemoteObject()", startTime, endTime);

        startTime = GetCurrentTime();
        cardEmulation->AddAidsForService(serviceName, aids, type);
        endTime = GetCurrentTime();
        PrintToFile("CardEmulation::AddAidsForService()", startTime, endTime);

        startTime = GetCurrentTime();
        cardEmulation->RemoveAidsForService(serviceName, type);
        endTime = GetCurrentTime();
        PrintToFile("CardEmulation::RemoveAidsForService()", startTime, endTime);

        startTime = GetCurrentTime();
        cardEmulation->RegisterOffHostService(serviceName, offHostSecureElement);
        endTime = GetCurrentTime();
        PrintToFile("CardEmulation::RegisterOffHostService()", startTime, endTime);

        startTime = GetCurrentTime();
        cardEmulation->UnregisterOffHostService(serviceName);
        endTime = GetCurrentTime();
        PrintToFile("CardEmulation::UnregisterOffHostService()", startTime, endTime);

        startTime = GetCurrentTime();
        cardEmulation->IsSupportedAidPrefixMode();
        endTime = GetCurrentTime();
        PrintToFile("CardEmulation::IsSupportedAidPrefixMode()", startTime, endTime);

        startTime = GetCurrentTime();
        cardEmulation->GetDescriptionForPreferredService();
        endTime = GetCurrentTime();
        PrintToFile("CardEmulation::GetDescriptionForPreferredService()", startTime, endTime);

        startTime = GetCurrentTime();
        cardEmulation->SetPreferredForegroundService(serviceName);
        endTime = GetCurrentTime();
        PrintToFile("CardEmulation::SetPreferredForegroundService()", startTime, endTime);

        startTime = GetCurrentTime();
        cardEmulation->UnsetPreferredForegroundService();
        endTime = GetCurrentTime();
        PrintToFile("CardEmulation::UnsetPreferredForegroundService()", startTime, endTime);

        startTime = GetCurrentTime();
        cardEmulation->GetPrimaryServiceForType(type);
        endTime = GetCurrentTime();
        PrintToFile("CardEmulation::GetPrimaryServiceForType()", startTime, endTime);

        startTime = GetCurrentTime();
        cardEmulation->SetPrimaryServiceForType(serviceName, type);
        endTime = GetCurrentTime();
        PrintToFile("CardEmulation::SetPrimaryServiceForType()", startTime, endTime);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

TEST_F(NfcCeStabilityTest, Host_Card_Emulation_Test)
{
    time_t t = time(NULL);
    char ch[64] = {0};
    strftime(ch, sizeof(ch) - 1, "%Y-%m-%d-%H-%M-%S", localtime(&t));
    std::string fileName = "Nfc-Ce-Stability-Test-Host-Card-Emulation-" + std::string(ch) + ".csv";
    outFile_.open(fileName, std::ios::out);
    outFile_ << "API Name"
             << ","
             << "start time stamp(microseconds)"
             << ","
             << "end time stamp(microseconds) "
             << ","
             << "time consumption(microseconds)" << std::endl;

    std::string startTime;
    std::string endTime;
    for (int i = 0; i < loopTimes_; i++) {
        InfoLog("===Nfc-Ce-Stability-Test-Host-Card-Emulation number: %d===", (i + 1));

        ElementName serviceName("DeviceId", "BundleName", "AbilityName");
        std::string type = "secure";
        std::vector<std::string> aids = {"A000000000112233445566778899AABB"};

        startTime = GetCurrentTime();
        std::shared_ptr<IHostCardEmulation> hostCardEmulation = nfcAgent_->CreateHostCardEmulation(serviceName);
        endTime = GetCurrentTime();
        PrintToFile("NfcAgent::CreateHostCardEmulation()", startTime, endTime);
        ASSERT_TRUE(hostCardEmulation);

        startTime = GetCurrentTime();
        hostCardEmulation->SetApduCommandCallback(NfcCeStabilityTest::TestApduCommandHandler);
        endTime = GetCurrentTime();
        PrintToFile("HostCardEmulation::SetApduCommandCallback()", startTime, endTime);

        startTime = GetCurrentTime();
        hostCardEmulation->SetDeactiveEventCallback(NfcCeStabilityTest::TestDeactiveEventHandler);
        endTime = GetCurrentTime();
        PrintToFile("HostCardEmulation::SetDeactiveEventCallback()", startTime, endTime);

        startTime = GetCurrentTime();
        hostCardEmulation->AddAidsForService(aids, type);
        endTime = GetCurrentTime();
        PrintToFile("HostCardEmulation::AddAidsForService()", startTime, endTime);

        startTime = GetCurrentTime();
        hostCardEmulation->RemoveAidsForService(type);
        endTime = GetCurrentTime();
        PrintToFile("HostCardEmulation::RemoveAidsForService()", startTime, endTime);

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

TEST_F(NfcCeStabilityTest, Host_Card_Emulation_Service_Test)
{
    time_t t = time(NULL);
    char ch[64] = {0};
    strftime(ch, sizeof(ch) - 1, "%Y-%m-%d-%H-%M-%S", localtime(&t));
    std::string fileName = "Nfc-Ce-Stability-Test-Host-Card-Emulation-Service-" + std::string(ch) + ".csv";
    outFile_.open(fileName, std::ios::out);
    outFile_ << "API Name"
             << ","
             << "start time stamp(microseconds)"
             << ","
             << "end time stamp(microseconds) "
             << ","
             << "time consumption(microseconds)" << std::endl;

    std::string startTime;
    std::string endTime;
    for (int i = 0; i < loopTimes_; i++) {
        InfoLog("===Nfc-Ce-Stability-Test-Host-Card-Emulation-Service number: %d===", (i + 1));

        ElementName serviceName("DeviceId", "BundleName", "AbilityName");
        std::vector<unsigned char> apdu = {0x90, 0x00};

        auto hostCardEmulationService = std::make_shared<TestNfcCEHostService>();

        startTime = GetCurrentTime();
        hostCardEmulationService->SendApduResponse(apdu);
        endTime = GetCurrentTime();
        PrintToFile("HostCardEmulationService::SendApduResponse()", startTime, endTime);

        startTime = GetCurrentTime();
        hostCardEmulationService->NotifyUnhandled();
        endTime = GetCurrentTime();
        PrintToFile("HostCardEmulationService::NotifyUnhandled()", startTime, endTime);

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

TEST_F(NfcCeStabilityTest, OffHost_Card_Emulation_Test)
{
    time_t t = time(NULL);
    char ch[64] = {0};
    strftime(ch, sizeof(ch) - 1, "%Y-%m-%d-%H-%M-%S", localtime(&t));
    std::string fileName = "Nfc-Ce-Stability-Test-OffHost-Card-Emulation-" + std::string(ch) + ".csv";
    outFile_.open(fileName, std::ios::out);
    outFile_ << "API Name"
             << ","
             << "start time stamp(microseconds)"
             << ","
             << "end time stamp(microseconds) "
             << ","
             << "time consumption(microseconds)" << std::endl;

    std::string startTime;
    std::string endTime;
    for (int i = 0; i < loopTimes_; i++) {
        InfoLog("===Nfc-Ce-Stability-Test-OffHost-Card-Emulation number: %d===", (i + 1));

        ElementName serviceName("DeviceId", "BundleName", "AbilityName");
        std::string type = "secure";
        std::vector<std::string> aids = {"A000000000112233445566778899AABB"};
        std::string offHostSecureElement = "OffHostSecureElement";

        startTime = GetCurrentTime();
        std::shared_ptr<IOffHostCardEmulation> offhostCardEmulation =
            nfcAgent_->CreateOffHostCardEmulation(serviceName);
        endTime = GetCurrentTime();
        PrintToFile("NfcAgent::CreateOffHostCardEmulation()", startTime, endTime);
        ASSERT_TRUE(offhostCardEmulation);

        startTime = GetCurrentTime();
        offhostCardEmulation->AddAidsForService(aids, type);
        endTime = GetCurrentTime();
        PrintToFile("OffHostCardEmulation::AddAidsForService()", startTime, endTime);

        startTime = GetCurrentTime();
        offhostCardEmulation->RemoveAidsForService(type);
        endTime = GetCurrentTime();
        PrintToFile("OffHostCardEmulation::RemoveAidsForService()", startTime, endTime);

        startTime = GetCurrentTime();
        offhostCardEmulation->RegisterOffHostService(offHostSecureElement);
        endTime = GetCurrentTime();
        PrintToFile("OffHostCardEmulation::RegisterOffHostService()", startTime, endTime);

        startTime = GetCurrentTime();
        offhostCardEmulation->UnregisterOffHostService();
        endTime = GetCurrentTime();
        PrintToFile("OffHostCardEmulation::UnregisterOffHostService()", startTime, endTime);

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

TEST_F(NfcCeStabilityTest, OffHost_Card_Emulation_Service_Test)
{
    time_t t = time(NULL);
    char ch[64] = {0};
    strftime(ch, sizeof(ch) - 1, "%Y-%m-%d-%H-%M-%S", localtime(&t));
    std::string fileName = "Nfc-Ce-Stability-Test-OffHost-Card-Emulation-Service-" + std::string(ch) + ".csv";
    outFile_.open(fileName, std::ios::out);
    outFile_ << "API Name"
             << ","
             << "start time stamp(microseconds)"
             << ","
             << "end time stamp(microseconds) "
             << ","
             << "time consumption(microseconds)" << std::endl;

    std::string startTime;
    std::string endTime;
    for (int i = 0; i < loopTimes_; i++) {
        InfoLog("===Nfc-Ce-Stability-Test-OffHost-Card-Emulation-Service number: %d===", (i + 1));

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}
}  // namespace OHOS::nfc::test