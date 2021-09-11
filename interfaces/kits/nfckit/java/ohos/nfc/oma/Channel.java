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
import ohos.nfc.NfcKitsUtils;
import ohos.rpc.IRemoteObject;
import ohos.rpc.RemoteException;
import ohos.utils.system.SystemCap;

/**
 * Provides interfaces to create {@code Channel} objects and obtain {@code Channel} information.
 *
 * <p>The interfaces can be used to check whether a channel is disabled or whether a channel is a basic one,
 * disable a channel, send an APDU command, and obtain the response of a secure element.
 *
 * @Syscap {@link SystemCapability.Communication#NFC}
 * @since 1
 */
@SystemCap("SystemCapability.Communication.NFC")
public final class Channel {
    private static final HiLogLabel LABEL = new HiLogLabel(HiLog.LOG_CORE, NfcKitsUtils.NFC_DOMAIN_ID,
        "SecureElementChannel");
    private SEService mSEService = null;
    private SecureElementProxy mSecureElementProxy;
    private IRemoteObject remoteObject = null;
    private int type = Reader.TYPE_BASIC_CHANNEL;

    /**
     * Constuctor used to create a secure element proxy.
     *
     * @param service indicates the secure element service.
     * @param proxy indicates the proxy to all functions of SA.
     * @param remoteObject indicates the remote object received from the OMA service.
     * @param type indicates the channel type.
     * @hide
     */
    public Channel(SEService service, SecureElementProxy proxy, IRemoteObject remoteObject, int type) {
        mSEService = service;
        mSecureElementProxy = proxy;
        this.remoteObject = remoteObject;
        this.type = type;
    }

    /**
     * Checks whether a channel is disabled.
     *
     * <p>A channel must have been enabled before an APDU command is sent through the channel.
     *
     * @return Returns {@code true} if the channel is disabled; returns {@code false} otherwise.
     * @since 1
     */
    public boolean isClosed() {
        try {
            if (mSEService.isConnected() && remoteObject != null) {
                return mSecureElementProxy.isChannelClosed(remoteObject);
            }
        } catch (RemoteException remoteException) {
            HiLog.error(LABEL, "isChannelClosed RemoteException");
        }
        return false;
    }

    /**
     * Checks whether a channel is a basic one.
     *
     * @return Returns {@code true} if the channel is a basic one; returns {@code false} otherwise.
     * @since 1
     */
    public boolean isBasicChannel() {
        return type == Reader.TYPE_BASIC_CHANNEL;
    }

    /**
     * Sends an APDU command to a secure element.
     *
     * @param command Indicates the APDU command.
     * @return Returns the response code.
     * @since 1
     */
    public byte[] transmit(byte[] command) {
        if (command == null || command.length == 0) {
            HiLog.warn(LABEL, "method transmit input param is null or empty");
            return new byte[0];
        }

        try {
            if (mSEService.isConnected() && remoteObject != null) {
                return mSecureElementProxy.transmit(remoteObject, command);
            }
        } catch (RemoteException remoteException) {
            HiLog.error(LABEL, "transmit RemoteException");
        }
        return new byte[0];
    }

    /**
     * Obtains the response to the command to select a secure element.
     *
     * <p>The response is in byte array format.
     *
     * @return Returns the response to the command to select a secure element.
     * @since 1
     */
    public byte[] getSelectResponse() {
        try {
            if (mSEService.isConnected() && remoteObject != null) {
                return mSecureElementProxy.getSelectResponse(remoteObject);
            }
        } catch (RemoteException remoteException) {
            HiLog.error(LABEL, "getSelectResponse RemoteException");
        }
        return new byte[0];
    }

    /**
     * Disables a channel.
     *
     * @since 1
     */
    public void closeChannel() {
        try {
            if (mSEService.isConnected() && remoteObject != null) {
                mSecureElementProxy.closeChannel(remoteObject);
            }
        } catch (RemoteException remoteException) {
            HiLog.error(LABEL, "closeChannel RemoteException");
        }
    }
}
