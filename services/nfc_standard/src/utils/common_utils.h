#ifndef COMMON_UTILS_H
#define COMMON_UTILS_H

#include <stdio.h>

namespace OHOS {
namespace nfc {
class CommonUtils final {
public:
    static int64_t GetCurrentMilliSecondsTime();
};
}  // namespace nfc
}  // namespace OHOS
#endif  // !COMMON_UTILS_H
