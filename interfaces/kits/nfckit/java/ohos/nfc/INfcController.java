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
import ohos.rpc.IRemoteBroker;
import ohos.rpc.RemoteException;
import ohos.rpc.IRemoteObject;
import ohos.interwork.utils.PacMapEx;
import ohos.nfc.NfcController.ReaderModeCallback;

import java.util.List;

/**
 * Provides interfaces for NFC management.
 *
 * @hide
 */
public interface INfcController extends IRemoteBroker {
    /**
     * Enables or disables NFC.
     *
     * @param isEnabled Indicates whether NFC is enabled. Value {@code true} means that NFC is on,
     * and values {@code false} means off.
     * @return Returns {@code 1} for success; {@code -1} for failure, and {@code -6} for no
     * permissions, and any other value for other failures.
     * @throws RemoteException throws this exception if getting the NFC service failed.
     */
    int setNfcEnabled(boolean isEnabled) throws RemoteException;

    /**
     * Gets the current NFC state.
     *
     * @return Returns the current NFC state.
     * @throws RemoteException throws this exception if getting the NFC service failed.
     */
    int getNfcState() throws RemoteException;

    /**
     * Checks if NFC is supported.
     *
     * @return Returns {@code true} if NFC is supported; returns {@code false} otherwise.
     * @throws RemoteException throws this exception if getting the NFC service failed.
     */
    boolean isNfcAvailable() throws RemoteException;

    /**
     * Registers foreground dispatch to the given ability.
     *
     * @param intent Indicates the IntentAgent to start for the dispatch.
     * @param intentFilterStrs Indicates the IntentFilters to override dispatching for, or null to always dispatch.
     * @param profiles Indicates the profiles used to perform matching for dispatching.
     * @throws RemoteException throws this exception if setting foreground dispatch failed.
     */
    void registerForegroundDispatch(IntentAgent intent, List<String> intentFilterStrs,
        ProfileParcel profiles) throws RemoteException;

    /**
     * Sets the NFC controller to reader mode only when this ability is in the foreground.
     *
     * @param token Indicates the remote object.
     * @param callBack Indicates the callback to be called when a tag is discovered.
     * @param flags Indicates poll profiles and other optional parameters.
     * @param pacMapEx Indicates additional extras for configuring reader mode.
     * @param controller Indicates the controller for configuring reader mode.
     * @throws RemoteException throws this exception if setting the reader mode failed.
     */
    void setReaderMode(IRemoteObject token, ReaderModeCallback callBack, int flags,
        PacMapEx pacMapEx, NfcController controller) throws RemoteException;
}