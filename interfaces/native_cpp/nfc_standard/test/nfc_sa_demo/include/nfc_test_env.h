#ifndef _NFC_TEST_ENV_H_
#define _NFC_TEST_ENV_H_

#include <memory>

namespace osal {
class Context;
// class IBroadcastReceiver;
}  // namespace osal

namespace OHOS {
namespace EventFwk {
class CommonEventSubscriber;
}
namespace nfc {
namespace test {
class NfcTestEnv {
public:
    NfcTestEnv() = default;
    ~NfcTestEnv();
    void Initialize();
    void SendSystemBoardcast(std::string action);
    void SetScreenOn(bool state);
    void SetKeyguardLocked(bool status);

private:
    std::shared_ptr<osal::Context> mContext_{};
    std::shared_ptr<OHOS::EventFwk::CommonEventSubscriber> mReceiver_{};
};
}  // namespace test
}  // namespace nfc
}  // namespace OHOS
#endif /* _NFC_TEST_STARTER_H_ */