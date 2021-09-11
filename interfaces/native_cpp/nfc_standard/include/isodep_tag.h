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
#ifndef ISODEP_TAG_H
#define ISODEP_TAG_H

#include "basic_tag_session.h"

namespace OHOS {
namespace nfc {
namespace sdk {
class Tag;
class IsoDepTag final : public BasicTagSession {
public:
    explicit IsoDepTag(std::weak_ptr<Tag> tag);
    ~IsoDepTag() {}

    /**
     * @Description Get an object of IsoDep for the given tag. It corresponding T1T(14443A-3) and T4T(14443A-4,
     * 14443B-4(Type-B)) defined by NFC Forum.
     * @param tag compatible with all types of tag
     * @return std::shared_ptr<IsoDepTag>
     */
    static std::shared_ptr<IsoDepTag> GetTag(std::weak_ptr<Tag> tag);
    /**
     * @Description Get Historical bytes of the tag.
     * @param void
     * @return Historical bytes
     */
    std::string GetHistoricalBytes() const;
    /**
     * @Description Get HiLayerResponse bytes of the tag.
     * @param void
     * @return HiLayerResponse bytes
     */
    std::string GetHiLayerResponse() const;

    // ISO 14443-3As
    /**
     * @Description Get SAK bytes of the tag.
     * @param void
     * @return SAK bytes
     */
    int GetSak() const;
    /**
     * @Description Get ATQA bytes of the tag.
     * @param void
     * @return ATQA bytes
     */
    std::string GetAtqa() const;

    // ISO 14443-3B
    /**
     * @Description Get AppData bytes of the tag.
     * @param void
     * @return AppData bytes
     */
    std::string GetAppData() const;
    /**
     * @Description Get ProtocolInfo bytes of the tag.
     * @param void
     * @return ProtocolInfo bytes
     */
    std::string GetProtocolInfo() const;

private:
    std::string mHistoricalBytes_{};
    std::string mHiLayerResponse_{};

    // ISO 14443-3A
    int mSak;
    std::string mAtqa_{};

    // ISO 14443-3B
    std::string mAppData_{};
    std::string mProtocolInfo_{};
};
}  // namespace sdk
}  // namespace nfc
}  // namespace OHOS
#endif  // ISODEP_TAG_H
