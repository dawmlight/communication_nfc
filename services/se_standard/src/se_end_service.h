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
#ifndef SE_END_SERVICE_H
#define SE_END_SERVICE_H

#include <memory>
#include <vector>

#include "secure_element_service.h"

namespace osal {
class Context;
class BundleManager;
}  // namespace osal

namespace OHOS {
namespace se {
class Terminal;
class SecureElementService;
using IdTerminalPair = std::pair<std::string, std::shared_ptr<Terminal>>;
using TerminalTable = std::vector<IdTerminalPair>;

class SeEndService final : public std::enable_shared_from_this<SeEndService> {
public:
    ~SeEndService();
    std::weak_ptr<osal::BundleManager> GetBundleManager();
    bool Init(std::shared_ptr<osal::Context> context);

private:
    std::string GetBundleNameFromCallingUid(int uid);
    /**
     * @brief Destruction All Terminals
     */
    void DestructionTerminals();
    /**
     * @brief Establish All Terminals
     */
    void EstablishTerminals();
    /**
     * @brief Add the set of se
     * @param terminalName the se name
     */
    void AddTerminal(const std::string& terminalName);

public:
    static constexpr const auto UICC_TERMINAL{"SIM"};
    static constexpr const auto ESE_TERMINAL{"eSE"};

private:
    sptr<SecureElementService> mSecureElementService_{};
    std::shared_ptr<osal::Context> context_{};
    // The vector will maintain SE Hal Terminals
    TerminalTable mTerminals_{};

    friend class SecureElementService;
    friend class SecureElementReader;
    friend class SecureElementSession;
    friend class SeServiceManager;
};
}  // namespace se
}  // namespace OHOS
#endif  // !SE_END_SERVICE_H
