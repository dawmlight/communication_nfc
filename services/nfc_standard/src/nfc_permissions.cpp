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
#include "nfc_permissions.h"

#include "context.h"
namespace OHOS {
namespace nfc {
/*
 * NFC ADMIN permission - only for system apps
 */
const std::string NfcPermissions::ADMIN_PERM = "ohos.Manifest.permission.WRITE_SECURE_SETTINGS";
const std::string NfcPermissions::ADMIN_PERM_ERROR = "WRITE_SECURE_SETTINGS permission required";
/*
 * Regular NFC permission
 */
const std::string NfcPermissions::NFC_PERMISSION = "ohos.Manifest.permission.NFC";
const std::string NfcPermissions::NFC_PERM_ERROR = "NFC permission required";
/*
 * NFC PREFERRED PAYMENT INFO permission
 */
const std::string NfcPermissions::NFC_PREFERRED_PAYMENT_INFO_PERMISSION =
    "ohos.Manifest.permission.NFC_PREFERRED_PAYMENT_INFO";
const std::string NfcPermissions::NFC_PREFERRED_PAYMENT_INFO_PERM_ERROR =
    "NFC_PREFERRED_PAYMENT_INFO permission required";
/**
 * @brief Enforce The Admin Permissions
 *
 * @param context The NFC Service Application Context
 * @return true Within The Admin Permissions
 * @return false Without The Admin Permissions
 */
bool NfcPermissions::EnforceAdminPermissions(std::weak_ptr<osal::Context> context)
{
    return context.lock()->EnforceCallingOrSelfPermission(ADMIN_PERM);
}
/**
 * @brief Enforce The User Permissions
 *
 * @param context The NFC Service Application Context
 * @return true Within The User Permissions
 * @return false Without The User Permissions
 */
bool NfcPermissions::EnforceUserPermissions(std::weak_ptr<osal::Context> context)
{
    return context.lock()->EnforceCallingOrSelfPermission(NFC_PERMISSION);
}
/**
 * @brief Enforce The Preferred Payment Info Permissions
 *
 * @param context The NFC Service Application Context
 * @return true Within The Preferred Payment Info Permissions
 * @return false Without The Preferred Payment Info Permissions
 */
bool NfcPermissions::EnforcePreferredPaymentInfoPermissions(std::weak_ptr<osal::Context> context)
{
    return context.lock()->EnforceCallingOrSelfPermission(NFC_PREFERRED_PAYMENT_INFO_PERMISSION);
}
}  // namespace nfc
}  // namespace OHOS