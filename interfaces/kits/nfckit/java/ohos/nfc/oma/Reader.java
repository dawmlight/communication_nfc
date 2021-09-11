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
import ohos.rpc.RemoteException;
import ohos.nfc.NfcKitsUtils;
import ohos.utils.system.SystemCap;

import java.util.Optional;

/**
 * Provides interfaces to obtain information about secure elements.
 *
 * <p>The interfaces can be used to obtain the name of a secure element, check whether the secure element
 * is available, and enable a session on the secure element. The {@code getReaders()} method in
 * {@link SEService} can be used to obtain all available secure elements on a device.
 *
 * @Syscap {@link SystemCapability.Communication#NFC}
 * @since 1
 */
@SystemCap("SystemCapability.Communication.NFC")
public final class Reader {
    /**
     * Indicates a basic channel.
     *
     * <p>This constant is used to specify the type of a channel to be enabled.
     *
     * @since 1
     */
    public static final int TYPE_BASIC_CHANNEL = 0;

    /**
     * Indicates a logical channel.
     *
     * <p>This constant is used to specify the type of a channel to be enabled.
     *
     * @since 1
     */
    public static final int TYPE_LOGICAL_CHANNEL = 1;

    private static final HiLogLabel LABEL = new HiLogLabel(HiLog.LOG_CORE, NfcKitsUtils.NFC_DOMAIN_ID, "Reader");
    private String mName;
    private SEService mSEService = null;
    private SecureElementProxy mSecureElementProxy;

    /**
     * A constructor used to create a {@code Reader} instance.
     *
     * @param proxy Indicates the proxy to call functions of NFC system ability.
     * @param name Indicates the name of Reader.
     * @param seService Indicates the secure element service.
     * @hide
     */
    public Reader(SecureElementProxy proxy, String name, SEService seService) {
        if (seService == null || proxy == null) {
            HiLog.error(LABEL, "Reader init fail");
        }
        mSecureElementProxy = proxy;
        mName = name;
        mSEService = seService;
    }

    /**
     * Gets secure element service.
     *
     * @return Returns the secure element service.
     * @hide
     */
    public SEService getSEService() {
        return mSEService;
    }

    /**
     * Obtains the name of a secure element.
     *
     * <p>The name may be {@code eSE} or {@code SIM}.
     *
     * @return Returns the name of the secure element.
     * @since 1
     */
    public String getName() {
        return mName;
    }

    /**
     * Checks whether a secure element is available.
     *
     * <p>Before performing operations on a secure element, check whether it is available.
     *
     * @return Returns {@code true} if the secure element is available; returns {@code false} otherwise.
     * @since 1
     */
    public boolean isSecureElementPresent() {
        try {
            if (mSEService.isConnected()) {
                return mSecureElementProxy.isSecureElementPresent(mName);
            }
        } catch (RemoteException remoteException) {
            HiLog.error(LABEL, "isSecureElementPresent RemoteException");
        }
        return false;
    }

    /**
     * Enables a session on a secure element.
     *
     * @return Returns the session that has been enabled.
     * @since 1
     */
    public Optional<Session> openSession() {
        try {
            if (mSEService.isConnected()) {
                return mSecureElementProxy.openSession(this);
            }
        } catch (RemoteException remoteException) {
            HiLog.error(LABEL, "openSession RemoteException");
        }
        return Optional.empty();
    }

    /**
     * Disables all sessions on a secure element.
     *
     * <p>Resources occupied by sessions are released when the sessions are disabled.
     *
     * @since 4
     */
    public void closeSessions() {
        try {
            if (mSEService.isConnected()) {
                mSecureElementProxy.closeSeSessions(this);
            }
        } catch (RemoteException remoteException) {
            HiLog.error(LABEL, "closeSeSessions RemoteException");
        }
    }
}
