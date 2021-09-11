#include "apdu_command_validator.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "loghelper.h"
#include "se_common_exception.h"

using namespace testing;
namespace OHOS {
namespace se {
namespace test {
/**
 * @brief The command byte array is less than 4 bytes long
 */
TEST(ApduCommandValidatorTest, APDU_length_Test)
{
    // apdu length is less then 4 bytes
    std::string apdu{0x00, 0x00, 0x00};
    try {
        ApduCommandValidator::Validate(apdu);
        EXPECT_FALSE(true);
    } catch (const IllegalArgumentError& e) {
        EXPECT_TRUE(true);
    } catch (const std::exception&) {
        EXPECT_FALSE(true);
    }
}

/**
 * @brief CLA byte is invalid according to [ISO 7816-4] (0xFF).
 */
TEST(ApduCommandValidatorTest, CLA_Test)
{
    // CLA cann't be 0xFF
    std::string apdu{(char)0xFF, 0x00, 0x00, 0x00};
    try {
        ApduCommandValidator::Validate(apdu);
        EXPECT_FALSE(true);
    } catch (const IllegalArgumentError& e) {
        EXPECT_TRUE(true);
    } catch (const std::exception&) {
        EXPECT_FALSE(true);
    }
}

/**
 * @brief INS byte is invalid according to [ISO 7816-4] (0x6X or 0x9X).
 */
TEST(ApduCommandValidatorTest, INS_Test)
{
    std::string apdu{(char)0x00, 0x65, 0x00, 0x00};
    try {
        ApduCommandValidator::Validate(apdu);
        EXPECT_FALSE(true);
    } catch (const IllegalArgumentError& e) {
        EXPECT_TRUE(true);
    } catch (const std::exception&) {
        EXPECT_FALSE(true);
    }

    std::string apdu2{(char)0x00, (char)0x95, 0x00, 0x00};
    try {
        ApduCommandValidator::Validate(apdu2);
        EXPECT_FALSE(true);
    } catch (const IllegalArgumentError& e) {
        EXPECT_TRUE(true);
    } catch (const std::exception&) {
        EXPECT_FALSE(true);
    }
}

/**
 * @brief Apdu Command length is 4 bytes
 */
TEST(ApduCommandValidatorTest, CMD_APDU_LENGTH_CASE1_Test)
{
    std::string apdu{0x00, 0x00, 0x00, 0x00};
    try {
        ApduCommandValidator::Validate(apdu);
        EXPECT_TRUE(true);
    } catch (const std::exception&) {
        EXPECT_FALSE(true);
    }
}

/**
 * @brief Checking Le when Apdu Command length is 5 bytes.
 */
TEST(ApduCommandValidatorTest, Le_Test_1)
{
    std::string apdu{0x00, 0x00, 0x00, 0x00, (char)0xFF};
    try {
        ApduCommandValidator::Validate(apdu);
        EXPECT_TRUE(true);
    } catch (const std::exception& e) {
        EXPECT_FALSE(true);
    }
}

/**
 * @brief Checking short Lc/Le
 */
TEST(ApduCommandValidatorTest, Short_Test)
{
    // P3 indicate Lc and absent Le
    std::string apdu{0x00, 0x00, 0x00, 0x00, 0x01, 0x00};
    try {
        ApduCommandValidator::Validate(apdu);
        EXPECT_TRUE(true);
    } catch (const std::exception& e) {
        EXPECT_FALSE(true);
    }

    // P3 indicate Lc and Le present
    std::string apdu2{0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x02};
    try {
        ApduCommandValidator::Validate(apdu2);
        EXPECT_TRUE(true);
    } catch (const std::exception& e) {
        EXPECT_FALSE(true);
    }

    // P3 indicate Lc and Le present, length is error
    std::string apdu3{0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01};
    try {
        ApduCommandValidator::Validate(apdu3);
        EXPECT_FALSE(true);
    } catch (const IllegalArgumentError& e) {
        InfoLog("%s", e.what());
        EXPECT_TRUE(true);
    } catch (const std::exception& e) {
        EXPECT_FALSE(true);
    }
}

/**
 * @brief Checking extended Le
 */
TEST(ApduCommandValidatorTest, Extended_Test_1)
{
    // Command Header + extended Le(When Lc is absent, Le length is 3 bytes)
    std::string apdu{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01};
    try {
        ApduCommandValidator::Validate(apdu);
        EXPECT_TRUE(true);
    } catch (const std::exception& e) {
        EXPECT_FALSE(true);
    }

    // extended Le is 0xFFFF
    std::string apdu2{0x00, 0x00, 0x00, 0x00, 0x00, (char)0xFF, (char)0xFF};
    try {
        ApduCommandValidator::Validate(apdu2);
        EXPECT_TRUE(true);
    } catch (const std::exception& e) {
        EXPECT_FALSE(true);
    }

    // apdu length is 6 bytes, but extended length is error
    std::string apdu3{0x00, 0x00, 0x00, 0x00, 0x00, 0x01};
    try {
        ApduCommandValidator::Validate(apdu3);
        EXPECT_FALSE(true);
    } catch (const IllegalArgumentError& e) {
        EXPECT_TRUE(true);
    } catch (const std::exception& e) {
        EXPECT_FALSE(true);
    }
}

/**
 * @brief Checking extended Lc
 */
TEST(ApduCommandValidatorTest, Extended_Test_2)
{
    // Command Header + extended Lc, Lc is equal of 0x0000 and Lc can't be 0
    std::string apdu{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    try {
        ApduCommandValidator::Validate(apdu);
        EXPECT_FALSE(true);
    } catch (const IllegalArgumentError& e) {
        EXPECT_TRUE(true);
    } catch (const std::exception& e) {
        EXPECT_FALSE(true);
    }

    // Command Header + only extended Lc
    std::string apdu2{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00};
    try {
        ApduCommandValidator::Validate(apdu2);
        EXPECT_TRUE(true);
    } catch (const std::exception& e) {
        InfoLog("%s", e.what());
        EXPECT_FALSE(true);
    }

    // Command Header + extended Lc + extended Le(length is 2 bytes, when Lc is present)
    std::string apdu3{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00};
    try {
        ApduCommandValidator::Validate(apdu3);
        EXPECT_TRUE(true);
    } catch (const std::exception& e) {
        InfoLog("%s", e.what());
        EXPECT_FALSE(true);
    }

    // Command Header + extended Lc + extended Le, but Le length is error or Lc length is error
    std::string apdu4{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00};
    try {
        ApduCommandValidator::Validate(apdu4);
        EXPECT_FALSE(true);
    } catch (const IllegalArgumentError& e) {
        EXPECT_TRUE(true);
    } catch (const std::exception& e) {
        EXPECT_FALSE(true);
    }
}
}  // namespace test
}  // namespace se
}  // namespace OHOS