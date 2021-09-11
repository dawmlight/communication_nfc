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
import ohos.utils.system.SystemCap;

import java.nio.ByteBuffer;
import java.util.Arrays;

/**
 * Represents an NDEF message, which is a container for one or more {@link MessageRecord} objects.
 *
 * @Syscap {@link SystemCapability.Communication#NFC}
 * @since 3
 */
@SystemCap("SystemCapability.Communication.NFC")
public class NdefMessage implements Sequenceable {
    private static int LIMIT_FOR_DATA = 1024;

    private MessageRecord[] mRecords;

    /**
     * Constructs an NDEF message by parsing raw bytes.
     *
     * @param data Indicates the raw bytes to parse.
     * @throws IllegalArgumentException if the data cannot be parsed.
     * @since 4
     */
    public NdefMessage(byte[] data) throws IllegalArgumentException {
        if (data == null) {
            throw new NullPointerException("data is null");
        }
        ByteBuffer byteBuf = ByteBuffer.wrap(data);
        mRecords = MessageRecord.parse(byteBuf, false);

        if (byteBuf.remaining() > 0) {
            throw new IllegalArgumentException("trailing data");
        }
    }
    /**
     * Constructs an NDEF message from one or more NDEF records.
     *
     * @param records Indicates one or more NDEF records.
     * @since 3
     */
    public NdefMessage(MessageRecord[] records) {
        if (records == null || records.length < 1 || records.length > LIMIT_FOR_DATA) {
            throw new IllegalArgumentException("records illegal when init NdefMessage");
        }

        for (MessageRecord record : records) {
            if (record == null) {
                throw new NullPointerException("record cannot be null");
            }
        }

        mRecords = new MessageRecord[records.length];
        System.arraycopy(records, 0, mRecords, 0, records.length);
    }

    /**
     * Obtains the NDEF records in this NDEF message.
     *
     * @return Returns an array of one or more NDEF records.
     * @since 3
     */
    public MessageRecord[] getRecords() {
        return mRecords;
    }

    /**
     * Obtains the length of this NDEF message.
     *
     * @return Returns the length of this NDEF message.
     * @since 3
     */
    public int getAllRecordsLength() {
        int length = 0;
        for (MessageRecord record : mRecords) {
            length += record.getAllRecordsLength();
        }
        return length;
    }

    @Override
    public int hashCode() {
        return Arrays.hashCode(mRecords);
    }

    @Override
    public boolean equals(Object obj) {
        if (obj == null) {
            return false;
        }
        if (obj == this) {
            return true;
        }
        if (!(obj instanceof NdefMessage)) {
            return false;
        }
        NdefMessage other = (NdefMessage) obj;
        return Arrays.equals(mRecords, other.mRecords);
    }

    @Override
    public boolean marshalling(Parcel out) {
        if (out == null) {
            return false;
        }
        if (mRecords != null) {
            int recordLength = mRecords.length;
            out.writeInt(recordLength);
            out.writeInt(recordLength);
            for (int i = 0; i < recordLength; i++) {
                out.writeSequenceable(mRecords[i]);
            }
        }
        return true;
    }

    @Override
    public boolean unmarshalling(Parcel in) {
        if (in == null) {
            return false;
        }
        int recordLength = in.readInt();
        if (recordLength < 0 || recordLength > LIMIT_FOR_DATA) {
            return true;
        }

        MessageRecord[] records = new MessageRecord[recordLength];
        if (recordLength == in.readInt()) {
            for (int i = 0; i < recordLength; i++) {
                in.readSequenceable(mRecords[i]);
            }
            mRecords = records;
        }
        return true;
    }
}
