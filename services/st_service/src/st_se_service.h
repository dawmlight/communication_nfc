/******************************************************************************
 *
 *  Copyright (C) 2010-2014 Broadcom Corporation
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at:
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 ******************************************************************************/

/******************************************************************************
 *
 *  This is the public interface file for NFA HCI, Broadcom's NFC
 *  application layer for mobile phones.
 *
 ******************************************************************************/
#ifndef ST_SE_SERVICE_H
#define ST_SE_SERVICE_H

#include <string>

#include "st_se_service_stub.h"

namespace OHOS {
namespace hardware {
namespace se {
namespace v1_0 {
class StSeService final : public StSeServiceStub {
    // observe the se service
    class SeServiceDeathRecipient;

public:
    explicit StSeService();
    ~StSeService();

    void Initialize(const sptr<IRemoteObject>& callback) override;
    std::string GetAtr() override;
    bool IsCardPresent() override;
    std::string Transmit(const std::string& cmd) override;
    std::unique_ptr<SEBasicRespData> OpenBasicChannel(const std::string& aid, char p2) override;
    std::unique_ptr<SELogicalRespData> OpenLogicalChannel(const std::string& aid, char p2) override;
    SecureElementStatus CloseChannel(uint8_t channelNumber) override;
    SecureElementStatus Reset() override;

private:
    sptr<ISecureElementCallback> callback_{};
    sptr<SeServiceDeathRecipient> mDeathRecipient_{};
};
}  // namespace v1_0
}  // namespace se
}  // namespace hardware
}  // namespace OHOS
#endif /* ST_MT_GLUE_H */
