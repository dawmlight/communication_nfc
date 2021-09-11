#ifndef ITAGSESSIONMOCK_H
#define ITAGSESSIONMOCK_H

#include <gmock/gmock.h>

#include "iremote_object.h"
#include "itag_session.h"

namespace OHOS {
namespace nfc {
namespace sdk {
class ITagSessionMock : public OHOS::nfc::reader::ITagSession {
public:
    ITagSessionMock() : OHOS::nfc::reader::ITagSession() {}
    MOCK_METHOD2(Connect, int(int nativeHandle, int technology));
    MOCK_METHOD1(Reconnect, int(int nativeHandle));
    MOCK_METHOD1(Disconnect, void(int nativeHandle));
    MOCK_METHOD1(GetTechList, std::vector<int>(int nativeHandle));
    MOCK_METHOD1(IsPresent, bool(int nativeHandle));
    MOCK_METHOD1(IsNdef, bool(int nativeHandle));
    MOCK_METHOD3(SendRawFrame,
                 std::unique_ptr<OHOS::nfc::reader::ResResult>(int nativeHandle, std::string data, bool raw));
    MOCK_METHOD1(NdefRead, std::string(int nativeHandle));
    MOCK_METHOD2(NdefWrite, int(int nativeHandle, std::string msg));
    MOCK_METHOD1(NdefMakeReadOnly, int(int nativeHandle));
    MOCK_METHOD2(FormatNdef, int(int nativeHandle, const std::string& key));
    MOCK_METHOD1(CanMakeReadOnly, bool(int nativeHandle));
    MOCK_METHOD1(GetMaxTransceiveLength, int(int nativeHandle));
    MOCK_METHOD0(IsSupportedApdusExtended, bool());
    MOCK_METHOD0(AsObject, sptr<IRemoteObject>());
};
}  // namespace sdk
}  // namespace nfc
}  // namespace OHOS
#endif  // !ITAGSESSIONMOCK_H
