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

#include "nfc_manager_proxy.h"
#include "ipc_types.h"
#include "nfc_log.h"

namespace OHOS {
namespace Nfc {
int32_t NfcManagerProxy::SetNfcEnabled(MessageParcel& data, MessageParcel& reply)
{
    /* NfcManagerProxy::SetNfcEnabled is unavailable for other module */
    data.ReadInt32();
    reply.ReadInt32();
    return false;
}

int32_t NfcManagerProxy::GetNfcState(MessageParcel& data, MessageParcel& reply)
{
    /* NfcManagerProxy::GetNfcState is unavailable for other module */
    data.ReadInt32();
    reply.ReadInt32();
    return 0;
}

bool NfcManagerProxy::IsNfcAvailable(MessageParcel& data, MessageParcel& reply)
{
    /* NfcManagerProxy::IsNfcAvailable is unavailable for other module */
    data.ReadInt32();
    reply.ReadInt32();
    return false;
}

void NfcManagerProxy::RegisterForegroundDispatch(MessageParcel& data, MessageParcel& reply)
{
    /* NfcManagerProxy::RegisterForegroundDispatch is unavailable for other module */
    data.ReadInt32();
    reply.ReadInt32();
    return;
}

bool NfcManagerProxy::SetReaderMode(MessageParcel& data, MessageParcel& reply)
{
    data.ReadInt32();
    reply.ReadInt32();
    return false;
}

bool NfcManagerProxy::ConnectTag(MessageParcel& data, MessageParcel& reply)
{
    /* NfcManagerProxy::this function is unavailable for other module currently */
    data.ReadInt32();
    reply.ReadInt32();
    return false;
}

bool NfcManagerProxy::IsTagConnected(MessageParcel& data, MessageParcel& reply)
{
    /* NfcManagerProxy::this function is unavailable for other module currently */
    data.ReadInt32();
    reply.ReadInt32();
    return false;
}

bool NfcManagerProxy::SetSendDataTimeout(MessageParcel& data, MessageParcel& reply)
{
    /* NfcManagerProxy::this function is unavailable for other module currently */
    data.ReadInt32();
    reply.ReadInt32();
    return false;
}

int32_t NfcManagerProxy::GetSendDataTimeout(MessageParcel& data, MessageParcel& reply)
{
    /* NfcManagerProxy::this function is unavailable for other module currently */
    data.ReadInt32();
    reply.ReadInt32();
    return 0;
}

int32_t NfcManagerProxy::GetMaxSendLength(MessageParcel& data, MessageParcel& reply)
{
    /* NfcManagerProxy::this function is unavailable for other module currently */
    data.ReadInt32();
    reply.ReadInt32();
    return 0;
}

int32_t NfcManagerProxy::SendData(MessageParcel& data, MessageParcel& reply)
{
    /* NfcManagerProxy::this function is unavailable for other module currently */
    data.ReadInt32();
    reply.ReadInt32();
    return 0;
}

bool NfcManagerProxy::ReconnectTag(MessageParcel& data, MessageParcel& reply)
{
    /* NfcManagerProxy::this function is unavailable for other module currently */
    data.ReadInt32();
    reply.ReadInt32();
    return false;
}

void NfcManagerProxy::ResetTimeouts(MessageParcel& data, MessageParcel& reply)
{
    /* NfcManagerProxy::this function is unavailable for other module currently */
    data.ReadInt32();
    reply.ReadInt32();
    return;
}

sptr<IRemoteObject> NfcManagerProxy::GetRemoteNfcService()
{
    return nullptr;
}

int32_t NfcManagerProxy::BindSeService(MessageParcel& data, MessageParcel& reply)
{
    data.ReadInt32();
    reply.ReadInt32();
    return 0;
}

int32_t NfcManagerProxy::IsSeServiceConnected(MessageParcel& data, MessageParcel& reply)
{
    data.ReadInt32();
    reply.ReadInt32();
    return 0;
}

int32_t NfcManagerProxy::GetSecureElements(MessageParcel& data, MessageParcel& reply)
{
    data.ReadInt32();
    reply.ReadInt32();
    return 0;
}

sptr<IRemoteObject> NfcManagerProxy::GetSecureElement(std::u16string seName)
{
    return nullptr;
}

int32_t NfcManagerProxy::IsSecureElementPresent(MessageParcel& data, MessageParcel& reply)
{
    data.ReadInt32();
    reply.ReadInt32();
    return 0;
}

int32_t NfcManagerProxy::OpenChannel(MessageParcel& data, MessageParcel& reply)
{
    data.ReadInt32();
    reply.ReadInt32();
    return 0;
}

int32_t NfcManagerProxy::TransmitApdu(MessageParcel& data, MessageParcel& reply)
{
    data.ReadInt32();
    reply.ReadInt32();
    return 0;
}

int32_t NfcManagerProxy::CloseChannel(MessageParcel& data, MessageParcel& reply)
{
    data.ReadInt32();
    reply.ReadInt32();
    return 0;
}

int32_t NfcManagerProxy::GetSelectResponse(MessageParcel& data, MessageParcel& reply)
{
    data.ReadInt32();
    reply.ReadInt32();
    return 0;
}

int32_t NfcManagerProxy::CloseSeSessions(MessageParcel& data, MessageParcel& reply)
{
    data.ReadInt32();
    reply.ReadInt32();
    return 0;
}

int32_t NfcManagerProxy::SetListenMode(MessageParcel& data, MessageParcel& reply)
{
    data.ReadInt32();
    reply.ReadInt32();
    return 0;
}

int32_t NfcManagerProxy::IsListenModeEnabled(MessageParcel& data, MessageParcel& reply)
{
    data.ReadInt32();
    reply.ReadInt32();
    return 0;
}

int32_t NfcManagerProxy::GetNfcInfo(MessageParcel& data, MessageParcel& reply)
{
    data.ReadInt32();
    reply.ReadInt32();
    return 0;
}

int32_t NfcManagerProxy::SetRfConfig(MessageParcel& data, MessageParcel& reply)
{
    data.ReadInt32();
    reply.ReadInt32();
    return 0;
}

int32_t NfcManagerProxy::OpenSession(MessageParcel& data, MessageParcel& reply)
{
    data.ReadInt32();
    reply.ReadInt32();
    return 0;
}

int32_t NfcManagerProxy::GetAtr(MessageParcel& data, MessageParcel& reply)
{
    data.ReadInt32();
    reply.ReadInt32();
    return 0;
}

int32_t NfcManagerProxy::CloseSessionChannels(MessageParcel& data, MessageParcel& reply)
{
    data.ReadInt32();
    reply.ReadInt32();
    return 0;
}

int32_t NfcManagerProxy::IsChannelClosed(MessageParcel& data, MessageParcel& reply)
{
    data.ReadInt32();
    reply.ReadInt32();
    return 0;
}

int32_t NfcManagerProxy::GetInfoFromConfig(MessageParcel& data, MessageParcel& reply)
{
    data.ReadInt32();
    reply.ReadInt32();
    return 0;
}

int32_t NfcManagerProxy::SetForegroundPreferred(MessageParcel& data, MessageParcel& reply)
{
    data.ReadInt32();
    reply.ReadInt32();
    return 0;
}

int32_t NfcManagerProxy::UnsetForegroundPreferred(MessageParcel& data, MessageParcel& reply)
{
    data.ReadInt32();
    reply.ReadInt32();
    return 0;
}

int32_t NfcManagerProxy::IsDefaultForAid(MessageParcel& data, MessageParcel& reply)
{
    data.ReadInt32();
    reply.ReadInt32();
    return 0;
}

int32_t NfcManagerProxy::RegisterAids(MessageParcel& data, MessageParcel& reply)
{
    data.ReadInt32();
    reply.ReadInt32();
    return 0;
}

int32_t NfcManagerProxy::RemoveAids(MessageParcel& data, MessageParcel& reply)
{
    data.ReadInt32();
    reply.ReadInt32();
    return 0;
}

int32_t NfcManagerProxy::GetAids(MessageParcel& data, MessageParcel& reply)
{
    data.ReadInt32();
    reply.ReadInt32();
    return 0;
}

int32_t NfcManagerProxy::IsDefaultForType(MessageParcel& data, MessageParcel& reply)
{
    data.ReadInt32();
    reply.ReadInt32();
    return 0;
}

/* NFC Tag operations 2.0 */
int32_t NfcManagerProxy::IsNdefTag(MessageParcel& data, MessageParcel& reply)
{
    data.ReadInt32();
    reply.ReadInt32();
    return 0;
}

int32_t NfcManagerProxy::NdefRead(MessageParcel& data, MessageParcel& reply)
{
    data.ReadInt32();
    reply.ReadInt32();
    return 0;
}

int32_t NfcManagerProxy::NdefWrite(MessageParcel& data, MessageParcel& reply)
{
    data.ReadInt32();
    reply.ReadInt32();
    return 0;
}

int32_t NfcManagerProxy::NdefSetReadOnly(MessageParcel& data, MessageParcel& reply)
{
    data.ReadInt32();
    reply.ReadInt32();
    return 0;
}

int32_t NfcManagerProxy::CanSetReadOnly(MessageParcel& data, MessageParcel& reply)
{
    data.ReadInt32();
    reply.ReadInt32();
    return 0;
}
} // namespace Nfc
} // namespace OHOS