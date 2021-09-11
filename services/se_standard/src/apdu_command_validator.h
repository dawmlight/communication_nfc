#ifndef APDU_COMMAND_VALIDATOR_H
#define APDU_COMMAND_VALIDATOR_H

#include <string>

namespace OHOS {
namespace se {
class ApduCommandValidator final {
    static const int CMD_APDU_LENGTH_CASE1 = 4;
    static const int CMD_APDU_LENGTH_CASE2 = 5;
    static const int CMD_APDU_LENGTH_CASE2_EXTENDED = 7;
    static const size_t CMD_APDU_LENGTH_CASE3_WITHOUT_DATA = 5;
    static const size_t CMD_APDU_LENGTH_CASE3_WITHOUT_DATA_EXTENDED = 7;
    static const size_t CMD_APDU_LENGTH_CASE4_WITHOUT_DATA = 6;
    static const size_t CMD_APDU_LENGTH_CASE4_WITHOUT_DATA_EXTENDED = 9;
    static const int MAX_EXPECTED_DATA_LENGTH = 65536;

    static const int OFFSET_CLA = 0;
    static const int OFFSET_INS = 1;
    static const int OFFSET_P3 = 4;
    static const int OFFSET_DATA = 5;
    static const int OFFSET_DATA_EXTENDED = 7;

private:
    ApduCommandValidator() {}
    static void CheckLe(int le);
    static void CheckCla(char cla);
    static void CheckIns(char ins);

public:
    static void Validate(const std::string& apdu);
};
}  // namespace se
}  // namespace OHOS
#endif  // !APDU_COMMAND_VALIDATOR_H