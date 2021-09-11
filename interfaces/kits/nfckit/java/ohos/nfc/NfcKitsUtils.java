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

import ohos.utils.Parcel;

/**
 * NFC kits utils.
 *
 * @since 1
 * @hide
 */
public final class NfcKitsUtils {
    /** NFC Domain ID. */
    public static final int NFC_DOMAIN_ID = 0xD001570;

    /** NFC Adapter descriptor to call */
    public static final String ADAPTER_DESCRIPTOR = "ohos.nfc.INfcAdapter";

    /** NXP NFC Adapter descriptor to call */
    public static final String NXP_ADAPTER_DESCRIPTOR = "com.nxp.nfc.INxpNfcAdapter";

    /** Tag descriptor to call */
    public static final String TAG_DESCRIPTOR = "ohos.nfc.INfcTag";

    /** SE service descriptor to call */
    public static final String SE_SERVICE_DESCRIPTOR = "ohos.se.omapi.ISecureElementService";

    /** Session descriptor to call */
    public static final String SESSION_DESCRIPTOR = "ohos.se.omapi.ISecureElementSession";

    /** Channel descriptor to call */
    public static final String CHANNEL_DESCRIPTOR = "ohos.se.omapi.ISecureElementChannel";

    /** Reader descriptor to call */
    public static final String READER_DESCRIPTOR = "ohos.se.omapi.ISecureElementReader";

    /** Callback package name to call */
    public static final String CALLBACK_DESCRIPTOR = "com.nxp.nfc.ISecureElementCallback";

    /** Callback package name to transcat */
    public static final String READER_MODE_CALLBACK_DESCRIPTOR = "com.nxp.nfc.IReaderModeCallback";

    /** Card emulation descriptor to call */
    public static final String CARD_EMULATION_DESCRIPTOR = "ohos.nfc.INfcCardEmulation";

    /** Strict policy to call */
    public static final int STRICT_POLICY = 1;

    /** Work source to call */
    public static final int WORK_SOURCE = 1;

    /** Sync call to the function of SA */
    public static final int TRANS_SYNC = 0;

    /** Set the NFC enabled state. */
    public static final int MSG_SET_NFC_ENABLED = 1;

    /** Transaction code for get the NFC state */
    public static final int MSG_GET_NFC_STATE = 2;

    /** Set the NFC available state. */
    public static final int MSG_IS_NFC_AVAILABLE = 3;

    /** Set the NFC connecting tag */
    public static final int MSG_CONNECT_TAG = 4;

    /** Set the NFC reconnecting tag */
    public static final int MSG_RECONNECT_TAG = 5;

    /** Set the NFC connected tag */
    public static final int MSG_IS_TAG_CONNECT = 6;

    /** Set timeout */
    public static final int MSG_SET_SEND_DATA_TIMEOUT = 7;

    /** Get timeout */
    public static final int MSG_GET_SEND_DATA_TIMEOUT = 8;

    /** Send raw data */
    public static final int MSG_SEND_RAW_APDU_DATA = 9;

    /** Get max send length */
    public static final int MSG_GET_MAX_SEND_LENGTH = 10;

    /** Reset timeout */
    public static final int MSG_RESET_DATA_TIMEOUT = 11;

    /** Get nxp service */
    public static final int GET_NFC_SERVICE = 12;

    /** Get secure elements */
    public static final int BIND_SE_SERVICE = 13;

    /** Set the SE service connected state */
    public static final int IS_SE_SERVICE_CONNECTED = 14;

    /** Transaction code for get secure elements */
    public static final int GET_SECURE_ELEMENTS = 15;

    /** Set the SE present state */
    public static final int IS_SE_PRESENT = 16;

    /** Open a channel */
    public static final int OPEN_CHANNEL = 17;

    /** Transmit APDU */
    public static final int TRANSMIT_APDU = 18;

    /** Close a channel */
    public static final int CLOSE_CHANNEL = 19;

    /** Set the listen mode */
    public static final int SET_LISTEN_MODE = 20;

    /** Set the listen mode to the enabled state */
    public static final int IS_LISTEN_MODE_ENABLED = 21;

    /** Transaction code for get NFC info */
    public static final int GET_NFC_INFO = 22;

    /** Transaction code for set RF config */
    public static final int SET_RF_CONFIG = 23;

    /** Transaction code for get info from config */
    public static final int GET_INFO_FROM_CONFIG = 24;

    /** Transaction code for get select response */
    public static final int GET_SELECT_RESPONSE = 25;

    /** Open sessions for this SE */
    public static final int OPEN_SESSION = 26;

    /** Close sessions for this SE */
    public static final int CLOSE_SE_SESSIONS = 27;

    /** Close channels of this session */
    public static final int CLOSE_SESSION_CHANNELS = 28;

    /** Transaction code for get atr from this session */
    public static final int GET_ATR = 29;

    /** Check the state of this channel */
    public static final int IS_CHANNEL_CLOSED = 30;

    /** Check if the tag is an NDEF tag */
    public static final int IS_NDEF = 31;

    /** Read messages from the NDEF tag */
    public static final int NDEF_READ = 32;

    /** Write messages to the NDEF tag */
    public static final int NDEF_WRITE = 33;

    /** Set the NDEF tag to read-only */
    public static final int NDEF_SET_READONLY = 34;

    /** Check if the NDEF tag can be set to read-only */
    public static final int CAN_SET_READONLY = 35;

    /** Set the foreground service */
    public static final int SET_FOREGROUND_SERVICE = 36;

    /** Unset the foreground service */
    public static final int UNSET_FOREGROUND_SERVICE = 37;

    /** Check if the app is the default handler for the AID */
    public static final int IS_DEFAULT_FOR_AID = 38;

    /** Transaction code for registering aids */
    public static final int REGISTER_AIDS = 39;

    /** Transaction code for removing aids */
    public static final int REMOVE_AIDS = 40;

    /** Transaction code for getting aids */
    public static final int GET_AIDS = 41;

    /** Check if the app is the default handler for the type */
    public static final int IS_DEFAULT_FOR_TYPE = 42;

    /** Register foreground dispatch */
    public static final int REGISTER_FOREGROUND_DISPATCH = 43;

    /** Set the reader mode */
    public static final int SET_READER_MODE = 44;

    /**
     * Writes an interface token to the parcel object for the SA.
     *
     * @param descriptor Indicates the descriptor of this API.
     * @param data Indicates the parcel object for the SA.
     */
    public static void writeInterfaceToken(String descriptor, Parcel data) {
        if (data == null) {
            return;
        }
        data.writeInt(STRICT_POLICY);
        data.writeInt(WORK_SOURCE);
        data.writeString(descriptor);
    }
}
