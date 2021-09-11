#ifndef _TEST_NFC_MIFARECLASSIC_H_
#define _TEST_NFC_MIFARECLASSIC_H_

#include "test_nfc_reader.h"

namespace OHOS {
namespace nfc {
namespace sdk{
class MifareClassicTag;
}
namespace test {
class TestReadModeTagCallBack;

class TestNfcMifareClassic : public TestNfcReader {
public:
    bool StartTest() override;

    // 发现MifareClassic标签
    bool TestMifareClassicTagDiscovered();

    // 验证正常读取MifareClassic
    void TestReadMifareClassicTag();

    // 验证正常写MifareClassic
    void TestWriteMifareClassicTag();
    
    // 验证自增
    void TestIncrementMifareClassicTag();

    // 验证自减
    void TestDecrementMifareClassicTag();

    int ReadWalletValue(std::shared_ptr<sdk::MifareClassicTag> mifareClassicTag, int sectorIndex, int blockIndex, bool bAuth);
private:
    std::shared_ptr<sdk::MifareClassicTag> ReadWriteMifareClassicTagSetUp();
};
}  // namespace test
}  // namespace nfc
}  // namespace OHOS
#endif  // !_TEST_NFC_MIFARECLASSIC_H_
