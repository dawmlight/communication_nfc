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

#ifndef NFC_LOG_H
#define NFC_LOG_H

#include "hilog/log.h"

namespace OHOS {
namespace Nfc {
static constexpr int NFC_DOMAIN_ID = 0xD001570;
static constexpr OHOS::HiviewDFX::HiLogLabel label = { LOG_CORE, NFC_DOMAIN_ID, "NFC" };

#ifdef HILOGD
#undef HILOGD
#endif

#ifdef HILOGF
#undef HILOGF
#endif

#ifdef HILOGE
#undef HILOGE
#endif

#ifdef HILOGW
#undef HILOGW
#endif

#ifdef HILOGI
#undef HILOGI
#endif

#define HILOGD(...) (void)OHOS::HiviewDFX::HiLog::Debug(label, __VA_ARGS__)
#define HILOGE(...) (void)OHOS::HiviewDFX::HiLog::Error(label, __VA_ARGS__)
#define HILOGF(...) (void)OHOS::HiviewDFX::HiLog::Fatal(label, __VA_ARGS__)
#define HILOGI(...) (void)OHOS::HiviewDFX::HiLog::Info(label, __VA_ARGS__)
#define HILOGW(...) (void)OHOS::HiviewDFX::HiLog::Warn(label, __VA_ARGS__)
} // namespace Nfc
} // namespace OHOS
#endif  // #ifndef NFC_LOG_H