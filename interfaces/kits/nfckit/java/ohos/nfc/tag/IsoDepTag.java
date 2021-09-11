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
 * Provides interfaces to obtain {@code IsoDepTag} objects and tag information.
 *
 * <p>This class is inherited from the {@link TagManager} abstract class and provides the methods to
 * create {@code IsoDepTag} objects and obtain the higher layer response and historical bytes.
 *
 * @Syscap {@link SystemCapability.Communication#NFC}
 * @since 1
 */
@SystemCap("SystemCapability.Communication.NFC")
public class IsoDepTag extends TagManager {
    /**
     * Indicates the keyword used to obtain the higher layer response of an ISO-DEP tag that is based
     * on the NFC-B technology.
     *
     * @since 1
     */
    public static final String EXTRA_HILAYER_RESP = "hiresp";

    /**
     * Indicates the keyword used to obtain the historical bytes of an ISO-DEP tag that is based on
     * the NFC-A technology.
     *
     * @since 1
     */
    public static final String EXTRA_HIST_BYTES = "histbytes";

    private byte[] mHiLayerResponse = null;
    private byte[] mHistBytes = null;

    /**
     * Obtains an instance of {@code IsoDepTag} object based on the tag information.
     *
     * <p>During tag reading, if the tag supports the ISO-DEP protocol, an {@code IsoDepTag} object
     * will be created based on the tag information.
     *
     * @param tagInfo Indicates the tag information.
     * @return IsoDepTag object
     * @since 1
     */
    public static Optional<IsoDepTag> getInstance(TagInfo tagInfo) {
        if (!tagInfo.isProfileSupported(TagManager.ISO_DEP)) {
            return Optional.empty();
        }
        return Optional.of(new IsoDepTag(tagInfo));
    }

    /**
     * Obtains an {@code IsoDepTag} object based on the tag information.
     *
     * <p>During tag reading, if the tag supports the ISO-DEP protocol, an {@code IsoDepTag} object
     * will be created based on the tag information.
     *
     * @param tagInfo Indicates the tag information.
     * @since 1
     * @Deprecated .
     */
    public IsoDepTag(TagInfo tagInfo) {
        super(tagInfo, TagManager.ISO_DEP);
        if (tagInfo == null) {
            return;
        }
        if (tagInfo.isProfileSupported(TagManager.ISO_DEP)) {
            Optional<PacMapEx> extrasOptional = tagInfo.getProfileExtras(TagManager.ISO_DEP);
            PacMapEx extras = extrasOptional.orElse(null);
            if (extras != null) {
                if (extras.getObjectValue(EXTRA_HILAYER_RESP).get() instanceof byte[]) {
                    Optional<Object> hilayerRespObject = extras.getObjectValue(EXTRA_HILAYER_RESP);
                    mHiLayerResponse = (byte[]) hilayerRespObject.get();
                }
                if (extras.getObjectValue(EXTRA_HIST_BYTES).get() instanceof byte[]) {
                    Optional<Object> histBytesObject = extras.getObjectValue(EXTRA_HIST_BYTES);
                    mHistBytes = (byte[]) histBytesObject.get();
                }
            }
        }
    }

    /**
     * Obtains the historical bytes of an ISO-DEP tag that is based on the NFC-A technology.
     *
     * @return Returns the historical bytes of the tag; returns an empty array with a length of 0 if
     * the ISO-DEP tag is not based on the NFC-A technology.
     * @since 1
     */
    public byte[] getHistoricalBytes() {
        if (mHistBytes != null) {
            return Arrays.copyOf(mHistBytes, mHistBytes.length);
        }
        return new byte[0];
    }

    /**
     * Obtains the higher layer response of an ISO-DEP tag that is based on the NFC-B technology.
     *
     * @return Returns the higher layer response of the tag; returns an empty array with a length of
     * 0 if the ISO-DEP tag is not based on the NFC-B technology.
     * @since 1
     */
    public byte[] getHiLayerResponse() {
        if (mHiLayerResponse != null) {
            return Arrays.copyOf(mHiLayerResponse, mHiLayerResponse.length);
        }
        return new byte[0];
    }
}