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

import ohos.app.Context;
import ohos.hiviewdfx.HiLog;
import ohos.hiviewdfx.HiLogLabel;
import ohos.nfc.NfcKitsUtils;
import ohos.rpc.RemoteException;
import ohos.utils.system.SystemCap;

import java.util.Arrays;

/**
 * Provides access to secure elements on a device. You can use the interfaces provided by this class to
 * obtain all secure elements and their names, check whether the secure element is available, and enable
 * a session on the secure element.
 *
 * <p>After that, you can send APDU commands to secure elements.
 *
 * @Syscap {@link SystemCapability.Communication#NFC}
 * @since 1
 */
@SystemCap("SystemCapability.Communication.NFC")
public final class SEService {
    private static final HiLogLabel LABEL = new HiLogLabel(HiLog.LOG_CORE, NfcKitsUtils.NFC_DOMAIN_ID,
        "SEService");
    private Context mContext = null;
    private OnCallback mCallback = null;
    private SecureElementCallbackProxy mCallbackProxy = null;
    private boolean mIsServiceConnected = false;
    private Reader[] mReaders = null;
    private String mVersion = "1.0";
    private SecureElementProxy mSecureElementProxy = null;

    /**
     * A callback interface used to notify your application of a successful connection to the secure element service.
     *
     * <p>This interface is called after the {@code serviceConnected} interface is called to connect to
     * the secure element service.
     *
     * @since 1
     */
    public interface OnCallback {
        /**
         * The interface that is called back when the secure element service is connected.
         *
         * @since 1
         */
        void serviceConnected();
    }

    /**
     * A constructor used to create a {@code SEService} instance.
     *
     * @param context Indicates the application context.
     * @param callback Indicates the interface that is called back when the secure element service is connected.
     * @throws IllegalArgumentException if the required context of the caller is missing.
     * @since 1
     */
    public SEService(Context context, OnCallback callback) throws IllegalArgumentException {
        if (context == null) {
            throw new IllegalArgumentException("context is null");
        }
        // init the proxy for zidl
        mSecureElementProxy = new SecureElementProxy(this);
        mContext = context.getApplicationContext();
        mCallback = callback;
        mCallbackProxy = new SecureElementCallbackProxy(this, callback, NfcKitsUtils.CALLBACK_DESCRIPTOR);

        try {
            mIsServiceConnected = mSecureElementProxy.isSeServiceConnected();
        } catch (RemoteException remoteException) {
            HiLog.error(LABEL, "SEService() RemoteException");
        }

        if (mIsServiceConnected) {
            if (mCallback != null) {
                mCallback.serviceConnected();
            }
        } else {
            try {
                if (mContext == null) {
                    HiLog.warn(LABEL, "SEService() context is null.");
                }

                // bind to the nxp service
                mSecureElementProxy.bindSeService(mCallbackProxy);
            } catch (RemoteException remoteException) {
                HiLog.error(LABEL, "SEService RemoteException");
            }
        }
    }

    /**
     * Notifies whether the service is connected or not.
     *
     * @param isConnected Indicates service is connected or not.
     * @hide
     */
    public void notifyServiceConnected(boolean isConnected) {
        mIsServiceConnected = isConnected;
    }

    /**
     * Checks whether the secure element service is connected.
     *
     * @return Returns {@code true} if the service is connected; returns {@code false} otherwise.
     * @since 1
     */
    public boolean isConnected() {
        return mIsServiceConnected;
    }

    /**
     * Disables the secure element service.
     *
     * <p>This method also disables all sessions and channels of the service.
     *
     * @since 1
     */
    public void shutdown() {
        if (mReaders != null && isConnected()) {
            for (Reader se : mReaders) {
                se.closeSessions();
            }
        }
        mReaders = null;
        mIsServiceConnected = false;
    }

    /**
     * Gets the callback.
     *
     * @return Returns the callback.
     * @hide
     */
    public OnCallback getCallback() {
        return mCallback;
    }

    /**
     * Gets the callback proxy.
     *
     * @return Returns the callback proxy.
     * @hide
     */
    public SecureElementCallbackProxy getCallbackProxy() {
        return mCallbackProxy;
    }

    /**
     * Obtains all secure elements.
     *
     * @return Returns secure elements in array format.
     * @since 1
     */
    public Reader[] getReaders() {
        // get secure elements from SA
        try {
            if (isConnected()) {
                mReaders = mSecureElementProxy.getReaders(this);
            }
        } catch (RemoteException remoteException) {
            HiLog.error(LABEL, "getReaders RemoteException");
        }

        if (mReaders != null) {
            return Arrays.copyOf(mReaders, mReaders.length);
        }
        return new Reader[0];
    }

    /**
     * Obtains the version of the secure element service.
     *
     * @return Returns the version of the secure element service.
     * @since 1
     */
    public String getVersion() {
        return mVersion;
    }
}
