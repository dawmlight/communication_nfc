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
#include "nfc_sdk_common.h"

namespace OHOS {
namespace nfc {
namespace sdk {
bool NfcSdkCommon::IsLittleEndian()
{
    union CheckData {
        int x;
        char y;
    };

    union CheckData data;
    data.x = 0x12345678;
    if (data.y == 0x78) {
        return true;
    }
    return false;
}

std::string NfcSdkCommon::IntToString(int num, bool bLittleEndian)
{
    std::stringstream ss;
    if (bLittleEndian) {
        for (size_t i = 0; i < SHIFT_TIME; i++) {
            ss << char((num >> (i * SHIFT_SIZE)) & 0xFF);
        }
    } else {
        for (size_t i = SHIFT_TIME; i > 0; i--) {
            ss << char((num >> (i * SHIFT_SIZE - SHIFT_SIZE)) & 0xFF);
        }
    }

    return ss.str();
}

int NfcSdkCommon::StringToInt(std::string s, bool bLittleEndian)
{
    int value = 0;
    if (bLittleEndian) {
        for (size_t i = SHIFT_TIME; i > 0; i--) {
            value += (int)(s.at(SHIFT_TIME - i)) << (i * SHIFT_SIZE - SHIFT_SIZE);
        }
    } else {
        for (size_t i = 0; i < SHIFT_TIME; i++) {
            value += (int)(s.at(i)) << (i * SHIFT_SIZE);
        }
    }
    return value;
}
}  // namespace sdk
}  // namespace nfc
}  // namespace OHOS