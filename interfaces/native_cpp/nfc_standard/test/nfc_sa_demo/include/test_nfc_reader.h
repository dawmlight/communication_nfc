#ifndef TEST_NFC_READER_H
#define TEST_NFC_READER_H

#include <memory>
#include <string>

namespace OHOS {
namespace nfc {
namespace sdk{
class NfcMap;
}
namespace test {
class TestNfcAbility;
class TestNfcReaderApp;
class TestReadModeTagCallBack;

class TestNfcReader {
public:
    virtual ~TestNfcReader() {}
    // Initailize Nfc Reader
    bool Initailize(int type = 0x0f);
    bool Initailize(std::shared_ptr<sdk::NfcMap> extras);
    bool Initailize(int type, std::shared_ptr<sdk::NfcMap> extras);
    bool Initailize(std::shared_ptr<TestReadModeTagCallBack> callback, int type, std::shared_ptr<sdk::NfcMap> extras);
    // De initialization    
    void DeInitailize();
    // Turn On Nfc
    void TurnOnNfc();
    // Turn Off Nfc    
    void TurnOffNfc();
    // Start up
    void StartAbility();
    void TerminalAbility();
    void ActiveAbility();
    void InactiveAbility();

    virtual bool StartTest() = 0;

protected:
    std::shared_ptr<TestReadModeTagCallBack> callback_;
private:
    std::shared_ptr<TestNfcReaderApp> application_{};
    std::shared_ptr<TestNfcAbility> ability_{};
};
}  // namespace test
}  // namespace nfc
}  // namespace OHOS
#endif /* TEST_NFC_READER_H */