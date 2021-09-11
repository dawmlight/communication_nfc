#ifndef _CONTEXT_H_
#define _CONTEXT_H_

#include <map>
#include <memory>
#include <mutex>
#include <string>

namespace OHOS {
namespace AAFwk {
class Want;
}  // namespace AAFwk
}  // namespace OHOS

namespace osal {
class BundleManager;
class SharedPreferences;
class Resources;
class PowerManager;
class KeyguardManager;

class Context {
public:
    Context();
    virtual ~Context();

    std::weak_ptr<PowerManager> GetPowerManagerService();
    std::weak_ptr<KeyguardManager> GetKeyguardManagerService();
    std::weak_ptr<SharedPreferences> GetSharedPreferences(std::string name);
    std::weak_ptr<Resources> GetResources();
    std::weak_ptr<BundleManager> GetBundleManager();
    // Launch a new activity.
    void StartAbility(const OHOS::AAFwk::Want& want);
    bool EnforceCallingOrSelfPermission(const std::string& permission);

private:
};
}  // namespace osal

#endif