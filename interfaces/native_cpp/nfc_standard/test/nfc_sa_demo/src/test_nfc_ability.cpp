#include "test_nfc_ability.h"

#include "ability_handler.h"
#include "ability_info.h"
#include "ability_loader.h"
#include "nfc_map.h"
#include "loghelper.h"
#include "nfc_agent.h"
#include "nfc_sdk_common.h"
#include "tag.h"
#include "test_nfc_reader_app.h"
#include "test_read_mode_tag_callback.h"
#include "want.h"

using namespace OHOS::nfc::sdk;
using namespace std;
namespace OHOS {
using AppExecFwk::AbilityHandler;
using AppExecFwk::AbilityInfo;
namespace nfc {
namespace test {
TestNfcAbility::TestNfcAbility() : mTagTypeFilter_(0x0F) {}

TestNfcAbility::~TestNfcAbility()
{
    if (mTagCallBack_) {
        mTagCallBack_->RemoveTag();
    }

    mTagTypeFilter_ = 0;
}

std::shared_ptr<TestNfcAbility> TestNfcAbility::GenAbility(const std::shared_ptr<TestNfcReaderApp>& application)
{
    std::string abilityName("test.nfc.ability");
    return GenAbility(abilityName, application);
}

std::shared_ptr<TestNfcAbility> TestNfcAbility::GenAbility(std::string& abilityName,
                                                           const std::shared_ptr<TestNfcReaderApp>& application)
{
    std::shared_ptr<AbilityInfo> abilityInfo = std::make_shared<AbilityInfo>();
    abilityInfo->name = abilityName;
    return GenAbility(abilityInfo, application);
}

std::shared_ptr<TestNfcAbility> TestNfcAbility::GenAbility(const std::shared_ptr<AbilityInfo>& abilityInfo,
                                                           const std::shared_ptr<TestNfcReaderApp>& application)
{
    std::shared_ptr<TestNfcAbility> ability = std::make_shared<TestNfcAbility>();
    std::shared_ptr<AbilityInfo> info = abilityInfo;
    if (!info) {
        info = std::make_shared<AbilityInfo>();
        info->name = "test.nfc.ability";
    }

    std::shared_ptr<AppExecFwk::AbilityHandler> hander{};
    sptr<IRemoteObject> token{};
    ability->Init(info, application, hander, token);
    ability->SetReaderApp(application);
    return ability;
}

void TestNfcAbility::OnStart(const Want& want)
{
    DebugLog("TestNfcAbility::OnStart");
    if (!mTagCallBack_) {
        mTagCallBack_ = std::make_shared<TestReadModeTagCallBack>();
    }
    if (!mExtras_) {
        mExtras_ = std::make_shared<NfcMap>();
    }
    std::shared_ptr<NfcAgent> nfcAgent = readerApp_->GetNfcAgent();
    if (nfcAgent) {
        InfoLog("mNFCAgent_ Is Exist.");
        nfcAgent->ActiveNfcReaderMode(shared_from_this(), mTagCallBack_, mTagTypeFilter_, mExtras_);
    } else {
        InfoLog("mNFCAgent_ Is Empty.");
    }
    
    // Ability::OnStart(want);
}

void TestNfcAbility::OnStop()
{
    mTagCallBack_ = std::shared_ptr<TestReadModeTagCallBack>();
    mExtras_ = std::shared_ptr<NfcMap>();
    Ability::OnStop();
}

void TestNfcAbility::SetReaderApp(std::shared_ptr<TestNfcReaderApp> readerApp)
{
    readerApp_ = readerApp;
}

std::shared_ptr<TestReadModeTagCallBack> TestNfcAbility::GetTagCallback()
{
    return mTagCallBack_;
}

void TestNfcAbility::SetTagCallback(std::shared_ptr<TestReadModeTagCallBack> tagCallback)
{
    mTagCallBack_ = tagCallback;
}

int TestNfcAbility::GetTagTypeFilter()
{
    return mTagTypeFilter_;
}

void TestNfcAbility::SetTagTypeFilter(int tagTypeFilter)
{
    mTagTypeFilter_ = tagTypeFilter;
}

std::shared_ptr<NfcMap> TestNfcAbility::GetExtras()
{
    return mExtras_;
}

void TestNfcAbility::SetExtras(std::shared_ptr<NfcMap> extras)
{
    mExtras_ = extras;
}

std::shared_ptr<Tag> TestNfcAbility::GetTag()
{
    return mTagCallBack_->GetTag();
}

void TestNfcAbility::RemoveTag()
{
    if (!mTagCallBack_) {
    }
    mTagCallBack_->RemoveTag();
}
}  // namespace test
}  // namespace nfc
using nfc::test::TestNfcAbility;
namespace AppExecFwk {
REGISTER_AA(TestNfcAbility)
}
}  // namespace OHOS