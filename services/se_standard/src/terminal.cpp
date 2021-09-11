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
#include "terminal.h"

#include <sys/time.h>

#include <sstream>
#include <stdexcept>

#include "access_control_enforcer.h"
#include "bundle_manager.h"
#include "channel_access_rule.h"
#include "context.h"
#include "event_handler.h"
#include "isecure_element.h"
#include "loghelper.h"
#include "se_callback_stub.h"
#include "se_channel.h"
#include "se_common_exception.h"
#include "se_end_service.h"
#include "secure_element_channel_stub.h"
#include "secure_element_service.h"
#include "utils/common_utils.h"

using namespace osal;
using ChannelAccessRule = OHOS::se::security::ChannelAccessRule;
using OHOS::hardware::se::v1_0::ISecureElement;
using OHOS::hardware::se::v1_0::SecureElementStatus;
using SecureElementCallbackStub = OHOS::hardware::se::v1_0::SeCallbackStub;

namespace OHOS {
using AppExecFwk::EventHandler;
using AppExecFwk::EventRunner;
namespace se {
static uint64_t GetExpireTimeMicros(uint64_t delay)
{
    struct timeval tv = {0, 0};
    gettimeofday(&tv, nullptr);
    return (tv.tv_sec * 1000 + tv.tv_usec / 1000 + delay);
}
/**
 * @brief To Get HAL service terminated abnormally regularly.
 *
 */
class Terminal::GetHalHandler : public AppExecFwk::EventHandler {
public:
    explicit GetHalHandler(std::weak_ptr<Terminal> terminal, const std::shared_ptr<EventRunner>& runner)
        : EventHandler(runner), mTerminal_(terminal)
    {
    }

    void ProcessEvent(const AppExecFwk::InnerEvent::Pointer& event) override
    {
        uint32_t what = event->GetInnerEventId();
        DebugLog("Terminal Handler receive a Message of %d", what);
        switch (what) {
            case EVENT_GET_HAL: {
                try {
                    mTerminal_.lock()->Initialize();
                } catch (const std::exception&) {
                    DebugLog("%s could not be initialized again.", mTerminal_.lock()->mName_.c_str());
                    SendTimingEvent(EVENT_GET_HAL, GetExpireTimeMicros(GET_SERVICE_DELAY_MILLIS));
                }
                break;
            }
            default:
                break;
        }
    }

private:
    std::weak_ptr<Terminal> mTerminal_;
};
/**
 * @brief The Hal service death recipient. Receive and handle the HAL service death event
 *
 */
class Terminal::SecureElementDeathRecipient : public IRemoteObject::DeathRecipient {
public:
    explicit SecureElementDeathRecipient(std::weak_ptr<Terminal> terminal) : mTerminal_(terminal) {}
    virtual void OnRemoteDied(const wptr<IRemoteObject>& object) override
    {
        std::shared_ptr<Terminal> terminal = mTerminal_.lock();
        DebugLog("%s died.", terminal->mName_.c_str());
        {
            std::lock_guard<std::recursive_mutex> lock(terminal->mLock_);
            terminal->mIsConnected_ = false;
            if (terminal->mAccessControlEnforcer_) {
                terminal->mAccessControlEnforcer_->Reset();
            }
        }

        terminal->mHandler_->SendTimingEvent(EVENT_GET_HAL, GetExpireTimeMicros(GET_SERVICE_DELAY_MILLIS));
    }

private:
    std::weak_ptr<Terminal> mTerminal_;
};
/**
 * @brief The HAL Service state change notification interface.
 *
 */
class Terminal::SecureElementCallback : public SecureElementCallbackStub {
public:
    explicit SecureElementCallback(std::weak_ptr<Terminal> terminal) : mTerminal_(terminal) {}

    /**
     * @brief the state change notification interface
     *
     * @param state true-success false-fail or abnormal
     * @param reason the state change reason
     */
    void OnStateChange(bool state, std::string reason) override
    {
        mTerminal_.lock()->StateChange(state, reason);
    }

private:
    std::weak_ptr<Terminal> mTerminal_;
};

void Terminal::StateChange(bool state, std::string reason)
{
    std::lock_guard<std::recursive_mutex> lock(mLock_);

    InfoLog("OnStateChange: %d reason: %s", state, reason.c_str());
    mIsConnected_ = state;
    if (!state) {
        if (mAccessControlEnforcer_) {
            mAccessControlEnforcer_->Reset();
        }
        return;
    }

    CloseChannels();
    try {
        InitializeAC();
    } catch (const std::exception& e) {
        DebugLog("StateChange: InitializeAC Is Failed. %s", e.what());
    }
    mDefaultAppOnBasicChannel_ = true;
}

/**
 * @brief Initializes the Access Control for the Terminal
 */
void Terminal::InitializeAC()
{
    std::lock_guard<std::recursive_mutex> lock(mLock_);
    if (!mAccessControlEnforcer_) {
        mAccessControlEnforcer_ = std::make_shared<AccessControlEnforcer>(shared_from_this());
    }
    try {
        mAccessControlEnforcer_->Initialize();
    } catch (const IOError& e) {
        mAccessControlEnforcer_ = std::shared_ptr<AccessControlEnforcer>();
        throw e;
    } catch (const MissingResourceError& e) {
        mAccessControlEnforcer_ = std::shared_ptr<AccessControlEnforcer>();
        throw e;
    }
}

/**
 * @brief Load-on the channel access of aid-bundleName
 *
 * @param aid Associated aid
 * @param bundleName Associated the Device Application name
 * @param pid Device Application's Process Id
 * @return std::shared_ptr<ChannelAccessRule> The Channel Access Rule object
 */
std::shared_ptr<ChannelAccessRule> Terminal::EstablishChannelAccess(std::string aid, std::string bundleName, int pid)
{
    bool isRefreshTag = true;
    if (HasPrivilegedPermission(bundleName)) {
        return ChannelAccessRule::GetPrivilegeAccessRule(bundleName, pid);
    }
    if (!mAccessControlEnforcer_ || mAccessControlEnforcer_->IsNoRuleFound()) {
        InitializeAC();
        isRefreshTag = false;
    }
    mAccessControlEnforcer_->SetBundleManager(mContext_.lock()->GetBundleManager());

    if (0 == GetName().compare(0, std::string(SeEndService::UICC_TERMINAL).length(), SeEndService::UICC_TERMINAL) &&
        HasCarrierPrivilegedPermission(bundleName)) {
        return ChannelAccessRule::GetCarrierPrivilegeAccessRule(bundleName, pid);
    }

    std::lock_guard<std::recursive_mutex> lock(mLock_);
    try {
        std::shared_ptr<ChannelAccessRule> channelAccessRule =
            mAccessControlEnforcer_->EstablishChannelAccessRuleForBundle(aid, bundleName, isRefreshTag);
        channelAccessRule->SetCallingPid(pid);
        return channelAccessRule;
    } catch (const IOError& e) {
        throw SecurityError(std::string("Exception in EstablishChannelAccessRuleForBundle() ") + e.what());
    } catch (const std::exception& e) {
        throw SecurityError(std::string("Exception in Establish SeChannel Access ") + e.what());
    }
}

/**
 * @brief Checks if there is Secure Element Privilege permission for the given
 * bundle
 * @param bundleName the given bundle name
 * @return the result: true - yes; false - no
 */
bool Terminal::HasPrivilegedPermission(const std::string& bundleName)
{
    std::weak_ptr<osal::BundleManager> pm = mContext_.lock()->GetBundleManager();
    if (pm.expired()) {
        return false;
    }
    return (pm.lock()->CheckPermission(SECURE_ELEMENT_PRIVILEGED_OPERATION_PERMISSION, bundleName) ==
            osal::BundleManager::PERMISSION_GRANTED);
}

/**
 * @brief Checks if there is Carrier Privilege Permission for the given bundle
 * @param bundleName the given bundle name
 * @return the result: true - yes; false - no
 */
bool Terminal::HasCarrierPrivilegedPermission(const std::string& bundleName)
{
    try {
        std::weak_ptr<osal::BundleManager> pm = mContext_.lock()->GetBundleManager();
        if (pm.expired()) {
            return false;
        }
        std::weak_ptr<BundleInfo> bundleInfo = pm.lock()->GetBundleInfo(bundleName, BundleManager::GET_SIGNATURES);
        return CheckCarrierPrivilegeRules(bundleInfo);
    } catch (const NameNotFoundError& ne) {
    }
    return false;
}

std::string Terminal::GetName()
{
    return mName_;
}

/**
 * @brief  Initializes the terminal. Retrieve and initialize the SE Service
 */
void Terminal::Initialize()
{
    std::lock_guard<std::recursive_mutex> lock(mLock_);
    DebugLog("Terminal::Initialize");
    if (!mDeathRecipient_) {
        mDeathRecipient_ = new SecureElementDeathRecipient(shared_from_this());
    }
    sptr<ISecureElement> seHal = ISecureElement::GetService(mName_, mDeathRecipient_);
    if (seHal == nullptr) {
        throw NoSuchElementError("No HAL is provided for " + mName_);
    }
    Initialize(seHal);
}

/**
 * @brief Initializes the terminal, and initialize the SE Service
 * @param seHalService the SE Service
 */
void Terminal::Initialize(const sptr<ISecureElement>& seHalService)
{
    DebugLog("Terminal::Initialize Parameter.");
    if (!seHalService) {
        // if there is no HAL implementation for the specified SE name
        throw NoSuchElementError("No HAL is provided for " + mName_);
    }
    // To create the GetHalHandler instance, and run the Handler
    if (!mHandler_) {
        std::shared_ptr<AppExecFwk::EventRunner> runner = AppExecFwk::EventRunner::Create("nfc service handler");
        mHandler_ = std::make_shared<GetHalHandler>(shared_from_this(), runner);
        runner->Run();
    }
    mSEHal_ = seHalService;
    if (!mSECallback_) {
        mSECallback_ = new SecureElementCallback(shared_from_this());
    }
    DebugLog("Terminal::Initialize Parameter. HAL Initialize.");
    mSEHal_->Initialize(mSECallback_->AsObject());
    InfoLog("%s was initialized.", mName_.c_str());
}

/**
 * @brief Closes the given channel
 * @param channel the closing the channel
 */
void Terminal::CloseChannel(std::weak_ptr<SeChannel> channel)
{
    if (channel.expired()) {
        return;
    }

    std::shared_ptr<SeChannel> tmpChannel = channel.lock();
    std::lock_guard<std::recursive_mutex> lock(mLock_);
    InfoLog("Terminal::CloseChannel Begin.");
    if (mIsConnected_) {
        try {
            char status = mSEHal_->CloseChannel((char)tmpChannel->GetChannelNumber());
            if (!tmpChannel->IsBasicChannel() && status != SEStatus::SUCCESS) {
                ErrorLog("Error closing channel %d", tmpChannel->GetChannelNumber());
            }
        } catch (const RemoteError& e) {
            ErrorLog("Error To Close SeChannel %s", e.what());
        }
    }
    mChannels_.erase(tmpChannel->GetChannelNumber());
    InfoLog("Terminal::CloseChannel End.");
}

/**
 * @brief Cleans up all the channels in use.
 */
void Terminal::CloseChannels()
{
    std::lock_guard<std::recursive_mutex> lock(mLock_);
    InfoLog("Terminal::CloseChannels Begin.");
    if (mChannels_.empty()) {
        return;
    }

    std::vector<std::shared_ptr<SeChannel>> channels;
    std::map<int, std::shared_ptr<SeChannel>>::iterator iter = mChannels_.begin();
    while (iter != mChannels_.end()) {
        channels.push_back(iter->second);
        ++iter;
    }
    // close all the se channels
    for (std::shared_ptr<SeChannel>& var : channels) {
        var->Close();
    }
    InfoLog("Terminal::CloseChannels End.");
}

/**
 * @brief Closes the terminal.
 */
void Terminal::Close()
{
    std::lock_guard<std::recursive_mutex> lock(mLock_);
    InfoLog("Terminal::Close Begin.");
    if (!mSEHal_) {
        try {
            mSEHal_->AsObject()->RemoveDeathRecipient(mDeathRecipient_);
        } catch (const RemoteError& e) {
        }
    }
    InfoLog("Terminal::Close End.");
}

/**
 * @brief Get the ATR of the Secure Element
 * @return  Returns the ATR of the Secure Element, or null if not available.
 */
std::string Terminal::GetAtr()
{
    if (!mIsConnected_) {
        return std::string{};
    }

    try {
        std::string response = mSEHal_->GetAtr();
        if (DEBUG) {
            InfoLog("ATR : %s", CommonUtils::BinToHex(response).c_str());
        }
        return response;
    } catch (const RemoteError& e) {
        InfoLog("Exception in getAtr() %s", e.what());
        return std::string{};
    }
}

/**
 * @brief Selects the default application on the basic channel.
 */
void Terminal::SelectDefaultApplication()
{
    try {
        SelectApplet("");
    } catch (const NoSuchElementError& e) {
        if (!mAccessControlEnforcer_) {
            return;
        }
        try {
            SelectApplet(mAccessControlEnforcer_->GetDefaultAccessControlAid());
        } catch (const std::exception& ignore) {
            DebugLog("ignore exception: %s", ignore.what());
        }
    } catch (const std::exception& ignore) {
        DebugLog("ignore exception: %s", ignore.what());
    }
}

void Terminal::SelectApplet(const std::string& aid)
{
    int commandSize = (aid.empty() ? 0 : aid.length()) + 5;
    std::string cmd(commandSize, 0x00);
    cmd[1] = (char)0xA4;
    cmd[2] = 0x04;
    if (!aid.empty()) {
        cmd[4] = (char)aid.length();
        cmd.replace(5, aid.length(), aid);
    }

    // Transmit Apdu Command
    std::string resp = Transmit(cmd);
    int sw1 = resp[resp.length() - 2] & 0xFF;
    int sw2 = resp[resp.length() - 1] & 0xFF;
    if (sw1 != 0x90 || sw2 != 0x00) {
        throw NoSuchElementError("Status word is incorrect");
    }
}

/**
 * @brief Opens a Basic SeChannel with the given AID and P2 paramters
 * @param aid the SE application ID
 * @param p2 the parameter 2 of APUD Command header
 * @param bundleName the bundle name of the calling application
 * @param pid the calling application project id.
 * @return the opened Basic SeChannel instance
 */
std::shared_ptr<SeChannel> Terminal::OpenBasicChannel(std::string aid, char p2, std::string bundleName, int pid)
{
    if (aid.empty()) {
    } else if (aid.length() < 5 || aid.length() > 16) {
        throw IllegalArgumentError("AID out of range");
    } else if (!mIsConnected_) {
        throw IOError("SE is not connected");
    }

    std::shared_ptr<ChannelAccessRule> channelAccessRule{};
    if (!bundleName.empty()) {
        WarnLog("Enable access control on basic channel for %s", bundleName.c_str());
        try {
            channelAccessRule = EstablishChannelAccess(aid, bundleName, pid);
        } catch (const MissingResourceError& e) {
            return std::shared_ptr<SeChannel>{};
        }
    }

    std::lock_guard<std::recursive_mutex> lock(mLock_);
    if (mChannels_.find(0) != mChannels_.end()) {
        ErrorLog("The Basic SeChannel is in use");
        return std::shared_ptr<SeChannel>{};
    }
    if (aid.empty() && !mDefaultAppOnBasicChannel_) {
        ErrorLog("The Default application is not selected");
        return std::shared_ptr<SeChannel>{};
    }

    std::unique_ptr<SEBasicRespData> respData{};
    try {
        respData = mSEHal_->OpenBasicChannel(aid.c_str(), p2);
    } catch (const RemoteError& e) {
        throw IOError(e.what());
    }

    if (!respData) {
        throw IOError("Failed To Open Basic SeChannel.");
    }
    std::string resp = respData->resp;
    SecureElementStatus status = respData->status;
    if (status == SEStatus::CHANNEL_NOT_AVAILABLE) {
        return std::shared_ptr<SeChannel>{};
    } else if (status == SEStatus::UNSUPPORTED_OPERATION) {
        throw UnsupportedOperationError("Failed To Open Basic SeChannel.");
    } else if (status == SEStatus::IO_ERROR) {
        throw IOError("Failed To Open Basic SeChannel.");
    } else if (status == SEStatus::NO_SUCH_SE_ERROR) {
        throw NoSuchElementError("Failed To Open Basic SeChannel.");
    }

    std::shared_ptr<SeChannel> basicChannel =
        std::shared_ptr<SeChannel>(new SeChannel(shared_from_this(), 0, resp, aid));
    basicChannel->Initialize(channelAccessRule);
    if (!aid.empty()) {
        mDefaultAppOnBasicChannel_ = false;
    }
    mChannels_[0] = basicChannel;
    return basicChannel;
}

/**
 * @brief Opens a logical SeChannel for Access Control enforece to be used to get Aeccess Rules.
 * @param aid the SE application ID
 * @return the opened logical SeChannel instance
 */
std::shared_ptr<SeChannel> Terminal::OpenLogicalChannelForAccessControl(std::string aid)
{
    std::string bundleName{};
    return OpenLogicalChannel(aid, (char)0x00, bundleName, 0);
}

/**
 * @brief Opens a logical SeChannel with AID.
 * @param aid the SE application ID
 * @param p2 the parameter 2 of APUD Command header
 * @param bundleName the bundle name of the calling application
 * @param pid the calling application project id.
 * @return the opened logical SeChannel instance
 */
std::shared_ptr<SeChannel> Terminal::OpenLogicalChannel(const std::string& aid,
                                                        char p2,
                                                        const std::string& bundleName,
                                                        int pid)
{
    if (aid.empty()) {
    } else if (aid.length() < 5 || aid.length() > 16) {
        throw IllegalArgumentError("AID out of range");
    } else if (!mIsConnected_) {
        throw IOError("Secure Element is not connected");
    }

    std::shared_ptr<ChannelAccessRule> channelAccessRule{};
    if (!bundleName.empty()) {
        WarnLog("Enable access control on logical channel for %s", bundleName.c_str());
        try {
            channelAccessRule = EstablishChannelAccess(aid, bundleName, pid);
        } catch (const MissingResourceError& e) {
            return std::shared_ptr<SeChannel>();
        }
    }

    std::lock_guard<std::recursive_mutex> lock(mLock_);
    std::unique_ptr<SELogicalRespData> respData{};

    try {
        respData = mSEHal_->OpenLogicalChannel(aid, p2);
    } catch (const RemoteError& e) {
        throw IOError(e.what());
    }

    if (!respData) {
        throw IOError("Failed To Open Logical SeChannel.");
    }

    std::string resp = respData->resp;
    SecureElementStatus status = respData->status;
    if (status == SecureElementStatus::CHANNEL_NOT_AVAILABLE) {
        return std::shared_ptr<SeChannel>();
    } else if (status == SecureElementStatus::UNSUPPORTED_OPERATION) {
        throw UnsupportedOperationError("OpenLogicalChannel() failed");
    } else if (status == SecureElementStatus::IO_ERROR) {
        throw IOError("OpenLogicalChannel() failed");
    } else if (status == SecureElementStatus::NO_SUCH_SE_ERROR) {
        throw NoSuchElementError("OpenLogicalChannel() failed");
    }
    int channelNumber = respData->channelNumber;
    if (channelNumber <= 0 || status != SecureElementStatus::SUCCESS) {
        return std::shared_ptr<SeChannel>();
    }
    std::shared_ptr<SeChannel> logicalChannel =
        std::shared_ptr<SeChannel>(new SeChannel(shared_from_this(), channelNumber, resp, aid));
    logicalChannel->Initialize(channelAccessRule);

    mChannels_[channelNumber] = logicalChannel;
    return logicalChannel;
}

/**
 * @brief Check if the given AID can be selected on the Terminal
 * @param aid the SE application ID
 * @return Returns true if the given AID can be selected on the Terminal
 */
bool Terminal::IsAidSelectable(const std::string& aid)
{
    if (aid.empty()) {
        throw NullPointerError("aid must not be null");
    } else if (!mIsConnected_) {
        ErrorLog("Secure Element is not connected");
        return false;
    }

    std::lock_guard<std::recursive_mutex> lock(mLock_);
    std::unique_ptr<SELogicalRespData> respData{};
    try {
        respData = mSEHal_->OpenLogicalChannel(aid, 0);
        std::string resp = respData->resp;
        SecureElementStatus status = respData->status;
        if (status == SecureElementStatus::SUCCESS) {
            mSEHal_->CloseChannel(respData->channelNumber);
            return true;
        }
        return false;
    } catch (const RemoteError& e) {
        ErrorLog("Error in isAidSelectable() returning false %s", e.what());
        return false;
    }
}

/**
 * @brief Transmits the specified command and returns the response.
 * @param cmd the command APDU to be transmitted.
 * @return the response received.
 */
std::string Terminal::Transmit(std::string cmd)
{
    if (!mIsConnected_) {
        ErrorLog("Secure Element is not connected");
        throw IOError("Secure Element is not connected");
    }

    std::string rsp = TransmitInternal(cmd);
    if (rsp.length() < 2) {
        throw UnsupportedOperationError("Transmit failed: Response length is less then 2");
    }
    int sw1 = rsp[rsp.length() - 2] & 0xFF;
    int sw2 = rsp[rsp.length() - 1] & 0xFF;

    if (sw1 == 0x6C) {
        cmd[cmd.length() - 1] = rsp[rsp.length() - 1];
        rsp = Transmit(cmd);
    } else if (sw1 == 0x61) {
        do {
            std::string tempCmd{cmd};
            tempCmd.append({(char)0xC0, 0x00, 0x00, (char)sw2});
            std::string tmpRsp = TransmitInternal(tempCmd);
            rsp = rsp.substr(0, rsp.length() - 2);
            rsp.append(tmpRsp);
            sw1 = rsp[rsp.length() - 2] & 0xFF;
            sw2 = rsp[rsp.length() - 1] & 0xFF;
        } while (sw1 == 0x61);
    }
    return rsp;
}

std::string Terminal::TransmitInternal(const std::string& cmd)
{
    std::string response;
    try {
        response = mSEHal_->Transmit(cmd);
    } catch (const RemoteError& e) {
        throw IOError(e.what());
    }
    if (response.empty()) {
        throw IOError("Error in transmit()");
    }
    if (DEBUG) {
        InfoLog("Sent : %s", CommonUtils::BinToHex(cmd).c_str());
        InfoLog("Received : %s", CommonUtils::BinToHex(response).c_str());
    }
    return response;
}

/**
 * @brief Checks if the application is authorized to receive the transaction
 * event.
 * @param bundleManager bundle manager
 * @param aid the SE application ID
 * @param bundleNames about the bundle names
 * @return the true/false list
 */
std::vector<bool> Terminal::IsNfcEventAllowed(std::weak_ptr<osal::BundleManager> bundleManager,
                                              std::string aid,
                                              std::vector<std::string> bundleNames)
{
    if (!mAccessControlEnforcer_ || mAccessControlEnforcer_->IsNoRuleFound()) {
        /**
         * If it failed due to a kind of temporary failure or no rule was found in the previous attempt. Attempt to
         * initialize the access control enforcer.
         */
        try {
            InitializeAC();
        } catch (const std::exception& e) {
            InfoLog("isNfcEventAllowed Exception: %s", e.what());
            return std::vector<bool>();
        }
    }
    mAccessControlEnforcer_->SetBundleManager(bundleManager);

    {
        std::lock_guard<std::recursive_mutex> lock(mLock_);
        try {
            return mAccessControlEnforcer_->IsNfcEventAllowed(aid, bundleNames);
        } catch (const std::exception& e) {
            InfoLog("isNfcEventAllowed Exception: %s", e.what());
            return std::vector<bool>();
        }
    }
}

/**
 * @brief  Check if the Secure Element is present
 * @return Returns true if the Secure Element is present
 */
bool Terminal::IsSecureElementPresent()
{
    try {
        return mSEHal_->IsCardPresent();
    } catch (const RemoteError& e) {
        ErrorLog("Error in isSecureElementPresent() %s", e.what());
        return false;
    }
}

/**
 * @brief Checks if Carrier Privilege exists for the given bundle
 * @param pInfo
 * @return true - exist; false - unexist
 */
bool Terminal::CheckCarrierPrivilegeRules(std::weak_ptr<BundleInfo> bundleInfo)
{
    bool isRefreshTag = true;
    if (!mAccessControlEnforcer_ || mAccessControlEnforcer_->IsNoRuleFound()) {
        try {
            InitializeAC();
        } catch (const IOError& e) {
            return false;
        }
        isRefreshTag = false;
    }
    mAccessControlEnforcer_->SetBundleManager(mContext_.lock()->GetBundleManager());

    try {
        return mAccessControlEnforcer_->CheckCarrierPrivilege(bundleInfo, isRefreshTag);
    } catch (const std::exception& e) {
        InfoLog("Check Carrier Privilege Exception: %s", e.what());
        return false;
    }
}

bool Terminal::Reset()
{
    try {
        SecureElementStatus status = mSEHal_->Reset();
        return (SecureElementStatus::SUCCESS == status);
    } catch (const RemoteError& e) {
        ErrorLog("Error in isSecureElementPresent() %s", e.what());
        return false;
    }
}

void Terminal::ResetDefaultAppState()
{
    std::lock_guard<std::recursive_mutex> lock(mLock_);
    mDefaultAppOnBasicChannel_ = true;
}

std::weak_ptr<AccessControlEnforcer> Terminal::GetAccessControlEnforcer()
{
    return mAccessControlEnforcer_;
}

std::weak_ptr<osal::Context> Terminal::GetContext()
{
    return mContext_;
}

Terminal::Terminal(std::string name, std::weak_ptr<osal::Context> context)
    : mName_(name), mContext_(context), mHandler_(nullptr)
{
}
}  // namespace se
}  // namespace OHOS