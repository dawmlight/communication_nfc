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

#ifndef CARD_EMULATION_DEF_H
#define CARD_EMULATION_DEF_H

#include <algorithm>
#include <string>
namespace OHOS::nfc::cardemulation {
// NFC execution environment
const std::string NFC_EE_HOST = "host";
const std::string NFC_EE_ESE = "eSE";
const std::string NFC_EE_UICC = "SIM";

const std::string NFC_EE_ESE_1 = NFC_EE_ESE + "1";
const std::string NFC_EE_UICC_1 = NFC_EE_UICC + "1";

// card emulation service type
const std::string CARDEMULATION_SERVICE_TYPE_SECURE = "secure";
const std::string CARDEMULATION_SERVICE_TYPE_NORMAL = "normal";

// mask of aid routing pattern 
constexpr int AID_ROUTING_MODE_MASK_PREFIX = 0x01;
constexpr int AID_ROUTING_MODE_MASK_SUBSET = 0x02;

// aid length
constexpr size_t AID_MIN_LENGTH_IN_BYTES = 5;
constexpr size_t AID_MAX_LENGTH_IN_BYTES = 16;

// card emulation technology
constexpr int TECH_HCE_APDU = 0x01;
constexpr int TECH_HCE_NFCF = 0x04;

/**
 * brief: is it secure type
 * parameter: 
 *   type -- service type
 * return: true -- secure type , false -- not secure type
 */
inline bool IsSecureType(const std::string& type)
{
    std::string t = type;
    std::transform(t.begin(), t.end(), t.begin(), ::tolower);

    return t == CARDEMULATION_SERVICE_TYPE_SECURE;
}
/**
 * brief: is it normal type
 * parameter: 
 *   type -- service type
 * return: true -- normal type , false -- not normal type
 */
inline bool IsNormalType(const std::string& type)
{
    std::string t = type;
    std::transform(t.begin(), t.end(), t.begin(), ::tolower);

    return t == CARDEMULATION_SERVICE_TYPE_NORMAL;
}
/**
 * brief: is it valid
 * parameter: 
 *   type -- service type
 * return: true -- valid , false -- invalid
 */
inline bool IsValidType(const std::string& type)
{
    return IsNormalType(type) || IsSecureType(type);
}
/**
 * brief: execution environment is on host?
 * parameter: 
 *   location -- execution environment
 * return: true -- on host , false -- not on host
 */
inline bool IsOnHost(const std::string& location)
{
    return location.compare(NFC_EE_HOST) == 0;
}
/**
 * brief: is it prefix mode
 * parameter: 
 *   mode -- aid matching pattern
 * return: true -- prefix mode , false -- not prefix mode
 */
inline bool IsAidPrefixMode(int mode)
{
    return (mode & AID_ROUTING_MODE_MASK_PREFIX) == AID_ROUTING_MODE_MASK_PREFIX;
}
/**
 * brief: is it subset mode 
 * parameter: 
 *   mode -- aid matching pattern
 * return: true -- subset mode , false -- not subset mode
 */
inline bool IsAidSubsetMode(int mode)
{
    return (mode & AID_ROUTING_MODE_MASK_SUBSET) == AID_ROUTING_MODE_MASK_SUBSET;
}
}  // namespace OHOS::nfc::cardemulation
#endif  // CARD_EMULATION_DEF_H
