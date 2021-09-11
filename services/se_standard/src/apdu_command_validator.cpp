#include "apdu_command_validator.h"

#include <sstream>

#include "loghelper.h"
#include "se_common_exception.h"

namespace OHOS {
namespace se {
void ApduCommandValidator::Validate(const std::string& apdu)
{
    if (apdu.length() < CMD_APDU_LENGTH_CASE1) {
        std::stringstream errorMsg;
        errorMsg << "Invalid length for command (" << apdu.length() << ").";
        throw IllegalArgumentError(errorMsg.str());
    }
    CheckCla(apdu[OFFSET_CLA]);
    CheckIns(apdu[OFFSET_INS]);

    if (apdu.length() == CMD_APDU_LENGTH_CASE1) {
        // Only the command head.
        return;  // Case 1
    }

    if (apdu.length() == CMD_APDU_LENGTH_CASE2) {
        // the command head and Le. OFFSET_P3 represent Le and Le's length is 1 byte
        CheckLe((int)0x0FF & apdu[OFFSET_P3]);
        return;  // Case 2S
    }

    // short Lc(P3 is not equal to 0x00)
    if (apdu[OFFSET_P3] != (char)0x00) {
        int lc = ((int)0x0FF & apdu[OFFSET_P3]);
        if (apdu.length() == CMD_APDU_LENGTH_CASE3_WITHOUT_DATA + lc) {
            // the apdu length = 4 bytes(command header) + 1byte(short lc) + N bytes(command data), and absent le,
            return;  // Case 3S
        }
        if (apdu.length() == CMD_APDU_LENGTH_CASE4_WITHOUT_DATA + lc) {
            // the apdu length = 4 bytes(command header) + 1byte(short lc) + N bytes(command data) + 1byte(short le),
            CheckLe((int)0x0FF & apdu[apdu.length() - 1]);
            return;  // Case 4S
        }
        std::stringstream errorMsg;
        errorMsg << "Unexpected value of Lc (" << lc << ")";
        throw IllegalArgumentError(errorMsg.str());
    }
    // Extended Le(apdu[OFFSET_P3] is equal of 0x00)
    if (apdu.length() == CMD_APDU_LENGTH_CASE2_EXTENDED) {
        // the apdu length = 4 bytes(command header) + 3byte(Extended le)
        CheckLe((((int)0x0FF & apdu[OFFSET_DATA]) << 8) + ((int)0x0FF & apdu[OFFSET_DATA + 1]));
        return;  // Case 2E
    }

    if (apdu.length() <= OFFSET_DATA_EXTENDED) {
        std::stringstream errorMsg;
        errorMsg << "Unexpected value of Lc or Le" << apdu.length();
        throw IllegalArgumentError(errorMsg.str());
    }

    // Extended Lc(apdu[OFFSET_P3] is equal of 0x00, and Apdu length is bigger then 7)
    int lc = (((int)0x0FF & apdu[OFFSET_DATA]) << 8) + ((int)0x0FF & apdu[OFFSET_DATA + 1]);
    if (lc == 0) {
        throw IllegalArgumentError("Lc can't be 0");
    }

    if (apdu.length() == CMD_APDU_LENGTH_CASE3_WITHOUT_DATA_EXTENDED + lc) {
        return;  // Case 3E
    }

    if (apdu.length() == CMD_APDU_LENGTH_CASE4_WITHOUT_DATA_EXTENDED + lc) {
        CheckLe((((int)0x0FF & apdu[apdu.length() - 2]) << 8) + ((int)0x0FF & apdu[apdu.length() - 1]));
        return;  // Case 4E
    }
    std::stringstream errorMsg;
    errorMsg << "Unexpected value of Lc (" << lc << ")";
    throw IllegalArgumentError(errorMsg.str());
}

void ApduCommandValidator::CheckCla(char cla)
{
    if (cla == (char)0xFF) {
        std::stringstream errorMsg;
        errorMsg << "Invalid value of CLA (" << std::hex << cla << ")";
        throw IllegalArgumentError(errorMsg.str());
    }
}

void ApduCommandValidator::CheckIns(char ins)
{
    if ((ins & 0xF0) == 0x60 || ((ins & 0xF0) == 0x90)) {
        std::stringstream errorMsg;
        errorMsg << "Invalid value of INS (" << std::hex << ins << "). 0x6X and 0x9X are not valid values";
        throw IllegalArgumentError(errorMsg.str());
    }
}

void ApduCommandValidator::CheckLe(int le)
{
    if (le < 0 || le > MAX_EXPECTED_DATA_LENGTH) {
        std::stringstream errorMsg;
        errorMsg << "Invalid value for le parameter (" << le << ").";
        throw IllegalArgumentError(errorMsg.str());
    }
}
}  // namespace se
}  // namespace OHOS