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
#ifndef MIFARE_CLASSIC_TAG_H
#define MIFARE_CLASSIC_TAG_H

#include "basic_tag_session.h"

namespace OHOS {
namespace nfc {
namespace sdk {
class Tag;

class MifareClassicTag final : public BasicTagSession {
public:
    enum EmMifareTagType { TYPE_UNKNOWN = -1, TYPE_CLASSIC = 0, TYPE_PLUS = 1, TYPE_PRO = 2 };

    static const int MC_BLOCK_SIZE = 16;
    static const int MC_MAX_BLOCK_INDEX = 256;
    static const int MC_KEY_LEN = 6;

    static const char MC_KEY_DEFAULT[MC_KEY_LEN];                       // 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
    static const char MC_KEY_MIFARE_APPLICATION_DIRECTORY[MC_KEY_LEN];  // 0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5
    static const char MC_KEY_NFC_FORUM[MC_KEY_LEN];                     // 0xD3, 0xF7, 0xD3, 0xF7, 0xD3, 0xF7

    // sector 0-32，4 blocks per sector， sector 33-40, 16 blocks per sector
    static const int MC_BLOCK_COUNT = 4;
    static const int MC_BLOCK_COUNT_OF_4K = 16;

    // 5 sectors per tag， 4 blocks per sector
    static const int MC_SIZE_MINI = 320;
    static const int MC_SECTOR_COUNT_OF_SIZE_MINI = 5;
    // 16 sectors per tag， 4 blocks per sector
    static const int MC_SIZE_1K = 1024;
    static const int MC_SECTOR_COUNT_OF_SIZE_1K = 16;
    // 32 sectors per tag， 4 blocks per sector
    static const int MC_SIZE_2K = 2048;
    static const int MC_SECTOR_COUNT_OF_SIZE_2K = 32;
    // 40 sectors per tag， 4 blocks per sector
    static const int MC_SIZE_4K = 4096;
    static const int MC_MAX_SECTOR_COUNT = 40;

public:
    explicit MifareClassicTag(std::weak_ptr<Tag> tag);
    ~MifareClassicTag() {}

    /**
     * @Description Get an object of MifareClassicTag for the given tag.
     * @param tag compatible with all types of tag
     * @return std::shared_ptr<MifareClassicTag>
     */
    static std::shared_ptr<MifareClassicTag> GetTag(std::weak_ptr<Tag> tag);
    /**
     * @Description Authenticate a sector with the key.Only successful authentication sector can be operated.
     * @param sectorIndex Index of sector to authenticate
     * @param key key(6-byte) to authenticate
     * @param bIsKeyA KeyA flag. true means KeyA, otherwise KeyB
     * @return Result of authenticattion. if return ture, means successful.
     */
    bool AuthenticateSector(int sectorIndex, const std::string& key, bool bIsKeyA);
    /**
     * @Description Read a block
     * @param blockIndex index of block to read
     * @return the block data
     */
    std::string ReadSingleBlock(int blockIndex);
    /**
     * @Description Write a block
     * @param blockIndex index of block to write
     * @param data block data to write
     * @return Errorcode of write. if return 0, means successful.
     */
    int WriteSingleBlock(int blockIndex, const std::string& data);
    /**
     * @Description Increment a value block
     * @param blockIndex index of block to increment
     * @param value value to increment, none-negative
     * @return Errorcode of increment. if return 0, means successful.
     */
    int IncrementBlock(int blockIndex, int value);
    /**
     * @Description Decrement a value block
     * @param blockIndex index of block to decrement
     * @param value value to increment, none-negative
     * @return Errorcode of decrement. if return 0, means successful.
     */
    int DecrementBlock(int blockIndex, int value);
    /**
     * @Description Copy from the value of register to the value block
     * @param blockIndex index of value block to copy to
     * @return Errorcode of operation. if return 0, means successful.
     */
    int TransferToBlock(int blockIndex);
    /**
     * @Description Copy from the value block to the register
     * @param blockIndex index of value block to copy from
     * @return Errorcode of operation. if return 0, means successful.
     */
    int RestoreFromBlock(int blockIndex);
    /**
     * @Description Get the number of sectors in mifareclassic tag
     * @param void
     * @return the number of sectors.
     */
    int GetSectorCount() const;
    /**
     * @Description Get the number of blocks in the sector.
     * @param sectorIndex index of sector
     * @return the number of blocks.
     */
    int GetBlockCountInSector(int sectorIndex) const;
    /**
     * @Description Get the type of the MifareClassic tag in bytes.
     * @param void
     * @return type of MifareClassic tag.
     */
    size_t GetMifareTagType() const;
    /**
     * @Description Get size of the tag in bytes.
     * @param void
     * @return size of the tag
     */
    int GetSize() const;
    /**
     * @Description check if if tag is emulated
     * @param void
     * @return return true if tag is emulated, otherwise return false.
     */
    bool IsEmulated() const;
    /**
     * @Description Get the first block of the sector.
     * @param sectorIndex index of sector
     * @return index of first block in the sector
     */
    int GetBlockIdexFromSector(int sectorIndex) const;

private:
    size_t mMifareTagType_{};
    int mSize_{};
    bool mIsEmulated_{};
};
}  // namespace sdk
}  // namespace nfc
}  // namespace OHOS
#endif  // !MIFARE_CLASSIC_TAG_H
