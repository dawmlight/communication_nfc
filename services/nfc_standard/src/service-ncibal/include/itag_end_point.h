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
#ifndef I_TAG_END_POINT_H
#define I_TAG_END_POINT_H

#include <functional>
#include <memory>
#include <vector>

namespace OHOS {
namespace nfc {
namespace sdk{
class NfcMap;
}
namespace ncibal {
class ITagEndPoint {
public:
    using TagDisconnectedCallBack = std::function<void(int)>;

    virtual ~ITagEndPoint() {}
    /**
     * @brief Connect the tag
     * @param technology the technology of the tag
     * @return True if ok
     */
    virtual bool Connect(int technology) = 0;
    /**
     * @brief Disconnect the tag
     * @return True if ok
     */
    virtual bool Disconnect() = 0;
    /**
     * @brief Reconnect the tag
     * @return True if ok
     */
    virtual bool Reconnect() = 0;
    /**
     * @brief Send data to tag and receive response
     * @param request the technology of the tag
     * @param response response from the tag
     * @return 0 if ok
     */
    virtual int Transceive(std::string& request, std::string& response) = 0;
    /**
     * @brief Check if the tag is in field
     * @return True if ok
     */
    virtual bool PresenceCheck() = 0;
    virtual bool IsPresent() = 0;
    virtual void StartPresenceChecking(int presenceCheckDelay, TagDisconnectedCallBack callback) = 0;
    virtual void StopPresenceChecking() = 0;
    virtual std::vector<int> GetTechList() = 0;
    virtual void RemoveTechnology(int technology) = 0;
    virtual std::string GetUid() = 0;
    virtual int GetHandle() = 0;
    virtual std::weak_ptr<sdk::NfcMap> GetTechExtras() = 0;
    virtual bool MakeReadOnly() = 0;
    virtual std::string ReadNdef() = 0;
    virtual bool WriteNdef(std::string& data) = 0;
    virtual bool FormatNdef(const std::string& key) = 0;
    virtual bool IsNdefFormatable() = 0;
    virtual bool CheckNdef(std::vector<int>& ndefInfo) = 0;
    virtual int GetConnectedTechnology() = 0;
};
}  // namespace ncibal
}  // namespace nfc
}  // namespace OHOS
#endif /* I_TAG_END_POINT_H */