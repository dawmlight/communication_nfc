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

#ifndef ICARD_EMULATION_EVENT_HANDLER_H
#define ICARD_EMULATION_EVENT_HANDLER_H

#include <cstdio>

namespace OHOS::nfc::cardemulation {
class ICardEmulationEventHandler {
public:
    virtual ~ICardEmulationEventHandler(){};
    /**
     * brief: Handle HCE activated events
     * parameter: void
     * return: int - 0 -- succeeded, not 0 -- error code
     */
    virtual int OnHCEActivated(void) = 0;
    /**
     * brief: Handle HCE data transfer events
     * parameter:
     *     data - HCE data transmitted by NCI
     *     len  - data length. unit: byte
     * return: int - 0 -- succeeded, not 0 -- error code
     */
    virtual int OnHCEData(const unsigned char* data, size_t len) = 0;
    /**
     * brief: Handles HCE deactivated events
     * parameter: void
     * return: int - 0 -- succeeded, not 0 -- error code
     */
    virtual int OnHCEDeactivated(void) = 0;
    /**
     * brief: Process OffHost transaction event
     * parameter: void
     * return: int - 0 -- succeeded, not 0 -- error code
     */
    virtual int OnOffHostTransaction(void) = 0;
};
}  // namespace OHOS::nfc::cardemulation
#endif  // ICARD_EMULATION_EVENT_HANDLER_H
