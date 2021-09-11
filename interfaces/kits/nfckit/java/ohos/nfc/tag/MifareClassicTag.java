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
import ohos.nfc.NfcKitsUtils;
import ohos.utils.system.SystemCap;

import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;

/**
 * Provides interfaces to create {@code MifareClassic} tags and obtain the tag information.
 *
 * <p>This class inherits from {@link TagManager} and provides interfaces to create
 * {@code MifareClassic} tags and obtain the block and sector information of the
 * {@code MifareClassic} tags.
 *
 * @Syscap {@link SystemCapability.Communication#NFC}
 * @since 3
 */
@SystemCap("SystemCapability.Communication.NFC")
public class MifareClassicTag extends TagManager {
    /**
     * Indicates a {@code MifareClassic} tag with 16 sectors, each having 4 blocks.
     *
     * @since 3
     */
    public static final int TAG_ROM_1K = 1024;
    /**
     * Indicates a {@code MifareClassic} tag with 32 sectors, each having 4 blocks.
     *
     * @since 3
     */
    public static final int TAG_ROM_2K = 2048;
    /**
     * Indicates a {@code MifareClassic} tag with 40 sectors, with the first 32 sectors each having
     * 4 blocks and the remaining 8 sectors each having 16 blocks.
     *
     * @since 3
     */
    public static final int TAG_ROM_4K = 4096;
    /**
     * Indicates a {@code MifareClassic} tag with 5 sectors, each having 4 blocks.
     *
     * @since 3
     */
    public static final int TAG_ROM_MINI = 320;
    /**
     * Indicates the size of each block of the {@code MifareClassic} tag in bytes.
     *
     * @since 3
     */
    public static final int BLOCK_BYTES = 16;

    /**
     * Indicates the Mifare tag of an unknown type.
     *
     * @since 3
     */
    public static final int MIFARE_UNKNOWN = -1;
    /**
     * Indicates the Mifare classic tag.
     *
     * @since 3
     */
    public static final int MIFARE_CLASSIC = 0;
    /**
     * Indicates the Mifare Plus tag.
     *
     * @since 3
     */
    public static final int MIFARE_PLUS = 1;
    /**
     * Indicates the Mifare Pro tag.
     *
     * @since 3
     */
    public static final int MIFARE_PRO = 2;
    /**
     * Indicates the Mifare authentication key of type A.
     *
     * @since 3
     */
    public static final byte USE_KEY_A = 0x60;
    /**
     * Indicates the Mifare authentication key of type B.
     *
     * @since 3
     */
    public static final byte USE_KEY_B = 0x61;

    private static final int MAX_BLOCK_COUNT = 256;
    private static final int MAX_SECTOR_COUNT = 40;
    private static final int SAK_01 = 0x01;
    private static final int SAK_08 = 0x08;
    private static final int SAK_09 = 0x09;
    private static final int SAK_10 = 0x10;
    private static final int SAK_11 = 0x11;
    private static final int SAK_18 = 0x18;
    private static final int SAK_19 = 0x19;
    private static final int SAK_28 = 0x28;
    private static final int SAK_38 = 0x38;
    private static final int SAK_88 = 0x88;
    private static final int SAK_98 = 0x98;
    private static final int SAK_B8 = 0xB8;

    private int mMifareType = MIFARE_UNKNOWN;
    private int mMifareSize = 0;

    /**
     * Obtains a {@code MifareClassic} tag based on the given tag information.
     *
     * <p>During tag reading, if the tag supports the NFC-A technology, a {@code MifareClassic}
     * object will be created based on the given tag information.
     *
     * @param tagInfo Indicates the given tag information.
     * @return Returns the {@code MifareClassic} tag.
     * @since 3
     */
    public static MifareClassicTag getInstance(TagInfo tagInfo) {
        if (tagInfo == null) {
            throw new NullPointerException("MifareClassicTag tagInfo is null");
        }
        if (!tagInfo.isProfileSupported(TagManager.NFC_A)) {
            return null;
        }
        return new MifareClassicTag(tagInfo);
    }

    private MifareClassicTag(TagInfo tagInfo) {
        super(tagInfo, TagManager.MIFARE_CLASSIC);

        // Mifare uses NfcA
        NfcATag nfcATag = NfcATag.getInstance(tagInfo);
        mMifareType = MIFARE_UNKNOWN;

        int sak = -1;
        if (nfcATag != null) {
            sak = nfcATag.getSak();
        }
        switch (sak) {
            case SAK_01:
            case SAK_08:
                mMifareType = MIFARE_CLASSIC;
                mMifareSize = TAG_ROM_1K;
                break;
            case SAK_09:
                mMifareType = MIFARE_CLASSIC;
                mMifareSize = TAG_ROM_MINI;
                break;
            case SAK_10:
                mMifareType = MIFARE_PLUS;
                mMifareSize = TAG_ROM_2K;
                break;
            case SAK_11:
                mMifareType = MIFARE_PLUS;
                mMifareSize = TAG_ROM_4K;
                break;
            case SAK_18:
                mMifareType = MIFARE_CLASSIC;
                mMifareSize = TAG_ROM_4K;
                break;
            case SAK_19:
                mMifareType = MIFARE_CLASSIC;
                mMifareSize = TAG_ROM_2K;
                break;
            case SAK_28:
                mMifareType = MIFARE_CLASSIC;
                mMifareSize = TAG_ROM_1K;
                break;
            case SAK_38:
                mMifareType = MIFARE_CLASSIC;
                mMifareSize = TAG_ROM_4K;
                break;
            case SAK_88:
                mMifareType = MIFARE_CLASSIC;
                mMifareSize = TAG_ROM_1K;
                break;
            case SAK_98:
            case SAK_B8:
                mMifareType = MIFARE_PRO;
                mMifareSize = TAG_ROM_4K;
                break;
            default:
                break;
        }

        if (mMifareType == MIFARE_UNKNOWN) {
            throw new IllegalArgumentException("Unknown Mifare type, received sak = " + sak);
        }
    }

    /**
     * Obtains the Mifare type of this {@code MifareClassic} tag.
     *
     * @return Returns the Mifare type of this {@code MifareClassic} tag, such as
     * {@link #MIFARE_CLASSIC} and {@link #MIFARE_PLUS}.
     * @since 3
     */
    public int getMifareType() {
        return mMifareType;
    }

    /**
     * Obtains the size of this {@code MifareClassic} tag.
     *
     * @return Returns the size of this {@code MifareClassic} tag, such as {@link #TAG_ROM_1K}
     * and {@link #TAG_ROM_2K}.
     * @since 3
     */
    public int getTagSize() {
        return mMifareSize;
    }

    /**
     * Obtains the number of sectors of this {@code MifareClassic} tag base on the size of tag.
     *
     * @return Returns the number of sectors of this {@code MifareClassic} tag. For example,
     * a {@code MifareClassic} tag that is {@link #TAG_ROM_1K} in size has 16 sectors.
     * @since 3
     */
    public int getSectorsNum() {
        switch (mMifareSize) {
            case TAG_ROM_1K:
                return 16; // Returns 16 sectors when the tag ROM is 1k.
            case TAG_ROM_2K:
                return 32; // Returns 32 sectors when the tag ROM is 2k.
            case TAG_ROM_4K:
                return 40; // Returns 40 sectors when the tag ROM is 4k.
            case TAG_ROM_MINI:
                return 5; // Returns 5 sectors when the tag ROM is Mini.
            default:
                return 0; // Returns 0 sector when the tag ROM is the default.
        }
    }

    /**
     * Obtains the number of blocks of this {@code MifareClassic} tag.
     *
     * @return Returns the number of blocks of this {@code MifareClassic} tag.
     * @since 3
     */
    public int getBlocksNum() {
        return mMifareSize / BLOCK_BYTES;
    }

    /**
     * Obtains the number of blocks in a specified sector of this {@code MifareClassic} tag.
     *
     * @param sectorId Indicates the ID of the specified sector.
     * @return Returns the number of blocks in the specified sector of this {@code MifareClassic} tag.
     * @since 3
     */
    public int getBlocksNumForSector(int sectorId) {
        checkSectorId(sectorId);

        // Only tag with sector id greater than 32 has 16 blocks, otherwise 4 blocks
        if (sectorId < 32) {
            return 4; // mean this tag has 4 blocks
        } else {
            return 16; // mean this tag has 16 blocks
        }
    }

    /**
     * Obtains the sector ID of a Mifare Classic tag based on the block ID of the Mifare tag.
     *
     * @param blockId Indicates the block ID.
     * @return Returns the sector ID.
     * @since 3
     */
    public int getSectorId(int blockId) {
        checkBlockId(blockId);

        // Only tag with sector id greater than 32 has 16 blocks, otherwise 4 blocks
        if (blockId < 32 * 4) {
            return blockId / 4;
        } else {
            return 32 + (blockId - 32 * 4) / 16;
        }
    }

    /**
     * Obtains the ID of the first block of a {@code MifareClassic} tag in a specified sector.
     *
     * @param sectorId Indicates the ID of the specified sector.
     * @return Returns the ID of the first block in the specified sector.
     * @since 3
     */
    public int getFirstBlockId(int sectorId) {
        // Only tag with sector id greater than 32 has 16 blocks, otherwise 4 blocks
        if (sectorId < 32) {
            return sectorId * 4;
        } else {
            return 32 * 4 + (sectorId - 32) * 16;
        }
    }

    /**
     * Authenticates a specified sector of this {@code MifareClassic} tag with a key.
     *
     * @param sectorId Indicates the ID of the specified sector.
     * @param key Indicates the key in bit array format.
     * @param keyType Indicates the key type, such as {@link #USE_KEY_A} and {@link #USE_KEY_B}.
     * @return Returns {@code true} if the authentication succeeds; returns {@code false} otherwise.
     * @since 3
     */
    public boolean authenSectorUseKey(int sectorId, byte[] key, byte keyType) {
        checkSectorId(sectorId);
        checkConnected();
        checkKeyType(keyType);

        // command with 12 bytes
        byte[] cmd = new byte[12];

        // First byte is the command
        cmd[0] = keyType; // 0x60：keyA; 0x61: keyB

        cmd[1] = (byte) getFirstBlockId(sectorId);

        // Next 4 bytes are last 4 bytes of UID
        byte[] uid = getTagInfo().getTagId();
        if (uid.length < 4) {
            return false;
        }
        System.arraycopy(uid, uid.length - 4, cmd, 2, 4);

        // Next 6 bytes are key
        if (key.length < 6) {
            return false;
        }
        System.arraycopy(key, 0, cmd, 6, 6);

        if (sendData(cmd) != null) {
            return true;
        }
        return false;
    }

    /**
     * Reads content of a specified block of this {@code MifareClassic} tag based on the block id.
     *
     * @param blockId Indicates the ID of the block to read.
     * @return Returns the specified block content of this {@code MifareClassic} tag.
     * @since 3
     */
    public byte[] readBlock(int blockId) {
        checkBlockId(blockId);
        checkConnected();

        // specific command for read
        byte[] cmd = {0x30, (byte) blockId};

        return sendData(cmd);
    }

    /**
     * Writes data to a specified block of this {@code MifareClassic} tag.
     *
     * @param blockId Indicates the ID of the specified block to which the data will be written.
     * @param data Indicates the data to write to the specified block.
     * @since 3
     */
    public void writeBlock(int blockId, byte[] data) {
        checkBlockId(blockId);
        checkConnected();
        // data length should be 16 bytes
        if (data.length != 16) {
            throw new IllegalArgumentException("should write 16-bytes");
        }

        // first 2 bytes are for command
        byte[] cmd = new byte[data.length + 2];
        // specific command for write
        cmd[0] = (byte) 0xA0;
        cmd[1] = (byte) blockId;
        System.arraycopy(data, 0, cmd, 2, data.length);

        sendData(cmd);
    }

    /**
     * Adds a value to the content of a specified block of this {@code MifareClassic} tag.
     *
     * @param blockId Indicates the ID of block to increment.
     * @param value Indicates the non-negative value to increment.
     * @since 3
     */
    public void incBlock(int blockId, int value) {
        checkBlockId(blockId);
        checkValue(value);
        checkConnected();

        // 6 bytes as the buffer length
        ByteBuffer cmd = ByteBuffer.allocate(6);
        cmd.order(ByteOrder.LITTLE_ENDIAN);
        // specific command for increment
        cmd.put((byte) 0xC1);
        cmd.put((byte) blockId);
        cmd.putInt(value);

        sendData(cmd.array());
    }

    /**
     * Deducts a value from the content of a specified block of this {@code MifareClassic} tag.
     *
     * @param blockId Indicates the ID of block to decrement.
     * @param value Indicates the non-negative value to decrement.
     * @since 3
     */
    public void decBlock(int blockId, int value) {
        checkBlockId(blockId);
        checkValue(value);
        checkConnected();

        // 6 bytes as the buffer length
        ByteBuffer cmd = ByteBuffer.allocate(6);
        cmd.order(ByteOrder.LITTLE_ENDIAN);
        // specific command for decrement
        cmd.put((byte) 0xC0);
        cmd.put((byte) blockId);
        cmd.putInt(value);

        sendData(cmd.array());
    }

    /**
     * Migrates the content of a specified block of this {@code MifareClassic} tag to an internal buffer.
     *
     * @param blockId Indicates the ID of the specified value block.
     * @since 3
     */
    public void restoreBlock(int blockId) {
        checkBlockId(blockId);
        checkConnected();

        // specific cmd for restoring data
        byte[] cmd = {(byte) 0xC2, (byte) blockId};

        sendData(cmd);
    }

    private void checkSectorId(int sectorId) {
        if (sectorId < 0 || sectorId >= MAX_SECTOR_COUNT) {
            throw new IndexOutOfBoundsException("sectorId out of bounds: " + sectorId);
        }
    }

    private void checkBlockId(int blockId) {
        if (blockId < 0 || blockId >= MAX_BLOCK_COUNT) {
            throw new IndexOutOfBoundsException("blockId out of bounds: " + blockId);
        }
    }

    private void checkValue(int value) {
        if (value < 0) {
            throw new IllegalArgumentException("input not valid, value = " + value);
        }
    }

    private void checkKeyType(byte keyType) {
        if (keyType != USE_KEY_A && keyType != USE_KEY_B) {
            throw new IllegalArgumentException("input not valid, keyType = " + keyType);
        }
    }
}