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

package ohos.nfc;

import ohos.aafwk.ability.Ability;
import ohos.event.intentagent.IntentAgent;
import ohos.hiviewdfx.HiLog;
import ohos.hiviewdfx.HiLogLabel;
import ohos.interwork.utils.PacMapEx;
import ohos.nfc.NfcController.ReaderModeCallback;
import ohos.nfc.NfcController.ReaderModeCallbackStub;
import ohos.rpc.IRemoteObject;
import ohos.rpc.IRemoteObject.DeathRecipient;
import ohos.rpc.MessageParcel;
import ohos.rpc.MessageOption;
import ohos.rpc.RemoteException;
import ohos.rpc.RemoteObject;
import ohos.sysability.samgr.SysAbilityManager;
import ohos.utils.Parcel;
import ohos.utils.Sequenceable;
import ohos.utils.system.safwk.java.SystemAbilityDefinition;

import java.util.List;

/**
 * Implements inter-process communicatioin (IPC) for the NFC service.
 *
 * @hide
 */
class NfcControllerProxy implements INfcController {
    private static final HiLogLabel LABEL = new HiLogLabel(HiLog.LOG_CORE, NfcKitsUtils.NFC_DOMAIN_ID,
        "NfcControllerProxy");
    private static final int TRUE = 1;
    private static final int FALSE = 0;
    private static final String NFC_CONTROLLER = "ohos.nfc.INfcController";
    private static final int TRANSCACTION_SET_READER_MODE = 41;

    private static NfcControllerProxy sNfcControllerProxy;
    private final Object mRemoteLock = new Object();
    private IRemoteObject mRemoteObject;

    private NfcControllerProxy() {
    }

    /**
     * Gets a {@code NfcControllerProxy} instance.
     *
     * @return Returns the instance of NfcControllerProxy.
     */
    public static synchronized NfcControllerProxy getInstance() {
        if (sNfcControllerProxy == null) {
            sNfcControllerProxy = new NfcControllerProxy();
        }
        return sNfcControllerProxy;
    }

    @Override
    public IRemoteObject asObject() {
        if (mRemoteObject != null) {
            return mRemoteObject;
        }
        synchronized (mRemoteLock) {
            mRemoteObject = SysAbilityManager.getSysAbility(SystemAbilityDefinition.NFC_MANAGER_SYS_ABILITY_ID);
            if (mRemoteObject != null) {
                mRemoteObject.addDeathRecipient(new NfcControllerDeathRecipient(), 0);
                HiLog.info(LABEL, "Get NfcManagerService completed.");
            } else {
                HiLog.error(LABEL, "getSysAbility(NfcManagerService) failed.");
            }
            return mRemoteObject;
        }
    }

    @Override
    public int setNfcEnabled(boolean isEnabled) throws RemoteException {
        IRemoteObject service = asObject();
        if (service == null) {
            throw new RemoteException("service is null");
        }
        MessageParcel data = MessageParcel.obtain();
        MessageParcel reply = MessageParcel.obtain();
        MessageOption option = new MessageOption();
        try {
            data.writeString(NFC_CONTROLLER);
            data.writeInt(isEnabled ? TRUE : FALSE);
            service.sendRequest(NfcKitsUtils.MSG_SET_NFC_ENABLED, data, reply, option);
            return reply.readInt();
        } catch (RemoteException remoteException) {
            HiLog.error(LABEL, "setNfcEnabled call failed.");
            throw remoteException;
        } finally {
            reply.reclaim();
            data.reclaim();
        }
    }

    @Override
    public int getNfcState() throws RemoteException {
        IRemoteObject service = asObject();
        if (service == null) {
            throw new RemoteException();
        }
        MessageParcel data = MessageParcel.obtain();
        MessageParcel reply = MessageParcel.obtain();
        MessageOption option = new MessageOption();
        try {
            data.writeString(NFC_CONTROLLER);
            service.sendRequest(NfcKitsUtils.MSG_GET_NFC_STATE, data, reply, option);
            return reply.readInt();
        } catch (RemoteException remoteException) {
            HiLog.error(LABEL, "getNfcState call failed.");
            throw remoteException;
        } finally {
            reply.reclaim();
            data.reclaim();
        }
    }

    @Override
    public boolean isNfcAvailable() throws RemoteException {
        IRemoteObject service = asObject();
        if (service == null) {
            throw new RemoteException();
        }
        MessageParcel data = MessageParcel.obtain();
        MessageParcel reply = MessageParcel.obtain();
        MessageOption option = new MessageOption();
        try {
            boolean writeResult = data.writeString(NFC_CONTROLLER);
            if (!writeResult) {
                HiLog.error(LABEL, "Error when try to write.");
                return false;
            }
            service.sendRequest(NfcKitsUtils.MSG_IS_NFC_AVAILABLE, data, reply, option);
            int result = reply.readInt();
            return result != FALSE;
        } catch (RemoteException remoteException) {
            HiLog.error(LABEL, "isNfcAvailable call failed.");
            throw remoteException;
        } finally {
            reply.reclaim();
            data.reclaim();
        }
    }

    @Override
    public void registerForegroundDispatch(IntentAgent intentAgent, List<String> intentFilterStrs,
        ProfileParcel profiles) throws RemoteException {
        IRemoteObject service = asObject();
        if (service == null) {
            throw new RemoteException();
        }
        MessageParcel data = MessageParcel.obtain();
        MessageParcel reply = MessageParcel.obtain();
        try {
            reply.writeString(NFC_CONTROLLER);
            if (intentFilterStrs != null) {
                int length = intentFilterStrs.size() / 2;
                reply.writeInt(length);
                for (String intentFilterStr : intentFilterStrs) {
                    reply.writeString(intentFilterStr);
                }
            }
            if (intentAgent instanceof Sequenceable) {
                data.writeSequenceable((Sequenceable) intentAgent);
            }
            if (profiles instanceof Sequenceable) {
                reply.writeSequenceable((Sequenceable) profiles);
            }
            MessageOption option = new MessageOption();
            service.sendRequest(NfcKitsUtils.REGISTER_FOREGROUND_DISPATCH, data, reply, option);
        } catch (RemoteException remoteException) {
            HiLog.error(LABEL, "registerForegroundDispatch call failed.");
            throw remoteException;
        } finally {
            reply.reclaim();
            data.reclaim();
        }
    }

    @Override
    public void setReaderMode(IRemoteObject token, ReaderModeCallback callback,
        int flags, PacMapEx pacMapEx, NfcController controller) throws RemoteException {
        ReaderModeCallbackStub proxy = null;
        if (controller != null) {
            proxy = controller.new ReaderModeCallbackStub(callback);
        }
        IRemoteObject service = asObject();
        if (service == null) {
            throw new RemoteException("service is null");
        }
        MessageParcel data = MessageParcel.obtain();
        MessageParcel reply = MessageParcel.obtain();
        try {
            IRemoteObject nfcService = getNfcService();
            if (nfcService == null) {
                HiLog.info(LABEL, "setReaderMode: nfcService = null.");
                return;
            }

            NfcKitsUtils.writeInterfaceToken(
                NfcAdapterUtils.stringReplace(NfcKitsUtils.ADAPTER_DESCRIPTOR), data);
            data.writeRemoteObject(token);
            if (proxy != null) {
                data.writeRemoteObject(proxy.asObject());
            }
            data.writeInt(flags);
            if (pacMapEx instanceof Sequenceable) {
                data.writeSequenceable((Sequenceable) pacMapEx);
            }
            MessageOption option = new MessageOption();
            boolean isSucc = nfcService.sendRequest(TRANSCACTION_SET_READER_MODE, data, reply, option);
            if (!isSucc) {
                HiLog.error(LABEL, "setReaderMode :sendRequest fail");
                return;
            }
            int errCode = reply.readInt();
            HiLog.info(LABEL, "setReaderMode: errCode = %{public}d", errCode);
        } catch (RemoteException remoteException) {
            HiLog.error(LABEL, "setReaderMode failed.");
            throw remoteException;
        } finally {
            reply.reclaim();
            data.reclaim();
        }
    }

    private void setRemoteObject(IRemoteObject remoteObject) {
        synchronized (mRemoteLock) {
            mRemoteObject = remoteObject;
        }
    }

    // Implementation of IPC Death Recipient.
    private class NfcControllerDeathRecipient implements DeathRecipient {
        @Override
        public void onRemoteDied() {
            HiLog.warn(LABEL, "NfcControllerDeathRecipient::onRemoteDied.");
            setRemoteObject(null);
        }
    }

    private IRemoteObject getNfcService() throws RemoteException {
        IRemoteObject service = asObject();
        if (service == null) {
            HiLog.error(LABEL, "getNfcService service is null");
            throw new RemoteException();
        }
        MessageParcel data = MessageParcel.obtain();
        MessageParcel reply = MessageParcel.obtain();
        MessageOption option = new MessageOption();
        NfcKitsUtils.writeInterfaceToken(NfcKitsUtils.ADAPTER_DESCRIPTOR, data);
        try {
            boolean isSucc = service.sendRequest(NfcKitsUtils.GET_NFC_SERVICE, data, reply, option);
            if (!isSucc) {
                HiLog.error(LABEL, "getNfcService: IPC error: %{public}d", reply.readInt());
                throw new RemoteException();
            }
            return reply.readRemoteObject();
        } catch (RemoteException remoteException) {
            HiLog.error(LABEL, "getNfcService : call fail");
            throw remoteException;
        } finally {
            reply.reclaim();
            data.reclaim();
        }
    }
}
