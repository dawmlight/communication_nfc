#ifndef ISECURE_ELEMENT_SERVICE_MOCK_H
#define ISECURE_ELEMENT_SERVICE_MOCK_H

#include <gmock/gmock.h>

#include "iremote_stub.h"
#include "isecure_element_service.h"

namespace OHOS {
namespace se {
namespace omapi {
class ISecureElementReader;
class ISecureElementServiceMock : public IRemoteStub<ISecureElementService> {
public:
    ISecureElementServiceMock() {}
    ~ISecureElementServiceMock() {}
    MOCK_METHOD0(GetReaders, std::vector<std::string>());
    MOCK_METHOD1(GetReader, std::shared_ptr<OHOS::se::omapi::ISecureElementReader>(std::string const&));
    MOCK_METHOD3(IsNfcEventAllowed,
                 std::vector<bool>(std::string const&, std::string const&, std::vector<std::string>));
    // MOCK_METHOD0(AsObject, sptr<OHOS::IRemoteObject>());
    // int OnRemoteRequest(uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option);
    MOCK_METHOD4(OnRemoteRequest, int(uint32_t, MessageParcel&, MessageParcel&, MessageOption&));
};
}  // namespace omapi
}  // namespace se
}  // namespace OHOS
#endif  // !ISECURE_ELEMENT_SERVICE_MOCK_H
