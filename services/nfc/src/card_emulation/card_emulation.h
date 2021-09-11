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

#ifndef OHOS_CARD_EMULATION_H
#define OHOS_CARD_EMULATION_H

#include "iremote_object.h"
#include "nfc_adapter_utils.h"
#include "nfc_cardemulation_adapter_utils.h"

namespace OHOS {
namespace Nfc {
namespace Card {
class CardEmulation {
public:
    static CardEmulation& GetInstance();
    ~CardEmulation();
    sptr<IRemoteObject> GetCardEmulationService();

    /* for listen mode */
    int32_t SetListenMode(MessageParcel& data, MessageParcel& reply);
    int32_t IsListenModeEnabled(MessageParcel& data, MessageParcel& reply);

    /* for get NFCC eSE info */
    int32_t GetNfcInfo(MessageParcel& data, MessageParcel& reply);

    /* for set RF parameter */
    int32_t SetRfConfig(MessageParcel& data, MessageParcel& reply);
    int32_t GetInfoFromConfig(MessageParcel& data, MessageParcel& reply);

    /* for set foreground app */
    int32_t SetForegroundPreferred(MessageParcel& data, MessageParcel& reply);
    int32_t UnsetForegroundPreferred(MessageParcel& data, MessageParcel& reply);

    /* for register aids */
    int32_t IsDefaultForAid(MessageParcel& data, MessageParcel& reply);
    int32_t RegisterAids(MessageParcel& data, MessageParcel& reply);
    int32_t RemoveAids(MessageParcel& data, MessageParcel& reply);
    int32_t GetAids(MessageParcel& data, MessageParcel& reply);
    int32_t IsDefaultForType(MessageParcel& data, MessageParcel& reply);
private:
    static CardEmulation g_cardEmulation;
    sptr<IRemoteObject> g_remoteNfcService;
    sptr<IRemoteObject> g_remoteNxpService;
    sptr<IRemoteObject> g_remoteCEService;
};
} // namespace Card
} // namespace Nfc
} // namespace OHOS
#endif // OHOS_CARD_EMULATION_H
