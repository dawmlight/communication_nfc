#ifndef TEST_NFC_READER_APP_H
#define TEST_NFC_READER_APP_H

#include <memory>
#include <string>
#include <vector>

#include "loghelper.h"
#include "ohos_application.h"

namespace OHOS {
namespace nfc {
namespace sdk {
class NfcAgent;
}
namespace test {
class TestNfcReaderApp : public AppExecFwk::OHOSApplication {
public:
    TestNfcReaderApp();
    ~TestNfcReaderApp();

    static std::shared_ptr<TestNfcReaderApp> GenAppliaction(const std::string& name);
    // Application function
    void OnStart() override;
    void OnTerminate() override;

    // Turn On Nfc
    void TurnOnNfc();
    // Turn Off Nfc
    void TurnOffNfc();

    std::shared_ptr<sdk::NfcAgent> GetNfcAgent();
    void SetNfcAgent(std::shared_ptr<sdk::NfcAgent> nfcAgent);

private:
    void SetInstance(std::shared_ptr<TestNfcReaderApp> ins)
    {
        ins_ = ins;
    }

private:
    std::shared_ptr<sdk::NfcAgent> mNFCAgent_;
    std::shared_ptr<TestNfcReaderApp> ins_;
};
}  // namespace test
}  // namespace nfc
}  // namespace OHOS
#endif /* TEST_NFC_READER_APP_H */
