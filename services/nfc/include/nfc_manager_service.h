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

#ifndef OHOS_NFC_MANAGER_SERVICE_H
#define OHOS_NFC_MANAGER_SERVICE_H

#include <memory>
#include <mutex>
#include <cstring>
#include <map>
#include <set>
#include "singleton.h"
#include "nocopyable.h"
#include "iremote_object.h"
#include "system_ability.h"
#include "if_system_ability_manager.h"
#include "iservice_registry.h"
#include "nfc_manager_stub.h"
#include "permission/permission_kit.h"

namespace OHOS {
namespace Nfc {
enum ServiceRunningState {
    STATE_NOT_START,
    STATE_RUNNING
};

class NfcManagerService : public SystemAbility, public NfcManagerStub {
DECLARE_SYSTEM_ABILITY(NfcManagerService); // necessary

public:
    DISALLOW_COPY_AND_MOVE(NfcManagerService);
    ~NfcManagerService();

    static sptr<NfcManagerService> GetInstance();
    int32_t CheckPermission(std::string permissionName, int callingUid);
    bool CheckRemoteDescriptor(std::string remoteDescriptor, std::string localDescriptor);

    /* NFC open or close operations */
    void OnStart() override;
    void OnStop() override;
    int32_t SetNfcEnabled(MessageParcel& data, MessageParcel& reply) override;
    bool IsNfcAvailable(MessageParcel& data, MessageParcel& reply) override;
    int32_t GetNfcState(MessageParcel& data, MessageParcel& reply) override;

    /* NFC Tag operations */
    bool ConnectTag(MessageParcel& data, MessageParcel& reply) override;
    bool IsTagConnected(MessageParcel& data, MessageParcel& reply) override;
    bool SetSendDataTimeout(MessageParcel& data, MessageParcel& reply) override;
    int32_t GetSendDataTimeout(MessageParcel& data, MessageParcel& reply) override;
    int32_t GetMaxSendLength(MessageParcel& data, MessageParcel& reply) override;
    int32_t SendData(MessageParcel& data, MessageParcel& reply) override;
    bool ReconnectTag(MessageParcel& data, MessageParcel& reply) override;
    void ResetTimeouts(MessageParcel& data, MessageParcel& reply) override;

    /* Secure element operations */
    sptr<IRemoteObject> GetRemoteNfcService() override;
    int32_t BindSeService(MessageParcel& data, MessageParcel& reply) override;
    int32_t IsSeServiceConnected(MessageParcel& data, MessageParcel& reply) override;
    int32_t GetSecureElements(MessageParcel& data, MessageParcel& reply) override;
    sptr<IRemoteObject> GetSecureElement(std::u16string seName) override;
    int32_t IsSecureElementPresent(MessageParcel& data, MessageParcel& reply) override;
    int32_t OpenSession(MessageParcel& data, MessageParcel& reply) override;
    int32_t GetAtr(MessageParcel& data, MessageParcel& reply) override;
    int32_t OpenChannel(MessageParcel& data, MessageParcel& reply) override;
    int32_t TransmitApdu(MessageParcel& data, MessageParcel& reply) override;
    int32_t CloseChannel(MessageParcel& data, MessageParcel& reply) override;
    int32_t IsChannelClosed(MessageParcel& data, MessageParcel& reply) override;
    int32_t GetSelectResponse(MessageParcel& data, MessageParcel& reply) override;
    int32_t CloseSeSessions(MessageParcel& data, MessageParcel& reply) override;
    int32_t CloseSessionChannels(MessageParcel& data, MessageParcel& reply) override;

    /* for listen mode */
    int32_t SetListenMode(MessageParcel& data, MessageParcel& reply) override;
    int32_t IsListenModeEnabled(MessageParcel& data, MessageParcel& reply) override;
    /* for get NFCC eSE info */
    int32_t GetNfcInfo(MessageParcel& data, MessageParcel& reply) override;
    /* for set RF parameter */
    int32_t SetRfConfig(MessageParcel& data, MessageParcel& reply) override;
    int32_t GetInfoFromConfig(MessageParcel& data, MessageParcel& reply) override;
    /* for set foreground app */
    int32_t SetForegroundPreferred(MessageParcel& data, MessageParcel& reply) override;
    int32_t UnsetForegroundPreferred(MessageParcel& data, MessageParcel& reply) override;

    /* for register aids */
    int32_t IsDefaultForAid(MessageParcel& data, MessageParcel& reply) override;
    int32_t RegisterAids(MessageParcel& data, MessageParcel& reply) override;
    int32_t RemoveAids(MessageParcel& data, MessageParcel& reply) override;
    int32_t GetAids(MessageParcel& data, MessageParcel& reply) override;
    int32_t IsDefaultForType(MessageParcel& data, MessageParcel& reply) override;

    /* NFC Tag operations 2.0 */
    int32_t IsNdefTag(MessageParcel& data, MessageParcel& reply) override;
    int32_t NdefRead(MessageParcel& data, MessageParcel& reply) override;
    int32_t NdefWrite(MessageParcel& data, MessageParcel& reply) override;
    int32_t NdefSetReadOnly(MessageParcel& data, MessageParcel& reply) override;
    int32_t CanSetReadOnly(MessageParcel& data, MessageParcel& reply) override;

    /* NFC Controller operations 2.0.1 */
    void RegisterForegroundDispatch(MessageParcel& data, MessageParcel& reply) override;
    bool SetReaderMode(MessageParcel& data, MessageParcel& reply) override;

    /* query service state */
    ServiceRunningState QueryServiceState() const
    {
        return state_;
    }
private:
    NfcManagerService();
    bool Init();
    void RegisterNfcConvertActions();
    bool registerToService_ = false;
    sptr<ISystemAbilityManager> systemManager_;
    ServiceRunningState state_ = ServiceRunningState::STATE_NOT_START;
    static std::mutex g_instanceLock;
    static sptr<NfcManagerService> g_nfcManagerService;
    sptr<IRemoteObject> g_remoteNfcService;
    sptr<IRemoteObject> g_remoteNxpService;
};
} // namespace Nfc
} // namespace OHOS
#endif // OHOS_NFC_ADAPTER_SERVICE_H