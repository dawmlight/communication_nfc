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

#ifndef CARD_EMULATION_EVENT_HANDLER_H
#define CARD_EMULATION_EVENT_HANDLER_H

#include <iremote_object.h>

#include <memory>
#include <mutex>
#include <string>
#include <vector>

#include "element_name.h"
#include "iaid_routing_manager.h"
#include "icard_emulation_device_host.h"
#include "refbase.h"
namespace OHOS::nfc {
namespace sdk::cardemulation {
class Msg;
class IApduChannel;
class ApduChannelStub;
}  // namespace sdk::cardemulation
namespace cardemulation {
class AbilityConnectionStub;

class CardEmulationEventHandler : public std::enable_shared_from_this<CardEmulationEventHandler>,
                                  public OHOS::IRemoteObject::DeathRecipient {
public:
    CardEmulationEventHandler(std::shared_ptr<ICardEmulationDeviceHost> ceDeviceHost,
                              std::shared_ptr<IAidRoutingManager> aidRoutingManager);
    ~CardEmulationEventHandler();
    void OnHCEActivated(void);
    int OnHCEData(const unsigned char* data, size_t len);
    void OnHCEDeactivated(void);
    void OnOffHostTransaction(void);

    void OnRemoteDied(const wptr<IRemoteObject>& object) override;

private:
    enum class EventHandlerState {
        IDLE,
        WAIT_FOR_SELECT_AID,
        WAIT_FOR_CONNECT_ABILITY,
        TRANSFERRING,
        WAIT_FOR_DEACTIVE
    };
    static int StateToInt(EventHandlerState state);

    void CloseTapAgainDialog();
    void SendDataToCeService(const OHOS::sptr<sdk::cardemulation::IApduChannel>& service,
                             const unsigned char* data,
                             size_t len);

    // 连接 ElementName
    OHOS::sptr<sdk::cardemulation::IApduChannel> ConnectAbility(
        int userId,
        const std::shared_ptr<OHOS::AppExecFwk::ElementName>& elementName);
    OHOS::sptr<sdk::cardemulation::IApduChannel> HandleConnectAbility(
        int userId,
        const std::shared_ptr<OHOS::AppExecFwk::ElementName>& elementName);
    OHOS::sptr<sdk::cardemulation::IApduChannel> HandleDisconnectAbility(
        int userId,
        const std::shared_ptr<OHOS::AppExecFwk::ElementName>& elementName);
    void DisconnectPrimaryService();
    void DisconnectNormalService();
    void CreateSecureServiceConnection();
    void CreateNormalServiceConnection();

    void HandleApduResponse(std::unique_ptr<sdk::cardemulation::Msg> msg);
    void NotifyActiveCEServiceDeactive(int reason);
    void LaunchTapAgainDialog();
    void LaunchSelectorDialog(const std::vector<ServiceInfoTypePair>& services,
                              const std::shared_ptr<OHOS::AppExecFwk::ElementName>& unhandledServiceName);
    void HandleDataInWaitForSelectState(const std::shared_ptr<CardEmulationServiceInfo>& serviceInfo,
                                        const std::string& aidSetType,
                                        const unsigned char* data,
                                        size_t len);
    int GetCurrentUserId() const;
    void SendDataToReader(std::vector<unsigned char> data);
    bool IsActiveServiceValid();
    OHOS::sptr<sdk::cardemulation::IApduChannel> GetActiveService();

private:
    std::shared_ptr<IAidRoutingManager> aidRoutingManager_{};
    std::mutex mu_{};
    EventHandlerState state_;
    OHOS::AppExecFwk::ElementName primaryServiceName_{};
    OHOS::sptr<sdk::cardemulation::IApduChannel> primaryService_{};
    OHOS::AppExecFwk::ElementName expectedPrimaryServiceName_{};

    OHOS::AppExecFwk::ElementName preferredServiceName_{};
    OHOS::sptr<sdk::cardemulation::IApduChannel> preferredService_{};

    OHOS::AppExecFwk::ElementName activeHCEServiceName_{};
    OHOS::wptr<sdk::cardemulation::IApduChannel> activeHCEService_{};
    OHOS::sptr<sdk::cardemulation::ApduChannelStub> apduChannel_{};
    enum ConnectionState { Disconnected, Connecting, Connected };
    ConnectionState connectionState_{Disconnected};
    OHOS::sptr<AbilityConnectionStub> secureConnectionObserver_;
    OHOS::sptr<AbilityConnectionStub> normalConnectionObserver_;

    std::string lastSelectAid_{};
    std::vector<unsigned char> selectApdu_{};
    std::weak_ptr<ICardEmulationDeviceHost> ceDeviceHost_{};
};
}  // namespace cardemulation
}  // namespace OHOS::nfc
#endif  // CARD_EMULATION_EVENT_HANDLER_H
