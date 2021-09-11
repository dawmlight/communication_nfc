/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
 * Copyright (C) 2006 The Android Open Source Project
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

package ohos.nfc;

/**
 * NFC os adapter function for NFC kit
 *
 * @hide
 */
public class NfcAdapterUtils {
    private static final String SEARCH_STRING = "ohos";
    private static final String REPLACE_STRING = "android";

    /**
     * replace string from ohos to android
     *
     * @param inStr Indicates the string that will be replaced.
     * @return return the string after replaced.
     * @hide
     */
    public static String stringReplace(String inStr) {
        if (inStr == null) {
            return "";
        }
        return inStr.replace(SEARCH_STRING, REPLACE_STRING);
    }
}