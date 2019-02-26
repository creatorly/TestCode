#ifndef __EOS_IODRV_H
#define __EOS_IODRV_H

#ifdef __cplusplus
extern "C" {
#endif

typedef _INT32 (*EOSDRVGETFUNC)(_INT32,_INT32,_INT32*,_CHAR8*);
typedef _INT32 (*EOSDRVSETFUNC)(_INT32,_INT32 ,_INT32 ,_CHAR8 *);
typedef _INT32 (*EOSDRVWRITEFUNC)(_INT32 ,_INT32 ,_CHAR8 *);
typedef _INT32 (*EOSDRVREADFUNC)(_INT32 ,_INT32 * ,_CHAR8 *);
typedef _INT32 (*EOSDRVINITFUNC)(_INT32 );

_INT32    eos_iodrv_init();
_INT32 eos_iodrv_load(const _CHAR8 *pcName,EOSDRVINITFUNC pfInit,EOSDRVREADFUNC pfRead,
    EOSDRVWRITEFUNC pfWrite,EOSDRVSETFUNC pfSet,EOSDRVGETFUNC pfGet);
_INT32 eos_iodrv_unload(_INT32 lIoIndex);
_INT32 eos_iodrv_open(_INT32 lIoIndex,_INT32 lParam);
_INT32 eos_iodrv_read(_INT32 lIoIndex,_INT32 lIndex,_INT32 * plLen,_CHAR8 *cValue);
_INT32 eos_iodrv_write(_INT32 lIoIndex,_INT32 lIndex,_INT32 lLen,_CHAR8 *cValue);
_INT32 eos_iodrv_set(_INT32 lIoIndex,_INT32 lType,_INT32 lIndex,_INT32 lLen,_CHAR8 *cValue);
_INT32 eos_iodrv_get(_INT32 lIoIndex,_INT32 lType,_INT32 lIndex,_INT32 *lLen,_CHAR8 *cValue);
_INT32 eos_iodrv_show(EOS_INFO_PRINTF pPrintFunc,_VOID *pData);

#ifdef __cplusplus
}
#endif
#endif











