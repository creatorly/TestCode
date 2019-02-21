
#ifndef __UT_LOG_H__
#define __UT_LOG_H__

#include <stdarg.h>

#include <eos_typedef.h>
#include <eos_statistics.h>
#include <eos_timer.h>
#include <utl_log.h>

#if defined(__cplusplus)
extern "C" {
#endif

typedef enum
{
    UT_LOG_LEVEL_EVENT = 0,
    UT_LOG_LEVEL_CRIT,
    UT_LOG_LEVEL_ERROR,
    UT_LOG_LEVEL_WARN,
    UT_LOG_LEVEL_INFO,
    UT_LOG_LEVEL_DBG3,
    UT_LOG_LEVEL_DBG2,
    UT_LOG_LEVEL_DBG1,

    UT_LOG_LEVEL_MAX
}EN_UT_LOG_LEVEL;

#ifdef __GNUC__
#define UT_CHK_FMTARGS(fmt_pos) \
            __attribute__((format(printf, fmt_pos, fmt_pos+1)))
#else
#define UT_CHK_FMTARGS(fmt_pos)
#endif

_INT32 UT_LogInit();
_INT32 UT_LogFileInit(const _CHAR8* logfile, _INT32 size, _INT32 count);
_INT32 UT_LogStart(EOS_TIMER_ID TimerGID);
_VOID UT_LogEnable(_BOOL enable);

utl_log_t* UT_LogGetUtlLog();
_VOID UT_LogSetUtlLog(utl_log_t* pUtlLog);

_VOID UT_LogInitMod(_INT32 mid, const _CHAR8* mname);
_INT32 UT_LogSetMod(const _CHAR8* name, _BOOL enable);
_INT32 UT_LogSetModEx(const _CHAR8* name, _BOOL enable, _BOOL to_stdout);

_INT32 UT_LogGetTraceId(_INT32 mid);

/* set the log buffer(max count of log) for each module */
_INT32 UT_LogSetBuf(const _CHAR8* mname, _INT32 maxLogs);

/* set the log level, the larger level log will be discarded */
_INT32 UT_LogSetModLevel(const _CHAR8* mname, _INT32 level);
_VOID UT_LogSetMask(_UINT32 uiLogMask);

_VOID UT_LogSetLevel(_INT32 level);
_VOID UT_LogSetStdout(_BOOL bToStdout);

#define UT_LogOut(m, fi, li, lv, c, s, fmt, args...) \
            UT_LogOutEx(m, s, fi, li, lv, c, -1, NULL, fmt, ##args)

_INT32 UT_CHK_FMTARGS(9) 
       UT_LogOutEx(_UINT32 mid, _INT32 size, const _CHAR8* file, _INT32 line, 
                   _INT32 level, _INT32 code, _INT32 index, const char* msg, const _CHAR8* fmt, ...);

_INT32 UT_LogVOut(_UINT32 mid, const _CHAR8* file, _INT32 line, 
                  _INT32 level, _INT32 code, _INT32 size, const _CHAR8* fmt, va_list ap);

_INT32 UT_CHK_FMTARGS(1) 
       UT_LogMsg(const _CHAR8* fmt, ...);

_INT32 UT_LogBuf(const _CHAR8* buf, _INT32 len);
_INT32 UT_LogLongBuf(const _CHAR8* buf, _INT32 len, _INT32 pkgsize, _INT32 interval);

_INT32 UT_CHK_FMTARGS(3) 
       UT_LogFile(_INT32 mid, _INT32 code, const _CHAR8* fmt, ...);

_INT32 UT_LogStat(EOS_INFO_PRINTF fpPrintf, _VOID* fd);

#if defined(__cplusplus)
}
#endif

#endif


