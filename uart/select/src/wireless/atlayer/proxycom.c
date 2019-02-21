
#include "proxycom.h"
#include "svrcom.h"
#include "proxy.h"

#include "../wireless_common.h"
#include "../wireless_debug.h"
#include "../wireless_interface_inc.h"

extern WIRELESSROOTST     stWirelessRootSt;

_UINT32 ProxySend(_UINT32 ChannelID, const _CHAR8* buf, _INT32 size)
{
    PWIRELESSROOTST     pWirelessRootSt;
    PWIRELESSCHANNELST  pWirelessChannelCtl;
    
    pWirelessRootSt = (PWIRELESSROOTST)&stWirelessRootSt;
    pWirelessChannelCtl = &pWirelessRootSt->stChannelSt[ChannelID];

    /*First, check to see if the proxy got the result, but the application didn't get away the result*/
    if(pWirelessChannelCtl->bGetResult)
    {
        if(pWirelessChannelCtl->stReqReply.param.reply.pBuff != NULL)
        {        
            WIRELESS_FREE(pWirelessChannelCtl->stReqReply.param.reply.pBuff);
            
            pWirelessChannelCtl->stReqReply.param.reply.pBuff = NULL;
        }
        pWirelessChannelCtl->bGetResult = EOS_FALSE;
    }
    return SvrRecv(ChannelID, buf, size);
}

_UINT32 ProxyRecv(_UINT32 ChannelID, const _CHAR8* buf, _INT32 size)
{
    PWIRELESSROOTST     pWirelessRootSt;
    PWIRELESSCHANNELST  pWirelessChannelCtl;
    T_MESSAGE *pMsg;
    _VOID* pData = NULL;
    
    pWirelessRootSt = (PWIRELESSROOTST)&stWirelessRootSt;
    pWirelessChannelCtl = &pWirelessRootSt->stChannelSt[ChannelID];
    
    pMsg = (T_MESSAGE *)buf;
    switch(pMsg->type)
    {
        case MSG_URC:
        {
            /*process URC message*/
            if(pMsg->param.URC.totalSize != 0)
            {
                pData = (_VOID*)WIRELESS_MALLOC(pMsg->param.URC.totalSize); 
                if(pData != NULL)
                {
                    memcpy(pData, pMsg->param.URC.pBuff, pMsg->param.URC.totalSize);
                }
                else
                {
                    WIRELESS_ERROR(WIRELESS_ERRORNO, "Channel %d Malloc(0x%x) Failed\r\n", pMsg->param.URC.totalSize);
                    return 0;
                }
            }
            RIL_ProcessURC(ChannelID, (HRIL)pMsg->param.URC.hRIL,
                            pMsg->param.URC.eventID, 
                            pMsg->param.URC.paramter,
                            pData, 
                            pMsg->param.URC.usedSize);
            break;
        }
        case MSG_REPLY:
        {
            /*process synchronize result*/
            memcpy(&(pWirelessChannelCtl->stReqReply), pMsg, sizeof(T_MESSAGE));
            if(pMsg->param.reply.totalSize != 0)
            {
                pData = (_VOID*)WIRELESS_MALLOC(pMsg->param.reply.totalSize);        
                memcpy(pData, pMsg->param.reply.pBuff, pMsg->param.reply.totalSize);
                pWirelessChannelCtl->stReqReply.param.reply.pBuff = pData;
            }
            else
            {
                pWirelessChannelCtl->stReqReply.param.reply.pBuff = NULL;
            }
            
            pWirelessChannelCtl->bGetResult = EOS_TRUE;
            break;
        }
        case MSG_RESPONSE:
        {
            /*process at response message*/
            pData = NULL;
            if(pMsg->param.response.totalSize != 0)
            {
                pData = (_VOID*)WIRELESS_MALLOC(pMsg->param.response.totalSize);     
                memcpy(pData, pMsg->param.response.pBuff, pMsg->param.response.totalSize);
            }
            RIL_ProcessRsp(ChannelID,
                           (HRIL)pMsg->param.response.hRIL,
                            pMsg->param.response.resultType,
                            pMsg->param.response.customParam,
                            pData,
                            pMsg->param.response.usedSize);
            /* Free Memory    */
            if(pData != NULL)
            {
                WIRELESS_FREE(pData); 
            }
            break;
        }
    }
    return (_UINT32)size;
}


T_MESSAGE* RIL_GetResult(_UINT32 ChannelID)
{
    PWIRELESSROOTST     pWirelessRootSt;
    PWIRELESSCHANNELST  pWirelessChannelCtl;
    
    pWirelessRootSt = (PWIRELESSROOTST)&stWirelessRootSt;
    pWirelessChannelCtl = &pWirelessRootSt->stChannelSt[ChannelID];

#if 0  /* GOIP don't need this */
    while(!(pWirelessChannelCtl->bGetResult))
    {
        ;    /*waiting for receiving the send response*/
        */
    }
    pWirelessChannelCtl->bGetResult = EOS_FALSE;
#endif
    
    return &(pWirelessChannelCtl->stReqReply);
}

