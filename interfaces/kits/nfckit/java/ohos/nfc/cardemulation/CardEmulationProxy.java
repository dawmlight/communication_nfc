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
import ohos.hiviewdfx.HiLog;
import ohos.hiviewdfx.HiLogLabel;
import ohos.nfc.NfcAdapterUtils;
import ohos.nfc.NfcCommProxy;
import ohos.nfc.NfcKitsUtils;
import ohos.rpc.IRemoteObject;
import ohos.rpc.IRemoteObject.DeathRecipient;
import ohos.rpc.MessageParcel;
import ohos.rpc.RemoteException;
import ohos.sysability.samgr.SysAbilityManager;
import ohos.utils.Parcel;
import ohos.utils.Sequenceable;
import ohos.utils.system.safwk.java.SystemAbilityDefinition;

/**
 * Implements IPC for the card emulation service.
 *
 * @hide
 */
class CardEmulationProxy extends NfcCommProxy implements ICardEmulation {
    private static final HiLogLabel LABEL = new HiLogLabel(HiLog.LOG_CORE, NfcKitsUtils.NFC_DOMAIN_ID,
        "CardEmulationProxy");

    private static CardEmulationProxy sCardEmulationProxy;

    private CardEmulationProxy() {
        super(SystemAbilityDefinition.NFC_MANAGER_SYS_ABILITY_ID);
    }

    /**
     * Gets a {@code CardEmulationProxy} instance.
     *
     * @return Returns an instance of {@code CardEmulationProxy}.
     */
    public static synchronized CardEmulationProxy getInstance() {
        if (sCardEmulationProxy == null) {
            sCardEmulationProxy = new CardEmulationProxy();
        }
        return sCardEmulationProxy;
    }

    @Override
    public void setListenMode(int mode) throws RemoteException {
        MessageParcel data = MessageParcel.obtain();

        NfcKitsUtils.writeInterfaceToken(NfcKitsUtils.NXP_ADAPTER_DESCRIPTOR, data);
        data.writeInt(mode);
        data.writeInt(1); // override
        MessageParcel reply = request(NfcKitsUtils.SET_LISTEN_MODE, data);
        reply.reclaim();
    }

    @Override
    public boolean isListenModeEnabled() throws RemoteException {
        MessageParcel data = MessageParcel.obtain();

        NfcKitsUtils.writeInterfaceToken(NfcKitsUtils.NXP_ADAPTER_DESCRIPTOR, data);
        MessageParcel reply = request(NfcKitsUtils.IS_LISTEN_MODE_ENABLED, data);
        boolean result = reply.readBoolean();
        reply.reclaim();
        return result;
    }

    @Override
    public String getNfcInfo(String key) throws RemoteException {
        if (key == null || key.length() == 0) {
            HiLog.warn(LABEL, "method getNfcInfo input param is null or empty");
            return "";
        }

        MessageParcel data = MessageParcel.obtain();

        NfcKitsUtils.writeInterfaceToken(NfcKitsUtils.NXP_ADAPTER_DESCRIPTOR, data);
        data.writeString(key);
        MessageParcel reply = request(NfcKitsUtils.GET_NFC_INFO, data);
        String result = reply.readString();
        reply.reclaim();
        return result;
    }

    @Override
    public int setRfConfig(String configs, String pkg) throws RemoteException {
        MessageParcel data = MessageParcel.obtain();

        NfcKitsUtils.writeInterfaceToken(NfcKitsUtils.NXP_ADAPTER_DESCRIPTOR, data);
        data.writeString(configs);
        data.writeString(pkg);
        MessageParcel reply = request(NfcKitsUtils.SET_RF_CONFIG, data);
        int result = reply.readInt();
        reply.reclaim();
        return result;
    }

    @Override
    public boolean isSupported(int feature) throws RemoteException {
        MessageParcel data = MessageParcel.obtain();

        NfcKitsUtils.writeInterfaceToken(NfcKitsUtils.NXP_ADAPTER_DESCRIPTOR, data);
        data.writeInt(feature);
        MessageParcel reply = request(NfcKitsUtils.GET_INFO_FROM_CONFIG, data);
        boolean result = reply.readInt() >= 0;
        reply.reclaim();
        return result;
    }

    @Override
    public boolean registerForegroundPreferred(ElementName appName) throws RemoteException {
        MessageParcel data = MessageParcel.obtain();

        NfcKitsUtils.writeInterfaceToken(NfcAdapterUtils.stringReplace(NfcKitsUtils.CARD_EMULATION_DESCRIPTOR), data);
        data.writeSequenceable(appName);
        MessageParcel reply = request(NfcKitsUtils.SET_FOREGROUND_SERVICE, data);
        boolean result = reply.readInt() >= 0;
        reply.reclaim();
        return result;
    }

    @Override
    public boolean unregisterForegroundPreferred() throws RemoteException {
        MessageParcel data = MessageParcel.obtain();

        NfcKitsUtils.writeInterfaceToken(NfcAdapterUtils.stringReplace(NfcKitsUtils.CARD_EMULATION_DESCRIPTOR), data);
        MessageParcel reply = request(NfcKitsUtils.UNSET_FOREGROUND_SERVICE, data);
        boolean result = reply.readInt() >= 0;
        reply.reclaim();
        return result;
    }

    @Override
    public boolean isDefaultForAid(int userId, ElementName appName, String aid) throws RemoteException {
        MessageParcel data = MessageParcel.obtain();

        NfcKitsUtils.writeInterfaceToken(NfcAdapterUtils.stringReplace(NfcKitsUtils.CARD_EMULATION_DESCRIPTOR), data);
        data.writeInt(userId);
        data.writeSequenceable(appName);
        data.writeString(aid);
        MessageParcel reply = request(NfcKitsUtils.IS_DEFAULT_FOR_AID, data);
        boolean result = reply.readInt() >= 0;
        reply.reclaim();
        return result;
    }

    @Override
    public boolean registerAids(int userId, ElementName appName, AidGroup aidGroup) throws RemoteException {
        MessageParcel data = MessageParcel.obtain();

        NfcKitsUtils.writeInterfaceToken(NfcAdapterUtils.stringReplace(NfcKitsUtils.ADAPTER_DESCRIPTOR), data);
        data.writeInt(userId);
        data.writeString(aidGroup.getType());
        data.writeStringList(aidGroup.getAids());
        data.writeSequenceable(appName);
        MessageParcel reply = request(NfcKitsUtils.REGISTER_AIDS, data);
        boolean result = reply.readInt() >= 0;
        reply.reclaim();
        return result;
    }

    @Override
    public boolean removeAids(int userId, ElementName appName, String type) throws RemoteException {
        MessageParcel data = MessageParcel.obtain();

        NfcKitsUtils.writeInterfaceToken(NfcAdapterUtils.stringReplace(NfcKitsUtils.CARD_EMULATION_DESCRIPTOR), data);
        data.writeInt(userId);
        data.writeSequenceable(appName);
        data.writeString(type);
        MessageParcel reply = request(NfcKitsUtils.REMOVE_AIDS, data);
        boolean result = reply.readInt() >= 0;
        reply.reclaim();
        return result;
    }

    @Override
    public AidGroup getAids(int userId, ElementName appName, String type) throws RemoteException {
        MessageParcel data = MessageParcel.obtain();

        NfcKitsUtils.writeInterfaceToken(NfcAdapterUtils.stringReplace(NfcKitsUtils.CARD_EMULATION_DESCRIPTOR), data);
        data.writeInt(userId);
        data.writeSequenceable(appName);
        data.writeString(type);
        MessageParcel reply = request(NfcKitsUtils.GET_AIDS, data);
        AidGroup result = new AidGroup();
        if (result instanceof Sequenceable) {
            reply.readSequenceable((Sequenceable) result);
        }
        reply.reclaim();
        return result;
    }

    @Override
    public boolean isDefaultForType(int userId, ElementName appName, String type) throws RemoteException {
        MessageParcel data = MessageParcel.obtain();

        NfcKitsUtils.writeInterfaceToken(NfcAdapterUtils.stringReplace(NfcKitsUtils.ADAPTER_DESCRIPTOR), data);
        data.writeInt(userId);
        data.writeString(type);
        data.writeSequenceable(appName);
        MessageParcel reply = request(NfcKitsUtils.IS_DEFAULT_FOR_TYPE, data);
        boolean result = reply.readInt() >= 0;
        reply.reclaim();
        return result;
    }
}
