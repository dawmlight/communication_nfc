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

package ohos.nfc;

/**
 * Defines error codes that can be returned by the service so that an exception is thrown on the
 * application level.
 *
 * @hide
 * @since 3
 */
public class NfcErrCodes {
    /**
     * Indicates the keyword used to describe the success result.
     *
     * @since 3
     * @hide
     */
    public static final int SUCCESS = 0;

    /**
     * Indicates the keyword used to describe an IO error.
     *
     * @since 3
     * @hide
     */
    public static final int ERROR_IO = -1;

    /**
     * Indicates the keyword used to describe an invalid param.
     *
     * @since 3
     * @hide
     */
    public static final int ERROR_INVALID_PARAM = -8;

    /**
     * Indicates whether a returned error code is an error or not.
     *
     * @returns {@code true} if the error code indicates an error; returns {@code false} otherwise.
     * @since 3
     */
    public static boolean isError(int code) {
        return code < 0;
    }
}