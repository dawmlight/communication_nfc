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
#ifndef TAG_SESSION_H
#define TAG_SESSION_H

#include <map>
#include <memory>
#include <mutex>
#include <vector>

#include "itag_session.h"
#include "tag_session_stub.h"

namespace osal {
class Context;
}

namespace OHOS {
namespace nfc::ncibal {
class IDeviceHost;
class ITagEndPoint;
}  // namespace nfc::ncibal
using nfc::ncibal::IDeviceHost;
using nfc::ncibal::ITagEndPoint;

namespace nfc {
class NfcService;
class INfcService;

namespace reader {
class TagDispatcher;

class TagSession final : public TagSessionStub {
public:
    // Constructor/Destructor
    TagSession(std::shared_ptr<nfc::INfcService> service, std::weak_ptr<osal::Context> context);
    ~TagSession();
    // copy/operator functions
    TagSession(const TagSession&) = delete;
    TagSession& operator=(const TagSession&) = delete;
    /**
     * @brief To connect the nativeHandle tag by technology.
     * @param nativeHandle the native handle of tag
     * @param technology the tag technology
     * @return the result to connect the tag
     */
    int Connect(int nativeHandle, int technology) override;
    /**
     * @brief To reconnect the nativeHandle tag.
     * @param nativeHandle the native handle of tag
     * @return the result to reconnect the tag
     */
    int Reconnect(int nativeHandle) override;
    /**
     * @brief To disconnect the nativeHandle tag.
     * @param nativeHandle the native handle of tag
     */
    void Disconnect(int nativeHandle) override;
    /**
     * @brief Get the TechList of the nativeHandle tag.
     * @param nativeHandle the native handle of tag
     * @return TechList
     */
    std::vector<int> GetTechList(int nativeHandle) override;
    /**
     * @brief Checking the nativeHandle tag is present.
     * @param nativeHandle the native handle of tag
     * @return true - Presnet; the other - No Presnet
     */
    bool IsPresent(int nativeHandle) override;
    /**
     * @brief Checking the nativeHandle tag is a Ndef Tag.
     * @param nativeHandle the native handle of tag
     * @return true - Ndef Tag; the other - No Ndef Tag
     */
    bool IsNdef(int nativeHandle) override;
    /**
     * @brief To send the data to the nativeHandle tag.
     * @param nativeHandle the native handle of tag
     * @param data the sent data
     * @param raw to send whether original data or un-original data
     * @return The response result from the End-Point tag
     */
    std::unique_ptr<ResResult> SendRawFrame(int nativeHandle, std::string data, bool raw) override;
    /**
     * @brief Reading from the End-Point tag
     * @param nativeHandle the native handle of tag
     * @return the read data
     */
    std::string NdefRead(int nativeHandle) override;
    /**
     * @brief Writing the data into the End-Point tag.
     * @param nativeHandle the native handle of tag
     * @param msg the wrote data
     * @return the Writing Result
     */
    int NdefWrite(int nativeHandle, std::string msg) override;
    /**
     * @brief Making the End-Point tag to read only.
     * @param nativeHandle the native handle of tag
     * @return the making result
     */
    int NdefMakeReadOnly(int nativeHandle) override;
    /**
     * @brief format the tag by Ndef
     * @param nativeHandle the native handle of tag
     * @param key the format key
     * @return the format result
     */
    int FormatNdef(int nativeHandle, const std::string& key) override;
    /**
     * @brief Checking the End-Point tag is Read only
     * @param technology the tag technology
     * @return true - ReadOnly; false - No Read Only
     */
    bool CanMakeReadOnly(int technology) override;
    /**
     * @brief Get Max Transceive Length
     * @param technology the tag technology
     * @return Max Transceive Length
     */
    int GetMaxTransceiveLength(int technology) override;
    /**
     * @brief Checking the DeviceHost whether It supported the extended Apdus
     * @return true - yes; false - no
     */
    bool IsSupportedApdusExtended() override;

private:
    //
    std::weak_ptr<nfc::INfcService> mNfcService_{};
    std::weak_ptr<IDeviceHost> mDeviceHost_{};
    std::weak_ptr<TagDispatcher> mDispatcher_{};
};
}  // namespace reader
}  // namespace nfc
}  // namespace OHOS
#endif  // !TAG_SESSION_H
