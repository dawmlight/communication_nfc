#ifndef COMMON_UTILS_H
#define COMMON_UTILS_H

#include <string>

namespace OHOS {
namespace se {
class CommonUtils {
public:
    static std::string BinToHex(const std::string& data);
};
}  // namespace se
}  // namespace OHOS
#endif  // !COMMON_UTILS_H
