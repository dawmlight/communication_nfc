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
#ifndef TAG_SESSION_PROXY_H
#define TAG_SESSION_PROXY_H

#include <memory>
#include <string>
#include <vector>

#include "iremote_proxy.h"
#include "itag_session.h"
#include "nfc_basic_proxy.h"

namespace OHOS {
namespace nfc {
namespace reader {
class TagSessionProxy final : public OHOS::IRemoteProxy<ITagSession>, public NfcBasicProxy {
public:
    explicit TagSessionProxy(const OHOS::sptr<OHOS::IRemoteObject>& remote)
        : OHOS::IRemoteProxy<ITagSession>(remote), NfcBasicProxy(remote)
    {
    }
    ~TagSessionProxy() {}

    int Connect(int nativeHandle, int technology) override;
    int Reconnect(int nativeHandle) override;
    void Disconnect(int nativeHandle) override;
    std::vector<int> GetTechList(int nativeHandle) override;
    bool IsPresent(int nativeHandle) override;
    bool IsNdef(int nativeHandle) override;
    std::unique_ptr<ResResult> SendRawFrame(int nativeHandle, std::string data, bool raw) override;
    std::string NdefRead(int nativeHandle) override;
    int NdefWrite(int nativeHandle, std::string msg) override;
    int NdefMakeReadOnly(int nativeHandle) override;
    int FormatNdef(int nativeHandle, const std::string& key) override;
    bool CanMakeReadOnly(int technology) override;
    int GetMaxTransceiveLength(int technology) override;
    bool IsSupportedApdusExtended() override;

private:
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
    static constexpr int COMMAND_GET_MAX_TRANSCEIVE_LENGTH = TAG_SESSION_START_ID + 10;
    static constexpr int COMMAND_IS_SUPPORTED_APDUS_EXTENDED = TAG_SESSION_START_ID + 11;
};
}  // namespace reader
}  // namespace nfc
}  // namespace OHOS
#endif  // !TAG_SESSION_PROXY_H