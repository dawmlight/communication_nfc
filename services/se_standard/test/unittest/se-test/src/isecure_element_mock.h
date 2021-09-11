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
#ifndef ISECURE_ELEMENT_READER_MOCK_H
#define ISECURE_ELEMENT_READER_MOCK_H

#include <gmock/gmock.h>

#include "iremote_object.h"
#include "isecure_element.h"

namespace OHOS {
namespace hardware {
namespace se {
namespace v1_0 {
class ISecureElementMock : public ISecureElement {
public:
    ISecureElementMock() {}
    ~ISecureElementMock() {}
    MOCK_METHOD1(Initialize, void(const sptr<IRemoteObject>&));
    MOCK_METHOD0(GetAtr, std::string());
    MOCK_METHOD0(CloseSessions, void());
    MOCK_METHOD0(IsCardPresent, bool());
    MOCK_METHOD1(Transmit, std::string(const std::string& cmd));
    MOCK_METHOD2(OpenBasicChannel, std::unique_ptr<BasicRespData>(const std::string& aid, char p2));
    MOCK_METHOD2(OpenLogicalChannel, std::unique_ptr<LogicalRespData>(const std::string& aid, char p2));
    MOCK_METHOD1(CloseChannel, SecureElementStatus(uint8_t channelNumber));
    MOCK_METHOD0(Reset, SecureElementStatus());
    MOCK_METHOD0(AsObject, sptr<OHOS::IRemoteObject>());
};
}  // namespace v1_0
}  // namespace se
}  // namespace hardware
}  // namespace OHOS
#endif  // !ISECURE_ELEMENT_READER_MOCK_H
