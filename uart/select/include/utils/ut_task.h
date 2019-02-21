
#ifndef __UT_TASK_H__
#define __UT_TASK_H__

#include "eos_statistics.h"

#if defined(__cplusplus)
extern "C" {
#endif

#undef ENABLE_UT_TASK

_INT32 UT_TaskMgrInit();
_VOID UT_TaskMgrFree();

_INT32 UT_TaskMgrStat(EOS_INFO_PRINTF fpPrintf, _VOID* fd);

/* create a vxWorks task */
_INT32 UT_TaskSpawn(const char *name, _INT32 priority, 
                    _INT32 options, _INT32 stackSize, FUNCPTR entryPt, 
                    _VOID* arg1, _INT32 arg2, _INT32 arg3, _INT32 arg4, _INT32 arg5, 
                    _INT32 arg6, _INT32 arg7,_INT32 arg8, _INT32 arg9, _INT32 arg10);

#if defined(__cplusplus)
}
#endif

#endif

