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

#ifndef IUSER_SWITCH_HANDLER_H
#define IUSER_SWITCH_HANDLER_H
namespace OHOS::nfc::cardemulation {
class IUserSwitchHandler {
public:
    virtual ~IUserSwitchHandler() = default;
    /**
     * brief: Handle user switchover events
     * parameter:
     *   userId -- user id
     * return: 0 -- succeeded, not 0 -- error code
     */
    virtual int OnUserSwitched(int userId) = 0;
private:
};
}  // namespace OHOS::nfc::cardemulation

#endif  // !IUSER_SWITCH_HANDLER_H
