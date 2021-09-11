#ifndef _TEST_NFC_NDEF_H_
#define _TEST_NFC_NDEF_H_

#include "test_nfc_reader.h"

namespace OHOS {
namespace nfc {
namespace sdk{
class NdefTag;
}
namespace test {
class TestReadModeTagCallBack;

class TestNfcNdefTag : public TestNfcReader {
public:
    bool StartTest() override;
    // 发现NdefTag标签
    bool TestNdefTagDiscovered();

    // 验证正常写Ndef tag uri
    void TestWriteNdefTagWithUri();
private:
    std::shared_ptr<sdk::NdefTag> ReadWriteNdefTagSetUp();
};
}  // namespace test
}  // namespace nfc
}  // namespace OHOS
#endif  // !_TEST_NFC_NDEF_H_
