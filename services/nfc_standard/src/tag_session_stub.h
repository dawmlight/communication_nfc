/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef TAG_SESSION_STUB_H
#define TAG_SESSION_STUB_H

#include "iremote_stub.h"
#include "itag_session.h"
#include "message_parcel.h"

namespace osal {
class Context;
}
namespace OHOS {
namespace nfc {
namespace reader {
class TagSessionStub : public OHOS::IRemoteStub<OHOS::nfc::reader::ITagSession> {
public:
    int OnRemoteRequest(uint32_t code,                         /* [in] */
                        OHOS::MessageParcel& data,             /* [in] */
                        OHOS::MessageParcel& reply,            /* [out] */
                        OHOS::MessageOption& option) override; /* [in] */

    TagSessionStub(std::weak_ptr<osal::Context> context) : mContext_(context) {}
    virtual ~TagSessionStub() {}

private:
    int HandleConnect(OHOS::MessageParcel& data, OHOS::MessageParcel& reply);
    int HandleReconnect(OHOS::MessageParcel& data, OHOS::MessageParcel& reply);
    int HandleDisconnect(OHOS::MessageParcel& data, OHOS::MessageParcel& reply);
    int HandleGetTechList(OHOS::MessageParcel& data, OHOS::MessageParcel& reply);
    int HandleIsPresent(OHOS::MessageParcel& data, OHOS::MessageParcel& reply);
    int HandleIsNdef(OHOS::MessageParcel& data, OHOS::MessageParcel& reply);
    int HandleSendRawFrame(OHOS::MessageParcel& data, OHOS::MessageParcel& reply);
    int HandleNdefRead(OHOS::MessageParcel& data, OHOS::MessageParcel& reply);
    int HandleNdefWrite(OHOS::MessageParcel& data, OHOS::MessageParcel& reply);
    int HandleNdefMakeReadOnly(OHOS::MessageParcel& data, OHOS::MessageParcel& reply);
    int HandleFormatNdef(OHOS::MessageParcel& data, OHOS::MessageParcel& reply);
    int HandleCanMakeReadOnly(OHOS::MessageParcel& data, OHOS::MessageParcel& reply);
    int HandleGetMaxTransceiveLength(OHOS::MessageParcel& data, OHOS::MessageParcel& reply);
    int HandleIsSupportedApdusExtended(OHOS::MessageParcel& data, OHOS::MessageParcel& reply);

private:
    std::weak_ptr<osal::Context> mContext_{};

    static constexpr int TAG_SESSION_START_ID = 200;
    static constexpr int COMMAND_CONNECT = TAG_SESSION_START_ID + 0;
    static constexpr int COMMAND_RECONNECT = TAG_SESSION_START_ID + 1;
    static constexpr int COMMAND_DISCONNECT = TAG_SESSION_START_ID + 2;
    static constexpr int COMMAND_GET_TECHLIST = TAG_SESSION_START_ID + 3;
    static constexpr int COMMAND_IS_PRESENT = TAG_SESSION_START_ID + 4;
    static constexpr int COMMAND_IS_NDEF = TAG_SESSION_START_ID + 5;
    static constexpr int COMMAND_SEND_RAW_FRAME = TAG_SESSION_START_ID + 6;
    static constexpr int COMMAND_NDEF_READ = TAG_SESSION_START_ID + 7;
    static constexpr int COMMAND_NDEF_WRITE = TAG_SESSION_START_ID + 8;
    static constexpr int COMMAND_NDEF_MAKE_READ_ONLY = TAG_SESSION_START_ID + 9;
    static constexpr int COMMAND_FORMAT_NDEF = TAG_SESSION_START_ID + 10;
    static constexpr int COMMAND_CAN_MAKE_READ_ONLY = TAG_SESSION_START_ID + 11;
    static constexpr int COMMAND_GET_MAX_TRANSCEIVE_LENGTH = TAG_SESSION_START_ID + 12;
    static constexpr int COMMAND_IS_SUPPORTED_APDUS_EXTENDED = TAG_SESSION_START_ID + 13;
};
}  // namespace reader
}  // namespace nfc
}  // namespace OHOS
#endif  // !TAG_SESSION_STUB_H