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

#ifndef I_HOST_CARD_EMULATION_SERVICE_H
#define I_HOST_CARD_EMULATION_SERVICE_H

#include <memory>
#include <string>
#include <vector>

#include "ability.h"
#include "element_name.h"
#include "iremote_object.h"
namespace OHOS {
namespace nfc::sdk::cardemulation {
class ApduChannelProxy;
class ApduChannelStub;

enum EN_DEACTIVATION_REASON { LINK_LOSS, DESELECTED };
enum EN_MSG_HCE { COMMAND_APDU, RESPONSE_APDU, DEACTIVATED, UNHANDLED };

class IHostCardEmulationService : public OHOS::AppExecFwk::Ability,
                                  public std::enable_shared_from_this<IHostCardEmulationService> {
public:
    IHostCardEmulationService();
    explicit IHostCardEmulationService(const OHOS::AppExecFwk::ElementName& serviceName);
    virtual ~IHostCardEmulationService();

    virtual std::vector<unsigned char> HandleApdu(const std::vector<unsigned char>&) = 0;
    virtual void OnDeactivated(int reason) = 0;
    void SendApduResponse(std::vector<unsigned char> data);
    void NotifyUnhandled();

    OHOS::sptr<IRemoteObject> OnConnect(const OHOS::AAFwk::Want& want) override;

private:
    void CreateMessenger();

private:
    OHOS::AppExecFwk::ElementName name_;
    OHOS::sptr<ApduChannelStub> channel_;
    OHOS::sptr<ApduChannelProxy> serviceChannel_;
};

}  // namespace nfc::sdk::cardemulation
}  // namespace OHOS
#endif  // I_HOST_CARD_EMULATION_SERVICE_H
