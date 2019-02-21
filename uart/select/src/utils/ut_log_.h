
#ifndef __UT_LOG__H__
#define __UT_LOG__H__

#if defined(__cplusplus)
extern "C" {
#endif

#define UT_LOG_USE_LOCK
#define UT_LOG_RECORD_TIME
#define UT_LOG_MAX_MOD  32

typedef struct tagstUtLogMod
{
    const _CHAR8* name;

    _INT32 mid;     /* returned by ut_log_registration() */
    _INT32 cseq;
    _BOOL  enable;
    _BOOL to_stdout;

    _INT32 level;
    _INT32 maxLogs;
}ST_UT_LOG_MOD;

typedef struct tagstUtLogErrRec
{
    _NODE node;
    
    const _CHAR8* file;
    _INT32 line;

    _INT32 err_no;
    _INT32 count;
}ST_UT_LOG_ERRREC;

typedef struct tagstUtLogRec
{
    _NODE node;
    
    _INT32 len;
    _INT32 size;
    _CHAR8 log[0];
}ST_UT_LOG_REC;

typedef struct tagstUtilsLog
{
    EOS_SEM_ID lock;

    utl_log_t* pUtlLog;

    ST_UT_LOG_MOD mods[UT_LOG_MAX_MOD];
    
    _UINT32 uiLogMask;
    _INT32 cseq;

    _BOOL udp_enable;
    _BOOL host_parsed;

    _USHORT16 loc_port;
    _USHORT16 rem_port;
    _CHAR8 rem_host[32];

    _LIST lsErrs;
    _LIST lsLogs;
    _INT32 maxLogs;
    
    eosSocket s;
    eosAddress rem;

    /* don't insert any member between mac and buf */
    _CHAR8 mac[18];         /* string MAC address, 17 bytes */
    _CHAR8 buf[4096 - 18];
    _UINT32 len;            /* the length of buf content */
}ST_UT_LOG;

#define UtLogAddCseq(log, mod) \
do{ \
    (log)->len += sprintf(&(log)->buf[(log)->len], "%04d-%03d ", (log)->cseq++, (mod)->cseq++); \
}while(0)

#ifdef UT_LOG_USE_LOCK
#define UT_LOG_LOCK(log)    eos_sem_p((log)->lock, EOS_WAIT_FOREVER)
#define UT_LOG_UNLOCK(log)  eos_sem_v((log)->lock)
#else
#define UT_LOG_LOCK(log)
#define UT_LOG_UNLOCK(log)
#endif

ST_UT_LOG_MOD* utLogFindMod(const _CHAR8* name);
_INT32 utLogVOut(ST_UT_LOG_MOD* mod, _INT32 errnum, _INT32 lv, 
            const _CHAR8* fi, _INT32 ln, const _CHAR8* fmt, va_list ap);

_INT32 utLogModVOut(ST_UT_LOG_MOD* mod, _INT32 errnum, _INT32 lv, 
            const _CHAR8* fi, _INT32 ln, const _CHAR8* fmt, va_list ap);
_INT32 utLogModLogAdd(ST_UT_LOG* pLog, const _CHAR8* buf, _INT32 len);
_INT32 utLogModShow(ST_UT_LOG* pLog, EOS_INFO_PRINTF fpPrintf, _VOID* fd);

_INT32 UtLogAddTime(ST_UT_LOG* pLog);

_VOID utLogSendTCB(ST_UT_LOG* log);
_INT32 utLogSend(ST_UT_LOG* log);

_VOID utLogSvrIpUpdate(_CHAR8* host, _CHAR8* ip, _UINT32 ipv4, ST_UT_LOG* log);

_VOID utLogSetMod(ST_UT_LOG_MOD* mod, _BOOL enable, _BOOL to_stdout);

_VOID utlLogErrorAdd(ST_UT_LOG* pLog, _INT32 iErrno, const _CHAR8* file, _INT32 line);
_INT32 utLogErrorShow(ST_UT_LOG* pLog, EOS_INFO_PRINTF fpPrintf, _VOID* fd);

#if defined(__cplusplus)
}
#endif

#endif


