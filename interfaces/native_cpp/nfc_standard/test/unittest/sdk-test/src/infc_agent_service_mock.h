#ifndef INFC_AGENT_SERVICE_MOCK_H
#define INFC_AGENT_SERVICE_MOCK_H

#include <gmock/gmock.h>

#include "infc_agent_service.h"
#include "iremote_object.h"

namespace OHOS {
namespace nfc {
namespace reader {
class ITagSession;
}

namespace sdk {
class NfcAgent;
class IAppCallback;

class INfcAgentServiceMock : public OHOS::nfc::INfcAgentService {
public:
    INfcAgentServiceMock() : OHOS::nfc::INfcAgentService() {}
    MOCK_METHOD0(GetState, int());
    MOCK_METHOD0(TurnOn, bool());
    MOCK_METHOD1(TurnOff, bool(bool saveState));
    MOCK_METHOD1(PausePolling, void(int timeoutInMs));
    MOCK_METHOD0(ResumePolling, void());
    MOCK_METHOD1(SetNfcSecure, bool(bool enable));
    MOCK_METHOD0(DeviceSupportsNfcSecure, bool());
    MOCK_METHOD0(IsNfcSecureEnabled, bool());
    MOCK_METHOD4(SetRWMode,
                 void(const sptr<IRemoteObject>& readerObj,
                      std::weak_ptr<OHOS::nfc::sdk::IAppCallback> appCallback,
                      int flags,
                      std::weak_ptr<sdk::NfcMap> extras));
    MOCK_METHOD3(SetIgnore,
                 bool(int handle, int debounceMs, std::weak_ptr<OHOS::nfc::sdk::ITagRemovedCallback> callback));
    MOCK_METHOD0(SetFrontDispatch, void());
    MOCK_METHOD0(GetNfcTagInterface, std::weak_ptr<OHOS::nfc::reader::ITagSession>());
    MOCK_METHOD0(GetNfcCardEmulationInterface, std::shared_ptr<OHOS::nfc::sdk::cardemulation::ICardEmulationAgent>());

    MOCK_METHOD0(AsObject, sptr<IRemoteObject>());
};
}  // namespace sdk
}  // namespace nfc
}  // namespace OHOS
#endif  // !INFC_AGENT_SERVICE_MOCK_H
