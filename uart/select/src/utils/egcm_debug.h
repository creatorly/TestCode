
#ifndef EGCM_DEBUG_H
#define EGCM_DEBUG_H

#if defined(__cplusplus)
extern "C" {
#endif

#define DEBUG_MODULE_EGCM       "EGCM"

#define DEBUG_UNIT_EGCM_ERROR   "ERROR"
#define DEBUG_UNIT_EGCM_WARNING "WARNING"
#define DEBUG_UNIT_EGCM_INFO    "INFO"
#define DEBUG_UNIT_EGCM_ASSERT  "ASSERT"

#define DEBUG_LEVEL_EGCM_ERROR      0
#define DEBUG_LEVEL_EGCM_WARNING    1
#define DEBUG_LEVEL_EGCM_INFO       2
#define DEBUG_LEVEL_EGCM_ASSERT     7

#define EGCM_DEBUG_ERROR            0x00000001
#define EGCM_DEBUG_WARNING          0x00000002
#define EGCM_DEBUG_INFO             0x00000004
#define EGCM_DEBUG_STATE            0x00000008
#define EGCM_DEBUG_SEND             0x00000010
#define EGCM_DEBUG_RECV             0x00000020
#define EGCM_DEBUG_ENTER            0x00000040
#define EGCM_DEBUG_LEAVE            0x00000080
#define EGCM_DEBUG_TRACE            0x00000100
#define EGCM_DEBUG_STAT             0x00000200
#define EGCM_DEBUG_ASSERT           0x00000400

#define EGCM_DEBUG_NONE             0
#define EGCM_DEBUG_ALL              (~0)
#define EGCM_DEBUG_DEFMASK          EGCM_DEBUG_ALL

#define EGCM_DEBUG_MASK             EGCM_DEBUG_DEFMASK
/*#define EGCM_EOS_TRACE_INCLUDE*/

#if (EGCM_DEBUG_MASK & EGCM_DEBUG_ERROR) 
#ifdef EGCM_EOS_TRACE_INCLUDE
#define EGCM_ERROR(errno, info)   \
    eos_trace_out(DEBUG_MODULE_EGCM, DEBUG_UNIT_EGCM_ERROR, \
        DEBUG_LEVEL_EGCM_ERROR, info, errno, __FILE__, __LINE__)
#else
#define EGCM_ERROR(errno, info)   \
    printf("%s: (%s)(%d)(0x%08x): %s", DEBUG_MODULE_EGCM, \
        __FILE__, __LINE__, errno, info);
#endif
#else
#define EGCM_ERROR(errno, info)
#endif

#if (EGCM_DEBUG_MASK & EGCM_DEBUG_WARNING) 
#ifdef EGCM_EOS_TRACE_INCLUDE
#define EGCM_WARNING(errno, info)   \
    eos_trace_out(DEBUG_MODULE_EGCM, DEBUG_UNIT_EGCM_WARNING, \
        DEBUG_LEVEL_EGCM_WARNING, info, errno, __FILE__, __LINE__)
#else
#define EGCM_WARNING(errno, info)   \
    printf("%s: (%s)(%d)(0x%08x): %s", DEBUG_MODULE_EGCM, \
        __FILE__, __LINE__, errno, info);
#endif
#else
#define EGCM_WARNING(errno, info)
#endif

#if (EGCM_DEBUG_MASK & EGCM_DEBUG_INFO)
#ifdef EGCM_EOS_TRACE_INCLUDE
#define EGCM_INFO(info)   \
    eos_trace_out(DEBUG_MODULE_EGCM, DEBUG_UNIT_EGCM_INFO, \
        DEBUG_LEVEL_EGCM_INFO, info, EOS_OK, __FILE__, __LINE__)
#else
#define EGCM_INFO(info) \
    printf("%s: (%s)(%d): %s", DEBUG_MODULE_EGCM, __FILE__, __LINE__, info)
#endif
#define EGCM_INFO2(fmtinfo, str1, str2) \
do{ \
    _CHAR8 acTmpBuf[128]; \
    EGCM_Sprintf(acTmpBuf, fmtinfo, str1, str2); \
    EGCM_INFO(acTmpBuf); \
}while(0)
#else
#define EGCM_INFO(info)
#define EGCM_INFO2(fmtinfo, str1, str2)
#endif

#if (EGCM_DEBUG_MASK & EGCM_DEBUG_ASSERT) 
#ifdef EGCM_EOS_TRACE_INCLUDE
#define EGCM_ASSERT(cond, errno, info) \
do{ \
    if (!(cond)) \
        eos_trace_out(DEBUG_MODULE_EGCM, DEBUG_UNIT_EGCM_ASSERT, \
            DEBUG_LEVEL_EGCM_ASSERT, info, errno, __FILE__, __LINE__); \
}while(0)
#else
#define EGCM_ASSERT(cond, errno, info) \
do{ \
    if (!(cond)) \
        printf("%s: (%s:%d)(0x%08x): %s", DEBUG_MODULE_EGCM, \
            __FILE__, __LINE__, errno, (_CHAR8*)(info)); \
}while(0)
#endif
#else
#define EGCM_ASSERT(cond, errno, info)
#endif

#define EGCM_CONDCHK(cond, errcode, prompt) \
do{ \
    if (!(cond)) \
    { \
        if (NULL != (prompt)) \
            EGCM_ERROR((_UINT32)(errcode), (_CHAR8*)(prompt)); \
        return errcode; \
    } \
}while(0)

#define EGCM_NULLPTR_CHK(ptr, err) \
    EGCM_CONDCHK(ptr, err, "null pointer passed!\r\n")
    
#define EGCM_MALLOC_CHK(ptr, err) \
    EGCM_CONDCHK(ptr, err, "failed to allocate memory!\r\n")
    
#define EGCM_NULLPTR_ASSERT(ptr, err) \
    EGCM_ASSERT(ptr, err, "null pointer passed!\r\n")
    
#define EGCM_MALLOC_ASSERT(ptr, err) \
    EGCM_ASSERT(ptr, err, "failed to allocate memory!\r\n")

#define EGCM_TRACE printf("%s:%d\r\n", __FILE__, __LINE__)

#define EGCM_Sprintf sprintf

#if defined(__cplusplus)
}
#endif

#endif

