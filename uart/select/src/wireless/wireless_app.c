
#ifdef __VXWORKS_OS__
#include <vxWorks.h>
#include <semLib.h>
#include <tickLib.h>
#endif

#include "eos.h"
#include "utils.h"
#include "ut_charset.h"
#include <utl_kval.h>
#include <utl_cfg.h>
#include <utl_string.h>
#include <utils/ut_fselect.h>

#include "wireless_common.h"
#include "wireless_string.h"
#include "wireless_interface_inc.h"
#include "wireless_debug.h"
#include "wireless_app.h"
#include "wireless_interface.h"
#include "wm_channel.h"
#include "wireless_vifi_uart.h"
 
#if (ACOM_BOARD_TYPE == MDM9X07)
#include "ril.h"
#include "ril_qmi.h"
#include "wifi_lib.h"
#include "util.h" 
#endif

extern WIRELESSROOTST     stWirelessRootSt;

#define WIRELESS_RINGOFF_CHECK_MS       2000

static _INT32 wirelessResetCmdList(_UINT32 uiChannelid);
static _INT32 WIRELESS_ComBufferReset(_UINT32 ChannelId);

#ifdef ENABLE_UT_RTS

_VOID wirelessEosTimerBegin(EOS_TIMER_ID tmrGrpId, _UINT32 tmrId, const _CHAR8* file, _INT32 line)
{
    UT_RtsBegin(NULL, file, line);
}

_VOID wirelessEosTimerEnd(EOS_TIMER_ID tmrGrpId, _UINT32 tmrId, const _CHAR8* file, _INT32 line)
{
    UT_RtsEnd(NULL, file, line);
}

#endif


static _UINT32 CallURCCallBack(_UINT32 ChannelID, _UINT32 eventID, _UINT32 param, 
                        _VOID* lpData, _UINT32 size, _UINT32 customParam)
{
    PWIRELESSROOTST     pWirelessRootSt;
    PWIRELESSCHANNELST  pWirelessChannelCtl;
    
    pWirelessRootSt = (PWIRELESSROOTST)&stWirelessRootSt;
    pWirelessChannelCtl = &pWirelessRootSt->stChannelSt[ChannelID];
    
    WIRELESS_INFO("Channel %d CallURCCallBack eventID:%d, param:%d\r\n", ChannelID, eventID, param);
    switch(eventID)
    {
        case RIL_NOTIFY_CALLSTATECHG:
        {
            WIRELESS_INFO("Channel %d CallURCCallBack Get Event RIL_NOTIFY_CALLSTATECHG, Param:%d\r\n", ChannelID, param);
            /*printf("Channel %d CallURCCallBack Get Event RIL_NOTIFY_CALLSTATECHG, Param:%d\r\n", ChannelID, param);*/
            if(CALLSTATECHG_RING == param)
            {
            }
            else if(CALLSTATECHG_MANUAL == param)
            {        
                WIRELESS_INFO("Channel %d CallURCCallBack Call Active\r\n", ChannelID);
                /*POTS_Ntfy(ChannelID, EN_POTS_EVENT_OFFHOOK, NULL);*/    
            }
            else if(CALLSTATECHG_NOCARRIER == param)
            {        
                _UINT32 uiActiveCallQty = 0;
                
                WIRELESS_INFO("Channel %d CallURCCallBack Call NO Carrier\r\n", ChannelID);

            }    
            else if(CALLSTATECHG_NOANSWER == param)
            {        
                _UINT32 uiActiveCallQty = 0;
                
                WIRELESS_INFO("Channel %d CallURCCallBack Call NO Answer\r\n", ChannelID);
            }
            else if(CALLSTATECHG_NODIALTONE == param)
            {        
                _UINT32 uiActiveCallQty = 0;
                
                WIRELESS_INFO("Channel %d CallURCCallBack Call No DialTone\r\n", ChannelID);
            }
            else if(CALLSTATECHG_BUSY == param)
            {        
                _UINT32 uiActiveCallQty = 0;
                
                WIRELESS_INFO("Channel %d CallURCCallBack Call Busy\r\n", ChannelID);
            }
            break;
        }
        case RIL_NOTIFY_RING:
        {
            WIRELESS_INFO("CallURCCallBack Get Event RIL_NOTIFY_RING\r\n");
            break;
        }
        case RIL_NOTIFY_CALLWAITING:
        {
            WIRELESS_INFO("CallURCCallBack Get Event RIL_NOTIFY_CALLWAITING\r\n");
            break;
        } 
        case RIL_NOTIFY_SUPSERVICEDATA:
        {  
            _CHAR8* pStrBuff = ((RIL_USSD_URC *)lpData)->str;

            WIRELESS_FREE(pStrBuff);
            WIRELESS_FREE(lpData);
            lpData = NULL;

            break;
        } 
        case RIL_NOTIFY_CALLDIALING:
        {
            WIRELESS_INFO("CallURCCallBack Get Event RIL_NOTIFY_CALLDIALING\r\n");
            break;
        } 
        case RIL_NOTIFY_SIGNALUPDATE:
        {
            _INT32 uiSignal;
            uiSignal = *(_INT32* )lpData;
            WIRELESS_INFO("Channel %d CallURCCallBack Get Event RIL_NOTIFY_SIGNALUPDATE, Signal = %d\r\n",
                       ChannelID, uiSignal);
            WIRELESS_FREE(lpData);
            lpData = NULL;
            break;
        }
        case RIL_NOTIFY_CALLINLINE:
        {
            RIL_URC_CLIP *pstClip;
            pstClip = (RIL_URC_CLIP*)lpData;
            if(EOS_FALSE == pWirelessChannelCtl->bIncomingRingRpt)
            {
                WIRELESS_INFO("RIL_NOTIFY_CALLINLINE, number = %s, type = %d, cli = %d.\r\n", pstClip->telenumber, pstClip->type, pstClip->cli);

                pWirelessChannelCtl->bIncomingRingRpt = EOS_TRUE;
            }
            
            WIRELESS_FREE(lpData);
            lpData = NULL;
            break;
        }
        default:
        {
            WIRELESS_INFO("CallURCCallBack Get Event %d\r\n", eventID);
            break;
        }
    }
    return 0;
}

static _INT32 wirelessResetCmdList(_UINT32 uiChannelid)
{
    _UINT32 iLstCount = 0;    
    T_HCHNL* pChnl = NULL;
    PWIRELESSROOTST pWirelessRootSt;
    PWIRELESSCHANNELST  pWirelessChannelCtl;
    T_ATCMD*    pATCmd = NULL;
    T_ATCMD*    pChnlATCmd = NULL;

    pWirelessRootSt = (PWIRELESSROOTST)&stWirelessRootSt;
    pWirelessChannelCtl = &pWirelessRootSt->stChannelSt[uiChannelid];
    pChnl = pWirelessChannelCtl->pChnl;
    
    if(pWirelessChannelCtl->stHATServer != NULL)
    {
        if(pWirelessChannelCtl->stHATServer->hCallManager != NULL)
        {
            CallManager_DeInit_Ex(pWirelessChannelCtl->stHATServer->hCallManager);
        }
    }
    
    WirelessSemP(pWirelessRootSt->ATCmdLstMutex, EOS_WAIT_FOREVER);
    iLstCount = eos_lst_count(&pChnl->atcmdlist);
    while(iLstCount > 0)
    {                
        pATCmd = (PT_ATCMD)eos_lst_get(&pChnl->atcmdlist);

        if(NULL != pATCmd->lpData)
        {                    
            WIRELESS_FREE(pATCmd->lpData);
        }
        if(NULL != pATCmd->pURC)
        {
            WIRELESS_FREE(pATCmd->pURC);
        }
        if(NULL != pATCmd->pResult)
        {
            WIRELESS_FREE(pATCmd->pResult);
        }

        if(eDEVICECALL == pATCmd->serviceType)
        {
            T_CALLPARAM *pParam;
            
            pParam = (T_CALLPARAM*)pATCmd->customHandle;

            #if 0
            if((pParam != NULL) && (pParam->hCall != NULL))
            {
                WIRELESS_FREE(pParam->hCall);            
                pParam->hCall = NULL;
            }
            #endif
                        
            if(((_INT32*)pParam != NULL) && ((_INT32*)pParam->customParam != NULL))
            {    
                WIRELESS_FREE((_VOID *)(pParam->customParam));
                pParam->customParam = (_UINT32)NULL;
            }
        }
        else if(eDEVICESMS == pATCmd->serviceType)
        {
            T_MSGPARAM *pParam;
            
            pParam = (T_MSGPARAM*)pATCmd->customHandle;

            if(((_INT32*)pParam != NULL) && ((_INT32*)(pParam->customParam) != NULL))
            {    
                WIRELESS_FREE((_VOID *)(pParam->customParam));
                pParam->customParam = (_UINT32)NULL;
            }
        }
        else if(eDEVICESIM == pATCmd->serviceType)
        {
            T_SIMPARAM *pParam;
            
            pParam = (T_SIMPARAM*)pATCmd->customHandle;
        
            if(((_INT32*)pParam != NULL) && (pParam->customParam))
            {    
                WIRELESS_FREE((_VOID *)(pParam->customParam));
                pParam->customParam = (_UINT32)NULL;
            }
        }
    
        if(NULL != (_INT32 *)(pATCmd->customHandle))
        {
            WIRELESS_FREE((_VOID *)(pATCmd->customHandle));
            pATCmd->customHandle = (_UINT32)NULL;
        }

        WIRELESS_FREE((_INT32 *)pATCmd);
        pATCmd = NULL;
        
        iLstCount = eos_lst_count(&pChnl->atcmdlist);
    }
    WirelessSemV(pWirelessRootSt->ATCmdLstMutex);

    pChnlATCmd = &(pWirelessChannelCtl->stATCmd);
    if(NULL != pChnlATCmd->lpData)
    {                    
        WIRELESS_FREE(pChnlATCmd->lpData);
        pChnlATCmd->lpData = NULL;
    }
    if(NULL != pChnlATCmd->pURC)
    {
        WIRELESS_FREE(pChnlATCmd->pURC);
        pChnlATCmd->pURC = NULL;
        pChnl->pURC = NULL;    
    }
    if(NULL != pChnlATCmd->pResult)
    {
        WIRELESS_FREE(pChnlATCmd->pResult);
        pChnlATCmd->pResult = NULL;
        pChnl->pResult = NULL;    
    }

    if(eDEVICECALL == pChnlATCmd->serviceType)
    {
        T_CALLPARAM *pParam;
        
        pParam = (T_CALLPARAM*)pChnlATCmd->customHandle;

        #if 0
        if((pParam->hCall != NULL) && (pWirelessChannelCtl->hCallHandle != NULL))
        {
            WIRELESS_FREE(pParam->hCall);
            pWirelessChannelCtl->hCallHandle = NULL;
        }
        #endif
        
        if((pParam != NULL) && (pParam->customParam))
        {    
            WIRELESS_FREE((_VOID *)(pParam->customParam));
            pParam->customParam = (_UINT32)NULL;
        }
    }
    else if(eDEVICESMS == pChnlATCmd->serviceType)
    {
        T_MSGPARAM *pParam;
        
        pParam = (T_MSGPARAM*)pChnlATCmd->customHandle;

        if((pParam != NULL) && (pParam->customParam))
        {    
            WIRELESS_FREE((_INT32 *)(pParam->customParam));
            pParam->customParam = (_UINT32)NULL;
        }
    }
    else if(eDEVICESIM == pChnlATCmd->serviceType)
    {
        T_SIMPARAM *pParam;
        
        pParam = (T_SIMPARAM*)pChnlATCmd->customHandle;

        if((pParam != NULL) && (pParam->customParam))
        {    
            WIRELESS_FREE((_INT32 *)(pParam->customParam));
            pParam->customParam = (_UINT32)NULL;
        }
    }
    
    if((_UINT32)NULL != pChnlATCmd->customHandle)
    {
        WIRELESS_FREE((_INT32 *)(pChnlATCmd->customHandle));
        pChnlATCmd->customHandle = (_UINT32)NULL;
    }

    pChnl->bWaitingRsp = EOS_FALSE;

    /* Clear Response List    */
    WirelessSemP(pWirelessRootSt->RSPLstMutex, EOS_WAIT_FOREVER);
    iLstCount = eos_lst_count(&pChnl->atrsplist);
    while(iLstCount > 0)
    {
        T_ATRESPONSE* pATRsp = (PT_ATRESPONSE)eos_lst_get(&pChnl->atrsplist);
                
        printf("Channel %d Free Response List\r\n", uiChannelid);                 
        if(EOS_TRUE == pATRsp->bNeedFree)
        {
             WIRELESS_FREE(pATRsp->lpData);
             pATRsp->bNeedFree = EOS_FALSE;
        }        
        
        WIRELESS_FREE(pATRsp);
        
        iLstCount = eos_lst_count(&pChnl->atrsplist);
    }
    WirelessSemV(pWirelessRootSt->RSPLstMutex);
    
    return EOS_OK;
}

static _INT32 WIRELESS_ComBufferReset(_UINT32 ChannelId)
{
     PWIRELESSROOTST pWirelessRootSt;
    PWIRELESSCHANNELST  pWirelessChannelCtl;
    
    pWirelessRootSt = (PWIRELESSROOTST)&stWirelessRootSt;
    pWirelessChannelCtl =  &pWirelessRootSt->stChannelSt[ChannelId];

    memset(pWirelessChannelCtl->pucExtComBuf, 0, WIRELESS_EXTCOMBUF_MAXSIZE);
    memset(pWirelessChannelCtl->pucCompleteInfoBuf, 0, WIRELESS_COMPLETEINFO_MAXSIZE);
    pWirelessChannelCtl->uiExtComHead = 0;
    pWirelessChannelCtl->uiExtComTail = 0;

    return EOS_OK;
}

_INT32 WIRELESS_ChannelInit(_UINT32 ChannelID)
{
    PWIRELESSROOTST pWirelessRootSt;
    PWIRELESSCHANNELST  pWirelessChannelCtl;

    pWirelessRootSt = (PWIRELESSROOTST)&stWirelessRootSt;    
    pWirelessChannelCtl = &pWirelessRootSt->stChannelSt[ChannelID];

    WIRELESS_ComBufferReset(ChannelID);

    pWirelessChannelCtl->Signal = -1;
    
    pWirelessChannelCtl->bWaitATDResponse = EOS_FALSE;
    pWirelessChannelCtl->bATDCLCCWaitOKResponse = EOS_FALSE;
    pWirelessChannelCtl->bIsRinging = EOS_FALSE;

    pWirelessChannelCtl->bEnterPinLock = 0;
    pWirelessChannelCtl->uiAllCallQty = 0;
    pWirelessChannelCtl->uiActiveQty = 0;
    pWirelessChannelCtl->uiHoldQty = 0;
    pWirelessChannelCtl->uiDialingQty = 0;
    pWirelessChannelCtl->uiWaitingQty = 0;
    pWirelessChannelCtl->uiIncomingQty = 0;

    pWirelessChannelCtl->ModInitStatus = RIL_INITSTART;
    pWirelessChannelCtl->bSIMExisted = EOS_FALSE;    
    pWirelessChannelCtl->bReadyReport = EOS_FALSE;    
    
    pWirelessChannelCtl->SlotSIMIsNew = EN_WIRELESS_SIM_UNKNOWN;
    
    pWirelessChannelCtl->uiBarringDetectCnt = 0;

    pWirelessChannelCtl->bReportReady = EOS_FALSE;
    pWirelessChannelCtl->bIncomingRingRpt = EOS_FALSE;
    
    pWirelessChannelCtl->uiRespTimeoutCnt = 0;     

    pWirelessChannelCtl->CallStatus = 0;
    
    pWirelessChannelCtl->uiCCIDPBReadCnt = 0;
    
    pWirelessChannelCtl->bTalkStart = EOS_FALSE;
    pWirelessChannelCtl->bIgnoreATDResponse = EOS_FALSE;
    pWirelessChannelCtl->bATDCLCCWaitOKResponse = EOS_FALSE;

    pWirelessChannelCtl->uiModRegCnt  = 0;
    pWirelessChannelCtl->uiModResetCnt = 0;

    pWirelessChannelCtl->uiModuleDetRestartCnt = 0;
    pWirelessChannelCtl->uiModuleDetectCnt = 0;
    pWirelessChannelCtl->uiModulePwrOffCnt = 0;
    pWirelessChannelCtl->uiSimDetectCnt = 0;
    pWirelessChannelCtl->uiSimDetectRetryCnt = 0;
    
    pWirelessChannelCtl->uiOprInquireCnt = 0;
    pWirelessChannelCtl->uiInqSMSCCnt = 0;
    
    pWirelessChannelCtl->uiSetCOPSFormatCnt = 0;
    pWirelessChannelCtl->uiSetCOPSFormatTick = 0;

    
    pWirelessChannelCtl->bRegUpdateWaiting = EOS_FALSE;
    pWirelessChannelCtl->uiIMEIReadCnt = 0;
    pWirelessChannelCtl->uiSetNewSMSStoreModeCnt = 0;

    pWirelessChannelCtl->uiReadUSSDCnt = 0;
    pWirelessChannelCtl->iUssdBalanceLeft = 0;
    pWirelessChannelCtl->uiUSSDRetryTime = 0;
    pWirelessChannelCtl->uiUSSDZeroBalanceRetryTime = WIRELESS_SEND_USSD_INQUIRE_MAX;
    
    pWirelessChannelCtl->NetRegStatus = RIL_NOTREGSEARCH;
    pWirelessChannelCtl->bPPPOk = EOS_FALSE;

    pWirelessChannelCtl->uiLostStartTick = 0;

    pWirelessChannelCtl->OperatorID = 0; 
    pWirelessChannelCtl->OprtMCC = 0; 
    pWirelessChannelCtl->OprtMNC = 0; 
    
    //ASYS_DrvSetParam(EN_ASYS_DRVIDX_NVM, NVM_GOIP_PORT_MNO, ChannelID, pWirelessChannelCtl->OperatorID, NULL);//weikk add 2016.1.21
    pWirelessChannelCtl->uiSimLostRestartCnt = WIRELESS_SIMLOST_RESTART_MAX;
    pWirelessChannelCtl->uiOprLostRestartCnt = WIRELESS_OPSLOST_RESTART_MAX;
    
    pWirelessChannelCtl->enFuctionStatus = EN_WIRELESS_ST_FUNCTION_ON;

    if(NULL != pWirelessChannelCtl->UrcParse_temp)
    {
        WIRELESS_FREE(pWirelessChannelCtl->UrcParse_temp);
        pWirelessChannelCtl->UrcParse_temp = NULL;
    }

    pWirelessChannelCtl->recSmsPdu = EOS_FALSE;
    pWirelessChannelCtl->recCdsPdu = EOS_FALSE;
    pWirelessChannelCtl->recBcmPdu = EOS_FALSE;

    wirelessResetCmdList(ChannelID);

    /* 2018.08.06 added by pengjian to reduce the cpu usage */
    WIRELESS_WakeUp();

    return EOS_OK;
}

_INT32 WIRELESS_AppInit(_LONG32 param)
{ 
    PWIRELESSROOTST     pWirelessRootSt;
    PWIRELESSCHANNELST  pWirelessChannelCtl;
    _UINT32 chID[ACOM_PORT_NUM][2] = {};
    _UINT32 uiChannelid = 0;
    _UINT32 i = 0;
    _UINT32 uiPortIndex = 0;
    _UINT32 uiOnOffDuring = 0;
        
    pWirelessRootSt = (PWIRELESSROOTST)&stWirelessRootSt;
         
    for(i = 0; i < pWirelessRootSt->uiMaxChannelNum; i++)
    {
        chID[i][0] = i;
        chID[i][1] = 0xff;
    }
    
    /* Initialize the wireless module manager    */
    for(uiChannelid = 0; uiChannelid < pWirelessRootSt->uiMaxModuleNum; uiChannelid++)
    {
        pWirelessChannelCtl = &pWirelessRootSt->stChannelSt[uiChannelid];

        WIRELESS_initUart(uiChannelid);
        if(-1 == pWirelessChannelCtl->fUartHandle)
        {
            //printf("WIRELESS_AppInit: Channel %d Open UART Handle Error.\n", uiChannelid);
            //return -1;
        }
    }
    
    for(uiChannelid = 0; uiChannelid < pWirelessRootSt->uiMaxChannelNum; uiChannelid++)
    {
        pWirelessChannelCtl = &pWirelessRootSt->stChannelSt[uiChannelid];
        
        pWirelessChannelCtl->stHATServer = NULL;
        WIRELESS_ChannelInit(uiChannelid);
    }
    
    for(uiChannelid = 0; uiChannelid < pWirelessRootSt->uiMaxModuleNum; uiChannelid++)
    {
        pWirelessChannelCtl = &pWirelessRootSt->stChannelSt[uiChannelid];
        RIL_Setup(1, chID[uiChannelid], WAVECOM, &(pWirelessChannelCtl->gb_hATServer));  
        RIL_Open(chID[uiChannelid], WM_TYPE_CALL, pWirelessChannelCtl->gb_hATServer,
                            CallURCCallBack, 1, &(pWirelessChannelCtl->gb_hCallRIL));
    }
     
    ut_timer_create("wirelesstimer", &(pWirelessRootSt->WirelessTimerID));

#if defined(ENABLE_UT_RTS) && defined(USE_EOS_TIMER)
    eos_timer_set_hook_begin(pWirelessRootSt->WirelessTimerID, wirelessEosTimerBegin);
    eos_timer_set_hook_end(pWirelessRootSt->WirelessTimerID, wirelessEosTimerEnd);
#endif
 
    return EOS_OK;
}

static _VOID WIRELESS_AppComDataRcvTask_Ltq(_VOID)
{
    PWIRELESSROOTST pWirelessRootSt = (PWIRELESSROOTST)&stWirelessRootSt;
    ioctl_channel_read_st stComRead;

    while (UT_MsgqGrab(pWirelessRootSt->ATRspMsgq, &stComRead, sizeof(stComRead)) > 0)
    {
        PWIRELESSCHANNELST  pWirelessChannelCtl;
        
        pWirelessChannelCtl = &pWirelessRootSt->stChannelSt[stComRead.channelid];
        if(pWirelessChannelCtl->bModuleUpdating)
        {
            continue;
        }

        if((pWirelessChannelCtl->uiExtComTail + stComRead.size) < WIRELESS_EXTCOMBUF_MAXSIZE)
        {
            memcpy((pWirelessChannelCtl->pucExtComBuf + pWirelessChannelCtl->uiExtComTail), stComRead.buff, stComRead.size);
            pWirelessChannelCtl->uiExtComTail += stComRead.size;
        }
        else
        {
            _UINT32 uiFirstReadLen = WIRELESS_EXTCOMBUF_MAXSIZE - pWirelessChannelCtl->uiExtComTail;
            
            memcpy((pWirelessChannelCtl->pucExtComBuf + pWirelessChannelCtl->uiExtComTail), stComRead.buff, uiFirstReadLen);
            memcpy(pWirelessChannelCtl->pucExtComBuf, (stComRead.buff + uiFirstReadLen), (stComRead.size - uiFirstReadLen));
            pWirelessChannelCtl->uiExtComTail = stComRead.size - uiFirstReadLen;
        }
        
        pWirelessChannelCtl->uiExtComTail &= (WIRELESS_EXTCOMBUF_MAXSIZE - 1);
    }
}


static _UINT32 wrlATInquireRegStatusCallback(_UINT32 ChannelID,HRIL hRIL,_UINT32 resultType,_VOID * pData,_UINT32 size)
{    
    PWIRELESSROOTST pWrlRootSt;
    PWIRELESSCHANNELST  pWrlChannelCtl;
    
    pWrlRootSt = (PWIRELESSROOTST)&stWirelessRootSt;    
    pWrlChannelCtl = &pWrlRootSt->stChannelSt[ChannelID];

    WIRELESS_INFO("Channel %d wrlATInquireRegStatusCallback:%s\r\n", ChannelID,  pData);

    return EOS_OK;
}

static _UINT32 wrlATInquireSignalCallback(_UINT32 ChannelID,HRIL hRIL,_UINT32 resultType,_VOID * pData,_UINT32 size)
{    
    PWIRELESSROOTST pWrlRootSt;
    PWIRELESSCHANNELST  pWrlChannelCtl;
    
    pWrlRootSt = (PWIRELESSROOTST)&stWirelessRootSt;    
    pWrlChannelCtl = &pWrlRootSt->stChannelSt[ChannelID];

    WIRELESS_INFO("Channel %d wrlATInquireSignalCallback:%s\r\n", ChannelID,  pData);

    return EOS_OK;
}

static _UINT32 wrlFSMQueryBattTimerCB(_UINT32 ChannelID, _UINT32 uiReserved)
{
    PWIRELESSROOTST pWrlRootSt;
    PWIRELESSCHANNELST  pWrlChannelCtl;
    
    pWrlRootSt = (PWIRELESSROOTST)&stWirelessRootSt;
    pWrlChannelCtl = &pWrlRootSt->stChannelSt[ChannelID];

    WIRELESS_INFO("wrlFSMQueryBattTimerCB:%d", ChannelID);
    RIL_InquireGNetRegStatus(ChannelID, pWrlChannelCtl->gb_hCallRIL, wrlATInquireRegStatusCallback, NULL);

    if(pWrlChannelCtl->BattQueryTimerID > 0)
    {
        ut_timer_stop(pWrlRootSt->WirelessTimerID, pWrlChannelCtl->BattQueryTimerID);
        pWrlChannelCtl->BattQueryTimerID = 0;
    }
    ut_timer_start(pWrlRootSt->WirelessTimerID, (5*1000), EN_TIMER_ONESHOT,
        (TIMERFUNC)wrlFSMQueryBattTimerCB, WRL_LOCSIM_CHN, 0, &pWrlChannelCtl->BattQueryTimerID);

}

static _UINT32 wrlFSMTestBattTimerCB(_UINT32 ChannelID, _UINT32 uiReserved)
{
    PWIRELESSROOTST pWrlRootSt;
    PWIRELESSCHANNELST  pWrlChannelCtl;
    
    pWrlRootSt = (PWIRELESSROOTST)&stWirelessRootSt;
    pWrlChannelCtl = &pWrlRootSt->stChannelSt[ChannelID];

    WIRELESS_INFO("wrlFSMTestBattTimerCB:%d", ChannelID);
    RIL_InquireSignalQuality(ChannelID, pWrlChannelCtl->gb_hCallRIL, wrlATInquireSignalCallback);
}


_VOID WIRELESS_AppMainTask(_VOID)
{
    _UINT32 uiChannelid = 0;
    _UINT32 uiMsgQLen = 0;   
    static _UINT32 vsimStart = 0;

    ST_WRL_FSM_MSGQ     stFSMMsgQ;
    PWIRELESSROOTST     pWrlRootSt;
    PWIRELESSCHANNELST  pWrlChannelCtl;

    pWrlRootSt = (PWIRELESSROOTST)&stWirelessRootSt;
    pWrlChannelCtl = &pWrlRootSt->stChannelSt[uiChannelid];

    if(pWrlChannelCtl->BattQueryTimerID > 0)
    {
        ut_timer_stop(pWrlRootSt->WirelessTimerID, pWrlChannelCtl->BattQueryTimerID);
        pWrlChannelCtl->BattQueryTimerID = 0;
    }
    ut_timer_start(pWrlRootSt->WirelessTimerID, (5*1000), EN_TIMER_ONESHOT,
            (TIMERFUNC)wrlFSMQueryBattTimerCB, WRL_LOCSIM_CHN, 0, &pWrlChannelCtl->BattQueryTimerID);

    if(pWrlChannelCtl->BattTestTimerID > 0)
    {
        ut_timer_stop(pWrlRootSt->WirelessTimerID, pWrlChannelCtl->BattTestTimerID);
        pWrlChannelCtl->BattTestTimerID = 0;
    }
    ut_timer_start(pWrlRootSt->WirelessTimerID, (1*1000), EN_TIMER_PERIODIC,
            (TIMERFUNC)wrlFSMTestBattTimerCB, WRL_LOCSIM_CHN, 0, &pWrlChannelCtl->BattTestTimerID);
    
             
    while(1)
    {
        /* !!! this task MUST block in utl_select_run() till a message received or a timer fired */
        int max_tmo = ut_timer_get_tmo(pWrlRootSt->WirelessTimerID);
        
        UT_RTS_BEGIN();
        utl_select_run(pWrlRootSt->sel, max_tmo);
        UT_RTS_END();

        UT_RTS_BEGIN();
        ut_timer_trig(pWrlRootSt->WirelessTimerID, 0);
        UT_RTS_END();
        
        UT_RTS_BEGIN();
        WIRELESS_AppComDataRcvTask_Ltq();
        UT_RTS_END();

        UT_RTS_BEGIN();
        ReceiveThread();   
        UT_RTS_END();

        UT_RTS_BEGIN();
        SendThread();
        UT_RTS_END();
    }
}

_INT32 WRL_FSM_SendEvent(_UINT32 ChannelID, _UINT32 EventType, EN_WRL_FSM_EVENT_ID_INDEX EventID, _UCHAR8 Param)
{
    PWIRELESSROOTST pWrlRootSt;
    ST_WRL_FSM_MSGQ stFSMMsgQ;
    
    pWrlRootSt = (PWIRELESSROOTST)&stWirelessRootSt;    

    stFSMMsgQ.ChannelID = ChannelID;    
    stFSMMsgQ.EventType = EventType;
    stFSMMsgQ.EventID = EventID;
    stFSMMsgQ.Data[0] = Param;
    WIRELESS_INFO("Channel %d FSM Send EventType:%d, EventID:%d, Param:%d\r\n", ChannelID, EventType, EventID, Param);
    UT_MsgqSend(pWrlRootSt->FSMMsgq, &stFSMMsgQ, sizeof(ST_WRL_FSM_MSGQ));

    /* 2018.08.01 added by pengjian to reduce the cpu usage */
    WIRELESS_WakeUp();

    return EOS_OK;
}

_VOID WIRELESS_WakeUp()
{
    utl_select_stop(stWirelessRootSt.sel);
}

_VOID WIRELESS_SetModInitStatus(_UINT32 ChannelID, RIL_INIT_STATUS initStatus)
{
    stWirelessRootSt.stChannelSt[ChannelID].ModInitStatus = initStatus;

    WIRELESS_WakeUp();
}

_VOID WIRELESS_SetRegStatus(_UINT32 ChannelID, RIL_NET_STAT regStatus)
{
    stWirelessRootSt.stChannelSt[ChannelID].NetRegStatus = regStatus;

    WIRELESS_WakeUp();
}

_VOID WIRELESS_SetSlotSIMIsNew(_UINT32 ChannelID, _UINT32 bSimIsNew, _BOOL bWakeup)
{
    stWirelessRootSt.stChannelSt[ChannelID].SlotSIMIsNew = bSimIsNew;

    if (bWakeup)
    {
        WIRELESS_WakeUp();
    }
}

_VOID WIRELESS_SetFakeResponse(_UINT32 ChannelID, _BOOL bFakeResponse)
{
    stWirelessRootSt.stChannelSt[ChannelID].bATDSendFakeResponse = bFakeResponse;

    if (bFakeResponse)
    {
        WIRELESS_WakeUp();
    }
}

static _UCHAR8 ConverHexCharToChar(_CHAR8* str_Hex)
{
    _UCHAR8 i,c;

    c = str_Hex[0];
    if('a' <= c && c <= 'f')
    {
        c -= 'a';
        c += 10;
    }
    else if('A' <= c && c <= 'F')
    {
        c -= 'A';
        c += 10;
    }
    else 
        c -= '0';

    i = c;
    i <<= 4;

    c = str_Hex[1];
    if('a' <= c && c <= 'f')
    {
        c -= 'a';
        c += 10;
    }
    else if('A' <= c && c <= 'F')
    {
        c -= 'A';
        c += 10;
    }
    else 
        c -= '0';

    i += c;
    return i;
}

int gsmString2Bytes(const char* pSrc, unsigned char* pDst, int nSrcLength)
{
    _INT32 i = 0;
    for ( i = 0; i < nSrcLength; i += 2)
    {
        if ((*pSrc >= '0') && (*pSrc <= '9'))
        {
            *pDst = (*pSrc - '0') << 4;
        }
        else
        {
            *pDst = (*pSrc - 'A' + 10) << 4;
        }

        pSrc++;

        if ((*pSrc>='0') && (*pSrc<='9'))
        {
            *pDst |= *pSrc - '0';
        }
        else
        {
            *pDst |= *pSrc - 'A' + 10;
        }

        pSrc++;
        pDst++;
    }

    return (nSrcLength / 2);
}

_SHORT16 Conv8BitDataToGSM(_CHAR8* str8BitData, _CHAR8* strGSM)
{
    _SHORT16    i,j, length;

    i = 0;
    j = 0;
    length = strlen(str8BitData);
    while(i < length)
    {
        strGSM[j++] = ConverHexCharToChar( str8BitData + i );
        i += 2;
    }
    strGSM[j] = 0;
    return j;
}
 
/*    for wireless statistics    */
_VOID WIRELESS_AppStat(EOS_INFO_PRINTF fpPrint, _VOID* pData)
{
    _CHAR8    acBuf[1024];
    PWIRELESSROOTST pWirelessRootSt;
    PWIRELESSCHANNELST  pWirelessChannelCtl;
    _UINT32 i = 0;

    pWirelessRootSt = (PWIRELESSROOTST)&stWirelessRootSt;    

    fpPrint(pData, "Wireless Statistics:\r\n");
    
    if(EOS_TRUE == pWirelessRootSt->bWirelessTaskRun)
    {
        sprintf(acBuf, "WirelessTaskState: Run\r\n");
    }
    else
    {
        sprintf(acBuf, "WirelessTaskState: Idle\r\n");
    }
    fpPrint(pData, acBuf);

    if(EOS_TRUE == pWirelessRootSt->bWirelessTaskRun1)
    {
        sprintf(acBuf, "WirelessTaskState1: Run\r\n");
    }
    else
    {
        sprintf(acBuf, "WirelessTaskState1: Idle\r\n");
    }
    fpPrint(pData, acBuf);

    if(EOS_TRUE == pWirelessRootSt->bWirelessTaskRun2)
    {
        sprintf(acBuf, "WirelessTaskState2: Run\r\n");
    }
    else
    {
        sprintf(acBuf, "WirelessTaskState2: Idle\r\n");
    }
    fpPrint(pData, acBuf);

    if(EOS_TRUE == pWirelessRootSt->bWirelessTaskRun3)
    {
        sprintf(acBuf, "WirelessTaskState3: Run\r\n");
    }
    else
    {
        sprintf(acBuf, "WirelessTaskState3: Idle\r\n");
    }
    fpPrint(pData, acBuf);

    if(EOS_TRUE == pWirelessRootSt->bWirelessTaskRun4)
    {
        sprintf(acBuf, "WirelessTaskState4: Run\r\n");
    }
    else
    {
        sprintf(acBuf, "WirelessTaskState4: Idle\r\n");
    }
    fpPrint(pData, acBuf);

    if(EOS_TRUE == pWirelessRootSt->bWirelessTaskRun5)
    {
        sprintf(acBuf, "WirelessTaskState5: Run\r\n");
    }
    else
    {
        sprintf(acBuf, "WirelessTaskState5: Idle\r\n");
    }
    fpPrint(pData, acBuf);

    if(EOS_TRUE == pWirelessRootSt->bWirelessTaskRun6)
    {
        sprintf(acBuf, "WirelessTaskState6: Run\r\n");
    }
    else
    {
        sprintf(acBuf, "WirelessTaskState6: Idle\r\n");
    }
    fpPrint(pData, acBuf);

    if(EOS_TRUE == pWirelessRootSt->bWirelessTaskRun7)
    {
        sprintf(acBuf, "WirelessTaskState7: Run\r\n");
    }
    else
    {
        sprintf(acBuf, "WirelessTaskState7: Idle\r\n");
    }
    fpPrint(pData, acBuf);
    
    if(EOS_TRUE == pWirelessRootSt->bWirelessTaskRun8)
    {
        sprintf(acBuf, "WirelessTaskState8: Run\r\n");
    }
    else
    {
        sprintf(acBuf, "WirelessTaskState8: Idle\r\n");
    }
    fpPrint(pData, acBuf);

    if(EOS_TRUE == pWirelessRootSt->bWirelessTaskRun9)
    {
        sprintf(acBuf, "WirelessTaskState9: Run\r\n");
    }
    else
    {
        sprintf(acBuf, "WirelessTaskState9: Idle\r\n");
    }
    fpPrint(pData, acBuf);
    
    if(EOS_TRUE == pWirelessRootSt->bWirelessTaskRun10)
    {
        sprintf(acBuf, "WirelessTaskState10: Run\r\n");
    }
    else
    {
        sprintf(acBuf, "WirelessTaskState10: Idle\r\n");
    }    
    fpPrint(pData, acBuf);
    
    if(EOS_TRUE == pWirelessRootSt->bWirelessTaskRun11)
    {
        sprintf(acBuf, "WirelessTaskState11: Run\r\n");
    }
    else
    {
        sprintf(acBuf, "WirelessTaskState11: Idle\r\n");
    }        
    fpPrint(pData, acBuf);

    if(EOS_TRUE == pWirelessRootSt->bWirelessTaskRun12)
    {
        sprintf(acBuf, "WirelessTaskState12: Run\r\n");
    }
    else
    {
        sprintf(acBuf, "WirelessTaskState12: Idle\r\n");
    }        
    fpPrint(pData, acBuf);

    
    ut_timer_show((EOS_TIMER_ID)(pWirelessRootSt->WirelessTimerID), fpPrint, pData);
    
    
    fpPrint(pData, "Max LoopTime Statistics(tick):\r\n");
    sprintf(acBuf, "MaxAppLoop: %d, MaxRcvCmdTick: %d, MaxComRcvTick: %d\r\n", 
                        pWirelessRootSt->uiWirelessMaxTick, pWirelessRootSt->uiMaxRcvCmdTick, pWirelessRootSt->uiMaxComRcvTick);
    fpPrint(pData, acBuf);
    
    fpPrint(pData, "Max Sub LoopTime Statistics(tick):\r\n");
    sprintf(acBuf, "MaxStartLoop: %d, MaxReadThreadTick: %d, MaxSendThreadTick: %d, MaxCmdProcess: %d\r\n", 
                        pWirelessRootSt->uiWirelessMaxTick1, pWirelessRootSt->uiWirelessMaxTick2, 
                        pWirelessRootSt->uiWirelessMaxTick3, pWirelessRootSt->uiWirelessMaxTick4);
    fpPrint(pData, acBuf);

    fpPrint(pData, "ATCMD Left Cnt:\r\n");
    for(i = 0; i < pWirelessRootSt->uiMaxModuleNum; i++)
    {
        T_HCHNL* pChnl = NULL;
        
        pWirelessChannelCtl = &pWirelessRootSt->stChannelSt[i];
        pChnl = pWirelessChannelCtl->pChnl;

        sprintf(acBuf, "Channel %d ATCMD Left:%d, ModReset:%d, MaxComRdLen:%d\r\n", i, eos_lst_count(&pChnl->atcmdlist), 
            pWirelessChannelCtl->uiModResetCnt, pWirelessChannelCtl->uiMaxComBuffLen);
        fpPrint(pData, acBuf);        
    }

#if ((ACOM_BOARD_TYPE == RALINK) || (ACOM_BOARD_TYPE == MT673X) || (ACOM_BOARD_TYPE == MDM9X07) || (ACOM_BOARD_TYPE == SC98XX))
    fpPrint(pData, "Channel FSM :\r\n");
    /*for(i = 0; i < pWirelessRootSt->uiMaxModuleNum; i++)
    {
        pWirelessChannelCtl = &pWirelessRootSt->stChannelSt[i];
        sprintf(acBuf, "Channel %d PreSubState: %s, SubState: %s\r\n", i, 
            WRL_FSM_GetSubEventDesc(pWirelessChannelCtl->enFSMPreSubSTATEIndex), WRL_FSM_GetSubEventDesc(pWirelessChannelCtl->enFSMSubSTATEIndex));
        fpPrint(pData, acBuf);
    }*/
#endif

    fpPrint(pData, "Channel Status 1:\r\n");
    for(i = 0; i < pWirelessRootSt->uiMaxModuleNum; i++)
    {
        pWirelessChannelCtl = &pWirelessRootSt->stChannelSt[i];
        sprintf(acBuf, "Channel %d RespTimeoutCnt: %d, Get USSD Err: %d, Jamming: %d, NoAlert: %d, ToneDet: %d\r\n", i, 
            pWirelessChannelCtl->uiRespTimeoutCnt, pWirelessChannelCtl->uiGetUSSDErr, pWirelessChannelCtl->uiModJammingCnt, pWirelessChannelCtl->uiNoAlertCnt, pWirelessChannelCtl->uiToneDetectCnt);
        fpPrint(pData, acBuf);
    }
    
    fpPrint(pData, "Channel Status 2:\r\n");
    for(i = 0; i < pWirelessRootSt->uiMaxModuleNum; i++)
    {
        pWirelessChannelCtl = &pWirelessRootSt->stChannelSt[i];

        sprintf(acBuf, "Channel %d OprID:%d, ModSMSt: %d, NetRegSt: %d, ModInitSt: %d, SIMSt: %d, FunStat:%d\r\n", i,
            pWirelessChannelCtl->OperatorID, pWirelessChannelCtl->enState, pWirelessChannelCtl->NetRegStatus, 
            pWirelessChannelCtl->ModInitStatus, pWirelessChannelCtl->SlotSIMIsNew, pWirelessChannelCtl->enFuctionStatus);
        fpPrint(pData, acBuf);
    }
    
    fpPrint(pData, "Channel Status 3:\r\n");
    for(i = 0; i < pWirelessRootSt->uiMaxModuleNum; i++)
    {
        pWirelessChannelCtl = &pWirelessRootSt->stChannelSt[i];

        sprintf(acBuf, "Channel %d CurSlot:%d, ReportReady:%d, UnVolW:%d, UnVolP:%d, OvVolW:%d, OvVolP:%d\r\n", i,
            pWirelessChannelCtl->SimSlot, pWirelessChannelCtl->bReadyReport,
            pWirelessChannelCtl->uiUnderVoltageWarningCnt, pWirelessChannelCtl->uiUnderVoltagePwroffCnt, 
            pWirelessChannelCtl->uiOverVoltageWarningCnt, pWirelessChannelCtl->uiOverVoltagePwroffCnt);
        fpPrint(pData, acBuf);
    }

    ut_mem_show(fpPrint, pData);
}

_VOID WIRELESS_ComStrWrite(_UINT32 ChanId, _CHAR8* strValue, _UINT32 Len)
{
    PWIRELESSROOTST pWirelessRootSt;
    PWIRELESSCHANNELST  pWirelessChannelCtl;
    ioctl_channel_write_st stComWrite;
    _INT32 iRetVal = -1;
    _INT32 iComWriteRtn = 0;

    pWirelessRootSt = (PWIRELESSROOTST)&stWirelessRootSt;
    pWirelessChannelCtl = &pWirelessRootSt->stChannelSt[ChanId];

    if(pWirelessChannelCtl->bModuleUpdating)
    {
        return;
    }    

    memset(&stComWrite, 0, sizeof(ioctl_channel_write_st));
    memcpy(stComWrite.buff, strValue, Len);
    stComWrite.size = Len;
    iComWriteRtn = WrlUart_Write(pWirelessChannelCtl->fUartHandle, stComWrite.buff, stComWrite.size);
    //WIRELESS_INFO("WrlUart_Write:(fd=%d)(%d)(%s)\r\n", pWirelessChannelCtl->fUartHandle, stComWrite.size, stComWrite.buff);

    if(EOS_OK == iComWriteRtn)
    {
        pWirelessChannelCtl->uiUartWriteErrCnt = 0;
    }
    else
    {
        pWirelessChannelCtl->uiUartWriteErrCnt++;
    }
 
} 

