#ifndef DEVICE_HOST_LISTENER_MOCK_H
#define DEVICE_HOST_LISTENER_MOCK_H

#include <gmock/gmock.h>

#include <memory>
#include <string>

#include "idevice_host.h"

class DeviceHostListenerMock : public OHOS::nfc::ncibal::IDeviceHost::IDeviceHostListener {
public:
    DeviceHostListenerMock() : OHOS::nfc::ncibal::IDeviceHost::IDeviceHostListener() {}
    MOCK_METHOD0(OnRemoteFieldActivated, void());
    MOCK_METHOD0(OnRemoteFieldDeactivated, void());
    MOCK_METHOD1(OnHostCardEmulationActivated, void(int technology));
    MOCK_METHOD1(OnHostCardEmulationDeactivated, void(int technology));
    MOCK_METHOD2(OnHostCardEmulationDataReceived, void(int technology, std::string& data));
    MOCK_METHOD1(OnTagDiscovered, void(std::shared_ptr<OHOS::nfc::ncibal::ITagEndPoint> tagEndPoint));
    MOCK_METHOD3(OnOffHostTransactionEvent, void(std::string& aid, std::string& data, std::string& seName));
    MOCK_METHOD0(OnEeUpdate, void());
};
#endif  // !DEVICE_HOST_LISTENER_MOCK_H