/******************************************************************************
 *
 *  Copyright (C) 2016 ST Microelectronics S.A.
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

// this file controls the NFA stack for the purpose of MTK tools tests only.
#include "st_se_service.h"

#include "loghelper.h"
#include "se_callback_proxy.h"
#include "st_mt_glue.h"

namespace OHOS {
namespace hardware {
namespace se {
namespace v1_0 {
class StSeService::SeServiceDeathRecipient final : public IRemoteObject::DeathRecipient {
public:
    explicit SeServiceDeathRecipient(wptr<StSeService> stSeService) : stSeService_(stSeService) {}
    void OnRemoteDied(const wptr<IRemoteObject>& object) override
    {
        if (object == nullptr) {
            DebugLog("wptrDeath is null");
            return;
        }
        stSeService_->callback_->AsObject()->RemoveDeathRecipient(stSeService_->mDeathRecipient_);
        stSeService_->callback_ = nullptr;
        InfoLog("The Front SE Service is died.");
    }

private:
    wptr<StSeService> stSeService_;
};

void StSeService::Initialize(const sptr<IRemoteObject>& callback)
{
    DebugLog("StSeService::Initialize Begin");
    int result = init();
    DebugLog("StSeService::Initialize: result is (%d)).", result);
    if (!mDeathRecipient_) {
        mDeathRecipient_ = new SeServiceDeathRecipient(this);
    }
    DebugLog("StSeService::Initialize: AddDeathRecipient to ISecureElementCallback.");
    callback->AddDeathRecipient(mDeathRecipient_);

    callback_ = new SeCallbackProxy(callback);
    if (NFA_STATUS_OK == result) {
        callback_->OnStateChange(true, "Initialize Success.");
    } else {
        callback_->OnStateChange(false, "Initialize Failed.");
    }
}

std::string StSeService::GetAtr()
{
    DebugLog("StSeService::GetAtr");
    std::string rsp{};
    uint8_t p_rsp_buf[255];
    uint8_t max_buf_size = 255;
    uint8_t rsp_size{0};
    int ret = getAtr(p_rsp_buf, max_buf_size, &rsp_size);
    if (ret == 0 && rsp_size > 0) {
        rsp.insert(0, (char*)&p_rsp_buf[0], rsp_size);
    }
    return rsp;
}

bool StSeService::IsCardPresent()
{
    DebugLog("StSeService::IsCardPresent");
    return true;
}

std::string StSeService::Transmit(const std::string& data)
{
    DebugLog("StSeService::Transmit");
    std::string rsp{};
    uint8_t p_rsp_buf[65538];
    uint32_t max_buf_size = 65538;
    uint32_t rsp_size{0};
    transmit((uint8_t*)data.c_str(), data.length(), p_rsp_buf, max_buf_size, &rsp_size);
    if (rsp_size > 0) {
        rsp.insert(0, (char*)&p_rsp_buf[0], rsp_size);
    }
    return rsp;
}

std::unique_ptr<SEBasicRespData> StSeService::OpenBasicChannel(const std::string& aid, char p2)
{
    DebugLog("StSeService::OpenBasicChannel");
    std::unique_ptr<SEBasicRespData> respData = std::make_unique<SEBasicRespData>();
    std::string rsp;
    uint8_t p_rsp_buf[255];
    uint8_t max_buf_size = 255;
    uint8_t rsp_size{0};
    int ret = openBasicChannel((uint8_t*)aid.c_str(), aid.length(), p_rsp_buf, max_buf_size, &rsp_size);
    if (ret != NFA_STATUS_OK || rsp_size < 1) {
        respData->status = IO_ERROR;
    } else {
        uint8_t sw1 = p_rsp_buf[rsp_size - 2];
        uint8_t sw2 = p_rsp_buf[rsp_size - 1];
        if ((sw1 == 0x90) && (sw2 == 0x00)) {
            respData->resp.insert(0, (char*)&p_rsp_buf[0], rsp_size);
            respData->status = SUCCESS;
        } else if (sw1 == 0x6A && sw2 == 0x82) {
            respData->status = NO_SUCH_SE_ERROR;
        } else if (sw1 == 0x6A && sw2 == 0x86) {
            respData->status = UNSUPPORTED_OPERATION;
        } else {
            respData->resp.insert(0, (char*)&p_rsp_buf[0], rsp_size);
        }
    }
    return respData;
}

std::unique_ptr<SELogicalRespData> StSeService::OpenLogicalChannel(const std::string& aid, char rsp)
{
    DebugLog("StSeService::OpenLogicalChannel");
    std::unique_ptr<SELogicalRespData> respData = std::make_unique<SELogicalRespData>();
    uint8_t p_rsp_buf[255];
    uint8_t max_buf_size = 255;
    uint8_t rsp_size{0};
    uint8_t channel_id{0};
    int ret = openLogicalChannel((uint8_t*)aid.c_str(), aid.length(), p_rsp_buf, max_buf_size, &rsp_size, &channel_id);
    DebugLog("Channel id : 0x%02X", channel_id);
    if (ret != NFA_STATUS_OK || rsp_size < 1) {
    } else {
        uint8_t sw1 = p_rsp_buf[rsp_size - 2];
        uint8_t sw2 = p_rsp_buf[rsp_size - 1];
        if ((sw1 == 0x90) && (sw2 == 0x00)) {
            respData->resp.insert(0, (char*)&p_rsp_buf[0], rsp_size);
            respData->channelNumber = channel_id;
            respData->status = SUCCESS;
            return respData;
        }
        if (sw1 == 0x6A && sw2 == 0x81) {
            respData->status = CHANNEL_NOT_AVAILABLE;
        } else if (((sw1 == 0x6E || sw1 == 0x6D) && sw2 == 0x00) || (sw1 == 0x6A && sw2 == 0x86)) {
            respData->status = UNSUPPORTED_OPERATION;
        } else if (sw1 == 0x6A && sw2 == 0x82) {
            respData->status = NO_SUCH_SE_ERROR;
        } else {
            respData->resp.insert(0, (char*)&p_rsp_buf[0], rsp_size);
        }
    }
    closeLogicalChannel(channel_id);
    return respData;
}

SecureElementStatus StSeService::CloseChannel(uint8_t channelNumber)
{
    DebugLog("StSeService::CloseChannel");
    if (channelNumber < 0 || channelNumber >= 20) {
        return SecureElementStatus::FAILED;
    }
    if (channelNumber > 0) {
        closeLogicalChannel(channelNumber);
    }
    return SecureElementStatus::SUCCESS;
}

SecureElementStatus StSeService::Reset()
{
    DebugLog("StSeService::Reset");
    return SecureElementStatus::SUCCESS;
}

StSeService::StSeService() : callback_(nullptr), mDeathRecipient_(nullptr) {}

StSeService::~StSeService()
{
    callback_ = nullptr;
    mDeathRecipient_ = nullptr;
}
}  // namespace v1_0
}  // namespace se
}  // namespace hardware
}  // namespace OHOS
