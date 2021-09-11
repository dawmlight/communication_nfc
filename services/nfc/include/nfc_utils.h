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

#ifndef OHOS_NFC_UTILS_H
#define OHOS_NFC_UTILS_H

#include "permission/permission_errors.h"
#include "permission/permission_kit.h"
#include "ipc_skeleton.h"
#include "nfc_adapter_utils.h"

namespace OHOS {
namespace Nfc {
const int32_t UID_UNCHECK = -1; // no ipc calling.
const int32_t UID_ROOT = 0; // root uid
const int32_t UID_SYSTEM = 1000; // system uid

#ifndef CHECK_PERMISSON
#define CHECK_PERMISSON(permName, callingUid)                                            \
    if (callingUid != UID_UNCHECK && callingUid != UID_ROOT && callingUid != UID_SYSTEM  \
        && OHOS::Security::Permission::PermissionKit::GRANTED !=                         \
        OHOS::Security::Permission::PermissionKit::CheckCallingPermission(permName)) {   \
        HILOGW("Security error, No Permission!");                                        \
        return Nfc::ERR_NO_PERMISSION;                                                   \
    }
#endif

#ifndef CHECK_REMOTE_DESC
#define CHECK_REMOTE_DESC(remoteDesc, validDesc)                      \
    if (remoteDesc.empty() || remoteDesc.compare(validDesc) != 0) {   \
        return Nfc::ERR_REMOTE_TRANSACT_FAILED;                       \
    }
#endif

#ifndef WRITE_INTF_TOKEN
#define WRITE_INTF_TOKEN(nfcInterface, data)                  \
    do {                                                      \
        if (!data.WriteInt32(1)) {                            \
            HILOGE("WRITE_INTF_TOKEN strictPolicy failed.");  \
            return Nfc::ERR_WRITE_PARCEL;                     \
        }                                                     \
        if (!data.WriteInt32(1)) {                            \
            HILOGE("WRITE_INTF_TOKEN workSource failed.");    \
            return Nfc::ERR_WRITE_PARCEL;                     \
        }                                                     \
        if (!data.WriteString16(nfcInterface)) {              \
            HILOGE("WRITE_INTF_TOKEN Interface failed.");     \
            return Nfc::ERR_WRITE_PARCEL;                     \
        }                                                     \
    } while (0)
#endif

#ifndef WRITE_INTF_TOKEN_NULL
#define WRITE_INTF_TOKEN_NULL(nfcInterface, data)             \
    do {                                                      \
        if (!data.WriteInt32(1)) {                            \
            HILOGE("WRITE_INTF_TOKEN strictPolicy failed.");  \
            return nullptr;                                   \
        }                                                     \
        if (!data.WriteInt32(1)) {                            \
            HILOGE("WRITE_INTF_TOKEN workSource failed.");    \
            return nullptr;                                   \
        }                                                     \
        if (!data.WriteString16(nfcInterface)) {              \
            HILOGE("WRITE_INTF_TOKEN Interface failed.");     \
            return nullptr;                                   \
        }                                                     \
    } while (0)
#endif
} // namespace Nfc
} // namespace OHOS
#endif  // #ifndef OHOS_NFC_UTILS_H
