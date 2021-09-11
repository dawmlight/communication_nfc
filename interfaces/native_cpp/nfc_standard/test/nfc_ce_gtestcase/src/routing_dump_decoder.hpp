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
#ifndef _ROUTING_DUMP_H_
#define _ROUTING_DUMP_H_

#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <zlib.h>

#include <algorithm>
#include <cassert>
#include <cstring>
#include <exception>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

namespace OHOS::nfc::test {
static const uint8_t MASK_MATCH_MODE = 0x30;
static const uint8_t MASK_AID_TYPE = 0x0f;
static const uint8_t AID_TYPE = 0x02;
static const uint8_t MIN_DATA_LEN = 2u;

static const size_t ENTRY_MIN_LEN = 4u;
static const size_t ENTRY_MAX_LEN = 20u;

// Number of entries for technical route and protocol route
constexpr size_t TECH_PROTO_ENTRY_COUNT = 4u;

constexpr uint8_t PATTERN_EXACTLY = 0x0;
constexpr uint8_t PATTERN_PREFIX = 0x10;
constexpr uint8_t POWER_STATE = 0x11;

static void base64Decode(const char* input, size_t inputLength, std::vector<char>& output, int& outputLength)
{
    output.reserve(inputLength);
    BIO* b64 = NULL;
    BIO* bio = NULL;
    b64 = BIO_new(BIO_f_base64());
    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
    bio = BIO_new_mem_buf(input, static_cast<int>(inputLength));
    bio = BIO_push(b64, bio);

    outputLength = BIO_read(bio, &output[0], inputLength);

    BIO_free_all(bio);
}
// 4103001104410300010540030011004003001101
static const std::vector<uint8_t> specialRoutingEntry{0x41, 0x03, 0x00, 0x11, 0x04, 0x41, 0x03, 0x00, 0x01, 0x05,
                                                      0x40, 0x03, 0x00, 0x11, 0x00, 0x40, 0x03, 0x00, 0x11, 0x01};

static size_t SundaySearch(const std::vector<uint8_t>& target, const std::vector<uint8_t>& pattern)
{
    if (target.size() == 0u || target.size() < pattern.size()) {
        return -1;
    }
    if (pattern.size() == 0u) {
        return 0;
    }
    std::vector<size_t> shift(0x100, pattern.size() + 1);
    for (size_t i = 0; i < pattern.size(); i++) {
        shift[pattern[i]] = pattern.size() - i;
    }
    for (size_t i = 0; i <= target.size() - pattern.size(); i += shift[target[i + pattern.size()]]) {
        for (size_t j = 0; j < pattern.size(); j++) {
            if (target[i + j] != pattern[j]) {
                break;
            }
            if (j == pattern.size() - 1) {
                return i;
            }
        }
    }
    return -1;
}

static bool writeToVector(std::vector<uint8_t>& output, size_t offset, const uint8_t* buf, size_t len)
{
    if (buf == nullptr || len == 0) {
        return false;
    }

    for (size_t i = 0; i < len; ++i) {
        output.insert(output.end(), buf[i]);
    }
    return true;
}

static int inflateData(const char* input, int inputLength, std::vector<uint8_t>& output)
{
    constexpr int CHUNK = 16384;
    int ret;
    unsigned have;
    z_stream strm;
    unsigned char in[CHUNK]{0};
    unsigned char out[CHUNK]{0};
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    strm.avail_in = 0;
    strm.next_in = Z_NULL;
    ret = inflateInit(&strm);
    if (ret != Z_OK) return ret;

    /* decompress until deflate stream ends or end of file */
    size_t written = 0u;
    int offset = 0;
    do {
        if (offset >= inputLength) {
            ret = Z_STREAM_END;
            break;
        }
        strm.avail_in = std::min(inputLength, CHUNK);
        if (strm.avail_in == 0) break;
        memcpy(in, &input[offset], strm.avail_in);
        offset += strm.avail_in;
        strm.next_in = in;
        /* run inflate() on input until output buffer not full */
        do {
            strm.avail_out = CHUNK;
            strm.next_out = out;
            ret = inflate(&strm, Z_NO_FLUSH);
            assert(ret != Z_STREAM_ERROR); /* state not clobbered */
            switch (ret) {
                case Z_NEED_DICT:
                    ret = Z_DATA_ERROR; /* and fall through */
                    [[fallthrough]];
                case Z_DATA_ERROR:
                    [[fallthrough]];
                case Z_MEM_ERROR:
                    (void)inflateEnd(&strm);
                    return ret;
            }
            have = CHUNK - strm.avail_out;
            writeToVector(output, written, out, static_cast<size_t>(have));
            written += static_cast<size_t>(have);
        } while (strm.avail_out == 0);
        /* done when inflate() says it's done */
    } while (ret != Z_STREAM_END);
    /* clean up and return */
    (void)inflateEnd(&strm);
    return ret == Z_STREAM_END ? Z_OK : Z_DATA_ERROR;
}

static std::string BytesToHexStr(const unsigned char* start, size_t len, bool upper) noexcept
{
    if (start == nullptr || len == 0) {
        return "";
    }
    std::stringstream ss;
    if (upper) {
        ss << std::uppercase;
    }
    for (size_t i = 0; i < len; i++) {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(start[i]);
    }

    return ss.str();
}

static std::string BytesToHexStr(const std::vector<uint8_t>& bytes, bool upper) noexcept
{
    if (bytes.empty()) {
        return std::string();
    }
    return BytesToHexStr(&bytes[0], bytes.size(), upper);
}

unsigned char HexCharToByte(char ch) noexcept
{
    if (ch >= '0' && ch <= '9') {
        return static_cast<unsigned char>(ch - '0');
    } else if (ch >= 'a' && ch <= 'f') {
        return static_cast<unsigned char>(ch - 'a' + 0x0a);
    } else if (ch >= 'A' && ch <= 'F') {
        return static_cast<unsigned char>(ch - 'A' + 0x0a);
    } else {
        return 0;
    }
}

static void HexStrToBytes(const std::string& src, std::vector<unsigned char>& bytes) noexcept
{
    bytes.clear();

    bool padding = (src.size() % 2 != 0);
    bytes.reserve(padding ? (src.size() + 1) / 2 : src.size() / 2);

    char hi = '0';
    char lo = '0';
    auto i = src.cbegin();
    while (i != src.cend()) {
        if (i == src.cbegin() && padding) {
            hi = '0';
            lo = *i;
            ++i;
        } else {
            hi = *i;
            ++i;
            lo = *i;
            ++i;
        }
        bytes.emplace_back((HexCharToByte(hi) << 4) | (HexCharToByte(lo)));
    }
}

struct FileCloser final {
    explicit FileCloser(std::ifstream& f) : f_(f) {}
    ~FileCloser()
    {
        if (f_) {
            f_.close();
        }
    }

private:
    std::ifstream& f_;
};
struct RoutingEntry {
    uint8_t head_;
    uint8_t env_;
    uint8_t powerstate_{POWER_STATE};
    std::vector<uint8_t> aid_;
    static bool IsQualifierTypeAid(uint8_t h)
    {
        constexpr uint8_t maskQualifierType = 0x0f;
        constexpr uint8_t qualifierTypeAid = 0x02;
        return (h & maskQualifierType) == qualifierTypeAid;
    }

    static uint8_t GetMatchPattern(uint8_t h)
    {
        constexpr uint8_t maskMatchPattern = 0x30;
        return (h & maskMatchPattern);
    }

    static bool IsPrefixPattern(uint8_t h)
    {
        return GetMatchPattern(h) == PATTERN_PREFIX;
    }
    static bool SurpportedEnv(uint8_t h)
    {
        constexpr uint8_t envHost = 0u;
        constexpr uint8_t envESE1 = 0x82;
        constexpr uint8_t envSIM1 = 0x81;
        return h == envHost || h == envESE1 || h == envSIM1;
    }

    static bool IsExpectedPowerState(uint8_t h)
    {
        return h == POWER_STATE || h == 0x3b;
    }
    static std::unique_ptr<RoutingEntry> Create(const std::vector<uint8_t>& buf)
    {
        std::unique_ptr<RoutingEntry> result;
        if (buf.size() < ENTRY_MIN_LEN) {
            return result;
        }
        constexpr size_t posHead = 0u;
        constexpr size_t posLen = 1u;
        constexpr size_t posEnv = 2u;
        constexpr size_t posPow = 3u;
        constexpr size_t posAid = 4u;
        constexpr size_t entryFieldLenMinValue = 2;  // Minimum value of entry length field

        if (!IsQualifierTypeAid(buf[posHead])) {
            return result;
        }
        if (static_cast<size_t>(buf[posLen]) != buf.size() - entryFieldLenMinValue) {
            return result;
        }
        if (static_cast<size_t>(buf[posLen]) == entryFieldLenMinValue &&
            !IsPrefixPattern(buf[posHead])) {  // The null aid is the default route which must be prefix matching mode

            return result;
        }
        if (!SurpportedEnv(buf[posEnv])) {
            return result;
        }
        if (!IsExpectedPowerState(buf[posPow])) {
            printf("actual power state: %u\n", buf[posPow]);
            return result;
        }
        std::vector<uint8_t> aid;
        if (buf.size() > ENTRY_MIN_LEN) {
            aid.insert(aid.end(), buf.begin() + posAid, buf.end());
        }
        result = std::make_unique<RoutingEntry>(buf[posHead], buf[posEnv], buf[posPow], std::move(aid));

        return result;
    }

    RoutingEntry(int head, int env, int powerstate, std::vector<uint8_t> aid)
        : head_(static_cast<uint8_t>(head)),
          env_(static_cast<uint8_t>(env)),
          powerstate_(static_cast<uint8_t>(powerstate)),
          aid_(std::move(aid))
    {
    }
    RoutingEntry(int head, int env, int powerstate, const std::string& aidString)
        : RoutingEntry(head, env, powerstate, std::vector<uint8_t>{})
    {
        HexStrToBytes(aidString, aid_);
    }
    RoutingEntry(const std::initializer_list<uint8_t>& l)
    {
        if (l.size() >= 2) {
            auto it = l.begin();
            head_ = (*it++);
            env_ = (*it++);
            powerstate_ = *it++;
            aid_.insert(aid_.end(), it, l.end());
        }
    }

    RoutingEntry(const RoutingEntry& other)
    {
        Copy(other);
    }
    RoutingEntry& operator=(const RoutingEntry& other)
    {
        if (this != &other) {
            Copy(other);
        }
        return *this;
    }
    RoutingEntry(RoutingEntry&& other)
    {
        Move(std::move(other));
    }
    RoutingEntry& operator=(RoutingEntry&& other)
    {
        if (this != &other) {
            Move(std::move(other));
        }
        return *this;
    }

    bool operator==(const RoutingEntry& e) const
    {
        bool aidEq = (aid_.size() == e.aid_.size());
        if (aidEq && !aid_.empty()) {
            aidEq = memcmp(&aid_[0], &e.aid_[0], aid_.size()) == 0;
        }
        return aidEq && head_ == e.head_ && env_ == e.env_ && powerstate_ == e.powerstate_;
    }
    bool operator!=(const RoutingEntry& e) const
    {
        return !operator==(e);
    }

    std::string ToString() const
    {
        std::stringstream ss;
        ss << "{0x" << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(head_);
        ss << ", 0x" << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(env_);
        ss << ", 0x" << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(powerstate_);

        ss << ",";
        ss << '"' << BytesToHexStr(aid_, true) << '"';
        ss << "},";
        return ss.str();
    }

private:
    void Copy(const RoutingEntry& other)
    {
        head_ = other.head_;
        env_ = other.env_;
        powerstate_ = other.powerstate_;
        aid_ = other.aid_;
    }
    void Move(RoutingEntry&& other)
    {
        head_ = other.head_;
        env_ = other.env_;
        powerstate_ = other.powerstate_;
        aid_ = std::move(other.aid_);
    }
};
static bool RoutingTableEquals(std::vector<RoutingEntry>::const_iterator lstart,
                               std::vector<RoutingEntry>::const_iterator lend,
                               std::vector<RoutingEntry>::const_iterator rstart,
                               std::vector<RoutingEntry>::const_iterator rend)
{
    bool rv = (std::distance(lstart, lend) == std::distance(rstart, rend));
    if (!rv) {
        return rv;
    }
    auto l = lstart;
    auto r = rstart;
    rv = true;
    while (l != lend) {
        if (*l != *r) {
            rv = false;
            break;
        }
        ++l;
        ++r;
    }
    return rv;
}
static std::string RoutingEntriesToString(const std::vector<RoutingEntry>& entries)
{
    std::stringstream ss;
    for (auto& en : entries) {
        ss << "\t" << en.ToString() << "\n";
    }
    return ss.str();
}
class RoutingDumpDecoder {
public:
    explicit RoutingDumpDecoder(const std::string& filepath) : filepath_(filepath) {}

    bool parseToFile(const std::string& output)
    {
        std::vector<uint8_t> routingTable;
        bool rv = parse(routingTable);
        if (!rv) {
            return false;
        }
        std::ofstream f1(output + ".hexstr", std::ios_base::out);
        if (!f1) {
            printf("create %s failed.", output.c_str());
            return false;
        }
        f1 << BytesToHexStr(routingTable, true);
        f1.close();
        std::ofstream f(output, std::ios_base::out | std::ofstream::app);
        if (!f) {
            printf("create %s failed.", output.c_str());
            return false;
        }
        f << "=========================================================\n";
        auto allEntries = getAllEntries(routingTable);

        for (size_t i = 0; i < allEntries.size(); i++) {
            std::for_each(allEntries[i].begin(), allEntries[i].end(), [&f](std::vector<RoutingEntry>::reference r) {
                f << r.ToString() << "\n";
            });
            f << "--------------------------------\n\n";
        }

        f.close();
        return rv;
    }
    std::vector<std::vector<RoutingEntry>> getAllEntries(const std::vector<uint8_t>& routingTable)
    {
        std::vector<std::vector<RoutingEntry>> result;
        size_t offset = 0u;
        while (offset < routingTable.size()) {
            // [other data] [the number of routing items (1 byte) |  aid routing items] technical routing items (2) and
            // protocol routing items (2)
            int specialEntryPos = SundaySearch(std::vector<uint8_t>(routingTable.begin() + offset, routingTable.end()),
                                               specialRoutingEntry);
            // printf("specialEntryPos: %d from offset: %zu\n", (specialEntryPos), offset);
            if (specialEntryPos < 1) {
                break;
            }
            std::vector<RoutingEntry> entries;
            findAidEntries(entries, &routingTable[offset], specialEntryPos);
            offset += specialEntryPos + specialRoutingEntry.size();
            if (!entries.empty()) {
                printf("entries(size: %zu): \n%s\n", entries.size(), RoutingEntriesToString(entries).c_str());
                result.emplace_back(entries);
            }
        }
        return result;
    }
    //
    // build RoutingEntry: {head, execEnv, aid}
    // head: 0x42 - exactly, 0x52 - prefix
    // execEnv: 0 - host, 0x81 - UICC1, 0x82 - eSE1
    // powerState: 0x11 (the default route is host) or 0x3B(the default route is 0x82)
    // example: {0x52, 0x82, 0x11,{"D2560000850101"}} means (prefix, eSE1,  0xD2560000850101)
    bool ExpectLastRoutingTable(const std::vector<RoutingEntry>& expected, bool withDefault)
    {
        std::vector<uint8_t> routingTable;
        bool rv = parse(routingTable);
        if (!rv) {
            printf("failed to parse.\n");
            return false;
        }
        std::vector<std::vector<RoutingEntry>> allEntries = getAllEntries(routingTable);
        if (allEntries.empty()) {
            return false;
        }
        std::vector<RoutingEntry>& entries = allEntries.back();
        if (withDefault) {
            rv = RoutingTableEquals(expected.begin(), expected.end(), entries.begin(), entries.end());
        } else {
            if (entries.empty()) {
                rv = false;
            } else {
                rv = RoutingTableEquals(
                    expected.begin(), expected.end(), entries.begin(), entries.begin() + entries.size() - 1);
            }
        };
        if (!rv) {
            printf("withDefault: %s\n", withDefault ? "true" : "false");
            printf("expected(%zu):\n%s\nactual(%zu):\n%s\n",
                   expected.size(),
                   RoutingEntriesToString(expected).c_str(),
                   entries.size(),
                   RoutingEntriesToString(entries).c_str());
        }
        return rv;
    }

private:
    size_t getRawLength(const std::string& line)
    {
        // --- BEGIN:NFCSNOOP_LOG_SUMMARY (317 bytes in) --
        const std::string begin = "--- BEGIN:NFCSNOOP_LOG_SUMMARY (";

        auto p = line.find(begin);
        if (p == std::string::npos) {
            return 0u;
        }
        auto surfix = line.substr(begin.size(), line.size() - begin.size());
        p = surfix.find(' ');
        if (p == std::string::npos) {
            return 0u;
        }
        auto lenStr = surfix.substr(0u, p);
        return static_cast<size_t>(std::atoi(lenStr.c_str()));
    }

    bool readBase64Data(std::ifstream& f, std::string& data, size_t& rawRoutingTableSize)
    {
        data.clear();

        std::stringstream ss;
        std::string line;
        std::getline(f, line);
        printf("first line: %s\n", line.c_str());
        rawRoutingTableSize = getRawLength(line);
        if (rawRoutingTableSize == 0) {
            printf("raw length is 0\n");
            return false;
        }

        while (std::getline(f, line)) {
            if (line.find("---") == 0u) {
                //--- END:NFCSNOOP_LOG_SUMMARY ---
                break;
            }
            ss << line;
        }
        data = ss.str();

        return !data.empty();
    }
    bool decompress(std::vector<uint8_t>& routingTable, const char* compressedData, size_t len)
    {
        if (compressedData == nullptr || len == 0) {
            printf("compressedData is empty.\n");
            return false;
        }
        int rv = inflateData(compressedData, static_cast<int>(len), routingTable);
        if (rv != Z_OK) {
            printf("inflate failed. rv: %d\n", rv);
            return false;
        }
        return true;
    }
    bool parse(std::vector<uint8_t>& routingTable)
    {
        std::ifstream f(filepath_);
        if (!f) {
            printf("open %s failed.\n", filepath_.c_str());
            return false;
        }
        FileCloser fc(f);
        std::string base64Data;
        size_t rawRoutingTableSize = 0;
        if (!readBase64Data(f, base64Data, rawRoutingTableSize)) {
            printf("base64 data is empty\n");
            return false;
        }
        std::vector<char> compressedData;
        int compressedLen = 0;
        base64Decode(base64Data.c_str(), base64Data.size(), compressedData, compressedLen);

        routingTable.reserve(rawRoutingTableSize);
        const size_t dataOffset = 9;  // version: 1 byte. timestamp: 8 bytes
        if (!decompress(routingTable, &compressedData[dataOffset], compressedLen)) {
            printf("decompress failed.");
        }
        return true;
    }

    void findAidEntries(std::vector<RoutingEntry>& entries, const uint8_t* routingTable, size_t tableSize)
    {
        if (tableSize < ENTRY_MIN_LEN + 1) {
            return;
        }
        size_t startOffset = tableSize - ENTRY_MIN_LEN;
        size_t endOffset = tableSize;
        while (startOffset > 0u) {
            auto ent = RoutingEntry::Create(std::vector<uint8_t>(&routingTable[startOffset], &routingTable[endOffset]));
            if (!ent) {
                startOffset--;
                if (endOffset - startOffset > ENTRY_MAX_LEN) {
                    //
                    break;
                }
                continue;
            }
            entries.insert(entries.begin(), std::move(*ent));
            endOffset = startOffset;
            startOffset -= ENTRY_MIN_LEN;
        }
        if (endOffset > 0u) {
            // printf("endOffset: %zu\n", endOffset);
            size_t entryCount = static_cast<size_t>(routingTable[endOffset - 1u]);
            if (entryCount != entries.size() + TECH_PROTO_ENTRY_COUNT) {
                printf("entry count: expected: %zu, actual: %zu\n", entryCount, entries.size());

                entries.clear();
            }
        }
    }

private:
    std::string filepath_;
};
}  // namespace OHOS::nfc::test
#endif  //_ROUTING_DUMP_H_
