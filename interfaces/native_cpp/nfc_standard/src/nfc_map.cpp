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
#include "nfc_map.h"

#include "loghelper.h"

namespace OHOS {
namespace nfc {
namespace sdk {
using SdkNfcMap = std::map<std::string, std::pair<int, std::shared_ptr<void>>>;
NfcMap::NfcMap() : mKeyValueMap_() {}

NfcMap::~NfcMap()
{
    Clear();
}

NfcMap::NfcMap(std::shared_ptr<NfcMap>& other) : mKeyValueMap_(other->mKeyValueMap_) {}

void NfcMap::Clear()
{
    std::lock_guard<std::mutex> lock(mtx_);
    mKeyValueMap_.clear();
}

std::weak_ptr<NfcMap> NfcMap::GetNfcMap(const std::string& key)
{
    std::lock_guard<std::mutex> lock(mtx_);
    SdkNfcMap::iterator iter = mKeyValueMap_.find(key);
    if (iter == mKeyValueMap_.end()) {
        return std::shared_ptr<NfcMap>();
    }
    if (iter->second.first != NfcMap_NfcMap) {
        return std::shared_ptr<NfcMap>();
    }
    return std::static_pointer_cast<NfcMap>(iter->second.second);
}

std::weak_ptr<char> NfcMap::GetChar(const std::string& key)
{
    std::lock_guard<std::mutex> lock(mtx_);
    SdkNfcMap::iterator iter = mKeyValueMap_.find(key);
    if (iter == mKeyValueMap_.end()) {
        return std::shared_ptr<char>();
    }
    if (iter->second.first != NfcMap_CHAR) {
        return std::shared_ptr<char>();
    }
    return std::static_pointer_cast<char>(iter->second.second);
}

std::weak_ptr<std::string> NfcMap::GetCharArray(const std::string& key)
{
    std::lock_guard<std::mutex> lock(mtx_);
    SdkNfcMap::iterator iter = mKeyValueMap_.find(key);
    if (iter == mKeyValueMap_.end()) {
        return std::shared_ptr<std::string>();
    }
    if (iter->second.first != NfcMap_CHAR_ARRAY) {
        return std::shared_ptr<std::string>();
    }
    return std::static_pointer_cast<std::string>(iter->second.second);
}

std::weak_ptr<long> NfcMap::GetLong(const std::string& key)
{
    std::lock_guard<std::mutex> lock(mtx_);
    SdkNfcMap::iterator iter = mKeyValueMap_.find(key);
    if (iter == mKeyValueMap_.end()) {
        return std::shared_ptr<long>();
    }
    if (iter->second.first != NfcMap_LONG) {
        return std::shared_ptr<long>();
    }
    return std::static_pointer_cast<long>(iter->second.second);
}

bool NfcMap::HasData(const std::string& key)
{
    std::lock_guard<std::mutex> lock(mtx_);
    SdkNfcMap::iterator iter = mKeyValueMap_.find(key);
    if (iter == mKeyValueMap_.end()) {
        return false;
    }
    return true;
}

void NfcMap::PutAll(std::weak_ptr<NfcMap> NfcMap)
{
    std::lock_guard<std::mutex> lock(mtx_);
    if (NfcMap.expired()) {
        return;
    }

    SdkNfcMap::iterator iter = NfcMap.lock()->mKeyValueMap_.begin();
    while (iter != NfcMap.lock()->mKeyValueMap_.end()) {
        mKeyValueMap_.insert(make_pair(iter->first, iter->second));
    }
}

void NfcMap::PutNfcMap(const std::string& key, std::shared_ptr<NfcMap>& value)
{
    std::lock_guard<std::mutex> lock(mtx_);
    mKeyValueMap_.insert(make_pair(key, std::make_pair(NfcMap_NfcMap, value)));
}

void NfcMap::PutChar(const std::string& key, char value)
{
    std::lock_guard<std::mutex> lock(mtx_);
    mKeyValueMap_.insert(make_pair(key, std::make_pair(NfcMap_CHAR, std::make_shared<char>(value))));
}

void NfcMap::PutCharArray(const std::string& key, std::shared_ptr<std::string> value)
{
    std::lock_guard<std::mutex> lock(mtx_);
    mKeyValueMap_.insert(make_pair(key, std::make_pair(NfcMap_CHAR_ARRAY, value)));
}
void NfcMap::PutLong(const std::string& key, long value)
{
    std::lock_guard<std::mutex> lock(mtx_);
    mKeyValueMap_.insert(make_pair(key, std::make_pair(NfcMap_LONG, std::make_shared<long>(value))));
}

void NfcMap::Remove(const std::string& key)
{
    std::lock_guard<std::mutex> lock(mtx_);
    mKeyValueMap_.erase(key);
}

bool NfcMap::Marshalling(Parcel& parcel) const
{
    DebugLog("NfcMap::Marshalling in mKeyValueMap_ len.%d", mKeyValueMap_.size());
    SdkNfcMap::const_iterator iter = mKeyValueMap_.begin();
    parcel.WriteInt32(mKeyValueMap_.size());
    while (iter != mKeyValueMap_.end()) {
        parcel.WriteString(iter->first);
        parcel.WriteInt8(iter->second.first);
        if (iter->second.first == NfcMap_CHAR) {
            std::shared_ptr<char> temp = std::static_pointer_cast<char>(iter->second.second);
            parcel.WriteInt8(*(temp.get()));
        } else if (iter->second.first == NfcMap_NfcMap) {
            std::shared_ptr<NfcMap> temp = std::static_pointer_cast<NfcMap>(iter->second.second);
            // temp->Marshalling(parcel);
            parcel.WriteParcelable(temp.get());
        } else if (iter->second.first == NfcMap_CHAR_ARRAY) {
            std::shared_ptr<std::string> temp = std::static_pointer_cast<std::string>(iter->second.second);
            parcel.WriteString(*(temp.get()));
        } else if (iter->second.first == NfcMap_LONG) {
            std::shared_ptr<long> temp = std::static_pointer_cast<long>(iter->second.second);
            parcel.WriteInt64(*(temp.get()));
        } else {
            InfoLog("It is the unsupported NfcMap type(%d).", iter->second.first);
        }
        ++iter;
    }
    return true;
}

NfcMap* NfcMap::Unmarshalling(Parcel& parcel)
{
    DebugLog("NfcMap::Unmarshalling in ");
    if (parcel.GetDataSize() == 0) {
        DebugLog("NfcMap::Unmarshalling parcel size 0, return ");
        return nullptr;
    }

    std::string key;
    int type;
    NfcMap* nfcMap = new NfcMap();
    int size = parcel.ReadInt32();
    while (size > 0 && parcel.GetReadableBytes() > 0) {
        key = parcel.ReadString();
        type = parcel.ReadInt8();
        if (type == NfcMap_CHAR) {
            nfcMap->PutChar(key, parcel.ReadInt8());
        } else if (type == NfcMap_NfcMap) {
            // NfcMap* NfcMap = NfcMap::Unmarshalling(parcel);
            NfcMap* nfcMapTmp = parcel.ReadParcelable<NfcMap>();
            std::shared_ptr<NfcMap> temp(nfcMapTmp);
            nfcMap->PutNfcMap(key, temp);
        } else if (type == NfcMap_CHAR_ARRAY) {
            std::shared_ptr<std::string> temp = std::make_shared<std::string>(parcel.ReadString());
            nfcMap->PutCharArray(key, temp);
        } else if (type == NfcMap_LONG) {
            nfcMap->PutLong(key, parcel.ReadInt64());
        } else {
            InfoLog("It is the unsupported NfcMap type(%d).", type);
        }
        --size;
    }
    return nfcMap;
}
}  // namespace sdk
}  // namespace nfc
}  // namespace OHOS