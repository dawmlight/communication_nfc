#ifndef INFCSERVICE_H
#define INFCSERVICE_H

#include <gmock/gmock.h>

#include "infc_service.h"

class NfcServiceMock : public OHOS::nfc::INfcService {
public:
    NfcServiceMock() : nfc::INfcService() {}
    MOCK_METHOD0(IsNfcEnabled, bool());
    MOCK_METHOD1(ApplyDiscovering, void(bool));
    MOCK_METHOD1(PauseDiscovering, void(int));
    MOCK_METHOD0(IsScreenUnlock, bool());
    MOCK_METHOD1(IsInForeground, bool(int));
    MOCK_METHOD0(PowerActivity, void());
    MOCK_METHOD1(PlaySound, void(int));

    MOCK_METHOD0(GetDeviceHost, std::weak_ptr<nfc::ncibal::IDeviceHost>());
#ifdef _NFC_SERVICE_READER_
    MOCK_METHOD0(GetTagSession, std::weak_ptr<nfc::reader::ITagSession>());
    MOCK_METHOD0(GetTagDispatcher, std::weak_ptr<nfc::reader::TagDispatcher>());
    MOCK_METHOD0(GetMessageHandler, std::weak_ptr<osal::MessageHandler>());
#endif
};
#endif  // !INFCSERVICE_H
