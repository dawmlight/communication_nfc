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
#ifndef NDEF_MESSAGE_H
#define NDEF_MESSAGE_H

#include <array>
#include <iostream>
#include <memory>
#include <string>
#include <tuple>
#include <vector>

namespace OHOS {
namespace nfc {
namespace sdk {
// record data
struct NdefRecord {
    size_t mTnf_;
    std::string mId_;
    std::string mPayload_;
    std::string mTagRtdType_;
};

// URI charactor code defined by NFC Forum
static const size_t MAX_URI_CODE_NUM = 0x24;
static std::array<std::string, MAX_URI_CODE_NUM> gUriPrefix = {
    "",                            // NFC Forum define value: 0x00
    "http://www.",                 // NFC Forum define value: 0x01
    "https://www.",                // NFC Forum define value: 0x02
    "http://",                     // NFC Forum define value: 0x03
    "https://",                    // NFC Forum define value: 0x04
    "tel:",                        // NFC Forum define value: 0x05
    "mailto:",                     // NFC Forum define value: 0x06
    "ftp://anonymous:anonymous@",  // NFC Forum define value: 0x07
    "ftp://ftp.",                  // NFC Forum define value: 0x08
    "ftps://",                     // NFC Forum define value: 0x09
    "sftp://",                     // NFC Forum define value: 0x0A
    "smb://",                      // NFC Forum define value: 0x0B
    "nfs://",                      // NFC Forum define value: 0x0C
    "ftp://",                      // NFC Forum define value: 0x0D
    "dav://",                      // NFC Forum define value: 0x0E
    "news:",                       // NFC Forum define value: 0x0F
    "telnet://",                   // NFC Forum define value: 0x10
    "imap:",                       // NFC Forum define value: 0x11
    "rtsp://",                     // NFC Forum define value: 0x12
    "urn:",                        // NFC Forum define value: 0x13
    "pop:",                        // NFC Forum define value: 0x14
    "sip:",                        // NFC Forum define value: 0x15
    "sips:",                       // NFC Forum define value: 0x16
    "tftp:",                       // NFC Forum define value: 0x17
    "btspp://",                    // NFC Forum define value: 0x18
    "btl2cap://",                  // NFC Forum define value: 0x19
    "btgoep://",                   // NFC Forum define value: 0x1A
    "tcpobex://",                  // NFC Forum define value: 0x1B
    "irdaobex://",                 // NFC Forum define value: 0x1C
    "file://",                     // NFC Forum define value: 0x1D
    "urn:epc:id:",                 // NFC Forum define value: 0x1E
    "urn:epc:tag:",                // NFC Forum define value: 0x1F
    "urn:epc:pat:",                // NFC Forum define value: 0x20
    "urn:epc:raw:",                // NFC Forum define value: 0x21
    "urn:epc:",                    // NFC Forum define value: 0x22
    "urn:nfc:",                    // NFC Forum define value: 0x23
};

class NdefMessage final {
public:
    const int MAX_RTD_TYPE_LEN = 2;
    static const long int MAX_PAYLOAD_ARRAY = 10 * (1 << 20);  // 10M payload limit
    static const int MAX_PAYLOAD_SIZE = 256;
    // TNF Type define
    enum EmTnfType {
        TNF_EMPTY = 0x00,
        TNF_WELL_KNOWN = 0x01,
        TNF_MIME_MEDIA = 0x02,
        TNF_ABSOLUTE_URI = 0x03,
        TNF_EXTERNAL_TYPE = 0x04,
        TNF_UNKNOWN = 0x05,
        TNF_UNCHANGED = 0x06,
        TNF_RESERVED = 0x07
    };

    // record Flag
    enum EmRecordFlag {
        FLAG_MB = 0x80,
        FLAG_ME = 0x40,
        FLAG_CF = 0x20,
        FLAG_SR = 0x10,
        FLAG_IL = 0x08,
        FLAG_TNF = 0x07
    };

    enum EmRtdType {
        RTD_INVALID,
        RTD_TEXT,
        RTD_URI,
        RTD_SMART_POSTER,
        RTD_ALTERNATIVE_CARRIER,
        RTD_HANDOVER_CARRIER,
        RTD_HANDOVER_REQUEST,
        RTD_HANDOVER_SELECT,
        RTD_OHOS_APP
    };

public:
    NdefMessage(std::vector<std::shared_ptr<NdefRecord>> ndefRecords);
    ~NdefMessage();

    /**
     * @Description constructe a ndef message with raw bytes.
     * @param data raw bytes to parse ndef message
     * @return std::shared_ptr<NdefMessage>
     */
    static std::shared_ptr<NdefMessage> GetNdefMessage(const std::string& data);
    /**
     * @Description constructe a ndef message with record list.
     * @param ndefRecords record list to parse ndef message
     * @return std::shared_ptr<NdefMessage>
     */
    static std::shared_ptr<NdefMessage> GetNdefMessage(std::vector<std::shared_ptr<NdefRecord>> ndefRecords);
    /**
     * @Description convert the rtd bytes into byte array defined in Nfc forum.
     * @param rtdtype rtd type of a record
     * @return rtd byte array
     */
    static std::string GetTagRtdType(EmRtdType rtdtype);
    /**
     * @Description Create a ndef record with uri data.
     * @param uriString uri data for new a ndef record
     * @return std::shared_ptr<NdefRecord>
     */
    static std::shared_ptr<NdefRecord> MakeUriRecord(const std::string& uriString);
    /**
     * @Description Create a ndef record with text data.
     * @param text text data for new a ndef record
     * @param locale language code for the ndef record . if locale is null, use default locale
     * @return std::shared_ptr<NdefRecord>
     */
    static std::shared_ptr<NdefRecord> MakeTextRecord(const std::string& text, const std::string& locale);
    /**
     * @Description Create a ndef record with mime data.
     * @param mimeType type of mime data for new a ndef record
     * @param mimeData mime data for new a ndef record
     * @return std::shared_ptr<NdefRecord>
     */
    static std::shared_ptr<NdefRecord> MakeMimeRecord(const std::string& mimeType, const std::string& mimeData);

    /**
     * @Description Create a ndef record with external data.
     * @param domainName domain name of issuing organization for the external data
     * @param serviceName domain specific type of data for the external data
     * @param externalData data payload of a ndef record
     * @return std::shared_ptr<NdefRecord>
     */
    static std::shared_ptr<NdefRecord> MakeExternalRecord(const std::string& domainName,
                                                          const std::string& serviceName,
                                                          const std::string& externalData);
    /**
     * @Description parse a ndef message into raw bytes.
     * @param ndefMessage a ndef message to parse
     * @return raw bytes of a ndef message
     */
    static std::string MessageToString(std::weak_ptr<NdefMessage> ndefMessage);
    /**
     * @Description parse a ndef record into raw bytes.
     * @param record a ndef record to parse
     * @param buffer raw bytes of a ndef record
     * @param bIsMB the flag of begin record
     * @param bIsME the flag of end record
     * @return void
     */
    static void NdefRecordToString(std::weak_ptr<NdefRecord> record, std::string& buffer, bool bIsMB, bool bIsME);
    /**
     * @Description Get all records of a ndef message.
     * @param void
     * @return record list of a ndef message
     */
    std::vector<std::shared_ptr<NdefRecord>> GetNdefRecords() const;

private:
    static std::shared_ptr<NdefRecord> InitNdefRecord(size_t tnf,
                                                      const std::string& id,
                                                      const std::string& payload,
                                                      const std::string& tagRtdType);
    static bool CheckTnf(size_t tnf, const std::string& tagRtdType, const std::string& id, const std::string& payload);
    static std::vector<std::shared_ptr<NdefRecord>> ParseRecord(const std::string& data, bool bIgnoreMbMe);

private:
    std::vector<std::shared_ptr<NdefRecord>> mNdefRecordList_{};
};
}  // namespace sdk
}  // namespace nfc
}  // namespace OHOS

#endif  // NDEF_MESSAGE_H
