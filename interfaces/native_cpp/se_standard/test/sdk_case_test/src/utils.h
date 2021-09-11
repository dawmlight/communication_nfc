#ifndef _UTILS_H_
#define _UTILS_H_
#include <fstream>

class Utils {
public:
    static void PrintString(std::string name, std::string str);
    static std::string GetCurrentTime();
    static void PrintToFile(std::ofstream& outFile,
                            std::string functionName,
                            std::string startTime,
                            std::string endTime);
};

#endif  // !_UTILS_H_