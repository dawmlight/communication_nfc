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
#ifndef SECURE_ELEMENT_SERVICE_PROXY_H
#define SECURE_ELEMENT_SERVICE_PROXY_H

#include <memory>

#include "errors.h"
#include "iremote_proxy.h"
#include "isecure_element_service.h"

namespace OHOS {
namespace se {
namespace omapi {
class SecureElementServiceProxy final : public OHOS::IRemoteProxy<ISecureElementService> {
public:
    explicit SecureElementServiceProxy(const OHOS::sptr<OHOS::IRemoteObject>& remote)
        : OHOS::IRemoteProxy<ISecureElementService>(remote)
    {
    }
    ~SecureElementServiceProxy() {}

    std::vector<std::string> GetReaders() override;
    sptr<ISecureElementReader> GetReader(const std::string& readerName, const sptr<IRemoteObject>& listener) override;
    std::vector<bool> IsNfcEventAllowed(const std::string& readerName,
                                        const std::string& aid,
                                        std::vector<std::string> packageNames) override;

private:
    static constexpr int COMMAND_SERVICE_START_ID = 100;
    static constexpr int COMMAND_SERVICE_GET_READERS = COMMAND_SERVICE_START_ID + 0;
    static constexpr int COMMAND_SERVICE_GET_READER = COMMAND_SERVICE_START_ID + 1;
    static constexpr int COMMAND_SERVICE_IS_NFCEVENT_ALLOWED = COMMAND_SERVICE_START_ID + 2;
};
}  // namespace omapi
}  // namespace se
}  // namespace OHOS
#endif  // !SECURE_ELEMENT_SERVICE_PROXY_H