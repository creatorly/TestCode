#ifndef __EOS_TIMER_H
#define __EOS_TIMER_H


#if defined(__cplusplus)
extern "C" {
#endif

typedef struct tagstEosTimers* EOS_TIMER_ID;

typedef _VOID (*TIMERFUNC)(_VOID* par1, _VOID* par2);
typedef _VOID (*EOS_TIMERFUNC_BEGIN)(EOS_TIMER_ID tmrGrpId, _UINT32 tmrId, const _CHAR8* file, _INT32 line);
typedef _VOID (*EOS_TIMERFUNC_END)(EOS_TIMER_ID tmrGrpId, _UINT32 tmrId, const _CHAR8* file, _INT32 line);

typedef enum
{
    EN_TIMER_ONESHOT,
    EN_TIMER_PERIODIC
} EOS_TIMER_MODE;

_INT32 eos_timer_init();

_INT32 eos_timer_create(const _CHAR8 * pcName,EOS_TIMER_ID * ulTimerID);

/* for all timers, only efficient in debug mode */
_VOID eos_timer_set_hook_begin(EOS_TIMER_ID TimerGID, EOS_TIMERFUNC_BEGIN fpBegin);
_VOID eos_timer_set_hook_end(EOS_TIMER_ID TimerGID, EOS_TIMERFUNC_END fpEnd);

_INT32 eos_timer_start_ex(EOS_TIMER_ID TimerGID, const _CHAR8* file, _INT32 line, 
                          const _CHAR8* name, _UINT32 len, EOS_TIMER_MODE mode, 
                          TIMERFUNC func, _VOID* par1, _VOID* par2, _UINT32* pTimerID);

_INT32 eos_timer_stop_ex(EOS_TIMER_ID pTimerID,_UINT32 ulTimerID, const _CHAR8* file, _INT32 line);

_INT32 eos_timer_reset_ex(EOS_TIMER_ID TimerGID, _UINT32 TimerID, _UINT32 ulNewLen,
                          const _CHAR8* file, _INT32 line);

#define eos_timer_start(tg, l, m, f, p1, p2, pt)    \
            eos_timer_start_ex(tg, __FILE__, __LINE__, #f, \
                l, m, (TIMERFUNC)(f), (_VOID*)(p1), (_VOID*)(p2), pt)

#define eos_timer_stop(tg, t)   \
            eos_timer_stop_ex(tg, t, __FILE__, __LINE__)

#define eos_timer_reset(tg, t, l) \
            eos_timer_reset_ex(tg, t, l, __FILE__, __LINE__)

_INT32 eos_timer_length(EOS_TIMER_ID TimerGID, _UINT32 TimerID);
_INT32 eos_timer_left(EOS_TIMER_ID TimerGID, _UINT32 TimerID);

/**
 * GET the timeout of the first timer of the timer group for next eos_timer_trig()s
 * @param TimerGID: The timer group to be worked on
 *
 * @return:         the timeout in milli-seconds
 */
_INT32 eos_timer_get_tmo(EOS_TIMER_ID TimerGID);

_INT32 eos_timer_trig(EOS_TIMER_ID pTimerID,_INT32 ulTime);
_INT32 eos_timer_show(EOS_TIMER_ID TimerGID, EOS_INFO_PRINTF pPrintFunc, _VOID *pData);

#if defined(__cplusplus)
}
#endif 
#endif



