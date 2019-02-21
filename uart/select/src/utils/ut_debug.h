
#ifndef __UT_DEBUG_H__
#define __UT_DEBUG_H__

#include "ut_log.h"

typedef enum
{
    EN_ASYS_MODULE_NVM = 0,
	EN_ASYS_MODULE_UT,
	EN_ASYS_MODULE_BUTT
}EN_ASYS_MODULE;

#define DEBUG_MODULE_UT        "UT"

#define DEBUG_UNIT_UT_ERROR    "ERROR"
#define DEBUG_UNIT_UT_WARN     "WARN"
#define DEBUG_UNIT_UT_INFO     "INFO"

#define DEBUG_LEVEL_UT_ERROR   UT_LOG_LEVEL_ERROR
#define DEBUG_LEVEL_UT_WARN    UT_LOG_LEVEL_WARN
#define DEBUG_LEVEL_UT_INFO    UT_LOG_LEVEL_INFO

#define UT_DEBUG_ERROR         0x00000001
#define UT_DEBUG_WARNING       0x00000002
#define UT_DEBUG_INFO          0x00000004
#define UT_DEBUG_STATE         0x00000008
#define UT_DEBUG_SEND          0x00000010
#define UT_DEBUG_RECV          0x00000020
#define UT_DEBUG_ENTER         0x00000040
#define UT_DEBUG_LEAVE         0x00000080
#define UT_DEBUG_TRACE         0x00000100
#define UT_DEBUG_STAT          0x00000200
#define UT_DEBUG_ASSERT        0x00000400

#define UT_DEBUG_NONE          0
#define UT_DEBUG_ALL           (~0)
#define UT_DEBUG_DEFMASK       UT_DEBUG_ALL

#define UT_DEBUG_MASK          UT_DEBUG_DEFMASK
#define UT_EOS_TRACE_INCLUDE   EOS_TRUE

#define UT_NOARG   "%s", ""

#define UT_NLOG(n, l, c, i, m, fmt, args...) \
            UT_LogOutEx(EN_ASYS_MODULE_UT, n, __FILE__, __LINE__, l, c, i, m, fmt, ##args)

#define UT_LOG(l, c, i, m, fmt, args...) \
            UT_NLOG(256, l, c, i, m, fmt, ##args)
#define UT_LLOG(l, c, i, m, fmt, args...) \
            UT_NLOG(2048, l, c, i, m, fmt, ##args)

#if (UT_DEBUG_MASK & UT_DEBUG_ERROR) 
#define UT_ERR(idx, code, msg, fmt, args...) \
            UT_LOG(DEBUG_LEVEL_UT_ERROR, code, idx, msg, fmt, ##args)
#else
#define UT_ERR(idx, code, msg, fmt, args...)
#endif
#define UT_ERROR(code, fmt, args...)    UT_ERR(-1, code, NULL, fmt, ##args)

#if (UT_DEBUG_MASK & UT_DEBUG_WARNING) 
#define UT_WARN(idx, code, msg, fmt, args...) \
            UT_LOG(DEBUG_LEVEL_UT_WARN, code, idx, msg, fmt, ##args)
#else
#define UT_WARN(idx, code, msg, fmt, args...)
#endif

#if (UT_DEBUG_MASK & UT_DEBUG_INFO) 
#define UT_INFO(fmt, arg...)        UT_LOG(DEBUG_LEVEL_UT_INFO, 0, -1, NULL, fmt, ##arg)
#else
#define UT_INFO(fmt, arg...)
#endif
#define UT_INFO2(fmt2, arg1, arg2)              UT_INFO(fmt2, arg1, arg2)
#define UT_INFO4(fmt4, arg1, arg2, arg3, arg4)  UT_INFO(fmt4, arg1, arg2, arg3, arg4)

#define UT_DBG3(i, m, fmt, args...)     UT_LOG(UT_LOG_LEVEL_DBG3, 0, i, m, fmt, ##args)
#define UT_DBG2(i, m, fmt, args...)     UT_LOG(UT_LOG_LEVEL_DBG2, 0, i, m, fmt, ##args)
#define UT_DBG1(i, m, fmt, args...)     UT_LOG(UT_LOG_LEVEL_DBG1, 0, i, m, fmt, ##args)

#define UT_LDBG3(i, m, fmt, args...)    UT_LLOG(UT_LOG_LEVEL_DBG3, 0, i, m, fmt, ##args)
#define UT_LDBG2(i, m, fmt, args...)    UT_LLOG(UT_LOG_LEVEL_DBG2, 0, i, m, fmt, ##args)
#define UT_LDBG1(i, m, fmt, args...)    UT_LLOG(UT_LOG_LEVEL_DBG1, 0, i, m, fmt, ##args)

#if (UT_DEBUG_MASK & UT_DEBUG_ASSERT) 
#define UT_ASSERT(cond) \
do{ \
    if (!(cond)) \
    { \
        UT_ERROR(EOS_ERROR, "assert (%s) failed!\r\n", #cond); \
    } \
}while(0)
#else
#define UT_ASSERT(cond)
#endif

#define UT_ST_CONSTRUCT(ptr, T) \
do{ \
    ptr = (T*)UT_MALLOC(sizeof(T)); \
    UT_MALLOC_CHK(ptr, EOS_ERROR); \
    eos_memset(ptr, 0, sizeof(T)); \
}while(0)

#define UT_ST_DESTRUCT(ptr) \
do{ \
    eos_memset(ptr, 0, sizeof(T)); \
    UT_FREE(ptr); \
}while(0)

#define UT_CONDCHK(cond, errcode, prompt) \
do{ \
    if (!(cond)) \
    { \
        if (NULL != (prompt)) \
        { \
            UT_ERROR((_UINT32)(errcode), (_CHAR8*)(prompt)); \
        } \
        return errcode; \
    } \
}while(0)

#define UT_NULLPTR_CHK(ptr, err) \
            UT_CONDCHK(ptr, err, "null pointer passed!\r\n")
            
#define UT_MALLOC_CHK(ptr, err) \
            UT_CONDCHK(ptr, err, "failed to allocate memory!\r\n")

#define UT_TRACE \
    printf("%s:%d\r\n", __FILE__, __LINE__)

#define UT_Sprintf sprintf

#endif

