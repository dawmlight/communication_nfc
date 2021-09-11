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
 * Provides interfaces to control the read and write of tags that support the NFC-A technology.
 *
 * <p>This class is inherited from the {@link TagManager} abstract class, and provides methods to create
 * {@code NfcATag} objects and obtain the ATQA and SAK.
 *
 * @Syscap {@link SystemCapability.Communication#NFC}
 * @since 1
 */
@SystemCap("SystemCapability.Communication.NFC")
public class NfcATag extends TagManager {
    /**
     * Indicates the keyword used to obtain the SAK of an NFC-A tag.
     *
     * @since 1
     */
    public static final String EXTRA_SAK = "sak";

    /**
     * Indicates the keyword used to obtain the ATQA of an NFC-A tag.
     *
     * @since 1
     */
    public static final String EXTRA_ATQA = "atqa";

    private static final int ATQA_LENGTH = 2;

    private short mSak = 0;
    private byte[] mAtqa = new byte[ATQA_LENGTH];

    /**
     * Obtains an {@code NfcATag} object based on the tag information.
     *
     * <p>During tag reading, if the tag supports the NFC-A technology, an {@code NfcATag} object
     * will be created based on the tag information.
     *
     * @param tagInfo Indicates the tag information.
     * @return Returns the {@code NfcATag} object.
     * @since 1
     */
    public static NfcATag getInstance(TagInfo tagInfo) {
        if (tagInfo == null) {
            throw new NullPointerException("NfcATag tagInfo is null");
        }
        if (!tagInfo.isProfileSupported(TagManager.NFC_A)) {
            return null;
        }
        return new NfcATag(tagInfo);
    }

    private NfcATag(TagInfo tagInfo) {
        super(tagInfo, TagManager.NFC_A);
        Optional<PacMapEx> extrasOptional;
        mSak = 0;
        if (tagInfo.isProfileSupported(TagManager.MIFARE_CLASSIC)) {
            extrasOptional = tagInfo.getProfileExtras(TagManager.MIFARE_CLASSIC);
            PacMapEx extras = extrasOptional.orElse(null);
            if (extras != null) {
                Optional<Object> sakObject = extras.getObjectValue(EXTRA_SAK);
                mSak = (short) sakObject.get();
            }
        }
        if (tagInfo.isProfileSupported(TagManager.NFC_A)) {
            extrasOptional = tagInfo.getProfileExtras(TagManager.NFC_A);
            PacMapEx extras = extrasOptional.orElse(null);
            if (extras != null) {
                Optional<Object> sakObject = extras.getObjectValue(EXTRA_SAK);
                mSak |= (short) sakObject.get();
                if (extras.getObjectValue(EXTRA_ATQA).get() instanceof byte[]) {
                    Optional<Object> atqaObject = extras.getObjectValue(EXTRA_ATQA);
                    mAtqa = (byte[]) atqaObject.get();
                }
            }
        }
    }

    /**
     * Obtains the SAK of an NFC-A tag.
     *
     * @return Returns the SAK of the NFC-A tag.
     * @since 1
     */
    public short getSak() {
        return mSak;
    }

    /**
     * Obtains the ATQA of an NFC-A tag.
     *
     * @return Returns the ATQA of the NFC-A tag.
     * @since 1
     */
    public byte[] getAtqa() {
        if (mAtqa != null) {
            return Arrays.copyOf(mAtqa, mAtqa.length);
        }
        return new byte[0];
    }
}
