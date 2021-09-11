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
#include "test_nfc_ce_app.h"

#include <string>

#include "app_loader.h"
#include "loghelper.h"
#include "nfc_ability_manager.h"
#include "nfc_agent.h"
#include "nfc_sdk_common.h"

using namespace OHOS::nfc::sdk;
using namespace std;
namespace OHOS {
using AppExecFwk::ApplicationInfo;
using AppExecFwk::ContextDeal;
using AppExecFwk::ProcessInfo;
namespace AppExecFwk {
REGISTER_APPLICATION("TestNfcCEApp", OHOSApplication)
}

namespace nfc {
namespace test {
TestNfcCEApp::TestNfcCEApp() : OHOSApplication() {}

TestNfcCEApp::~TestNfcCEApp() {}

std::shared_ptr<TestNfcCEApp> TestNfcCEApp::GenAppliaction(const std::string& name)
{
    std::shared_ptr<ContextDeal> contextDeal = std::make_shared<ContextDeal>();
    std::shared_ptr<TestNfcCEApp> application = std::make_shared<TestNfcCEApp>();
    std::shared_ptr<ApplicationInfo> appInfo = std::make_shared<ApplicationInfo>();
    std::shared_ptr<ProcessInfo> processInfo = std::make_shared<ProcessInfo>();

    appInfo->name = name;
    appInfo->codePath = "codePath";
    appInfo->dataBaseDir = "dataBaseDir";
    appInfo->dataDir = "dataDir";
    appInfo->cacheDir = "cacheDir";
    appInfo->bundleName = "bundleName";

    contextDeal->SetProcessInfo(processInfo);
    contextDeal->SetApplicationInfo(appInfo);
    contextDeal->SetApplicationContext(application);
    application->AttachBaseContext(contextDeal);
    application->SetInstance(application);

    return application;
}

// Turn On Nfc
void TestNfcCEApp::TurnOnNfc()
{
    InfoLog("TestNfcCEApp::TurnOnNfc");
    if (!mNFCAgent_) {
        InfoLog("Get the NFC Agent Instance");
        mNFCAgent_ = NfcAgent::GetNFCAgent(ins_);
        if (!mNFCAgent_) {
            InfoLog("The NFC Agent Instance Is Empty.");
            return;
        }

        int res = mNFCAgent_->ActiveNfc();
        InfoLog("Turn On the NFC res.%d", res);
        return;
    }

    if (mNFCAgent_->GetNfcState() == sdk::STATE_OFF) {
        InfoLog("Turn On the NFC");
        int res = mNFCAgent_->ActiveNfc();
        InfoLog("Turn On the NFC res.%d", res);
    }
}

// Turn Off Nfc
void TestNfcCEApp::TurnOffNfc()
{
    InfoLog("TestNfcCEApp::TurnOffNfc");
    if (!mNFCAgent_) {
        InfoLog("The NFC Agent Instance Is Empty.");
        return;
    }

    if (mNFCAgent_->GetNfcState() == sdk::STATE_OFF) {
        InfoLog("Nfc State off");
        return;
    }
    InfoLog("Turn Off the NFC");
    int res = mNFCAgent_->DeactiveNfc();
    InfoLog("Turn Off the NFC res.%d", res);
}

void TestNfcCEApp::OnStart()
{
    InfoLog("TestNfcCEApp::OnStart");
    if (!mNFCAgent_) {
        mNFCAgent_ = NfcAgent::GetNFCAgent(ins_);
    }
}

void TestNfcCEApp::OnTerminate()
{
    InfoLog("TestNfcCEApp::OnTerminate");
    if (mNFCAgent_) {
        mNFCAgent_ = std::shared_ptr<sdk::NfcAgent>();
    }
}

std::shared_ptr<NfcAgent> TestNfcCEApp::GetNfcAgent()
{
    return mNFCAgent_;
}

void TestNfcCEApp::SetNfcAgent(std::shared_ptr<sdk::NfcAgent> nfcAgent)
{
    mNFCAgent_ = nfcAgent;
}
}  // namespace test
}  // namespace nfc
}  // namespace OHOS