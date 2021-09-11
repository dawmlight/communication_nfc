#ifndef ISECURE_ELEMENT_READER_MOCK_H
#define ISECURE_ELEMENT_READER_MOCK_H

#include <gmock/gmock.h>

#include "iremote_object.h"
#include "isecure_element_reader.h"

namespace OHOS {
namespace se {
namespace omapi {
class ISecureElementSession;
class ISecureElementReaderMock : public OHOS::se::omapi::ISecureElementReader {
public:
    ISecureElementReaderMock() {}
    ~ISecureElementReaderMock() {}
    MOCK_METHOD0(IsSecureElementPresent, bool());
    MOCK_METHOD0(OpenSession, std::shared_ptr<OHOS::se::omapi::ISecureElementSession>());
    MOCK_METHOD0(CloseSessions, void());
    MOCK_METHOD0(Reset, bool());
    MOCK_METHOD0(AsObject, sptr<OHOS::IRemoteObject>());
};
}  // namespace omapi
}  // namespace se
}  // namespace OHOS
#endif  // !ISECURE_ELEMENT_READER_MOCK_H
