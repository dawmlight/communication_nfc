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

#include "nfc_adapter_utils.h"

#include <iostream>

#include "string_ex.h"

namespace OHOS {
namespace Nfc {
std::u16string StringReplace(const std::string &str)
{
    const std::string search = "ohos";
    const std::string replace = "android";
    std::string replaceStr = str;

    for (size_t pos = 0; pos <= str.length(); pos += replace.length()) {
        // Find the location of the key string to replace
        pos = replaceStr.find(search, pos);
        if (pos == std::string::npos) {
            break;
        }

        // Erasing and inserting the replace keyword
        replaceStr.erase(pos, search.length());
        replaceStr.insert(pos, replace);
    }
    return Str8ToStr16(replaceStr);
}

std::string StringReplaceReturnEight(const std::string &str)
{
    const std::string search = "ohos";
    const std::string replace = "android";
    std::string replaceStr = str;

    for (size_t pos = 0; pos <= str.length(); pos += replace.length()) {
        // Find the location of the key string to replace
        pos = replaceStr.find(search, pos);
        if (pos == std::string::npos) {
            break;
        }

        // Erasing and inserting the replace keyword
        replaceStr.erase(pos, search.length());
        replaceStr.insert(pos, replace);
    }
    return replaceStr;
}
} // namespace Nfc
} // namespace OHOS