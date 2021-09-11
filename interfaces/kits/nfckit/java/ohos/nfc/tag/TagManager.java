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

package ohos.nfc.tag;

import ohos.hiviewdfx.HiLog;
import ohos.hiviewdfx.HiLogLabel;
import ohos.rpc.RemoteException;
import ohos.nfc.NfcKitsUtils;

/**
 * Controls tag read and write.
 *
 * <p>Classes for different types of tags inherit from this abstract class to control connections to
 * tags, read data from tags, and write data to tags.
 *
 * @since 1
 */
public abstract class TagManager {
    /**
     * Indicates an NFC-A tag.
     *
     * @since 1
     */
    public static final int NFC_A = 1;

    /**
     * Indicates an NFC-B tag.
     *
     * @since 1
     */
    public static final int NFC_B = 2;

    /**
     * Indicates an ISO-DEP tag.
     *
     * @since 1
     */
    public static final int ISO_DEP = 3;

    /**
     * Indicates an NFC-F tag.
     *
     * @since 3
     */
    public static final int NFC_F = 4;

    /**
     * Indicates an NFC-V tag.
     *
     * @since 3
     */
    public static final int NFC_V = 5;

    /**
     * Indicated an NDEF tag.
     *
     * @since 3
     */
    public static final int NDEF = 6;

    /**
     * Indicates a MifareClassic tag.
     *
     * @since 3
     */
    public static final int MIFARE_CLASSIC = 8;

    /**
     * Indicates a MifareUltralight tag.
     *
     * @since 3
     */
    public static final int MIFARE_ULTRALIGHT = 9;

    private static final HiLogLabel LABEL = new HiLogLabel(HiLog.LOG_CORE, NfcKitsUtils.NFC_DOMAIN_ID, "TagManager");

    private TagInfo mTagInfo;
    private int mTagProfile;
    private NfcTagProxy mNfcTagProxy;
    private int mTagHandle;
    private boolean mIsTagConnected = false;

    /**
     * Initializes the constructor for tag objects.
     *
     * @param tagInfo Indicates information about the tag object.
     * @param profile Indicates the profile of the tag object.
     * @hide
     */
    TagManager(TagInfo tagInfo, int profile) {
        if (tagInfo != null) {
            mTagInfo = tagInfo;
            mTagProfile = profile;
            mNfcTagProxy = NfcTagProxy.getInstance();
            mTagHandle = mTagInfo.getTagHandle();
        }
    }

    /**
     * Obtains the tag information.
     *
     * @return Returns the tag information, which is a {@link TagInfo} object.
     * @since 1
     */
    public TagInfo getTagInfo() {
        return mTagInfo;
    }

    /**
     * Connects to a tag.
     *
     * <p>This method must be called before data is read from or written to the tag.
     *
     * @return Returns {@code true} if the connection is set up; returns {@code false} otherwise.
     * @since 1
     */
    public boolean connectTag() {
        try {
            mIsTagConnected = mNfcTagProxy.connectTag(mTagHandle, mTagProfile);
            return mIsTagConnected;
        } catch (RemoteException remoteException) {
            HiLog.error(LABEL, "connectTag open RemoteException");
        }
        return false;
    }

    /**
     * Resets a connection with a tag and restores the default timeout duration for writing data to the tag.
     *
     * @since 1
     */
    public void reset() {
        resetSendDataTimeout();
        reconnectTag();
        mIsTagConnected = false;
    }

    /**
     * Checks whether a connection has been set up with a tag.
     *
     * @return Returns {@code true} if a connection has been set up with the tag;
     * returns {@code false} otherwise.
     * @since 1
     */
    public boolean isTagConnected() {
        try {
            return mNfcTagProxy.isTagConnected(mTagHandle);
        } catch (RemoteException remoteException) {
            HiLog.error(LABEL, "isTagConnected open RemoteException");
        }
        return false;
    }

    /**
     * Sets the timeout duration (ms) for sending data to a tag.
     *
     * <p>If data is not sent to the tag within the duration, data sending fails.
     *
     * @param timeout Indicates the timeout duration to be set.
     * @return Returns {@code true} if the setting is successful; returns {@code false} otherwise.
     * @since 1
     */
    public boolean setSendDataTimeout(int timeout) {
        try {
            return mNfcTagProxy.setSendDataTimeout(mTagProfile, timeout);
        } catch (RemoteException remoteException) {
            HiLog.error(LABEL, "setSendDataTimeout open RemoteException");
        }
        return false;
    }

    /**
     * Queries the timeout duration (ms) for sending data to a tag.
     *
     * @return Returns the timeout duration.
     * @since 1
     */
    public int getSendDataTimeout() {
        try {
            return mNfcTagProxy.getSendDataTimeout(mTagProfile);
        } catch (RemoteException remoteException) {
            HiLog.error(LABEL, "getSendDataTimeout open RemoteException");
        }
        return 0;
    }

    /**
     * Writes data to a tag.
     *
     * @param data Indicates the data to be written to the tag.
     * @return Returns bytes received in response. Or bytes with a length of 0 if the
     * data fails to be written to the tag.
     * @since 1
     */
    public byte[] sendData(byte[] data) {
        if (data == null || data.length == 0) {
            HiLog.warn(LABEL, "method sendData input param is null or empty");
            return new byte[0];
        }

        try {
            return mNfcTagProxy.sendData(mTagHandle, data);
        } catch (RemoteException remoteException) {
            HiLog.error(LABEL, "sendData open RemoteException");
        }
        return new byte[0];
    }

    /**
     * Queries the maximum length of data that can be sent to a tag.
     *
     * @return Returns the maximum length of the data to be sent to the tag.
     * @since 1
     */
    public int getMaxSendLength() {
        try {
            return mNfcTagProxy.getMaxSendLength(mTagProfile);
        } catch (RemoteException remoteException) {
            HiLog.error(LABEL, "getMaxSendLength open RemoteException");
        }
        return 0;
    }

    /**
     * Checks whether this tag is connected or not.
     *
     * @throws IllegalStateException if tag is not connected.
     * @since 3
     */
    public void checkConnected() {
        if (!mIsTagConnected) {
            throw new IllegalStateException("Call connectTag() first!");
        }
    }

    private boolean reconnectTag() {
        try {
            return mNfcTagProxy.reconnectTag(mTagHandle);
        } catch (RemoteException remoteException) {
            HiLog.error(LABEL, "reconnectTag open RemoteException");
        }
        return false;
    }

    private void resetSendDataTimeout() {
        try {
            mNfcTagProxy.resetSendDataTimeout();
        } catch (RemoteException remoteException) {
            HiLog.error(LABEL, "resetSendDataTimeout open RemoteException");
        }
    }
}
