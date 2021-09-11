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
#ifndef SE_BASE_TEST_H
#define SE_BASE_TEST_H

#include <memory>

#include "isecure_element_mock.h"
#include "se_end_service.h"

namespace osal {
class Context;
}
namespace OHOS::se {
class Terminal;
}
using OHOS::hardware::se::v1_0::ISecureElementMock;
namespace OHOS {
namespace se {
namespace test {
class BaseTest {
public:
    static void SetSeInitializeCall(bool status, sptr<ISecureElementMock> seMock);
    /**
     * @brief Load Secure Element, Default intialize failed.
     * @return the ISecureElementMock instance
     */
    static sptr<ISecureElementMock> LoadSecureElement(bool state = false);
    static TerminalTable LoadTerminalTable(std::shared_ptr<osal::Context> context, sptr<ISecureElementMock> seMock);
    static void SetUpOpenLogicalChannel(sptr<ISecureElementMock>& seMock);
    static void SetUpOpenLogicalChannelWithPreData(sptr<ISecureElementMock>& seMock,
                                                   std::unique_ptr<SELogicalRespData>& preRespData);
    static void SetUpBundleInfo(std::string bundleName, std::shared_ptr<osal::Context> context);
};
}  // namespace test
}  // namespace se
}  // namespace OHOS
#endif  // !SE_BASE_TEST_H