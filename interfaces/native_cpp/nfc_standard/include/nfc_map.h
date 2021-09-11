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
#ifndef NFC_MAP_H
#define NFC_MAP_H

#include <map>
#include <memory>
#include <mutex>
#include <string>

#include "parcel.h"

/* A mapping from string& keys to various Parcelable values. */
namespace OHOS {
namespace nfc {
namespace sdk {
class NfcMap final : public Parcelable {
public:
    NfcMap();
    NfcMap(std::shared_ptr<NfcMap>& other);
    ~NfcMap();
    // clear
    void Clear();
    // Get Functions
    std::weak_ptr<NfcMap> GetNfcMap(const std::string& key);
    std::weak_ptr<char> GetChar(const std::string& key);
    std::weak_ptr<std::string> GetCharArray(const std::string& key);
    std::weak_ptr<long> GetLong(const std::string& key);
    // check
    bool HasData(const std::string& key);
    // put functions
    void PutAll(std::weak_ptr<NfcMap> nfcMap);
    void PutNfcMap(const std::string& key, std::shared_ptr<NfcMap>& value);
    void PutChar(const std::string& key, char value);
    void PutCharArray(const std::string& key, std::shared_ptr<std::string> value);
    void PutLong(const std::string& key, long value);
    // remove
    void Remove(const std::string& key);

    bool Marshalling(Parcel& parcel) const override;
    static NfcMap* Unmarshalling(Parcel& parcel);

private:
    enum NfcMapType { NfcMap_CHAR = 0, NfcMap_CHAR_ARRAY, NfcMap_LONG, NfcMap_NfcMap };
    std::mutex mtx_{};
    std::map<std::string, std::pair<int, std::shared_ptr<void>>> mKeyValueMap_;
};
}  // namespace sdk
}  // namespace nfc
}  // namespace OHOS
#endif  // !NFC_MAP_H
