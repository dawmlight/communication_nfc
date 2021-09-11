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

import ohos.rpc.IRemoteBroker;
import ohos.rpc.RemoteException;

/**
 * Provides interfaces to manage or access tags.
 *
 * The NfcTagProxy implements these interfaces.
 *
 * @hide
 */
public interface ITagInfo extends IRemoteBroker {
    /**
     * Connects a tag based on the protocol or technology.
     *
     * @param tagHandle Indicates the handle the NFC service assigned for this tag.
     * @param tagProfile Indicates the specific protocol or technology.
     * @return Returns {@code true} if the connection is set up; returns {@code false} otherwise.
     * @throws RemoteException throws this exception if the tag fails to be connected.
     */
    boolean connectTag(int tagHandle, int tagProfile) throws RemoteException;

    /**
     * Reconnects a tag based on the tag handle.
     *
     * @param tagHandle Indicates the handle the NFC service assigned for this tag.
     * @return Returns {@code true} if the tag is reconnected successfully; returns {@code false} otherwise.
     * @throws RemoteException throws this exception if the tag fails to be reconnected.
     */
    boolean reconnectTag(int tagHandle) throws RemoteException;

    /**
     * Checks whether tag is connected or not.
     *
     * @param tagHandle Indicates the handle the NFC service assigned for this tag.
     * @return Returns {@code true} if the tag is connected; returns {@code false} otherwise.
     * @throws RemoteException throws this exception if the check failed.
     */
    boolean isTagConnected(int tagHandle) throws RemoteException;

    /**
     * Sets a timeout value for data sending.
     *
     * @param tagProfile Indicates the specific protocol.
     * @param timeout Indicates the timeout value to set, in ms.
     * @return Returns {@code true} if the timeout value is successfully set; returns {@code false} otherwise.
     * @throws RemoteException throws this exception if setting the timeout value failed.
     */
    boolean setSendDataTimeout(int tagProfile, int timeout) throws RemoteException;

    /**
     * Gets the timeout value of the current NFC tag.
     *
     * @param tagProfile Indicates the specific protocol or technology.
     * @return Returns the timeout of this tag.
     * @throws RemoteException Throws this exception if getting the timeout value failed.
     */
    int getSendDataTimeout(int tagProfile) throws RemoteException;

    /**
     * Writes data to tag device.
     *
     * @param tagHandle Indicates the handle the NFC service assigned for this tag.
     * @param cmdData Indicates the data writed to tag device.
     * @return Returns the response from tag device.
     * @throws RemoteException throws this exception if writing data failed.
     */
    byte[] sendData(int tagHandle, byte[] cmdData) throws RemoteException;

    /**
     * Gets the maximum length of data to be sent.
     *
     * @param tagProfile Indicates the specific protocol or technology.
     * @return Returns the tag supported maximum length.
     * @throws RemoteException throws this exception if getting the maximum data length failed.
     */
    int getMaxSendLength(int tagProfile) throws RemoteException;

    /**
     * Resets the timeout value to the default.
     *
     * @throws RemoteException throws this exception if resetting the timeout value failed.
     */
    void resetSendDataTimeout() throws RemoteException;

    /**
     * Checks if the tag connected is an NDEF tag.
     *
     * @param TagHandle Indicates the handle the NFC service assigned for this tag.
     * @return Returns {@code true} if the tag is NDEF tag; returns {@code false} otherwise.
     * @throws RemoteException throws this exception if the check failed.
     */
    boolean isNdefTag(int TagHandle) throws RemoteException;

    /**
     * Reads message from NDEF tag.
     *
     * @param tagHandle Indicates the handle the NFC service assigned for this tag.
     * @return Returns the read NDEF messages if any; returns null string otherwise.
     * @throws RemoteException throws this exception if the reading failed.
     */
    NdefMessage ndefRead(int tagHandle) throws RemoteException;

    /**
     * Writes messages to the NDEF tag.
     *
     * @param tagHandle Indicates the handle the NFC service assigned for this tag.
     * @param Instance Indicates an NDEF message.
     * @return Returns {@code 0} if the writing is successful; returns {@code -1} otherwise.
     * @throws RemoteException throw exception when write failed.
     */
    int ndefWrite(int tagHandle, NdefMessage ndefMsg) throws RemoteException;

    /**
     * Sets the NDEF tag to read-only.
     *
     * @param tagHandle Indicates the handle the NFC service assigned for this tag.
     * @return Returns {@code 0} is the setting is successful; returns {@code -1} otherwise.
     * @throws RemoteException throws this exception if the setting failed.
     */
    int ndefSetReadOnly(int tagHandle) throws RemoteException;

    /**
     * Checks if the the NDEF tag of specified type can be set to the read-only mode.
     *
     * @param ndefType Indicates the NDEF type.
     * @return Returns {@code true} if the setting is successful; returns {@code false} otherwise.
     * @throws RemoteException throws this exception if the setting failed.
     */
    boolean canSetReadOnly(int ndefType) throws RemoteException;
}