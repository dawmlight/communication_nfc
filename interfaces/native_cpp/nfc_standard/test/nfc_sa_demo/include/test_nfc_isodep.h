#ifndef _TEST_NFC_ISODEP_H_
#define _TEST_NFC_ISODEP_H_

#include "test_nfc_reader.h"

namespace OHOS {
namespace nfc {
namespace sdk{
class IsoDepTag;
}
namespace test {
class TestReadModeTagCallBack;

class TestNfcIsoDep : public TestNfcReader {
public:
    bool StartTest() override;
    // 发现IsoDep标签
    bool TestIsoDepTagDiscovered();

    // 验证正常读取IsoDep标签
    void TestIsoDepTagSendCommand();
private:
    std::shared_ptr<sdk::IsoDepTag> ReadWriteIsoDepTagSetUp();
};
}  // namespace test
}  // namespace nfc
}  // namespace OHOS
#endif  // !_TEST_NFC_ISODEP_H_
