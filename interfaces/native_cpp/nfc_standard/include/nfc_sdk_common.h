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
#ifndef NFC_SDK_COMMON_H
#define NFC_SDK_COMMON_H

#include <sstream>

namespace OHOS {
namespace nfc {
namespace sdk {
enum NfcErrorCode : const int {
    NFC_SUCCESS = 0,
    // SDK ERROR CODE
    NFC_SDK_ERROR_NOT_INITIALIZED = 0x00000100,
    NFC_SDK_ERROR_INVALID_PARAM,
    NFC_SDK_ERROR_TAG_NOT_CONNECT,
    NFC_SDK_ERROR_TAG_INVALID,
    NFC_SDK_ERROR_NOT_NDEF_TAG,
    NFC_SDK_ERROR_NFC_STATE_INVALID,
    NFC_SDK_ERROR_DEVICE_NOT_SUPPORT_NFC,
    NFC_SDK_ERROR_DISABLE_MAKE_READONLY,
    NFC_SDK_ERROR_UNKOWN,
    NFC_SDK_ERROR_PERMISSION,

    NFC_SDK_ERROR_CARDEMULATION_AGENT_NOT_INITIALIZED,
    NFC_SDK_ERROR_CARDEMULATION_HANDLER_IS_NULL,

    // SERVICE ERROR CODE
    NFC_SER_ERROR_NOT_INITIALIZED = 0x00000200,
    NFC_SER_ERROR_DISCONNECT,
    NFC_SER_ERROR_IO,
    NFC_SER_ERROR_INVALID_PARAM
};

// Polling technology masks
enum NfcPollTech { NFC_POLL_A = 0x01, NFC_POLL_B = 0x02, NFC_POLL_F = 0x04, NFC_POLL_V = 0x08 };

enum NfcState { STATE_OFF = 1, STATE_TURNING_ON = 2, STATE_ON = 3, STATE_TURNING_OFF = 4 };

enum NfcPreferredPaymentState { STATE_LOADED = 1, STATE_CHANGED = 2, STATE_UPDATED = 3 };

static const int SHIFT_SIZE = 8;
static const int SHIFT_TIME = 4;

class NfcSdkCommon final {
public:
    static bool IsLittleEndian();
    static std::string IntToString(int num, bool bLittleEndian = true);
    static int StringToInt(std::string s, bool bLittleEndian = true);
};
}  // namespace sdk
}  // namespace nfc
}  // namespace OHOS
#endif  // !NFC_SDK_COMMON_H
