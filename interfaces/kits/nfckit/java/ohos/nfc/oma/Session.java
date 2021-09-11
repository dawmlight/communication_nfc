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

import java.util.Optional;

/**
 * <p>The interfaces can be used to enable a basic channel, create a logical channel, and disable a session.
 *
 * @Syscap {@link SystemCapability.Communication#NFC}
 * @since 1
 */
@SystemCap("SystemCapability.Communication.NFC")
public final class Session {
    private static final HiLogLabel LABEL = new HiLogLabel(HiLog.LOG_CORE, NfcKitsUtils.NFC_DOMAIN_ID,
        "SecureElementSession");
    private SEService seService = null;
    private SecureElementProxy seProxy = null;
    private IRemoteObject remoteSession = null;

    /**
     * A constructor used to create sessions for a secure element service.
     *
     * @param Indicates the secure element service.
     * @param Indicates the proxy to all functions of SA.
     * @param Indicates the remote object received from the OMA service.
     * @hide
     */
    public Session(SEService service, SecureElementProxy proxy, IRemoteObject remoteSession) {
        if (service == null || seProxy == null) {
            HiLog.error(LABEL, "Session init fail");
        }
        seService = service;
        seProxy = proxy;
        this.remoteSession = remoteSession;
    }

    /**
     * Gets the remote session object.
     *
     * @return Returns the remote object received from OMA service.
     * @hide
     */
    public IRemoteObject getSessionObject() {
        return remoteSession;
    }

    /**
     * Gets the secure element service.
     *
     * @return Returns the secure element service.
     * @hide
     */
    public SEService getSEService() {
        return seService;
    }

    /**
     * Opens a basic channel.
     *
     * @param aid Indicates the {@link Aid} for opening a channel.
     * @return Returns the basic channel.
     * @since 1
     */
    public Optional<Channel> openBasicChannel(Aid aid) {
        if (aid != null && aid.isAidValid() && seService.isConnected() && remoteSession != null) {
            try {
                return seProxy.openBasicChannel(this, aid.getAidBytes());
            } catch (RemoteException remoteException) {
                HiLog.error(LABEL, "openBasicChannel RemoteException");
            }
        }
        return Optional.empty();
    }

    /**
     * Opens a logical channel.
     *
     * @param aid Indicates the {@link Aid} for opening a channel.
     * @return Returns the opened logical channel.
     * @since 1
     */
    public Optional<Channel> openLogicalChannel(Aid aid) {
        if (aid != null && aid.isAidValid() && seService.isConnected() && remoteSession != null) {
            try {
                return seProxy.openLogicalChannel(this, aid.getAidBytes());
            } catch (RemoteException remoteException) {
                HiLog.error(LABEL, "openLogicalChannel RemoteException");
            }
        }
        return Optional.empty();
    }

    /**
     * Obtains the Answer to Reset of a secure element.
     *
     * @return Returns the Answer to Reset in byte array format.
     * @since 1
     */
    public byte[] getATR() {
        try {
            if (seService != null && seService.isConnected() && remoteSession != null) {
                return seProxy.getATR(remoteSession);
            }
        } catch (RemoteException remoteException) {
            HiLog.error(LABEL, "getATR RemoteException");
        }
        return new byte[0];
    }

    /**
     * Disables all channels of a session.
     *
     * @since 1
     */
    public void close() {
        try {
            if (seService.isConnected() && remoteSession != null) {
                seProxy.close(remoteSession);
            }
        } catch (RemoteException remoteException) {
            HiLog.error(LABEL, "close RemoteException");
        }
    }
}
