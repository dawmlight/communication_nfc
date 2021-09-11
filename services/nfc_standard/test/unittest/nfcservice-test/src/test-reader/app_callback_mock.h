#ifndef APP_CALLBACK_MOCK_H
#define APP_CALLBACK_MOCK_H

#include <gmock/gmock.h>

#include <memory>

#include "iapp_callback.h"
#include "iremote_object.h"

namespace OHOS {
namespace nfc {
namespace sdk {
class Tag;
class AppCallbackMock : public IAppCallback {
public:
    AppCallbackMock() : IAppCallback() {}
    MOCK_METHOD1(OnTagDiscovered, void(std::unique_ptr<Tag> tag));
    MOCK_METHOD0(AsObject, sptr<IRemoteObject>());
};

class TagRemovedCallbackMock : public ITagRemovedCallback {
public:
    TagRemovedCallbackMock() : ITagRemovedCallback() {}
    MOCK_METHOD0(OnTagRemoved, void());
    MOCK_METHOD0(AsObject, sptr<IRemoteObject>());
};
}  // namespace sdk
}  // namespace nfc
}  // namespace OHOS
#endif  // !APP_CALLBACK_MOCK_H