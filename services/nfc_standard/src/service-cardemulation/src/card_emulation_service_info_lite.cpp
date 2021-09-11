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

#include "card_emulation_service_info_lite.h"

#include "element_name.h"
namespace OHOS::nfc::sdk::cardemulation {
CardEmulationServiceInfoLite::CardEmulationServiceInfoLite(const OHOS::AppExecFwk::ElementName& name,
                                                           std::string label,
                                                           std::string icon,
                                                           std::string executionEnvironment)
    : name_(std::make_unique<OHOS::AppExecFwk::ElementName>(name.GetDeviceID(),
                                                          name.GetBundleName(),
                                                          name.GetAbilityName())),
    label_(std::move(label)),
    icon_(std::move(icon)),
    executionEnvironment_(std::move(executionEnvironment))
{
}
CardEmulationServiceInfoLite::CardEmulationServiceInfoLite(const CardEmulationServiceInfoLite& other)
{
    Copy(other);
}
CardEmulationServiceInfoLite& CardEmulationServiceInfoLite::operator=(const CardEmulationServiceInfoLite& other)
{
    if (this != &other) {
        Copy(other);
    }

    return *this;
}
CardEmulationServiceInfoLite::CardEmulationServiceInfoLite(CardEmulationServiceInfoLite&& other)
{
    Move(std::move(other));
}
CardEmulationServiceInfoLite& CardEmulationServiceInfoLite::operator=(CardEmulationServiceInfoLite&& other)
{
    if (this != &other) {
        Move(std::move(other));
    }
    return *this;
}
bool CardEmulationServiceInfoLite::operator==(const CardEmulationServiceInfoLite& other) const
{
    if (this == &other) {
        return true;
    }
    if (!name_ && !other.name_) {
        return true;
    }
    if (name_ && other.name_) {
        return name_->operator==(*other.name_);
    }
    return false;
}
bool CardEmulationServiceInfoLite::operator!=(const CardEmulationServiceInfoLite& other) const
{
    return !operator==(other);
}
OHOS::AppExecFwk::ElementName CardEmulationServiceInfoLite::GetName() const
{
    if (name_) {
        return *name_;
    }
    return OHOS::AppExecFwk::ElementName();
}

std::string CardEmulationServiceInfoLite::GetLabel() const
{
    return label_;
}

std::string CardEmulationServiceInfoLite::GetIcon() const
{
    return icon_;
}

std::string CardEmulationServiceInfoLite::GetExecutionEnvironment() const
{
    return executionEnvironment_;
}

bool CardEmulationServiceInfoLite::Marshalling(OHOS::Parcel& parcel) const
{
    parcel.WriteParcelable(name_.get());
    parcel.WriteString(label_);
    parcel.WriteString(icon_);
    parcel.WriteString(executionEnvironment_);
    return true;
}

bool CardEmulationServiceInfoLite::ReadFromParcel(OHOS::Parcel& parcel)
{
    name_ = std::unique_ptr<OHOS::AppExecFwk::ElementName>(parcel.ReadParcelable<OHOS::AppExecFwk::ElementName>());
    return parcel.ReadString(label_) && parcel.ReadString(icon_) && parcel.ReadString(executionEnvironment_);
}

CardEmulationServiceInfoLite* CardEmulationServiceInfoLite::Unmarshalling(OHOS::Parcel& parcel)
{
    auto info = std::make_unique<CardEmulationServiceInfoLite>();
    if (info && info->ReadFromParcel(parcel)) {
        return info.release();
    }
    return nullptr;
}
void CardEmulationServiceInfoLite::Copy(const CardEmulationServiceInfoLite& other)
{
    using namespace OHOS::AppExecFwk;
    ElementName name = other.GetName();

    name_ = std::make_unique<ElementName>(name.GetDeviceID(), name.GetBundleName(), name.GetAbilityName());

    label_ = other.label_;
    icon_ = other.icon_;
    executionEnvironment_ = other.executionEnvironment_;
}
void CardEmulationServiceInfoLite::Move(CardEmulationServiceInfoLite&& other)
{
    if (this == &other) {
        return;
    }
    name_ = std::move(other.name_);
    label_ = std::move(other.label_);
    icon_ = std::move(other.icon_);
    executionEnvironment_ = std::move(other.executionEnvironment_);
}
}  // namespace OHOS::nfc::sdk::cardemulation
