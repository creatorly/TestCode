
#ifndef WIRELESS_DEBUG_H
#define WIRELESS_DEBUG_H

/* 2009.11.23 pengjian added */
#include <utils/ut_log.h>

#define DEBUG_MODULE_WIRELESS       "SLIC"

#define DEBUG_UNIT_WIRELESS_ERROR   "ERROR"
#define DEBUG_UNIT_WIRELESS_WARNING "WARN"
#define DEBUG_UNIT_WIRELESS_INFO    "INFO"

#define DEBUG_LEVEL_WIRELESS_ERROR      UT_LOG_LEVEL_ERROR
#define DEBUG_LEVEL_WIRELESS_WARNING    UT_LOG_LEVEL_WARN
#define DEBUG_LEVEL_WIRELESS_EVENT      UT_LOG_LEVEL_EVENT
#define DEBUG_LEVEL_WIRELESS_INFO       UT_LOG_LEVEL_INFO

#define WIRELESS_ERRORNO                  -1

#define WIRELESS_DEBUG_ERROR            0x00000001
#define WIRELESS_DEBUG_WARNING          0x00000002
#define WIRELESS_DEBUG_INFO             0x00000004
#define WIRELESS_DEBUG_STATE            0x00000008
#define WIRELESS_DEBUG_SEND             0x00000010
#define WIRELESS_DEBUG_RECV             0x00000020
#define WIRELESS_DEBUG_ENTER            0x00000040
#define WIRELESS_DEBUG_LEAVE            0x00000080
#define WIRELESS_DEBUG_TRACE            0x00000100
#define WIRELESS_DEBUG_STAT             0x00000200
#define WIRELESS_DEBUG_ASSERT           0x00000400

#define WIRELESS_DEBUG_NONE             0
#define WIRELESS_DEBUG_ALL              (~0)
#define WIRELESS_DEBUG_DEFMASK          WIRELESS_DEBUG_ALL

#define WIRELESS_DEBUG_MASK             WIRELESS_DEBUG_DEFMASK
#define WIRELESS_EOS_TRACE_INCLUDE      EOS_TRUE

#if 1
#define WIRELESS_LOG(lv, code, fmt, arg...) \
            UT_LogOut(2, __FILE__, __LINE__, lv, code, 256, fmt, ##arg)

#define WIRELESS_LLOG(lv, code, fmt, arg...) \
            UT_LogOut(2, __FILE__, __LINE__, lv, code, 2048, fmt, ##arg)

#if (WIRELESS_DEBUG_MASK & WIRELESS_DEBUG_ERROR) 
#define WIRELESS_ERROR(code, fmt, arg...) WIRELESS_LOG(DEBUG_LEVEL_WIRELESS_ERROR, code, fmt, ##arg)

#else
#define WIRELESS_ERROR(code, fmt, arg...)
#endif

#if (WIRELESS_DEBUG_MASK & WIRELESS_DEBUG_WARNING) 
#define WIRELESS_WARN(code, fmt, arg...) WIRELESS_LOG(DEBUG_LEVEL_WIRELESS_WARNING, code, fmt, ##arg)
#else
#define WIRELESS_WARN(code, fmt, arg...)
#endif

#if (WIRELESS_DEBUG_MASK & WIRELESS_DEBUG_WARNING) 
#define WIRELESS_EVENT(fmt, arg...) WIRELESS_LOG(DEBUG_LEVEL_WIRELESS_EVENT, 0, fmt, ##arg)
#else
#define WIRELESS_EVENT(fmt, arg...)
#endif

#if (WIRELESS_DEBUG_MASK & WIRELESS_DEBUG_INFO) 
#define WIRELESS_INFO(fmt, arg...)  WIRELESS_LOG(DEBUG_LEVEL_WIRELESS_INFO, 0, fmt, ##arg)
#define WIRELESS_LINFO(fmt, arg...) WIRELESS_LLOG(DEBUG_LEVEL_WIRELESS_INFO, 0, fmt, ##arg)
#else
#define WIRELESS_INFO(fmt, arg...)
#define WIRELESS_LINFO(fmt, arg...)
#endif

#else
#define WIRELESS_LOG(lv, code, fmt, arg...) printf(fmt, ##arg)
#define WIRELESS_LLOG(lv, code, fmt, arg...) printf(fmt, ##arg)
#define WIRELESS_ERROR(code, fmt, arg...) printf(fmt, ##arg)
#define WIRELESS_WARN(code, fmt, arg...) printf(fmt, ##arg)
#define WIRELESS_EVENT(fmt, arg...) WIRELESS_LOG(DEBUG_LEVEL_WIRELESS_EVENT, 0, fmt, ##arg)
#define WIRELESS_INFO(fmt, arg...) printf(fmt, ##arg)
#endif

#if (WIRELESS_DEBUG_MASK & WIRELESS_DEBUG_ASSERT) 
#define WIRELESS_ASSERT(cond) \
do{ \
    if (!(cond)) \
        printf("%s %d: assert failed!\r\n", __FILE__, __LINE__); \
}while(0)
#else
#define WIRELESS_ASSERT(cond)
#endif

#define WIRELESS_CONDCHK(cond, errcode, prompt) \
do{ \
    if (!(cond)) \
    { \
        if (NULL != (prompt)) \
            WIRELESS_ERROR((_UINT32)(errcode), (_CHAR8*)(prompt)); \
        return errcode; \
    } \
}while(0)

#define WIRELESS_NPCHK(ptr, err) \
    WIRELESS_CONDCHK(ptr, err, "null pointer passed!\r\n")
    
#define WIRELESS_MPCHK(ptr, err) \
    WIRELESS_CONDCHK(ptr, err, "failed to allocate memory!\r\n")

#define WIRELESS_TRACE        printf("(%s)%d\r\n", __FILE__, __LINE__)
#define WIRELESS_Sprintf sprintf

#define WRL_PRINTF(format, arg...) utl_log_printf(NULL, format, ##arg)

#endif
