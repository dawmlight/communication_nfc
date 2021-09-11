#ifndef DEVICE_HOST_MOCK_H
#define DEVICE_HOST_MOCK_H

#include <gmock/gmock.h>

#include "idevice_host.h"

class DeviceHostMock : public OHOS::nfc::ncibal::IDeviceHost {
public:
    DeviceHostMock() : OHOS::nfc::ncibal::IDeviceHost() {}
    using IDHListener = OHOS::nfc::ncibal::IDeviceHost::IDeviceHostListener;
    MOCK_METHOD1(SetDeviceHostListener, void(std::weak_ptr<IDHListener> listener));
    /**
     * @brief Turn on NFC
     * @return True if ok
     */
    MOCK_METHOD0(Initialize, bool());
    /**
     * @brief Turn off NFC
     * @return True if ok
     */
    MOCK_METHOD0(Deinitialize, bool());
    /**
     * @brief Start polling and listening
     * @param techMask bitmask of the technologies
     * @param enableReaderMode if enable tag polling
     * @param enableHostRouting if enable host routing
     * @param restart if need restart
     */
    MOCK_METHOD4(EnableDiscovery, void(int techMask, bool enableReaderMode, bool enableHostRouting, bool restart));
    /**
     * @brief Stop polling and listening
     */
    MOCK_METHOD0(DisableDiscovery, void());
    /**
     * @brief Send a raw frame
     * @param rawData raw frame
     * @return True if ok
     */
    MOCK_METHOD1(SendRawFrame, bool(std::string& rawData));
    /**
     * @brief Send screen statue
     * @param screenStateMask bitmask of the screen state
     * @return True if ok
     */
    MOCK_METHOD1(SetScreenStatus, bool(unsigned char screenStateMask));
    /**
     * @brief Get Nci version
     * @return Nci version
     */
    MOCK_METHOD0(GetNciVersion, int());
    /**
     * @brief Set Secure Nfc
     * @param secure if need secure nfc
     * @return True if ok
     */
    MOCK_METHOD1(SetSecureNfc, bool(bool secure));
    /**
     * @brief Get maximum tranceive length
     * @return max length
     */
    MOCK_METHOD0(GetIsoDepMaxTransceiveLength, int());
    MOCK_METHOD1(RegisterT3tIdentifier, int(std::string& lfT3tIdentifier));
    MOCK_METHOD1(DeregisterT3tIdentifier, void(std::string& lfT3tIdentifier));
    MOCK_METHOD0(ClearT3tIdentifiersCache, void());
    MOCK_METHOD0(GetLfT3tMax, int());
    MOCK_METHOD0(GetLastError, int());
    MOCK_METHOD0(Abort, void());
    /**
     * @brief Download firmware
     * @return True if ok
     */
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
#endif  // !DEVICE_HOST_MOCK_H