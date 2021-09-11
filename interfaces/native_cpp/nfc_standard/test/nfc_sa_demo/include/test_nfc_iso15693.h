#ifndef _TEST_NFC_ISO15693_H_
#define _TEST_NFC_ISO15693_H_

#include "test_nfc_reader.h"

namespace OHOS {
namespace nfc {
namespace sdk{
class Iso15693Tag;
}
namespace test {
class TestReadModeTagCallBack;

class TestNfcIso15693Tag : public TestNfcReader {
public:
    bool StartTest() override;

    // 发现 Iso15693Tag 标签
    bool TestIso15693TagDiscovered();

    // 验证正常读取 Iso15693Tag 一个块
    void TestReadIso15693TagSingleBlock();

    // 验证正常写 Iso15693Tag 一个块
    void TestWriteIso15693TagSingleBlock();
    
    // 验证正常读取 Iso15693Tag 多个块
    void TestReadIso15693TagMultiBlock();

    // 验证正常写 Iso15693Tag 多个块
    void TestWriteIso15693TagMultiBlock();
    
    // 验证正常锁 Iso15693Tag 一个块
    void TestLockIso15693TagSingleBlock();
    
private:
    std::shared_ptr<sdk::Iso15693Tag> ReadWriteIso15693TagSetUp();
};
}  // namespace test
}  // namespace nfc
}  // namespace OHOS
#endif  // !_TEST_NFC_ISO15693_H_
