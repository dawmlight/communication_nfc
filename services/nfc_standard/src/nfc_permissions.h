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
#ifndef NFC_PERMISSIONS_H
#define NFC_PERMISSIONS_H

#include <memory>
#include <string>

namespace osal {
class Context;
}
namespace OHOS {
namespace nfc {
class NfcPermissions {
public:
    static bool EnforceAdminPermissions(std::weak_ptr<osal::Context> context);
    static bool EnforceUserPermissions(std::weak_ptr<osal::Context> context);
    static bool EnforcePreferredPaymentInfoPermissions(std::weak_ptr<osal::Context> context);

private:
    /*
     * NFC ADMIN permission - only for system apps
     */
    static const std::string ADMIN_PERM;
    static const std::string ADMIN_PERM_ERROR;
    /*
     * Regular NFC permission
     */
    static const std::string NFC_PERMISSION;
    static const std::string NFC_PERM_ERROR;
    /*
     * NFC PREFERRED PAYMENT INFO permission
     */
    static const std::string NFC_PREFERRED_PAYMENT_INFO_PERMISSION;
    static const std::string NFC_PREFERRED_PAYMENT_INFO_PERM_ERROR;
};
}  // namespace nfc
}  // namespace OHOS
#endif  //! NFC_PERMISSIONS_H
