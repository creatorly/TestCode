EOS_DECLARE_HANDLE(HWDT);

/* this defination don't change    */
#define    EOS_SFTWDT_HANDLE_MAX                8
#define    EOS_SFTWDT_DEFAULT_PORT                2080


typedef enum tagEosWdtMsgT{
    enWdtMsgTick=0,
    enWdtMsgLog,
/*     add new msg type here and wdt only supports 8 message types    */
    
    enWdtMsgEnd
}EOSWdtMsgT;

typedef enum tagEosWdtIeNa{
    enWdtIeGeData=1,
    enWdtIeTskId,
    enWdtIeTskNa,
    enWdtIeTskTick,
    enWdtIeLogData
/* new ie add  from here    */    
}EOSWdtIeNa;

typedef enum tagEosWdtCmd{
    enWdtCmdRst
}EosWdtCmd;


typedef _INT32 (*EOSWDTFUNC)(_CHAR8*,_INT32,eosAddress*,_VOID* pData);

#if (EJOIN_OS_TYPE == EJOIN_OS_TYPE_VXWORKS)    
    typedef _INT32 (*EOSWDTSTARTFUNC)(_UINT32);
    typedef _INT32 (*EOSWDTKILLFUNC)(_UINT32);
#elif (EJOIN_OS_TYPE == EJOIN_OS_TYPE_LINUX)
    typedef _INT32 (*EOSWDTSTARTFUNC)(_CHAR8*);
    typedef _INT32 (*EOSWDTKILLFUNC)(pid_t);
#endif

typedef _INT32 (*EOSWDTIEADD)(_CHAR8*,EOSWdtMsgT,_VOID*);
typedef _INT32 (*EOSWDTCHECKFUNC)(_VOID*);

_INT32 eos_sftwdt_clnt_contruction(_USHORT16 usPort,_CHAR8* pNa,EOSWDTIEADD pfAddIe,LPHWDT lpWdt);

#if (EJOIN_OS_TYPE == EJOIN_OS_TYPE_VXWORKS)    
_INT32 eos_sftwdt_ser_cfg(HWDT hWdt,_CHAR8* pcNa,_UINT32 pPath,
        _UINT32 uiScnd,EOSWDTSTARTFUNC pfStart,EOSWDTKILLFUNC pfStop);
#elif (EJOIN_OS_TYPE == EJOIN_OS_TYPE_LINUX)
_INT32 eos_sftwdt_ser_cfg(HWDT hWdt,_CHAR8* pcNa,_CHAR8* pPath,
        _UINT32 uiScnd,EOSWDTSTARTFUNC pfStart,EOSWDTKILLFUNC pfStop);
#endif
_INT32 eos_sftwdt_ser_checkfunc_add(HWDT hWdt,EOSWDTCHECKFUNC pFc,_VOID* pData);
_INT32 eos_sftwdt_ser_contruction(_USHORT16 usPort,_UINT32 uiRs,LPHWDT lpWdt);
_INT32 eos_sftwdt_notify(HWDT hWdt,EOSWdtMsgT enMsgT,_VOID* pUsr);
_INT32 eos_sftwdt_ser_running(HWDT hWdt);
_INT32 eos_sftwdt_ser_set(HWDT hWdt,EosWdtCmd enCmd,_INT32 iVal,_CHAR8* pStr);

