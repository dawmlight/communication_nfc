#include "common_utils.h"

#include <sstream>

namespace OHOS {
namespace se {
std::string CommonUtils::BinToHex(const std::string& data)
{
    const char* hex{"0123456789ABCDEF"};
    std::stringstream ss;

    for (char ch : data) {
        ss << hex[ch >> 4] << hex[ch & 0xf];
    }
    return ss.str();
}
}  // namespace se
}  // namespace OHOS
