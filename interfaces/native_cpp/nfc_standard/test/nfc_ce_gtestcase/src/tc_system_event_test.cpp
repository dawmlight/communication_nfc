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
#include <gtest/gtest.h>

#include "bundle_changed_broadcast.hpp"
#include "nfc_agent.h"
#include "setting_broadcast.hpp"

using namespace OHOS::nfc::test;
namespace OHOS::nfc::test {
/**
 * @tc.number    : NFC_TC_System_Event_SettingBroadcast_001_001
 * @tc.name      : set primary service envent
 * @tc.desc      :
 */
TEST(SettingBroadcast, notify)
{
    SettingBroadcast::NotifyPrimaryServiceChanged(OHOS::AppExecFwk::ElementName("", "", "ability_hs_1"));
}

/**
 * @tc.number    : NFC_TC_System_Event_BundleChangedBroadcast_001_001
 * @tc.name      : install service envent
 * @tc.desc      :
 */
TEST(BundleChangedBroadcast, notify_install)
{
    BundleChangedBroadcast::NotifyPrimaryServiceChanged(true, "bundle1");
}
/**
 * @tc.number    : NFC_TC_System_Event_BundleChangedBroadcast_001_002
 * @tc.name      : uninstall service envent
 * @tc.desc      :
 */
TEST(BundleChangedBroadcast, notify_uninstall)
{
    BundleChangedBroadcast::NotifyPrimaryServiceChanged(false, "bundle1");
}
}  // namespace OHOS::nfc::test
