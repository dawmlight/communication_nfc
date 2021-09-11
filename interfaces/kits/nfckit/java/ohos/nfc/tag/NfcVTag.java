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
 * Provides methods for creating an NFC-V tag, obtaining tag information, and controlling tag read and write.
 *
 * <p>This class inherits from the {@link TagManager} abstract class.
 *
 * @Syscap {@link SystemCapability.Communication#NFC}
 * @since 5
 */
@SystemCap("SystemCapability.Communication.NFC")
public class NfcVTag extends TagManager {
    private static final String EXTRA_RESP_FLAGS = "respflags";
    private static final String EXTRA_DSFID = "dsfid";

    private byte mResponseFlags;
    private byte mDsfId;

    /**
     * Obtains an {@code NfcVTag} instance for the given tag.
     *
     * <p>You can call this method to obtain an {@code NfcVTag} instance only if the given tag supports NFC-V.
     *
     * @param tagInfo Indicates information about the given tag.
     * @return Returns the {@code NfcVTag} instance.
     * @since 5
     */
    public static Optional<NfcVTag> getInstance(TagInfo tagInfo) {
        if (!tagInfo.isProfileSupported(TagManager.NFC_F)) {
            return Optional.empty();
        }
        return Optional.of(new NfcVTag(tagInfo));
    }

    /**
     * Obtains an {@code NfcVTag} instance for the given tag.
     *
     * <p>You can call this method to obtain an {@code NfcVTag} instance only if the given tag supports NFC-V.
     *
     * @param tagInfo Indicates information about the given tag.
     * @return Returns the {@code NfcVTag} instance.
     * @hide
     * @since 5
     */
    public NfcVTag(TagInfo tagInfo) {
        super(tagInfo, TagManager.NFC_V);
        Optional<PacMapEx> extrasOptional = tagInfo.getProfileExtras(TagManager.NFC_V);
        PacMapEx extras = extrasOptional.orElse(null);
        if (extras != null) {
            if (extras.getObjectValue(EXTRA_RESP_FLAGS).get() instanceof Byte) {
                Optional<Object> responseFlagsObject = extras.getObjectValue(EXTRA_RESP_FLAGS);
                mResponseFlags = (byte) responseFlagsObject.get();
            }
            if (extras.getObjectValue(EXTRA_DSFID).get() instanceof Byte) {
                Optional<Object> dsfIdObject = extras.getObjectValue(EXTRA_DSFID);
                mDsfId = (byte) dsfIdObject.get();
            }
        }
    }

    /**
     * Obtains the response flags from this {@code NfcVTag} instance.
     *
     * @return Returns the response flags.
     * @since 5
     */
    public byte getResponseFlags() {
        return mResponseFlags;
    }

    /**
     * Obtains the data storage format identifier (DSFID) from this {@code NfcVTag} instance.
     *
     * @return Returns the DSFID.
     * @since 5
     */
    public byte getDsfId() {
        return mDsfId;
    }
}
