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

import ohos.aafwk.content.Intent;
import ohos.hiviewdfx.HiLog;
import ohos.hiviewdfx.HiLogLabel;
import ohos.nfc.NfcKitsUtils;
import ohos.rpc.RemoteException;
import ohos.utils.net.Uri;
import ohos.utils.Parcel;
import ohos.utils.Sequenceable;
import ohos.utils.system.SystemCap;

import java.nio.BufferUnderflowException;
import java.nio.ByteBuffer;
import java.nio.charset.StandardCharsets;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.Locale;
import java.util.Optional;

/**
 * Represents an NDEF record.
 *
 * <p>{@code MessageRecord} is the metadata of {@code NdefMessage}. An {@code NdefMessage} instance
 * consists of one or more {@code MessageRecord}.
 *
 * @Syscap {@link SystemCapability.Communication#NFC}
 * @since 3
 */
@SystemCap("SystemCapability.Communication.NFC")
public class MessageRecord implements Sequenceable {
    /**
     * Indicates that the NDEF record is empty.
     *
     * @hide
     */
    public static final short TNF_EMPTY = 0x00;

    /**
     * Indicates that the type field contains a well-known RTD type.
     *
     * @hide
     */
    public static final short TNF_WELL_KNOWN = 0x01;

    /**
     * Indicates that the type field contains a media-type BNF construct, as defined by RFC 2046.
     *
     * @hide
     */
    public static final short TNF_MIME_MEDIA = 0x02;

    /**
     * Indicates that the type field contains an absolute-URI BNF construct, as defined by RFC 3986.
     *
     * @hide
     */
    public static final short TNF_ABSOLUTE_URI = 0x03;

    /**
     * Indicates that the type field contains an external type.
     *
     * @hide
     */
    public static final short TNF_EXTERNAL_TYPE = 0x04;

    /**
     * Indicates that the payload type is unknown.
     *
     * @hide
     */
    public static final short TNF_UNKNOWN = 0x05;

    /**
     * Indicates that the payload is an intermediate or final chunk of a chunked NDEF record.
     *
     * @hide
     */
    public static final short TNF_UNCHANGED = 0x06;

    /**
     * Reserved TNF type
     *
     * @hide
     */
    public static final short TNF_RESERVED = 0x07;

    /**
     * RTD Smart Poster type
     *
     * @hide
     */
    public static final byte[] RTD_SMART_POSTER = {0x53, 0x70};  // "Sp"

    /**
     * RTD Text type
     *
     * @hide
     */
    public static final byte[] RTD_TEXT = {0x54};  // "T"

    /**
     * RTD URI type
     *
     * @hide
     */
    public static final byte[] RTD_URI = {0x55};   // "U"

    /**
     * RTD OHOS App type.
     *
     * Use {@link #buildApplicationRecord(String)} to create RTD_OHOS_APP records.
     *
     * @hide
     */
    public static final byte[] RTD_OHOS_APP = "ohos.com:pkg".getBytes();

    private static class RecordFlag {
        private boolean isMessageBegin;
        private boolean isChunkFlag;
        private boolean isShortRecord;
        private boolean isIdLengthExist;
        private short typeNameFormat;
        private byte flag;

        RecordFlag(byte flag) {
            this.flag = flag;
            this.isMessageBegin = (flag & MessageRecord.FLAG_MB) != 0;
            this.isChunkFlag = (flag & MessageRecord.FLAG_CF) != 0;
            this.isShortRecord = (flag & MessageRecord.FLAG_SR) != 0;
            this.isIdLengthExist = (flag & MessageRecord.FLAG_IL) != 0;
            this.typeNameFormat = (short) (flag & 0x07);
        }
    }

    private static final HiLogLabel LABEL = new HiLogLabel(HiLog.LOG_CORE, NfcKitsUtils.NFC_DOMAIN_ID,
        "MessageRecord");

    private static final byte[] EMPTY_BYTE_ARRAY = new byte[0];

    private static final byte FLAG_MB = (byte) 0x80;
    private static final byte FLAG_ME = (byte) 0x40;
    private static final byte FLAG_CF = (byte) 0x20;
    private static final byte FLAG_SR = (byte) 0x10;
    private static final byte FLAG_IL = (byte) 0x08;

    private static final int BYTE_UNSIGN_MASK = 0xFF;
    private static final long LONG_BYTE_UNSIGN_MASK = 0xFFFFFFFFL;

    /**
     * NFC Forum "URI Record Type Definition"<p>
     * This is a mapping of "URI Identifier Codes" to URI string prefixes,
     * per section 3.2.2 of the NFC Forum URI Record Type Definition document.
     */
    private static final String[] URI_PREFIX_MAP = new String[] {
            "", // 0x00
            "http://www.", // 0x01
            "https://www.", // 0x02
            "http://", // 0x03
            "https://", // 0x04
            "tel:", // 0x05
            "mailto:", // 0x06
            "ftp://anonymous:anonymous@", // 0x07
            "ftp://ftp.", // 0x08
            "ftps://", // 0x09
            "sftp://", // 0x0A
            "smb://", // 0x0B
            "nfs://", // 0x0C
            "ftp://", // 0x0D
            "dav://", // 0x0E
            "news:", // 0x0F
            "telnet://", // 0x10
            "imap:", // 0x11
            "rtsp://", // 0x12
            "urn:", // 0x13
            "pop:", // 0x14
            "sip:", // 0x15
            "sips:", // 0x16
            "tftp:", // 0x17
            "btspp://", // 0x18
            "btl2cap://", // 0x19
            "btgoep://", // 0x1A
            "tcpobex://", // 0x1B
            "irdaobex://", // 0x1C
            "file://", // 0x1D
            "urn:epc:id:", // 0x1E
            "urn:epc:tag:", // 0x1F
            "urn:epc:pat:", // 0x20
            "urn:epc:raw:", // 0x21
            "urn:epc:", // 0x22
            "urn:nfc:", // 0x23
    };

    private static final int MAX_PAYLOAD_SIZE = 10 * (1 << 20);  // 10 MB payload limit

    private short mTnf;
    private byte[] mType;
    private byte[] mId;
    private byte[] mPayload;

    /**
     * Constructs an NDEF record with specified parameters.
     *
     * @param tnf Indicates the TNF of the NDEF record.
     * @param type Indicates the payload type of the NDEF record.
     * @param id Indicates the ID of the NDEF record, which is used for referencing between NDEF records.
     * @param payload Indicates the payload, which contains user data.
     * @since 3
     */
    public MessageRecord(short tnf, byte[] type, byte[] id, byte[] payload) {
        if (type == null) {
            type = EMPTY_BYTE_ARRAY;
        }
        if (id == null) {
            id = EMPTY_BYTE_ARRAY;
        }
        if (payload == null) {
            payload = EMPTY_BYTE_ARRAY;
        }

        Optional<String> msg = checkTnf(tnf, type, id, payload);
        if (msg.isPresent()) {
            throw new IllegalArgumentException(msg.get());
        }

        mTnf = tnf;
        mType = type;
        mId = id;
        mPayload = payload;
    }

    /**
     * Obtains the TNF of the NDEF record.
     *
     * @return Returns the TNF of the NDEF record.
     * @since 3
     */
    public short getTnf() {
        return mTnf;
    }

    /**
     * Obtains the payload type of the NDEF record.
     *
     * @return Returns the payload type of the NDEF record.
     * @since 3
     */
    public byte[] getType() {
        return Arrays.copyOf(mType, mType.length);
    }

    /**
     * Obtains the ID of the NDEF record.
     *
     * @return Returns the ID of the NDEF record.
     * @since 3
     */
    public byte[] getId() {
        return Arrays.copyOf(mId, mId.length);
    }

    /**
     * Obtains the payload of the NDEF record.
     *
     * @return Returns the payload of the NDEF record.
     * @since 3
     */
    public byte[] getPayload() {
        return Arrays.copyOf(mPayload, mPayload.length);
    }

    /**
     * Gets the hash code of the object.
     *
     * @return Returns the hashcode.
     * @since 3
     */
    @Override
    public int hashCode() {
        final int prime = 31;
        int result = 1;
        result = prime * result + Arrays.hashCode(mId);
        result = prime * result + Arrays.hashCode(mPayload);
        result = prime * result + mTnf;
        result = prime * result + Arrays.hashCode(mType);
        return result;
    }

    /**
     * Check whethers two objects are equal.
     *
     * @return Returns {@code true} if two objects are equal; returns {@code false} otherwise.
     * @since 3
     */
    @Override
    public boolean equals(Object obj) {
        if (this == obj) {
            return true;
        }
        if (obj == null) {
            return false;
        }
        if (getClass() != obj.getClass()) {
            return false;
        }
        if (!(obj instanceof MessageRecord)) {
            return false;
        }
        MessageRecord other = (MessageRecord) obj;
        if (!Arrays.equals(mId, other.mId)) {
            return false;
        }
        if (!Arrays.equals(mPayload, other.mPayload)) {
            return false;
        }
        if (mTnf != other.mTnf) {
            return false;
        }
        return Arrays.equals(mType, other.mType);
    }

    /**
     * Creates an NDEF record for an OHOS application.
     *
     * @param packageName Indicates the package name of the application.
     * @return Returns the created NDEF record.
     * @throws IllegalArgumentException if the package name is empty.
     * @since 3
     */
    public static MessageRecord buildApplicationRecord(String packageName) {
        if (packageName == null) {
            throw new NullPointerException("packageName is null");
        }
        if (packageName.length() == 0) {
            throw new IllegalArgumentException("packageName is empty");
        }

        return new MessageRecord(TNF_EXTERNAL_TYPE, RTD_OHOS_APP, null,
                packageName.getBytes(StandardCharsets.UTF_8));
    }

    /**
     * Creates an NDEF record containing MIME data.
     *
     * @param mimeType Indicates the MIME type.
     * @param mimeData Indicates the MIME data.
     * @return Returns the NDEF record containing MIME data.
     * @throws IllegalArgumentException if the mimeType is empty or invalid.
     * @since 3
     */
    public static MessageRecord buildMime(String mimeType, byte[] mimeData) {
        if (mimeType == null) {
            throw new NullPointerException("mimeType is null");
        }

        /* Do only basic MIME type validation: trying to follow the
         * RFC rules strictly only ends in tears, since there are lots of MIME
         * types in common use that are not strictly valid as per RFC rules
         */
        Optional<String> normalMimeType = normalizeMimeType(mimeType);
        if (normalMimeType.get().length() == 0) {
            throw new IllegalArgumentException("mimeType is empty");
        }
        int slashIndex = normalMimeType.get().indexOf('/');
        if (slashIndex == 0) {
            throw new IllegalArgumentException("mimeType must have major type");
        }
        if (slashIndex == normalMimeType.get().length() - 1) {
            throw new IllegalArgumentException("mimeType must have minor type");
        }
        // missing '/' is allowed

        // MIME RFCs suggest ASCII encoding for content-type
        byte[] typeBytes = normalMimeType.get().getBytes(StandardCharsets.US_ASCII);
        return new MessageRecord(TNF_MIME_MEDIA, typeBytes, null, mimeData);
    }

    /**
     * Creates an NDEF record containing external data.
     *
     * @param domain Indicates the domain name of the data issuing organization.
     * @param type Indicates the data type.
     * @param data Indicates the external data.
     * @return Returns the NDEF record containing external data.
     * @throws IllegalArgumentException if either domain or type are empty or invalid.
     * @since 3
     */
    public static MessageRecord buildExternal(String domain, String type, byte[] data) {
        if (domain == null) {
            throw new NullPointerException("domain is null");
        }
        if (type == null) {
            throw new NullPointerException("type is null");
        }

        domain = domain.trim().toLowerCase(Locale.ROOT);
        type = type.trim().toLowerCase(Locale.ROOT);

        if (domain.length() == 0) {
            throw new IllegalArgumentException("domain is empty");
        }
        if (type.length() == 0) {
            throw new IllegalArgumentException("type is empty");
        }

        byte[] byteDomain = domain.getBytes(StandardCharsets.UTF_8);
        byte[] byteType = type.getBytes(StandardCharsets.UTF_8);
        byte[] byteDomainAndType = new byte[byteDomain.length + 1 + byteType.length];
        System.arraycopy(byteDomain, 0, byteDomainAndType, 0, byteDomain.length);
        byteDomainAndType[byteDomain.length] = ':';
        System.arraycopy(byteType, 0, byteDomainAndType, byteDomain.length + 1, byteType.length);

        return new MessageRecord(TNF_EXTERNAL_TYPE, byteDomainAndType, null, data);
    }

    /**
     * Creates an NDEF record containing text data.
     *
     * @param languageCode Indicates the language code.
     * @param text Indicates the text.
     * @return Returns the NDEF record containing text data.
     * @return Returns a MessageRecord object
     * @throws IllegalArgumentException if the languageCode is empty or invalid.
     * @since 3
     */
    public static MessageRecord buildTextRecord(String languageCode, String text) {
        if (text == null) {
            throw new NullPointerException("text is null");
        }

        byte[] textBytes = text.getBytes(StandardCharsets.UTF_8);

        byte[] languageCodeBytes = null;
        if (languageCode != null && !languageCode.isEmpty()) {
            languageCodeBytes = languageCode.getBytes(StandardCharsets.US_ASCII);
        } else {
            languageCodeBytes = Locale.getDefault().getLanguage().getBytes(StandardCharsets.US_ASCII);
        }
        // We only have 6 bits to indicate ISO/IANA language code.
        if (languageCodeBytes.length >= 64) {
            throw new IllegalArgumentException("language code is too long, must be <64 bytes.");
        }
        ByteBuffer buffer = ByteBuffer.allocate(1 + languageCodeBytes.length + textBytes.length);

        byte status = (byte) (languageCodeBytes.length & BYTE_UNSIGN_MASK); // UTF-8
        buffer.put(status);
        buffer.put(languageCodeBytes);
        buffer.put(textBytes);

        return new MessageRecord(TNF_WELL_KNOWN, RTD_TEXT, null, buffer.array());
    }

    /**
     * Checks whether the TNF is valid.
     *
     * @return null if the TNF is valid, or a string error if invalid.
     * @since 3
     * @hide
     */
    static Optional<String> checkTnf(short tnf, byte[] type, byte[] id, byte[] payload) {
        switch (tnf) {
            case TNF_EMPTY:
                if (type.length != 0 || id.length != 0 || payload.length != 0) {
                    return Optional.of("error data in TNF_EMPTY record.");
                }
                return Optional.empty();
            case TNF_WELL_KNOWN:
            case TNF_MIME_MEDIA:
            case TNF_ABSOLUTE_URI:
            case TNF_EXTERNAL_TYPE:
                return Optional.empty();
            case TNF_UNKNOWN:
            case TNF_RESERVED:
                if (type.length != 0) {
                    return Optional.of("error type field in TNF_UNKNOWN or TNF_RESERVEd record");
                }
                return Optional.empty();
            case TNF_UNCHANGED:
                return Optional.of("error TNF_UNCHANGED in first chunk or logical record");
            default:
                return Optional.of(String.format(Locale.ENGLISH, "error type name format value: 0x%02x", tnf));
        }
    }

    /**
     * Normalizes an MIME data type.
     *
     * @param mimeType Indicates MIME data type to normalize.
     * @return Returns normalized MIME data type; returns null if the input was empty.
     * @hide
     */
    static Optional<String> normalizeMimeType(String mimeType) {
        if (mimeType == null) {
            return Optional.empty();
        }

        mimeType = mimeType.trim().toLowerCase(Locale.ROOT);
        final int semicolonIndex = mimeType.indexOf(';');
        if (semicolonIndex != -1) {
            mimeType = mimeType.substring(0, semicolonIndex);
        }
        return Optional.of(mimeType);
    }

    /**
     * Gets byte length of serialized record.
     *
     * @return Returns the byte length of record.
     * @since 3
     * @hide
     */
    public int getAllRecordsLength() {
        int length = 3 + mType.length + mId.length + mPayload.length;

        boolean isShortRecord = mPayload.length < 256;
        boolean isIdLengthExist = mTnf == TNF_EMPTY ? true : mId.length > 0;

        if (!isShortRecord) {
            length += 3;
        }
        if (isIdLengthExist) {
            length += 1;
        }

        return length;
    }

    /**
     * Builds a URI from this NDEF record.
     *
     * @return Returns a URI in lowercase if the buding is successful; returns null if it is not a URI record.
     * @since 4
     */
    public Optional<Uri> buildUri() {
        return buildUri(false);
    }

    private Optional<Uri> buildUri(boolean isInSmartPoster) {
        switch (mTnf) {
            case TNF_WELL_KNOWN:
                if (Arrays.equals(mType, RTD_SMART_POSTER) && !isInSmartPoster) {
                    try {
                        // checks payload for a NdefMessage has a URI
                        NdefMessage ndefMessage = new NdefMessage(mPayload);
                        for (MessageRecord messageRecord : ndefMessage.getRecords()) {
                            Optional<Uri> uri = messageRecord.buildUri(true);
                            if (uri.isPresent()) {
                                return uri;
                            }
                        }
                    } catch (IllegalArgumentException e) {
                        HiLog.error(LABEL, "buildUri catch IllegalArgumentException");
                    }
                } else if (Arrays.equals(mType, RTD_URI)) {
                    Optional<Uri> wktUri = marshallingWktUri();
                    return (wktUri.isPresent() ? Optional.of(wktUri.get().getLowerCaseScheme()) : Optional.empty());
                } else {
                    break;
                }
                break;
            case TNF_ABSOLUTE_URI:
                Uri uri = Uri.parse(new String(mType, StandardCharsets.UTF_8));
                return Optional.of(uri.getLowerCaseScheme());
            case TNF_EXTERNAL_TYPE:
                if (isInSmartPoster) {
                    break;
                }
                return Optional.of(Uri.parse("vnd.ohos.nfc://ext/" + new String(mType, StandardCharsets.US_ASCII)));
            default:
                return Optional.empty();
        }
        return Optional.empty();
    }

    /**
     * Parses complete URI from TNF_WELL_KNOWN and RTD_URI records.
     *
     * @return Returns a complete URI if any; returns null otherwise.
     */
    private Optional<Uri> marshallingWktUri() {
        // payload's length no less than 2, it must contains index of prefix and suffix of a URI.
        if (mPayload.length < 2) {
            return Optional.empty();
        }

        // payload[0] contains the URI Identifier Code
        int prefixIdx = (mPayload[0] & (byte) BYTE_UNSIGN_MASK);
        if (prefixIdx < 0 || prefixIdx >= URI_PREFIX_MAP.length) {
            return Optional.empty();
        }
        String prefix = URI_PREFIX_MAP[prefixIdx];
        String suffix = new String(Arrays.copyOfRange(mPayload, 1, mPayload.length), StandardCharsets.UTF_8);
        return Optional.of(Uri.parse(prefix + suffix));
    }

    /**
     * Parses MessageRecords from bytebuffer.
     *
     * @param buffer Indicates the bytebuffer to parse.
     * @param ignoreMbMe Indicates whether to ignore MbMe or not.
     * @returns the parsed MessageRecords if any; returns null otherwise.
     * @throws IllegalArgumentException if the input is illegal.
     * @hide
     */
    public static MessageRecord[] parse(ByteBuffer buffer, boolean ignoreMbMe) throws IllegalArgumentException {
        List<MessageRecord> records = new ArrayList<MessageRecord>();
        try {
            byte[] type = null;
            byte[] id = null;
            boolean inChunk = false;
            boolean isMessageEnd = false;

            while (!isMessageEnd) {
                RecordFlag recordFlag = new RecordFlag(buffer.get());
                isMessageEnd = (recordFlag.flag & MessageRecord.FLAG_ME) != 0;

                if (isValidRecordFlag(recordFlag, isMessageEnd, inChunk, ignoreMbMe, records.size())) {
                    int typeLength = buffer.get() & BYTE_UNSIGN_MASK;
                    long payloadLength =  recordFlag.isShortRecord ?
                        (buffer.get() & BYTE_UNSIGN_MASK) : (buffer.getInt() & LONG_BYTE_UNSIGN_MASK);
                    int idLength = recordFlag.isIdLengthExist ? (buffer.get() & BYTE_UNSIGN_MASK) : 0;

                    if (inChunk && typeLength != 0) {
                        throw new IllegalArgumentException("expected zero-length type in non-leading chunk");
                    }

                    if (!inChunk) {
                        type = (typeLength > 0 ? new byte[typeLength] : EMPTY_BYTE_ARRAY);
                        id = (idLength > 0 ? new byte[idLength] : EMPTY_BYTE_ARRAY);
                        buffer.get(type);
                        buffer.get(id);
                    }

                    byte[] payload = checkPayloadLengthAndGetPayload(buffer, payloadLength);
                    handleChunks(recordFlag, inChunk, typeLength, payload);

                    if (recordFlag.isChunkFlag) {
                        // more chunks to come
                        inChunk = true;
                        continue;
                    } else {
                        inChunk = false;
                    }

                    Optional<String> error = checkTypeNameFormat(recordFlag.typeNameFormat, type, id, payload);
                    if (error.isPresent()) {
                        throw new IllegalArgumentException(error.get());
                    }
                    records.add(new MessageRecord(recordFlag.typeNameFormat, type, id, payload));
                    if (ignoreMbMe) {  // for parsing a single NdefRecord
                        break;
                    }
                }
            }
        } catch (BufferUnderflowException e) {
            throw new IllegalArgumentException("expected more data when parsing", e);
        }
        return records.toArray(new MessageRecord[records.size()]);
    }

    private static byte[] checkPayloadLengthAndGetPayload(ByteBuffer buffer, long payloadLength) {
        checkPayloadSize(payloadLength);
        byte[] payload = null;
        payload = (payloadLength > 0 ? new byte[(int) payloadLength] : EMPTY_BYTE_ARRAY);
        buffer.get(payload);
        return payload;
    }

    private static boolean isValidRecordFlag(RecordFlag recordFlag, boolean isMessageEnd, boolean inChunk,
        boolean ignoreMbMe, int recordsSize) {
        if (!recordFlag.isMessageBegin && recordsSize == 0 && !inChunk && !ignoreMbMe) {
            throw new IllegalArgumentException("expected MessageBegin record flag");
        } else if (recordFlag.isMessageBegin && (recordsSize != 0 || inChunk) && !ignoreMbMe) {
            throw new IllegalArgumentException("error MessageBegin record flag");
        } else if (recordFlag.isChunkFlag && isMessageEnd) {
            throw new IllegalArgumentException("error MessageEnd record flag in non-trailing record chunk");
        } else if (inChunk && recordFlag.isIdLengthExist) {
            throw new IllegalArgumentException("error IdLength record flag in non-leading record chunk");
        } else if (inChunk && recordFlag.typeNameFormat != MessageRecord.TNF_UNCHANGED) {
            throw new IllegalArgumentException("expected TNF_UNCHANGED record flag in non-leading record chunk");
        } else if (!inChunk && recordFlag.typeNameFormat == MessageRecord.TNF_UNCHANGED) {
            throw new IllegalArgumentException("error TNF_UNCHANGED record flag "
                + "in first chunk or unchunked record");
        } else {
            return true;
        }
    }

    private static void handleChunks(RecordFlag recordFlag, boolean inChunk, int typeLength, byte[] payload) {
        ArrayList<byte[]> chunks = new ArrayList<byte[]>();
        short chunkTnf = -1; // init as -1, means the type name format is not set
        if (recordFlag.isChunkFlag && !inChunk) {
            // first chunk
            if (typeLength == 0 && recordFlag.typeNameFormat != MessageRecord.TNF_UNKNOWN) {
                throw new IllegalArgumentException("expected non-zero type length in record first chunk");
            }
            chunks.clear();
            chunkTnf = recordFlag.typeNameFormat;
        }
        if (recordFlag.isChunkFlag || inChunk) {
            // any chunk
            chunks.add(payload);
        }
        if (!recordFlag.isChunkFlag && inChunk) {
            // last chunk, flatten the payload
            long payloadLength = 0L;
            for (byte[] payloadInChunks : chunks) {
                payloadLength += payloadInChunks.length;
            }
            checkPayloadSize(payloadLength);
            payload = new byte[(int) payloadLength];
            int i = 0;
            for (byte[] payloadInChunks : chunks) {
                System.arraycopy(payloadInChunks, 0, payload, i, payloadInChunks.length);
                i += payloadInChunks.length;
            }
            recordFlag.typeNameFormat = chunkTnf;
        }
    }

    private static void checkPayloadSize(long size) throws IllegalArgumentException {
        if (size > MAX_PAYLOAD_SIZE) {
            throw new IllegalArgumentException(
                "payload size larger than limit: " + MAX_PAYLOAD_SIZE);
        }
    }

    private static Optional<String> checkTypeNameFormat(short tnf, byte[] type, byte[] id, byte[] payload) {
        switch (tnf) {
            case TNF_WELL_KNOWN:
            case TNF_MIME_MEDIA:
            case TNF_ABSOLUTE_URI:
            case TNF_EXTERNAL_TYPE:
                return Optional.empty();
            case TNF_EMPTY:
                if (type.length != 0 || id.length != 0 || payload.length != 0) {
                    return Optional.of("error data in TNF_EMPTY message record");
                }
                return Optional.empty();
            case TNF_UNKNOWN:
            case TNF_RESERVED:
                if (type.length != 0) {
                    return Optional.of("error type field in TNF_UNKNOWN or TNF_RESERVEd message record");
                }
                return Optional.empty();
            case TNF_UNCHANGED:
                return Optional.of("error TNF_UNCHANGED in first chunk or logical message record");
            default:
                return Optional.of(String.format(Locale.ROOT, "error tnf value: 0x%02x", tnf));
        }
    }

    @Override
    public boolean marshalling(Parcel out) {
        if (out == null) {
            return false;
        }
        out.writeInt(mTnf);
        out.writeInt(mType.length);
        out.writeByteArray(mType);
        out.writeInt(mId.length);
        out.writeByteArray(mId);
        out.writeInt(mPayload.length);
        out.writeByteArray(mPayload);
        return true;
    }

    @Override
    public boolean unmarshalling(Parcel in) {
        if (in == null) {
            return false;
        }
        this.mTnf = (short)in.readInt();
        int typeLength = in.readInt();
        this.mType = in.readByteArray();
        int idLength = in.readInt();
        this.mId = in.readByteArray();
        int payloadLength = in.readInt();
        this.mPayload = in.readByteArray();
        return true;
    }
}
