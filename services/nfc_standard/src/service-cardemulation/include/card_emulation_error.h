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

#ifndef CARD_EMULATION_ERROR_H
#define CARD_EMULATION_ERROR_H

namespace OHOS::nfc::cardemulation {
constexpr int ERR_OK = 0;
constexpr int ERR_COMMON_START = ERR_OK - 1;
constexpr int ERR_UNKNOWN = ERR_OK - 2;
constexpr int ERR_NULL_POINTER = ERR_OK - 3;
constexpr int ERR_UNIMPLEMENT = ERR_OK - 4;
constexpr int ERR_UNSURPPORTED = ERR_OK - 5;

constexpr int ERR_CARD_EMULATION_START = 0x00000300;
constexpr int ERR_CARD_EMULATION_SERVICE_NOT_INIT = ERR_CARD_EMULATION_START + 1;  // The card emulation service is not initialized
constexpr int ERR_AID_ILLIGLE_LENGTH = ERR_CARD_EMULATION_START + 2;               // AID length error
constexpr int ERR_AID_INVALID = ERR_CARD_EMULATION_START + 3;                      // AID invalid (contains invalid characters)
constexpr int ERR_NOT_FOUND_USER_APP_INFO = ERR_CARD_EMULATION_START + 4;  // User card emulation service information not found
constexpr int ERR_AIDSET_IS_EMPTY = ERR_CARD_EMULATION_START + 5;  // Possible cause: construct failed with invalid AID.
constexpr int ERR_NOT_FOUND_USER_INFO = ERR_CARD_EMULATION_START + 6;           // User information was not found
constexpr int ERR_NOT_FOUND_CESERVICE_INFO = ERR_CARD_EMULATION_START + 7;      // Card emulation service information not found
constexpr int ERR_HOST_NO_MODIFICATION_ALLOWED = ERR_CARD_EMULATION_START + 8;  // The HCE service cannot modify the EE location
constexpr int ERR_INVALID_ELEMENT_NAME = ERR_CARD_EMULATION_START + 9;          // Invalid name
constexpr int ERR_CESERVICE_GETTER_IS_NULL = ERR_CARD_EMULATION_START + 10;     // The card analog service getter is nullptr
constexpr int ERR_ADD_ROUTING_AID_FAILED = ERR_CARD_EMULATION_START + 11;       // Failed to add the AID route.
constexpr int ERR_REMOVE_ROUTING_AID_FAILED = ERR_CARD_EMULATION_START + 12;    // Failed to move the AID route.
constexpr int ERR_COMMIT_ROUTING_FAILED = ERR_CARD_EMULATION_START + 13;        // Failed to commit the AID route.
constexpr int ERR_NOT_INIT_NCI = ERR_CARD_EMULATION_START + 14;                 // The NCI object is not initialized
constexpr int ERR_NOT_SUPPORT_TECH = ERR_CARD_EMULATION_START + 15;             // Unsupported technical identifier
constexpr int ERR_EMPTY_APDU = ERR_CARD_EMULATION_START + 16;                   // empty apdu
constexpr int ERR_ROUTING_TABLE_NOT_ENOUGH_CAPACITY = ERR_CARD_EMULATION_START + 17;  // The routing table did not have sufficient capacity.
constexpr int ERR_DROP_HCE_EVENT_DATA = ERR_CARD_EMULATION_START + 18;                // Discard HCE events and data
constexpr int ERR_INVALID_REQUEST_HEADER = ERR_CARD_EMULATION_START + 19;             // The remote request header is invalid
constexpr int ERR_INVALID_EXEUCTION_ENVIRONMENT = ERR_CARD_EMULATION_START + 20;      // Invalid execution environment
constexpr int ERR_INVALID_USERID = ERR_CARD_EMULATION_START + 21;                     // invalid user ID
constexpr int ERR_INVALID_SERVICE_TYPE = ERR_CARD_EMULATION_START + 22;               // invalid service type
constexpr int ERR_CONFILICTS_WITH_DEFAULT = ERR_CARD_EMULATION_START + 23;            // confilicts with default(primary service): maybe service type or aid

inline bool IS_OK(int err_code)
{
    return err_code == ERR_OK;
}
inline bool IS_NOK(int err_code)
{
    return !IS_OK(err_code);
}
}  // namespace OHOS::nfc::cardemulation
#endif  // CARD_EMULATION_ERROR_H
