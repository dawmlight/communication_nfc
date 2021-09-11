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

import ohos.aafwk.ability.Ability;
import ohos.aafwk.ability.AbilityLifecycleCallbacks;
import ohos.aafwk.ability.Lifecycle;
import ohos.aafwk.ability.LifecycleObserver;
import ohos.aafwk.ability.AbilityPackage;
import ohos.aafwk.content.Intent;
import ohos.agp.components.Component;
import ohos.hiviewdfx.HiLog;
import ohos.hiviewdfx.HiLogLabel;
import ohos.interwork.utils.PacMapEx;
import ohos.nfc.NfcController.ReaderModeCallback;
import ohos.nfc.tag.NdefMessage;
import ohos.rpc.RemoteException;
import ohos.rpc.RemoteObject;
import ohos.utils.PacMap;
import ohos.utils.net.Uri;
import ohos.utils.system.safwk.java.SystemAbilityDefinition;

import java.util.ArrayList;
import java.util.LinkedList;
import java.util.List;
import java.util.Optional;

/**
 * Provides methods for setting reader mode for an {@code Ability}.
 *
 * @hide
 * @since 3
 */
class NfcAbilityManager extends NfcCommProxy implements AbilityLifecycleCallbacks {
    static final HiLogLabel LABEL = new HiLogLabel(HiLog.LOG_CORE, NfcKitsUtils.NFC_DOMAIN_ID,
        "NfcAbilityManager");

    private final NfcController mNfcController;
    private final List<NfcAppState> mAppStates;
    private final List<NfcAbilityState> mAbilities;
    private NfcControllerProxy mNfcControllerProxy;

    /**
     * NFC state associated with an AbilityPackage.
     *
     * @hide
     */
    class NfcAppState {
        int refCount = 0;
        final AbilityPackage app;
        public NfcAppState(AbilityPackage app) {
            this.app = app;
        }
        /**
         * Register the NFC AbilityPackage state.
         *
         * @since 3
         */
        public void register() {
            refCount++;
            if (refCount == 1) {
                this.app.registerCallbacks(NfcAbilityManager.this, null);
            }
        }

        /**
         * Unregister the NFC AbilityPackage state.
         *
         * @since 3
         */
        public void unregister() {
            refCount--;
            if (refCount == 0) {
                this.app.unregisterCallbacks(NfcAbilityManager.this, null);
            } else if (refCount < 0) {
                HiLog.error(LABEL, "refCount less than zero.");
            } else {
                return;
            }
        }
    }

    /* get the app's NFC state of AbilityPackage */
    Optional<NfcAppState> getAppState(AbilityPackage app) {
        for (NfcAppState appState : mAppStates) {
            if (appState.app.equals(app)) {
                return Optional.of(appState);
            }
        }
        return Optional.empty();
    }

    /* register a AbilityPackage */
    void registerHarmonyApplication(AbilityPackage app) {
        Optional<NfcAppState> appState = getAppState(app);
        if (!appState.isPresent()) {
            appState = Optional.of(new NfcAppState(app));
            mAppStates.add(appState.get());
        }
        appState.get().register();
    }

    /* unregister a AbilityPackage */
    void unregisterHarmonyApplication(AbilityPackage app) {
        Optional<NfcAppState> appState = getAppState(app);
        if (!appState.isPresent()) {
            HiLog.error(LABEL, "app was not registered : %{public}s", app);
            return;
        }
        appState.get().unregister();
    }

    class NfcAbilityState {
        boolean activated = true;
        Ability ability;
        Optional<NdefMessage> ndefMessage = Optional.empty();
        Uri[] uris = null;
        int flags = 0;
        int readerModeFlags = 0;
        ReaderModeCallback readerModeCallback = null;
        PacMapEx readerModeExtras = null;
        RemoteObject token;
        NfcController controller;

        public NfcAbilityState(Ability ability) {
            // Check if ability is activated right now, as we will not immediately get a callback for that
            LifecycleObserver lifecycleObserver = new LifecycleObserver() {
                @Override
                public void onStart(Intent intent) {
                    HiLog.debug(LABEL, "NfcAbilityState onStart");
                }
                @Override
                public void onInactive() {
                    HiLog.debug(LABEL, "NfcAbilityState onInactive");
                    activated = false;
                }
                @Override
                public void onActive() {
                    HiLog.debug(LABEL, "NfcAbilityState onActive");
                    activated = true;
                }
                @Override
                public void onBackground() {
                    HiLog.debug(LABEL, "NfcAbilityState onBackground");
                    activated = false;
                }
                @Override
                public void onForeground(Intent intent) {
                    HiLog.debug(LABEL, "NfcAbilityState onForeground");
                    activated = true;
                }
                @Override
                public void onStop() {
                    HiLog.debug(LABEL, "NfcAbilityState onStop");
                    activated = false;
                }
            };
            ability.getLifecycle().addObserver(lifecycleObserver);
            this.ability = ability;
            this.token = new RemoteObject("NfcAbilityState");
            this.controller = mNfcController;
            registerHarmonyApplication(ability.getAbilityPackage());
        }

        /**
         * Resets the NFC state when the Ability of Application is terminated
         *
         * @since 3
         */
        public void terminate() {
            unregisterHarmonyApplication(ability.getAbilityPackage());
            activated = false;
            ability = null;
            ndefMessage = Optional.empty();
            uris = null;
            readerModeFlags = 0;
            token = null;
        }
    }

    synchronized void terminateAbilityState(Ability ability) {
        Optional<NfcAbilityState> abilityState = findAbilityState(ability);
        if (abilityState.isPresent()) {
            abilityState.get().terminate();
            mAbilities.remove(abilityState.get());
        }
    }

    public NfcAbilityManager(NfcController nfcController) {
        super(SystemAbilityDefinition.NFC_MANAGER_SYS_ABILITY_ID);
        mNfcController = nfcController;
        mAbilities = new LinkedList<NfcAbilityState>();
        mAppStates = new ArrayList<NfcAppState>(1);  // OHOS VM usually has 1 app
        mNfcControllerProxy = NfcControllerProxy.getInstance();
    }

    /**
     * Disables reader mode for a specified {@code Ability}.
     *
     * @param ability Indicates the specified {@code Ability}.
     * @since 3
     */
    public void unsetReaderMode(Ability ability) {
        boolean isActivated;
        RemoteObject token;
        synchronized (NfcAbilityManager.this) {
            NfcAbilityState state = getAbilityState(ability).get();
            state.readerModeCallback = null;
            state.readerModeFlags = 0;
            state.readerModeExtras = null;
            token = state.token;
            isActivated = state.activated;
        }
        if (isActivated) {
            setReaderMode(token, null, 0, null, null);
        }
    }

    /**
     * Enables reader mode for a specified {@code Ability}.
     *
     * @param ability Indicates the specified {@code Ability}.
     * @param callback Indicates the callback to be invoked when an NFC tag is discovered.
     * @param flags Indicates polling technologies and some optional parameters.
     * @param pacMapEx Indicates additional parameters for enabling reader mode.
     * @param controller Indicates the controller for configuring reader mode.
     * @since 3
     */
    public void setReaderMode(Ability ability, ReaderModeCallback callback, int flags,
        PacMapEx pacMapEx, NfcController controller) {
        boolean isActive;
        RemoteObject token;
        synchronized (NfcAbilityManager.this) {
            NfcAbilityState state = getAbilityState(ability).get();
            state.readerModeCallback = callback;
            state.readerModeFlags = flags;
            state.readerModeExtras = pacMapEx;
            token = state.token;
            isActive = state.activated;
        }
        if (isActive) {
            setReaderMode(token, callback, flags, pacMapEx, controller);
        }
    }

    /**
     * Sets the NFC AbilityPackage ability to reader mode only when this ability is in the foreground.
     *
     * @param token Indicates the token of NFC state.
     * @param callback Indicates the callback to be called when a tag is discovered.
     * @param flags Indicates poll profiles and other optional parameters.
     * @param pacMapEx Indicates additional extras for configuring reader mode.
     * @param controller Indicates the controller for configuring reader mode.
     * @since 3
     */
    public void setReaderMode(RemoteObject token, ReaderModeCallback callback,
        int flags, PacMapEx pacMapEx, NfcController controller) {
        HiLog.debug(LABEL, "Setting reader mode in");
        try {
            mNfcControllerProxy.setReaderMode(token, callback, flags, pacMapEx, controller);
        } catch (RemoteException e) {
            HiLog.error(LABEL, "setReaderMode failed!");
        }
    }

    /**
     * gets NfcAbilityState from mActivities if any and creates one if none.
     *
     * @param ability Indicates the NFC AbilityPackage ability.
     * @return Returns the NfcAbilityState obtained from mActivities if any; returns the newly created
     * NfcAbilityState otherwise.
     * @since 3
     */
    synchronized Optional<NfcAbilityState> getAbilityState(Ability ability) {
        Optional<NfcAbilityState> state = findAbilityState(ability);
        if (!state.isPresent()) {
            state = Optional.of(new NfcAbilityState(ability));
            mAbilities.add(state.get());
        }
        return state;
    }

    /**
     * Finds NfcAbilityState from mActivities.
     *
     * @param ability Indicates the NFC AbilityPackage ability.
     * @return returns the NfcAbilityState found from mActivities.
     * @since 3
     */
    synchronized Optional<NfcAbilityState> findAbilityState(Ability ability) {
        for (NfcAbilityState state : mAbilities) {
            if (state.ability.equals(ability)) {
                return Optional.of(state);
            }
        }
        return Optional.empty();
    }

    /**
     * Callback from the Ability lifecycle when an Ability is started in the main thread.
     *
     * @param ability Indicates the NFC AbilityPackage ability.
     * @since 3
     */
    @Override
    public void onAbilityStart(Ability ability) { /* NO-OP */ }
    /**
     * Callback from the Ability lifecycle when an Ability is activated in the main thread.
     *
     * @param ability Indicates the NFC AbilityPackage ability.
     * @since 3
     */
    @Override
    public void onAbilityActive(Ability ability) { /* NO-OP */ }
    /**
     * Callback from Ability life-cycle when ability active, on main thread.
     *
     * @param ability Indicates the NFC AbilityPackage ability.
     * @since 3
     */
    @Override
    public void onAbilityInactive(Ability ability) { /* NO-OP */ }
    /**
     * Callback from the Ability lifecycle when an Ability in the main thread is running at the foreground.
     *
     * @param ability Indicates the NFC AbilityPackage ability.
     * @since 3
     */
    @Override
    public void onAbilityForeground(Ability ability) {
        int readerModeFlags = 0;
        PacMapEx readerModeExtras = null;
        RemoteObject token;
        ReaderModeCallback callback;
        NfcController controller;
        synchronized (NfcAbilityManager.this) {
            Optional<NfcAbilityState> state = findAbilityState(ability);
            HiLog.debug(LABEL, "onAbilityForeground() for %{public}s state", ability);
            if (!state.isPresent()) {
                return;
            }
            state.get().activated = true;
            token = state.get().token;
            readerModeFlags = state.get().readerModeFlags;
            readerModeExtras = state.get().readerModeExtras;
            callback = state.get().readerModeCallback;
            controller = state.get().controller;
        }
        if (readerModeFlags != 0) {
            setReaderMode(token, callback, readerModeFlags, readerModeExtras, controller);
        }
    }

    /**
     * Callback from the Ability lifecycle when an Ability in the main thread is running at the background.
     *
     * @param ability Indicates the NFC AbilityPackage ability.
     * @since 3
     */
    @Override
    public void onAbilityBackground(Ability ability) {
        boolean readerModeFlagsSet;
        RemoteObject token;
        NfcController controller;
        synchronized (NfcAbilityManager.this) {
            Optional<NfcAbilityState> state = findAbilityState(ability);
            HiLog.debug(LABEL, "onAbilityBackground() for %{public}s state", ability);
            if (!state.isPresent()) {
                return;
            }
            state.get().activated = false;
            token = state.get().token;
            readerModeFlagsSet = state.get().readerModeFlags != 0;
            controller = state.get().controller;
        }
        if (readerModeFlagsSet) {
            // Restore default p2p modes
            setReaderMode(token, null, 0, null, controller);
        }
    }

    /**
     * Callback from the Ability lifecycle when an Ability is stopped in the main thread.
     *
     * @param ability Indicates the NFC AbilityPackage ability.
     * @since 3
     */
    @Override
    public void onAbilityStop(Ability ability) {
        synchronized (NfcAbilityManager.this) {
            Optional<NfcAbilityState> state = findAbilityState(ability);
            HiLog.debug(LABEL, "onAbilityStop() for %{public}s state", ability);
            if (state.isPresent()) {
                // release all associated references
                terminateAbilityState(ability);
            }
        }
    }

    @Override
    public void onAbilitySaveState(PacMap pacMap) { /* NO-OP */ }
}