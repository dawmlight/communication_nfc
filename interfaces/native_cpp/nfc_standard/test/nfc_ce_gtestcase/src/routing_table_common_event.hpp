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
#ifndef _ROUTING_TABLE_COMMON_EVENT_H_
#define _ROUTING_TABLE_COMMON_EVENT_H_

#include <algorithm>
#include <condition_variable>
#include <cstdio>
#include <memory>
#include <mutex>
#include <sstream>

#include "common_event_manager.h"
#include "common_event_subscriber.h"
#include "loghelper.h"
namespace OHOS::nfc::test {
static bool StringEqual(const std::string& s1, const std::string& s2, bool ignoreCase)
{
    if (ignoreCase) {
        char const* p1 = s1.c_str();
        char const* p2 = s2.c_str();
        while (*p1 && *p2) {
            if (toupper(*p1++) != toupper(*p2++)) {
                return false;
            }
        }
        if (*p1 != '\0' || *p2 != '\0') {
            return false;
        }
        return true;
    }
    return s1 == s2;
}
static std::string VectorStringFormat(const std::vector<std::string>& a)
{
    std::stringstream ss;
    std::for_each(a.begin(), a.end(), [&ss](const std::string& r) { ss << r << " "; });
    return ss.str();
}
static bool StringsAInB(const std::vector<std::string>& a, const std::vector<std::string>& b)
{
    if (a.empty()) {
        return false;
    }
    return std::all_of(a.begin(), a.end(), [&b](const std::string& a0) {
        bool found =
            std::any_of(b.begin(), b.end(), [&a0](const std::string& b0) { return (StringEqual(a0, b0, true)); });
        if (!found) {
            printf("not found: %s\n", a0.c_str());
        }
        return found;
    });
}
static bool StringsNotAInB(const std::vector<std::string>& a, const std::vector<std::string>& b)
{
    if (a.empty()) {
        return false;
    }
    return std::all_of(a.begin(), a.end(), [&b](const std::string& a0) {
        bool found =
            std::any_of(b.begin(), b.end(), [&a0](const std::string& b0) { return StringEqual(a0, b0, true); });
        if (found) {
            printf("found: %s\n", a0.c_str());
        }
        return !found;
    });
}
class RoutingChangedSubscriber : public OHOS::EventFwk::CommonEventSubscriber {
public:
    static constexpr int RESULT_OK = 0;
    static constexpr int RESULT_TIMEOUT = -1;
    static constexpr int RESULT_NOK = 1;
    static std::shared_ptr<RoutingChangedSubscriber> SubscribeRoutingChanged()
    {
        static std::shared_ptr<RoutingChangedSubscriber> subscriber;
        OHOS::EventFwk::MatchingSkills matchSkill;
        matchSkill.AddEvent(RoutingChangedSubscriber::ACTION_AID_ROUTING_CHANGED);
        OHOS::EventFwk::CommonEventSubscribeInfo info(matchSkill);

        subscriber = std::make_shared<RoutingChangedSubscriber>(info);

        bool rv = OHOS::EventFwk::CommonEventManager::SubscribeCommonEvent(subscriber);
        DebugLog("subscribe routing change event result: %s\n", rv ? "succeeded" : "failed");
        return subscriber;
    }

    explicit RoutingChangedSubscriber(const OHOS::EventFwk::CommonEventSubscribeInfo& info)
        : OHOS::EventFwk::CommonEventSubscriber(info)
    {
    }
    int Expect(const std::vector<std::string>& expectedAids, const std::vector<std::string>& excludingAids)
    {
        std::vector<std::string> table;
        int rv = WaitingNotify(table);
        if (rv == RESULT_TIMEOUT) {
            return rv;
        }
        DebugLog("\nexpected: %s\nexcluding: %s\nnotified: %s\n",
                 VectorStringFormat(expectedAids).c_str(),
                 VectorStringFormat(excludingAids).c_str(),
                 VectorStringFormat(table).c_str());
        bool allIncluded = StringsAInB(expectedAids, table);
        bool allExcluded = excludingAids.empty() ? true : StringsNotAInB(excludingAids, table);
        return (allIncluded && allExcluded) ? RESULT_OK : RESULT_NOK;
    }
    int ExpectInclude(const std::vector<std::string>& expectedAids)
    {
        std::vector<std::string> table;
        int rv = WaitingNotify(table);
        if (rv == RESULT_TIMEOUT) {
            return rv;
        }

        DebugLog("\nexpected: %s\nnotified: %s\n",
                 VectorStringFormat(expectedAids).c_str(),
                 VectorStringFormat(table).c_str());
        bool allIncluded = StringsAInB(expectedAids, table);
        return allIncluded ? RESULT_OK : RESULT_NOK;
    }
    int ExpectExclude(const std::vector<std::string>& excludingAids)
    {
        std::vector<std::string> table;
        int rv = WaitingNotify(table);
        if (rv == RESULT_TIMEOUT) {
            return rv;
        }

        DebugLog("\nexcluding: %s\nnotified: %s\n",
                 VectorStringFormat(excludingAids).c_str(),
                 VectorStringFormat(table).c_str());
        bool result = StringsNotAInB(excludingAids, table);
        return result ? RESULT_OK : RESULT_NOK;
    }
    void OnReceiveEvent(const OHOS::EventFwk::CommonEventData& data) override
    {
        using namespace OHOS::AAFwk;
        Want want = data.GetWant();
        std::vector<std::string> table = want.GetStringArrayParam(KEY_AID_ROUTING_TABLE);
        std::stringstream ss;
        DebugLog("OnReceiveEvent: ");
        std::for_each(table.begin(), table.end(), [&ss](std::string& r) { ss << r << ", "; });
        printf("%s\n", ss.str().c_str());
        std::lock_guard<std::mutex> locker(mu_);
        routingTable_ = std::move(table);
        notified_ = true;
        cv_.notify_one();
    }

private:
    int WaitingNotify(std::vector<std::string>& table)
    {
        static constexpr int maxTimeoutTimes = 3;

        int timeoutTimes = 0;
        std::unique_lock<std::mutex> locker(mu_);
        while (!notified_ && timeoutTimes < maxTimeoutTimes) {
            if (cv_.wait_for(locker, std::chrono::seconds(2)) == std::cv_status::timeout) {
                timeoutTimes++;
            }
        }
        if (!notified_) {
            return RESULT_TIMEOUT;
        }
        table.swap(routingTable_);
        notified_ = false;
        return RESULT_OK;
    }

private:
    std::mutex mu_;
    std::condition_variable cv_;
    std::vector<std::string> routingTable_;
    bool notified_;
    static const std::string ACTION_AID_ROUTING_CHANGED;
    static const std::string KEY_AID_ROUTING_TABLE;
};
const std::string RoutingChangedSubscriber::ACTION_AID_ROUTING_CHANGED = "ACTION_AID_ROUTING_CHANGED";
const std::string RoutingChangedSubscriber::KEY_AID_ROUTING_TABLE = "KEY_AID_ROUTING_TABLE";
}  // namespace OHOS::nfc::test
#endif  //_ROUTING_TABLE_COMMON_EVENT_H_
