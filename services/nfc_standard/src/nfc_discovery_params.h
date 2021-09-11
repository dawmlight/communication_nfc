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
#ifndef NFC_DISCONVERY_PARAMS_H
#define NFC_DISCONVERY_PARAMS_H

#include <memory>
namespace OHOS{
namespace nfc {
class NfcDiscoveryParams final {
public:
    class Builder final {
    public:
        Builder();
        ~Builder();
        Builder(const Builder& manager) = delete;
        const Builder& operator=(const Builder& manager) = delete;

        void SetTechMask(int techMask) const;
        void SetEnableReaderMode(bool enable) const;
        void SetEnableHostRouting(bool enable) const;
        std::unique_ptr<NfcDiscoveryParams> Build();

    private:
        std::unique_ptr<NfcDiscoveryParams> mParameters_;

        friend class NfcDiscoveryParams;
    };
    bool operator==(const NfcDiscoveryParams& params) const;
    static std::unique_ptr<NfcDiscoveryParams> GetNfcOffParameters();
    int GetTechMask() const;
    bool ShouldEnableLowPowerDiscovery() const;
    bool ShouldEnableReaderMode() const;
    bool ShouldEnableHostRouting() const;
    bool ShouldEnableDiscovery() const;

    ~NfcDiscoveryParams() {}
    NfcDiscoveryParams();

private:
    int mTechMask_;
    bool mEnableLowPowerDiscovery_;
    bool mEnableReaderMode_;
    bool mEnableHostRouting_;
};
}  // namespace nfc
}  // namespace OHOS
#endif  // !NFC_DISCONVERY_PARAMS_H
