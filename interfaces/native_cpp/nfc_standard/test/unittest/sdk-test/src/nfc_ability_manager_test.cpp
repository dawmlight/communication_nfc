#include "nfc_ability_manager.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "ability.h"
#include "ohos_application.h"

using OHOS::AppExecFwk::Ability;
using OHOS::AppExecFwk::OHOSApplication;
using OHOS::nfc::sdk::NfcAbilityManager;
/**
 * @tc.number    : NFC_NfcAbilityManager_API_0001
 * @tc.name      : API SetTagTypeFilter test
 * @tc.desc      : Api test
 */
TEST(NfcAbilityManager, SetTagTypeFilter_Test)
{
    auto nam = std::make_shared<NfcAbilityManager>();
    auto a = std::make_shared<Ability>();
    auto ao = std::make_shared<NfcAbilityManager::NfcAbilityObject>(a, true);
    ao->SetTagTypeFilter(1);
    ao->GetTagTypeFilter();
    ao->GetBinder();
}

/**
 * @tc.number    : NFC_NfcAbilityManager_API_0002
 * @tc.name      : API NfcApplicationObject test
 * @tc.desc      : Api test
 */
TEST(NfcAbilityManager, NfcApplicationObject_Test)
{
    auto nam = std::make_shared<NfcAbilityManager>();
    auto a = std::make_shared<OHOSApplication>();
    auto ao = std::make_shared<NfcAbilityManager::NfcApplicationObject>(a);
    ao->RegistApp(nam);
}