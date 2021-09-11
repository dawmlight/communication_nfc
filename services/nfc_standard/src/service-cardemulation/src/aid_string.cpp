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

#include "aid_string.h"

#include <assert.h>

#include <algorithm>
#include <cctype>
#include <functional>
#include <sstream>

#include "card_emulation_error.h"
#include "card_emulation_util.h"
#include "loghelper.h"
#ifdef USE_HILOG
DEFINE_NFC_LOG_LABEL("AidSet");
#endif

namespace OHOS::nfc::cardemulation {
AidString::AidString(/* args */) : aid_("")
{
    aid_.reserve(2 * MAX_LEN_AID_BYTES);
}

AidString::AidString(std::string str) : aid_("")
{
    From(std::move(str));
}

AidString::AidString(const AidString& other) : aid_(other.aid_)
{
}

AidString& AidString::operator=(const AidString& other)
{
    if (this != &other) {
        aid_ = other.aid_;
    }
    return *this;
}

AidString::AidString(AidString&& tmp) : aid_("")
{
    this->aid_ = std::move(tmp.aid_);
}

AidString& AidString::operator=(AidString&& tmp)
{
    if (this != &tmp) {
        this->aid_ = std::move(tmp.aid_);
    }
    return *this;
}

void AidString::swap(AidString& other) noexcept
{
    using std::swap;
    swap(aid_, other.aid_);
}

bool AidString::IsValidAidChar(char ch)
{
    return std::isxdigit(static_cast<unsigned char>(ch)) || ch == PREFIX_AID_FLAG || ch == SUBSET_AID_FLAG;
}

int AidString::VerifyRawString(const std::string& str)
{
    if (str.size() < 2 * MIN_LEN_AID_BYTES || str.size() > 2 * MAX_LEN_AID_BYTES) {
        return ERR_AID_ILLIGLE_LENGTH;
    }
    bool foundAidFlag = false;
    size_t cnt = 0;
    for (auto&& ch : str) {
        if (foundAidFlag) {
            return ERR_AID_INVALID;
        }

        if (std::isxdigit(static_cast<unsigned char>(ch))) {
            ++cnt;
        } else if (ch == PREFIX_AID_FLAG || ch == SUBSET_AID_FLAG) {
            foundAidFlag = true;
        } else {
            return ERR_AID_INVALID;
        }
    }
    if (cnt % 2 != 0) {
        return ERR_AID_INVALID;
    }
    return ERR_OK;
}

static int FromRawString(std::string& str)
{
    int rv = AidString::VerifyRawString(str);
    if (rv != ERR_OK) {
        return rv;
    }
    std::transform(str.begin(), str.end(), str.begin(), toupper);
    return ERR_OK;
}
int AidString::From(std::string str)
{
    aid_.clear();
    auto rv = FromRawString(str);
    if (rv == ERR_OK) {
        aid_.swap(str);
    }

    return rv;
}
int AidString::From(std::vector<unsigned char> const& aidBytes)
{
    aid_.clear();
    if (aidBytes.size() < MIN_LEN_AID_BYTES || aidBytes.size() > MAX_LEN_AID_BYTES) {
        return ERR_AID_ILLIGLE_LENGTH;
    }
    aid_ = BytesToHexStr(aidBytes);
    return ERR_OK;
}

bool AidString::IsPrefix() const
{
    if (IsValid()) {
        return IsPrefixNoCheck();
    }

    return false;
}

bool AidString::IsSubset() const
{
    if (IsValid()) {
        return IsSubsetNoCheck();
    }

    return false;
}

bool AidString::IsExact() const
{
    if (IsValid()) {
        return IsExactNoCheck();
    }
    return false;
}

int AidString::ToBytes(std::vector<unsigned char>& aidBytes) const
{
    if (!IsValid()) {
        return ERR_AID_INVALID;
    }

    std::string aid = StringTrim(aid_.c_str(), TRIM_AID_FLAGS);
    HexStrToBytes(aid, aidBytes);
    return ERR_OK;
}

std::string AidString::ToString(bool upper, bool fillSpace) const
{
    std::string str(aid_);
    std::transform(str.begin(), str.end(), str.begin(), upper ? toupper : tolower);
    if (fillSpace) {
        std::stringstream ss;
        for (size_t i = 0; i < str.size(); i++) {
            if (i > 0 && (i % 2 == 0)) {
                ss << " ";
            }
            ss << str[i];
        }
        return ss.str();
    }
    return str;
}

bool AidString::IsValid() const
{
    std::string aid = aid_;
    return FromRawString(aid) == ERR_OK;
}

bool AidString::ConflictWith(const AidString& other) const
{
    if (this == &other) {
        return true;
    }
    if (!IsValid() || !other.IsValid()) {
        return false;
    }
    auto exactStr = [](const std::string& s) { return s.substr(0, s.size() - 1); };

    if (IsExactNoCheck()) {
        switch (other.GetType()) {
            case AidType::EXACT: {
                return aid_.compare(other.aid_) == 0;
            }
            case AidType::PREFIX: {
                return StringStartsWith(aid_, exactStr(other.aid_));
            }
            case AidType::SUBSET: {
                return StringStartsWith(other.aid_, aid_);
            }
            default:
                return false;
        }
    } else if (IsPrefixNoCheck()) {
        // aid_.IsExactNoCheck() == false
        switch (other.GetType()) {
            case AidType::EXACT: {
                return StringStartsWith(other.aid_, exactStr(aid_));
            }
            case AidType::PREFIX: {
                const std::string& a = exactStr(aid_);
                const std::string& b = exactStr(other.aid_);
                return StringStartsWith(a, b) || StringStartsWith(b, a);
            }
            case AidType::SUBSET: {
                return StringStartsWith(exactStr(other.aid_), exactStr(aid_));
            }
            default:
                return false;
        }
    } else if (IsSubsetNoCheck()) {
        switch (other.GetType()) {
            case AidType::EXACT: {
                return StringStartsWith(aid_, other.aid_);
            }
            case AidType::PREFIX: {
                return StringStartsWith(exactStr(aid_), exactStr(other.aid_));
            }
            case AidType::SUBSET: {
                const std::string& a = exactStr(aid_);
                const std::string& b = exactStr(other.aid_);
                return StringStartsWith(a, b) || StringStartsWith(b, a);
            }
            default:
                return false;
        }
    } else {
        assert(false);
        return false;
    }
}

std::string AidString::Intersection(const AidString& other) const
{
    if (this == &other) {
        return ToString(true);
    }
    if (!IsValid() || !other.IsValid()) {
        //
        return std::string();
    }
    auto exactStr = [](const std::string& s) { return s.substr(0, s.size() - 1); };
    if (IsExactNoCheck()) {
        switch (other.GetType()) {
            case AidType::EXACT: {
                return (aid_.compare(other.aid_) == 0) ? ToString(true) : std::string();
            }
            case AidType::PREFIX: {
                return StringStartsWith(aid_, exactStr(other.aid_)) ? other.ToString(true) : std::string();
            }
            case AidType::SUBSET: {
                return StringStartsWith(other.aid_, aid_) ? other.ToString(true) : std::string();
            }
            default:
                return std::string();
        }
    } else if (IsPrefixNoCheck()) {
        switch (other.GetType()) {
            case AidType::EXACT: {
                return StringStartsWith(other.aid_, exactStr(aid_)) ? ToString(true) : std::string();
            }
            case AidType::PREFIX: {
                const std::string& a = exactStr(aid_);
                const std::string& b = exactStr(other.aid_);
                if (StringStartsWith(a, b)) {
                    return other.ToString(true);
                } else if (StringStartsWith(b, a)) {
                    return ToString(true);
                } else {
                    return std::string();
                };
            }
            case AidType::SUBSET: {
                return StringStartsWith(exactStr(other.aid_), exactStr(aid_)) ? ToString(true) : std::string();
            }
            default:
                return std::string();
        }
    } else if (IsSubsetNoCheck()) {
        switch (other.GetType()) {
            case AidType::EXACT: {
                return StringStartsWith(aid_, other.aid_) ? ToString() : std::string();
            }
            case AidType::PREFIX: {
                return StringStartsWith(exactStr(aid_), exactStr(other.aid_)) ? other.ToString() : std::string();
            }
            case AidType::SUBSET: {
                const std::string& a = exactStr(aid_);
                const std::string& b = exactStr(other.aid_);
                if (StringStartsWith(a, b)) {
                    return ToString(true);
                } else if (StringStartsWith(b, a)) {
                    return other.ToString();

                } else {
                    return std::string();
                }
            }
            default:
                return std::string();
        }
    } else {
        assert(false);
        return std::string();
    }
}

AidType AidString::GetType() const
{
    if (!IsValid()) {
        return AidType::INVALID;
    }
    if (IsExactNoCheck()) {
        return AidType::EXACT;
    }
    if (IsPrefixNoCheck()) {
        return AidType::PREFIX;
    }
    if (IsSubsetNoCheck()) {
        return AidType::SUBSET;
    }
    return AidType::INVALID;
}

bool AidString::ExactlyEqualTo(const AidString& other) const
{
    if (this == &other) {
        return true;
    }
    DebugLog("ExactlyEqualTo: %s(this) == %s(other)", aid_.c_str(), other.aid_.c_str());
    return (aid_.compare(other.aid_) == 0);
}

bool AidString::EqualToWithoutPattern(const AidString& other) const
{
    if (this == &other) {
        return true;
    }
    if (!IsValid() || !other.IsValid()) {
        return false;
    }
    auto getExactlyAid = [](const AidString& aidstr) {
        if (!aidstr.IsExactNoCheck()) {
            return aidstr.aid_.substr(0u, aidstr.aid_.size() - 1);
        }
        return aidstr.aid_;
    };
    std::string exactlyThis = getExactlyAid(*this);
    std::string exactlyOther = getExactlyAid(other);
    DebugLog("EqualToWithoutPattern: %s(this) == %s(other)", exactlyThis.c_str(), exactlyOther.c_str());
    return (exactlyThis.compare(exactlyOther) == 0);
}

bool AidString::operator==(const AidString& other) const
{
    return !(this->operator<(other)) && !(other.operator<(*this));
}

bool AidString::operator<(const AidString& other) const
{
    if (this == &other) {
        return false;
    }
    if (ConflictWith(other)) {
        return false;
    }
    return aid_ < other.aid_;
}

bool AidString::IsPrefixNoCheck() const
{
    return StringEndsWith(aid_, STR_PREFIX_AID_FLAG);
}

bool AidString::IsSubsetNoCheck() const
{
    return StringEndsWith(aid_, STR_SUBSET_AID_FLAG);
}

bool AidString::IsExactNoCheck() const
{
    return !IsPrefixNoCheck() && !IsSubsetNoCheck();
}

int AidTypeToInt(AidType t)
{
    // mapping nci config
    switch (t) {
        case nfc::cardemulation::AidType::EXACT:
            return 0;
        case nfc::cardemulation::AidType::PREFIX:
            return 0x10;
        case nfc::cardemulation::AidType::SUBSET:
            return 0x20;
        default:
            return 0;
    }
}

bool AidStringSPtrCompare::operator()(const std::shared_ptr<AidString>& a, const std::shared_ptr<AidString>& b)
{
    return a->operator<(*b);
}

AidStringSPtrCompare::~AidStringSPtrCompare()
{
}
}  // namespace OHOS::nfc::cardemulation
