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

import ohos.interwork.utils.PacMapEx;
import ohos.utils.system.SystemCap;

import java.util.Optional;

/**
 * Provides methods for creating an NFC-F tag, obtaining tag information, and controlling tag read and write.
 *
 * <p>This class inherits from the {@link TagManager} abstract class.
 *
 * @Syscap {@link SystemCapability.Communication#NFC}
 * @since 5
 */
@SystemCap("SystemCapability.Communication.NFC")
public class NfcFTag extends TagManager {
    private static final String EXTRA_SC = "systemcode";
    private static final String EXTRA_PMM = "pmm";

    private byte[] mSystemCode = null;
    private byte[] mPMm = null;

    /**
     * Obtains an {@code NfcFTag} instance for the given tag.
     *
     * <p>You can call this method to obtain an {@code NfcFTag} instance only if the given tag supports NFC-F.
     *
     * @param tagInfo Indicates information about the given tag.
     * @return Returns the {@code NfcFTag} instance.
     * @since 5
     */
    public static Optional<NfcFTag> getInstance(TagInfo tagInfo) {
        if (!tagInfo.isProfileSupported(TagManager.NFC_F)) {
            return Optional.empty();
        }
        return Optional.of(new NfcFTag(tagInfo));
    }

    /**
     * Obtains an {@code NfcFTag} instance for the given tag.
     *
     * <p>You can call this method to obtain an {@code NfcFTag} instance only if the given tag supports NFC-F.
     *
     * @param tagInfo Indicates information about the given tag.
     * @return Returns the {@code NfcFTag} instance.
     * @hide
     * @since 5
     */
    public NfcFTag(TagInfo tagInfo) {
        super(tagInfo, TagManager.NFC_F);
        Optional<PacMapEx> extrasOptional;
        if (tagInfo.isProfileSupported(TagManager.NFC_F)) {
            extrasOptional = tagInfo.getProfileExtras(TagManager.NFC_F);
            PacMapEx extras = extrasOptional.orElse(null);
            if (extras != null) {
                if (extras.getObjectValue(EXTRA_SC).get() instanceof byte[]) {
                    Optional<Object> systemCodeObject = extras.getObjectValue(EXTRA_SC);
                    mSystemCode = (byte[]) systemCodeObject.get();
                }
                if (extras.getObjectValue(EXTRA_PMM).get() instanceof byte[]) {
                    Optional<Object> pMmObject = extras.getObjectValue(EXTRA_PMM);
                    mPMm = (byte[]) pMmObject.get();
                }
            }
        }
    }

    /**
     * Obtains the system code from this {@code NfcFTag} instance.
     *
     * @return Returns the system code.
     * @since 5
     */
    public byte[] getSystemCode() {
        return mSystemCode;
    }

    /**
     * Obtains the PMm (consisting of the IC code and manufacturer parameters) from this {@code NfcFTag} instance.
     *
     * @return Returns the PMm.
     * @since 5
     */
    public byte[] getPMm() {
        return mPMm;
    }
}
