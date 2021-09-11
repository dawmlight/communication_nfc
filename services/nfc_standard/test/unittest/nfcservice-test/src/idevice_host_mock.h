#ifndef IDEVICE_HOST_MOCK_H
#define IDEVICE_HOST_MOCK_H

#include <gmock/gmock.h>

#include "idevice_host.h"

class DeviceHostMock : public OHOS::nfc::ncibal::IDeviceHost {
public:
    MOCK_METHOD0(Initialize, bool());
    MOCK_METHOD0(Deinitialize, bool());
    MOCK_METHOD4(EnableDiscovery, void(int techMask, bool enableReaderMode, bool enableHostRouting, bool restart));
    MOCK_METHOD0(DisableDiscovery, void());
    MOCK_METHOD1(SendRawFrame, bool(std::string& rawData));
    MOCK_METHOD1(SetScreenStatus, bool(unsigned char screenStateMask));
    MOCK_METHOD0(GetNciVersion, int());
    MOCK_METHOD1(SetSecureNfc, bool(bool secure));
    MOCK_METHOD0(GetIsoDepMaxTransceiveLength, int());
    MOCK_METHOD1(RegisterT3tIdentifier, int(std::string& lfT3tIdentifier));
    MOCK_METHOD1(DeregisterT3tIdentifier, void(std::string& lfT3tIdentifier));
    MOCK_METHOD0(ClearT3tIdentifiersCache, void());
    MOCK_METHOD0(GetLfT3tMax, int());
    MOCK_METHOD0(GetLastError, int());
    MOCK_METHOD0(Abort, void());
    MOCK_METHOD0(CheckFirmware, bool());
    MOCK_METHOD1(Dump, void(int fd));
    MOCK_METHOD0(FactoryReset, void());
    MOCK_METHOD0(Shutdown, void());
    MOCK_METHOD3(AddAidRouting, bool(std::string& aid, int route, int aidInfo));
    MOCK_METHOD1(RemoveAidRouting, bool(std::string& aid));
    MOCK_METHOD0(CommitRouting, bool());
    MOCK_METHOD0(GetAidRoutingTableSize, int());
    MOCK_METHOD0(GetDefaultRoute, int());
    MOCK_METHOD0(GetDefaultOffHostRoute, int());
    MOCK_METHOD0(GetOffHostUiccRoute, int());
    MOCK_METHOD0(GetOffHostEseRoute, int());
    MOCK_METHOD0(GetAidMatchingMode, int());
    MOCK_METHOD0(GetDefaultIsoDepRouteDestination, int());
    MOCK_METHOD1(CanMakeReadOnly, bool(int ndefType));
    MOCK_METHOD0(GetExtendedLengthApdusSupported, bool());
};

#endif  // !IDEVICE_HOST_MOCK_H
