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
#include "tag_session.h"

#include "idevice_host.h"
#include "infc_service.h"
#include "itag_end_point.h"
#include "loghelper.h"
#include "nfc_service_define.h"
#include "tag_dispatcher.h"

using namespace std;
namespace OHOS {
namespace nfc {
namespace reader {
/**
 * @brief To connect the nativeHandle tag by technology.
 * @param nativeHandle the native handle of tag
 * @param technology the tag technology
 * @return the result to connect the tag
 */
int TagSession::Connect(int nativeHandle, int technology)
{
    if (!mNfcService_.lock()->IsNfcEnabled()) {
        return nfc::sdk::NfcErrorCode::NFC_SER_ERROR_NOT_INITIALIZED;
    }

    /* find the tag in the hmap */
    std::weak_ptr<ITagEndPoint> tag = mDispatcher_.lock()->FindObject(nativeHandle);
    if (tag.expired()) {
        return nfc::sdk::NfcErrorCode::NFC_SER_ERROR_DISCONNECT;
    }

    if (!tag.lock()->IsPresent()) {
        return nfc::sdk::NfcErrorCode::NFC_SER_ERROR_DISCONNECT;
    }

    // Note that on most tags, all technologies are behind a single
    // handle. This means that the connect at the lower levels
    // will do nothing, as the tag is already connected to that handle.
    if (tag.lock()->Connect(technology)) {
        return nfc::sdk::NfcErrorCode::NFC_SUCCESS;
    } else {
        return nfc::sdk::NfcErrorCode::NFC_SER_ERROR_DISCONNECT;
    }
}
/**
 * @brief To reconnect the nativeHandle tag.
 * @param nativeHandle the native handle of tag
 * @return the result to reconnect the tag
 */
int TagSession::Reconnect(int nativeHandle)
{
    // Check if NFC is enabled
    if (!mNfcService_.lock()->IsNfcEnabled()) {
        return nfc::sdk::NfcErrorCode::NFC_SER_ERROR_NOT_INITIALIZED;
    }

    /* find the tag in the hmap */
    std::weak_ptr<ITagEndPoint> tag = mDispatcher_.lock()->FindObject(nativeHandle);
    if (tag.expired()) {
        return nfc::sdk::NfcErrorCode::NFC_SER_ERROR_DISCONNECT;
    }

    if (tag.lock()->Reconnect()) {
        return nfc::sdk::NfcErrorCode::NFC_SUCCESS;
    } else {
        return nfc::sdk::NfcErrorCode::NFC_SER_ERROR_DISCONNECT;
    }
}
/**
 * @brief To disconnect the nativeHandle tag.
 * @param nativeHandle the native handle of tag
 */
void TagSession::Disconnect(int nativeHandle)
{
    // Check if NFC is enabled
    if (!mNfcService_.lock()->IsNfcEnabled()) {
        return;
    }

    /* find the tag in the hmap */
    std::weak_ptr<ITagEndPoint> tag = mDispatcher_.lock()->FindObject(nativeHandle);
    if (tag.expired()) {
        return;
    }
    tag.lock()->Disconnect();
}
/**
 * @brief Get the TechList of the nativeHandle tag.
 * @param nativeHandle the native handle of tag
 * @return TechList
 */
std::vector<int> TagSession::GetTechList(int nativeHandle)
{
    std::vector<int> techList;
    // Check if NFC is enabled
    if (!mNfcService_.lock()->IsNfcEnabled()) {
        return techList;
    }

    /* find the tag in the hmap */
    std::weak_ptr<ITagEndPoint> tag = mDispatcher_.lock()->FindObject(nativeHandle);
    if (tag.expired()) {
        return techList;
    }
    return tag.lock()->GetTechList();
}
/**
 * @brief Checking the nativeHandle tag is present.
 * @param nativeHandle the native handle of tag
 * @return true - Presnet; the other - No Presnet
 */
bool TagSession::IsPresent(int nativeHandle)
{
    // Check if NFC is enabled
    if (!mNfcService_.lock()->IsNfcEnabled()) {
        return false;
    }

    /* find the tag in the hmap */
    std::weak_ptr<ITagEndPoint> tag = mDispatcher_.lock()->FindObject(nativeHandle);
    if (tag.expired()) {
        return false;
    }
    return tag.lock()->IsPresent();
}
/**
 * @brief Checking the nativeHandle tag is a Ndef Tag.
 * @param nativeHandle the native handle of tag
 * @return true - Ndef Tag; the other - No Ndef Tag
 */
bool TagSession::IsNdef(int nativeHandle)
{
    // Check if NFC is enabled
    if (!mNfcService_.lock()->IsNfcEnabled()) {
        return false;
    }

    /* find the tag in the hmap */
    std::weak_ptr<ITagEndPoint> tag = mDispatcher_.lock()->FindObject(nativeHandle);
    if (tag.expired()) {
        return false;
    }
    std::vector<int> ndefInfo;
    return tag.lock()->CheckNdef(ndefInfo);
}
/**
 * @brief To send the data to the nativeHandle tag.
 * @param nativeHandle the native handle of tag
 * @param data the sent data
 * @param raw to send whether original data or un-original data
 * @return The response result from the End-Point tag
 */
std::unique_ptr<ResResult> TagSession::SendRawFrame(int nativeHandle, std::string data, bool raw)
{
    DebugLog("Send Raw(%d) Frame", raw);
    // Check if NFC is enabled
    if (!mNfcService_.lock()->IsNfcEnabled()) {
        return nullptr;
    }

    /* find the tag in the hmap */
    std::weak_ptr<ITagEndPoint> tag = mDispatcher_.lock()->FindObject(nativeHandle);
    if (tag.expired()) {
        return nullptr;
    }
    std::unique_ptr<ResResult> resResult = std::make_unique<ResResult>();
    // Check if length is within limits
    if (data.length() > static_cast<unsigned int>(GetMaxTransceiveLength(tag.lock()->GetConnectedTechnology()))) {
        resResult->SetResult(ResResult::RESULT_EXCEEDED_LENGTH);
        return resResult;
    }
    std::string response;
    int result = tag.lock()->Transceive(data, response);
    if (!response.empty()) {
        resResult->SetResult(ResResult::RESULT_SUCCESS);
    } else if (result == 1) {  // result == 1 means that Tag lost
        resResult->SetResult(ResResult::RESULT_TAGLOST);
    } else {
        resResult->SetResult(ResResult::RESULT_FAILURE);
    }
    resResult->SetResData(response);
    return resResult;
}
/**
 * @brief Reading from the End-Point tag
 * @param nativeHandle the native handle of tag
 * @return the read data
 */
std::string TagSession::NdefRead(int nativeHandle)
{
    // Check if NFC is enabled
    if (!mNfcService_.lock()->IsNfcEnabled()) {
        return "";
    }
    /* find the tag in the hmap */
    std::weak_ptr<ITagEndPoint> tag = mDispatcher_.lock()->FindObject(nativeHandle);
    if (tag.expired()) {
        return "";
    }
    return tag.lock()->ReadNdef();
}
/**
 * @brief Writing the data into the End-Point tag.
 * @param nativeHandle the native handle of tag
 * @param msg the wrote data
 * @return the Writing Result
 */
int TagSession::NdefWrite(int nativeHandle, std::string msg)
{
    // Check if NFC is enabled
    if (!mNfcService_.lock()->IsNfcEnabled()) {
        return nfc::sdk::NfcErrorCode::NFC_SER_ERROR_NOT_INITIALIZED;
    }

    /* find the tag in the hmap */
    std::weak_ptr<ITagEndPoint> tag = mDispatcher_.lock()->FindObject(nativeHandle);
    if (tag.expired()) {
        return nfc::sdk::NfcErrorCode::NFC_SER_ERROR_IO;
    }

    if (msg.empty()) {
        return nfc::sdk::NfcErrorCode::NFC_SER_ERROR_INVALID_PARAM;
    }

    if (tag.lock()->WriteNdef(msg)) {
        return nfc::sdk::NfcErrorCode::NFC_SUCCESS;
    }
    return nfc::sdk::NfcErrorCode::NFC_SER_ERROR_IO;
}
/**
 * @brief Making the End-Point tag to read only.
 * @param nativeHandle the native handle of tag
 * @return the making result
 */
int TagSession::NdefMakeReadOnly(int nativeHandle)
{
    // Check if NFC is enabled
    if (!mNfcService_.lock()->IsNfcEnabled()) {
        return nfc::sdk::NfcErrorCode::NFC_SER_ERROR_NOT_INITIALIZED;
    }

    /* find the tag in the hmap */
    std::weak_ptr<ITagEndPoint> tag = mDispatcher_.lock()->FindObject(nativeHandle);
    if (tag.expired()) {
        return nfc::sdk::NfcErrorCode::NFC_SER_ERROR_IO;
    }

    if (tag.lock()->MakeReadOnly()) {
        return nfc::sdk::NfcErrorCode::NFC_SUCCESS;
    }
    return nfc::sdk::NfcErrorCode::NFC_SER_ERROR_IO;
}
/**
 * @brief format the tag by Ndef
 * @param nativeHandle the native handle of tag
 * @param key the format key
 * @return the format result
 */
int TagSession::FormatNdef(int nativeHandle, const std::string& key)
{
    // Check if NFC is enabled
    if (!mNfcService_.lock()->IsNfcEnabled()) {
        return nfc::sdk::NfcErrorCode::NFC_SER_ERROR_NOT_INITIALIZED;
    }

    /* find the tag in the hmap */
    std::weak_ptr<nfc::ncibal::ITagEndPoint> tag = mDispatcher_.lock()->FindObject(nativeHandle);
    if (tag.expired()) {
        return nfc::sdk::NfcErrorCode::NFC_SER_ERROR_IO;
    }

    if (tag.lock()->FormatNdef(key)) {
        return nfc::sdk::NfcErrorCode::NFC_SUCCESS;
    }
    return nfc::sdk::NfcErrorCode::NFC_SER_ERROR_IO;
}
/**
 * @brief Checking the End-Point tag is Read only
 * @param technology the tag technology
 * @return true - ReadOnly; false - No Read Only
 */
bool TagSession::CanMakeReadOnly(int technology)
{
    return mDeviceHost_.lock()->CanMakeReadOnly(technology);
}
/**
 * @brief Get Max Transceive Length
 * @param technology the tag technology
 * @return Max Transceive Length
 */
int TagSession::GetMaxTransceiveLength(int technology)
{
    return mDeviceHost_.lock()->GetIsoDepMaxTransceiveLength();
}
/**
 * @brief Checking the DeviceHost whether It supported the extended Apdus
 * @return true - yes; false - no
 */
bool TagSession::IsSupportedApdusExtended()
{
    return mDeviceHost_.lock()->GetExtendedLengthApdusSupported();
}

TagSession::TagSession(std::shared_ptr<INfcService> service, std::weak_ptr<osal::Context> context)
    : TagSessionStub(context), mNfcService_(service)
{
    mDeviceHost_ = service->GetDeviceHost();
    mDispatcher_ = service->GetTagDispatcher();
}

TagSession::~TagSession() {}
}  // namespace reader
}  // namespace nfc
}  // namespace OHOS