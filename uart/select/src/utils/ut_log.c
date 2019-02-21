
#include <eos.h>
#include <eos_buf.h>
#include <utils.h>
#include <ut_trace.h>
#include "ut_debug.h"

#include <utl_log.h>
#include <ut_log.h>
#include "ut_log_.h"

#if defined(__cplusplus)
extern "C" {
#endif

#define UT_LOG_DYINGMSG     "/tffs/var/dyingmsg.log"

#define UT_LOG_SEND_SIZE    1024
#define UT_LOG_SEND_PERIOD  1000

static ST_UT_LOG g_stUtLog = {0};

static const _CHAR8 g_aacUtlogLvNames[][16] = 
{
    "EVENT",
    "CRIT",
    "ERROR",
    "WARN",
    "INFO",
    "DBG3",
    "DBG2",
    "DBG1",
};

_INT32 utLogFileInit(ST_UT_LOG* pLog, const _CHAR8* logfile, _INT32 size, _INT32 count)
{
    utl_log_t* pUtlLog = NULL;
    
#ifdef __POSIX_OS__
    pUtlLog = utl_log_create(UT_LOG_LEVEL_DBG1, logfile, size, count);
    if (pUtlLog != NULL)
    {
        /* no need to print log name, index, file path and CRLF */
        utl_log_disable_flags(pUtlLog, UTL_LOG_NAME);

        utl_log_enable_flags(pUtlLog, UTL_LOG_NOPATH|UTL_LOG_NOCRLF);
        utl_log_enable_flags(pUtlLog, UTL_LOG_NOBUF);

        /* to test the function tickGet() */
        utl_log_enable_flags(pUtlLog, UTL_LOG_TICK);

        /* only print the log whose level is lower INFO */
        utl_log_set_self(pUtlLog, UT_LOG_LEVEL_DBG1);
    }
    else
    {
        printf("failed to create utl log file!\r\n");
    }
#endif

    pLog->pUtlLog = pUtlLog;

    return EOS_OK;
}

_INT32 UT_LogInit()
{
#ifdef ENABLE_UT_TRACE
    /* initialize the trace first, 64K log buffer at most */
    if (ut_trace_init(0, 640*1024) != EOS_OK)
    {        
        return EOS_ERROR;
    }
#endif

    memset(&g_stUtLog, 0, sizeof(g_stUtLog));
    
    if (0 == g_stUtLog.lock)
    {
        eos_sem_create("utLogSem", &g_stUtLog.lock);
    }

    eos_lst_init(&g_stUtLog.lsErrs);
    eos_lst_init(&g_stUtLog.lsLogs);

    g_stUtLog.udp_enable = EOS_FALSE;
    g_stUtLog.host_parsed = EOS_FALSE;
    g_stUtLog.cseq = 1;
    g_stUtLog.maxLogs = 0;

    /* initialized in UT_LogInit2, to trace all modules first */
    g_stUtLog.uiLogMask = ~0;

    UT_LogInitMod(EN_ASYS_MODULE_UT, DEBUG_MODULE_UT);

    return EOS_OK;
}

_INT32 UT_LogFileInit(const _CHAR8* logfile, _INT32 size, _INT32 count)
{
    return utLogFileInit(&g_stUtLog, logfile, size, count);
}

_INT32 UT_LogStart(EOS_TIMER_ID TimerGID)
{
#ifdef ENABLE_UT_DNSIP
    UT_DnsIpAddEx(g_stUtLog.rem_host, (FP_UT_IPUPDATE)utLogSvrIpUpdate, &g_stUtLog);
#endif
    
    return ut_timer_start(TimerGID, UT_LOG_SEND_PERIOD, EN_TIMER_PERIODIC, 
                (TIMERFUNC) utLogSendTCB, (_UINT32)&g_stUtLog, 0, NULL);
}

_VOID UT_LogEnable(_BOOL enable)
{
    UT_LOG_LOCK(&g_stUtLog);
    g_stUtLog.udp_enable = enable;
    UT_LOG_UNLOCK(&g_stUtLog);
}

utl_log_t* UT_LogGetUtlLog()
{
    return g_stUtLog.pUtlLog;
}

_VOID UT_LogSetUtlLog(utl_log_t* pUtlLog)
{
    if (g_stUtLog.pUtlLog != NULL)
    {
        utl_log_destroy(g_stUtLog.pUtlLog);
    }

    g_stUtLog.pUtlLog = pUtlLog;
}

_INT32 UT_LogGetTraceId(_INT32 mid)
{
    return g_stUtLog.mods[mid].mid;
}

_VOID UT_LogInitMod(_INT32 mid, const _CHAR8* mname)
{
    if (mid >= 0 && mid < EOS_NELEMENTS(g_stUtLog.mods))
    {
        ST_UT_LOG_MOD* pMod = &g_stUtLog.mods[mid];
        
        pMod->name = mname;

        pMod->mid = 0;
        pMod->cseq = 1;
        pMod->maxLogs = 0;
        pMod->level = 255;
    }
}

_INT32 UT_LogSetMod(const _CHAR8* name, _BOOL enable)
{
    return UT_LogSetModEx(name, enable, EOS_FALSE);
}

_INT32 UT_LogSetModEx(const _CHAR8* name, _BOOL enable, _BOOL to_stdout)
{
    ST_UT_LOG* pLog = &g_stUtLog;
    ST_UT_LOG_MOD* pMod;
    _INT32 iRet = EOS_OK;

    UT_LOG_LOCK(pLog);

    pMod = utLogFindMod(name);

    if (pMod != NULL)
    {
        utLogSetMod(pMod, enable, to_stdout);
    }
    else if (!UT_StrCaseCmp(name, "all"))
    {
        int i;
        for (i=0; i < EOS_NELEMENTS(pLog->mods); i++)
        {
            utLogSetMod(&pLog->mods[i], enable, to_stdout);
        }
    }
    else
    {
        printf("invalid module name %s!", name);
        iRet = EOS_ERROR;
    }

    UT_LOG_UNLOCK(pLog);

    return iRet;
}

_INT32 UT_LogSetBuf(const _CHAR8* mname, _INT32 maxLogs)
{
    ST_UT_LOG* pLog = &g_stUtLog;
    ST_UT_LOG_MOD* pMod;

    if (maxLogs <= 0)
    {
        printf("invalid maxLogs: %d\r\n", maxLogs);
        return EOS_ERROR;
    }

    UT_LOG_LOCK(pLog);

    pMod = utLogFindMod(mname);
    if (pMod != NULL)
    {
        pLog->maxLogs = pLog->maxLogs - pMod->maxLogs + maxLogs;
        pMod->maxLogs = maxLogs;
    }

    while (eos_lst_count(&pLog->lsLogs) > pLog->maxLogs)
    {
        _NODE* pRec = eos_lst_get(&pLog->lsLogs);
        UT_FREE(pRec);
    }

    UT_LOG_UNLOCK(pLog);

    return EOS_OK;
}

_INT32 UT_LogSetModLevel(const _CHAR8* mname, _INT32 level)
{
    ST_UT_LOG* pLog = &g_stUtLog;
    ST_UT_LOG_MOD* pMod;

    UT_LOG_LOCK(pLog);

    pMod = utLogFindMod(mname);
    if (pMod != NULL)
    {
        pMod->level = (level > 0) ? level : 255;
    }

    UT_LOG_UNLOCK(pLog);

    return EOS_OK;
}

_VOID UT_LogSetMask(_UINT32 uiLogMask)
{
    g_stUtLog.uiLogMask = (uiLogMask | (1 << EN_ASYS_MODULE_UT));
}

_VOID UT_LogSetLevel(_INT32 level)
{
    utl_log_set_level(g_stUtLog.pUtlLog, level);
}

_VOID UT_LogSetStdout(_BOOL bToStdout)
{
    utl_log_t* pLog = g_stUtLog.pUtlLog;
    
    if (bToStdout)
    {
        utl_log_enable_flags(pLog, UTL_LOG_STDOUT | UTL_LOG_TICK);
    }
    else
    {
        utl_log_disable_flags(pLog, UTL_LOG_STDOUT);
    }
}

_INT32 UT_LogOutEx(_UINT32 mid, _INT32 size, const _CHAR8* file, _INT32 line, 
                   _INT32 level, _INT32 code, _INT32 index, const char* msg, const _CHAR8* fmt, ...)
{
    ST_UT_LOG* pLog = &g_stUtLog;
    ST_UT_LOG_MOD* mod = &pLog->mods[mid];
    _INT32 iLen = 0;
    va_list ap;

#if 0
    if (code != 0)
    {
        utlLogErrorAdd(pLog, code, file, line);
    }
#endif

    if (pLog->uiLogMask & (1 << mid))
    {
        va_start(ap, fmt);
        iLen = utl_log_vnadd(pLog->pUtlLog, size, file, line, level, code, index, mod->name, msg, fmt, ap);
        va_end(ap);
    }

#if 0
    if (level < mod->level)
    {
        if (mod->enable)
        {
            va_start(ap, fmt);
            iLen = utLogVOut(mod, code, level, file, line, fmt, ap);
            va_end(ap);
        }
        else if (mod->maxLogs > 0)
        {
            va_start(ap, fmt);
            iLen = utLogModVOut(mod, code, level, file, line, fmt, ap);
            va_end(ap);
        }
    }
#endif

    return iLen;
}

_INT32 UT_LogVOut(_UINT32 mid, const _CHAR8* file, _INT32 line, 
                  _INT32 level, _INT32 code, _INT32 size, const _CHAR8* fmt, va_list ap)
{
    ST_UT_LOG* pLog = &g_stUtLog;
    ST_UT_LOG_MOD* mod = &pLog->mods[mid];
    _INT32 iLen = 0;

    if (code != 0)
    {
        utlLogErrorAdd(pLog, code, file, line);
    }
    
    if (pLog->uiLogMask & (1 << mid))
    {
        utl_log_vnadd(pLog->pUtlLog, size, file, line, level, code, -1, mod->name, NULL, fmt, ap);
    }

    if (level < mod->level)
    {
        if (mod->enable)
        {
            iLen = utLogVOut(mod, code, level, file, line, fmt, ap);
        }
        else if (mod->maxLogs > 0)
        {
            iLen = utLogModVOut(mod, code, level, file, line, fmt, ap);
        }
    }

    return iLen;
}

_INT32 UT_LogMsg(const _CHAR8* fmt, ...)
{
    ST_UT_LOG* log = &g_stUtLog;
    _INT32 len = 0;
    va_list ap;

    va_start(ap, fmt);

    UT_LOG_LOCK(log);

    if (log->udp_enable)
    {
        len = log->len;
        log->len += vsprintf(&log->buf[log->len], fmt, ap);

        /* send the log to the server immediately  */
        utLogSend(log);

        len = log->len - len;
        log->len = 0;
    }
    else
    {
        len = vprintf(fmt, ap);
    }

    UT_LOG_UNLOCK(log);

    va_end(ap);

    return len;
}

_INT32 UT_LogBuf(const _CHAR8* buf, _INT32 len)
{
    return UT_LogLongBuf(buf, len, UT_LOG_SEND_SIZE, 0);
}

_INT32 UT_LogLongBuf(const _CHAR8* buf, _INT32 len, _INT32 pkgsize, _INT32 interval)
{
    ST_UT_LOG* log = &g_stUtLog;
    const _CHAR8* buf_cpy = buf;
    EOSSize_t sent = 0;

    UT_LOG_LOCK(log);

    if (log->len > 0)
    {
        /* send the log to the server immediately  */
        sent += utLogSend(log);
        log->len = 0;

        if (interval > 0)
        {
            /* DRV_SysSetParam(SYS_WDTCLR, 0, 0, NULL); */
            taskDelay(interval*sysClkRateGet()/1000);
        }
    }

    while (len > pkgsize)
    {
        /* to ensure the package is not than 1024 */
        memcpy(log->buf, buf, pkgsize);
        log->len = pkgsize;

        /* send the log to the server immediately */
        sent += utLogSend(log);

        buf += pkgsize;
        len -= pkgsize;

        if (interval > 0)
        {
            /* DRV_SysSetParam(SYS_WDTCLR, 0, 0, NULL); */
            taskDelay(interval*sysClkRateGet()/1000);
        }
    }

    if (len > 0)
    {
        memcpy(log->buf, buf, len);
        log->len = len;

        /* send the log to the server immediately */
        sent += utLogSend(log);

        if (interval > 0)
        {
            /* DRV_SysSetParam(SYS_WDTCLR, 0, 0, NULL); */
            taskDelay(interval*sysClkRateGet()/1000);
        }
    }

    log->len = 0;

    if (!log->udp_enable)
    {
        sent = printf("%s", buf_cpy);
    }

    UT_LOG_UNLOCK(log);

    return sent;
}

_INT32 UT_LogFile(_INT32 mid, _INT32 code, const _CHAR8* fmt, ...)
{
    ST_UT_LOG* pLog = &g_stUtLog;
    _INT32 len = 0;

    if (pLog->uiLogMask & (1 << mid))
    {
        va_list m_va;
       
        va_start(m_va, fmt);
        len = utl_log_vprintf(pLog->pUtlLog, fmt, m_va);
        va_end(m_va);
    }

    return len;
}

_INT32 UT_LogStat(EOS_INFO_PRINTF fpPrintf, _VOID* fd)
{
    ST_UT_LOG* pLog = &g_stUtLog;
    _CHAR8 buf[64];
    _INT32 len = 0;

    len = sprintf(buf, "%s", "UT Log Statistics:\r\n");
    fpPrintf(fd, buf);

    UT_LOG_LOCK(pLog);

    len += utLogErrorShow(pLog, fpPrintf, fd);
    len += utLogModShow(pLog, fpPrintf, fd);

    UT_LOG_UNLOCK(pLog);

    return len;
}

/* trace the log information by invoking eos_trace_out() */
_INT32 utLogVOut(ST_UT_LOG_MOD* mod, _INT32 errnum, _INT32 lv, 
            const _CHAR8* fi, _INT32 ln, const _CHAR8* fmt, va_list ap)
{
    ST_UT_LOG* log = &g_stUtLog;
    const _CHAR8* lname = g_aacUtlogLvNames[lv];
    _INT32 b_len = 0;
    _INT32 len = 0;

    UT_LOG_LOCK(&g_stUtLog);

    b_len = log->len;

#ifdef UT_LOG_RECORD_TIME
    UtLogAddTime(log);
#endif

    UtLogAddCseq(log, mod);

    if (log->udp_enable)
    {
        log->len += sprintf(&log->buf[log->len], "%s %s", mod->name, lname);
        
        if (errnum != EOS_OK)
        {
            log->len += sprintf(&log->buf[log->len], "(%s:%d)(%d)", fi, ln, errnum);
        }

        log->len += sprintf(&log->buf[log->len], ": ");
        log->len += vsprintf(&log->buf[log->len], fmt, ap);

        len = log->len - b_len;
        
        while (log->len > UT_LOG_SEND_SIZE)
        {
            /* to ensure the package is not than 1024 */
            b_len = log->len;
            log->len = UT_LOG_SEND_SIZE;

            /* send the log to the server immediately */
            utLogSend(log);

            log->len = b_len - UT_LOG_SEND_SIZE;
            memmove(log->buf, &log->buf[UT_LOG_SEND_SIZE], log->len);
        }
    }
    else if (mod->to_stdout)
    {
        log->len += sprintf(&log->buf[log->len], "%s %s", mod->name, lname);
        
        if (errnum != EOS_OK)
        {
            log->len += sprintf(&log->buf[log->len], "(%s:%d)(%d)", fi, ln, errnum);
        }
    
        log->len += sprintf(&log->buf[log->len], ": ");
        log->len += vsprintf(&log->buf[log->len], fmt, ap);
    
        printf("%s", log->buf);
        
        len = log->len - b_len;
        log->len = 0;
    }
    else
    {
        log->len += vsprintf(&log->buf[log->len], fmt, ap);
    
    #ifdef ENABLE_UT_TRACE
        ut_trace_out(mod->mid, lname, lv, log->buf, errnum, fi, ln);
    #endif
    
        len = log->len - b_len;
        log->len = 0;
    }
    
    UT_LOG_UNLOCK(&g_stUtLog);
    
    return len;
}

/* trace the log information by invoking eos_trace_out() */
_INT32 utLogModVOut(ST_UT_LOG_MOD* mod, _INT32 errnum, _INT32 lv, 
            const _CHAR8* fi, _INT32 ln, const _CHAR8* fmt, va_list ap)
{
    ST_UT_LOG* log = &g_stUtLog;
    const _CHAR8* lname = g_aacUtlogLvNames[lv];
    _INT32 len = 0;

    UT_LOG_LOCK(&g_stUtLog);

    len = log->len;

#ifdef UT_LOG_RECORD_TIME
    UtLogAddTime(log);
#endif

    UtLogAddCseq(log, mod);

    log->len += sprintf(&log->buf[log->len], "%s %s", mod->name, lname);
    
    if (errnum != EOS_OK)
    {
        log->len += sprintf(&log->buf[log->len], "(%s:%d)(%d)", fi, ln, errnum);
    }

    log->len += sprintf(&log->buf[log->len], ": ");
    log->len += vsprintf(&log->buf[log->len], fmt, ap);

    utLogModLogAdd(log, log->buf, log->len);
    
    len = log->len - len;
    log->len = 0;

    UT_LOG_UNLOCK(&g_stUtLog);

    return len;
}

_INT32 utLogModLogAdd(ST_UT_LOG* pLog, const _CHAR8* buf, _INT32 len)
{
    ST_UT_LOG_REC* pRec = NULL;
    
    if (eos_lst_count(&pLog->lsLogs) >= pLog->maxLogs)
    {
        pRec = (ST_UT_LOG_REC*)eos_lst_get(&pLog->lsLogs);
        if (pRec->size <= len)
        {
            UT_FREE(pRec);
            pRec = NULL;
        }
    }

    if (NULL == pRec)
    {
        pRec = (ST_UT_LOG_REC*)UT_MALLOC(sizeof(ST_UT_LOG_REC) + len + 1);
        if (NULL == pRec)
        {
            printf("failed to malloc for ST_UT_LOG_REC 1! size = %d\r\n", len);
            return EOS_ERROR;
        }

        pRec->size = len + 1;
    }

    memcpy(pRec->log, buf, len);
    pRec->len = len;
    pRec->log[len] = '\0';

    eos_lst_add(&pLog->lsLogs, &pRec->node);

    return EOS_OK;
}

_INT32 utLogModShow(ST_UT_LOG* pLog, EOS_INFO_PRINTF fpPrintf, _VOID* fd)
{
    _CHAR8 buf[128];
    _INT32 len = 0;
    
    ST_UT_LOG_REC* pRec = NULL;

    len = sprintf(buf, "Log Info: size(%d)\r\n", eos_lst_count(&pLog->lsLogs));
    fpPrintf(fd, buf);

    pRec = (ST_UT_LOG_REC*)eos_lst_get(&pLog->lsLogs);

    while (pRec != NULL)
    {
        fpPrintf(fd, pRec->log);
        len += pRec->len;

        /* free the log to avoid reshow it */
        UT_FREE(pRec);

        pRec = (ST_UT_LOG_REC*)eos_lst_get(&pLog->lsLogs);
    }

    pLog->len = 0;

    return len;
}

ST_UT_LOG_MOD* utLogFindMod(const _CHAR8* name)
{
    ST_UT_LOG* pLog = &g_stUtLog;
    _UINT32 i;

    for (i=0; i < EOS_NELEMENTS(pLog->mods); i++)
    {
        if (pLog->mods[i].name && !strcmp(name, pLog->mods[i].name))
        {
            return &pLog->mods[i];
        }
    }

    return NULL;
}

_INT32 UtLogAddTime(ST_UT_LOG* pLog)
{
#ifdef __VXWORKS_OS__
    struct timespec ts;
    struct tm loc_tm;

    clock_gettime(CLOCK_REALTIME, &ts);
    eos_localtime_r(&ts.tv_sec, &loc_tm);

    pLog->len += sprintf(pLog->buf + pLog->len,
        "%02d/%02d %02d:%02d:%02d.%03ld|",
        loc_tm.tm_mon+1, loc_tm.tm_mday, loc_tm.tm_hour, 
        loc_tm.tm_min, loc_tm.tm_sec, ts.tv_nsec/1000000);
#else
    struct timeval tv;
    struct tm loc_tm;
    
    gettimeofday(&tv, NULL);
    eos_localtime_r(&tv.tv_sec, &loc_tm);

    pLog->len += sprintf(pLog->buf + pLog->len,
        "%02d/%02d %02d:%02d:%02d.%03ld|",
        loc_tm.tm_mon+1, loc_tm.tm_mday, loc_tm.tm_hour, 
        loc_tm.tm_min, loc_tm.tm_sec, tv.tv_usec/1000);
#endif

    return pLog->len;
}

_VOID utLogSendTCB(ST_UT_LOG* log)
{
    UT_LOG_LOCK(log);

    if (log->len > 0)
    {
        utLogSend(log);
        log->len = 0;
    }
    
    UT_LOG_UNLOCK(log);
}

_INT32 utLogSend(ST_UT_LOG* log)
{
    EOSSize_t sent = 0;

    if (log->host_parsed)
    {
        eos_socket_send_buffer(&log->s, log->mac, sizeof(log->mac)+log->len, &log->rem, &sent);
    }

    return sent;
}

_VOID utLogSvrIpUpdate(_CHAR8* host, _CHAR8* ip, _UINT32 ipv4, ST_UT_LOG* log)
{
    UT_LOG_LOCK(log);

    if (!strcmp(host, log->rem_host))
    {
        eos_address_construct_ipv4(&log->rem, ipv4, log->rem_port);
        log->host_parsed = EOS_TRUE;
    }
    
    UT_LOG_UNLOCK(log);
}

_VOID utLogSetMod(ST_UT_LOG_MOD* mod, _BOOL enable, _BOOL to_stdout)
{
#ifdef ENABLE_UT_TRACE
    if (enable == EOS_TRUE)
    {
        if (!mod->enable)
        {
            mod->mid = ut_trace_registration(mod->name, NULL, 0);
            mod->enable = (mod->mid > 0);
            mod->to_stdout = to_stdout;
        }
    }
    else
    {
        if (mod->enable && (mod->mid > 0))
        {
            ut_trace_unregistration(mod->mid);
        }
        
        mod->mid = 0;
        mod->enable = EOS_FALSE;
        mod->to_stdout = to_stdout;
    }
#endif
}

_VOID utlLogErrorAdd(ST_UT_LOG* pLog, _INT32 iErrno, const _CHAR8* file, _INT32 line)
{
    ST_UT_LOG_ERRREC* pErrRec;

    UT_LOG_LOCK(&g_stUtLog);

    pErrRec = (ST_UT_LOG_ERRREC*)eos_lst_first(&pLog->lsErrs);
    while (pErrRec != NULL)
    {
        if ((line == pErrRec->line) && (!strcmp(file, pErrRec->file)))
        {
            pErrRec->err_no = iErrno;
            pErrRec->count++;
            
            UT_LOG_UNLOCK(&g_stUtLog);

            return;
        }
        
        pErrRec = (ST_UT_LOG_ERRREC*)eos_lst_next(&pErrRec->node);
    }

    /* not found, allocate a new */
    pErrRec = (ST_UT_LOG_ERRREC*)UT_MALLOC(sizeof(ST_UT_LOG_ERRREC));
    if (NULL == pErrRec)
    {
        UT_LOG_UNLOCK(&g_stUtLog);

        printf("failed to allocate memory for ST_UT_LOG_ERRREC!\r\n");
        return;
    }

    pErrRec->file = file;
    pErrRec->line = line;
    pErrRec->err_no = iErrno;
    pErrRec->count = 1;

    eos_lst_add(&pLog->lsErrs, &pErrRec->node);

    UT_LOG_UNLOCK(&g_stUtLog);
}

_INT32 utLogErrorShow(ST_UT_LOG* pLog, EOS_INFO_PRINTF fpPrintf, _VOID* fd)
{
    _CHAR8 buf[128];
    _INT32 len = 0;
    
    ST_UT_LOG_ERRREC* pErrRec = 
        (ST_UT_LOG_ERRREC*)eos_lst_first(&pLog->lsErrs);

    len = sprintf(buf, "Error Info List: size(%d)\r\n", eos_lst_count(&pLog->lsErrs));
    fpPrintf(fd, buf);

    while (pErrRec != NULL)
    {
        len += sprintf(buf, "%s:%d: errno(%d) count(%d)\r\n", 
            pErrRec->file, pErrRec->line, pErrRec->err_no, pErrRec->count);
        fpPrintf(fd, buf);
        
        pErrRec = (ST_UT_LOG_ERRREC*)eos_lst_next(&pErrRec->node);
    }

    return len;
}

#if defined(__cplusplus)
}
#endif

