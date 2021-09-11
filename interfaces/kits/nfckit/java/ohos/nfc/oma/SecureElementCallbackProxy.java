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

import ohos.rpc.IRemoteObject;
import ohos.rpc.RemoteException;
import ohos.rpc.RemoteObject;
import ohos.rpc.MessageParcel;
import ohos.rpc.MessageOption;

/**
 * Implements the proxy, which called from the underlying service to notify the service status.
 *
 * @hide
 */
public final class SecureElementCallbackProxy extends RemoteObject implements ISecureElementCallback {
    private static final int ON_SERVICE_CONNECTED = 1;
    private static final int ON_SERVICE_DISCONNECTED = 2;
    private SEService mService = null;
    private SEService.OnCallback mCallback = null;

    public SecureElementCallbackProxy(SEService service,
        SEService.OnCallback callback, String descriptor) {
        super(descriptor);
        mService = service;
        mCallback = callback;
    }

    @Override
    public IRemoteObject asObject() {
        return this;
    }

    @Override
    public boolean onRemoteRequest(int code, MessageParcel data, MessageParcel reply, MessageOption option)
        throws RemoteException {
        switch (code) {
            case ON_SERVICE_CONNECTED:
                onServiceConnected();
                return true;
            case ON_SERVICE_DISCONNECTED:
                onServiceDisconnected();
                return true;
            default:
                break;
        }
        return super.onRemoteRequest(code, data, reply, option);
    }

    @Override
    public void onServiceConnected() {
        if (mCallback != null) {
            mCallback.serviceConnected();
        }
        if (mService != null) {
            mService.notifyServiceConnected(true);
        }
    }

    @Override
    public void onServiceDisconnected() {
    }
}