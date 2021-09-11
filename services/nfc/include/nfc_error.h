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

#ifndef OHOS_NFC_ERROR_H
#define OHOS_NFC_ERROR_H

#include "hilog/log.h"

namespace OHOS {
namespace Nfc {
/* NO ERROR */
const int32_t ERR_NONE = 0;

/* remote service is disconnected */
const int32_t ERR_REMOTE_SERVICE_DISCONNECTED = -1;

/* send AIDL request, but the return is error number */
const int32_t ERR_REMOTE_TRANSACT_FAILED = -2;

/* the AIDL function return is error */
const int32_t ERR_REMOTE_EXCEPTION_RETURN = -3;

/* write value into the message parcel is error */
const int32_t ERR_WRITE_PARCEL = -4;

/* no permission to call the function of SA */
const int32_t ERR_NO_PERMISSION = -6;

/* no permission to call the function of SA */
const int32_t ERR_REMOTE_RETURN_ERR = -7;
}
}
#endif  // #ifndef OHOS_NFC_ERROR_H
