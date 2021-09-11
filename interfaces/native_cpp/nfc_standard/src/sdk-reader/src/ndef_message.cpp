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
#include "ndef_message.h"

#include <algorithm>
#include <sstream>

#include "loghelper.h"
#include "nfc_sdk_common.h"
using namespace std;

namespace OHOS {
namespace nfc {
namespace sdk {
NdefMessage::NdefMessage(std::vector<std::shared_ptr<NdefRecord>> ndefRecords)
    : mNdefRecordList_(std::move(ndefRecords))
{
}

NdefMessage::~NdefMessage()
{
    mNdefRecordList_.clear();
}

std::shared_ptr<NdefMessage> NdefMessage::GetNdefMessage(const string& data)
{
    std::vector<std::shared_ptr<NdefRecord>> ndefRecords = ParseRecord(data, false);
    if (ndefRecords.empty()) {
        return std::shared_ptr<NdefMessage>();
    }

    return GetNdefMessage(ndefRecords);
}

std::shared_ptr<NdefMessage> NdefMessage::GetNdefMessage(std::vector<std::shared_ptr<NdefRecord>> ndefRecords)
{
    return std::make_shared<NdefMessage>(std::move(ndefRecords));
}

string NdefMessage::GetTagRtdType(EmRtdType rtdtype)
{
    string rtd;
    switch (rtdtype) {
        case EmRtdType::RTD_TEXT:
            rtd = "T";  // 0x54
            break;
        case EmRtdType::RTD_URI:
            rtd = "U";  // 0x55
            break;
        case EmRtdType::RTD_SMART_POSTER:
            rtd = "Sp";  // 0x53, 0x70
            break;
        case EmRtdType::RTD_ALTERNATIVE_CARRIER:
            rtd = "ac";  // 0x61, 0x63
            break;
        case EmRtdType::RTD_HANDOVER_CARRIER:
            rtd = "Hc";  // 0x48, 0x63
            break;
        case EmRtdType::RTD_HANDOVER_REQUEST:
            rtd = "Hr";  // 0x48, 0x72
            break;
        case EmRtdType::RTD_HANDOVER_SELECT:
            rtd = "Hs";  // 0x48, 0x73
            break;
        case EmRtdType::RTD_OHOS_APP:
            rtd = "ohos.com:pkg";  // "ohos.com:pkg"
            break;
        default:
            rtd.clear();
            break;
    }
    return rtd;
}

std::shared_ptr<NdefRecord> NdefMessage::InitNdefRecord(size_t tnf,
                                                        const string& id,
                                                        const string& payload,
                                                        const string& tagRtdType)
{
    bool res = CheckTnf(tnf, tagRtdType, id, payload);
    if (!res) {
        return std::shared_ptr<NdefRecord>();
    }
    std::shared_ptr<NdefRecord> ndefRecord = std::make_shared<NdefRecord>();
    ndefRecord->mTnf_ = tnf;
    ndefRecord->mId_ = id;
    ndefRecord->mPayload_ = payload;
    ndefRecord->mTagRtdType_ = tagRtdType;
    return ndefRecord;
}

bool NdefMessage::CheckTnf(size_t tnf, const string& tagRtdType, const string& id, const string& payload)
{
    switch (tnf) {
        case TNF_EMPTY:
            if (!tagRtdType.empty() || !id.empty() || !payload.empty()) {
                return false;
            }
            break;
        case TNF_WELL_KNOWN:
        case TNF_MIME_MEDIA:
        case TNF_ABSOLUTE_URI:
        case TNF_EXTERNAL_TYPE:
            return true;
        case TNF_UNKNOWN:
        case TNF_RESERVED:
            if (tagRtdType.empty()) {
                return false;
            }
            return true;
        case TNF_UNCHANGED:
            return false;
        default:
            break;
    }
    return false;
}

std::vector<std::shared_ptr<NdefRecord>> NdefMessage::GetNdefRecords() const
{
    return mNdefRecordList_;
}

std::shared_ptr<NdefRecord> NdefMessage::MakeUriRecord(const string& uriString)
{
    if (uriString.empty()) {
        return std::shared_ptr<NdefRecord>();
    }

    string payLoad;
    string uri = uriString;
    for (size_t i = 1; i < gUriPrefix.size() - 1; i++) {
        if (!uriString.compare(0, gUriPrefix[i].size(), gUriPrefix[i])) {
            payLoad += (i & 0xFF);
            uri = uriString.substr(gUriPrefix[i].size());
            DebugLog("prefer index .%d", i);
            break;
        }
    }

    payLoad += uri;

    string id = "";
    string tagRtdType = GetTagRtdType(EmRtdType::RTD_URI);
    return InitNdefRecord(TNF_WELL_KNOWN, id, payLoad, tagRtdType);
}

std::shared_ptr<NdefRecord> NdefMessage::MakeTextRecord(const string& text, const string& locale)
{
    string tagRtdType = GetTagRtdType(EmRtdType::RTD_TEXT);
    string id = "";
    string payLoad = to_string(locale.size());
    payLoad += locale + text;
    return InitNdefRecord(TNF_WELL_KNOWN, id, payLoad, tagRtdType);
}

std::shared_ptr<NdefRecord> NdefMessage::MakeMimeRecord(const string& mimeType, const string& mimeData)
{
    if (mimeData.empty()) {
        return std::shared_ptr<NdefRecord>();
    }
    string id = "";
    size_t t = mimeType.find_first_of('/');
    if (t > 0 && t < mimeType.size() - 1) {
        return InitNdefRecord(TNF_MIME_MEDIA, id, mimeData, mimeType);
    }
    return std::shared_ptr<NdefRecord>();
}

std::shared_ptr<NdefRecord> NdefMessage::MakeExternalRecord(const string& domainName,
                                                            const string& serviceName,
                                                            const string& externalData)
{
    if (domainName.empty() || serviceName.empty() || externalData.empty()) {
        return std::shared_ptr<NdefRecord>();
    }

    string domain = domainName;
    string service = serviceName;
    domain.erase(0, domain.find_first_not_of("\r\t\n "));
    domain.erase(domain.find_last_not_of("\r\t\n ") + 1);
    transform(domain.begin(), domain.end(), domain.begin(), ::tolower);
    service.erase(0, service.find_first_not_of("\r\t\n "));
    service.erase(service.find_last_not_of("\r\t\n ") + 1);
    transform(service.begin(), service.end(), service.begin(), ::tolower);

    if (domain.empty() || service.empty()) {
        return std::shared_ptr<NdefRecord>();
    }

    string tagRtdType = domain + ":" + service;
    string id = "";

    return InitNdefRecord(TNF_EXTERNAL_TYPE, id, externalData, tagRtdType);
}

string NdefMessage::MessageToString(std::weak_ptr<NdefMessage> ndefMessage)
{
    string buffer;
    if (ndefMessage.expired()) {
        return buffer;
    }
    for (size_t i = 0; i < ndefMessage.lock()->mNdefRecordList_.size(); i++) {
        bool bIsMB = (i == 0);                                                // first record
        bool bIsME = (i == ndefMessage.lock()->mNdefRecordList_.size() - 1);  // last record
        NdefRecordToString(ndefMessage.lock()->mNdefRecordList_.at(i), buffer, bIsMB, bIsME);
    }
    return buffer;
}

void NdefMessage::NdefRecordToString(std::weak_ptr<NdefRecord> record, string& buffer, bool bIsMB, bool bIsME)
{
    if (record.expired()) {
        return;
    }
    string payload = record.lock()->mPayload_;
    int tnf = record.lock()->mTnf_;
    string id = record.lock()->mId_;
    string rtdType = record.lock()->mTagRtdType_;
    bool sr = payload.size() < MAX_PAYLOAD_SIZE;
    bool il = (tnf == TNF_EMPTY) ? true : (id.size() > 0);
    char flag =
        char((bIsMB ? FLAG_MB : 0) | (bIsME ? FLAG_ME : 0) | (sr ? FLAG_SR : 0) | (il ? FLAG_IL : 0)) | (char)tnf;
    buffer.push_back(flag);
    buffer.push_back((char)rtdType.size());
    if (sr) {
        buffer.push_back(char(payload.size()));
    } else {
        buffer.append(NfcSdkCommon::IntToString(payload.size(), NfcSdkCommon::IsLittleEndian()));
    }
    if (il) {
        buffer.push_back(char(id.size()));
    }

    buffer.append(rtdType);
    buffer.append(id);
    buffer.append(payload);
}

std::vector<std::shared_ptr<NdefRecord>> NdefMessage::ParseRecord(const string& data, bool bIgnoreMbMe)
{
    std::vector<std::shared_ptr<NdefRecord>> recordList;
    if (data.empty()) {
        return recordList;
    }

    string tagRtdType, id, payload;

    std::vector<string> chunks;
    bool bInChunk = false;
    char chunkTnf = -1;
    string buffer = data;
    bool bME = false;
    int index = 0;
    while (!bME) {
        char flag = buffer.at(index++);
        bool bMB = (flag & FLAG_MB) != 0;
        bME = (flag & FLAG_ME) != 0;
        bool cf = (flag & FLAG_CF) != 0;
        bool sr = (flag & FLAG_SR) != 0;
        bool il = (flag & FLAG_IL) != 0;

        char tnf = char(flag & FLAG_TNF);

        if (!bMB && recordList.size() == 0 && !bInChunk && !bIgnoreMbMe) {
            return recordList;
        } else if (bMB && (recordList.size() != 0 || bInChunk) && !bIgnoreMbMe) {
            return recordList;
        } else if (bInChunk && il) {
            return recordList;
        } else if (cf && bME) {
            return recordList;
        } else if (bInChunk && tnf != TNF_UNCHANGED) {
            return recordList;
        } else if (!bInChunk && tnf == TNF_UNCHANGED) {
            return recordList;
        }

        int tagRtdTypeLength = buffer.at(index++) & 0xFF;
        long payloadLength = 0;
        if (sr) {
            payloadLength = buffer.at(index++) & 0xFF;
        } else {
            if (static_cast<int>(buffer.size()) < index + int(sizeof(int))) {
                ErrorLog("buffer len.%d index.%d payloadLength error", static_cast<int>(buffer.size()), index);
                return recordList;
            }

            string lenString = buffer.substr(index, sizeof(int));
            payloadLength = NfcSdkCommon::StringToInt(lenString, NfcSdkCommon::IsLittleEndian());
            index += sizeof(int);
        }

        int idLength = il ? (buffer.at(index++) & 0xFF) : 0;
        if (bInChunk && tagRtdTypeLength != 0) {
            return recordList;
        }

        if (!bInChunk) {
            if (tagRtdTypeLength <= 0) {
                tagRtdType.clear();
            } else if (static_cast<int>(buffer.size()) < index + tagRtdTypeLength) {
                ErrorLog("buffer len.%d index.%d rtdtype len.%d error",
                         static_cast<int>(buffer.size()),
                         index,
                         tagRtdTypeLength);
                return recordList;
            } else {
                tagRtdType = buffer.substr(index, tagRtdTypeLength);
                index += tagRtdTypeLength;
            }

            if (idLength <= 0) {
                id.clear();
            } else if (static_cast<int>(buffer.size()) < index + idLength) {
                ErrorLog("buffer len.%d index.%d id len.%d error", static_cast<int>(buffer.size()), index, idLength);
                return recordList;
            } else {
                id = buffer.substr(index, idLength);
                index += idLength;
            }
        }

        if (payloadLength > MAX_PAYLOAD_ARRAY) {
            return recordList;
        }

        if (payloadLength > 0) {
            if (static_cast<int>(buffer.size()) < (index + payloadLength)) {
                ErrorLog("buffer len.%d index.%d error", static_cast<int>(buffer.size()), index);
                return recordList;
            }
            payload = buffer.substr(index, payloadLength);
            index += payloadLength;
        } else {
            payload.clear();
        }

        if (cf && !bInChunk) {
            // first chunk
            if (tagRtdTypeLength == 0 && tnf != TNF_UNKNOWN) {
                return recordList;
            }
            chunks.clear();
            chunkTnf = tnf;
        }
        if (cf && bInChunk) {
            chunks.push_back(payload);
        }
        if (!cf && bInChunk) {
            payloadLength = 0;
            for (string n : chunks) {
                payloadLength += n.length();
            }
            if (payloadLength > MAX_PAYLOAD_ARRAY) {
                return recordList;
            }
            for (string n : chunks) {
                payload += n;
                index += n.length();
            }
            tnf = chunkTnf;
        }

        if (cf) {
            // more chunks to come
            bInChunk = true;
            continue;
        } else {
            bInChunk = false;
        }

        std::shared_ptr<NdefRecord> record = InitNdefRecord(tnf, id, payload, tagRtdType);
        if (!record) {
            return recordList;
        }

        recordList.push_back(record);
        if (bIgnoreMbMe) {
            // for parsing a single NdefRecord
            break;
        }
    }
    return recordList;
}
}  // namespace sdk
}  // namespace nfc
}  // namespace OHOS