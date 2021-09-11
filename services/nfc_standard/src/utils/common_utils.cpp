#include "common_utils.h"

#include <chrono>

namespace OHOS {
namespace nfc {
int64_t CommonUtils::GetCurrentMilliSecondsTime()
{
    std::chrono::time_point<std::chrono::system_clock> nowSys = std::chrono::system_clock::now();
    auto epoch = nowSys.time_since_epoch();
    auto value = std::chrono::duration_cast<std::chrono::milliseconds>(epoch);
    int64_t duration = value.count();
    return duration;
}
}  // namespace nfc
}  // namespace OHOS
