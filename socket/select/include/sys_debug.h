
#ifndef __SYS_DEBUG_H__
#define __SYS_DEBUG_H__

#include <utils/ut_log.h>

#define DEBUG_MODULE_SYS        "SYS"

#define DEBUG_UNIT_SYS_ERROR    "ERROR"
#define DEBUG_UNIT_SYS_WARN     "WARN"
#define DEBUG_UNIT_SYS_INFO     "INFO"

#define DEBUG_LEVEL_SYS_ERROR   UT_LOG_LEVEL_ERROR
#define DEBUG_LEVEL_SYS_WARN    UT_LOG_LEVEL_WARN
#define DEBUG_LEVEL_SYS_INFO    UT_LOG_LEVEL_INFO

#define SYS_DEBUG_ERROR         0x00000001
#define SYS_DEBUG_WARNING       0x00000002
#define SYS_DEBUG_INFO          0x00000004
#define SYS_DEBUG_STATE         0x00000008
#define SYS_DEBUG_SEND          0x00000010
#define SYS_DEBUG_RECV          0x00000020
#define SYS_DEBUG_ENTER         0x00000040
#define SYS_DEBUG_LEAVE         0x00000080
#define SYS_DEBUG_TRACE         0x00000100
#define SYS_DEBUG_STAT          0x00000200
#define SYS_DEBUG_ASSERT        0x00000400

#define SYS_DEBUG_NONE          0
#define SYS_DEBUG_ALL           (~0)
#define SYS_DEBUG_DEFMASK       SYS_DEBUG_ALL

#define SYS_DEBUG_MASK          SYS_DEBUG_DEFMASK
#define SYS_EOS_TRACE_INCLUDE   EOS_TRUE

#define SYS_LOG(lv, code, fmt, arg...) \
            UT_LogOut(0, __FILE__, __LINE__, lv, code, 256, fmt, ##arg)

#if (SYS_DEBUG_MASK & SYS_DEBUG_ERROR) 
#define SYS_ERROR(code, fmt, arg...) SYS_LOG(DEBUG_LEVEL_SYS_ERROR, code, fmt, ##arg)
#else
#define SYS_ERROR(code, fmt, arg...)
#endif

#if (SYS_DEBUG_MASK & SYS_DEBUG_WARNING) 
#define SYS_WARN(code, fmt, arg...) SYS_LOG(DEBUG_LEVEL_SYS_WARN, code, fmt, ##arg)
#else
#define SYS_WARN(code, fmt, arg...)
#endif

#if (SYS_DEBUG_MASK & SYS_DEBUG_INFO) 
#define SYS_INFO(fmt, arg...) SYS_LOG(DEBUG_LEVEL_SYS_INFO, 0, fmt, ##arg)
#else
#define SYS_INFO(fmt, arg...)
#endif

#if (SYS_DEBUG_MASK & SYS_DEBUG_ASSERT) 
#define SYS_ASSERT(cond) \
do{ \
    if (!(cond)) \
    { \
        SYS_ERROR(EOS_ERROR, "assert (%s) failed!\r\n", #cond); \
    } \
}while(0)
#else
#define SYS_ASSERT(cond)
#endif

#define SYS_CONDCHK(cond, errcode, prompt) \
do{ \
    if (!(cond)) \
    { \
        if (NULL != (prompt)) \
        { \
            SYS_ERROR((_UINT32)(errcode), (_CHAR8*)(prompt)); \
        } \
        return errcode; \
    } \
}while(0)

#define SYS_NPCHK(ptr, err) \
    SYS_CONDCHK(ptr, err, "null pointer passed!\r\n")
    
#define SYS_MPCHK(ptr, err) \
    SYS_CONDCHK(ptr, err, "failed to allocate memory!\r\n")

#define SYS_Sprintf sprintf

#ifdef ANDROID
#include <android/log.h>
#define SYS_PRINTF(fmt, args...) \
            __android_log_print(ANDROID_LOG_DEBUG, "SYS", fmt, ##args);
#else
#define SYS_PRINTF(fmt, args...) printf(fmt, ##args)
#endif

#define SYS_Sprintf sprintf

#define SYS_TRACE \
    printf("%s:%d\r\n", __FILE__, __LINE__)

#endif

