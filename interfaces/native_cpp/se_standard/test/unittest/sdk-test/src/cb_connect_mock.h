#ifndef CB_CONNECT_MOCK_H
#define CB_CONNECT_MOCK_H

#include <gmock/gmock.h>

#include "se_service.h"

namespace OHOS::se {
namespace omapi {
class CBConnectMock : public OHOS::se::omapi::SEService::OnConnectedListener {
public:
    CBConnectMock(){};
    ~CBConnectMock(){};
    MOCK_METHOD0(CBConnected, void());
};
}  // namespace omapi
}  // namespace OHOS::se
#endif  // !CB_CONNECT_MOCK_H
