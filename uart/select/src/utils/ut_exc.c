
#include "eos.h"

#include "ut_debug.h"
#include "ut_mem.h"
#include "ut_exc.h"
#include "ut_exc_.h"

#if defined(__cplusplus)
extern "C" {
#endif

#ifdef __VXWORKS_OS__

_INT32 UT_ExcInit(EOS_INFO_PRINTF dump, _VOID* data)
{
    memset(&g_stUtExc, 0, sizeof(g_stUtExc));
    g_stUtExc.dump = dump;
    g_stUtExc.data = data;

    g_stUtExc.mutex = semMCreate(0);
    g_stUtExc.sem = semBCreate(0, SEM_EMPTY);

    excHookAdd((FUNCPTR)utExcHandler);

    return EOS_OK;
}

_INT32 UT_ExcHookAdd(FP_UT_EXC_HANDLER handler, _VOID* data)
{
    return UT_ExcTaskHookAdd(taskIdSelf(), handler, data);
}

_INT32 UT_ExcTaskHookAdd(_INT32 task, FP_UT_EXC_HANDLER handler, _VOID* data)
{
    _INT32 i = 0;
    
    semTake(g_stUtExc.mutex, EOS_WAIT_FOREVER);

    for (i = 0; i < EOS_NELEMENTS(g_stUtExc.handlers); i++)
    {
        if ((0 == g_stUtExc.handlers[i].task)
            || (task == g_stUtExc.handlers[i].task))
        {
            g_stUtExc.handlers[i].task = task;
            g_stUtExc.handlers[i].func = handler ? handler : utExcDefHandler;
            g_stUtExc.handlers[i].data = data;

            break;
        }
    }

    semGive(g_stUtExc.mutex);

    if (i == EOS_NELEMENTS(g_stUtExc.handlers))
    {
        UT_ERROR(EOS_ERROR, "too many task handler registered!\r\n");
        return EOS_ERROR;
    }

    return EOS_OK;
}

_INT32 UT_ExcStart(_VOID)
{
    /* start the exception guard task */
    UT_TaskSpawn("tExcWdg", 50, 0, 32*1024, 
        (FUNCPTR) utExcWdgMain, (_INT32)&g_stUtExc, 0, 0, 0, 0, 0, 0, 0, 0, 0);

    return EOS_OK;
}

_INT32 UT_ExcClrWdgStart(_INT32 iIntervalMs, _INT32 n)
{
    _INT32 taskPriority = 80;
    _INT32 ownTask = taskIdSelf();
    _INT32 wdgTask = 0;
    _INT32 i = 0;
    
    taskPriorityGet(ownTask, &taskPriority);

    /* the task priority for clearing watch dog should be priorior to current task */
    if (taskPriority > 1)
    {
        taskPriority--;
    }
        
    wdgTask = UT_TaskSpawn("excClrWdg", taskPriority, 0, 20*1024, 
                        (FUNCPTR) excClrWdgMain, iIntervalMs, n, 
                        0, 0, 0, 0, 0, 0, 0, 0);
    if (wdgTask == ERROR)
    {
        UT_ERROR(EOS_ERROR, "start exception wdg failed!\r\n");
        return EOS_ERROR;
    }

    semTake(g_stUtExc.mutex, EOS_WAIT_FOREVER);
    
    /* save the watch dog task id */
    for (i = 0; i < EOS_NELEMENTS(g_stUtExc.wdgTasks); i++)
    {
        if ((0 == g_stUtExc.wdgTasks[i].task)
            || (ownTask == g_stUtExc.wdgTasks[i].task))
        {
            g_stUtExc.wdgTasks[i].task = ownTask;
            g_stUtExc.wdgTasks[i].wdgTask = wdgTask;
            break;
        }
    }
    
    semGive(g_stUtExc.mutex);

    return EOS_OK;
}

_VOID UT_ExcClrWdgStop(_VOID)
{
    _INT32 ownTask = taskIdSelf();
    _INT32 wdgTask = 0;
    _INT32 i = 0;
    
    semTake(g_stUtExc.mutex, EOS_WAIT_FOREVER);

    /* get the watch dog task id */
    for (i = 0; i < EOS_NELEMENTS(g_stUtExc.wdgTasks); i++)
    {
        if (ownTask == g_stUtExc.wdgTasks[i].task)
        {
            wdgTask = g_stUtExc.wdgTasks[i].wdgTask;
            break;
        }
    }
    
    semGive(g_stUtExc.mutex);

    if (EOS_OK == taskIdVerify(wdgTask))
    {
        taskDelete(wdgTask);
    }
}

_VOID utExcHandler(_INT32 task, _INT32 vecNum, ESF* esf)
{
    semTake(g_stUtExc.mutex, EOS_WAIT_FOREVER);
    
    g_stUtExc.task = task;
    g_stUtExc.vecNum = vecNum;
    g_stUtExc.esf = esf;

    semGive(g_stUtExc.mutex);

    /* release the sem to wakeup the guard task */
    semGive(g_stUtExc.sem);
}

_VOID utExcWdgMain(ST_UT_EXC* exc)
{
    _CHAR8* buf = NULL;
    _INT32 size = 8*1024;
    _INT32 i = 0;

    buf = (_CHAR8*)UT_MALLOC(size);
    if (NULL == buf)
    {
        printf("out of memory(%d)!\r\n", size);
        return;
    }

    while (1)
    {
        _VOID* sp = NULL;
        _VOID* pc = NULL;
        ST_UT_EXC_HANDLER stHdr;

        memset(&stHdr, 0, sizeof(stHdr));
        buf[0] = '\0';

        /* waiting for a exception ocurred */
        semTake(exc->sem, WAIT_FOREVER);

        semTake(exc->mutex, EOS_WAIT_FOREVER);

    #if (EOS_ARCH_ENDIAN == EOS_ARCH_LITTLE_ENDIAN)
        sp = exc->esf;
        pc = exc->esf->pc;
    #elif (EOS_ARCH_ENDIAN == EOS_ARCH_BIG_ENDIAN)
        sp = exc->esf->spCopy;
        pc = exc->esf->regSet.pc;
    #endif

        for (i = 0; i < EOS_NELEMENTS(exc->handlers); i++)
        {
            if (exc->task == exc->handlers[i].task)
            {
                /* save the exception handler */
                memcpy(&stHdr, &exc->handlers[i], sizeof(stHdr));
                break;
            }
            else if (exc->handlers[i].task <= 0)
            {
                break;
            }
        }
            
        semGive(exc->mutex);

        utExcInfoGet(sp, buf, size);
        utExcInfoDump(exc, buf);

        /* because may delay or crash, analyze stack after semGive(exc->mutex) */
        #ifdef ACOM_DEBUG
        excClrWdgMain(100, 10);
    
        utExcAnalyzeStack(sp, pc, buf, size);
        utExcInfoDump(exc, buf);
        #endif

        /* to ensure the exception information sent to log server, 
           clear watch dog every 100ms within 1s */
        excClrWdgMain(100, 10);
        
        /* resume the system or task */
        if (stHdr.func != NULL)
        {
            stHdr.func(stHdr.task, stHdr.data);
        }
    }

    UT_FREE(buf);
}

_INT32 utExcDefHandler(_INT32 task, _VOID* data)
{
    return taskRestart(task);
}

_INT32 utExcInfoGet(_VOID* sp, _CHAR8* buf, _INT32 size)
{
    _UINT32* pc;
    _UINT32 cpsr;
    _INT32 len = 0;
    
#if (EOS_ARCH_ENDIAN == EOS_ARCH_LITTLE_ENDIAN)
    pc = (_UINT32*)g_stUtExc.esf->pc;
    cpsr = g_stUtExc.esf->cpsr;
#elif (EOS_ARCH_ENDIAN == EOS_ARCH_BIG_ENDIAN)
    pc = (_UINT32*)g_stUtExc.esf->regSet.pc;
    cpsr = g_stUtExc.esf->regSet.msr;
#endif

    len = sprintf(buf, "\r\n\r\n");

    len += ASYS_GetSysInfo(buf+len, size-len);
    
    len += sprintf(buf+len, "\r\ntask \"%s\" had a exception! "
        "(PC = %p, CPSR = 0x%x, vecNum = 0x%x)\r\n\r\n", 
        taskName(g_stUtExc.task), pc, cpsr, g_stUtExc.vecNum);

    len += sprintf(&buf[len], "\r\n");

    return len;
}

#ifdef ACOM_DEBUG

_INT32 utExcAnalyzeStack(_VOID* sp, _VOID* pc, _CHAR8* buf, _INT32 size)
{
    ST_UT_SF* sf = (ST_UT_SF*)sp;
    _INT32 len = 0;
    _INT32 maxDeepth = 10;
    _INT32 n = 0;
    
    /* to avoid dead loop */
    _INT32 iMaxTicks = tickGet() + 10;

    buf[0] = '\0';

#if (EOS_ARCH_ENDIAN == EOS_ARCH_LITTLE_ENDIAN)
    /* get the exception sp, always can get one */
    while ((sf->ip != sf->sp) && (tickGet() < iMaxTicks))
    {
        sf = (ST_UT_SF*)&sf->ip;
    }

    iMaxTicks = tickGet() + 10;
    
    /* get the exception fp */
    sf = (ST_UT_SF*)sf->ip;
    while ((sf->ip != sf->sp) && (tickGet() < iMaxTicks))
    {
        sf = (ST_UT_SF*)&sf->ip;
    }

    if (tickGet() > iMaxTicks)
    {
        len += sprintf(&buf[len], "the stack is broken!\r\n");
        return len;
    }
    
    len += sprintf(&buf[len], "$%d: %p", n++, pc);
    len += sprintf(&buf[len], "(%p, %p, %p, %p)\r\n", 
        (&sf->fp)[-1], (&sf->fp)[-2], (&sf->fp)[-3], (&sf->fp)[-4]);

    while ((n < maxDeepth) 
        && (sf->fp > sf->sp) && (sf->fp[-2] == (_UINT32)&sf->fp[1]))
    {
        len += sprintf(&buf[len], "$%d: %p", n++, sf->lr);

        /* get next fp */
        sf = (ST_UT_SF*)&sf->fp[-3];

        len += sprintf(&buf[len], "(%p, %p, %p, %p)\r\n", 
            (&sf->fp)[-1], (&sf->fp)[-2], (&sf->fp)[-3], (&sf->fp)[-4]);
    }
    
#elif (EOS_ARCH_ENDIAN == EOS_ARCH_BIG_ENDIAN)
    len += sprintf(&buf[len], "$%d: %p", n++, pc);

    /* the exception has not returned address */
    len += sprintf(&buf[len], "(%p, %p, %p, %p)\r\n", 
        sf->args[-1], sf->args[0], sf->args[1], sf->args[2]);

    while ((n < maxDeepth)
        && (sf->sp > (_UINT32*)&sf->sp) && (sf->sp == (_UINT32*)sf->sp[-1]))
    {
        sf = (ST_UT_SF*)sf->sp;

        len += sprintf(&buf[len], "$%d: %p", n++, sf->lr);

        len += sprintf(&buf[len], "(%p, %p, %p, %p)\r\n", 
            sf->args[0], sf->args[1], sf->args[2], sf->args[3]);
    }
#endif

    len += sprintf(&buf[len], "\r\n");

    return len;
}

#endif

_INT32 utExcInfoDump(ST_UT_EXC* exc, _CHAR8* buf)
{
    _INT32 len;
    
    if (exc->dump)
    {
        len = exc->dump(exc->data, buf);
    }
    else
    {
        len = printf("\r\n%s\r\n", buf);
    }

    return len;
}

_VOID excClrWdgMain(_INT32 interval, _INT32 n)
{
    _INT32 iDelayTicks;
    
    if (interval <= 0)
    {
        /* clear watch dog one timers every 500 ms default */
        interval = 500;
    }

    iDelayTicks = interval*sysClkRateGet()/1000;
    
    if (n > 0)
    {
        while (n-- > 0)
        {
            taskDelay(iDelayTicks);
        }
    }
    else
    {
        while (1)
        {
            taskDelay(iDelayTicks);
        }
    }
}

#else

_INT32 UT_ExcInit(EOS_INFO_PRINTF dump, _VOID* data)
{
    return EOS_OK;
}

_INT32 UT_ExcHookAdd(FP_UT_EXC_HANDLER handler, _VOID* data)
{
    return EOS_OK;
}

_INT32 UT_ExcTaskHookAdd(_INT32 task, FP_UT_EXC_HANDLER handler, _VOID* data)
{
    return EOS_OK;
}

_INT32 UT_ExcStart(_VOID)
{
    return EOS_OK;
}

/* start a task to clear the watch dog, 
 *  @param iIntervalMs: clear watch dog intervals in ms
 *  @param n:           clear watch dog times, if 0 clear for ever
 *  @return:            the task id if success 
 */
_INT32 UT_ExcClrWdgStart(_INT32 iIntervalMs, _INT32 n)
{
    return EOS_OK;
}

_VOID UT_ExcClrWdgStop(_VOID)
{
}

#endif

#if defined(__cplusplus)
}
#endif

