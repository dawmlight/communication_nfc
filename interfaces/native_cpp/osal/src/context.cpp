#include "context.h"

#include <string>
#include <thread>

#include "bundle_manager.h"
#include "key_guard_manager.h"
#include "loghelper.h"
#include "power_manager.h"
#include "resources.h"
#include "shared_preferences.h"
#include "want.h"

using namespace std;
using OHOS::AAFwk::Want;
namespace osal {
std::mutex mtx{};

std::weak_ptr<PowerManager> Context::GetPowerManagerService()
{
    static std::shared_ptr<PowerManager> instance = std::make_shared<PowerManager>();
    return instance;
}

std::weak_ptr<KeyguardManager> Context::GetKeyguardManagerService()
{
    static std::shared_ptr<KeyguardManager> instance = std::make_shared<KeyguardManager>();
    return instance;
}

std::weak_ptr<SharedPreferences> Context::GetSharedPreferences(string name)
{
    static std::shared_ptr<SharedPreferences> instance = std::make_shared<SharedPreferences>();
    static bool flag = true;
    if (flag) {
        const std::string ADMIN_PERM = "hos.Manifest.permission.WRITE_SECURE_SETTINGS";
        const std::string NFC_PERMISSION = "hos.Manifest.permission.NFC";
        const std::string NFC_PREFERRED_PAYMENT_INFO_PERMISSION =
            "android.Manifest.permission.NFC_PREFERRED_PAYMENT_INFO";

        flag = false;
        instance->PutBool(ADMIN_PERM, true);
        instance->PutBool(NFC_PERMISSION, true);
        instance->PutBool(NFC_PREFERRED_PAYMENT_INFO_PERMISSION, true);
    }
    return instance;
}

std::weak_ptr<Resources> Context::GetResources()
{
    static std::shared_ptr<Resources> instance = std::make_shared<Resources>();
    return instance;
}

std::weak_ptr<BundleManager> Context::GetBundleManager()
{
    static std::shared_ptr<BundleManager> instance = std::make_shared<BundleManager>();
    return instance;
}

void Context::StartAbility(const Want& want)
{
    DebugLog("StartAbility By Want Action %s", want.GetAction().c_str());
}

/**
 * @brief Check Permission
 *
 * @param permission The name of the permission being checked. This value cannot be null.
 * @return true Within The Special Permissions
 * @return false Without The Special Permissions
 */
bool Context::EnforceCallingOrSelfPermission(const std::string& permission)
{
    static std::weak_ptr<SharedPreferences> pref = GetSharedPreferences("nfc");
    if (!pref.lock()->GetBool(permission, false)) {
        return false;
    }
    return true;
}

Context::Context() {}

Context::~Context() {}
}  // namespace osal