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

#ifndef loghelper_h
#define loghelper_h

#ifdef USE_HILOG
#include "hilog/log_c.h"
#include "hilog/log_cpp.h"

#define DEFINE_NFC_LOG_LABEL(name) static constexpr OHOS::HiviewDFX::HiLogLabel NFC_LOG_LABEL = {LOG_CORE, 9999, name};

#define __SRC_FILE_NAME__ (__builtin_strrchr(__FILE__, '/') ? __builtin_strrchr(__FILE__, '/') + 1 : __FILE__)

#define ErrorLog(format, ...)                                             \
    (void)OHOS::HiviewDFX::HiLog::Error(NFC_LOG_LABEL,                    \
                                        "[%{public}s:%{public}d]" format, \
                                        __SRC_FILE_NAME__,                \
                                        __LINE__,                         \
                                        ##__VA_ARGS__)
#define WarnLog(format, ...)                                             \
    (void)OHOS::HiviewDFX::HiLog::Warn(NFC_LOG_LABEL,                    \
                                       "[%{public}s:%{public}d]" format, \
                                       __SRC_FILE_NAME__,                \
                                       __LINE__,                         \
                                       ##__VA_ARGS__)
#define InfoLog(format, ...)                                             \
    (void)OHOS::HiviewDFX::HiLog::Info(NFC_LOG_LABEL,                    \
                                       "[%{public}s:%{public}d]" format, \
                                       __SRC_FILE_NAME__,                \
                                       __LINE__,                         \
                                       ##__VA_ARGS__)
#ifdef DEBUG
#define DebugLog(format, ...)                                            \
    (void)OHOS::HiviewDFX::HiLog::Info(NFC_LOG_LABEL,                    \
                                       "[%{public}s:%{public}d]" format, \
                                       __SRC_FILE_NAME__,                \
                                       __LINE__,                         \
                                       ##__VA_ARGS__)
#else

#define DebugLog(format, ...)                                             \
    (void)OHOS::HiviewDFX::HiLog::Debug(NFC_LOG_LABEL,                    \
                                        "[%{public}s:%{public}d]" format, \
                                        __SRC_FILE_NAME__,                \
                                        __LINE__,                         \
                                        ##__VA_ARGS__)
#endif
#else

#ifdef DEBUG
#include <stdio.h>
#include <sys/syscall.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#define GET_TIME()                                                                                          \
    {                                                                                                       \
        struct timeval time;                                                                                \
        gettimeofday(&time, NULL);                                                                          \
        printf("%ld%ld [0x%X]", time.tv_sec, time.tv_usec, static_cast<unsigned int>(syscall(SYS_gettid))); \
    }

#define InfoLog(format, ...)                                                       \
    {                                                                              \
        GET_TIME();                                                                \
        printf(" " __FILE__ "(%05d) INFO: " format "\n", __LINE__, ##__VA_ARGS__); \
    }
#define DebugLog(format, ...)                                                       \
    {                                                                               \
        GET_TIME();                                                                 \
        printf(" " __FILE__ "(%05d) DEBUG: " format "\n", __LINE__, ##__VA_ARGS__); \
    }
#define WarnLog(format, ...)                                                       \
    {                                                                              \
        GET_TIME();                                                                \
        printf(" " __FILE__ "(%05d) WARN: " format "\n", __LINE__, ##__VA_ARGS__); \
    }
#define ErrorLog(format, ...)                                                       \
    {                                                                               \
        GET_TIME();                                                                 \
        printf(" " __FILE__ "(%05d) ERROR: " format "\n", __LINE__, ##__VA_ARGS__); \
    }
#else
#define InfoLog(format, ...)
#define DebugLog(format, ...)
#define WarnLog(format, ...)
#define ErrorLog(format, ...)
#endif

#endif
#endif  // loghelper_h
