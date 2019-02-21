#ifndef __EOS_RTDBG_H
#define __EOS_RTDBG_H

#ifdef __cplusplus
extern "C" {
#endif

EOS_DECLARE_HANDLE(HRTDBG);

_UINT32 eos_rtdbg_construct(IN _CHAR8* pcRtNa,OUT LPHRTDBG lpRtDbg);
_UINT32 eos_rtdbg_start(IN HRTDBG hRtDbg);
_UINT32 eos_rtdbg_stop(IN HRTDBG hRtDbg,IN _UINT32* puiRecord,IN EOS_INFO_PRINTF pPrintFunc,IN _VOID *pData);

#ifdef __cplusplus
}
#endif
#endif

