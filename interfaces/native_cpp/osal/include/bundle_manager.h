#ifndef OSAL_BUNDLE_MANAGER_H
#define OSAL_BUNDLE_MANAGER_H

#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

namespace OHOS::NativePreferences {
class Element;
}

namespace osal {
class BundleInfo;

class BundleManager {
public:
    BundleManager(/* args */);
    ~BundleManager();

    // For SE
    std::string GetCallingBundleName(int64_t uid);
    int CheckPermission(const std::string& permission, const std::string& bundleName);
    void AddPermission(const std::string& permission, const std::string& bundleName);
    std::vector<std::string> GetBundlesForUid(int64_t uid);
    void SetBundlesForUid(int64_t uid, const std::vector<std::string>& bundleNames);
    void RemoveBundlesForUid(int64_t uid);
    std::shared_ptr<BundleInfo> GetBundleInfo(std::string bundleName, int flags);
    void SetBundleInfo(const std::string& bundleName, std::shared_ptr<BundleInfo> bundleInfo);
    bool Marshalling(const std::string& configFile);
    bool Unmarshalling(const std::string& configFile);

public:
    static const int PERMISSION_GRANTED{0};
    static const int PERMISSION_DENIED{-1};
    static const int GET_SIGNATURES{64};
    static constexpr const auto SE_TEST_CONFIG{"/data/nfc/param/se_test_config.cfg"};

private:
    // set up
    bool SetUpBundleNames(OHOS::NativePreferences::Element& bundleNames);
    bool SetUpBundleInfos(OHOS::NativePreferences::Element& bundleInfos);
    // read
    bool ParseBundleNames(const std::vector<OHOS::NativePreferences::Element>& elements);
    bool ParseBundleInfos(const std::vector<OHOS::NativePreferences::Element>& element);
    std::shared_ptr<BundleInfo> LoadBundleInfo(const std::vector<OHOS::NativePreferences::Element>& elements);
    std::vector<std::string> LoadStringVector(const std::vector<OHOS::NativePreferences::Element>& elements);

private:
    std::mutex mtx_;
    // uid(high 32bit)+pid(low 32bit, default 0x0000) --- bundle name
    std::map<int64_t, std::vector<std::string>> bundleNames_{};
    // Bundle name - BundleInfo
    std::map<std::string, std::shared_ptr<BundleInfo>> bundleInfos_;
};

}  // namespace osal

#endif  // OSAL_BUNDLE_MANAGER_H
