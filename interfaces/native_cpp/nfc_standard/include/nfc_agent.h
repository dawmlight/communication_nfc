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
#ifndef NFC_AGENT_H
#define NFC_AGENT_H

#include <iostream>
#include <map>
#include <memory>
#include <mutex>

#include "app_callback_stub.h"

namespace OHOS {
namespace AppExecFwk {
class Ability;
class OHOSApplication;
class ElementName;
}  // namespace AppExecFwk
using AppExecFwk::Ability;
using AppExecFwk::OHOSApplication;

namespace nfc {
class INfcAgentService;
class NfcServiceProxy;
namespace reader {
class ITagSession;
}

namespace sdk {
class NfcAbilityManager;
class Tag;
class NfcMap;
namespace cardemulation {
class ICardEmulationAgent;
class IHostCardEmulation;
class IOffHostCardEmulation;
}  // namespace cardemulation

static const std::string NFC_SERVICE_NAME = "nfc";
class NfcAgent final {
public:
    class TagCallback {
    public:
        virtual ~TagCallback() {}
        /**
         * @Description In reader mode, a callback for tag discoved to the foreground ability. Application needs to
         * inherit the implementation to handle the tag discovry.
         * @param tag compatible with all types of tag
         * @return void
         */
        virtual void CBTagDiscoverCommond(std::unique_ptr<sdk::Tag> tag) = 0;
        /**
         * @Description when the tag is ignored, this callback to be invoked to the foreground ability.Application needs
         * to inherit the implementation to handle the tag removed.
         * @param tag compatible with all types of tag
         * @return void
         */
        virtual void CBTagRemovedMsg() = 0;
    };

    class TagRemovedCallback : public TagRemovedCallbackStub {
    public:
        explicit TagRemovedCallback(std::weak_ptr<NfcAgent::TagCallback> callback) : mCallback_(callback) {}
        ~TagRemovedCallback() {}
        void OnTagRemoved() override;

    private:
        std::weak_ptr<NfcAgent::TagCallback> mCallback_;
    };

public:
    explicit NfcAgent();
    ~NfcAgent();

    /**
     * @Description Get an object of nfc agent for an Application.
     * @param application the application that get nfc agent
     * @return an object of nfc agent
     */
    static std::shared_ptr<NfcAgent> GetNFCAgent(std::weak_ptr<OHOSApplication> application);
    /**
     * @Description Check device is support Nfc.
     * @param void
     * @return return ture if device support nfc, otherwise return false.
     */
    static bool HasNfcCapability();
    /**
     * @Description Check device is support Nfc HCE.
     * @param void
     * @return return ture if device support Nfc HCE, otherwise return false.
     */
    static bool HasNfcHceCapability();
    /**
     * @Description Get an interface of nfc card emulation service.
     * @param void
     * @return OHOS::wptr<ICardEmulationAgent>
     */
    OHOS::wptr<cardemulation::ICardEmulationAgent> GetNfcCardEmulationService();
    /**
     * @Description Create an interface of nfc host card emulation service.
     * @param void
     * @return std::shared_ptr<IHostCardEmulation>
     */
    std::shared_ptr<cardemulation::IHostCardEmulation> CreateHostCardEmulation(
        const OHOS::AppExecFwk::ElementName& name);
    std::shared_ptr<cardemulation::IOffHostCardEmulation> CreateOffHostCardEmulation(
        const OHOS::AppExecFwk::ElementName& name);
    /**
     * @Description Turn on Nfc of the device.
     * @param void
     * @return Errorcode of turn on nfc. if return 0, means successful.
     */
    int ActiveNfc();
    /**
     * @Description Turn off Nfc of the device.
     * @param void
     * @return Errorcode of turn off nfc. if return 0, means successful.
     */
    int DeactiveNfc();
    /**
     * @Description Get nfc state of device.
     * @param void
     * @return nfc state.
     */
    static int GetNfcState();
    /**
     * @Description Turn on Nfc secure of the device.
     * @param void
     * @return Errorcode of turn on nfc secure. if return 0, means successful.
     */
    int ActiveNfcSecure();
    /**
     * @Description Turn off Nfc secure of the device.
     * @param void
     * @return Errorcode of turn off nfc secure. if return 0, means successful.
     */
    int DeactiveNfcSecure();
    /**
     * @Description Get nfc secure state of device.
     * @param void
     * @return nfc secure state.
     */
    static int GetNfcSecureState();
    /**
     * @Description Check device is support Nfc secure.
     * @param void
     * @return return ture if device support Nfc secure, otherwise return false.
     */
    bool HasSecureNfcCapability();
    /**
     * @Description Pause the polling on the device.
     * @param timeoutInMs time to puase
     * @return void
     */
    void PausePolling(int timeoutInMs);
    /**
     * @Description Resume the polling on the device.
     * @param void
     * @return void
     */
    void ResumePolling();
    /**
     * @Description Trun on reader mode when the ability is in the foreground.
     * @param ability the ability that need to turn on reader mode
     * @param callback the callback will be invoke when a tag discovered
     * @param tagTypeFilter the polling type filter of the reader mode
     * @param extras the extras configs of the reader mode
     * @return void
     */
    int ActiveNfcReaderMode(std::weak_ptr<Ability> ability,
                            std::weak_ptr<NfcAgent::TagCallback> callback,
                            int tagTypeFilter,
                            std::weak_ptr<NfcMap> extras);
    /**
     * @Description Trun off reader mode of the ability.Resume the default polling
     * @param ability the ability that need to turn off reader mode
     * @return Errorcode of turn off reader mode. if return 0, means successful.
     */
    int DeactiveNfcReaderMode(std::weak_ptr<Ability> ability);
    /**
     * @Description Set ignore a tag for applitcation.
     * @param tag the tag that need to ignore
     * @param callback the callback will be invoke when a tag removed
     * @param debounceMs the time that need to ignore
     * @return Errorcode of ignore tag. if return 0, means successful.
     */
    bool IgnoreTag(std::weak_ptr<Tag> tag, std::weak_ptr<NfcAgent::TagCallback> callback, int debounceMs);

private:
    static std::shared_ptr<NfcAgent> InitNFCAgent(std::weak_ptr<OHOSApplication> application);

private:
    static bool mbInitialized_;
    static bool mbHasNfcCapability_;

    static std::shared_ptr<NfcServiceProxy> nfcServiceProxy_;
    static std::weak_ptr<OHOS::nfc::INfcAgentService> mNfcAgentService_;
    static OHOS::sptr<OHOS::nfc::reader::ITagSession> mTagSession_;
    static OHOS::sptr<cardemulation::ICardEmulationAgent> mNfcCardEmulationService_;
    static std::mutex mMutex_;

    std::shared_ptr<NfcAbilityManager> mNfcAbilityManager_;
    static std::
        map<std::weak_ptr<OHOSApplication>, std::weak_ptr<NfcAgent>, std::owner_less<std::weak_ptr<OHOSApplication>>>
            mNfcAgents_;
    friend class NfcAbilityManager;
};
}  // namespace sdk
}  // namespace nfc
}  // namespace OHOS
#endif  // NFC_AGENT_H
