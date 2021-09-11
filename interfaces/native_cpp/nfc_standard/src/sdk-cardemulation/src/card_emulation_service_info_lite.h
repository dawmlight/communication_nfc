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

    OHOS::AppExecFwk::ElementName GetName() const;
    std::string GetLabel() const;
    std::string GetIcon() const;
    std::string GetExecutionEnvironment() const;

    bool Marshalling(OHOS::Parcel& parcel) const override;
    bool ReadFromParcel(OHOS::Parcel& parcel);
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
