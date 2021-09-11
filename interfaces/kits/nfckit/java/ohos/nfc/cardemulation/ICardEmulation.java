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
import ohos.bundle.ElementName;
import ohos.rpc.IRemoteBroker;
import ohos.rpc.RemoteException;

/**
 * Provides interfaces for card emulation management.
 *
 * @since 1
 * @hide
 */
public interface ICardEmulation extends IRemoteBroker {
    /**
     * Sets the card emulation mode.
     *
     * @param mode Indicates the card emulation mode to be set.
     * @return Returns {@code 0} if the setting is successful; returns a non-zero value otherwise.
     * @throws RemoteException throws this exception if the setting failed.
     */
    void setListenMode(int mode) throws RemoteException;

    /**
     * Checks whether card emulation is enabled.
     *
     * @return Returns {@code true} if card emulation is enabled; returns {@code false} otherwise.
     * @throws RemoteException this exception if the check failed.
     */
    boolean isListenModeEnabled() throws RemoteException;

    /**
     * Obtains the NFC information.
     *
     * <p>This method is used to obtain the NFCC or enabled SE information.
     *
     * @param key Indicates the keyword of the required information.
     * @return Returns the NFCC or enabled SE information.
     * @throws RemoteException Throws this exception when getting NFCC information failed.
     */
    String getNfcInfo(String key) throws RemoteException;

    /**
     * Customizes RF parameters of card emulation.
     *
     * @param configs Indicates the RF parameters to be customized.
     * @param pkg Indicates the package name of calling app.
     * @return Returns {@code 0} if the RF parameters are customized successfully; returns other
     * error codes otherwise.
     * @throws RemoteException throw exception when set configs failed.
     */
    int setRfConfig(String configs, String pkg) throws RemoteException;

    /**
     * Checks whether a specified type of card emulation is supported.
     *
     * <p>This method is used to check Whether the host or secure element supports card emulation.
     *
     * @param feature Indicates the card emulation type, {@code HCE}, {@code SIM}, or {@code ESE}.
     * @return Returns {@code true} if the specified type of card emulation is supported; returns
     * {@code false} otherwise.
     * @throws RemoteException throws this exception if the check failed.
     */
    boolean isSupported(int feature) throws RemoteException;

    /**
     * Registers a foreground app for card emulation.
     *
     * @param appName Indicates the appName that will register.
     * @return Returns {@code true} if the operation is successful; returns {@code false} otherwise.
     * @throws Throws this exception if an error occurs when registering the foreground app.
     */
    boolean registerForegroundPreferred(ElementName appName) throws RemoteException;

    /**
     * Unregisters a preferred foreground app used for card emulation.
     *
     * @return Returns {@code true} if the operation is successful; returns {@code false} otherwise.
     * @throws RemoteException throws this exception if an error occurs when unregistering the preferred foreground app.
     */
    boolean unregisterForegroundPreferred() throws RemoteException;

    /**
     * Queries whether the app is currently the default handler for a specified AID.
     *
     * @param userId Indicates the id of current user.
     * @param appName Indicates the element name of the app.
     * @param aid Indicates the specified aid.
     * @return Returns {@code true} if the app is default handler; returns {@code false} otherwise.
     * @throws RemoteException throws this exception if the check failed.
     */
    boolean isDefaultForAid(int userId, ElementName appName, String aid) throws RemoteException;

    /**
     * Registers a list of AIDs for a specific type for the specified app.
     *
     * @param userId Indicates the id of current user.
     * @param appName Indicates the element name of the app.
     * @param aidGroup Indicates the aid group to register.
     * @return Returns {@code true} if registers success; returns {@code false} otherwise.
     * @throws RemoteException throw exception when register AIDs.
     */
    boolean registerAids(int userId, ElementName appName, AidGroup aidGroup) throws RemoteException;

    /**
     * Removes a previously registered list of AIDs for a specific type for the specified app.
     *
     * @param userId Indicates the id of current user.
     * @param appName Indicates the element name of the app.
     * @param type Indicates the specified aid type.
     * @return Returns {@code true} if removes success; returns {@code false} otherwise.
     * @throws RemoteException throws this exception if the removal failed.
     */
    boolean removeAids(int userId, ElementName appName, String type) throws RemoteException;

    /**
     * Gets a list of AIDs for a specific type for the specified app.
     *
     * @param userId Indicates the id of current user.
     * @param appName Indicates the element name of the app.
     * @param type Indicates the specified aid type.
     * @return Returns an AID group.
     * @throws RemoteException throws this exception if an error occurs when getting the list of AIDs.
     */
    AidGroup getAids(int userId, ElementName appName, String type) throws RemoteException;

    /**
     * Checks whether the specified application is currently the default handler for the specified type.
     *
     * @param userId Indicates the id of current user.
     * @param appName Indicates the name of the application to check.
     * @param type Indicates the specified type in a string.
     * @return Returns {@code true} if the specified application is the default handler for the specified type;
     * returns {@code false} otherwise.
     * @throws RemoteException throws this exception if an error occurs when checking the specified application.
     */
    boolean isDefaultForType(int userId, ElementName appName, String type) throws RemoteException;
}