/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
 * Copyright (C) 2006 The Android Open Source Project
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

import ohos.aafwk.content.Intent;
import ohos.aafwk.content.Skills;
import ohos.aafwk.content.IntentParams;
import ohos.event.commonevent.CommonEventBaseConverter;
import ohos.hiviewdfx.HiLog;
import ohos.hiviewdfx.HiLogLabel;

import java.util.Collections;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;
import java.util.Optional;
import java.util.Set;

/**
 * NfcEventsConverter provides common event interface for other application. Make it possible to be
 * notified for different information like NFC status change or NCI message.
 *
 * @since 1
 * @hide
 */
public class NfcEventsConverter extends CommonEventBaseConverter {
    private static final int NFC_DOMAIN_ID = 0xD001570;
    private static final HiLogLabel LABEL = new HiLogLabel(HiLog.LOG_CORE, NFC_DOMAIN_ID,
        "NfcEventsConverter");
    private static final String ADAPTER_STATE_CHANGED = "android.nfc.action.ADAPTER_STATE_CHANGED";
    private static final String RF_FIELD_ON_DETECTED = "com.android.nfc_extras.action.RF_FIELD_ON_DETECTED";
    private static final String RF_FIELD_OFF_DETECTED = "com.android.nfc_extras.action.RF_FIELD_OFF_DETECTED";
    private static final String EXTRA_ADAPTER_STATE = "android.nfc.extra.ADAPTER_STATE";
    private static final String EXTRA_TRANSACTION = "transactionIntent";
    private static final String EXTRA_NFC_STATE = "extra_nfc_state";
    private static final String EXTRA_NFC_TRANSACTION = "extra_nfc_transaction";
    private static final String FIELD_ON_DETECTED = "usual.event.nfc.action.RF_FIELD_ON_DETECTED";
    private static final String FIELD_OFF_DETECTED = "usual.event.nfc.action.RF_FIELD_OFF_DETECTED";
    private static final String STATE_CHANGED = "usual.event.nfc.action.ADAPTER_STATE_CHANGED";

    private static final Map<String, String> MAP_EXTRAS_ADAPTER_STATE =
        Collections.unmodifiableMap(new HashMap<String, String>() {
        {
            put(EXTRA_ADAPTER_STATE, EXTRA_NFC_STATE);
        }
    });

    private static final Map<String, String> MAP_EXTRAS_EXTRA_TRANSACTION_INTENT =
        Collections.unmodifiableMap(new HashMap<String, String>() {
        {
            put(EXTRA_TRANSACTION, EXTRA_NFC_TRANSACTION);
        }
    });

    private Object convertExtraValue(String aAction, String aExtraKey, Object aExtraValue) {
        // no need to change the value for current broadcast intents.
        return aExtraValue;
    }

    private static final Map<String, OhosAction> MAP_ACTION_CONVERTER =
        Collections.unmodifiableMap(new HashMap<String, OhosAction>() {
        {
            put(ADAPTER_STATE_CHANGED,
                    new OhosAction(STATE_CHANGED, MAP_EXTRAS_ADAPTER_STATE));
            put(RF_FIELD_ON_DETECTED,
                    new OhosAction(FIELD_ON_DETECTED, MAP_EXTRAS_EXTRA_TRANSACTION_INTENT));
            put(RF_FIELD_OFF_DETECTED, new OhosAction(FIELD_OFF_DETECTED, null));
        }
    });

    private static final class OhosAction {
        /** The action of OHOS */
        public String action;

        /** The extras content of OHOS */
        public Map<String, String> extrasConverter;

        /**
         * The mapping action and extras of OHOS.
         *
         * @param action Indicates the action of OHOS.
         * @param extras Indicates the extras of mapping of OHOS and AndroidOS.
         */
        public OhosAction(String action, Map<String, String> extras) {
            this.action = action;
            this.extrasConverter = extras;
        }
    }

    // Convert OHOS Intent to AndroidOS Intent
    @Override
    public Optional<android.content.Intent> convertIntentToAospIntent(Intent ohosIntent) {
        android.content.Intent aIntent = new android.content.Intent();

        return Optional.ofNullable(aIntent);
    }

    // Convert AndroidOS Intent to OHOS Intent
    @Override
    public Optional<Intent> convertAospIntentToIntent(android.content.Intent aIntent) {
        HiLog.info(LABEL, "ENTER convertAospIntentToIntent");
        if (aIntent == null) {
            HiLog.warn(LABEL, "convertAospIntentToIntent, aIntent is null");
            return Optional.empty();
        }
        String aAction = aIntent.getAction();
        OhosAction ohosAction = MAP_ACTION_CONVERTER.get(aAction);
        if (ohosAction == null) {
            HiLog.warn(LABEL, "convertAospIntentToIntent, ohosAction is null, aAction = %{public}s", aAction);
            return Optional.empty();
        }

        Intent ohosIntent = new Intent();
        ohosIntent.setAction(ohosAction.action);
        android.os.Bundle aExtras = aIntent.getExtras();
        HiLog.info(LABEL, "aAction: %{public}s, ohosAction: %{public}s", aAction, ohosAction.action);

        if (aExtras != null && aExtras.keySet() != null) {
            Set<String> aExtraSet = aExtras.keySet();
            IntentParams params = new IntentParams();
            for (Iterator<String> it = aExtraSet.iterator(); it.hasNext();) {
                Object obj = it.next();
                String aExtraKey = null;
                if (obj != null && obj instanceof String) {
                    aExtraKey = (String) obj;
                } else {
                    HiLog.warn(LABEL, "aExtraKey is null.");
                    continue;
                }
                if (ohosAction.extrasConverter != null && ohosAction.extrasConverter.get(aExtraKey) != null) {
                    String ohosExtraKey = ohosAction.extrasConverter.get(aExtraKey);
                    Object aExtraValue = aExtras.get(aExtraKey);
                    Object ohosExtraValue = convertExtraValue(aAction, aExtraKey, aExtraValue);
                    params.setParam(ohosExtraKey, ohosExtraValue);
                } else {
                    HiLog.warn(LABEL, "Cannot get ohos key from a key: %{public}s or" +
                        " extrasConverter is null", aExtraKey);
                }
            }
            ohosIntent.setParams(params);
        }

        HiLog.info(LABEL, "convertAospIntentToIntent return ohosIntent");
        return Optional.ofNullable(ohosIntent);
    }

    // Convert OHOS Skills to AndroidOS IntentFilter
    @Override
    public void convertSkillsToAospIntentFilter(Skills ohosSkills,
        android.content.IntentFilter aIntentFilter) {
        HiLog.warn(LABEL, "unhandle convertSkillsToAospIntentFilter");
    }
}
