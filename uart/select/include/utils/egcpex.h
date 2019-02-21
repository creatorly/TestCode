#ifndef __EGCPEX_H
#define __EGCPEX_H

#if defined(__cplusplus)
extern "C" {
#endif

#define    EGCP_OS_MALLOC                    malloc
#define    EGCP_OS_FREE                    free

#define    EGCP_TRANS_MAX                    64
#define    EGCP_MEM_SIZE                    EOS_MEM_SIZE_1024
#define    EGCP_MEM_MAX                    EGCP_TRANS_MAX*2

#define    EGCP_HASH_TRANS_KEY_MAX            EGCP_TRANS_MAX/2
#define    EGCP_HASH_TRANS_ELEM_MAX        EGCP_TRANS_MAX/2

#define    EGCP_HASH_TRANS_KEY_SIZE        4
#define    EGCP_HASH_TRANS_ELEM_SIZE        4


#define    EGCP_MEM_NAME                    "egcp-mem"
#define    EGCP_HASH_NAME                    "egcp_hash"
#define    EGCP_TIEMR_NAME                    "egcp_timer"
#define    EGCP_SEM_NAME                    "egcp_sem"

#define    EGCP_DEBUG_NAME                    "EGCP"

#define    EGCP_DEBUG_PARAMTER_ERROR        "function error\r\n"

#define    EGCP_SOCK_INFO                    "SOCK"
#define    EGCP_HASH_INFO                    "HASH"
#define    EGCP_TRANS_INFO                    "TRANS"


#define    EGCP_EOS_TRACE_INCLUDE        

#ifdef     EGCP_EOS_TRACE_INCLUDE
#define    EGCP_DEBUG_INFO(pStr,lError)    \
            eos_trace_out(EGCP_DEBUG_MODULE_NAME,NULL,EGCP_DEBUG_LEVEL,pStr,lError,__FILE__,__LINE__);
#else
#define    EGCP_DEBUG_INFO(pStr,lError)    \
            printf("%s:(%s)(%d)%s", EGCP_DEBUG_MODULE_NAME, \
            __FILE__, __LINE__,pStr);
#endif



#define EGCP_DEBUG_ERROR(_S,_E)            \
        eos_trace_out(EGCP_DEBUG_NAME,NULL,0,_S,_E,__FILE__,__LINE__);

#define    EGCP_ZERO_NULL_CHECK(_x)        \
if(_x==NULL)    \
    {    \
        EGCP_DEBUG_ERROR(EGCP_DEBUG_PARAMTER_ERROR,EGCP_DEBUG_LEVEL)    \
        return EOS_ERROR;    \
    }    

#if 0    
#define    EGCP_MALLOC(memId)                EOS_MALLOC(memId, EGCP_MEM_SIZE)
#else
#define    EGCP_MALLOC(memId)                \
    EOS_MALLOC(memId, EGCP_MEM_SIZE);        \
{    \
    _CHAR8 cBuf[128];    \
    sprintf(cBuf,"malloc\r\n");    \
    EGCP_DEBUG_INFO(cBuf, 0)    \
}    
#endif

#if 0
#define    EGCP_FREE(memId,memAddr)        EOS_FREE(memId,memAddr)
#else
#define    EGCP_FREE(memId,memAddr)        \
    {    \
        _CHAR8 cBuf[128];    \
        sprintf(cBuf,"H%x\r\n",(_UINT32)memAddr);    \
        EGCP_DEBUG_INFO(cBuf, 0)    \
        EOS_FREE(memId,memAddr);    \
    }    
#endif

typedef enum{
    EnTInit =0,
    EnTSndSnding =1,
    EnTSndProceeding =2,
    EnTSndCompleted =3,

    EnTRcvProceeding =1,
    EnTRcvCompleted =2,
    EnTRcvComfirmed =3,
    EnTTerminated =4,
    EnTEgcpStateEnd
}EgcpTransState;

typedef enum{
    EnEgcpTransAddHsh,
    EnEgcpTransDelHsh,
    EnEgcpTransApp,
    EnEgcpTransAddSndLst,
    EnEgcpTransDelSndLst,
    EnEgcpTransAddRcvLst,
    EnEgcpTransDelRcvLst,

    EnEgcpTransRcvAckRes,
    EnEgcpTransSndAckRes,

    EnEgcpTransType,
    EnEgcpTransReAddr,

    EnEgcpTransRcvMsg,
    EnEgcpTransSndMsg,

    EnEgcpTransEnd
}EgcpTransParam;


typedef enum{
    EnTransTSnd=0,
    EnTransTRcv,
    EnTransEnd
}EgcpTransType;    

typedef struct tagEgcpMsgHdSt{
    _NODE        stTransNode;
    EOSBool     bState; 
    _UINT32     stShotOneTiD;
    _UINT32     stPeriodTiD;
    eosAddress    stReAddr;
    _VOID*        pTrans;
    _VOID*        pstResMsg;
    _UINT32        uiSeq;
}EgcpMsgHdSt;

typedef enum{
    EnEgcpMsgIn =0,
    EnEgcpMsgOut,
    EnEgcpTimeOut,
    EnEgcpDirectEnd
}EgcpMsgDirect;

typedef struct tagEgcpMsgSt{
    EgcpMsgHdSt        stEgcpMsgHd;
    _CHAR8            data[0];        
}EgcpMsgSt;

#define    EGCP_MSG_HEADER_SIZE            EOS_ALIGN32(sizeof(EgcpMsgHdSt))

#define    MEMBUF_GET_MSG(_x)        ((_CHAR8*)_x+EGCP_MSG_HEADER_SIZE)
#define    MSG_GET_MEMBUF(_x)        ((_CHAR8*)_x-EGCP_MSG_HEADER_SIZE)

typedef _INT32 (*EgcpTransPkgFormFunc)(IN HEGCPTRANS pTrans,IN EgcpMsgType enMsgT,
    IN _CHAR8* pcBuf,IN _INT32 iLen,IN EgcpMsgSt* pstMsg,OUT _CHAR8** ppcPkg);
typedef _INT32 (*EgcpTransPhySndFunc)(IN HEGCPTRANS pTrans,IN _CHAR8* pcBuf,IN _INT32 iLen);


typedef    struct    tagEgcpTransApp{
    _LIST                    stTransIdleLst;

    EOS_MEM_ID                stMemId;
    EOS_TIMER_ID            stTimerId;
    EOS_SEM_ID                stSemId;

    EOSHHASH                pstTransHsh;    
    EgcpTransPhySndFunc        pfTransSndFunc;
    EgcpTransPkgFormFunc    pfTransPkgFunc;
    _INT32                    iTransMax;

/* rev app callback    */
    pfEgcpRcvReqFunc        pfAppRcvFunc;

    eosAddress                stReAddr;


    _UINT32                    uiSidVal;
        
    eosAddress                stLocalIp;
    eosSocket                stSock;    
    _VOID*                    pStartAddr;

/* user data    */
    _VOID*                    pData;
    
}EgcpTransAppSt;

typedef struct tagEgcpTrans{
    _NODE                stTransNode;
    HEGCP                hEgcp;
    EgcpTransAppSt*        pstApp;

    _LIST                stSndMsgLst;
    _LIST                stRcvMsgLst;
/* egcp transaction    */
    EgcpTransState        enTransState;
    EgcpTransType        enTransT;
    EgcpEvent            enEvent;

/* timer id    */
    _UINT32                pstStateTId;
    _UINT32                pstSndTiId;

    EgcpMsgSt*            pvSndMsg;
    EgcpMsgSt*            pvRcvMsg;

/* trans info    */
    eosAddress            stReAddr;
    _UINT32                uiTransSId;
    _UINT32                uiTransDId;
    _UINT32                uiSeq;
    _UINT32                uiSeqCnt;
    _UINT32                uiCreateTicks;
/* app callback    */
    pfEgcpSndCallBackFunc    pfSndCb;
/*     user data    */
    _VOID*                pData;
}EgcpTransSt;

typedef _INT32 (*pfEgcpTransFunc)(IN EgcpTransSt* pTrans,IN EgcpMsgSt* pMsg,IN EgcpMsgDirect enDirect);


_INT32 egcpTransPkgForm(IN EgcpTransSt* pTrans,IN EgcpMsgType enMsgT,
    IN _CHAR8* pcBuf,IN _INT32 iLen,IN EgcpMsgSt* pstMsg,OUT _CHAR8** ppcPkg);
_INT32 egcpTransPhySnd(IN EgcpTransSt* pTrans,IN _CHAR8* pcBuf,IN _INT32 iLen);
_INT32 egcpTransSetState(IN EgcpTransAppSt* pTapp,IN EgcpTransSt* pTrans,IN EgcpMsgSt* pMsg,IN EgcpTransState enState);

#if defined(__cplusplus)
}
#endif 
#endif

