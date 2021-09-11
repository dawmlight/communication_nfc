/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
 * Copyright (C) 2006 The Android Open Source Project
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


#ifndef NFC_SECUREELEMENT_ADAPTER_UTILS_H
#define NFC_SECUREELEMENT_ADAPTER_UTILS_H

#include <string>
#include <iostream>

namespace OHOS {
namespace Nfc {
enum {
    /* android.se.omapi.ISecureElementService */
    TRANSACTION_GET_READERS = 1,
    TRANSACTION_GET_READER = 2,
};

enum {
    /* android.se.omapi.ISecureElementReader */
    TRANSACTION_IS_SE_PRESENT = 1,
    TRANSACTION_OPEN_SESSION = 2,
    TRANSACTION_CLOSE_SESSIONS = 3,
};

enum {
    /* android.se.omapi.ISecureElementSession */
    TRANSACTION_GET_ATR = 1,
    TRANSACTION_CLOSE_CHANNELS = 3,
    TRANSACTION_OPEN_BASICHANNEL = 5,
    TRANSACTION_OPEN_LOGICALCHANNEL = 6,
};

enum {
    /* android.se.omapi.ISecureElementChannel */
    TRANSACTION_CLOSE = 1,
    TRANSACTION_IS_CLOSED = 2,
    TRANSACTION_SELECT_RESPONSE = 4,
    TRANSACTION_TRANSMIT = 5,
};

enum {
    /* android.nfc.INfcAdapter */
    TRANSACTION_BIND_SE_SERVICE = 39,
};
} // namespace Nfc
} // namespace OHOS

#endif // NFC_SECUREELEMENT_ADAPTER_UTILS_H