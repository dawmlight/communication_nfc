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
import ohos.nfc.NfcErrCodes;
import ohos.nfc.NfcKitsUtils;
import ohos.rpc.RemoteException;
import ohos.utils.system.SystemCap;

import java.io.IOException;
import java.util.Arrays;
import java.util.Optional;

/**
 * Provides interfaces to create an {@code NdefTag} object, control connections to the NDEF tag,
 * and read data from and write data to the NDEF tag.
 *
 * <p>This class inherits from {@link TagManager}.
 *
 * @Syscap {@link SystemCapability.Communication#NFC}
 * @since 3
 */
@SystemCap("SystemCapability.Communication.NFC")
public class NdefTag extends TagManager {
    /** @hide */
    public static final int NDEF_MODE_READ_ONLY = 1;

    /** @hide */
    public static final int NDEF_MODE_READ_WRITE = 2;

    /** @hide */
    public static final int NDEF_MODE_UNKNOWN = 3;

    /**
     * Indicates the keyword used to obtain the maximum NDEF message size.
     *
     * @since 3
     * @hide
     */
    public static final String EXTRA_NDEF_MAX_LENGTH = "ndefmaxlength";

    /**
     * Indicates the keyword used to obtain the card state of a NDEF tag that is based
     * on the NDEF technology.
     *
     * @since 3
     * @hide
     */
    public static final String EXTRA_NDEF_CARDSTATE = "ndefcardstate";

    /**
     * Indicates the keyword used to obtain the messages of a NDEF tag that is based
     * on the NDEF technology.
     *
     * @since 3
     * @hide
     */
    public static final String EXTRA_NDEF_MSG = "ndefmsg";

    /**
     * Indicates the keyword used to obtain the type of a NDEF tag that is based
     * on the NDEF technology.
     *
     * @since 3
     * @hide
     */
    public static final String EXTRA_NDEF_TYPE = "ndeftype";

    /**
     * Indicates the keyword used to describe other type of a NDEF tag.
     * on the NDEF technology.
     *
     * @since 3
     * @hide
     */
    public static final int TYPE_OTHER = -1;

    /**
     * Indicates the keyword used to describe NFC Forum Tag Type 1 NDEF tag.
     *
     * @since 3
     * @hide
     */
    public static final int TYPE_1 = 1;

    /**
     * Indicates the keyword used to describe NFC Forum Tag Type 2 NDEF tag.
     *
     * @since 3
     * @hide
     */
    public static final int TYPE_2 = 2;

    /**
     * Indicates the keyword used to describe NFC Forum Tag Type 3 NDEF tag.
     *
     * @since 3
     * @hide
     */
    public static final int TYPE_3 = 3;

    /**
     * Indicates the keyword used to describe NFC Forum Tag Type 4 NDEF tag.
     *
     * @since 3
     * @hide
     */
    public static final int TYPE_4 = 4;

    /**
     * Indicates the keyword used to describe MIFARE Classic type of NDEF tag.
     *
     * @since 3
     * @hide
     */
    public static final int TYPE_MIFARE_CLASSIC = 101;

    /**
     * Indicates the keyword used to describe iCODE SLI type of NDEF tag.
     *
     * @since 3
     * @hide
     */
    public static final int TYPE_ICODE_SLI = 102;

    /**
     * Indicates the keyword used to describe unknown type of NDEF tag.
     *
     * @since 3
     * @hide
     */
    public static final String NDEF_TYPE_UNKNOWN = "ohos.ndef.unknown";

    /**
     * Indicates the NDEF tag of type 1.
     *
     * @since 3
     */
    public static final String NDEF_TYPE_1 = "nfcforum.type1";

    /**
     * Indicates the NDEF tag of type 2.
     *
     * @since 3
     */
    public static final String NDEF_TYPE_2 = "nfcforum.type2";

    /**
     * Indicates the NDEF tag of type 3.
     *
     * @since 3
     */
    public static final String NDEF_TYPE_3 = "nfcforum.type3";

    /**
     * Indicates the NDEF tag of type 4.
     *
     * @since 3
     */
    public static final String NDEF_TYPE_4 = "nfcforum.type4";

    /**
     * Indicates the NDEF tag of the Mifare Classic type..
     *
     * @since 3
     */
    public static final String NDEF_TYPE_MIFARE_CLASSIC = "nxp.mifare.classic";

    /**
     * Indicates the NDEF tag of the iCODE SLI type.
     *
     * @since 3
     */
    public static final String NDEF_TYPE_ICODE_SLI = "nxp.icode.sli";

    private static final HiLogLabel LABEL = new HiLogLabel(HiLog.LOG_CORE, NfcKitsUtils.NFC_DOMAIN_ID, "NdefTag");

    private final int mMaxNdefSize;
    private final int mCardState;
    private final NdefMessage mNdefMsg; // to define NdefMessage class.
    private final int mNdefType;

    private NfcTagProxy mNfcTagProxy;
    private TagInfo mTagInfo;

    /**
     * Obtains an instance of {@code NdefTag} object based on the given tag information.
     *
     * <p>If the given tag is NDEF formatted, an {@code NdefTag} object will be created based on the
     * given tag information; otherwise, an empty {@code NdefTag} object will be created.
     *
     * @param tagInfo Indicates the information about the given tag.
     * @return NdefTag object
     * @since 3
     */
    public static Optional<NdefTag> getInstance(TagInfo tagInfo) {
        if (!tagInfo.isProfileSupported(TagManager.NDEF)) {
            return Optional.empty();
        }
        return Optional.of(new NdefTag(tagInfo));
    }

    /**
     * Obtains an {@code NdefTag} object based on the given tag information.
     *
     * <p>If the given tag is NDEF formatted, an {@code NdefTag} object will be created based on the
     * given tag information; otherwise, an empty {@code NdefTag} object will be created.
     *
     * @param tagInfo Indicates the information about the given tag.
     * @since 3
     * @Deprecated .
     */
    public NdefTag(TagInfo tagInfo) {
        super(tagInfo, TagManager.NDEF);
        if (tagInfo == null) {
            throw new NullPointerException("NDEF TagInfo is null.");
        }
        if (tagInfo.isProfileSupported(TagManager.NDEF)) {
            Optional<PacMapEx> extrasOptional = tagInfo.getProfileExtras(TagManager.NDEF);
            PacMapEx extras = extrasOptional.orElse(null);
            if (extras != null) {
                Optional<Object> maxNdefSizeObject = extras.getObjectValue(EXTRA_NDEF_MAX_LENGTH);
                mMaxNdefSize = (int) maxNdefSizeObject.get();
                Optional<Object> ndefCardStateObject = extras.getObjectValue(EXTRA_NDEF_CARDSTATE);
                mCardState = (int) ndefCardStateObject.get();
                if (extras.getObjectValue(EXTRA_NDEF_MSG).get() instanceof NdefMessage) {
                    Optional<Object> ndefMsgObject = extras.getObjectValue(EXTRA_NDEF_MSG);
                    mNdefMsg = (NdefMessage) ndefMsgObject.get();
                } else {
                    mNdefMsg = new NdefMessage(new MessageRecord[0]);
                }
                Optional<Object> ndefTypeObject = extras.getObjectValue(EXTRA_NDEF_TYPE);
                mNdefType = (int) ndefTypeObject.get();
            } else {
                throw new NullPointerException("NDEF tech extras are null");
            }
        } else {
            mMaxNdefSize = 0;
            mCardState = 0;
            mNdefMsg = new NdefMessage(new MessageRecord[0]);
            mNdefType = 0;
        }
        mNfcTagProxy = NfcTagProxy.getInstance();
        mTagInfo = getTagInfo();
    }

    /**
     * Obtains the NDEF message from this NDEF tag.
     *
     * @return Returns the NDEF message obtained from this NDEF tag.
     * @since 3
     */
    public NdefMessage getNdefMessage() {
        return mNdefMsg;
    }

    /**
     * Obtains the maximum NDEF message size.
     *
     * @return Returns the maximum NDEF message size in bytes.
     * @since 3
     */
    public int getNdefMaxSize() {
        return mMaxNdefSize;
    }

    /**
     * Obtains the NDEF tag type. Example types include {@link #NDEF_TYPE_1}, {@link #NDEF_TYPE_2},
     * {@link #NDEF_TYPE_3}, and {@link #NDEF_TYPE_4}.
     *
     * @return Returns the NDEF tag type.
     * @since 3
     */
    public String getTagType() {
        switch (mNdefType) {
            case TYPE_1:
                return NDEF_TYPE_1;
            case TYPE_2:
                return NDEF_TYPE_2;
            case TYPE_3:
                return NDEF_TYPE_3;
            case TYPE_4:
                return NDEF_TYPE_4;
            case TYPE_MIFARE_CLASSIC:
                return NDEF_TYPE_MIFARE_CLASSIC;
            case TYPE_ICODE_SLI:
                return NDEF_TYPE_ICODE_SLI;
            default:
                return NDEF_TYPE_UNKNOWN;
        }
    }

    /**
     * Reads the NDEF message from currently connected tag.
     *
     * @return Returns the NDEF message.
     * @throws IOException if there is an I/O failure, or the operation is canceled.
     * @throws IllegalArgumentException if the NDEF Message on the tag is malformed.
     * @throws RemoteException if read the NDEF failed.
     * @since 3
     */
    public NdefMessage readNdefMessage() throws IOException, IllegalArgumentException {
        checkConnected();
        try {
            if (mNfcTagProxy == null) {
                throw new IOException("Mock tags don't support this operation.");
            }
            int tagHandle = mTagInfo.getTagHandle();
            if (mNfcTagProxy.isNdefTag(tagHandle)) {
                NdefMessage msg = mNfcTagProxy.ndefRead(tagHandle);
                if (msg == null && !mNfcTagProxy.isTagConnected(tagHandle)) {
                    throw new IllegalArgumentException("Invalid param when read");
                }
                return msg;
            } else if (!mNfcTagProxy.isTagConnected(tagHandle)) {
                throw new IllegalArgumentException("Invalid param when read");
            } else {
                return null;
            }
        } catch (RemoteException remoteException) {
            HiLog.error(LABEL, "readNdefMessage open RemoteException");
            return null;
        }
    }

    /**
     * Writes the NDEF message to currently connected NDEF tag.
     *
     * @param msg Indicates the NDEF message to write, must not be null.
     * @throws IOException if there is an I/O failure, or the operation is canceled.
     * @throws IllegalArgumentException if the NDEF Message to write is malformed.
     * @throws RemoteException if write NDEF Message failed.
     * @since 3
     */
    public void writeNdefMessage(NdefMessage msg) throws IOException, IllegalArgumentException {
        checkConnected();

        try {
            if (mNfcTagProxy == null) {
                throw new IOException("Mock tags don't support this operation.");
            }
            int tagHandle = mTagInfo.getTagHandle();
            if (mNfcTagProxy.isNdefTag(tagHandle)) {
                int errCode = mNfcTagProxy.ndefWrite(tagHandle, msg);
                switch (errCode) {
                    case NfcErrCodes.SUCCESS:
                        break;
                    case NfcErrCodes.ERROR_IO:
                        throw new IOException("IO Exception occurred when write");
                    case NfcErrCodes.ERROR_INVALID_PARAM:
                        throw new IllegalArgumentException("Invalid param when write");
                    default:
                        throw new IOException("IO Exception occurred when write");
                }
            } else {
                throw new IOException("Tag is not NDEF.");
            }
        } catch (RemoteException remoteException) {
            HiLog.error(LABEL, "writeNdefMessage open RemoteException");
        }
    }

    /**
     * Checks whether this NDEF tag can be set to read-only.
     *
     * @return Returns {@code true} if this NDEF tag can be set to read-only;
     * returns {@code false} otherwise.
     * @since 3
     */
    public boolean canSetReadOnly() {
        if (mNfcTagProxy == null) {
            return false;
        }
        try {
            return mNfcTagProxy.canSetReadOnly(mNdefType);
        } catch (RemoteException remoteException) {
            HiLog.error(LABEL, "canSetReadOnly open RemoteException");
            return false;
        }
    }

    /**
     * Sets the NDEF tag to read-only.
     *
     * @return Returns {@code true} if the NDEF tag is set to read-only; returns {@code false} otherwise.
     * @throws IOException if there is an I/O failure, or the operation is canceled.
     * @since 3
     */
    public boolean setReadOnly() throws IOException {
        checkConnected();

        try {
            if (mNfcTagProxy == null) {
                return false;
            }
            int tagHandle = mTagInfo.getTagHandle();
            if (mNfcTagProxy.isNdefTag(tagHandle)) {
                int errCode = mNfcTagProxy.ndefSetReadOnly(tagHandle);
                switch (errCode) {
                    case NfcErrCodes.SUCCESS:
                        return true;
                    case NfcErrCodes.ERROR_IO:
                        throw new IOException("IO Exception occurred when setReadOnly");
                    case NfcErrCodes.ERROR_INVALID_PARAM:
                        return false;
                    default:
                        throw new IOException("IO Exception occurred when setReadOnly");
                }
            } else {
                throw new IOException("Tag is not NDEF.");
            }
        } catch (RemoteException remoteException) {
            HiLog.error(LABEL, "setReadOnly open RemoteException");
            return false;
        }
    }

    /**
     * Checks whether the NDEF tag is writable.
     *
     * @return Returns {@code true} if the NDEF tag is writable; returns {@code false} otherwise.
     * @since 3
     */
    public boolean isNdefWritable() {
        return (mCardState == NDEF_MODE_READ_WRITE);
    }
}
