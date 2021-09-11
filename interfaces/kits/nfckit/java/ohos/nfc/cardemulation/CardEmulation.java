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
import ohos.app.Context;
import ohos.bundle.ElementName;
import ohos.hiviewdfx.HiLog;
import ohos.hiviewdfx.HiLogLabel;
import ohos.nfc.NfcKitsUtils;
import ohos.nfc.NfcController;
import ohos.rpc.RemoteException;
import ohos.utils.system.SystemCap;

import java.util.ArrayList;
import java.util.List;
import java.util.regex.Pattern;

/**
 * Provides methods for card emulation management.
 *
 * <p>These methods can be used to enable and disable card emulation, check whether card emulation is supported, query
 * the card emulation state, customize radio frequency (RF) parameters, and set a preferred foreground application.
 *
 * @Syscap {@link SystemCapability.Communication#NFC}
 * @since 1
 */
@SystemCap("SystemCapability.Communication.NFC")
public final class CardEmulation {
    /**
     * Indicates that card emulation of type A and B is disabled.
     *
     * @since 1
     */
    public static final int DISABLE_MODE_A_B = 0x0;

    /**
     * Indicates that all types of card emulation are enabled.
     *
     * @since 1
     */
    public static final int ENABLE_MODE_ALL = 0x3;

    /**
     * Indicates the keyword used to obtain the enabled SE type.
     *
     * @since 1
     */
    public static final String KEY_ENABLED_SE_TYPE = "ese_type";

    /**
     * Indicates the keyword used to obtain the HISEE status.
     *
     * @since 1
     */
    public static final String KEY_HISEE_READY = "hisee_ready";

    /**
     * Indicates the keyword used to obtain the status of supporting RSSI query.
     *
     * @since 1
     */
    public static final String KEY_RSSI_SUPPORTED = "rssi_support";

    /**
     * Indicates the enabled SE type is eSE.
     *
     * @since 1
     */
    public static final String ENABLED_SE_TYPE_ESE = "11";

    /**
     * Indicates the enabled SE type is hisee.
     *
     * @since 1
     */
    public static final String ENABLED_SE_TYPE_HISEE = "12";

    /**
     * Indicates the hisee is ready.
     *
     * @since 1
     */
    public static final String HISEE_READY = "true";

    /**
     * Indicates the hisee is unready.
     *
     * @since 1
     */
    public static final String HISEE_UNREADY = "false";

    /**
     * Indicates the rssi query is supported.
     *
     * @since 1
     */
    public static final String RSSI_QUERY_SUPPORTED = "true";

    /**
     * Indicates the rssi query is unsupported.
     *
     * @since 1
     */
    public static final String RSSI_QUERY_UNSUPPORTED = "false";

    /**
     * Sets a preferred foreground application.
     *
     * @since 1
     */
    public static final int SET_PRIORITY = 0x1;

    /**
     * Releases a preferred foreground application.
     *
     * @since 1
     */
    public static final int RELEASE_PRIORITY = 0x0;

    /**
     * Indicates the HCE attribute.
     *
     * <p>This constant is used to check whether HCE card emulation is supported.
     *
     * @since 1
     */
    public static final int FEATURE_HCE = 0;

    /**
     * Indicates the SIM card attribute.
     *
     * <p>This constant is used to check whether UICC card emulation is supported.
     *
     * @since 1
     */
    public static final int FEATURE_UICC = 1;

    /**
     * Indicates the eSE attribute.
     *
     * <p>This constant is used to check whether eSE card emulation is supported.
     *
     * @since 1
     */
    public static final int FEATURE_ESE = 2;

    /**
     * Indicates a payment service.
     *
     * @since 1
     */
    public static final String CATEGORY_PAYMENT = "payment";

    /**
     * Indicates a non-payment service.
     *
     * @since 1
     */
    public static final String CATEGORY_OTHER = "other";

    /**
     * Indicates that the default payment service is used.
     *
     * @since 1
     */
    public static final int SELECTION_TYPE_PREFER_DEFAULT = 0;

    /**
     * Indicates that the user selects a service from all available services.
     *
     * @since 1
     */
    public static final int SELECTION_TYPE_ASK_IF_CONFLICT = 1;

    private static final HiLogLabel LABEL = new HiLogLabel(HiLog.LOG_CORE, NfcKitsUtils.NFC_DOMAIN_ID,
        "CardEmulation");

    private static final Pattern AID_PATTERN = Pattern.compile("[0-9A-Fa-f]{10,32}\\*?\\#?");

    private static CardEmulation sCardEmulation;
    private static Context sContext;
    private CardEmulationProxy mCardEmulationProxy;
    private NfcController mNfcController;

    private CardEmulation(NfcController controller) {
        mNfcController = controller;
        if (mNfcController == null) {
            HiLog.warn(LABEL, "mNfcController is null.");
        }
        mCardEmulationProxy = CardEmulationProxy.getInstance();
    }

    /**
     * Obtains a singleton instance of the {@code CardEmulation} class.
     *
     * <p>If no singleton instance is available, a singleton instance will be created.
     *
     * @param controller Indicates the singleton instance of the {@code NfcController} class.
     * @return Returns a singleton {@code CardEmulation} instance.
     * @since 1
     */
    public static synchronized CardEmulation getInstance(NfcController controller) {
        if (sCardEmulation == null) {
            if (controller != null) {
                sCardEmulation = new CardEmulation(controller);
                sContext = controller.getContext();
            }
        }
        return sCardEmulation;
    }

    /**
     * Checks whether a specified type of card emulation is supported.
     *
     * <p>This method is used to check Whether the host or secure element supports card emulation.
     *
     * @param feature Indicates the card emulation type, {@code HCE}, {@code SIM}, or {@code ESE}.
     * @return Returns {@code true} if the specified type of card emulation is supported; returns
     * {@code false} otherwise.
     * @since 1
     */
    public boolean isSupported(int feature) {
        try {
            if (feature == FEATURE_ESE || feature == FEATURE_HCE || feature == FEATURE_UICC) {
                return mCardEmulationProxy.isSupported(feature);
            }
        } catch (RemoteException remoteException) {
            HiLog.error(LABEL, "isSupported RemoteException");
        }
        return false;
    }

    /**
     * Sets the card emulation mode.
     *
     * @param mode Indicates the card emulation mode to be set. {@link #DISABLE_MODE_A_B}: Indicates
     * that card emulation is disabled. {@link #ENABLE_MODE_ALL}: Indicates that card emulation is enabled.
     * @since 1
     */
    public void setListenMode(int mode) {
        try {
            mCardEmulationProxy.setListenMode(mode);
        } catch (RemoteException remoteException) {
            HiLog.error(LABEL, "setListenMode RemoteException");
        }
    }

    /**
     * Checks whether card emulation is enabled.
     *
     * @return Returns {@code true} if card emulation is enabled; returns {@code false} otherwise.
     * @since 1
     */
    public boolean isListenModeEnabled() {
        try {
            return mCardEmulationProxy.isListenModeEnabled();
        } catch (RemoteException remoteException) {
            HiLog.error(LABEL, "isListenModeEnabled RemoteException");
        }
        return false;
    }

    /**
     * Obtains the method of selecting the service based on the NFC service type.
     *
     * @param category Indicates the NFC service type. {@link #CATEGORY_PAYMENT} indicates a payment
     * service, and {@link #CATEGORY_OTHER} indicates a non-payment service.
     * @return Returns {@link #SELECTION_TYPE_PREFER_DEFAULT} if the default payment service is used;
     * returns {@link #SELECTION_TYPE_ASK_IF_CONFLICT} if the user selects a service from all available
     * services.
     * @since 1
     */
    public int getSelectionType(String category) {
        if (CATEGORY_PAYMENT.equals(category)) {
            return SELECTION_TYPE_PREFER_DEFAULT;
        } else {
            return SELECTION_TYPE_ASK_IF_CONFLICT;
        }
    }

    /**
     * Obtains the NFC information.
     *
     * <p>This method is used to obtain the NFCC or enabled SE information.
     *
     * @param key Indicates the keyword of the required information. See {@link #KEY_ENABLED_SE_TYPE},
     * {@link #KEY_HISEE_READY}, {@link #KEY_RSSI_SUPPORTED}.
     * @return Returns the NFCC or enabled SE information. See {@link #ENABLED_SE_TYPE_ESE},
     * {@link #ENABLED_SE_TYPE_HISEE}, {@link #HISEE_READY}, {@link #HISEE_UNREADY},
     * {@link #RSSI_QUERY_SUPPORTED}, and {@link #RSSI_QUERY_UNSUPPORTED}.
     * @since 1
     */
    public String getNfcInfo(String key) {
        try {
            if (KEY_ENABLED_SE_TYPE.equals(key)
                || KEY_HISEE_READY.equals(key)
                || KEY_RSSI_SUPPORTED.equals(key)) {
                return mCardEmulationProxy.getNfcInfo(key);
            }
        } catch (RemoteException remoteException) {
            HiLog.error(LABEL, "getNfcInfo RemoteException");
        }
        return "";
    }

    /**
     * Customizes RF parameters of card emulation.
     *
     * @param configs Indicates the RF parameters to be customized.
     * @return Returns {@code 0} if the RF parameters are customized successfully; returns other
     * error codes otherwise.
     * @hide
     * @since 1
     */
    public int setRfConfig(String configs) {
        try {
            String caller = "";
            if (sContext != null && sContext.getAbilityInfo() != null) {
                caller = sContext.getAbilityInfo().getBundleName();
            }
            return mCardEmulationProxy.setRfConfig(configs, caller);
        } catch (RemoteException remoteException) {
            HiLog.error(LABEL, "setRfConfig RemoteException");
        }
        return -1;
    }

    /**
     * Registers a preferred foreground application.
     *
     * <p>Before using host-based card emulation (HCE) for your application in the foreground, you can call this method
     * to register your application as a preferred application, so that it will be preferentially used when swiping
     * card.
     *
     * @param appAbility Indicates the ability object of the preferred foreground application.
     * @param appName Indicates the name of the preferred foreground application.
     * @return Returns {@code true} if the registration is successful; returns {@code false} otherwise.
     * @since 3
     */
    public boolean registerForegroundPreferred(Ability appAbility, ElementName appName) {
        if (appAbility == null || appName == null) {
            throw new NullPointerException("appAbility or appName is null");
        }

        try {
            return mCardEmulationProxy.registerForegroundPreferred(appName);
        } catch (RemoteException remoteException) {
            HiLog.error(LABEL, "registerForegroundPreferred RemoteException");
        }
        return false;
    }

    /**
     * Unregisters a preferred foreground application.
     *
     * <p>Before exiting an HCE application from the foreground, you can call this method to unregister it as the
     * preferred foreground application.
     *
     * @param appAbility Indicates the ability object of the preferred foreground application.
     * @return Returns {@code true} if the unregistration is successful; returns {@code false} otherwise.
     * @since 3
     */
    public boolean unregisterForegroundPreferred(Ability appAbility) {
        if (appAbility == null) {
            throw new NullPointerException("appAbility is null");
        }

        try {
            return mCardEmulationProxy.unregisterForegroundPreferred();
        } catch (RemoteException remoteException) {
            HiLog.error(LABEL, "unregisterForegroundPreferred RemoteException");
        }
        return false;
    }

    /**
     * Checks whether the specified application is currently the default handler for the specified AID.
     *
     * @param appName Indicates the name of the application to check.
     * @param aid Indicates the specified AID in a string.
     * @return Returns {@code true} if the specified application is the default handler for the specified AID;
     * returns {@code false} otherwise.
     * @since 3
     */
    public boolean isDefaultForAid(ElementName appName, String aid) {
        try {
            int userId = 0;
            return mCardEmulationProxy.isDefaultForAid(userId, appName, aid);
        } catch (RemoteException remoteException) {
            HiLog.error(LABEL, "isDefaultForAid RemoteException");
        }
        return false;
    }

    /**
     * Registers a list of AIDs of the specified type for a given application.
     *
     * <p>If a list of AIDs of the specified type has been registered for the application, calling this method will
     * replace that list with a new one.
     *
     * @param appName Indicates the name of the application for which the list of AIDs is to be registered.
     * @param type Indicates the AID type. The value can be {@link #CATEGORY_PAYMENT} or {@link #CATEGORY_OTHER}.
     * @param aids Indicates the list of AIDs to register.
     * @return Returns {@code true} if the list of AIDs is registered successfully; returns {@code false} otherwise.
     * @since 3
     */
    public boolean registerAids(ElementName appName, String type, List<String> aids) {
        // get aidGroup
        AidGroup aidGroup = new AidGroup(aids, type);

        try {
            int userId = 0;
            return mCardEmulationProxy.registerAids(userId, appName, aidGroup);
        } catch (RemoteException e) {
            HiLog.error(LABEL, "registerAids RemoteException");
        }
        return false;
    }

    /**
     * Removes a previously registered list of AIDs of the specified type from a given application.
     *
     * <p>This method only removes the AIDs that are dynamically registered using
     * {@link #registerAids(ElementName, String, List)}, and does not remove the AIDs that are statically registered.
     * If dynamically registered AIDs of a specified type are removed but the statically registered ones are available
     * in the application, the static AID group will become active.
     *
     * @param appName Indicates the name of the application from which the list of AIDs is to be removed.
     * @param type Indicates the AID type. The value can be {@link #CATEGORY_PAYMENT} or {@link #CATEGORY_OTHER}.
     * @return Returns {@code true} if the list of AIDs is removed successfully; returns {@code false} otherwise.
     * @since 3
     */
    public boolean removeAids(ElementName appName, String type) {
        try {
            int userId = 0;
            return mCardEmulationProxy.removeAids(userId, appName, type);
        } catch (RemoteException e) {
            HiLog.error(LABEL, "removeAids RemoteException");
        }
        return false;
    }

    /**
     * Obtains a list of AIDs of the specified type in a given application.
     *
     * <p>This method only obtains AIDs that are dynamically registered using
     * {@link #registerAids(ElementName, String, List)}, and cannot obtain statically registered AIDs.
     *
     * @param appName Indicates the name of the application from which the list of AIDs is to be obtained.
     * @param type Indicates the AID type. The value can be {@link #CATEGORY_PAYMENT} or {@link #CATEGORY_OTHER}.
     * @return Returns the list of AIDs of the specified type in the specified application.
     * @since 3
     */
    public List<String> getAids(ElementName appName, String type) {
        try {
            int userId = 0;
            AidGroup aidGroup = mCardEmulationProxy.getAids(userId, appName, type);
            return (aidGroup != null ? aidGroup.getAids() : new ArrayList<String>());
        } catch (RemoteException e) {
            HiLog.error(LABEL, "removeAids RemoteException");
        }
        return new ArrayList<String>();
    }

    /**
     * Queries if the aid is valid according to ISO/IEC 7816-4.
     *
     * @hide
     */
    public static boolean isAidValid(String aid) {
        if (aid == null) {
            return false;
        }

        if ((aid.endsWith("*") || aid.endsWith("#")) && ((aid.length() % 2) == 0)) {
            HiLog.error(LABEL, "aid %{public}s is not valid.", aid);
            return false;
        }

        if ((!(aid.endsWith("*") || aid.endsWith("#"))) && ((aid.length() % 2) != 0)) {
            HiLog.error(LABEL, "aid %{public}s is not valid.", aid);
            return false;
        }

        if (!AID_PATTERN.matcher(aid).matches()) {
            HiLog.error(LABEL, "aid %{public}s is not valid.", aid);
            return false;
        }

        return true;
    }

    /**
     * Checks whether the specified application is currently the default handler for the specified type.
     *
     * @param appName Indicates the name of the application to check.
     * @param type Indicates the specified type in a string, including 'payment' and 'other'.
     * @return Returns {@code true} if the specified application is the default handler for the specified type;
     * returns {@code false} otherwise.
     * @since 4
     */
    public boolean isDefaultForType(ElementName appName, String type) {
        try {
            int userId = 0;
            return mCardEmulationProxy.isDefaultForType(userId, appName, type);
        } catch (RemoteException remoteException) {
            HiLog.error(LABEL, "isDefaultForType RemoteException");
        }
        return false;
    }

}
