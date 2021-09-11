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

package ohos.nfc.oma;

import ohos.hiviewdfx.HiLog;
import ohos.hiviewdfx.HiLogLabel;
import ohos.nfc.NfcAdapterUtils;
import ohos.nfc.NfcCommProxy;
import ohos.nfc.NfcKitsUtils;
import ohos.rpc.IRemoteObject;
import ohos.rpc.IRemoteObject.DeathRecipient;
import ohos.rpc.RemoteException;
import ohos.sysability.samgr.SysAbilityManager;
import ohos.utils.Parcel;
import ohos.rpc.MessageParcel;
import ohos.rpc.MessageOption;
import ohos.utils.system.safwk.java.SystemAbilityDefinition;

import java.util.Optional;

/**
 * Provides interfaces to access secure element
 *
 * @hide
 */
public final class SecureElementProxy extends NfcCommProxy implements ISecureElement {
    private static final HiLogLabel LABEL = new HiLogLabel(HiLog.LOG_CORE, NfcKitsUtils.NFC_DOMAIN_ID,
        "SecureElementProxy");
    private static final int TRUE = 1;
    private static final int FALSE = 0;
    private static final int TRANSACTION_BIND_SECURE_ELEMENT = 50; // ID mapping to service func.
    private static final String SECURE_ELEMENT_TOKEN = "ohos.nfc.oma.ISecureElement";

    private SEService mSeService = null;

    /**
     * A constructor used to create a {@code SecureElementProxy} instance.
     *
     * @param service Indicates the service bound to this proxy.
     */
    public SecureElementProxy(SEService service) {
        super(SystemAbilityDefinition.NFC_MANAGER_SYS_ABILITY_ID);
        mSeService = service;
    }

    @Override
    public boolean isSeServiceConnected() throws RemoteException {
        MessageParcel data = MessageParcel.obtain();

        request(NfcKitsUtils.IS_SE_SERVICE_CONNECTED, data);
        return true;
    }

    @Override
    public void bindSeService(SecureElementCallbackProxy proxy) throws RemoteException {
        if (proxy == null) {
            HiLog.warn(LABEL, "method bindSeService input param is null");
            return;
        }

        MessageParcel data = MessageParcel.obtain();
        NfcKitsUtils.writeInterfaceToken(
            NfcAdapterUtils.stringReplace(NfcKitsUtils.ADAPTER_DESCRIPTOR), data);
        MessageParcel reply = request(NfcKitsUtils.BIND_SE_SERVICE, data);
        reply.reclaim();
    }

    @Override
    public Reader[] getReaders(SEService seService) throws RemoteException {
        if (seService == null) {
            HiLog.warn(LABEL, "method getReaders input param is null");
            return new Reader[0];
        }

        MessageParcel data = MessageParcel.obtain();
        NfcKitsUtils.writeInterfaceToken(
            NfcAdapterUtils.stringReplace(NfcKitsUtils.SE_SERVICE_DESCRIPTOR), data);
        MessageParcel reply = request(NfcKitsUtils.GET_SECURE_ELEMENTS, data);
        final int seSize = reply.readInt();
        if (seSize <= 0 || seSize > 3) {
            reply.reclaim();
            return new Reader[0];
        }
        Reader[] readers = new Reader[seSize];
        for (int i = 0; i < seSize; i++) {
            String name = reply.readString();
            readers[i] = new Reader(this, name, seService);
        }
        reply.reclaim();
        return readers;
    }

    @Override
    public boolean isSecureElementPresent(String name) throws RemoteException {
        if (name == null || name.length() == 0) {
            HiLog.warn(LABEL, "method isSecureElementPresent input param is null or empty");
            return false;
        }

        MessageParcel data = MessageParcel.obtain();
        data.writeString(SECURE_ELEMENT_TOKEN);
        data.writeString(name);
        MessageParcel reply = request(NfcKitsUtils.IS_SE_PRESENT, data);
        final int present = reply.readInt();
        reply.reclaim();
        HiLog.info(LABEL, "isPresent: present = %{public}d, name = %{public}s", present, name);
        return present == TRUE;
    }

    @Override
    public Optional<Channel> openBasicChannel(Session session, byte[] aid) throws RemoteException {
        if (session == null || aid == null || aid.length == 0) {
            HiLog.warn(LABEL, "method openBasicChannel input param is null or empty");
            return Optional.empty();
        }

        MessageParcel data = MessageParcel.obtain();

        // data for NFC SA
        data.writeString(SECURE_ELEMENT_TOKEN);
        data.writeRemoteObject(session.getSessionObject());
        data.writeInt(Reader.TYPE_BASIC_CHANNEL);

        // data for SE service
        data.writeByteArray(aid);
        data.writeByte((byte) 0x00);
        data.writeRemoteObject(session.getSEService().getCallbackProxy().asObject());

        MessageParcel reply = request(NfcKitsUtils.OPEN_CHANNEL, data);
        IRemoteObject object = reply.readRemoteObject();
        reply.reclaim();
        if (object == null) {
            HiLog.error(LABEL, "openBasicChannel: readRemoteObject is null");
            return Optional.empty();
        }
        return Optional.of(new Channel(session.getSEService(), this, object, Reader.TYPE_BASIC_CHANNEL));
    }

    @Override
    public Optional<Channel> openLogicalChannel(Session session, byte[] aid) throws RemoteException {
        if (session == null || aid == null || aid.length == 0) {
            HiLog.warn(LABEL, "method openLogicalChannel input param is null or empty");
            return Optional.empty();
        }

        MessageParcel data = MessageParcel.obtain();

        // data for NFC SA
        data.writeString(SECURE_ELEMENT_TOKEN);
        data.writeRemoteObject(session.getSessionObject());
        data.writeInt(Reader.TYPE_LOGICAL_CHANNEL);

        data.writeByteArray(aid);
        data.writeByte((byte) 0x7f); // 7f: p2 of logical channel in se service.
        data.writeRemoteObject(session.getSEService().getCallbackProxy().asObject());

        MessageParcel reply = request(NfcKitsUtils.OPEN_CHANNEL, data);
        IRemoteObject object = reply.readRemoteObject();
        reply.reclaim();
        if (object == null) {
            HiLog.error(LABEL, "openLogicalChannel: readRemoteObject is null");
            return Optional.empty();
        }
        return Optional.of(new Channel(session.getSEService(), this, object, Reader.TYPE_LOGICAL_CHANNEL));
    }

    @Override
    public byte[] transmit(IRemoteObject remoteObject, byte[] command) throws RemoteException {
        if (remoteObject == null || command == null || command.length == 0) {
            HiLog.warn(LABEL, "method transmit input param is null or empty");
            return new byte[0];
        }

        MessageParcel data = MessageParcel.obtain();

        // data for NFC SA
        data.writeString(SECURE_ELEMENT_TOKEN);
        data.writeRemoteObject(remoteObject);
        data.writeByteArray(command);

        MessageParcel reply = request(NfcKitsUtils.TRANSMIT_APDU, data);
        byte[] result = reply.readByteArray();
        reply.reclaim();
        return result;
    }

    @Override
    public byte[] getSelectResponse(IRemoteObject remoteObject) throws RemoteException {
        if (remoteObject == null) {
            HiLog.warn(LABEL, "method getSelectResponse input param is null");
            return new byte[0];
        }

        MessageParcel data = MessageParcel.obtain();
        data.writeString(SECURE_ELEMENT_TOKEN);
        data.writeRemoteObject(remoteObject);
        MessageParcel reply = request(NfcKitsUtils.GET_SELECT_RESPONSE, data);
        byte[] result = reply.readByteArray();
        reply.reclaim();
        return result;
    }

    @Override
    public Optional<Session> openSession(Reader se) throws RemoteException {
        if (se == null) {
            HiLog.warn(LABEL, "method openSession input param is null");
            return Optional.empty();
        }

        MessageParcel data = MessageParcel.obtain();

        data.writeString(SECURE_ELEMENT_TOKEN);
        data.writeString(se.getName());
        MessageParcel reply = request(NfcKitsUtils.OPEN_SESSION, data);
        IRemoteObject object = reply.readRemoteObject();
        reply.reclaim();
        if (object == null) {
            HiLog.error(LABEL, "openSession: readRemoteObject is null");
            return Optional.empty();
        }
        return Optional.of(new Session(mSeService, this, object));
    }

    @Override
    public byte[] getATR(IRemoteObject remoteObject) throws RemoteException {
        if (remoteObject == null) {
            HiLog.warn(LABEL, "method getATR input param is null");
            return new byte[0];
        }

        MessageParcel data = MessageParcel.obtain();
        data.writeString(SECURE_ELEMENT_TOKEN);
        data.writeRemoteObject(remoteObject);
        MessageParcel reply = request(NfcKitsUtils.GET_ATR, data);
        byte[] result = reply.readByteArray();
        reply.reclaim();
        return result;
    }

    @Override
    public void closeSeSessions(Reader se) throws RemoteException {
        if (se == null) {
            HiLog.warn(LABEL, "method closeSeSessions input param is null");
            return;
        }

        MessageParcel data = MessageParcel.obtain();

        data.writeString(SECURE_ELEMENT_TOKEN);
        data.writeString(se.getName());
        MessageParcel reply = request(NfcKitsUtils.CLOSE_SE_SESSIONS, data);
        reply.reclaim();
    }

    @Override
    public void close(IRemoteObject remoteObject) throws RemoteException {
        if (remoteObject == null) {
            HiLog.warn(LABEL, "method close input param is null");
            return;
        }

        MessageParcel data = MessageParcel.obtain();
        data.writeString(SECURE_ELEMENT_TOKEN);
        data.writeRemoteObject(remoteObject);
        MessageParcel reply = request(NfcKitsUtils.CLOSE_SESSION_CHANNELS, data);
        reply.reclaim();
    }

    @Override
    public void closeChannel(IRemoteObject remoteObject) throws RemoteException {
        if (remoteObject == null) {
            HiLog.warn(LABEL, "method closeChannel input param is null");
            return;
        }

        MessageParcel data = MessageParcel.obtain();

        // data for NFC SA.
        data.writeString(SECURE_ELEMENT_TOKEN);
        data.writeRemoteObject(remoteObject);
        MessageParcel reply = request(NfcKitsUtils.CLOSE_CHANNEL, data);
        reply.reclaim();
    }

    @Override
    public boolean isChannelClosed(IRemoteObject remoteObject) throws RemoteException {
        if (remoteObject == null) {
            HiLog.warn(LABEL, "method isChannelClosed input param is null");
            return false;
        }

        MessageParcel data = MessageParcel.obtain();

        // data for NFC SA.
        data.writeString(SECURE_ELEMENT_TOKEN);
        data.writeRemoteObject(remoteObject);
        MessageParcel reply = request(NfcKitsUtils.IS_CHANNEL_CLOSED, data);
        boolean result = reply.readBoolean();
        reply.reclaim();
        return result;
    }
}
