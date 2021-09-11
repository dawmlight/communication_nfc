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
#ifndef BASIC_TAG_SESSION_H
#define BASIC_TAG_SESSION_H

#include "tag.h"

namespace OHOS {
namespace nfc {
namespace reader {
class ITagSession;
// class ResResult;
}  // namespace reader
using reader::ITagSession;
// using reader::ResResult;

namespace sdk {
class BasicTagSession {
public:
    explicit BasicTagSession(std::weak_ptr<Tag> tag, int technology);
    virtual ~BasicTagSession() {}

    int Connect();
    bool IsConnect() const;
    int Close();
    std::string GetTagId();
    std::string SendCommand(std::string& data, bool raw, int& response);
    int GetMaxSendCommandLength() const;
    bool IsSupportApduExtended() const;

protected:
    OHOS::sptr<ITagSession> GetTagService() const;
    int GetTagServiceHandle() const;
    void SetConnectTagTech(int tech) const;
    int GetConnectTagTech() const;

private:
    std::weak_ptr<Tag> mTag_;
    int mTagTechnology_;
    bool mIsConnect_;
};
}  // namespace sdk
}  // namespace nfc
}  // namespace OHOS

#endif  // BASIC_TAG_SESSION_H