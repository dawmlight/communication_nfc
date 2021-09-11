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
#ifndef TEST_NFC_CE_APP_H
#define TEST_NFC_CE_APP_H

#include <memory>
#include <string>
#include <vector>

#include "loghelper.h"
#include "ohos_application.h"

namespace OHOS {
namespace nfc {
namespace sdk {
class NfcAgent;
}
namespace test {
class TestNfcCEApp : public AppExecFwk::OHOSApplication {
public:
    TestNfcCEApp();
    ~TestNfcCEApp();

    static std::shared_ptr<TestNfcCEApp> GenAppliaction(const std::string& name);
    // Application function
    void OnStart() override;
    void OnTerminate() override;

    // Turn On Nfc
    void TurnOnNfc();
    // Turn Off Nfc
    void TurnOffNfc();

    std::shared_ptr<sdk::NfcAgent> GetNfcAgent();
    void SetNfcAgent(std::shared_ptr<sdk::NfcAgent> nfcAgent);

private:
    void SetInstance(std::shared_ptr<TestNfcCEApp> ins)
    {
        ins_ = ins;
    }

private:
    std::shared_ptr<sdk::NfcAgent> mNFCAgent_;
    std::shared_ptr<TestNfcCEApp> ins_;
};
}  // namespace test
}  // namespace nfc
}  // namespace OHOS
#endif /* TEST_NFC_CE_APP_H */
