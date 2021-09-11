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

package ohos.nfc.cardemulation;

import ohos.aafwk.ability.Ability;
import ohos.aafwk.content.Intent;
import ohos.aafwk.content.IntentParams;
import ohos.eventhandler.Courier;
import ohos.eventhandler.EventRunner;
import ohos.eventhandler.InnerEvent;
import ohos.interwork.eventhandler.CourierEx;
import ohos.interwork.eventhandler.EventHandlerEx;
import ohos.interwork.eventhandler.InnerEventUtils;
import ohos.interwork.utils.PacMapEx;
import ohos.hiviewdfx.HiLog;
import ohos.hiviewdfx.HiLogLabel;
import ohos.nfc.NfcKitsUtils;
import ohos.rpc.IRemoteObject;
import ohos.rpc.RemoteException;
import ohos.rpc.RemoteObject;
import ohos.utils.PacMap;

import java.util.Optional;

/**
 * An abstract class that is inherited by the NFC host application.
 *
 * <p>The NFC host application inherits from this abstract class, then NFC service can access the application
 * installation information and connect to services of the application.
 *
 * @since 3
 */
public abstract class HostService extends Ability {
    /**
     * Reason for {@link #disabledCallback(int)}.
     * Indicates disabledCallback was due to the NFC link lost.
     *
     * @hide
     */
    public static final int ERR_LINK_LOSS = 0;

    /**
     * Reason for {@link #disabledCallback(int)}.
     * Indicates that disabledCallback was due to selection of different AIDs.
     *
     * @hide
     */
    public static final int ERR_DESELECTED = 1;

    /**
     * Indicates the name of the service meta-data element.
     *
     * @since 3
     */
    public static final String META_DATA_NAME = "ohos.nfc.cardemulation.data.host_service";

    /**
     * Indicates the service name.
     *
     * @since 3
     */
    public static final String SERVICE_NAME = "ohos.nfc.cardemulation.action.HOST_SERVICE";

    /**
     * Indicates the message ID for the remote command.
     *
     * @hide
     */
    public static final int MSG_REMOTE_COMMAND = 0;

    /**
     * Message ID For App Response.
     *
     * @hide
     */
    public static final int MSG_APP_RESPONSE = 1;

    /**
     * Message ID For Disable Callback.
     *
     * @hide
     */
    public static final int MSG_DISABLED_CALLBACK = 2;


    /**
     * Message ID For Unhandle.
     *
     * @hide
     */
    public static final int MSG_UNHANDLED = 3;

    /**
     * data key in pacmap
     *
     * @hide
     */
    public static final String KEY_DATA = "data";

    private static final HiLogLabel LABEL = new HiLogLabel(HiLog.LOG_CORE, NfcKitsUtils.NFC_DOMAIN_ID,
        "HostService");

    Courier mNfcService = null;

    final CourierEx mCourier = new CourierEx(new MsgHandler(EventRunner.create(true)));

    final class MsgHandler extends EventHandlerEx {
        public MsgHandler(EventRunner runner) {
            super(runner);
        }

        @Override
        public void processEvent(InnerEvent event) {
            if (event == null) {
                return;
            }
            Object object = event.object;
            switch (event.eventId) {
                case MSG_REMOTE_COMMAND:
                    HiLog.error(LABEL, "processEvent: %{public}d", MSG_REMOTE_COMMAND);
                    PacMap dataPacMap = event.getPacMap();
                    if (dataPacMap == null) {
                        return;
                    }
                    if (mNfcService == null) {
                        mNfcService = event.replyTo;
                    }
                    PacMapEx pacMapEx = InnerEventUtils.getPacMapEx(event);
                    if (pacMapEx == null) {
                        return;
                    }
                    Optional<Object> objectOpt = pacMapEx.getObjectValue(KEY_DATA);
                    Object valueObject = objectOpt.get();
                    Optional<byte[]> optByteArray = getObjectValue(valueObject, byte[].class);
                    byte[] apdu = optByteArray.get();
                    if (apdu != null) {
                        byte[] responseApdu = handleRemoteCommand(apdu, null);
                        if (responseApdu != null) {
                            if (mNfcService == null) {
                                HiLog.error(LABEL, "Response not sent; service was deactivated.");
                                return;
                            }
                            InnerEvent responseInnerEvent = InnerEvent.get(MSG_APP_RESPONSE);
                            PacMapEx responsePacMap = new PacMapEx();
                            responsePacMap.putObjectValue(KEY_DATA, responseApdu);
                            InnerEventUtils.setExInfo(responseInnerEvent, 0, 0);
                            responseInnerEvent.replyTo = mCourier;
                            try {
                                mNfcService.send(responseInnerEvent);
                            } catch (RemoteException e) {
                                HiLog.error(LABEL, "Response not sent; RemoteException calling into NfcService.");
                            }
                        }
                    } else {
                        HiLog.error(LABEL, "Received MSG_COMMAND_APDU without data.");
                    }
                    break;
                case MSG_APP_RESPONSE:
                    HiLog.error(LABEL, "processEvent: %{public}d", MSG_APP_RESPONSE);
                    if (mNfcService == null) {
                        HiLog.error(LABEL, "Response not sent; service was deactivated.");
                        return;
                    }
                    try {
                        event.replyTo = mCourier;
                        mNfcService.send(event);
                    } catch (RemoteException e) {
                        HiLog.error(LABEL, "RemoteException calling into NfcService.");
                    }
                    break;
                case MSG_DISABLED_CALLBACK:
                    HiLog.error(LABEL, "processEvent: %{public}d", MSG_DISABLED_CALLBACK);
                    HiLog.info(LABEL, "Received MSG_DISABLED_CALLBACK");
                    mNfcService = null;
                    disabledCallback((int) event.param);
                    break;
                case MSG_UNHANDLED:
                    HiLog.error(LABEL, "processEvent: %{public}d", MSG_UNHANDLED);
                    if (mNfcService == null) {
                        HiLog.error(LABEL, "notifyUnhandled not sent; service was deactivated.");
                        return;
                    }
                    try {
                        event.replyTo = mCourier;
                        mNfcService.send(event);
                    } catch (RemoteException e) {
                        HiLog.error(LABEL, "RemoteException calling into NfcService.");
                    }
                    break;
                default:
                    HiLog.warn(LABEL, "ignored event: %{public}d", event.eventId);
                    break;
            }
        }
    }

    /**
     * Sends a response APDU to the remote device.
     *
     * <p>This method is used by a host application when swiping card.
     *
     * @param response Indicates the response, which is a byte array.
     * @since 3
     */
    public final void sendResponse(byte[] responseApdu) {
        InnerEvent responseInnerEvent = InnerEvent.get(MSG_APP_RESPONSE);
        PacMap dataPacMap = new PacMap();
        dataPacMap.putByteValueArray(KEY_DATA, responseApdu);
        responseInnerEvent.setPacMap(dataPacMap);
        try {
            mCourier.send(responseInnerEvent);
        } catch (RemoteException e) {
            HiLog.error(LABEL, "Local messenger has died.");
        }
    }

    /**
     * Processes a command sent by the remote device.
     *
     * <p>This method is used by a host application when swiping card.
     *
     * @param cmd Indicates the command (a byte array) sent by the remote device.
     * @param params Indicates additional information, which can be null.
     * @return Returns a byte array containing the response sent by the host application.
     * @since 3
     */
    public abstract byte[] handleRemoteCommand(byte[] cmd, IntentParams params);

    /**
     * Callback for a connection exception.
     *
     * <p>This method is called when an exception occurs when connecting to a service.
     *
     * @param errCode Indicates the cause of the exception.
     * @since 3
     */
    public abstract void disabledCallback(int errCode);

    /**
     * Connects to the Host service and obtains a remote service object.
     *
     * <p>If the service is connected, this method can be invoked to obtain a remote service object
     *    for callback.
     *
     * @param intent Indicates the intent of the service that is called back.
     * @return Returns a remote service object.
     * @since 3
     */
    @Override
    public IRemoteObject onConnect(Intent intent) {
        HiLog.error(LABEL, "onConnect!");
        super.onConnect(intent);
        return mCourier.getRemoteObject();
    }

    private final <T> Optional<T> getObjectValue(Object value, Class<T> clazz) {
        if (value == null || clazz == null) {
            return Optional.empty();
        }
        if (clazz == value.getClass()) {
            return (Optional<T>) Optional.of(value);
        }
        HiLog.error(LABEL, "PacMap has type error, the key expect the value type is %{public}s not"
            + " %{public}s, just return default value.", clazz.getName(), value.getClass().getName());
        return Optional.empty();
    }
}
