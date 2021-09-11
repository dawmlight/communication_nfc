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

import ohos.hiviewdfx.HiLog;
import ohos.hiviewdfx.HiLogLabel;
import ohos.rpc.IRemoteBroker;
import ohos.rpc.IRemoteObject;
import ohos.rpc.MessageOption;
import ohos.rpc.MessageParcel;
import ohos.rpc.RemoteException;
import ohos.sysability.samgr.SysAbilityManager;

/**
 * Implementation of NFC enhancer-related APIs that can be called by the NFC enhancer service.
 *
 * @author liujiawei
 * @since 3
 * @hide
 */
public class NfcCommProxy implements IRemoteBroker {
    private static final HiLogLabel LABEL = new HiLogLabel(HiLog.LOG_CORE, NfcKitsUtils.NFC_DOMAIN_ID,
        "NfcCommProxy");
    private static final int ERR_OK = 0;
    private final Object mLock = new Object();
    private IRemoteObject mRemoteObject;
    private int mAbilityId;

    protected NfcCommProxy(int abilityId) {
        mAbilityId = abilityId;
    }

    /* Register native functions. */
    static {
        /* Native functions are defined in ipc_core.z */
        System.loadLibrary("ipc_core.z");
    }

    /**
     * Get NFC device SA by Nfc_Manager_SYS_ABILITY_ID.
     *
     * @hide
     */
    @Override
    public IRemoteObject asObject() {
        synchronized (mLock) {
            if (mRemoteObject != null) {
                return mRemoteObject;
            }

            /* get NFC device service via getSysAbility */
            mRemoteObject = SysAbilityManager.getSysAbility(mAbilityId);
            if (mRemoteObject != null) {
                mRemoteObject.addDeathRecipient(new NfcDeathRecipient(), 0);
                HiLog.info(LABEL, "getSysAbility %{public}d completed.", mAbilityId);
            } else {
                HiLog.error(LABEL, "getSysAbility %{public}d failed.", mAbilityId);
            }

            return mRemoteObject;
        }
    }

    /**
     * Implementation of IPC Death Recipient.
     *
     * @hide
     */
    private class NfcDeathRecipient implements IRemoteObject.DeathRecipient {
        @Override
        public void onRemoteDied() {
            HiLog.warn(LABEL, "NfcDeathRecipient::onRemoteDied.");
            synchronized (mLock) {
                mRemoteObject = null;
            }
        }
    }

    private IRemoteObject getRemote() throws RemoteException {
        IRemoteObject remote = asObject();
        if (remote == null) {
            HiLog.error(LABEL, "Failed to get remote object of NFC SA");
            throw new RemoteException("get remote failed for NFC SA");
        }
        return remote;
    }

    /**
     * Send request to remote ability.
     *
     * @param cmd Command sent to remote ability.
     * @param data Parameters sent to remote ability.
     * @return Returned reply MessageParcel which could be read result.
     * @throws RemoteException if the caller is missing required permissions.
     * @hide
     */
    protected MessageParcel request(int cmd, MessageParcel data) throws RemoteException {
        MessageParcel replyFromNfcSa = MessageParcel.obtain();
        MessageOption option = new MessageOption(MessageOption.TF_SYNC);

        try {
            getRemote().sendRequest(cmd, data, replyFromNfcSa, option);
            int result = replyFromNfcSa.readInt();
            if (result != ERR_OK) {
                throw new RemoteException("result != ERR_OK");
            }
            return replyFromNfcSa;
        } catch (RemoteException e) {
            replyFromNfcSa.reclaim();
            throw new RemoteException("connect to NFC SA failed via request");
        } finally {
            if (data != null) {
                data.reclaim();
            }
        }
    }

    /**
     * Send request to remote ability without checking the result error code.
     *
     * @param cmd Command sent to remote ability.
     * @param data Parameters sent to remote ability.
     * @return MessageParcel which could be read result.
     * @throws RemoteException if the caller does not have required permissions.
     * @hide
     */
    protected MessageParcel requestWithoutCheck(int cmd, MessageParcel data) throws RemoteException {
        MessageParcel replyFromNfcSa = MessageParcel.obtain();
        MessageOption option = new MessageOption(MessageOption.TF_SYNC);

        try {
            getRemote().sendRequest(cmd, data, replyFromNfcSa, option);
            return replyFromNfcSa;
        } catch (RemoteException e) {
            replyFromNfcSa.reclaim();
            throw new RemoteException("connect to NFC SA failed via requestWithoutCheck");
        } finally {
            if (data != null) {
                data.reclaim();
            }
        }
    }
}