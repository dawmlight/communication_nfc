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
#ifndef MIFARE_ULTRALIGHT_TAG_H
#define MIFARE_ULTRALIGHT_TAG_H

#include "basic_tag_session.h"

namespace OHOS {
namespace nfc {
namespace sdk {
class MifareUltralightTag final : public BasicTagSession {
public:
    static const int NXP_MANUFACTURER_ID = 0x04;
    static const int MU_MAX_PAGE_COUNT = 256;
    static const int MU_PAGE_SIZE = 4;

    enum EmMifareUltralightType { TYPE_UNKOWN = -1, TYPE_ULTRALIGHT = 1, TYPE_ULTRALIGHT_C = 2 };

public:
    explicit MifareUltralightTag(std::weak_ptr<Tag> tag);
    ~MifareUltralightTag();

    /**
     * @Description Get an object of MifareUltralightTag for the given tag.
     * @param tag compatible with all types of tag
     * @return std::shared_ptr<MifareUltralightTag>
     */
    static std::shared_ptr<MifareUltralightTag> GetTag(std::weak_ptr<Tag> tag);
    /**
     * @Description Read 4 pages(16 bytes).
     * @param pageIndex index of page to read
     * @return 4 pages data
     */
    std::string ReadMultiplePages(int pageIndex);
    /**
     * @Description Write a page
     * @param pageIndex index of page to write
     * @param data page data to write
     * @return Errorcode of write. if return 0, means successful.
     */
    int WriteSinglePages(int pageIndex, const std::string& data);
    /**
     * @Description Get the type of the MifareUltralight tag in bytes.
     * @param void
     * @return type of MifareUltralight tag.
     */
    EmMifareUltralightType GetType() const;

private:
    EmMifareUltralightType mType_{EmMifareUltralightType::TYPE_UNKOWN};
};
}  // namespace sdk
}  // namespace nfc
}  // namespace OHOS
#endif  // !MIFARE_ULTRALIGHT_TAG_H
