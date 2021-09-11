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
#ifndef NDEF_FORMATABLE_TAG_H
#define NDEF_FORMATABLE_TAG_H
#include "basic_tag_session.h"

namespace OHOS {
namespace nfc {
namespace sdk {
class Tag;
class NdefMessage;

class NdefFormatableTag final : public BasicTagSession {
public:
    explicit NdefFormatableTag(std::weak_ptr<Tag> tag);
    ~NdefFormatableTag() {}

    /**
     * @Description Get an object of NdefFormatableTag for the given tag.
     * @param tag compatible with all types of tag
     * @return std::shared_ptr<NdefFormatableTag>
     */
    static std::shared_ptr<NdefFormatableTag> GetTag(std::weak_ptr<Tag> tag);
    /**
     * @Description format a tag as NDEF tag, then write Ndef message into the Ndef Tag
     * @param firstMessage Ndef message to write while format successful. it can be null, then only format the tag, not
     * write tag
     * @return Errorcode of operation. if return 0, means successful.
     */
    int Format(std::weak_ptr<NdefMessage> firstMessage);
    /**
     * @Description format a tag as NDEF tag, then write Ndef message into the Ndef Tag, then set the tag readonly
     * @param firstMessage Ndef message to write while format successful. it can be null, then only format the tag, not
     * write tag
     * @return Errorcode of operation. if return 0, means successful.
     */
    int FormatReadOnly(std::weak_ptr<NdefMessage> firstMessage);

private:
    int Format(std::weak_ptr<NdefMessage> firstMessage, bool bMakeReadOnly);
};
}  // namespace sdk
}  // namespace nfc
}  // namespace OHOS

#endif  // !NDEF_FORMATABLE_TAG_H