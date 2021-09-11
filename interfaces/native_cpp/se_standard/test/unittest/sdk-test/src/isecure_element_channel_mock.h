#ifndef ISECURE_ELEMENT_CHANNEL_MOCK_H
#define ISECURE_ELEMENT_CHANNEL_MOCK_H

#include <gmock/gmock.h>

#include "iremote_object.h"
#include "isecure_element_channel.h"

namespace OHOS {
namespace se {
namespace omapi {
class ISecureElementChannelMock : public OHOS::se::omapi::ISecureElementChannel {
public:
    ISecureElementChannelMock() {}
    ~ISecureElementChannelMock() {}
    MOCK_METHOD0(Close, void());
    MOCK_METHOD0(IsClosed, bool());
    MOCK_METHOD0(IsBasicChannel, bool());
    MOCK_METHOD0(GetSelectResponse, std::string());
    MOCK_METHOD1(Transmit, std::string(std::string const&));
    MOCK_METHOD0(SelectNext, bool());
    MOCK_METHOD0(AsObject, sptr<OHOS::IRemoteObject>());
};
}  // namespace omapi
}  // namespace se
}  // namespace OHOS
#endif  // !ISECURE_ELEMENT_CHANNEL_MOCK_H
