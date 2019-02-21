
#include <eos.h>
#include <ut_log.h>

#include "utl_list.h"
#include "ut_mem.h"
#include "ut_task.h"

#if defined(__cplusplus)
extern "C" {
#endif

#ifdef ENABLE_UT_TASK

#define UT_TASK_DELIMITERS      0x5aa5a55a
#define UT_TASK_RESERVED_BYTES  0x80

typedef void (*UT_TASK_FUNC)(const _VOID* a0,int a1,int a2,int a3,int a4,int a5,int a6,int a7,int a8,int a9);

typedef struct tagstUtilsTask
{
    utl_node_t node;
    
    const char* name;
    _INT32 tid;
    _INT32 stackSize;

    UT_TASK_FUNC fpMain;
    _VOID* arg1;                /* other args will be passed via function arguments */

    _INT32* pStackBottom;      /* the pointer to the statck bottom */
}ST_UT_TASK;

typedef struct tagstUtilsTaskMgr
{
    SEM_ID mutex;
    
    utl_list_t lsTasks;
}ST_UT_TASK_MGR;

typedef _INT32 (*FP_UT_TASK_MAIN)(ST_UT_TASK* pTask, _INT32 arg1, _INT32 arg2, _INT32 arg3, _INT32 arg4, 
                               _INT32 arg5, _INT32 arg6, _INT32 arg7, _INT32 arg8, _INT32 arg9);

#define CCM_TASK_LOCK(pTaskMgr)    semTake((pTaskMgr)->mutex, WAIT_FOREVER)
#define CCM_TASK_UNLOCK(pTaskMgr)  semGive((pTaskMgr)->mutex)

static ST_UT_TASK_MGR g_stUtTasks;

int utTaskWdgDump(ST_UT_TASK_MGR* pTaskMgr, EOS_INFO_PRINTF fpPrintf, _VOID* fd)
{
    utl_node_t* pNode;
    _INT32 len = 0;
    
    pNode = utl_list_begin(&pTaskMgr->lsTasks);
    
    while (pNode != utl_list_end(&pTaskMgr->lsTasks))
    {
        ST_UT_TASK* pTask = (ST_UT_TASK*)pNode;

        if (*(pTask->pStackBottom) != UT_TASK_DELIMITERS)
        {
            const _CHAR8* pcPrevTask = "null";
            const _CHAR8* pcNextTask = "null";
            _CHAR8 buf[256];
            
            if (utl_list_prev(pNode) != utl_list_end(&pTaskMgr->lsTasks))
            {
                pcPrevTask = ((ST_UT_TASK*)utl_list_prev(pNode))->name;
            }

            if (utl_list_next(pNode) != utl_list_end(&pTaskMgr->lsTasks))
            {
                pcNextTask = ((ST_UT_TASK*)utl_list_next(pNode))->name;
            }
            
            if (*(pTask->pStackBottom) != UT_TASK_DELIMITERS)
            {
                sprintf(buf, "task \"%s\" bottom overflow! "
                    "prev = \"%s\", next = \"%s\"\r\n", 
                    pTask->name, pcPrevTask, pcNextTask);

                len += fpPrintf(fd, buf);
            }
        }

        pNode = utl_list_next(pNode);
    }

    return len;
}

_INT32 utTaskDump(_VOID *pData, const _CHAR8 * cBuf)
{
    return UT_LogBuf(cBuf, strlen(cBuf));
}

/* add a vxWorks task */
_INT32 utTaskMgrAdd(ST_UT_TASK_MGR* pTaskMgr, ST_UT_TASK* pTask)
{
    utl_node_t* pnNext = utl_list_begin(&pTaskMgr->lsTasks);

    /* find the first task with a smaller bottom from the descending task list */
    while (pnNext != utl_list_end(&pTaskMgr->lsTasks))
    {
        ST_UT_TASK* ptNext = (ST_UT_TASK*)pnNext;

        if ((size_t)ptNext->pStackBottom < (size_t)pTask->pStackBottom)
        {
            break;
        }

        pnNext = utl_list_next(pnNext);
    }

    /* add the task to the front of the found task */
    utl_list_ins(&pTaskMgr->lsTasks, utl_list_prev(pnNext), &pTask->node);

    return utl_list_size(&pTaskMgr->lsTasks);
}

/* delete a vxWorks task */
_VOID utTaskMgrDel(ST_UT_TASK_MGR* pTaskMgr, ST_UT_TASK* pTask)
{
    utl_list_del(&pTaskMgr->lsTasks, &pTask->node);
}

void utTaskMain(ST_UT_TASK* pTask, _INT32 arg2, _INT32 arg3, _INT32 arg4, _INT32 arg5, 
               _INT32 arg6, _INT32 arg7, _INT32 arg8, _INT32 arg9, _INT32 arg10)
{
    _INT32 stackBottom;
    ST_UT_TASK_MGR* pTaskMgr = &g_stUtTasks;

    CCM_TASK_LOCK(pTaskMgr);

    pTask->tid = taskIdSelf();
    pTask->pStackBottom = &stackBottom;

    /* set the "bottom" and "top" of the stack */
    *(pTask->pStackBottom) = UT_TASK_DELIMITERS;

    utTaskMgrAdd(pTaskMgr, pTask);

    CCM_TASK_UNLOCK(pTaskMgr);

    if (pTask->fpMain != NULL)
    {
        pTask->fpMain(pTask->arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);
    }

    CCM_TASK_LOCK(pTaskMgr);

    utTaskMgrDel(pTaskMgr, pTask);
    UT_FREE(pTask);

    CCM_TASK_UNLOCK(pTaskMgr);
}

void utTaskWdgMain(ST_UT_TASK_MGR* pTaskMgr)
{
    CCM_TASK_LOCK(pTaskMgr);
    
    utTaskWdgDump(pTaskMgr, utTaskDump, pTaskMgr);
    
    CCM_TASK_UNLOCK(pTaskMgr);
}

_INT32 UT_TaskMgrInit()
{
    ST_UT_TASK_MGR* pTaskMgr = &g_stUtTasks;
    
    memset(pTaskMgr, 0, sizeof(ST_UT_TASK_MGR));
    
    pTaskMgr->mutex = semMCreate(0);
    if (NULL == pTaskMgr->mutex)
    {
        printf("UT_TaskInit: failed to initialize mutex!\r\n");
        return -2;
    }

    utl_list_init(&pTaskMgr->lsTasks);

#ifdef __VXWORKS_OS__
    eos_wdg_add_timer(1000, (_VOID(*)(_VOID*))utTaskWdgMain, pTaskMgr);
#endif

    return EOS_OK;
}

_VOID UT_TaskMgrFree()
{
}

_INT32 UT_TaskMgrStat(EOS_INFO_PRINTF fpPrintf, _VOID* fd)
{
    ST_UT_TASK_MGR* pTaskMgr = &g_stUtTasks;

    char buf[256];
    _INT32 len = 0;

    len = sprintf(buf, "%s", "UT Task Statistics:\r\n");
    fpPrintf(fd, buf);

    CCM_TASK_LOCK(pTaskMgr);

    len += utTaskWdgDump(pTaskMgr, fpPrintf, fd);

    CCM_TASK_UNLOCK(pTaskMgr);

    return len;
}

/* create a vxWorks task */
_INT32 UT_TaskSpawn(const char *name, _INT32 priority, 
                    _INT32 options, _INT32 stackSize, FUNCPTR entryPt, 
                    _VOID* arg1, _INT32 arg2, _INT32 arg3, _INT32 arg4, _INT32 arg5, 
                    _INT32 arg6, _INT32 arg7,_INT32 arg8, _INT32 arg9, _INT32 arg10)
{
    ST_UT_TASK* pTask = NULL;
    _INT32 tid = EOS_ERROR;

    pTask = UT_TALLOC(ST_UT_TASK);
    if (NULL == pTask)
    {
        printf("UT_TaskSpawn: failed to malloc ST_UT_TASK!\r\n");
        return EOS_ERROR;
    }

    memset(pTask, 0, sizeof(ST_UT_TASK));
    
    pTask->name = name;
    pTask->stackSize = stackSize;
    pTask->fpMain = (UT_TASK_FUNC)entryPt;
    pTask->arg1 = arg1;

    /* increase 128 bytes, avoid to stack overflow */
    tid = taskSpawn(name, priority, options, 
                    stackSize + UT_TASK_RESERVED_BYTES, 
                    (FUNCPTR) utTaskMain, pTask, arg2, 
                    arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);
    if (EOS_ERROR == tid)
    {
        UT_FREE(pTask);
        
        printf("UT_TaskSpawn: failed to spawn task \"%s\"!\r\n", name);
        return -1;
    }
    
    /* DO NOT access the pTask again because it may be destroyed in utTaskMain() */

    return tid;
}

#else

_INT32 UT_TaskMgrInit()
{
    return EOS_OK;
}

_VOID UT_TaskMgrFree()
{
}

_INT32 UT_TaskMgrStat(EOS_INFO_PRINTF fpPrintf, _VOID* fd)
{
    return 0;
}

/* create a vxWorks task */
_INT32 UT_TaskSpawn(const char *name, _INT32 priority, 
                    _INT32 options, _INT32 stackSize, FUNCPTR entryPt, 
                    _VOID* arg1, _INT32 arg2, _INT32 arg3, _INT32 arg4, _INT32 arg5, 
                    _INT32 arg6, _INT32 arg7,_INT32 arg8, _INT32 arg9, _INT32 arg10)
{
    return taskSpawn(name, priority, options, stackSize, entryPt,
                     arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);
}

#endif

#if defined(__cplusplus)
}
#endif

