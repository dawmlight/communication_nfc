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
#ifndef I_TAG_SESSION_H
#define I_TAG_SESSION_H

#include <memory>
#include <string>
#include <vector>

#include "iremote_broker.h"
#include "parcel.h"

namespace OHOS {
namespace nfc {
namespace reader {
class ResResult : public OHOS::Parcelable {
public:
    ResResult() : result(0), resData("") {}
    virtual ~ResResult() {}

    bool Marshalling(OHOS::Parcel &parcel) const override{
        parcel.WriteInt32(result);
        parcel.WriteString(resData);
        return true;
    }
    static ResResult* Unmarshalling(OHOS::Parcel &parcel){
        ResResult* res = new ResResult();
        res->SetResult(parcel.ReadInt32());
        res->SetResData(parcel.ReadString());
        return res;
    }

    void SetResult(int32_t r)
    {
        result = r;
    }
    int32_t GetResult() const
    {
        return result;
    }
    void SetResData(const std::string data)
    {
        resData = data;
    }
    std::string GetResData() const
    {
        return resData;
    }
    enum ResponseResult { RESULT_SUCCESS = 0, RESULT_EXCEEDED_LENGTH, RESULT_TAGLOST, RESULT_FAILURE };

private:
    int32_t result;
    std::string resData;
};

class ITagSession : public OHOS::IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.nfc.reader.ITagSession");

    virtual ~ITagSession() {}
    /**
     * @brief To connect the nativeHandle tag by technology.
     * @param nativeHandle the native handle of tag
     * @param technology the tag technology
     * @return the result to connect the tag
     */
    virtual int Connect(int nativeHandle, int technology) = 0;
    /**
     * @brief To reconnect the nativeHandle tag.
     * @param nativeHandle the native handle of tag
     * @return the result to reconnect the tag
     */
    virtual int Reconnect(int nativeHandle) = 0;
    /**
     * @brief To disconnect the nativeHandle tag.
     * @param nativeHandle the native handle of tag
     */
    virtual void Disconnect(int nativeHandle) = 0;
    /**
     * @brief Get the TechList of the nativeHandle tag.
     * @param nativeHandle the native handle of tag
     * @return TechList
     */
    virtual std::vector<int> GetTechList(int nativeHandle) = 0;
    /**
     * @brief Checking the nativeHandle tag is present.
     * @param nativeHandle the native handle of tag
     * @return true - Presnet; the other - No Presnet
     */
    virtual bool IsPresent(int nativeHandle) = 0;
    /**
     * @brief Checking the nativeHandle tag is a Ndef Tag.
     * @param nativeHandle the native handle of tag
     * @return true - Ndef Tag; the other - No Ndef Tag
     */
    virtual bool IsNdef(int nativeHandle) = 0;
    /**
     * @brief To send the data to the nativeHandle tag.
     * @param nativeHandle the native handle of tag
     * @param data the sent data
     * @param raw to send whether original data or un-original data
     * @return The response result from the End-Point tag
     */
    virtual std::unique_ptr<ResResult> SendRawFrame(int nativeHandle, std::string data, bool raw) = 0;
    /**
     * @brief Reading from the End-Point tag
     * @param nativeHandle the native handle of tag
     * @return the read data
     */
    virtual std::string NdefRead(int nativeHandle) = 0;
    /**
     * @brief Writing the data into the End-Point tag.
     * @param nativeHandle the native handle of tag
     * @param msg the wrote data
     * @return the Writing Result
     */
    virtual int NdefWrite(int nativeHandle, std::string msg) = 0;
    /**
     * @brief Making the End-Point tag to read only.
     * @param nativeHandle the native handle of tag
     * @return the making result
     */
    virtual int NdefMakeReadOnly(int nativeHandle) = 0;
    /**
     * @brief format the tag by Ndef
     * @param nativeHandle the native handle of tag
     * @param key the format key
     * @return the format result
     */
    virtual int FormatNdef(int nativeHandle, const std::string& key) = 0;
    /**
     * @brief Checking the End-Point tag is Read only
     * @param technology the tag technology
     * @return true - ReadOnly; false - No Read Only
     */
    virtual bool CanMakeReadOnly(int technology) = 0;
    /**
     * @brief Get Max Transceive Length
     * @param technology the tag technology
     * @return Max Transceive Length
     */
    virtual int GetMaxTransceiveLength(int technology) = 0;
    /**
     * @brief Checking the DeviceHost whether It supported the extended Apdus
     * @return true - yes; false - no
     */
    virtual bool IsSupportedApdusExtended() = 0;

private:
};
}  // namespace reader
}  // namespace nfc
}  // namespace OHOS
#endif  // !I_TAG_SESSION_H
