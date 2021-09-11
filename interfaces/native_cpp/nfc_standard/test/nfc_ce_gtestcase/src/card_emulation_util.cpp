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

#include "card_emulation_util.h"

#include <algorithm>
#include <cctype>
#include <iomanip>
#include <iostream>
#include <regex>
#include <sstream>
namespace OHOS::nfc::cardemulation {
std::string StringToLower(const std::string& str) noexcept
{
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}
std::string StringToUpper(const std::string& str) noexcept
{
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}
bool StringStartsWith(const std::string& src, const std::string& subStr, bool ignoreCase) noexcept
{
    if ((subStr.empty()) || (subStr.size() > src.size())) {
        return false;
    }
    if (ignoreCase) {
        std::string lowerSrc = StringToLower(src);
        std::string lowerSub = StringToLower(subStr);
        return lowerSrc.find(lowerSub) == 0u;
    }
    return src.find(subStr) == 0u;
}

bool StringEndsWith(const std::string& src, const std::string& subStr, bool ignoreCase) noexcept
{
    if ((subStr.empty()) || (subStr.size() > src.size())) {
        return false;
    }
    if (ignoreCase) {
        std::string lowerSrc = StringToLower(src);
        std::string lowerSub = StringToLower(subStr);
        return lowerSrc.rfind(lowerSub) == (lowerSrc.size() - lowerSub.size());
    }
    return src.rfind(subStr) == (src.size() - subStr.size());
}

std::string& StringTrimLeft(std::string& src, const std::string& trim)
{
    if (!src.empty() && !trim.empty()) {
        auto pos = src.find_first_not_of(trim);
        if (pos != std::string::npos) {
            src.erase(0, pos);
        }
    }
    return src;
}

std::string& StringTrimRight(std::string& src, const std::string& trim)
{
    if (!src.empty() && !trim.empty()) {
        auto pos = src.find_last_not_of(trim);
        if (pos != std::string::npos) {
            src.erase(pos + 1);
        }
    }
    return src;
}

std::string& StringTrim(std::string& src, const std::string& trim)
{
    if (!src.empty()) {
        return StringTrimLeft(StringTrimRight(src, trim), trim);
    }
    return src;
}

std::string StringTrimLeft(const std::string& src, const std::string& trim)
{
    std::string s = src;
    return StringTrimLeft(s, trim);
}

std::string StringTrimRight(const std::string& src, const std::string& trim)
{
    std::string s = src;
    return StringTrimRight(s, trim);
}

std::string StringTrim(const std::string& src, const std::string& trim)
{
    std::string s = src;
    return StringTrim(s, trim);
}
std::vector<std::string> StringSplit(const std::string& src, const std::string& reg)
{
    std::regex re{reg};

    return std::vector<std::string>{std::sregex_token_iterator(src.begin(), src.end(), re, -1),
                                    std::sregex_token_iterator()};
}
bool StringEqual(const std::string& s1, const std::string& s2, bool ignoreCase)
{
    if (ignoreCase) {
        char const* p1 = s1.c_str();
        char const* p2 = s2.c_str();
        while (*p1 && *p2) {
            if (toupper(*p1++) != toupper(*p2++)) {
                return false;
            }
        }
        if (*p1 != '\0' || *p2 != '\0') {
            return false;
        }
        return true;
    }
    return s1 == s2;
}

std::string StringVectorToString(const std::vector<std::string>& v) noexcept
{
    std::stringstream ss;
    ss << "[";
    std::for_each(v.begin(), v.end(), [&ss](auto r) { ss << r << " "; });
    ss << "]";
    return ss.str();
}
#if 1
std::string BytesToHexStr(const unsigned char* start, size_t len, bool upper) noexcept
{
    std::vector<char> rv((len + 1) * 2, '\0');
    for (size_t i = 0; i < len; i++) {
        sprintf(&rv[2 * i], upper ? "%02X" : "%02x", start[i]);
    }

    return std::string(&rv[0]);
}
#else
static inline int tolower_noop(int c) noexcept
{
    return c;
}
std::string BytesToHexStr(const unsigned char* start, size_t len, bool upper) noexcept
{
    std::string rv(len * 2, '0');
    constexpr char hexchars[] = {
        '0',
        '1',
        '2',
        '3',
        '4',
        '5',
        '6',
        '7',
        '8',
        '9',
        'a',
        'b',
        'c',
        'd',
        'e',
        'f',
    };

    auto func = upper ? toupper : tolower_noop;
    for (size_t i = 0; i < len; ++i) {
        rv[2 * i] = static_cast<char>(func(hexchars[(start[i] & 0xF0) >> 4]));
        rv[2 * i + 1] = static_cast<char>(func(hexchars[(start[i] & 0x0F)]));
    }

    return rv;
}
#endif

std::string BytesToHexStr(std::vector<unsigned char> const& bytes, bool upper) noexcept
{
    if (bytes.empty()) {
        return std::string();
    }
    return BytesToHexStr(&bytes[0], bytes.size(), upper);
}

unsigned char HexCharToByte(char ch) noexcept
{
    if (ch >= '0' && ch <= '9') {
        return static_cast<unsigned char>(ch - '0');
    } else if (ch >= 'a' && ch <= 'f') {
        return static_cast<unsigned char>(ch - 'a' + 0x0a);
    } else if (ch >= 'A' && ch <= 'F') {
        return static_cast<unsigned char>(ch - 'A' + 0x0a);
    } else {
        return 0;
    }
}

void HexStrToBytes(const std::string& src, std::vector<unsigned char>& bytes) noexcept
{
    bytes.clear();

    bool padding = (src.size() % 2 != 0);
    bytes.reserve(padding ? (src.size() + 1) / 2 : src.size() / 2);

    char hi = '0';
    char lo = '0';
    auto i = src.cbegin();
    while (i != src.cend()) {
        if (i == src.cbegin() && padding) {
            hi = '0';
            lo = *i;
            ++i;
        } else {
            hi = *i;
            ++i;
            lo = *i;
            ++i;
        }
        bytes.emplace_back((HexCharToByte(hi) << 4) | (HexCharToByte(lo)));
    }
}

std::vector<unsigned char> HexStrToBytes(const std::string& src) noexcept
{
    std::vector<unsigned char> rv;

    HexStrToBytes(src, rv);
    return rv;
}

std::string FindSelectAid(const std::vector<unsigned char>& data) noexcept
{
    //
    if (data.empty()) {
        return std::string();
    }
    return FindSelectAid(&data[0], data.size());
}

std::string FindSelectAid(const unsigned char* data, size_t len) noexcept
{
    constexpr size_t LEN_SELECT_AUDU_HEADER = 5UL;
    constexpr size_t LEN_MINIMUM_AID = 5UL;
    constexpr size_t LEN_MAXIMUM_AID = 16UL;
    constexpr unsigned char SELECT_AID_CLA = 0x00;
    constexpr unsigned char SELECT_AID_INS = 0xA4;
    constexpr unsigned char SELECT_AID_P1 = 0x04;
    constexpr unsigned char SELECT_AID_P2_MUST_BE = 0x00;

    std::string rv;
    if (len < LEN_SELECT_AUDU_HEADER + LEN_MINIMUM_AID || data == nullptr) {
        return rv;
    }
    if (data[0] == SELECT_AID_CLA && data[1] == SELECT_AID_INS && data[2] == SELECT_AID_P1) {
        if (data[3] != SELECT_AID_P2_MUST_BE) {
            // Log.d(TAG, "Selecting next, last or previous AID occurrence is not supported");
        }
        size_t aidLen = data[4];
        if (aidLen > LEN_MAXIMUM_AID || len < LEN_SELECT_AUDU_HEADER + aidLen) {
            return rv;
        }

        rv = BytesToHexStr(&data[LEN_SELECT_AUDU_HEADER], aidLen);
    }
    //
    return rv;
}

std::vector<unsigned char> StringToBytes(const std::string& str) noexcept
{
    return std::vector<unsigned char>(str.begin(), str.end());
}
}  // namespace OHOS::nfc::cardemulation
