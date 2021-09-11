#ifndef TAG_END_POINT_MOCK_H
#define TAG_END_POINT_MOCK_H

#include <gmock/gmock.h>

#include "itag_end_point.h"

class TagEndPointMock : public OHOS::nfc::ncibal::ITagEndPoint {
public:
    TagEndPointMock() : OHOS::nfc::ncibal::ITagEndPoint() {}
    ~TagEndPointMock() = default;
    /**
     * @brief Connect the tag
     * @param technology the technology of the tag
     * @return True if ok
     */
    MOCK_METHOD1(Connect, bool(int technology));
    /**
     * @brief Disconnect the tag
     * @return True if ok
     */
    MOCK_METHOD0(Disconnect, bool());
    /**
     * @brief Reconnect the tag
     * @return True if ok
     */
    MOCK_METHOD0(Reconnect, bool());
    /**
     * @brief Send data to tag and receive response
     * @param request the technology of the tag
     * @param response response from the tag
     * @return 0 if ok
     */
    MOCK_METHOD2(Transceive, int(std::string& request, std::string& response));
    /**
     * @brief Check if the tag is in field
     * @return True if ok
     */
    MOCK_METHOD0(PresenceCheck, bool());
    MOCK_METHOD0(IsPresent, bool());
    MOCK_METHOD2(StartPresenceChecking, void(int presenceCheckDelay, TagDisconnectedCallBack callback));
    MOCK_METHOD0(StopPresenceChecking, void());
    MOCK_METHOD0(GetTechList, std::vector<int>());
    MOCK_METHOD1(RemoveTechnology, void(int technology));
    MOCK_METHOD0(GetUid, std::string());
    MOCK_METHOD0(GetHandle, int());
    MOCK_METHOD0(GetTechExtras, std::weak_ptr<OHOS::nfc::sdk::NfcMap>());
    MOCK_METHOD0(MakeReadOnly, bool());
    MOCK_METHOD0(ReadNdef, std::string());
    MOCK_METHOD1(WriteNdef, bool(std::string& data));
    MOCK_METHOD1(FormatNdef, bool(const std::string& key));
    MOCK_METHOD0(IsNdefFormatable, bool());
    MOCK_METHOD1(CheckNdef, bool(std::vector<int>& ndefInfo));
    MOCK_METHOD0(GetConnectedTechnology, int());
};
#endif  // !TAG_END_POINT_MOCK_H