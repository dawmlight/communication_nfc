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
#ifndef I_APP_CALLBACK_H
#define I_APP_CALLBACK_H

#include <memory>

#include "iremote_broker.h"

namespace OHOS {
namespace nfc {
namespace sdk {
class Tag;
class IAppCallback : public OHOS::IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.nfc.sdk.IAppCallback");

    virtual ~IAppCallback() {}
    /**
     * @Description In reader mode, a callback for tag discoved to the foreground ability
     * @param tag compatible with all types of tag
     * @return void
     */
    virtual void OnTagDiscovered(std::unique_ptr<Tag> tag) = 0;

private:
};

class ITagRemovedCallback : public OHOS::IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.nfc.sdk.ITagRemovedCallback");

    virtual ~ITagRemovedCallback() {}
    /**
     * @Description when the tag is ignored, this callback to be invoked to the foreground ability
     * @param void
     * @return void
     */
    virtual void OnTagRemoved() = 0;

private:
};
}  // namespace sdk
}  // namespace nfc
}  // namespace OHOS
#endif  // !I_APP_CALLBACK_H
