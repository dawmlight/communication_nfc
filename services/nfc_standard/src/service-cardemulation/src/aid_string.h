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

#ifndef AID_STRING_H
#define AID_STRING_H

#include <algorithm>
#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace OHOS::nfc::cardemulation {
constexpr size_t MIN_LEN_AID_BYTES = 5;
constexpr size_t MAX_LEN_AID_BYTES = 16;
constexpr char PREFIX_AID_FLAG = '*';
constexpr char SUBSET_AID_FLAG = '#';
constexpr char const* STR_PREFIX_AID_FLAG = "*";
constexpr char const* STR_SUBSET_AID_FLAG = "#";
constexpr const char* TRIM_AID_FLAGS = "*#";

enum class AidType { INVALID = -1, EXACT, PREFIX, SUBSET };
int AidTypeToInt(AidType t);

class AidString final {
public:
    static bool IsValidAidChar(char c);
    static int VerifyRawString(const std::string& aid);
    AidString(/* args */);
    explicit AidString(std::string str);

    AidString(const AidString& other);
    AidString& operator=(const AidString& other);

    AidString(AidString&& tmp);
    AidString& operator=(AidString&& tmp);
    void swap(AidString& other) noexcept;

    int From(std::string str);
    int From(std::vector<unsigned char> const& aidBytes);
    bool IsPrefix() const;
    bool IsSubset() const;
    bool IsExact() const;

    int ToBytes(std::vector<unsigned char>& aidBytes) const;
    std::string ToString(bool upper = true, bool fillSpace = false) const;
    bool IsValid() const;
    bool ConflictWith(const AidString& other) const;
    std::string Intersection(const AidString& other) const;

    AidType GetType() const;
    bool ExactlyEqualTo(const AidString& other) const;
    bool EqualToWithoutPattern(const AidString& other) const;
    bool operator==(const AidString& other) const;
    bool operator<(const AidString& other) const;

private:
    bool IsPrefixNoCheck() const;
    bool IsSubsetNoCheck() const;
    bool IsExactNoCheck() const;

private:
    std::string aid_;
};

using AidStringSPtr = std::shared_ptr<AidString>;

struct AidStringSPtrCompare {
    virtual bool operator()(const std::shared_ptr<AidString>& a, const std::shared_ptr<AidString>& b);
    virtual ~AidStringSPtrCompare();
};
}  // namespace OHOS::nfc::cardemulation
#endif  // AID_STRING_H
