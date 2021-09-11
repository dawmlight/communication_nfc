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
import ohos.interwork.utils.PacMapEx;
import ohos.nfc.NfcKitsUtils;
import ohos.utils.system.SystemCap;

import java.util.Arrays;
import java.util.Optional;

/**
 * Provides interfaces to create an {@code NfcBTag} and perform I/O operations on the tag.
 *
 * <p>This class inherits from the {@link TagManager} abstract class and provides interfaces to create an
 * {@code NfcBTag} and obtain the tag information.
 *
 * @Syscap {@link SystemCapability.Communication#NFC}
 * @since 3
 */
@SystemCap("SystemCapability.Communication.NFC")
public class NfcBTag extends TagManager {
    /**
     * @hide
     */
    public static final String EXTRA_RESPAPPDATA = "appdata";

    /**
     * @hide
     */
    public static final String EXTRA_RESPPROTOCOL = "protinfo";

    private byte[] mRespAppData;

    private byte[] mRespProtocol;

    /**
     * Obtains an {@code NfcBTag} object based on the given tag.
     *
     * <p>During tag reading, if the tag supports the NFC-B technology, an {@code NfcBTag} object will be created based
     * on the tag information.
     *
     * @param tagInfo Indicates the information about the given tag.
     * @return Returns the {@code NfcBTag} object.
     * @since 3
     */
    public static NfcBTag getInstance(TagInfo tagInfo) {
        if (tagInfo == null) {
            throw new NullPointerException("NfcBTag tagInfo is null");
        }
        if (!tagInfo.isProfileSupported(TagManager.NFC_B)) {
            return null;
        }
        return new NfcBTag(tagInfo);
    }

    private NfcBTag(TagInfo tagInfo) {
        super(tagInfo, TagManager.NFC_B);
        Optional<PacMapEx> extrasOptional;
        if (tagInfo.isProfileSupported(TagManager.NFC_B)) {
            extrasOptional = tagInfo.getProfileExtras(TagManager.NFC_B);
            PacMapEx extras = extrasOptional.orElse(null);
            if (extras != null) {
                if (extras.getObjectValue(EXTRA_RESPAPPDATA).get() instanceof byte[]) {
                    Optional<Object> respAppDataObject = extras.getObjectValue(EXTRA_RESPAPPDATA);
                    mRespAppData = (byte[]) respAppDataObject.get();
                }
                if (extras.getObjectValue(EXTRA_RESPPROTOCOL).get() instanceof byte[]) {
                    Optional<Object> respProtocolObject = extras.getObjectValue(EXTRA_RESPPROTOCOL);
                    mRespProtocol = (byte[]) respProtocolObject.get();
                }
            }
        }
    }

    /**
     * Obtains the application data of a tag.
     *
     * @return Returns the application data of the tag.
     * @since 3
     */
    public byte[] getRespAppData() {
        return mRespAppData;
    }

    /**
     * Obtains the protocol information of a tag.
     *
     * @return Returns the protocol information of the tag.
     * @since 3
     */
    public byte[] getRespProtocol() {
        return mRespProtocol;
    }
}
