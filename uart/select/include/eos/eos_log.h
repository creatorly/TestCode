EOS_DECLARE_HANDLE(HLOG);

typedef enum tagEosLogLPMod{
    EOSLOGSYNC=1,
    EOSLOGRST,    
    EOSLOGEND    
}EOSLOGLPMOD;


typedef _INT32 (*EOSLOGOPENFUNC)(EOSBool,_VOID*,_VOID**);
typedef _INT32 (*EOSLOGWRITFUNC)(_VOID*,_CHAR8*,_INT32);
typedef _INT32 (*EOSLOGSETFUNC)(_VOID*,EOSLOGLPMOD,_INT32,_CHAR8*);
typedef _INT32 (*EOSLOGGETFUNC)(_VOID*,EOSLOGLPMOD,_INT32*,_CHAR8*);
typedef _INT32 (*EOSLOGCLOSEFUNC)(_VOID*);
typedef _INT32 (*EOSLOGPRINTFUNC)(_VOID*,EOS_INFO_PRINTF,_VOID *);

typedef struct tagEosLogCfgSt{
    _INT32                iCachSize;
    EOSLOGOPENFUNC        pfOpen;        
    EOSLOGWRITFUNC        pfWr;
    EOSLOGSETFUNC        pfSet;
    EOSLOGGETFUNC        pfGet;
    EOSLOGCLOSEFUNC        pfClose;
    EOSLOGPRINTFUNC        pfPrnt;
/* 20070110    */    
    EOSBool                iSync;
    _VOID*                pDrvCfg;
    _INT32                iCfgSize;
}EosLogCfgSt;

_INT32 eos_log_construct(EosLogCfgSt* pstLogCfg,LPHLOG lpLog);
_INT32 eos_log_disstruct(HLOG pLog);
_INT32 eos_log_wr(HLOG pLog,_CHAR8* pcData,_INT32 iLen,EOSBool bSyn);
_INT32 eos_log_set(HLOG pLog,EOSLOGLPMOD enMod,_INT32 iData,_CHAR8* pcData);
_INT32 eos_log_show(HLOG pLog,EOS_INFO_PRINTF pPrintFunc,_VOID *pData);

_INT32 eos_filog_open(EOSBool iSync,_VOID* pData,_VOID** lpFiLog);
_INT32 eos_filog_write(_VOID* pFiLog,_CHAR8* pData,_INT32 iLen);
_INT32 eos_filog_set(_VOID* pFiLog,EOSLOGLPMOD enMod,_INT32 iVal,_CHAR8* pStr);
_INT32 eos_filog_get(_VOID* pFiLog,EOSLOGLPMOD enMod,_INT32* piVal,_CHAR8* pStr);
_INT32 eos_filog_close(_VOID* pFiLog);
_INT32 eos_filog_show(_VOID* pFiLog,EOS_INFO_PRINTF pPrintFunc,_VOID *pData);








