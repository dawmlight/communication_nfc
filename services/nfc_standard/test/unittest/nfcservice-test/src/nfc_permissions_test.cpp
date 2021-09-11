#include "nfc_permissions.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "context.h"
#include "shared_preferences.h"

using namespace OHOS::nfc;
using namespace osal;

const std::string ADMIN_PERM = "hos.Manifest.permission.WRITE_SECURE_SETTINGS";
const std::string ADMIN_PERM_ERROR = "WRITE_SECURE_SETTINGS permission required";
const std::string NFC_PERMISSION = "hos.Manifest.permission.NFC";
const std::string NFC_PERM_ERROR = "NFC permission required";
const std::string NFC_PREFERRED_PAYMENT_INFO_PERMISSION = "android.Manifest.permission.NFC_PREFERRED_PAYMENT_INFO";
const std::string NFC_PREFERRED_PAYMENT_INFO_PERM_ERROR = "NFC_PREFERRED_PAYMENT_INFO permission required";

TEST(NfcPermissions, EnforceAdminPermissions_Test)
{
    std::shared_ptr<Context> ctx = std::make_shared<Context>();
    std::weak_ptr<SharedPreferences> pref = ctx->GetSharedPreferences("nfc");
    pref.lock()->PutBool(ADMIN_PERM, false);
    try {
        NfcPermissions::EnforceAdminPermissions(ctx);
    } catch (const std::exception& e) {
        EXPECT_STREQ(e.what(), ADMIN_PERM_ERROR.c_str());
    }
    pref.lock()->PutBool(ADMIN_PERM, true);
    try {
        NfcPermissions::EnforceAdminPermissions(ctx);
    } catch (const std::exception& e) {
        EXPECT_STREQ(e.what(), "");
    }
}

TEST(NfcPermissions, EnforceUserPermissions_Test)
{
    std::shared_ptr<Context> ctx = std::make_shared<Context>();
    std::weak_ptr<SharedPreferences> pref = ctx->GetSharedPreferences("nfc");
    pref.lock()->PutBool(NFC_PERMISSION, false);
    try {
        NfcPermissions::EnforceUserPermissions(ctx);
    } catch (const std::exception& e) {
        EXPECT_STREQ(e.what(), NFC_PERM_ERROR.c_str());
    }
    pref.lock()->PutBool(NFC_PERMISSION, true);
    try {
        NfcPermissions::EnforceUserPermissions(ctx);
    } catch (const std::exception& e) {
        EXPECT_STREQ(e.what(), "");
    }
}

TEST(NfcPermissions, EnforcePreferredPaymentInfoPermissions_Test)
{
    std::shared_ptr<Context> ctx = std::make_shared<Context>();
    std::weak_ptr<SharedPreferences> pref = ctx->GetSharedPreferences("nfc");
    pref.lock()->PutBool(NFC_PREFERRED_PAYMENT_INFO_PERMISSION, false);
    try {
        NfcPermissions::EnforcePreferredPaymentInfoPermissions(ctx);
    } catch (const std::exception& e) {
        EXPECT_STREQ(e.what(), NFC_PREFERRED_PAYMENT_INFO_PERM_ERROR.c_str());
    }
    pref.lock()->PutBool(NFC_PREFERRED_PAYMENT_INFO_PERMISSION, true);
    try {
        NfcPermissions::EnforcePreferredPaymentInfoPermissions(ctx);
    } catch (const std::exception& e) {
        EXPECT_STREQ(e.what(), "");
    }
}
