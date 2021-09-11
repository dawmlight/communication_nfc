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

#ifndef ROUTING_FILTER_H
#define ROUTING_FILTER_H

#include <algorithm>
#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "aid_set.h"
#include "card_emulation_service_info.h"
#include "card_emulation_util.h"
namespace OHOS::nfc::cardemulation {
class IRoutingFilter {
public:
    virtual ~IRoutingFilter(){};
    virtual bool Allow(const std::shared_ptr<AidSet>& aidset) = 0;
};

struct LocationFilter : public IRoutingFilter {
    explicit LocationFilter(std::vector<std::string> supportedLocation);
    virtual ~LocationFilter(){};
    bool Allow(const std::shared_ptr<AidSet>& aidset) override;

private:
    std::vector<std::string> supportedLocation_;
};

struct TagFilter : public IRoutingFilter {
    explicit TagFilter(std::function<bool(const std::string&)> isTagAllowed);
    virtual ~TagFilter(){};
    bool Allow(const std::shared_ptr<AidSet>& aidset) override;

private:
    std::function<bool(const std::string&)> isTagAllowed_;
};

struct AidFilter : public IRoutingFilter {
    explicit AidFilter(std::function<bool(const std::shared_ptr<AidSet>&)> isAidAllowed);
    virtual ~AidFilter(){};
    bool Allow(const std::shared_ptr<AidSet>& aidset) override;

private:
    std::function<bool(const std::shared_ptr<AidSet>&)> isAidAllowed_;
};

struct ModeFilter : public IRoutingFilter {
    explicit ModeFilter(int mode);
    virtual ~ModeFilter(){};
    bool Allow(const std::shared_ptr<AidSet>& aidset) override;

private:
    bool SupportedPrefix() const;
    bool SupportedSubset() const;

private:
    int mode_;
};
}  // namespace OHOS::nfc::cardemulation
#endif  // ROUTING_FILTER_H
