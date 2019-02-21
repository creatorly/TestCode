#ifndef __EGCP_H
#define __EGCP_H

#if defined(__cplusplus)
extern "C" {
#endif

EOS_DECLARE_HANDLE(HEGCP);
EOS_DECLARE_HANDLE(HEGCPTRANS);

#define    EGCP_DEBUG_MODULE_NAME            "EGCP"
#define    EGCP_DEBUG_LEVEL                1
#define    EGCP_ERROR_LEVEL                0


typedef enum{
    EnInfo=0,
    EnProceeding,
    EnOk,
    EnError,
    EnAck,
    EnInfoRes,
    EnAckRes,
    EnMsgEnd
}EgcpMsgType;

typedef enum{
    EnEntSndInfo=0,
    EnEntSndInfoRes,
    EnEntSndProceeding,
    EnEntSndOk,
    EnEntSndError,
    EnEntSndAck,
    EnEntSndAckRes,
    EnEntEgcpEnd,

    EnEntRcvInfo=0x100,
    EnEntRcvInfoRes,
    EnEntRcvProceeding,
    EnEntRcvOk,
    EnEntRcvError,
    EnEntRcvAck,
    EnEntRcvAckRes,
        
    EnEntTimeOut=0x200
    
}EgcpEvent;

typedef enum{
    EnEgcpValidData= 0,
    EnEgcpSndOk,
    EnEgcpSndError,
    EnEgcpNoEnd
}EgcpNotifyEnt;

typedef enum{
    EnEgcpSock,
    EnEgcpMem,
    EnEgcpTimer,
    EnEgcpReAddr
}EgcpParam;

typedef _INT32 (*pfEgcpSndCallBackFunc)(HEGCP hEgcp,HEGCPTRANS hEgcpTrans,EgcpNotifyEnt enEnt,_CHAR8* pcBuf,_INT32 iLen,_VOID* pData);
typedef _INT32 (*pfEgcpRcvReqFunc)(HEGCP hEgcp,_CHAR8* pcBuf,_INT32 iRcvByte,HEGCPTRANS hTrans,_VOID* pUsrData);

_INT32 egcpInit(pfEgcpRcvReqFunc pRcvFunc,_USHORT16 usPort,_INT32 iMax,LPHEGCP lpstEgcp,_VOID* pData);
_INT32 egcpSndRequest(HEGCP hEgcp,_CHAR8* pcBuf,_INT32 iLen,eosAddress* pReAddr,pfEgcpSndCallBackFunc pfCallBack,_VOID* pDtata);
_VOID egcpRecvCBFunc(_INT32 iSock,eosMsgEvents sEvent,_VOID* iUserData);
_INT32 egcpSndResp(HEGCP hEgcp,HEGCPTRANS hTrans,EgcpNotifyEnt enEnt,_CHAR8* pcBuf,_INT32 iLen);
_INT32    egcpGetParam(HEGCP hEgcp,HEGCPTRANS hTrans,EgcpParam enEnt,_INT32* ppiVal);
_INT32 egcpShow(HEGCP hEgcp,EOS_INFO_PRINTF pPrintFunc,_VOID *pData);

#if defined(__cplusplus)
}
#endif 
#endif

