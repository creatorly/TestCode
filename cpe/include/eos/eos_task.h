
#ifndef __EOS_TASK_H
#define __EOS_TASK_H

#include <eos_typedef.h>
#include <eos_statistics.h>

#if defined(__cplusplus)
extern "C" {
#endif

typedef _INT32 TASK_ID;

TASK_ID eos_task_start(const _CHAR8* pcName,_INT32 iPriority,_UINT32 stackSize,_VOID* pFuncAddress, _VOID* pFuncData);
_INT32 eos_task_sleep(_UINT32 uiTimems);
_INT32 eos_task_stop(TASK_ID tid);
_INT32 eos_task_show(EOS_INFO_PRINTF pPrintFunc,_VOID *pData);

_INT32 eos_task_init();
_INT32 eos_task_set_pidfile(const char* pidfile);
_INT32 eos_task_set_priority(TASK_ID tid, _INT32 iPriority);

_INT32 eos_task_save_pid(const _CHAR8* pidfile);

#if (EJOIN_OS_TYPE != EJOIN_OS_TYPE_VXWORKS)

typedef int (*FUNCPTR)();

int taskSpawn(const char *name, 
              int priority, int options, int stackSize,
              FUNCPTR entryPt, void* arg1, int arg2, int arg3,
              int arg4, int arg5, int arg6, int arg7,
              int arg8, int arg9, int arg10);

int taskDelete(int tid);
int taskRestart(int tid);

const char* taskName(int tid);
const char* taskNameByPid(int pid);

int taskDelay(int ticks);
int taskIdSelf();
int taskIdVerify(int tid);

#endif

const char* taskNameByPid(int pid);
int taskStackSize();
int taskStackLength();

#if defined(__cplusplus)
}
#endif

#endif

