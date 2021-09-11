/*
  * Copyright (C) 2021 Huawei Device Co., Ltd.
  * Licensed under the Apache License, Version 2.0 (the "License");

 * * you may not use this file except in compliance with the License.
  * You may obtain a copy of the License at
  *

 * *     http://www.apache.org/licenses/LICENSE-2.0
  *
  * Unless required by applicable law or agreed to in writing,
 * software
  * distributed under the License is distributed on an "AS IS" BASIS,
  * WITHOUT WARRANTIES OR CONDITIONS
 * OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  *
 * limitations under the License.
  */

#ifndef CARD_EMULATION_ADAPTER_PROXY_H
#define CARD_EMULATION_ADAPTER_PROXY_H

#include "icard_emulation_agent.h"
#include "iremote_proxy.h"

namespace OHOS {
namespace nfc::sdk::cardemulation {
class CardEmulationAgentProxy : public OHOS::IRemoteProxy<ICardEmulationAgent> {
public:
    explicit CardEmulationAgentProxy(const OHOS::sptr<OHOS::IRemoteObject>& ro);
    int GetServices(int userId,
                    const std::string& type,
                    std::vector<std::unique_ptr<CardEmulationServiceInfoLite>>&) override;
    int GetAidsForService(int userId,
                          const OHOS::AppExecFwk::ElementName& serviceName,
                          const std::string& type,
                          std::vector<std::string>& result) override;
    int RegisterRemoteObject(int userId,
                             const OHOS::AppExecFwk::ElementName& serviceName,
                             const OHOS::sptr<OHOS::IRemoteObject>& apduChannel) override;
    int AddAidsForService(int userId,
                          const OHOS::AppExecFwk::ElementName& serviceName,
                          std::vector<std::string> aids,
                          std::string type) override;
    int RemoveAidsForService(int userId, const OHOS::AppExecFwk::ElementName& serviceName, std::string type) override;
    int RegisterOffHostService(int userId,
                               const OHOS::AppExecFwk::ElementName& serviceName,
                               std::string offHostSecureElement) override;
    int UnregisterOffHostService(int userId, const OHOS::AppExecFwk::ElementName& serviceName) override;
    int IsSupportedAidPrefixMode(bool& result) override;
    int GetDescriptionForPreferredService(int userId, std::string type, std::string& result) override;
    int SetPreferredForegroundService(int userId, const OHOS::AppExecFwk::ElementName& serviceName) override;
    int UnsetPreferredForegroundService(int userId) override;
    int GetPrimaryServiceForType(int userId, const std::string& type, OHOS::AppExecFwk::ElementName& result) override;
    int SetPrimaryServiceForType(int userId,
                                 const OHOS::AppExecFwk::ElementName& serviceName,
                                 const std::string& type) override;

protected:
    void WriteHeader(OHOS::MessageParcel& data) const;
    int CheckHeader(OHOS::MessageParcel& reply) const;
};
}  // namespace nfc::sdk::cardemulation
}  // namespace OHOS
#endif  // !CARD_EMULATION_ADAPTER_PROXY_H
