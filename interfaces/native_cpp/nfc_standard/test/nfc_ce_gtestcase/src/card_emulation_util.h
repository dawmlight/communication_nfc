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

#ifndef CARD_EMULATION_UTIL_H
#define CARD_EMULATION_UTIL_H

#include <string>
#include <vector>

namespace OHOS::nfc::cardemulation {

std::string StringToLower(const std::string& str) noexcept;
std::string StringToUpper(const std::string& str) noexcept;

bool StringStartsWith(const std::string& src, const std::string& subStr, bool ignoreCase = false) noexcept;
bool StringEndsWith(const std::string& src, const std::string& subStr, bool ignoreCase = false) noexcept;
std::string& StringTrimLeft(std::string& src, const std::string& trim = " \t\r\n\f\v");
std::string& StringTrimRight(std::string& src, const std::string& trim = " \t\r\n\f\v");
std::string& StringTrim(std::string& src, const std::string& trim = " \t\r\n\f\v");
std::string StringTrimLeft(const std::string& src, const std::string& trim = " \t\r\n\f\v");
std::string StringTrimRight(const std::string& src, const std::string& trim = " \t\r\n\f\v");
std::string StringTrim(const std::string& src, const std::string& trim = " \t\r\n\f\v");

std::vector<std::string> StringSplit(const std::string& src, const std::string& reg);
bool StringEqual(const std::string& s1, const std::string& s2, bool ignoreCase = false);
std::string StringVectorToString(const std::vector<std::string>& v) noexcept;

std::string BytesToHexStr(std::vector<unsigned char> const& bytes, bool upper = true) noexcept;

std::string BytesToHexStr(const unsigned char* start, size_t len, bool upper = true) noexcept;
unsigned char HexCharToByte(char ch) noexcept;
void HexStrToBytes(const std::string& src, std::vector<unsigned char>& bytes) noexcept;
std::vector<unsigned char> HexStrToBytes(const std::string& src) noexcept;

std::string FindSelectAid(const std::vector<unsigned char>& data) noexcept;
std::string FindSelectAid(const unsigned char* data, size_t len) noexcept;
std::vector<unsigned char> StringToBytes(const std::string& str) noexcept;

}  // namespace OHOS::nfc::cardemulation
#endif  // CARD_EMULATION_UTIL_H
