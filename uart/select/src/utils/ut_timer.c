/********************************************************
* 
*
* XP workroom platform
* Copyright (C) 2008-2008, All rights reserved
* Designed by XP workroom.
* 
* 
* File:         ut_timer.c
* PACKAGE:      libut
*   Implements of:      ut timer
*   Date:               08/04/08
*   Author:             PengJian
*   Version:            1.0
*
* Change
* 
*********************************************************/

#include <eos.h>
#include <ut_timer.h>
#include "ut_mem.h"

#include "ut_debug.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef USE_EOS_TIMER

typedef struct tagstUtlTimerGroup
{
    char acName[32];
    _LIST lsTimers;
    _LIST lsFrees;

    EOS_SEM_ID sem;
}UTL_TIMERG_T;

typedef struct tagstUtTimer
{
    _NODE node;

    EOS_TIMER_MODE enMode;
    _UINT32 iLength;                /* length in milli-seconds */
    _UINT32 iPeriod;                /* length in ticks */
    _UINT32 tvEndTime;
    
    const _CHAR8* pcFuncName;
    TIMERFUNC pfTimerCB;
    _UINT32 par1;
    _UINT32 par2;

    _BOOL active;
    _BOOL running;
    _BOOL reset;
}UT_TIMER_T;

_VOID utGtimerAddTimer(UTL_TIMERG_T* pstTimerG, UT_TIMER_T* pstTimer);

/* create and destroy a group timer */
_INT32 ut_timer_create(const _CHAR8 * pcName, EOS_TIMER_ID * pTimerGID)
{
    UTL_TIMERG_T* pstTimerG = NULL;
    _INT32 iRet;

    *pTimerGID = 0;

    pstTimerG = (UTL_TIMERG_T*)UT_MALLOC(sizeof(UTL_TIMERG_T));
    if (NULL == pstTimerG)
    {
        UT_ERROR(EOS_ERROR, "out of memory!");
        return EOS_ERROR;
    }

    memset(pstTimerG, 0, sizeof(UTL_TIMERG_T));

    iRet = eos_sem_create(pcName, &pstTimerG->sem);
    if (iRet != EOS_OK)
    {
        UT_FREE(pstTimerG);
        
        UT_ERROR(EOS_ERROR, "failed to initialize timer-sem!");
        return EOS_ERROR;
    }

    eos_sem_p(pstTimerG->sem, EOS_WAIT_FOREVER);

    if (NULL != pcName)
    {
        strncpy(pstTimerG->acName, pcName, sizeof(pstTimerG->acName) - 1);
    }

    eos_lst_init(&pstTimerG->lsTimers);
    eos_lst_init(&pstTimerG->lsFrees);

    eos_sem_v(pstTimerG->sem);

    *pTimerGID = (_UINT32)pstTimerG;

    return EOS_OK;
}

/* start and stop a timer in a specified group timer */
#ifdef UTL_TIMER_DEBUG
_INT32 ut_timer_start_ex(EOS_TIMER_ID TimerGID,_UINT32 len,EOS_TIMER_MODE mode,
                          TIMERFUNC func, const _CHAR8* funcName, 
                         _UINT32 par1,_UINT32 par2, _UINT32 *pTimerID, const _CHAR8* fi, _INT32 li)
#else
_INT32 ut_timer_start(EOS_TIMER_ID TimerGID,_UINT32 len, EOS_TIMER_MODE mode,
                       TIMERFUNC func, _UINT32 par1,_UINT32 par2, _UINT32* pTimerID)
#endif
{
    UTL_TIMERG_T* pstTimerG = (UTL_TIMERG_T*)TimerGID;
    UT_TIMER_T* pstTimer = NULL;
    
    if (pTimerID != NULL)
    {
        *pTimerID = 0;
    }

    if (func == NULL)
    {
        UT_ERROR(EOS_ERROR, "timer func expected!");
        return EOS_ERROR;
    }

    eos_sem_p(pstTimerG->sem, EOS_WAIT_FOREVER);
    
    if (eos_lst_count(&pstTimerG->lsFrees) > 0)
    {
        pstTimer = (UT_TIMER_T*)eos_lst_get(&pstTimerG->lsFrees);
    }
    else
    {
    #ifdef UTL_TIMER_DEBUG
        pstTimer = (UT_TIMER_T*)UT_MemAlloc(sizeof(UT_TIMER_T), fi, li);
    #else
        pstTimer = (UT_TIMER_T*)UT_MALLOC(sizeof(UT_TIMER_T));
    #endif
    
        if (NULL == pstTimer)
        {
            eos_sem_v(pstTimerG->sem);

            UT_ERROR(EOS_ERROR, "out of timer memory!");
            return EOS_ERROR;
        }
    }

    /* memset(pstTimer, 0, sizeof(UT_TIMER_T)); */

    pstTimer->iLength = len;
    pstTimer->iPeriod = EOS_MS2TICK(len);
    pstTimer->tvEndTime = tickGet() + pstTimer->iPeriod;

    pstTimer->enMode = mode;
    pstTimer->pfTimerCB = func;
    pstTimer->par1 = par1;
    pstTimer->par2 = par2;

    pstTimer->active = EOS_FALSE;
    pstTimer->running = EOS_FALSE;
    pstTimer->reset = EOS_TRUE;

#ifdef UTL_TIMER_DEBUG
    pstTimer->pcFuncName = funcName;
#else
    pstTimer->pcFuncName = "";
#endif

    utGtimerAddTimer(pstTimerG, pstTimer);

    eos_sem_v(pstTimerG->sem);

    if (NULL != pTimerID)
    {
        *pTimerID = (_UINT32)pstTimer;
    }

    return EOS_OK;    
}

_INT32 ut_timer_stop(EOS_TIMER_ID TimerGID,_UINT32 TimerID)
{
    UTL_TIMERG_T* pstTimerG = (UTL_TIMERG_T*)TimerGID;
    UT_TIMER_T* pstTimer = (UT_TIMER_T*)TimerID;

    /* user must ensure the timer is got from ut_timer_start() */

    eos_sem_p(pstTimerG->sem, EOS_WAIT_FOREVER);

    if (pstTimer->active)
    {        
        eos_lst_del(&pstTimerG->lsTimers, &pstTimer->node);
        eos_lst_add(&pstTimerG->lsFrees, &pstTimer->node);

        pstTimer->active = EOS_FALSE;
    }

    pstTimer->reset = EOS_TRUE;

    eos_sem_v(pstTimerG->sem);

    return EOS_OK;
}

_INT32 ut_timer_reset(EOS_TIMER_ID TimerGID, _UINT32 TimerID, _UINT32 ulNewLen)
{
    UTL_TIMERG_T* pstTimerG = (UTL_TIMERG_T*)TimerGID;
    UT_TIMER_T* pstTimer = (UT_TIMER_T*)TimerID;

    /* user must ensure the timer is in the group timer */

    eos_sem_p(pstTimerG->sem, EOS_WAIT_FOREVER);

    if (pstTimer->active)
    {
        eos_lst_del(&pstTimerG->lsTimers, &pstTimer->node);
    }

    if (ulNewLen > 0)
    {
        pstTimer->iLength = ulNewLen;
    }
    
    pstTimer->iPeriod = EOS_MS2TICK(pstTimer->iLength);
    pstTimer->tvEndTime = tickGet() + pstTimer->iPeriod;
        
    utGtimerAddTimer(pstTimerG, pstTimer);

    pstTimer->reset = EOS_TRUE;
    
    eos_sem_v(pstTimerG->sem);

    return EOS_OK;
}

_INT32 ut_timer_length(EOS_TIMER_ID TimerGID, _UINT32 TimerID)
{
    UT_TIMER_T* pstTimer = (UT_TIMER_T*)TimerID;

    return pstTimer->iLength;
}

_INT32 ut_timer_left(EOS_TIMER_ID TimerGID, _UINT32 TimerID)
{
    _INT32 iTicks = tickGet();
    UT_TIMER_T* pstTimer = (UT_TIMER_T*)TimerID;

    if (pstTimer->active)
    {
        if (iTicks < pstTimer->tvEndTime)
        {
            return (pstTimer->tvEndTime - iTicks) * 1000 / sysClkRateGet();
        }
    }

    return 0;
}

_INT32 ut_timer_trig(EOS_TIMER_ID TimerGID, _INT32 ulTime)
{
    UTL_TIMERG_T* pstTimerG = (UTL_TIMERG_T*)TimerGID;
    UT_TIMER_T* pstTimer;
    _UINT32 tv = tickGet();

    eos_sem_p(pstTimerG->sem, EOS_WAIT_FOREVER);

    while (eos_lst_count(&pstTimerG->lsTimers) > 0)
    {
        pstTimer = (UT_TIMER_T*)eos_lst_first(&pstTimerG->lsTimers);
        if (tv < pstTimer->tvEndTime)
        {
            /* no timer timeout */
            break;
        }
        
        /* clear the reset flag maybe set before */
        pstTimer->reset = EOS_FALSE;
        pstTimer->running = EOS_TRUE;

        eos_sem_v(pstTimerG->sem);

        /* invoke the timer's callback function */
        pstTimer->pfTimerCB(pstTimer->par1, pstTimer->par2);

        eos_sem_p(pstTimerG->sem, EOS_WAIT_FOREVER);
        
        pstTimer->running = EOS_FALSE;

        if (!pstTimer->reset)
        {
            /* the timer is not be stopped or reset */
            if (EN_TIMER_PERIODIC == pstTimer->enMode)
            {
                eos_lst_del(&pstTimerG->lsTimers, &pstTimer->node);
        
                /* restart the periodic timer */
                pstTimer->tvEndTime += pstTimer->iPeriod;
                utGtimerAddTimer(pstTimerG, pstTimer);
            }
            else
            {
                /* delete the oneshot timer */
                eos_lst_del(&pstTimerG->lsTimers, &pstTimer->node);
                eos_lst_add(&pstTimerG->lsFrees, &pstTimer->node);

                pstTimer->active = EOS_FALSE;
            }
        }
    }

    eos_sem_v(pstTimerG->sem);

    return EOS_OK;
}

_INT32 ut_timer_show(EOS_TIMER_ID TimerGID, EOS_INFO_PRINTF pPrintFunc, _VOID *pData)
{
    UTL_TIMERG_T* pstTimerG = (UTL_TIMERG_T*)TimerGID;
    UT_TIMER_T* pstTimer = (UT_TIMER_T*)eos_lst_first(&pstTimerG->lsTimers);
    _UINT32 now = tickGet();
    _CHAR8 buf[512];
    _INT32 len = 0;

    /* cancel the mutex to avoid waiting the mutex unlock when crashed in timer API */
    /* eos_sem_p(pstTimerG->sem, EOS_WAIT_FOREVER); */

    len = sprintf(buf, "timer %s: used(%d), free(%d)\r\n", 
                        pstTimerG->acName, 
                        eos_lst_count(&pstTimerG->lsTimers),
                        eos_lst_count(&pstTimerG->lsFrees));

    pPrintFunc(pData, buf);

    while (pstTimer != NULL)
    {
        len += sprintf(buf, "\t%d(%d)-%s-%p %s(%x, %x)-%s\r\n",
                    pstTimer->iLength,
                    ((pstTimer->tvEndTime > now) ? 
                        EOS_TICK2MS(pstTimer->tvEndTime - now) : 0), 
                    pstTimer->enMode ? "PERIODIC" : "ONESHOT",
                    pstTimer->pfTimerCB, pstTimer->pcFuncName,
                    pstTimer->par1, pstTimer->par2,
                    pstTimer->running ? "RUNNING" : "IDLE");

        pPrintFunc(pData, buf);

        pstTimer = (UT_TIMER_T*)eos_lst_next(&pstTimer->node);
    }

    /* eos_sem_v(pstTimerG->sem); */

    return len;
}

_VOID utGtimerAddTimer(UTL_TIMERG_T* pstTimerG, UT_TIMER_T* pstTimer)
{
    _LIST* plsTimerG = &pstTimerG->lsTimers;
    UT_TIMER_T* prev = NULL;

    /* check from tail because most new timer will timeout last */
    prev = (UT_TIMER_T*)eos_lst_last(plsTimerG);
    while (prev != NULL)
    {
        if (pstTimer->tvEndTime >= prev->tvEndTime)
        {
            eos_lst_insert(plsTimerG, &prev->node, &pstTimer->node);
            pstTimer->active = EOS_TRUE;
            pstTimer->running = EOS_FALSE;

            return;
        }

        prev = (UT_TIMER_T*)eos_lst_prev(&prev->node);
    }

    eos_lst_addfront(plsTimerG, &pstTimer->node);
    pstTimer->active = EOS_TRUE;
    pstTimer->running = EOS_FALSE;

    return;
}

#endif

#ifdef __cplusplus
}
#endif

