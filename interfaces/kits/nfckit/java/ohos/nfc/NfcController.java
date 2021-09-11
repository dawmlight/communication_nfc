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
import ohos.aafwk.ability.Lifecycle;
import ohos.app.Context;
import ohos.event.intentagent.IntentAgent;
import ohos.utils.Parcel;
import ohos.utils.Sequenceable;
import ohos.utils.system.SystemCap;
import ohos.hiviewdfx.HiLog;
import ohos.hiviewdfx.HiLogLabel;
import ohos.rpc.MessageParcel;
import ohos.rpc.MessageOption;
import ohos.rpc.RemoteException;
import ohos.rpc.RemoteObject;
import ohos.rpc.IRemoteBroker;
import ohos.rpc.IRemoteObject;
import ohos.interwork.utils.PacMapEx;
import ohos.nfc.tag.TagInfo;

import java.util.List;

/**
 * Provides interfaces for NFC management.
 *
 * <p>Using interfaces provided by this class, you can enable and disable NFC, and query the NFC
 * capabilities and status.
 *
 * @Syscap {@link SystemCapability.Communication#NFC}
 * @since 1
 */
@SystemCap("SystemCapability.Communication.NFC")
public final class NfcController {
    /**
     * Indicates that NFC is disabled.
     *
     * @since 1
     */
    public static final int STATE_OFF = 1;

    /**
     * Indicates that NFC is being enabled.
     *
     * @since 1
     */
    public static final int STATE_TURNING_ON = 2;

    /**
     * Indicates that NFC is enabled.
     *
     * @since 1
     */
    public static final int STATE_ON = 3;

    /**
     * Indicates that NFC is being disabled.
     *
     * @since 1
     */
    public static final int STATE_TURNING_OFF = 4;

    /**
     * Indicates an NFC status change.
     *
     * <p>This is a string constant used to send broadcast messages of NFC status changes.
     *
     * @since 1
     */
    public static final String STATE_CHANGED = "usual.event.nfc.action.ADAPTER_STATE_CHANGED";

    /**
     * Indicates a field-on event detected by NFC.
     *
     * <p>This is a string constant used to send broadcast messages of field-on events.
     *
     * @since 1
     */
    public static final String FIELD_ON_DETECTED = "usual.event.nfc.action.RF_FIELD_ON_DETECTED";

    /**
     * Indicates a field-off event detected by NFC.
     *
     * <p>This is a string constant used to send broadcast messages of field-off events.
     *
     * @since 1
     */
    public static final String FIELD_OFF_DETECTED = "usual.event.nfc.action.RF_FIELD_OFF_DETECTED";

    /**
     * Indicates the NFC status.
     *
     * <p>This is a string constant used to read the NFC status from a broadcast message.
     *
     * @since 1
     */
    public static final String EXTRA_NFC_STATE = "extra_nfc_state";

    /**
     * Indicates the NFC transaction information.
     *
     * <p>This is a constant string used to obtain the NFC transaction information from a
     * field-on broadcast message.
     *
     * @since 1
     */
    public static final String EXTRA_NFC_TRANSACTION = "extra_nfc_transaction";

    /**
     * Indicates the key (in string format) used to obtain the tag information.
     *
     * @since 1
     */
    public static final String EXTRA_TAG_INFO = "extra_nfc_TAG_INFO";

    /** NFC no permission */
    private static final int NO_PERMISSION = -6;

    /** NFC enable and disable result from SA */
    private static final int NO_ERROR = 1;

    private static final HiLogLabel LABEL = new HiLogLabel(HiLog.LOG_CORE, NfcKitsUtils.NFC_DOMAIN_ID, "NfcController");
    private static NfcController sNfcController;
    private NfcControllerProxy mNfcControllerProxy;
    private Context mContext;
    private NfcAbilityManager mNfcAbilityManager;

    /**
     * A callback to be invoked when the system finds a tag while the foreground page is operating in reader mode.
     *
     * @see NfcController#setReaderMode
     */
    public interface ReaderModeCallback extends IRemoteBroker {
        /**
         * Indicates the transaction code of the callback specific to inter-process communication (IPC).
         */
        int TRANSACTION_ON_CALLBACK = 1;

        /**
         * Called when a tag is discovered.
         *
         * @param tagInfo Indicates information of the discovered tag.
         * @since 4
         */
        void onTagDiscovered(TagInfo tagInfo);
    }

    /**
     * Provides ReaderModeCallback for communicate with remote.
     *
     * @since 4
     */
    public class ReaderModeCallbackStub extends RemoteObject implements ReaderModeCallback {
        private ReaderModeCallback callback;

        public ReaderModeCallbackStub(ReaderModeCallback callback) {
            super(NfcKitsUtils.READER_MODE_CALLBACK_DESCRIPTOR);
            this.callback = callback;
        }

        private void readInterfaceToken(Parcel data) {
            data.readInt();
            data.readInt();
            data.readString();
        }

        @Override
        public boolean onRemoteRequest(
            int code, MessageParcel data, MessageParcel reply, MessageOption option) throws RemoteException {
            HiLog.debug(LABEL, "call ReaderModeCallbackStub's onTransact");

            switch (code) {
                case TRANSACTION_ON_CALLBACK: {
                    readInterfaceToken(data);
                    HiLog.debug(LABEL, "ReaderModeCallbackStub onCallback was invoked.");
                    TagInfo tagInfo = new TagInfo(null, null, null, 0);
                    if (tagInfo instanceof Sequenceable) {
                        data.readSequenceable((Sequenceable) tagInfo);
                    }
                    onTagDiscovered(tagInfo);

                    // writeNoException
                    reply.writeInt(0);
                    break;
                }
                default: {
                    HiLog.error(LABEL, "ReaderModeCallbackStub unsupported transaction code.");
                    return super.onRemoteRequest(code, data, reply, option);
                }
            }
            return true;
        }

        @Override
        public IRemoteObject asObject() {
            return this;
        }

        @Override
        public void onTagDiscovered(TagInfo tagInfo) {
            if (callback != null) {
                callback.onTagDiscovered(tagInfo);
            }
        }
    }

    private NfcController(Context context) {
        mNfcControllerProxy = NfcControllerProxy.getInstance();
        mContext = context;
        mNfcAbilityManager = new NfcAbilityManager(this);
    }

    /**
     * Obtains a singleton instance of the {@code NfcController} class.
     *
     * <p>If no singleton instance is available, a singleton instance will be created.
     *
     * @param context Indicates the application context.
     * @return Returns a singleton instance.
     * @throws IllegalArgumentException if the caller is missing required context.
     * @since 1
     */
    public static synchronized NfcController getInstance(Context context) throws IllegalArgumentException {
        if (sNfcController == null) {
            if (context == null) {
                throw new IllegalArgumentException("context is null");
            }
            sNfcController = new NfcController(context.getApplicationContext());
        }
        return sNfcController;
    }

    /**
     * get the context of application
     *
     * @return the context of application
     * @hide
     */
    public Context getContext() {
        return mContext;
    }

    /**
     * Enables NFC.
     *
     * @return Returns {@code true} if NFC is enabled or has been enabled; returns {@code false} otherwise.
     * @throws NfcPermissionException Throws this exception if you have no permission to invoke this method.
     * @since 1
     */
    public boolean openNfc() throws NfcPermissionException {
        try {
            int result = mNfcControllerProxy.setNfcEnabled(true);
            if (result == NO_PERMISSION) {
                throw new NfcPermissionException("No NFC Permission");
            }
            return result == NO_ERROR;
        } catch (RemoteException remoteException) {
            HiLog.error(LABEL, "open RemoteException");
        }
        return false;
    }

    /**
     * Disables NFC.
     *
     * @return Returns {@code true} if NFC is disabled or has been disabled; returns {@code false} otherwise.
     * @throws NfcPermissionException Throws this exception if you have no permission to invoke this method.
     * @since 1
     */
    public boolean closeNfc() throws NfcPermissionException {
        try {
            int result = mNfcControllerProxy.setNfcEnabled(false);
            if (result == NO_PERMISSION) {
                throw new NfcPermissionException("No NFC Permission");
            }
            return result == NO_ERROR;
        } catch (RemoteException remoteException) {
            HiLog.error(LABEL, "close RemoteException");
        }
        return false;
    }

    /**
     * Checks whether NFC is enabled.
     *
     * @return Returns {@code true} if NFC is enabled; returns {@code false} otherwise.
     * @since 1
     */
    public boolean isNfcOpen() {
        return getNfcState() == STATE_ON;
    }

    /**
     * Obtains the NFC status.
     *
     * <p>The NFC status can be any of the following: <ul><li>{@link #STATE_OFF}: Indicates that NFC
     * is disabled. <li>{@link #STATE_TURNING_ON}: Indicates that NFC is being enabled.
     * <li>{@link #STATE_ON}: Indicates that NFC is enabled. <li>{@link #STATE_TURNING_OFF}: Indicates
     * that NFC is being disabled.</ul>
     *
     * @return Returns the NFC status.
     * @since 1
     */
    public int getNfcState() {
        try {
            return mNfcControllerProxy.getNfcState();
        } catch (RemoteException remoteException) {
            HiLog.error(LABEL, "getNfcState RemoteException");
        }
        return STATE_OFF;
    }

    /**
     * Checks whether a device supports NFC.
     *
     * @return Returns {@code true} if the device supports NFC; returns {@code false} otherwise.
     * @since 1
     */
    public boolean isNfcAvailable() {
        try {
            return mNfcControllerProxy.isNfcAvailable();
        } catch (RemoteException remoteException) {
            HiLog.error(LABEL, "isNfcAvailable RemoteException");
        }
        return false;
    }

    /**
     * Sets the NFC OHOSApplication ability to reader mode only when this ability is in the foreground.
     *
     * @param ability Indicates the NFC OHOSApplication ability to set.
     * @param callback Indicates the callback to be called when a tag is discovered.
     * @param flags Indicates poll profiles and other optional parameters.
     * @param pacMapEx Indicates additional extras for configuring reader mode.
     * @since 4
     */
    public void setReaderMode(Ability ability, ReaderModeCallback callback, int flags, PacMapEx pacMapEx) {
        synchronized (NfcController.class) {
            if (!isNfcAvailable()) {
                throw new UnsupportedOperationException();
            }
        }
        mNfcAbilityManager.setReaderMode(ability, callback, flags, pacMapEx, this);
    }

    /**
     * Unsets the NFC OHOSApplication ability from reader mode only when this ability is in the foreground.
     *
     * @param ability Indicates the NFC OHOSApplication ability to unset.
     * @since 4
     */
    public void unsetReaderMode(Ability ability) {
        synchronized (NfcController.class) {
            if (!isNfcAvailable()) {
                throw new UnsupportedOperationException();
            }
        }
        mNfcAbilityManager.unsetReaderMode(ability);
    }

    /**
     * Register foreground dispatch to the given NFC OHOSApplication ability.
     *
     * @param ability Indicates the NFC OHOSApplication ability.
     * @param intent Indicates the IntentAgent to start for the dispatch.
     * @param filters Indicates the IntentFilters to override dispatching for, or null to always dispatch.
     * @param profiles Indicates the profiles used to perform matching for dispatching.
     * @since 4
     */
    public void registerForegroundDispatch(Ability ability, IntentAgent intent, List<String> filters,
        String[][] profiles) {
        synchronized (NfcController.class) {
            if (!isNfcAvailable()) {
                throw new UnsupportedOperationException();
            }
        }
        if (ability == null || intent == null) {
            throw new IllegalArgumentException("abilitySlice or intent is null");
        }
        if (ability.getLifecycle().getLifecycleState() != Lifecycle.Event.valueOf("ON_ACTIVE")) {
            throw new IllegalStateException("Foreground dispatch can not be enabled when ability slice is not active.");
        }
        try {
            ProfileParcel profileParcel = null;
            if (profiles != null && profiles.length > 0) {
                profileParcel = new ProfileParcel(profiles);
            }
            mNfcControllerProxy.registerForegroundDispatch(intent, filters, profileParcel);
        } catch (RemoteException e) {
            HiLog.error(LABEL, "registerForegroundDispatch failed!");
        }
    }

    /**
     * Unregister foreground dispatch to the given NFC OHOSApplication ability.
     *
     * @param ability Indicates the NFC OHOSApplication ability.
     * @since 4
     */
    public void unregisterForegroundDispatch(Ability ability) {
        synchronized (NfcController.class) {
            if (!isNfcAvailable()) {
                throw new UnsupportedOperationException();
            }
        }
        unregisterForegroundDispatchInternal(ability, false);
    }

    private void unregisterForegroundDispatchInternal(Ability ability, boolean force) {
        try {
            mNfcControllerProxy.registerForegroundDispatch(null, null, null);
            if (!force && ability.getLifecycle().getLifecycleState() != Lifecycle.Event.valueOf("ON_ACTIVE")) {
                throw new IllegalStateException("Can not unset foreground dispatching while ability is not active.");
            }
        } catch (RemoteException e) {
            HiLog.error(LABEL, "unsetForegroundDispatchInternal failed!");
        }
    }
}
