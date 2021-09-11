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

import ohos.rpc.IRemoteBroker;
import ohos.rpc.IRemoteObject;
import ohos.rpc.RemoteException;

import java.util.Optional;

/**
 * Provides interfaces to access secure elements.
 *
 * These interfaces are implemented by {@code SecureElementProxy}.
 *
 * @hide
 */
public interface ISecureElement extends IRemoteBroker {
    /**
     * Checks whether the secure element service is connected or not.
     *
     * @return Returns {@code true} if the service is connected; returns {@code false} otherwise.
     * @throws RemoteException throws this exception when the check failed.
     */
    boolean isSeServiceConnected() throws RemoteException;

    /**
     * Binds the secure element service.
     *
     * @param proxy Indicates the proxy for callback to application.
     * @throws RemoteException throws this exception when service binding failed.
     */
    void bindSeService(SecureElementCallbackProxy proxy) throws RemoteException;

    /**
     * Obtain all secure elements.
     *
     * @param service Indicates the secure element service.
     * @return Returns a list of secure elements in array format.
     * @throws RemoteException throw exception when obtain secure elements failed.
     */
    Reader[] getReaders(SEService service) throws RemoteException;

    /**
     * Checks whether a secure element is available.
     *
     * <p>Before performing operations on a secure element, check whether it is available.
     *
     * @param name Indicates the name of secure element.
     * @return Returns {@code true} if the secure element is available; returns {@code false} otherwise.
     * @throws RemoteException throws this exception when checking the secure element status failed.
     */
    boolean isSecureElementPresent(String name) throws RemoteException;

    /**
     * Opens the basic channel.
     *
     * @param session Indicates the session to open channel.
     * @param aid Indicates the aid to open basic channel.
     * @return Returns the basic channel.
     * @throws RemoteException throws this exception when opening the basic channel failed.
     */
    Optional<Channel> openBasicChannel(Session session, byte[] aid) throws RemoteException;

    /**
     * Opens the logical channel.
     *
     * @param session Indicates the session to open channel.
     * @param aid Indicates the aid to open the logical channel.
     * @return Returns the logical channel.
     * @throws RemoteException throw this exception when opening the logical channel failed.
     */
    Optional<Channel> openLogicalChannel(Session session, byte[] aid) throws RemoteException;

    /**
     * Sends an APDU command to the NFC controller.
     *
     * @param remoteObject Indicates the remote object of this session.
     * @param command Indicates the apdu to send.
     * @return Returns the response received from the NFC controller.
     * @throws this exception if an error occurs when sending the APDU command.
     */
    byte[] transmit(IRemoteObject remoteObject, byte[] command) throws RemoteException;

    /**
     * Gets the response of the application select command.
     *
     * @param remoteObject Indicates the remote object of this session.
     * @return Returns the response.
     * @throws RemoteException throws this exception when getting the response failed.
     */
    byte[] getSelectResponse(IRemoteObject remoteObject) throws RemoteException;

    /**
     * Opens a session on the secure element.
     *
     * @param se Indicates the secure element to open session.
     * @return Returns the session.
     * @throws RemoteException throws this exception when opening the session on this secure element failed.
     */
    Optional<Session> openSession(Reader se) throws RemoteException;

    /**
     * Gets the Answer to Reset (ATR) of this secure element.
     *
     * @param remoteObject Indicates the remote object of this session.
     * @return Returns the ATR.
     * @throws RemoteException throws this exception when getting the ATR failed.
     */
    byte[] getATR(IRemoteObject remoteObject) throws RemoteException;

    /**
     * Closes all sessions on the secure element.
     *
     * @param se Indicates the secure element to close sessions.
     * @throws RemoteException throws this exception when closing sessions on this secure element failed.
     */
    void closeSeSessions(Reader se) throws RemoteException;

    /**
     * Closes all channels of this session.
     *
     * @param remoteObject Indicates the remote object of session.
     * @throws RemoteException throws this exception when closing channels on this session failed.
     */
    void close(IRemoteObject remoteObject) throws RemoteException;

    /**
     * Closes the channel.
     *
     * @param remoteObject Indicates the remote object of channel.
     * @throws RemoteException throws this exception when closing the channel failed.
     */
    void closeChannel(IRemoteObject remoteObject) throws RemoteException;

    /**
     * Checks if the channel is closed or not.
     *
     * @param remoteObject Indicates the remote object of channel.
     * @return Returns the result indicating whether the channel is closed.
     * @throws RemoteException throws this exception when getting the channel status failed.
     */
    boolean isChannelClosed(IRemoteObject remoteObject) throws RemoteException;
}