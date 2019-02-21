
#ifndef UT_RTSTAT_H
#define UT_RTSTAT_H

#include <utl_log.h>

#if defined(__cplusplus)
extern "C" {
#endif

typedef _VOID* H_UT_RTS;

typedef _VOID* (*FP_UTRTS_MALLOC)(_UINT32 uiSize);
typedef _VOID (*FP_UTRTS_FREE)(_VOID* pMem);

typedef _INT32 UTL_CHK_FMTARGS(2) (*FP_UT_RTS_PRINTF)(const _VOID* usrData, const _CHAR8* fmt, ...);

_UINT32 UTRTS_Construct(H_UT_RTS* phRts, 
                        const _CHAR8* pcFile,
                        _UINT32 uiLine,
                        FP_UT_RTS_PRINTF fpPrint,
                        _VOID* data);

_UINT32 UTRTS_Destruct(H_UT_RTS hRts);

_UINT32 UTRTS_Begin(H_UT_RTS hRts, const _CHAR8* file, _INT32 line);
_UINT32 UTRTS_End(H_UT_RTS hRts, const _CHAR8* file, _INT32 line);
_VOID UTRTS_Show(H_UT_RTS hRts, FP_UT_RTS_PRINTF fpPrint, const _VOID* data);

#if defined(__cplusplus)
}
#endif

#endif

