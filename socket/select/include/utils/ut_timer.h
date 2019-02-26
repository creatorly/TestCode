/********************************************************
* 
*
* XP workroom platform
* Copyright (C) 2008-2008, All rights reserved
* Designed by XP workroom.
* 
* 
* File:         ut_timer.h
* PACKAGE:      libutl
*   Header of:          ut_timer.c
*   Date:               08/04/08
*   Author:             PengJian
*   Version:            1.0
*
* Change
* 
*********************************************************/

#ifndef  __UT_TIMER_H__
#define  __UT_TIMER_H__
        
#include <eos_timer.h>

#ifdef __cplusplus
extern "C" {
#endif    

#define USE_EOS_TIMER

#ifdef USE_EOS_TIMER

#define ut_timer_create(n, __ptid)      eos_timer_create(n, __ptid)
#define ut_timer_trig(__tid, d)         eos_timer_trig(__tid, d)
#define ut_timer_show(__tid, f, d)      eos_timer_show(__tid, f, d)

#define ut_timer_start(__tid, l, m, fp, p1, p2, pt) \
            eos_timer_start(__tid, l, m, fp, p1, p2, pt)

#define ut_timer_stop(__tid, t)         eos_timer_stop(__tid, t)
#define ut_timer_reset(__tid, t, l)     eos_timer_reset(__tid, t, l)

#define ut_timer_length(__tid, t)       eos_timer_length(__tid, t)
#define ut_timer_left(__tid, t)         eos_timer_left(__tid, t)

#define ut_timer_get_tmo(__tid)         eos_timer_get_tmo(__tid)

#else

#define UTL_TIMER_DEBUG

_INT32 ut_timer_create(const _CHAR8 * pcName,EOS_TIMER_ID* pTimerGID);
/* ulTime: reserved */
_INT32 ut_timer_trig(EOS_TIMER_ID TimerGID,_INT32 ulTime);
_INT32 ut_timer_show(EOS_TIMER_ID TimerGID, EOS_INFO_PRINTF pPrintFunc, _VOID *pData);

/* ulTimerLength: the timer length in milli-seconds */
#ifdef UTL_TIMER_DEBUG
#define ut_timer_start(tgid, l, m, fp, p1, p2, ptid) \
            ut_timer_start_ex(tgid, l, m, (TIMERFUNC)fp, #fp, (_UINT32)p1, (_UINT32)p2, ptid, __FILE__, __LINE__)
                
_INT32 ut_timer_start_ex(EOS_TIMER_ID TimerGID,_UINT32 len,EOS_TIMER_MODE mode,
                          TIMERFUNC func, const _CHAR8* funcName, 
                         _UINT32 par1,_UINT32 par2, _UINT32 *pTimerID, const _CHAR8* fi, _INT32 li);
#else
_INT32 ut_timer_start(EOS_TIMER_ID TimerGID,_UINT32 len,EOS_TIMER_MODE mode,
                       TIMERFUNC func,_UINT32 par1,_UINT32 par2,_UINT32 *pTimerID);
#endif

_INT32 ut_timer_stop(EOS_TIMER_ID TimerGID,_UINT32 TimerID);

/* ulNewLen: the timer length in milli-seconds */
_INT32 ut_timer_reset(EOS_TIMER_ID TimerGID, _UINT32 TimerID, _UINT32 ulNewLen);

_INT32 ut_timer_length(EOS_TIMER_ID TimerGID, _UINT32 TimerID);
_INT32 ut_timer_left(EOS_TIMER_ID TimerGID, _UINT32 TimerID);

#endif

#ifdef __cplusplus
}
#endif

#endif

