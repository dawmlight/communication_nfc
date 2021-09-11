#include "test_nfc_reader.h"

#include <thread>

#include "ability_local_record.h"
#include "nfc_map.h"
#include "loghelper.h"
#include "test_nfc_ability.h"
#include "test_nfc_reader_app.h"
#include "test_read_mode_tag_callback.h"
#include "want.h"

using namespace OHOS::nfc::sdk;
using OHOS::AAFwk::Want;

namespace OHOS {
using AppExecFwk::AbilityLocalRecord;
namespace nfc {
namespace test {
// Initailize Nfc Reader
bool TestNfcReader::Initailize(int type)
{
    std::shared_ptr<TestReadModeTagCallBack> callback = std::make_shared<TestReadModeTagCallBack>();
    std::shared_ptr<NfcMap> extras = std::make_shared<NfcMap>();
    return Initailize(callback, type, extras);
}

bool TestNfcReader::Initailize(std::shared_ptr<NfcMap> extras)
{
    std::shared_ptr<TestReadModeTagCallBack> callback = std::make_shared<TestReadModeTagCallBack>();
    return Initailize(callback, 0x0f, extras);
}

bool TestNfcReader::Initailize(int type, std::shared_ptr<NfcMap> extras)
{
    std::shared_ptr<TestReadModeTagCallBack> callback = std::make_shared<TestReadModeTagCallBack>();
    return Initailize(callback, type, extras);
}

bool TestNfcReader::Initailize(std::shared_ptr<TestReadModeTagCallBack> callback,
                               int type,
                               std::shared_ptr<NfcMap> extras)
{
    std::string appName("test.nfc.reader.app");
    application_ = TestNfcReaderApp::GenAppliaction(appName);
    ability_ = TestNfcAbility::GenAbility(application_);
    callback_ = callback;
    ability_->SetTagCallback(callback);
    ability_->SetTagTypeFilter(type);
    ability_->SetExtras(extras);

    // start ability
    application_->OnAbilityStart(ability_);

    InfoLog("Get the NFC Agent Instance");
    std::shared_ptr<NfcAgent> nfcAgent = NfcAgent::GetNFCAgent(application_);
    if (!nfcAgent) {
        InfoLog("The NFC Agent Instance Is Empty.");
    }else
    {
        application_->SetNfcAgent(nfcAgent);
    }
    return true;
}

void TestNfcReader::DeInitailize()
{
    application_ = std::shared_ptr<TestNfcReaderApp>();
    ability_ = std::shared_ptr<TestNfcAbility>();
    callback_ = std::shared_ptr<TestReadModeTagCallBack>();
}

// Turn On Nfc
void TestNfcReader::TurnOnNfc()
{
    application_->TurnOnNfc();
}

// Turn Off Nfc
void TestNfcReader::TurnOffNfc()
{
    application_->TurnOffNfc();
}

// Start up
void TestNfcReader::StartAbility()
{
    Want want;
    if (ability_) {
        ability_->OnStart(want);
    } else {
        InfoLog("ability_ is empty.");
    }
}

void TestNfcReader::TerminalAbility()
{
    ability_->OnStop();
}

void TestNfcReader::ActiveAbility()
{
    ability_->OnActive();
    application_->OnAbilityActive(ability_);
}

void TestNfcReader::InactiveAbility()
{
    ability_->OnInactive();
    application_->OnAbilityInactive(ability_);
}
}  // namespace test
}  // namespace nfc
}  // namespace OHOS