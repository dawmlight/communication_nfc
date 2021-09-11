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


#ifndef NFC_TAG_ADAPTER_UTILS_H
#define NFC_TAG_ADAPTER_UTILS_H

#include <string>
#include <iostream>

namespace OHOS {
namespace Nfc {
enum {
    TRANSACTION_GET_NFCTAGINTERFACE = 1,
    TRANSACTION_CONNECT = 1,
    TRANSACTION_RECONNECT = 2,
    TRANSACTION_ISNDEF = 4,
    TRANSACTION_IS_PRESENT = 5,
    TRANSACTION_TRANSCEIVE = 6,
    TRANSACTION_NDEFREAD = 7,
    TRANSACTION_NDEFWRITE = 8,
    TRANSACTION_NDEFSETREADONLY = 9,
    TRANSACTION_SET_TIMEOUT = 13,
    TRANSACTION_GET_TIMEOUT = 14,
    TRANSACTION_RESETTIMEOUTS = 15,
    TRANSACTION_CANSETREADONLY = 16,
    TRANSACTION_GET_MAX_TRANSCEIVE_LENGTH = 17,
};
} // namespace Nfc
} // namespace OHOS

#endif // NFC_TAG_ADAPTER_UTILS_H