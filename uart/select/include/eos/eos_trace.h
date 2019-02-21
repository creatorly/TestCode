#ifndef __EOS_TRACE_H
#define __EOS_TRACE_H


#if defined(__cplusplus)
extern "C" {
#endif



/* 20070111    */
typedef enum tagEosTraceCmd{
    enEosTraAddOut,
    enEosTraDelOut
}EosTraCmd;

typedef _INT32 (*EOSTRACEOUT)(_CHAR8*,_VOID*);

_INT32    eos_trace_init();
_INT32 eos_trace_registration(const _CHAR8* pcMName, const _CHAR8* pcSMName, _INT32 level);
_INT32 eos_trace_out(const _CHAR8 *pcMName,const _CHAR8 *pcSMName,_UCHAR8 ucLevel,const _CHAR8*pcInfo,_INT32 lErrorNum,const _CHAR8* cFile,_INT32 lLine);
_INT32 eos_trace_unregistration(_CHAR8 *pcMName,_CHAR8 *pcSMName,_CHAR8 cLevel);
_INT32 eos_trace_show(EOS_INFO_PRINTF pPrintFunc,_VOID *pData);
_INT32    eos_trace_print(EOS_INFO_PRINTF pPrintFunc,_VOID *pData);
_INT32 eos_trace_set_level(_UCHAR8 ucLevel);
_INT32 eos_trace_set(EosTraCmd enCmd,_INT32 iVal,_CHAR8* pStr);

#define    EOS_DEBUG(Name,sName,Level,Info,Err)    eos_trace_out(Name,sName,Level,Info,Err,__FILE__,__LINE__)    
#if defined(__cplusplus)
}
#endif 
#endif
