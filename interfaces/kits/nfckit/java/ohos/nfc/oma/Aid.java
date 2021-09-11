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

package ohos.nfc.oma;

import ohos.utils.system.SystemCap;

import java.util.Arrays;
import java.util.Locale;

/**
 * Provides interfaces to create {@code Aid} objects and obtain {@code Aid} information.
 *
 * <p>The interfaces can be used to create {@code Aid} objects, check validity of {@code Aid} objects, and define
 * common {@code Aid} values. The {@code Aid} object is used to enable a basic or logical channel in {@link Session}.
 *
 * @Syscap {@link SystemCapability.Communication#NFC}
 * @since 1
 */
@SystemCap("SystemCapability.Communication.NFC")
public class Aid {
    /**
     * Indicates the PSE value.
     *
     * @since 1
     */
    public static final String AID_PSE = "315041592e5359532e4444463031";

    /**
     * Indicates the PPSE value.
     *
     * @since 1
     */
    public static final String AID_PPSE = "325041592e5359532e4444463031";

    /**
     * Indicates the CRS value.
     *
     * @since 1
     */
    public static final String AID_CRS = "a00000015143525300";

    private static final int MIN_AID_BYTES = 5;
    private static final int MAX_AID_BYTES = 16;
    private byte[] mAidValue = null;

    /**
     * A constructor used to create an {@code Aid} instance.
     *
     * @param aid Indicates the {@code Aid} byte array.
     * @param offset Indicates the initial offset of the {@code Aid} byte array.
     * @param length Indicates the byte length of the {@code Aid}.
     * @since 1
     */
    public Aid(byte[] aid, int offset, int length) {
        if (aid != null && (length + offset) <= aid.length) {
            mAidValue = Arrays.copyOfRange(aid, offset, length + offset);
        }
    }

    /**
     * Checks whether an {@code Aid} is valid.
     *
     * @return Returns {@code true} if the {@code Aid} is valid; returns {@code false} otherwise.
     * @since 1
     */
    public boolean isAidValid() {
        return mAidValue != null && mAidValue.length >= MIN_AID_BYTES && mAidValue.length <= MAX_AID_BYTES;
    }

    /**
     * Obtains the value of {@code Aid} in byte array format.
     *
     * @return Returns the value of {@code Aid} in byte array format.
     * @since 1
     */
    public byte[] getAidBytes() {
        if (mAidValue != null) {
            return Arrays.copyOf(mAidValue, mAidValue.length);
        }
        return new byte[0];
    }

    /**
     * Obtains the bytes array of a string.
     *
     * @param str Indicates the hex string.
     * @return Returns the AID byte array as a hex string.
     * @hide
     */
    public static byte[] hexStringToBytes(String str) {
        if (str == null || str.length() == 0) {
            return new byte[0];
        }
        String tmp = str;
        int len = tmp.length();
        if (len % 2 != 0) {
            tmp = '0' + tmp;
            len++;
        }
        byte[] data = new byte[len / 2];
        for (int i = 0; i < len; i += 2) {
            data[i / 2] = (byte) ((Character.digit(tmp.charAt(i), 16) << 4)
                + Character.digit(tmp.charAt(i + 1), 16));
        }
        return data;
    }

    /**
     * Obtains the string format of AID byte arrays.
     *
     * @param byteArray Indicates the AID byte array.
     * @return Returns a hex-encoded string.
     * @hide
     */
    public static String byteArrayToHexString(byte[] byteArray) {
        if (byteArray == null) {
            return "";
        }
        StringBuilder stringBuilder = new StringBuilder();
        for (int i = 0; i < byteArray.length; i++) {
            stringBuilder.append(String.format(Locale.ENGLISH, "%02x", byteArray[i] & 0xFF));
        }
        return stringBuilder.toString();
    }
}
