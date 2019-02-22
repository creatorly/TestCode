#ifndef __EOS_TIMER_H
#define __EOS_TIMER_H


#if defined(__cplusplus)
extern "C" {
#endif

typedef        _VOID (*TIMERFUNC)(_UINT32 ulParam1, _UINT32 ulParam2);

typedef struct tagstEosTimers* EOS_TIMER_ID;

typedef enum
{
    EN_TIMER_ONESHOT,
    EN_TIMER_PERIODIC
} EOS_TIMER_MODE;

_INT32 eos_timer_init();

_INT32 eos_timer_create(const _CHAR8 * pcName,EOS_TIMER_ID * ulTimerID);

_INT32 eos_timer_start(EOS_TIMER_ID pTimerID,_UINT32 ulTimerLength,EOS_TIMER_MODE enTimerMode,
                          TIMERFUNC pfnCallback,_UINT32 ulParam1,_UINT32 ulParam2,_UINT32 *pulTimerId);
_INT32 eos_timer_stop(EOS_TIMER_ID pTimerID,_UINT32 ulTimerID);

_INT32 eos_timer_reset(EOS_TIMER_ID TimerGID, _UINT32 TimerID, _UINT32 ulNewLen);

_INT32 eos_timer_length(EOS_TIMER_ID TimerGID, _UINT32 TimerID);
_INT32 eos_timer_left(EOS_TIMER_ID TimerGID, _UINT32 TimerID);

_INT32 eos_timer_trig(EOS_TIMER_ID pTimerID,_INT32 ulTime);
_INT32 eos_timer_show(EOS_TIMER_ID TimerGID, EOS_INFO_PRINTF pPrintFunc, _VOID *pData);

#if defined(__cplusplus)
}
#endif 
#endif



