#include "isecure_element.h"

#include <map>

#include "iservice_registry.h"
#include "loghelper.h"
#include "st_se_service_proxy.h"
#include "system_ability_definition.h"

namespace OHOS {
namespace hardware {
namespace se {
namespace v1_0 {
const std::map<std::string, int32_t> g_SeServiceIdTable{{"eSE1", static_cast<int32_t>(ST_ESE1_SYS_ABILITY_ID)},
                                                        {"SIM1", static_cast<int32_t>(ST_SIM1_SYS_ABILITY_ID)}};
sptr<ISecureElement> ISecureElement::GetService(const std::string& name,
                                                const sptr<IRemoteObject::DeathRecipient>& recipient)
{
    if (name.empty()) {
        InfoLog("SE Name can't be empty.");
        return nullptr;
    }

    std::map<std::string, int32_t>::const_iterator iter = g_SeServiceIdTable.find(name);
    if (iter == g_SeServiceIdTable.end()) {
        InfoLog("The SE of Name (%s) is unexist.", name.c_str());
        return nullptr;
    }
    sptr<ISystemAbilityManager> sm = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (sm == nullptr) {
        InfoLog("ISystemAbilityManager (%s) is unexist.", name.c_str());
        return nullptr;
    }
    OHOS::sptr<OHOS::IRemoteObject> remoteObject_ = sm->CheckSystemAbility(iter->second);
    if (remoteObject_ == nullptr) {
        InfoLog("The SE service of (%s) isn't started. The ServiceId is %d", name.c_str(), iter->second);
        return nullptr;
    }
    remoteObject_->AddDeathRecipient(recipient);
    return new StSeServiceProxy(remoteObject_);
}
}  // namespace v1_0
}  // namespace se
}  // namespace hardware
}  // namespace OHOS
