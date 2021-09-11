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
#ifndef TAG_SESSION_PROXY_H
#define TAG_SESSION_PROXY_H

#include <memory>

#include "errors.h"
#include "iapp_callback.h"
#include "iremote_proxy.h"

namespace OHOS {
namespace nfc {
namespace sdk {
class Tag;

class AppCallbackProxy final : public OHOS::IRemoteProxy<IAppCallback> {
public:
    explicit AppCallbackProxy(const OHOS::sptr<OHOS::IRemoteObject>& remote) : OHOS::IRemoteProxy<IAppCallback>(remote)
    {
    }
    ~AppCallbackProxy() {}

    void OnTagDiscovered(std::unique_ptr<Tag> tag) override;

private:
    static constexpr int APP_CALLBACK_START_ID = 300;
    static constexpr int COMMAND_TAG_DISCOVERED = APP_CALLBACK_START_ID + 0;
};

class TagRemovedCallbackProxy final : public OHOS::IRemoteProxy<ITagRemovedCallback> {
public:
    explicit TagRemovedCallbackProxy(const OHOS::sptr<OHOS::IRemoteObject>& remote)
        : OHOS::IRemoteProxy<ITagRemovedCallback>(remote)
    {
    }
    virtual ~TagRemovedCallbackProxy() {}

    void OnTagRemoved() override;

private:
    static constexpr int TAG_REMOVED_CALLBACK_START_ID = 400;
    static constexpr int COMMAND_TAG_REMOVED = TAG_REMOVED_CALLBACK_START_ID + 0;
};
}  // namespace sdk
}  // namespace nfc
}  // namespace OHOS
#endif  // !TAG_SESSION_PROXY_H