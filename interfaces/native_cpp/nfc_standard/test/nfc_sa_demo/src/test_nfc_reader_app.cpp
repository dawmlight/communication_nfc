#include "test_nfc_reader_app.h"

#include <string>

#include "app_loader.h"
#include "loghelper.h"
#include "nfc_ability_manager.h"
#include "nfc_agent.h"
#include "nfc_sdk_common.h"
#include "test_read_mode_tag_callback.h"

using namespace OHOS::nfc::sdk;
using namespace std;
namespace OHOS {
using AppExecFwk::ApplicationInfo;
using AppExecFwk::ContextDeal;
using AppExecFwk::ProcessInfo;
namespace AppExecFwk {
REGISTER_APPLICATION("TestNfcReaderApp", OHOSApplication)
}

namespace nfc {
namespace test {
TestNfcReaderApp::TestNfcReaderApp() : OHOSApplication() {}

TestNfcReaderApp::~TestNfcReaderApp() {}

std::shared_ptr<TestNfcReaderApp> TestNfcReaderApp::GenAppliaction(const std::string& name)
{
    std::shared_ptr<ContextDeal> contextDeal = std::make_shared<ContextDeal>();
    std::shared_ptr<TestNfcReaderApp> application = std::make_shared<TestNfcReaderApp>();
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
void TestNfcReaderApp::TurnOnNfc()
{
    InfoLog("TestNfcReaderApp::TurnOnNfc");
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
void TestNfcReaderApp::TurnOffNfc()
{
    InfoLog("TestNfcReaderApp::TurnOffNfc");
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

void TestNfcReaderApp::OnStart()
{
    InfoLog("TestNfcReaderApp::OnStart");
    if (!mNFCAgent_) {
        mNFCAgent_ = NfcAgent::GetNFCAgent(ins_);
    }
}

void TestNfcReaderApp::OnTerminate()
{
    InfoLog("TestNfcReaderApp::OnTerminate");
    if (mNFCAgent_) {
        mNFCAgent_ = std::shared_ptr<sdk::NfcAgent>();
    }
}

std::shared_ptr<NfcAgent> TestNfcReaderApp::GetNfcAgent()
{
    return mNFCAgent_;
}

void TestNfcReaderApp::SetNfcAgent(std::shared_ptr<sdk::NfcAgent> nfcAgent)
{
    mNFCAgent_ = nfcAgent;
}
}  // namespace test
}  // namespace nfc
}  // namespace OHOS