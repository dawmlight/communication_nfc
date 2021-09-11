#include "utils.h"

#include <iomanip>
#include <sstream>

void Utils::PrintString(std::string name, std::string str)
{
    printf("%s ", name.c_str());
    for (std::size_t i = 0; i < str.length(); i++) {
        printf("0x%X ", str.at(i));
    }
    printf("\n");
}

std::string Utils::GetCurrentTime()
{
    struct timespec time;
    clock_gettime(CLOCK_REALTIME, &time);
    std::stringstream ss;
    // 精确到微秒
    ss << std::setw(6) << std::setfill('0') << time.tv_nsec / 1000;
    return std::to_string(time.tv_sec) + ss.str();
}

void Utils::PrintToFile(std::ofstream& outFile, std::string functionName, std::string startTime, std::string endTime)
{
    std::string consumeTime = std::to_string(atoll(endTime.c_str()) - atoll(startTime.c_str()));
    outFile << functionName << "," << startTime << "," << endTime << "," << consumeTime << std::endl;
}