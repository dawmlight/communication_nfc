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

#include "card_emulation_manager.h"

#include <gtest/gtest.h>

#include "card_emulation_error.h"
#include "nfc_service.h"

using namespace OHOS::nfc::cardemulation;
using namespace OHOS::nfc;
namespace OHOS::nfc::cardemulation::test{
/**
* @tc.number:
* @tc.name  :
* @tc.desc  :
*/
TEST(CardEmulationManager, Init_Failed)
{
    std::shared_ptr<NfcService> nfcService;
    auto cem = std::make_shared<CardEmulationManager>(nfcService);
    EXPECT_EQ(cem->Init(), ERR_OK);

    cem->OnHostCardEmulationData(-1, {});
    cem->OnOffHostTransaction();
    cem->OnNfcDisabled();
    cem->OnNfcEnabled();
    cem->OnSecureNfcToggled();
    cem->Deinit();
}
}