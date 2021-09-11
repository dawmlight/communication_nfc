#ifndef ISECURE_ELEMENT_SESSION_MOCK_H
#define ISECURE_ELEMENT_SESSION_MOCK_H

#include <gmock/gmock.h>

#include "iremote_object.h"
#include "isecure_element_session.h"

namespace OHOS {
namespace se {
namespace omapi {
class ISecureElementChannel;
class ISecureElementSessionMock : public OHOS::se::omapi::ISecureElementSession {
public:
    ISecureElementSessionMock() {}
    ~ISecureElementSessionMock() {}
    MOCK_METHOD0(GetATR, std::string());
    MOCK_METHOD0(Close, void());
    MOCK_METHOD0(IsClosed, bool());
    MOCK_METHOD0(CloseChannels, void());
    MOCK_METHOD3(OpenBasicChannel,
                 std::shared_ptr<OHOS::se::omapi::ISecureElementChannel>(std::string const&,
                                                                         int,
                                                                         OHOS::sptr<OHOS::IRemoteObject>));
    MOCK_METHOD3(OpenLogicalChannel,
                 std::shared_ptr<OHOS::se::omapi::ISecureElementChannel>(std::string const&,
                                                                         int,
                                                                         OHOS::sptr<OHOS::IRemoteObject>));
    MOCK_METHOD0(AsObject, sptr<OHOS::IRemoteObject>());
};
}  // namespace omapi
}  // namespace se
}  // namespace OHOS
#endif  // !ISECURE_ELEMENT_SESSION_MOCK_H