
#ifndef __UT_EXC_H__
#define __UT_EXC_H__

#include "eos_statistics.h"

#if defined(__cplusplus)
extern "C" {
#endif

typedef _INT32 (*FP_UT_EXC_HANDLER)(_INT32 task, _VOID* data);

_INT32 UT_ExcInit(EOS_INFO_PRINTF dump, _VOID* data);
_INT32 UT_ExcHookAdd(FP_UT_EXC_HANDLER handler, _VOID* data);
_INT32 UT_ExcTaskHookAdd(_INT32 task, FP_UT_EXC_HANDLER handler, _VOID* data);

_INT32 UT_ExcStart(_VOID);

/* start a task to clear the watch dog, 
 *  @param iIntervalMs: clear watch dog intervals in ms
 *  @param n:           clear watch dog times, if 0 clear for ever
 *  @return:            the task id if success 
 */
_INT32 UT_ExcClrWdgStart(_INT32 iIntervalMs, _INT32 n);
_VOID UT_ExcClrWdgStop(_VOID);

#if defined(__cplusplus)
}
#endif

#endif


