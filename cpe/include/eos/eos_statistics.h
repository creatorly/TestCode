#ifndef __EOS_STATISTICS_H
#define __EOS_STATISTICS_H


#if defined(__cplusplus)
extern "C" {
#endif

/* v.009    */
typedef    _INT32 (*EOS_INFO_PRINTF)(_VOID *pData, const _CHAR8 * cBuf);

/* young add for show of list and hash    */
typedef struct tagEosShowSt{
    _VOID*                pData;
    EOS_INFO_PRINTF        pFunc;
    _VOID*                pPads;
}EosShowSt;

/* v.009    */
_INT32 eos_statistics_info(EOS_INFO_PRINTF pPrintFunc,_VOID *pData);
_INT32 eos_info_default_print(_VOID *pData, const _CHAR8 * cBuf);

#define    EOS_DEFAULT_INFO_PRINT_FUNC            eos_info_default_print

#if defined(__cplusplus)
}
#endif 
#endif

