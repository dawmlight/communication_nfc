#ifndef ITAGCALLBACKMOCK_H
#define ITAGCALLBACKMOCK_H

#include <gmock/gmock.h>

#include "nfc_agent.h"

namespace OHOS {
namespace nfc {
namespace sdk {
class Tag;
class TagCallbackMock : public NfcAgent::TagCallback {
public:
    MOCK_METHOD1(CBTagDiscoverCommond, void(std::unique_ptr<Tag> tag));
    MOCK_METHOD0(CBTagRemovedMsg, void());
};
}  // namespace sdk
}  // namespace nfc
}  // namespace OHOS
#endif  // !ITAGCALLBACKMOCK_H
