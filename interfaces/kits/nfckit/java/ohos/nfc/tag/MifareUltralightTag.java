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

import java.io.IOException;
import java.util.Arrays;
import java.util.Optional;

/**
 * Provides interfaces to create a MIFARE Ultralight tag and perform I/O operations on the tag.
 *
 * <p>This class inherits from the {@link TagManager} abstract class and provides interfaces to create a MIFARE
 * Ultralight tag, obtain the tag type, and read and write pages of the tag.
 *
 * @Syscap {@link SystemCapability.Communication#NFC}
 * @since 3
 */
@SystemCap("SystemCapability.Communication.NFC")
public class MifareUltralightTag extends TagManager {
    /**
     * A MIFARE Ultralight tag of an unknown type.
     *
     * @since 3
     */
    public static final int MIFARE_UNKNOWN = -1;

    /**
     * A MIFARE Ultralight tag.
     *
     * @since 3
     */
    public static final int MIFARE_ULTRALIGHT = 1;

    /**
     * A MIFARE Ultralight C tag.
     *
     * @since 3
     */
    public static final int MIFARE_ULTRALIGHT_C = 2;

    /**
     * @hide
     */
    public static final String EXTRA_IS_ULC = "isulc";

    private static final int NFC_MANUFACTURER_ID = 0x04;
    private static final int SAK_00 = 0x00;
    private static final int MAX_PAGE_COUNT = 256;

    private int mMifareType;

    /**
     * Obtains a {@code MifareUltralightTag} object based on the given tag information.
     *
     * <p>During tag reading, if the tag supports the NFC-A technology, a {@code MifareUltralightTag} object will be
     * created based on the given tag information.
     *
     * @param tagInfo Indicates the information about the given tag.
     * @return Returns the {@code MifareUltralightTag} object.
     * @since 3
     */
    public static MifareUltralightTag getInstance(TagInfo tagInfo) {
        if (tagInfo == null) {
            throw new NullPointerException("MifareUltralightTag tagInfo is null");
        }
        if (!tagInfo.isProfileSupported(TagManager.NFC_A)) {
            return null;
        }
        return new MifareUltralightTag(tagInfo);
    }

    private MifareUltralightTag(TagInfo tagInfo) {
        super(tagInfo, TagManager.MIFARE_ULTRALIGHT);
        // Mifare uses NfcA
        NfcATag nfcATag = NfcATag.getInstance(tagInfo);
        mMifareType = MIFARE_UNKNOWN;

        int sak = -1;
        if (nfcATag != null) {
            sak = nfcATag.getSak();
        }
        if (sak == SAK_00 && tagInfo.getTagId()[0] == NFC_MANUFACTURER_ID) {
            Optional<PacMapEx> extrasOptional = tagInfo.getProfileExtras(TagManager.MIFARE_ULTRALIGHT);
            PacMapEx extras = extrasOptional.orElse(null);
            if (extras != null) {
                if (extras.getObjectValue(EXTRA_IS_ULC).get() instanceof Boolean) {
                    Optional<Object> extraIsUlcObject = extras.getObjectValue(EXTRA_IS_ULC);
                    if ((boolean) extraIsUlcObject.get()) {
                        mMifareType = MIFARE_ULTRALIGHT_C;
                    } else {
                        mMifareType = MIFARE_ULTRALIGHT;
                    }
                }
            }
        }
    }

    /**
     * Obtains the type of the MIFARE Ultralight tag. The type can be {@link #MIFARE_ULTRALIGHT},
     * {@link #MIFARE_ULTRALIGHT_C}, or {@link #MIFARE_UNKNOWN}.
     *
     * @return Returns the type of the MIFARE Ultralight tag.
     * @since 3
     */
    public int getMifareType() {
        return mMifareType;
    }

    /**
     * Reads four pages (16 bytes) starting from the specified page offset.
     *
     * @param pageOffset Indicates the specified page offset, starting from 0.
     * @return Returns the four pages (16 bytes) that are read.
     * @throws IOException if there is an I/O failure, or the operation is canceled.
     * @since 3
     */
    public byte[] readFourPages(int pageOffset) throws IOException {
        checkPageIdx(pageOffset);
        checkConnected();

        byte[] cmd = {(byte)0x30, (byte) pageOffset}; // cmd to read pages
        return sendData(cmd);
    }

    /**
     * Writes data to the specified page.
     *
     * @param pageOffset Indicates the offset of the page, starting from 0.
     * @param data Indicates the data to write to the page.
     * @throws IOException if there is an I/O failure, or the operation is canceled.
     * @since 3
     */
    public void writeOnePage(int pageOffset, byte[] data) throws IOException {
        checkPageIdx(pageOffset);
        checkConnected();

        byte[] cmd = new byte[data.length + 2];
        cmd[0] = (byte) 0xA2; // cmd to write page.
        cmd[1] = (byte) pageOffset;
        System.arraycopy(data, 0, cmd, 2, data.length);

        sendData(cmd);
    }

    private static void checkPageIdx(int pageIdx) {
        if (pageIdx < 0 || pageIdx >= MAX_PAGE_COUNT) {
            throw new IllegalArgumentException("Invalid page index: " + pageIdx);
        }
    }
}
