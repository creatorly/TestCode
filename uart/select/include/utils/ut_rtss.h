
#ifndef UT_RTSS_H
#define UT_RTSS_H

#include <utils/ut_rtstat.h>

#if defined(__cplusplus)
extern "C" {
#endif

typedef _VOID* H_UT_RTSS;

_UINT32 UT_RTSS_Construct(H_UT_RTSS* phRtss);
_UINT32 UT_RTSS_Destruct(H_UT_RTSS hRtss);

_VOID* UT_RTSS_Add(H_UT_RTSS hRtss, 
                    const _CHAR8* pcFile,
                    _UINT32 uiLine,
                    _VOID* fpPrint,
                    _VOID* data);

_UINT32 UT_RTSS_Del(H_UT_RTSS hRtss, _VOID* hRts);
_VOID UT_RTSS_Show(H_UT_RTSS hRtss, FP_UT_RTS_PRINTF fpPrint, _VOID* data);

#if defined(__cplusplus)
}
#endif

#endif

