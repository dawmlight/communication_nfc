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

package ohos.nfc.tag;

import ohos.hiviewdfx.HiLog;
import ohos.hiviewdfx.HiLogLabel;
import ohos.nfc.NfcAdapterUtils;
import ohos.nfc.NfcCommProxy;
import ohos.nfc.NfcKitsUtils;
import ohos.rpc.IRemoteObject;
import ohos.rpc.IRemoteObject.DeathRecipient;
import ohos.rpc.MessageParcel;
import ohos.rpc.MessageOption;
import ohos.rpc.RemoteException;
import ohos.sysability.samgr.SysAbilityManager;
import ohos.utils.Parcel;
import ohos.utils.Sequenceable;
import ohos.utils.system.safwk.java.SystemAbilityDefinition;

/**
 * Provides interfaces to access tag.
 *
 * @since 1
 * @hide
 */
class NfcTagProxy extends NfcCommProxy implements ITagInfo {
    private static final String TAG_TOKEN = "ohos.nfc.ITagInfo";
    private static final HiLogLabel LABEL = new HiLogLabel(HiLog.LOG_CORE, NfcKitsUtils.NFC_DOMAIN_ID, "NfcTagProxy");

    private static NfcTagProxy sNfcTagProxy;

    private NfcTagProxy() {
        super(SystemAbilityDefinition.NFC_MANAGER_SYS_ABILITY_ID);
    }

    /**
     * A constructor used to create a {@code NfcTagProxy} instance.
     *
     * @return Returns the instance of NfcTagProxy.
     */
    public static synchronized NfcTagProxy getInstance() {
        if (sNfcTagProxy == null) {
            sNfcTagProxy = new NfcTagProxy();
        }
        return sNfcTagProxy;
    }

    @Override
    public boolean connectTag(int tagHandle, int tagProfile) throws RemoteException {
        MessageParcel data = MessageParcel.obtain();

        data.writeString(TAG_TOKEN);
        data.writeInt(tagHandle);
        data.writeInt(tagProfile);
        MessageParcel reply = requestWithoutCheck(NfcKitsUtils.MSG_CONNECT_TAG, data);
        boolean result = reply.readBoolean();
        reply.reclaim();
        return result;
    }

    @Override
    public boolean reconnectTag(int tagHandle) throws RemoteException {
        MessageParcel data = MessageParcel.obtain();

        data.writeString(TAG_TOKEN);
        data.writeInt(tagHandle);
        MessageParcel reply = requestWithoutCheck(NfcKitsUtils.MSG_RECONNECT_TAG, data);
        boolean result = reply.readBoolean();
        reply.reclaim();
        return result;
    }

    @Override
    public boolean isTagConnected(int tagHandle) throws RemoteException {
        MessageParcel data = MessageParcel.obtain();

        data.writeString(TAG_TOKEN);
        data.writeInt(tagHandle);
        MessageParcel reply = requestWithoutCheck(NfcKitsUtils.MSG_IS_TAG_CONNECT, data);
        boolean result = reply.readBoolean();
        reply.reclaim();
        return result;
    }

    @Override
    public boolean setSendDataTimeout(int tagProfile, int timeout) throws RemoteException {
        MessageParcel data = MessageParcel.obtain();

        data.writeString(TAG_TOKEN);
        data.writeInt(tagProfile);
        data.writeInt(timeout);
        MessageParcel reply = requestWithoutCheck(NfcKitsUtils.MSG_SET_SEND_DATA_TIMEOUT, data);
        boolean isSuccess = reply.readBoolean();
        reply.reclaim();
        return isSuccess;
    }

    @Override
    public int getSendDataTimeout(int tagProfile) throws RemoteException {
        MessageParcel data = MessageParcel.obtain();

        data.writeString(TAG_TOKEN);
        data.writeInt(tagProfile);
        MessageParcel reply = requestWithoutCheck(NfcKitsUtils.MSG_GET_SEND_DATA_TIMEOUT, data);
        int result = reply.readInt();
        reply.reclaim();
        return result;
    }

    @Override
    public byte[] sendData(int tagHandle, byte[] cmdData) throws RemoteException {
        if (cmdData == null || cmdData.length == 0) {
            HiLog.warn(LABEL, "method sendData input param is null or empty");
            return new byte[0];
        }

        MessageParcel data = MessageParcel.obtain();

        NfcKitsUtils.writeInterfaceToken(NfcAdapterUtils.stringReplace(NfcKitsUtils.TAG_DESCRIPTOR), data);
        data.writeInt(tagHandle);
        data.writeByteArray(cmdData);
        MessageParcel reply = request(NfcKitsUtils.MSG_SEND_RAW_APDU_DATA, data);
        ResponseInfo result = new ResponseInfo();
        if (result instanceof Sequenceable) {
            reply.readSequenceable((Sequenceable) result);
        }
        reply.reclaim();
        return result.getResponse();
    }

    @Override
    public int getMaxSendLength(int tagProfile) throws RemoteException {
        MessageParcel data = MessageParcel.obtain();

        data.writeString(TAG_TOKEN);
        data.writeInt(tagProfile);
        MessageParcel reply = requestWithoutCheck(NfcKitsUtils.MSG_GET_MAX_SEND_LENGTH, data);
        int result = reply.readInt();
        reply.reclaim();
        return result;
    }

    @Override
    public void resetSendDataTimeout() throws RemoteException {
        MessageParcel data = MessageParcel.obtain();

        data.writeString(TAG_TOKEN);
        MessageParcel reply = request(NfcKitsUtils.MSG_RESET_DATA_TIMEOUT, data);
        reply.reclaim();
    }

    @Override
    public boolean isNdefTag(int tagHandle) throws RemoteException {
        MessageParcel data = MessageParcel.obtain();

        NfcKitsUtils.writeInterfaceToken(NfcAdapterUtils.stringReplace(NfcKitsUtils.TAG_DESCRIPTOR), data);
        data.writeInt(tagHandle);
        MessageParcel reply = requestWithoutCheck(NfcKitsUtils.IS_NDEF, data);
        reply.reclaim();
        return reply.readBoolean();
    }

    @Override
    public NdefMessage ndefRead(int tagHandle) throws RemoteException {
        MessageParcel data = MessageParcel.obtain();

        NfcKitsUtils.writeInterfaceToken(NfcAdapterUtils.stringReplace(NfcKitsUtils.TAG_DESCRIPTOR), data);
        data.writeInt(tagHandle);
        MessageParcel reply = requestWithoutCheck(NfcKitsUtils.NDEF_READ, data);
        NdefMessage result = new NdefMessage(new MessageRecord[0]);
        if (result instanceof Sequenceable) {
            reply.readSequenceable((Sequenceable) result);
        }
        reply.reclaim();
        return result;
    }

    @Override
    public int ndefWrite(int tagHandle, NdefMessage ndefMsg) throws RemoteException {
        MessageParcel data = MessageParcel.obtain();

        NfcKitsUtils.writeInterfaceToken(NfcAdapterUtils.stringReplace(NfcKitsUtils.TAG_DESCRIPTOR), data);
        data.writeInt(tagHandle);
        if (ndefMsg instanceof Sequenceable) {
            data.writeSequenceable((Sequenceable) ndefMsg);
        }
        MessageParcel reply = requestWithoutCheck(NfcKitsUtils.NDEF_WRITE, data);
        int result = reply.readInt();
        reply.reclaim();
        return result;
    }

    @Override
    public boolean canSetReadOnly(int ndefType) throws RemoteException {
        MessageParcel data = MessageParcel.obtain();

        NfcKitsUtils.writeInterfaceToken(NfcAdapterUtils.stringReplace(NfcKitsUtils.TAG_DESCRIPTOR), data);
        data.writeInt(ndefType);
        MessageParcel reply = requestWithoutCheck(NfcKitsUtils.CAN_SET_READONLY, data);
        boolean result = reply.readBoolean();
        reply.reclaim();
        return result;
    }

    @Override
    public int ndefSetReadOnly(int tagHandle) throws RemoteException {
        MessageParcel data = MessageParcel.obtain();

        NfcKitsUtils.writeInterfaceToken(NfcAdapterUtils.stringReplace(NfcKitsUtils.TAG_DESCRIPTOR), data);
        data.writeInt(tagHandle);
        MessageParcel reply = requestWithoutCheck(NfcKitsUtils.NDEF_SET_READONLY, data);
        int result = reply.readInt();
        reply.reclaim();
        return result;
    }
}
