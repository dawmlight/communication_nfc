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
#ifndef SE_SERVICE_H
#define SE_SERVICE_H

#include <iostream>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <vector>

#include "if_system_ability_connection_callback.h"
#include "isecure_element_service.h"

namespace OHOS {
class IRemoteObject;
class ISystemAbilityManager;

namespace se::omapi {
class Reader;
class ISecureElementReader;
class SEService : public std::enable_shared_from_this<SEService> {
    class SaConnectionHandler;

public:
    static const int IO_ERROR = 1;
    static const int NO_SUCH_SE_ERROR = 2;

public:
    class OnConnectedListener {
    public:
        virtual ~OnConnectedListener() = default;
        /**
         * @brief the notify when the service connect with se end service.
         *
         */
        virtual void CBConnected() = 0;
    };

    SEService();
    ~SEService();
    bool InitSEService(std::shared_ptr<OnConnectedListener> listener);
    /**
     * @brief Tells whether or not the service is connected.
     * @return true if the service is connected, false otherwise.
     */
    bool IsConnected();
    /**
     * @brief Get the list of available SE readers. There must be no duplicated objects in the returned list. All
     * available readers SHALL be listed even if no card is inserted
     *
     * @return std::vector<std::shared_ptr<Reader>> The readers list, as an array of readers. If there are no readers,
     * the returned array is of length 0
     */
    std::vector<std::shared_ptr<Reader>> GetReaders();
    /**
     * @brief Get the Uicc Reader object
     *
     * @param slotNumber the special uicc slot number
     * @return std::shared_ptr<Reader> the UICC reader instance
     */
    std::shared_ptr<Reader> GetUiccReader(int slotNumber);
    /**
     * @brief Releases all SE resources allocated by this SEService (including any binding to an underlying service). As
     * a result isConnected() will return false after shutdown() was called. After this method call, the SEService
     * object is not connected.
     */
    void Shutdown();
    /**
     * @brief Returns the version of the Open Mobile API Specification this implementation is based on.
     *
     * @return String containing the Open Mobile API version (e.g. “3.3” for Open Mobile API Specification version 3.3).
     */
    std::string GetVersion() const;

private:
    sptr<ISecureElementReader> GetReader(const std::string& readerName);
    void LoadReaders();
    bool LoadSecureElementService();

private:
    static std::string mUiccTerminal_;
    OHOS::sptr<ISecureElementService> mSecureElementService_;
    OHOS::sptr<ISystemAbilityManager> mSystemAbilityMgr_;
    std::mutex mMutex_;
    std::unordered_map<std::string, std::shared_ptr<Reader>> mReaders_;
    std::shared_ptr<OnConnectedListener> mConnListener_;
    std::shared_ptr<SaConnectionHandler> mConnectionHandler_;
    sptr<ISystemAbilityConnectionCallback> mServiceConnecter_;
    sptr<IRemoteObject> mListener_;

    friend class SystemAbilityConnectCallbackStub;
};
}  // namespace se::omapi
}  // namespace OHOS
#endif  // SE_SERVICE_H
