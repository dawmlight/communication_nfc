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
#ifndef NDEF_TAG_H
#define NDEF_TAG_H
#include "basic_tag_session.h"

namespace OHOS {
namespace nfc {
namespace sdk {
class Tag;
class NdefMessage;

class NdefTag final : public BasicTagSession {
public:
    enum EmNfcForumType {
        NFC_FORUM_TYPE_1_OTHER = -1,
        NFC_FORUM_TYPE_1 = 1,
        NFC_FORUM_TYPE_2 = 2,
        NFC_FORUM_TYPE_3 = 3,
        NFC_FORUM_TYPE_4 = 4,
        MIFARE_CLASSIC = 101,
        ICODE_SLI = 102
    };

    const std::string STRING_NFC_FORUM_TYPE_1 = "org.nfcforum.ndef.type1";
    const std::string STRING_NFC_FORUM_TYPE_2 = "org.nfcforum.ndef.type2";
    const std::string STRING_NFC_FORUM_TYPE_3 = "org.nfcforum.ndef.type3";
    const std::string STRING_NFC_FORUM_TYPE_4 = "org.nfcforum.ndef.type4";
    const std::string STRING_MIFARE_CLASSIC = "com.nxp.ndef.mifareclassic";
    const std::string STRING_ICODE_SLI = "com.nxp.ndef.icodesli";

    enum EmNdefTagMode { MODE_INVALID, MODE_READ_ONLY, MODE_READ_WRITE, MODE_UNKNOW };

public:
    explicit NdefTag(std::weak_ptr<Tag> tag);
    ~NdefTag() {}

    /**
     * @Description Get an object of NdefTag for the given tag.
     * @param tag compatible with all types of tag
     * @return std::shared_ptr<NdefTag>
     */
    static std::shared_ptr<NdefTag> GetTag(std::weak_ptr<Tag> tag);
    /**
     * @Description Get the type of the Ndef tag in bytes.
     * @param void
     * @return type of Ndef tag.
     */
    EmNfcForumType GetNdefTagType() const;
    /**
     * @Description Get the mode of the Ndef tag in bytes.(readonly, read/write, unkown)
     * @param void
     * @return mode of Ndef tag.
     */
    EmNdefTagMode GetNdefTagMode() const;
    /**
     * @Description Get the ndef message that was read from ndef tag when tag discovery.
     * @param void
     * @return ndef message.
     */
    std::shared_ptr<NdefMessage> GetCachedNdefMsg() const;
    /**
     * @Description Check ndef tag is writable
     * @param void
     * @return return true if the tag is writable, otherwise return false.
     */
    bool IsNdefWritable() const;
    /**
     * @Description Read ndef tag
     * @param void
     * @return ndef message in tag.
     */
    std::shared_ptr<NdefMessage> ReadNdef();
    /**
     * @Description write ndef tag
     * @param msg ndef message to write
     * @return Errorcode of write. if return 0, means successful.
     */
    int WriteNdef(std::shared_ptr<NdefMessage> msg);
    /**
     * @Description check ndef tag can be set read-only
     * @param void
     * @return return true if the tag can be set readonly, otherwise return false.
     */
    bool IsEnableReadOnly();
    /**
     * @Description set ndef tag read-only
     * @param void
     * @return Errorcode of write. if return 0, means successful.
     */
    int EnableReadOnly();
    /**
     * @Description convert the Nfc forum type into byte array defined in Nfc forum.
     * @param emNfcForumType Nfc forum type of ndef tag
     * @return Nfc forum type byte array
     */
    std::string GetNdefTagTypeString(EmNfcForumType emNfcForumType);

private:
    EmNfcForumType mNfcForumType_{};
    EmNdefTagMode mNdefTagMode_{};
    std::string mNdefMsg_{};
};
}  // namespace sdk
}  // namespace nfc
}  // namespace OHOS

#endif  // !NDEF_TAG_H