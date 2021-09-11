#ifndef _loghelper_h_
#define _loghelper_h_

#ifdef DEBUG
#include <stdio.h>
#include <time.h>

// 获取微秒级时间
#define GET_TIME()                                               \
    {                                                            \
        struct timespec xTime;                                   \
        clock_gettime(CLOCK_REALTIME, &xTime);                   \
        printf("%ld%06ld ", xTime.tv_sec, xTime.tv_nsec / 1000); \
    }

#define InfoLog(format, ...)                                                   \
    {                                                                          \
        GET_TIME();                                                            \
        printf(__FILE__ "(%05d) INFO: " format "\n", __LINE__, ##__VA_ARGS__); \
    }
#define DebugLog(format, ...)                                                   \
    {                                                                           \
        GET_TIME();                                                             \
        printf(__FILE__ "(%05d) DEBUG: " format "\n", __LINE__, ##__VA_ARGS__); \
    }
#define WarnLog(format, ...)                                                   \
    {                                                                          \
        GET_TIME();                                                            \
        printf(__FILE__ "(%05d) WARN: " format "\n", __LINE__, ##__VA_ARGS__); \
    }
#define ErrorLog(format, ...)                                                   \
    {                                                                           \
        GET_TIME();                                                             \
        printf(__FILE__ "(%05d) ERROR: " format "\n", __LINE__, ##__VA_ARGS__); \
    }
#else
#define InfoLog(format, ...)
#define DebugLog(format, ...)
#define WarnLog(format, ...)
#define ErrorLog(format, ...)
#endif

#endif  // _loghelper_h_
