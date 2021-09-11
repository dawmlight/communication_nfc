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

#ifndef INFC_EVENT_HANDLER_H
#define INFC_EVENT_HANDLER_H
namespace OHOS::nfc::cardemulation {
class INfcEventHandler {
public:
    virtual ~INfcEventHandler() = default;
    /**
     * brief: Handle NFC enabled events
     * parameter: void
     * return: void
     */
    virtual void OnNfcEnabled() = 0;
    /**
     * brief: Handle NFC disabled events
     * parameter: void
     * return: void
     */
    virtual void OnNfcDisabled() = 0;
    /**
     * brief: Handle secure NFC toggled events
     * parameter: void
     * return: void
     */
    virtual void OnSecureNfcToggled() = 0;
private:
};
}
#endif  // !INFC_EVENT_HANDLER_H
