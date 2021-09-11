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


#ifndef NFC_AGENT_ADAPTER_UTILS_H
#define NFC_AGENT_ADAPTER_UTILS_H

#include <string>
#include <iostream>

namespace OHOS {
namespace Nfc {
enum {
    /* TRANSACTION ID must matched android NFC service */
    /* android.nfc.INfcAdapter */
    TRANSACTION_GET_STATE = 7,
    TRANSACTION_DISABLE = 8,
    TRANSACTION_ENABLE = 9,
    TRANSACTION_SET_FOREGROUND_DISPATCH = 20,
    TRANSACTION_SET_READER_MODE = 26,
};
} // namespace Nfc
} // namespace OHOS

#endif // NFC_AGENT_ADAPTER_UTILS_H