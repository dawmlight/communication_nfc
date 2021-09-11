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
#include "nfc_discovery_params.h"

namespace OHOS{
namespace nfc {
NfcDiscoveryParams::NfcDiscoveryParams()
    : mTechMask_(0), mEnableLowPowerDiscovery_(false), mEnableReaderMode_(false), mEnableHostRouting_(false)
{
}

bool NfcDiscoveryParams::operator==(const NfcDiscoveryParams& params) const
{
    return mTechMask_ == params.mTechMask_ && (mEnableLowPowerDiscovery_ == params.mEnableLowPowerDiscovery_) &&
           (mEnableReaderMode_ == params.mEnableReaderMode_) && (mEnableHostRouting_ == params.mEnableHostRouting_);
}

std::unique_ptr<NfcDiscoveryParams> NfcDiscoveryParams::GetNfcOffParameters()
{
    return std::make_unique<NfcDiscoveryParams>();
}

int NfcDiscoveryParams::GetTechMask() const
{
    return mTechMask_;
}

bool NfcDiscoveryParams::ShouldEnableLowPowerDiscovery() const
{
    return mEnableLowPowerDiscovery_;
}

bool NfcDiscoveryParams::ShouldEnableReaderMode() const
{
    return mEnableReaderMode_;
}

bool NfcDiscoveryParams::ShouldEnableHostRouting() const
{
    return mEnableHostRouting_;
}

bool NfcDiscoveryParams::ShouldEnableDiscovery() const
{
    return mTechMask_ != 0 || mEnableHostRouting_;
}

NfcDiscoveryParams::Builder::Builder() : mParameters_(std::make_unique<NfcDiscoveryParams>()) {}

NfcDiscoveryParams::Builder::~Builder()
{
    mParameters_ = nullptr;
}

std::unique_ptr<NfcDiscoveryParams> NfcDiscoveryParams::Builder::Build()
{
    return std::move(mParameters_);
}

void NfcDiscoveryParams::Builder::SetTechMask(int techMask) const
{
    mParameters_->mTechMask_ = techMask;
}

void NfcDiscoveryParams::Builder::SetEnableReaderMode(bool enable) const
{
    mParameters_->mEnableReaderMode_ = enable;
    if (enable) {
        mParameters_->mEnableLowPowerDiscovery_ = false;
    }
}

void NfcDiscoveryParams::Builder::SetEnableHostRouting(bool enable) const
{
    mParameters_->mEnableHostRouting_ = enable;
}
}  // namespace nfc
}  // namespace OHOS