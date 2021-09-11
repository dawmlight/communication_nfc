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


#ifndef ICARD_EMULATION_SERVICE
#define ICARD_EMULATION_SERVICE

#include <memory>
#include <string>
#include <vector>

#include "icard_emulation_event_handler.h"
#include "infc_event_handler.h"
#include "iremote_object.h"
#include "iuser_switch_handler.h"
#include "refbase.h"
namespace OHOS::AppExecFwk {
class ElementName;
}

namespace OHOS::nfc::sdk::cardemulation {
class CardEmulationServiceInfoLite;
}
namespace OHOS::nfc::cardemulation {
class AidSet;
class CardEmulationServiceInfo;
class ICardEmulationService : public ICardEmulationEventHandler, public IUserSwitchHandler, public INfcEventHandler {
public:
    virtual ~ICardEmulationService() = default;
    /**
     * brief: initialize the card emulation service
     * parameter: void
     * return: 0 - succeeded, not 0 -- error code
     */
    virtual int Init() = 0;
    /**
     * brief: deinitialize the card emulation service
     * parameter: void
     * return: void
     */
    virtual int Deinit() = 0;
    /**
     * brief: Checks whether the specified service is the default service by element name and type
     * parameter:
     *   userId -- user id
     *   elementName -- the card emulation service name
     *   type -- service type
     * return: true -- is default(primary), false -- is not default(primary)
     */
    virtual bool IsDefaultServiceForType(int userId,
                                         const OHOS::AppExecFwk::ElementName& elementName,
                                         const std::string& type) const = 0;
    /**
     * brief: Checks whether the specified service is the default service by element name and aid
     * parameter:
     *   userId -- user id
     *   elementName -- the card emulation service name
     *   aid -- aid
     * return: true -- is default(primary), false -- is not default(primary)
     */
    virtual bool IsDefaultServiceForAid(int userId,
                                        const OHOS::AppExecFwk::ElementName& elementName,
                                        const std::string& aid) const = 0;
    /**
     * brief: get the primary card emulation service by type
     * parameter:
     *   userId -- user id
     *   type -- service type
     * return: card emulation service name
     */
    virtual OHOS::AppExecFwk::ElementName GetPrimaryServiceForType(int userId, const std::string& type) = 0;
    /**
     * brief: set the primary card emulation service by type
     * parameter:
     *   userId -- user id
     *   elementName -- card emulation service name
     *   type -- service type
     * return: 0 -- succeeded, not 0 -- error code
     */
    virtual int SetPrimaryServiceForType(int userId,
                                         const OHOS::AppExecFwk::ElementName& elementName,
                                         const std::string& type) = 0;
    /**
     * brief: register remote object with the NFC service
     * parameter:
     *   userId -- user id
     *   elementName -- card emulation service name
     *   apduChannel -- apdu channel object
     * return: 0 -- succeeded, not 0 -- error code
     */
    virtual int RegisterRemoteObject(int userId,
                                     const OHOS::AppExecFwk::ElementName& serviceName,
                                     const OHOS::sptr<OHOS::IRemoteObject>& apduChannel) = 0;
    /**
     * brief: add aids for the card emulation service
     * parameter:
     *   userId -- user id
     *   elementName -- card emulation service name
     *   type -- service type
     *   aidSet -- aids
     * return: 0 -- succeeded, not 0 -- error code
     */
    virtual int AddAidsForService(int userId,
                                  const OHOS::AppExecFwk::ElementName& elementName,
                                  const std::string& type,
                                  const std::vector<std::string>& aidSet) = 0;
    /**
     * brief: remove aids for the card emulation service
     * parameter:
     *   userId -- user id
     *   elementName -- card emulation service name
     *   type -- service type
     * return: 0 -- succeeded, not 0 -- error code
     */
    virtual int RemoveAidsForService(int userId,
                                     const OHOS::AppExecFwk::ElementName& elementName,
                                     const std::string& type) = 0;
    /**
     * brief: register  the offhost card emulation service execution environment
     * parameter:
     *   userId -- user id
     *   elementName -- card emulation service name
     *   offHostSecureElement -- secure element name (execution environment)
     * return: 0 -- succeeded, not 0 -- error code
     */
    virtual int MarkOffHostForService(int userId,
                                      const OHOS::AppExecFwk::ElementName& elementName,
                                      const std::string& offHostSecureElement) = 0;
    /**
     * brief: unregister the offhost card emulation service execution environment
     * parameter:
     *   userId -- user id
     *   elementName -- card emulation service name
     * return: 0 -- succeeded, not 0 -- error code
     */
    virtual int UnmarkOffHostForService(int userId, const OHOS::AppExecFwk::ElementName& elementName) = 0;
    /**
     * brief: get aids for the card emulation service
     * parameter:
     *   userId -- user id
     *   elementName -- card emulation service name
     *   type -- service type
     * return: aids of card emulation service
     */
    virtual std::vector<std::string> GetAidsForService(int userId,
                                                       const OHOS::AppExecFwk::ElementName& elementName,
                                                       const std::string& type) const = 0;
    /**
     * brief: get the card emulation service description 
     * parameter:
     *   userId -- user id
     *   type -- service type
     * return: description
     */
    virtual std::string GetDescriptionForPreferredService(int userId, std::string type) = 0;
    /**
     * brief: set preferred foreground service
     * parameter:
     *   userId -- user id
     *   elementName -- card emulation service name
     * return: 0 -- succeeded, not 0 -- error code
     */
    virtual int SetPreferredService(int userId, const OHOS::AppExecFwk::ElementName& elementName) = 0;
    /**
     * brief: unset preferred foreground service
     * parameter:
     *   userId -- user id
     * return: 0 -- succeeded, not 0 -- error code
     */
    virtual int UnsetPreferredService(int userId) = 0;
    /**
     * brief: whether the NCI supports prefix mode
     * parameter: void
     * return: true -- supported, false -- not supported
     */
    virtual bool SupportsAidPrefix() const = 0;
    /**
     * brief: get the card emulation service infomations by type
     * parameter:
     *   userId -- user id
     *   type -- service type
     * return: card emulation service infomations
     */
    virtual std::vector<std::unique_ptr<OHOS::nfc::sdk::cardemulation::CardEmulationServiceInfoLite>> GetServicesByType(
        int userId,
        const std::string& type) = 0;
};
}  // namespace OHOS::nfc::cardemulation

#endif  // ICARD_EMULATION_SERVICE
