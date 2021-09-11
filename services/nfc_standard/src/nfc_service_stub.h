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
#ifndef NFC_SERVICE_STUB_H
#define NFC_SERVICE_STUB_H

#include "infc_agent_service.h"
#include "iremote_stub.h"
#include "message_parcel.h"

namespace osal {
class Context;
}

namespace OHOS {
namespace nfc {
class NfcServiceStub : public OHOS::IRemoteStub<OHOS::nfc::INfcAgentService> {
public:
    int OnRemoteRequest(uint32_t code,                /* [in] */
                        OHOS::MessageParcel& data,    /* [in] */
                        OHOS::MessageParcel& reply,   /* [out] */
                        OHOS::MessageOption& option); /* [in] */

    NfcServiceStub(std::weak_ptr<osal::Context> context) : mContext_(context)
    {
    }
    virtual ~NfcServiceStub()
    {
    }

private:
    int HandleGetState(OHOS::MessageParcel& data, OHOS::MessageParcel& reply);
    int HandleTurnOn(OHOS::MessageParcel& data, OHOS::MessageParcel& reply);
    int HandleTurnOff(OHOS::MessageParcel& data, OHOS::MessageParcel& reply);
    int HandlePausePolling(OHOS::MessageParcel& data, OHOS::MessageParcel& reply);
    int HandleResumePolling(OHOS::MessageParcel& data, OHOS::MessageParcel& reply);
    int HandleSetNfcSecure(OHOS::MessageParcel& data, OHOS::MessageParcel& reply);
    int HandleDeviceSupportsNfcSecure(OHOS::MessageParcel& data, OHOS::MessageParcel& reply);
    int HandleIsNfcSecureEnabled(OHOS::MessageParcel& data, OHOS::MessageParcel& reply);
    int HandleSetRWMode(OHOS::MessageParcel& data, OHOS::MessageParcel& reply);
    int HandleSetIgnore(OHOS::MessageParcel& data, OHOS::MessageParcel& reply);
    int HandleSetFrontDispatch(OHOS::MessageParcel& data, OHOS::MessageParcel& reply);
    int HandleGetNfcTagInterface(OHOS::MessageParcel& data, OHOS::MessageParcel& reply);
    int HandleGetCardEmulationInterface(OHOS::MessageParcel& data, OHOS::MessageParcel& reply);

private:
    std::weak_ptr<osal::Context> mContext_{};

    static constexpr int UID_ID = 100;
    static constexpr int COMMAND_GET_STATE = UID_ID + 0;
    static constexpr int COMMAND_TURN_ON = UID_ID + 1;
    static constexpr int COMMAND_TURN_OFF = UID_ID + 2;
    static constexpr int COMMAND_PAUSE_POLLING = UID_ID + 3;
    static constexpr int COMMAND_RESUME_POLLING = UID_ID + 4;
    static constexpr int COMMAND_SET_NFC_SECURE = UID_ID + 5;
    static constexpr int COMMAND_DEVICE_SUPPORT_SECURE = UID_ID + 6;
    static constexpr int COMMAND_IS_SECURE_ENABLED = UID_ID + 7;
    static constexpr int COMMAND_SET_RW_MODE = UID_ID + 8;
    static constexpr int COMMAND_SET_IGNORE = UID_ID + 9;
    static constexpr int COMMAND_SET_FRONT_DISPATCH = UID_ID + 10;
    static constexpr int COMMAND_GET_TAG_INTERFACE = UID_ID + 11;
    static constexpr int COMMAND_GET_CARDEMULATION_INTERFACE = UID_ID + 12;
};
}  // namespace nfc
}  // namespace OHOS
#endif  // !NFC_SERVICE_STUB_H