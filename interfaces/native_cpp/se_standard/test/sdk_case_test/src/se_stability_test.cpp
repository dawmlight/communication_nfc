#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <fstream>
#include <memory>
#include <sstream>
#include <thread>
#include <vector>

#include "cb_connect_mock.h"
#include "channel.h"
#include "loghelper.h"
#include "reader.h"
#include "se_service.h"
#include "session.h"
#include "utils.h"

using namespace testing;

namespace OHOS::se {
namespace omapi {
namespace test {
// AID1 apdu-always
const std::string AID1 = {
    (char)0xA0, 0x00, 0x00, 0x03, 0x33, 0x00, 0x53, 0x44, 0x01, 0x21, 0x56, 0x11, 0x26, 0x24, 0x10, 0x00};
const std::string GET_CHALLENGE = {0x00, (char)0x84, 0x00, 0x00, 0x04};

TEST(SeStabilityTest, Stability_Test)
{
    time_t t = time(NULL);
    char ch[64] = {0};
    strftime(ch, sizeof(ch) - 1, "%Y-%m-%d-%H-%M-%S", localtime(&t));
    std::string fileName = "Se-Stability-Test-" + std::string(ch) + ".csv";

    std::ofstream outFile;
    outFile.open(fileName, std::ios::out);
    outFile << "API名称" << ',' << "开始时间戳（微秒）" << ',' << "结束时间戳（微秒）" << ',' << "耗时（微秒）"
            << std::endl;

    std::string startTime;
    std::string endTime;
    std::shared_ptr<SEService> seService;
    std::shared_ptr<Reader> reader;
    std::shared_ptr<Session> session;
    std::shared_ptr<CBConnectMock> listener = std::make_shared<CBConnectMock>();
    for (size_t i = 0; i < 1000; ++i) {
        InfoLog("===Stability_Test number: %d===", (i + 1));
        EXPECT_CALL(*listener, CBConnected()).Times(1);
        seService = std::make_shared<SEService>();

        startTime = Utils::GetCurrentTime();
        EXPECT_TRUE(seService->InitSEService(listener));
        endTime = Utils::GetCurrentTime();
        Utils::PrintToFile(outFile, "SEService::InitSEService", startTime, endTime);
        std::this_thread::sleep_for(std::chrono::milliseconds(50));

        startTime = Utils::GetCurrentTime();
        EXPECT_TRUE(seService->IsConnected());
        endTime = Utils::GetCurrentTime();
        Utils::PrintToFile(outFile, "SEService::IsConnected", startTime, endTime);

        startTime = Utils::GetCurrentTime();
        std::vector<std::shared_ptr<Reader>> readerList = seService->GetReaders();
        endTime = Utils::GetCurrentTime();
        Utils::PrintToFile(outFile, "SEService::GetReaders", startTime, endTime);
        ASSERT_TRUE(readerList.size() >= 1);
        reader = readerList.at(0);

        startTime = Utils::GetCurrentTime();
        reader->GetSEService();
        endTime = Utils::GetCurrentTime();
        Utils::PrintToFile(outFile, "Reader::GetSEService", startTime, endTime);

        startTime = Utils::GetCurrentTime();
        EXPECT_FALSE(reader->GetName().empty());
        endTime = Utils::GetCurrentTime();
        Utils::PrintToFile(outFile, "Reader::GetName", startTime, endTime);

        startTime = Utils::GetCurrentTime();
        session = reader->OpenSession();
        endTime = Utils::GetCurrentTime();
        Utils::PrintToFile(outFile, "Reader::OpenSession", startTime, endTime);
        ASSERT_TRUE(session);

        startTime = Utils::GetCurrentTime();
        std::shared_ptr<Channel> channelBasic = session->OpenBasicChannel(AID1);
        endTime = Utils::GetCurrentTime();
        Utils::PrintToFile(outFile, "Session::OpenBasicChannel", startTime, endTime);
        ASSERT_TRUE(channelBasic);

        startTime = Utils::GetCurrentTime();
        std::shared_ptr<Channel> channel = session->OpenLogicalChannel(AID1, 0x00);
        endTime = Utils::GetCurrentTime();
        Utils::PrintToFile(outFile, "Session::OpenLogicalChannel", startTime, endTime);
        ASSERT_TRUE(channel);

        startTime = Utils::GetCurrentTime();
        EXPECT_FALSE(channel->IsClosed());
        endTime = Utils::GetCurrentTime();
        Utils::PrintToFile(outFile, "Channel::IsClosed", startTime, endTime);

        startTime = Utils::GetCurrentTime();
        EXPECT_FALSE(channel->GetSelectResponse().empty());
        endTime = Utils::GetCurrentTime();
        Utils::PrintToFile(outFile, "Channel::GetSelectResponse", startTime, endTime);

        startTime = Utils::GetCurrentTime();
        std::string res = channel->Transmit(GET_CHALLENGE);
        endTime = Utils::GetCurrentTime();
        Utils::PrintToFile(outFile, "Channel::Transmit", startTime, endTime);
        EXPECT_FALSE(res.empty());

        startTime = Utils::GetCurrentTime();
        channel->Close();
        endTime = Utils::GetCurrentTime();
        Utils::PrintToFile(outFile, "Channel::Close", startTime, endTime);

        startTime = Utils::GetCurrentTime();
        EXPECT_TRUE(channel->IsClosed());
        endTime = Utils::GetCurrentTime();
        Utils::PrintToFile(outFile, "Channel::IsClosed", startTime, endTime);

        startTime = Utils::GetCurrentTime();
        session->CloseChannels();
        endTime = Utils::GetCurrentTime();
        Utils::PrintToFile(outFile, "Session::CloseChannels", startTime, endTime);

        startTime = Utils::GetCurrentTime();
        session->Close();
        endTime = Utils::GetCurrentTime();
        Utils::PrintToFile(outFile, "Session::IsClosed", startTime, endTime);
        EXPECT_TRUE(session->IsClosed());

        startTime = Utils::GetCurrentTime();
        reader->CloseSessions();
        endTime = Utils::GetCurrentTime();
        Utils::PrintToFile(outFile, "Reader::CloseSessions", startTime, endTime);

        startTime = Utils::GetCurrentTime();
        seService->Shutdown();
        endTime = Utils::GetCurrentTime();
        Utils::PrintToFile(outFile, "SEService::Shutdown", startTime, endTime);

        startTime = Utils::GetCurrentTime();
        EXPECT_FALSE(reader->IsSecureElementPresent());
        endTime = Utils::GetCurrentTime();
        Utils::PrintToFile(outFile, "Reader::IsSecureElementPresent", startTime, endTime);

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    outFile.close();
}
}  // namespace test
}  // namespace omapi
}  // namespace OHOS::se