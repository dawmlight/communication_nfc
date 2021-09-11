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

import ohos.utils.Parcel;
import ohos.utils.Sequenceable;

import java.util.Arrays;

/**
 * Response information of sending command data to tag.
 *
 * @hide
 */
public class ResponseInfo implements Sequenceable {
    private static final int RESULT_SUCCESS = 0;
    private static final int RESULT_FAILURE = 1;
    private static final int RESULT_TAGLOST = 2;
    private static final int RESULT_EXCEEDED_LENGTH = 3;

    private int mResult;
    private int mResponseLength;
    private byte[] mResponseData;

    /**
     * A constructor used to create a {@code ResponseInfo} instance.
     *
     * @hide
     */
    public ResponseInfo() {
    }

    /**
     * Gets the response from tag.
     *
     * @return Returns the response from tag.
     * @hide
     */
    public byte[] getResponse() {
        switch (mResult) {
            case RESULT_SUCCESS: {
                if (mResponseData != null) {
                    return Arrays.copyOf(mResponseData, mResponseData.length);
                }
                return new byte[0];
            }
            case RESULT_TAGLOST:
            case RESULT_EXCEEDED_LENGTH:
            default: {
                return new byte[0];
            }
        }
    }

    /**
     * Marshalls the tags.
     *
     * @hide
     */
    @Override
    public boolean marshalling(Parcel out) {
        if (out == null) {
            return false;
        }
        out.writeInt(mResult);
        out.writeInt(mResponseLength);
        out.writeByteArray(mResponseData);
        return true;
    }

    /**
     * Unmarshalls the tags.
     *
     * @hide
     */
    @Override
    public boolean unmarshalling(Parcel in) {
        if (in == null) {
            return false;
        }
        this.mResult = in.readInt();
        this.mResponseLength = in.readInt();
        this.mResponseData = in.readByteArray();
        return true;
    }
}
