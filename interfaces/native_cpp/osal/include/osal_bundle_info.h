#ifndef OSAL_BUNDLE_INFO_H
#define OSAL_BUNDLE_INFO_H

#include <string>
#include <vector>

namespace osal {
class BundleInfo {
public:
    std::string mBundleName_;
    std::vector<std::string> signatures_;
    std::vector<std::string> permissions_;
};
}  // namespace osal

#endif  // !OSAL_BUNDLE_INFO_H