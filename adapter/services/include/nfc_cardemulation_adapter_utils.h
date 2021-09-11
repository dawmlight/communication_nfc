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


#ifndef NFC_CARDEMULATION_ADAPTER_UTILS_H
#define NFC_CARDEMULATION_ADAPTER_UTILS_H

#include <string>
#include <iostream>

namespace OHOS {
namespace Nfc {
enum {
    /* android.nfc.INfcAdapter */
    TRANS_GET_CARD_EMU_INTERFACE = 2,
    IS_DEFAULT_FOR_TYPE = 38,
    REGISTER_AIDS = 40,
};

enum {
    /* com.nxp.nfc.INxpNfcAdapter */
    SET_LISTENTECHMASK = 16,
    IS_LISTENTRCHMASK = 38,
    SET_CONFIG = 35,
    GET_NFC_INFO = 36,
    GET_INFO_FROM_CONFIG = 51,
};

enum {
    /* android.nfc.INfcCardEmulation */
    IS_DEFAULT_FOR_AID = 2,
    GET_AIDS = 8,
    REMOVE_AIDS = 9,
    SET_FOREGROUND_SERVICE = 11,
    UNSET_FOREGROUND_SERVICE = 12,
};
} // namespace Nfc
} // namespace OHOS

#endif // NFC_CARDEMULATION_ADAPTER_UTILS_H