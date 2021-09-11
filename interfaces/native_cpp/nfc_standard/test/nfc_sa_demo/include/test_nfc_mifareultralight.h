#ifndef _TEST_NFC_MIFAREULTRALIGHT_H_
#define _TEST_NFC_MIFAREULTRALIGHT_H_

#include "test_nfc_reader.h"

namespace OHOS {
namespace nfc {
namespace sdk{
class MifareUltralightTag;
}
namespace test {
class TestReadModeTagCallBack;

class TestNfcMifareUltralight : public TestNfcReader {
public:
    bool StartTest() override;
    // 发现MifareClassic标签
    bool TestMifareUltralightTagDiscovered();

    // 验证正常读取MifareUltralight标签4个pages
    void TestReadMifareUltralightTag4Pages();

    // 验证正常写MifareUltralight标签1个page
    void TestWriteMifareUltralightTagSinglePage();
private:
    std::shared_ptr<sdk::MifareUltralightTag> ReadWriteMifareUltralightTagSetUp();
};
}  // namespace test
}  // namespace nfc
}  // namespace OHOS
#endif  // !_TEST_NFC_MIFAREULTRALIGHT_H_
