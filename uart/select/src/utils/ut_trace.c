
#include <eos.h>
#include <acomsys.h>
#include <utils.h>
#include "ut_debug.h"

#include <ut_trace.h>

#if defined(__cplusplus)
extern "C" {
#endif

#ifdef ENABLE_UT_TRACE

#define UT_TRACE_MODNAME_SIZE   8
#define UT_TRACE_MOD_SIZE       32
#define UT_TRACE_LOG_SIZE       (1024*256)//(1024*8)

#define UT_TRACE_USE_SEM

#ifdef UT_TRACE_USE_SEM
#define UT_TRACE_SEM_CREATE(semid)  eos_sem_create("utTraceSem", &semid)
#define UT_TRACE_SEM_DESTROY(semid) eos_sem_delete(semid)
#define UT_TRACE_LOCK(semid)        eos_sem_p(semid, EOS_WAIT_FOREVER)
#define UT_TRACE_UNLOCK(semid)      eos_sem_v(semid)
#else
#define UT_TRACE_SEM_CREATE(semid)
#define UT_TRACE_SEM_DESTROY(semid)
#define UT_TRACE_LOCK(semid)
#define UT_TRACE_UNLOCK(semid)
#endif

#define UT_TRACE_BUFCPY(b, s) \
    do { \
        strncpy(b, s, sizeof(b)); \
        b[sizeof(b) - 1] = '\0'; \
    }while(0)

#define UT_TRACE_MID_ISVALID(t, mid) \
            (((mid) > 0) && ((mid) <= (t)->msize) && ((mid) == (t)->mods[(mid) - 1].mid))

typedef struct tagstUtilsTraceModule
{
    _BOOL enabled;

    _CHAR8 szName[UT_TRACE_MODNAME_SIZE];
    _CHAR8 szSubName[UT_TRACE_MODNAME_SIZE];
    _INT32 mlen;        /* the length of module name */
    _INT32 smLen;       /* the length of sub name */

    _INT32 mid;
    _INT32 level;
    _INT32 cseq;
}ut_trace_mod_t;

typedef struct tagstUtilsTraceLog
{
    struct tagstUtilsTraceLog* next;
    _INT32 size;
    _INT32 len;

    _BOOL bIsPrint;         /* the log handle is being printed */
    _INT32 printed;         /* the length of printed bytes */

    /* log message buffer, store the 4-byte total log length
       (include length itself and the end char '\0'), 
       module name, content of each log, such as: 0013POTS: OK'\0' */
    _CHAR8 buf[UT_TRACE_LOG_SIZE];
}ut_trace_log_t;

typedef struct tagstUtilsTrace
{
    _BOOL initialized;

    _INT32 iMaxBlks;
    _INT32 level;
    _INT32 cseq;

    _INT32 msize;
    _INT32 mcount;
    ut_trace_mod_t* mods;

    _BOOL bIsPrint;
    _INT32 logs;
    _INT32 maxLogs;
    ut_trace_log_t* logHdr;
    ut_trace_log_t* logTail;
    ut_trace_log_t* logFree;

    EOS_SEM_ID wrSemId;
    /* EOS_SEM_ID rdSemId; */
}ut_trace_t;

static ut_trace_t g_stUtilsTrace = {EOS_FALSE};

static ut_trace_log_t* utTraceLogGet(ut_trace_t* pTrace, ut_trace_log_t* logTail)
{
    ut_trace_log_t* pLog = NULL;

    /* allocate a new log buffer */
    if (pTrace->logHdr != logTail)
    {
        pLog = pTrace->logHdr;
        pTrace->logHdr = pLog->next;
        
        pTrace->logs--;
    }

    return pLog;
}

static void utTraceLogFree(ut_trace_t* pTrace, ut_trace_log_t* pLog)
{    
    pLog->next = pTrace->logFree;
    pTrace->logFree = pLog;
}

static _INT32 utTraceLogPrint(ut_trace_log_t* pLog, _INT32 iLogLen, EOS_INFO_PRINTF pPrintFunc, _VOID* pData)
{
    _INT32 len = 0;
    _INT32 pos = pLog->printed;

    while (pos < iLogLen)
    {
        pLog->bIsPrint = EOS_TRUE;
        len += pPrintFunc(pData, pLog->buf + pos + 4);
        pLog->bIsPrint = EOS_FALSE;
    
        pos += atoi(pLog->buf + pos);
    }

    pLog->printed = iLogLen;

    return len;
}

static ut_trace_log_t* utTraceLogAlloc(ut_trace_t* pTrace)
{
    ut_trace_log_t* pLog = NULL;
    
    /* allocate a new log buffer */
    if (pTrace->logFree != NULL)
    {
        pLog = pTrace->logFree;
        pTrace->logFree = pLog->next;
    }
    else if (pTrace->iMaxBlks > 0 && pTrace->maxLogs >= pTrace->iMaxBlks)
    {
        if (!pTrace->bIsPrint)
        {
            /* ut_trace_print is not running, use the log allocated earliest */
            pLog = utTraceLogGet(pTrace, pTrace->logTail);
        }

        if (NULL == pLog)
        {
            /* printf("utTraceLogAlloc: too many log buffer allocated!\r\n"); */
            return NULL;
        }
    }
    else if (pTrace->iMaxBlks > 0 && pTrace->maxLogs < pTrace->iMaxBlks)
    {
        pLog = (ut_trace_log_t*)UT_MALLOC(sizeof(ut_trace_log_t));
        if (NULL == pLog)
        {
            printf("utTraceLogAlloc: failed to allocate log buffer!\r\n");
            return NULL;
        }

        pTrace->maxLogs++;
    }

    pLog->next = NULL;
    pLog->len = 0;
    pLog->printed = 0;
    pLog->size = sizeof(pLog->buf);

    if (pTrace->logTail != NULL)
    {
        pTrace->logTail->next = pLog;
    }
    
    pTrace->logTail = pLog;

    pTrace->logs++;

    return pLog;
}

_INT32 ut_trace_init(_INT32 level, _INT32 max_bytes)
{
    ut_trace_t* pTrace = &g_stUtilsTrace;
    
    if (pTrace->initialized)
    {
        printf("the utils trace module has been initialized!\r\n");
        return EOS_ERROR;
    }

    memset(pTrace, 0, sizeof(ut_trace_t));

    UT_TRACE_SEM_CREATE(pTrace->wrSemId);
    UT_TRACE_LOCK(pTrace->wrSemId);

    pTrace->iMaxBlks = (max_bytes+UT_TRACE_LOG_SIZE-1)/UT_TRACE_LOG_SIZE;
    pTrace->level = level;
    
    pTrace->cseq = 1;
    pTrace->msize = UT_TRACE_MOD_SIZE;
    pTrace->mcount = 0;

    pTrace->mods = (ut_trace_mod_t*)UT_MALLOC(sizeof(ut_trace_mod_t)*pTrace->msize);
    if (NULL == pTrace->mods)
    {
        UT_TRACE_UNLOCK(pTrace->wrSemId);
        UT_TRACE_SEM_DESTROY(pTrace->wrSemId);
        
        printf("failed to allocate the memory of trace modules!\r\n");
        return EOS_ERROR;
    }

    memset(pTrace->mods, 0, sizeof(ut_trace_mod_t)*pTrace->msize);

    pTrace->logHdr = utTraceLogAlloc(pTrace);
    if (NULL == pTrace->logHdr)
    {
        UT_FREE(pTrace->mods);

        UT_TRACE_UNLOCK(pTrace->wrSemId);
        UT_TRACE_SEM_DESTROY(pTrace->wrSemId);
        
        printf("failed to allocate the memory of trace logs!\r\n");
        return EOS_ERROR;
    }
        
    pTrace->bIsPrint = EOS_FALSE;
    pTrace->initialized = EOS_TRUE;

    UT_TRACE_UNLOCK(pTrace->wrSemId);

    return EOS_OK;
}

_INT32 ut_trace_registration(const _CHAR8* pcMName, const _CHAR8* pcSMName, _INT32 level)
{
    ut_trace_t* pTrace = &g_stUtilsTrace;
    ut_trace_mod_t* pMod = NULL;
    _BOOL addMod = EOS_TRUE;
    _INT32 mcount = 0;
    _INT32 i = 0;

    UT_ASSERT(pcMName != NULL);
    UT_ASSERT(pTrace->initialized == EOS_TRUE);
    
    UT_TRACE_LOCK(pTrace->wrSemId);
    
    for (i = 0; i < pTrace->msize; i++)
    {
        if (pTrace->mods[i].enabled == EOS_TRUE)
        {
            if (!strcmp(pcMName, pTrace->mods[i].szName))
            {
                pMod = &pTrace->mods[i];
                addMod = EOS_FALSE;
                
                break;
            }

            mcount++;
        }
        else
        {
            if (NULL == pMod)
            {
                pMod = &pTrace->mods[i];
            }

            if (mcount >= pTrace->mcount)
            {
                /* has no enabled module */
                break;
            }
        }
    }

    if (NULL == pMod)
    {
        ut_trace_mod_t* mods;
        
        /* reallocate the module memory */
        mods = UT_REALLOC(pTrace->mods, sizeof(ut_trace_mod_t)*pTrace->msize*2);
        if (NULL == mods)
        {
            UT_TRACE_UNLOCK(pTrace->wrSemId);

            printf("failed to reallocate the memory of trace modules!\r\n");
            return EOS_ERROR;
        }

        memset(&mods[pTrace->msize], 0, sizeof(ut_trace_mod_t)*pTrace->msize);

        pMod = &mods[pTrace->msize];

        pTrace->mods = mods;
        pTrace->msize *= 2;
    }
    
    UT_TRACE_BUFCPY(pMod->szName, pcMName);

    if (pcSMName != NULL)
    {
        UT_TRACE_BUFCPY(pMod->szSubName, pcSMName);
    }

    pMod->mlen = strlen(pMod->szName);
    pMod->smLen = strlen(pMod->szSubName);

    pMod->enabled = EOS_TRUE;
    pMod->level = ((level > 0) && (level < 255)) ? level : 255;

    if (addMod)
    {
        pMod->mid = pMod - pTrace->mods + 1;
        pMod->cseq = 1;
        pTrace->mcount++;
    }

    UT_TRACE_UNLOCK(pTrace->wrSemId);

    return pMod->mid;
}

_INT32 ut_trace_unregistration(_INT32 mid)
{
    ut_trace_t* pTrace = &g_stUtilsTrace;
    ut_trace_mod_t* pMod = NULL;

    UT_ASSERT(pTrace->initialized == EOS_TRUE);
    
    UT_TRACE_LOCK(pTrace->wrSemId);

    if (!UT_TRACE_MID_ISVALID(pTrace, mid))
    {
        UT_TRACE_UNLOCK(pTrace->wrSemId);
        
        printf("ut_trace: invalid module id %d!\r\n", mid);
        return EOS_ERROR;
    }

    pMod = &pTrace->mods[mid - 1];

    pMod->enabled = EOS_FALSE;
    pMod->mid = 0;

    pTrace->mcount--;

    UT_TRACE_UNLOCK(pTrace->wrSemId);

    return EOS_OK;
}

_INT32 ut_trace_out(_INT32 mid, const _CHAR8* pcSMName, _INT32 iLevel, 
                    const _CHAR8* pcInfo, _INT32 lErrorNum, const _CHAR8* cFile, _INT32 lLine)
{
    
    ut_trace_t* pTrace = &g_stUtilsTrace;
    ut_trace_mod_t* pMod = NULL;
    _INT32 len = 0;

    UT_ASSERT(pTrace->initialized == EOS_TRUE);
    
    UT_TRACE_LOCK(pTrace->wrSemId);

    if (!UT_TRACE_MID_ISVALID(pTrace, mid))
    {
        UT_TRACE_UNLOCK(pTrace->wrSemId);
        
        printf("ut_trace: invalid module id %d!\r\n", mid);
        return EOS_ERROR;
    }

    pMod = &pTrace->mods[mid - 1];

    if ((pMod->enabled) && (iLevel <= pMod->level))
    {
        _INT32 logLen = strlen(pcInfo);
        ut_trace_log_t* pLog = pTrace->logTail;

        /* 4 is for the total length of the log message */
        len = 4 + pMod->mlen + sizeof(": ") + logLen;

        if (lErrorNum != EOS_OK)
        {
            /* the first 16 is the max length of line and the last for errcode */
            len += strlen(cFile) + 16 + 16;
        }

        if (pLog->len + len >= pLog->size)
        {
            /* allocate a new log buffer */
            pLog = utTraceLogAlloc(pTrace);
            if (NULL == pLog)
            {
                UT_TRACE_UNLOCK(pTrace->wrSemId);

                printf("ut_trace: failed to allocate log buffer!\r\n");
                return EOS_ERROR;
            }
        }

        len = pLog->len;

        /* 4-byte reserved for the log length */
        pLog->len += 4;

        /* print the module name */
        pLog->len += sprintf(&pLog->buf[pLog->len], "%s: ", pMod->szName);

        if (lErrorNum != EOS_OK)
        {
            pLog->len += sprintf(&pLog->buf[pLog->len], "%s:%d-%d ", cFile, lLine, lErrorNum);
        }

        memcpy(&pLog->buf[pLog->len], pcInfo, logLen);
        pLog->len += logLen;

        /* add log end, save the end char '\0' */
        pLog->buf[pLog->len++] = '\0';

        /* set the log message length */
        sprintf(&pLog->buf[len], "%04d", pLog->len - len);

        /* reset the first byte of module name */
        pLog->buf[len + 4] = pMod->szName[0];

        len = pLog->len - len - 4;
    }

    UT_TRACE_UNLOCK(pTrace->wrSemId);

    return len;
}

_INT32 ut_trace_print(EOS_INFO_PRINTF pPrintFunc, _VOID* pData)
{
    ut_trace_t* pTrace = &g_stUtilsTrace;
    ut_trace_log_t* pLog = NULL;
    ut_trace_log_t* pLogTail = NULL;
    _INT32 lastLen = 0;
    _INT32 len = 0;

    if (!pTrace->initialized)
    {
        /* the trace module is not initialized */
        return 0;
    }

    if (NULL == pPrintFunc)
    {
        pPrintFunc = EOS_DEFAULT_INFO_PRINT_FUNC;
    }

    UT_TRACE_LOCK(pTrace->wrSemId);

    if ((pTrace->logTail->len == pTrace->logTail->printed) || pTrace->bIsPrint)
    {
        /* no log added or log is being printed */
        UT_TRACE_UNLOCK(pTrace->wrSemId);
        return 0;
    }

    pTrace->bIsPrint = EOS_TRUE;

    pLogTail = pTrace->logTail;
    lastLen = pLogTail->len;
    
    UT_TRACE_UNLOCK(pTrace->wrSemId);

    while ((pLog = utTraceLogGet(pTrace, pLogTail)))
    {
        len += utTraceLogPrint(pLog, pLog->len, pPrintFunc, pData);

        UT_TRACE_LOCK(pTrace->wrSemId);
        utTraceLogFree(pTrace, pLog);
        UT_TRACE_UNLOCK(pTrace->wrSemId);
    }

    len += utTraceLogPrint(pLogTail, lastLen, pPrintFunc, pData);

    UT_TRACE_LOCK(pTrace->wrSemId);

    if (pLogTail->len == lastLen)
    {
        /* no new logs were added */
        pLogTail->len = 0;
        pLogTail->printed = 0;
    }

    pTrace->bIsPrint = EOS_FALSE;
    
    UT_TRACE_UNLOCK(pTrace->wrSemId);

    return len;
}

_INT32 ut_trace_show(EOS_INFO_PRINTF pPrintFunc, _VOID* pData)
{
    ut_trace_t* pTrace = &g_stUtilsTrace;
    _INT32 i = 0;
    _CHAR8 buf[1024];
    _INT32 len = 0;
    _INT32 mcount = 0;

    UT_ASSERT(pTrace->initialized == EOS_TRUE);

    if (NULL == pPrintFunc)
    {
        pPrintFunc = EOS_DEFAULT_INFO_PRINT_FUNC;
    }
    
    UT_TRACE_LOCK(pTrace->wrSemId);

    len = sprintf(buf, "trace: printing=%d tot=%d used=%d mods=%d, ", 
        pTrace->bIsPrint, pTrace->maxLogs, pTrace->logs, pTrace->mcount);
    
    for (i = 0; (mcount < pTrace->mcount) && (i < pTrace->msize); i++)
    {
        ut_trace_mod_t* pMod = &pTrace->mods[i];
        
        if (pMod->enabled == EOS_TRUE)
        {
            len += sprintf(&buf[len], "%s=on, ", pMod->szName);
            mcount++;
        }
    }

    strcpy(&buf[len - 2], "\r\n");

    pPrintFunc(pData, buf);

    UT_TRACE_UNLOCK(pTrace->wrSemId);

    return len;
}

_INT32 ut_trace_set_level(_INT32 mid, _INT32 iLevel)
{
    return EOS_OK;
}

#endif

#if defined(__cplusplus)
}
#endif

