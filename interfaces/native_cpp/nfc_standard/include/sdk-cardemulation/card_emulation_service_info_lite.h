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

#ifndef SDK_CARD_EMULATION_SERVICE_INFO_LITE_H
#define SDK_CARD_EMULATION_SERVICE_INFO_LITE_H

#include <memory>
#include <string>

#include "parcel.h"
namespace OHOS::AppExecFwk {
class ElementName;
}

namespace OHOS::nfc::sdk::cardemulation {
class CardEmulationServiceInfoLite final : public OHOS::Parcelable {
public:
    CardEmulationServiceInfoLite() = default;
    ~CardEmulationServiceInfoLite() = default;
    CardEmulationServiceInfoLite(const OHOS::AppExecFwk::ElementName& name,
                                 std::string label,
                                 std::string description,
                                 std::string executionEnvironment);

    CardEmulationServiceInfoLite(const CardEmulationServiceInfoLite& other);
    CardEmulationServiceInfoLite& operator=(const CardEmulationServiceInfoLite& other);
    CardEmulationServiceInfoLite(CardEmulationServiceInfoLite&& other);
    CardEmulationServiceInfoLite& operator=(CardEmulationServiceInfoLite&& other);

    bool operator==(const CardEmulationServiceInfoLite& other) const;
    bool operator!=(const CardEmulationServiceInfoLite& other) const;

    /**
     * brief: get the card emulation service name
     * parameter: void
     * return: card emulation service name
     */
    OHOS::AppExecFwk::ElementName GetName() const;
    /**
     * brief: get the card emulation service label
     * parameter: void
     * return: card emulation service label
     */
    std::string GetLabel() const;
    /**
     * brief: get the card emulation service icon
     * parameter: void
     * return: card emulation service icon
     */
    std::string GetIcon() const;
    /**
     * brief: get the card emulation service execution environment
     * parameter: void
     * return: card emulation service execution environment
     */
    std::string GetExecutionEnvironment() const;

    /**
     * brief: marshalling infomation 
     * parameter: 
     *   parcel[out] -- parcel object 
     * return: true -- succeeded, false -- failed
     */
    bool Marshalling(OHOS::Parcel& parcel) const override;
    /**
     * brief: read infomation from parcel object
     * parameter: 
     *   parcel[in] -- parcel object
     * return: true -- succeeded, false -- failed
     */
    bool ReadFromParcel(OHOS::Parcel& parcel);
    /**
     * brief: unmarshalling infomation from parcel object
     * parameter: 
     *   parcel[in] -- parcel object
     * return: a CardEmulationServiceInfoLite pointer(may be nullptr). Released by the caller. 
     */
    static CardEmulationServiceInfoLite* Unmarshalling(OHOS::Parcel& parcel);

private:
    void Copy(const CardEmulationServiceInfoLite& other);
    void Move(CardEmulationServiceInfoLite&& other);

private:
    std::unique_ptr<OHOS::AppExecFwk::ElementName> name_;
    std::string label_;
    std::string icon_;
    std::string executionEnvironment_;
};
}  // namespace OHOS::nfc::sdk::cardemulation
#endif
