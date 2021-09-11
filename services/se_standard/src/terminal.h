/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef TERMINAL_H
#define TERMINAL_H

#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

#include "iremote_object.h"
#include "isecure_element.h"

namespace osal {
class Context;
class BundleManager;
class BundleInfo;
}  // namespace osal

using ISecureElement = OHOS::hardware::se::v1_0::ISecureElement;

namespace OHOS::se::security {
class AccessControlEnforcer;
}
using AccessControlEnforcer = OHOS::se::security::AccessControlEnforcer;

namespace OHOS {
namespace se {
class SeChannel;
class SecureElementSession;
namespace security {
class ChannelAccessRule;
}

class Terminal final : public std::enable_shared_from_this<Terminal> {
    /*
     * Declare nesting class
     */
    class SecureElementDeathRecipient;
    /*
     * The entry for processing the EVENT_GET_HAL
     */
    class GetHalHandler;

public:
    class SecureElementCallback;

    explicit Terminal(std::string name, std::weak_ptr<osal::Context> context);
    std::weak_ptr<AccessControlEnforcer> GetAccessControlEnforcer();
    std::weak_ptr<osal::Context> GetContext();

    std::string GetName();
    void Initialize();
    void CloseChannel(std::weak_ptr<SeChannel> channel);
    void CloseChannels();
    void Close();
    std::string GetAtr();
    void SelectDefaultApplication();
    std::shared_ptr<SeChannel> OpenBasicChannel(std::string aid, char p2, std::string bundleName, int pid);
    std::shared_ptr<SeChannel> OpenLogicalChannelForAccessControl(std::string aid);
    std::shared_ptr<SeChannel> OpenLogicalChannel(const std::string& aid,
                                                  char p2,
                                                  const std::string& bundleName,
                                                  int pid);
    bool IsAidSelectable(const std::string& aid);
    std::string Transmit(std::string cmd);
    std::vector<bool> IsNfcEventAllowed(std::weak_ptr<osal::BundleManager> bundleManager,
                                        std::string aid,
                                        std::vector<std::string> bundleNames);
    bool IsSecureElementPresent();
    bool CheckCarrierPrivilegeRules(std::weak_ptr<osal::BundleInfo> pInfo);

private:
    void Initialize(const sptr<ISecureElement>& seHalService);
    void SelectApplet(const std::string& aid);
    void StateChange(bool state, std::string reason);
    void InitializeAC();
    std::shared_ptr<security::ChannelAccessRule> EstablishChannelAccess(std::string aid,
                                                                        std::string bundleName,
                                                                        int pid);
    bool HasPrivilegedPermission(const std::string& bundleName);
    bool HasCarrierPrivilegedPermission(const std::string& bundleName);
    std::string TransmitInternal(const std::string& cmd);
    bool Reset();
    void ResetDefaultAppState();

private:
    bool mIsConnected_{false};
    /**
     * @brief the terminal(SE Hal Service) name
     */
    std::string mName_{};
    /**
     * @brief All Channels About the terminal
     */
    std::map<int, std::shared_ptr<SeChannel>> mChannels_{};
    std::recursive_mutex mLock_{};
    std::weak_ptr<osal::Context> mContext_{};
    bool mDefaultAppOnBasicChannel_{true};
    /**
     * @brief The SE Hal Instance
     */
    sptr<ISecureElement> mSEHal_{};
    /**
     * @brief Callback is used to notify the state change
     */
    sptr<OHOS::hardware::se::v1_0::ISecureElementCallback> mSECallback_{};
    /**
     * @brief There is one AccessController object for each Terminal
     */
    std::shared_ptr<AccessControlEnforcer> mAccessControlEnforcer_{};
    sptr<IRemoteObject::DeathRecipient> mDeathRecipient_{};
    std::shared_ptr<GetHalHandler> mHandler_{};

    static const uint64_t GET_SERVICE_DELAY_MILLIS{4 * 1000};
    static const uint32_t EVENT_GET_HAL{1};
    const std::string SECURE_ELEMENT_PRIVILEGED_OPERATION_PERMISSION{
        "ohos.permission.SECURE_ELEMENT_PRIVILEGED_OPERATION"};

    friend class SecureElementReader;
};
}  // namespace se
}  // namespace OHOS
#endif  // !TERMINAL_H