

#include "eos.h"
#include "utl_string.h"
#include "utils.h"

#include "wm_channel.h"
#include "wireless_error.h"
#include "wireless_string.h"

#include "../wireless_common.h"
#include "../wireless_debug.h"
#include "../wireless_interface_inc.h"

extern WIRELESSROOTST  stWirelessRootSt;

_BOOL FilterShortLine;

static _BOOL wirelessIsUssdRspComplete(_UINT32 ChannelID, _UINT32 Len, _UINT32 uiHead, _CHAR8* DataBuf, _UINT32* RtnLen);
static _UINT32 WIRELESS_ReadCompleteInfo(_UINT32 ChannelID, _CHAR8* DataBuf, _UINT32 MaxLen);
static _UINT32 WIRELESS_ReadCompleteInfoEx(_UINT32 ChannelID, _CHAR8* DataBuf, _UINT32 MaxLen);

/* 2017.12.13 pengjian added to trance AT commands */
const _CHAR8* getPrintableAtCmd(const _VOID* at_cmd)
{
    const _CHAR8* cmd_str = (_CHAR8*)at_cmd;
    
    if (0 == utl_strncasecmp(cmd_str, "AT+CPIN", 7))
    {
        return "AT+CPIN";
    }
    else if (0 == utl_strncasecmp(cmd_str, "AT+CLCK", 7))
    {
        return "AT+CLCK";
    }
    else if (0 == utl_strncasecmp(cmd_str, "AT+CPWD", 7))
    {
        return "AT+CPWD";
    }
    else if (0 == utl_strncasecmp(cmd_str, "AT", 2))
    {
        return cmd_str;
    }
    else
    {
        return "data...";
    }
}

_VOID  wmChannelAtCmdTmo(_UINT32 channelid, _UINT32 uiReserved)
{
    PWIRELESSCHANNELST  pWirelessChannelCtl = &stWirelessRootSt.stChannelSt[channelid];

    pWirelessChannelCtl->tmr_wait_atrsp = 0;

    WIRELESS_INFO("Channel %d AT command timeout! Wakeup the main task...\r\n", channelid);
    
    /* just wake up the main task */
    WIRELESS_WakeUp();
}

_VOID wmChannelStopTimerWaitAtResp(_UINT32 channelid)
{
    PWIRELESSROOTST pWirelessRootSt = &stWirelessRootSt;
    PWIRELESSCHANNELST  pWirelessChannelCtl = &pWirelessRootSt->stChannelSt[channelid];

    if (pWirelessChannelCtl->tmr_wait_atrsp > 0)
    {
        ut_timer_stop(pWirelessRootSt->WirelessTimerID, pWirelessChannelCtl->tmr_wait_atrsp);
        pWirelessChannelCtl->tmr_wait_atrsp = 0;
    }
}

_VOID wmChannelStartTimerWaitAtResp(_UINT32 channelid, _INT32 timeout)
{
    PWIRELESSROOTST pWirelessRootSt = &stWirelessRootSt;
    PWIRELESSCHANNELST  pWirelessChannelCtl = &pWirelessRootSt->stChannelSt[channelid];

    ut_timer_start(pWirelessRootSt->WirelessTimerID, timeout*1000, EN_TIMER_ONESHOT,
        (TIMERFUNC) wmChannelAtCmdTmo, channelid, 0, &(pWirelessChannelCtl->tmr_wait_atrsp));            
}

_VOID wmChannelSetWaitResp(T_HCHNL* pChnl, _BOOL bWaitResp)
{
    pChnl->bWaitingRsp = bWaitResp;

    if (!pChnl->bWaitingRsp)
    {
        PWIRELESSROOTST pWirelessRootSt = &stWirelessRootSt;

        WirelessSemP(pWirelessRootSt->ATCmdLstMutex, EOS_WAIT_FOREVER);

        if (eos_lst_count(&pChnl->atcmdlist) > 0)
        {
            /* AT command queue not empty, wake the main task to send next */
            WIRELESS_WakeUp();
        }
        
        WirelessSemV(pWirelessRootSt->ATCmdLstMutex);
    }
}

/**
 *@briefͨ�������������ͨ��������ͨ�����
 *@param    ChID             [in]ͨ����
 *@param pWM_Handle         [in]����ģ����

 *@return     T_HCHNL*
 *@retval    ��������ͨ�����ָ��
*/
HCHNL CreateChannel(HWM pWM_Handle, _USHORT16 ChID, T_HCHNL *pChnl)
{

/*
    T_HCHNL *pChnl = NULL;

    pChnl = (T_HCHNL*)WIRELESS_MALLOC(sizeof(T_HCHNL));
    memset(pChnl, 0, sizeof(T_HCHNL));
*/
    /*���ͨ�����*/
    pChnl->ChID = ChID;
    pChnl->ulQueueSize = ATQueueSize;
    pChnl->lFreeSpace = ATQueueSize;    
    pChnl->hWM = pWM_Handle;
    pChnl->channelMode = CHANNEL_MODE_AT;
    pChnl->pURC = NULL;
    pChnl->pResult = NULL;

    pChnl->ATRsp.lpData = pChnl->ATBuffer;
    pChnl->ATRsp.bufferSize = ATBufferSize;
    pChnl->ATRsp.bNeedFree = EOS_FALSE;
    pChnl->ATRsp.ulData = 0;

    /*�����������*/
    eos_lst_init(&(pChnl->atcmdlist));
    /*������Ӧ����*/
    eos_lst_init(&(pChnl->atrsplist));

    FilterShortLine = EOS_TRUE;
    return pChnl;
}

_INT32 SuspendChannel(HWM pWM_Handle, _USHORT16 ChID)
{

    #if 0 /* Kinson */
    _UINT32 count = 0;

    if(!pWM_Handle->bInitOK)
    {
        return RIL_RTN_WM_NOTREADY;
    }

    for (count = 0; count < (pWM_Handle->ulChnlNum); count++)
    {
        if(pWM_Handle->Chnl_Array[count]->ChID == ChID)
        {
            AK_Suspend_Task(pWM_Handle->Chnl_Array[count]->hReceiveTask);
            AK_Suspend_Task(pWM_Handle->Chnl_Array[count]->hSendTask);
            break;
        }
    }
    #endif
    return 0;
}

_INT32 ResumeChannel(HWM pWM_Handle, _USHORT16 ChID)
{
    #if 0 /* Kinson */
    _UINT32 count = 0;

    if(!pWM_Handle->bInitOK)
    {
        return RIL_RTN_WM_NOTREADY;
    }

    for (count = 0; count < (pWM_Handle->ulChnlNum); count++)
    {
        if(pWM_Handle->Chnl_Array[count]->ChID == ChID)
        {
            AK_Resume_Task(pWM_Handle->Chnl_Array[count]->hReceiveTask);
            AK_Resume_Task(pWM_Handle->Chnl_Array[count]->hSendTask);
            break;
        }
    }
    #endif
    return 0;
}

/**
 *@briefͨ�������������ͨ��
 *@param    pChnl        [in]ͨ�����

 *@return     _UINT32
 *@retval    >0 ����ͨ���ɹ�
*/
_INT32 DestroyChannel(HCHNL hChnl)
{
    _INT32 status = 0;
#if 0 /* Kinson */
    if(hChnl->hSendTask >= 0)
    {
        /*��ֹ�����߳�*/
        status = AK_Terminate_Task(hChnl->hSendTask);
        /*ɾ��������*/
        status = AK_Delete_Task(hChnl->hSendTask);
    }

    if(hChnl->hReceiveTask >= 0)
    {
        /*��ֹ�����߳�*/
        status = AK_Terminate_Task(hChnl->hReceiveTask);
        /*ɾ�������߳�*/
        status = AK_Delete_Task(hChnl->hReceiveTask);
    }
#endif    
#if 0
    if(hChnl->hATCQ >= 0)
    {
        /*ɾ��AT�������*/
        status = AK_Delete_Queue(hChnl->hATCQ);
    }
    if(hChnl->hRSPQ >= 0)
    {
        /*ɾ����Ӧ����*/
        status = AK_Delete_Queue(hChnl->hRSPQ);
    }
#endif

    /*ɾ��ͨ�����*/
    WIRELESS_FREE(hChnl);
    hChnl= NULL; 
    return status;
}


/**
 *@briefͨ������������AT����ͨ��
 *@param    pChnl        [in]ͨ�����

 *@return     _UINT32
 *@retval    >0 ���ͨ���ɹ�
*/
_INT32 EmptyChannel(HCHNL hChnl)
{
    _INT32 status = 0;

#if 0
    if(hChnl->hReceiveTask >= 0)
    {
        /*��ֹ�����߳�*/
        status = AK_Terminate_Task(hChnl->hReceiveTask);
        /*ɾ�������߳�*/
        status = AK_Delete_Task(hChnl->hReceiveTask);

        hChnl->hReceiveTask = -1;
    }

    if(hChnl->hRSPQ >= 0)
    {
        /*ɾ����Ӧ����*/
        status = AK_Delete_Queue(hChnl->hRSPQ);
        hChnl->hRSPQ = -1;
    }
#endif    
    return status;
}

_VOID SendThread(_VOID)
{
    T_HCHNL* pChnl = NULL;
    _INT32 RspStatus[ACOM_PORT_NUM] = {0};
    T_ATCMD*    pATCmd[ACOM_PORT_NUM] = {NULL};
    static T_ATCMD    sATCmd[ACOM_PORT_NUM];
    T_ATCMD*    pChnlATCmd = NULL;
    
    _UINT32     ATCmd_Size[ACOM_PORT_NUM] = {0};
    static T_ATRESPONSE ATRsp[ACOM_PORT_NUM];
    
    _UINT32         ATRsp_Size[ACOM_PORT_NUM] = {0};
    RIL_PARSE_RES  Result_Type[ACOM_PORT_NUM] = {RIL_prNULL};
    _UINT32  Result_Num[ACOM_PORT_NUM] = {0};
    _VOID* Result_Data[ACOM_PORT_NUM] = {NULL};
    _UINT32  Result_size[ACOM_PORT_NUM] = {0};
    _UINT32    uiChannelid = 0;
    _INT32  iLstCount = 0;
    _UINT32 i = 0;
    _UINT32 j = 0;
    _UINT32 uiWriteCnt = 0;
    UtRtsDefine(rts);

    PWIRELESSROOTST pWirelessRootSt;
    PWIRELESSCHANNELST  pWirelessChannelCtl;

    pWirelessRootSt = (PWIRELESSROOTST)&stWirelessRootSt;
    
    for(uiChannelid = 0; uiChannelid < pWirelessRootSt->uiMaxModuleNum; uiChannelid++)
    {
        pWirelessChannelCtl = &pWirelessRootSt->stChannelSt[uiChannelid];
        pChnl = pWirelessChannelCtl->pChnl;
        pChnlATCmd = &(pWirelessChannelCtl->stATCmd);
         
        if(EOS_FALSE == pChnl->bWaitingRsp)
        {
            if(0)//if((tickGet() - pWirelessChannelCtl->uiLastATCmdSendTick) <= EOS_MS2TICK(40))
            {
                continue;
            }
        
            /* Clear AT Response List    */    
            //WirelessSemP(pWirelessRootSt->RSPLstMutex, EOS_WAIT_FOREVER);
            iLstCount = eos_lst_count(&pChnl->atrsplist);
            while(iLstCount > 0)
            {            
                T_ATRESPONSE*    pATRspTemp = NULL;

                pATRspTemp = (T_ATRESPONSE*)eos_lst_get(&pChnl->atrsplist);
                if(EOS_TRUE == pATRspTemp->bNeedFree)
                {
                     WIRELESS_FREE(pATRspTemp->lpData);
                     pATRspTemp->bNeedFree = EOS_FALSE;
                }    
                
                WIRELESS_FREE(pATRspTemp);
                iLstCount = eos_lst_count(&pChnl->atrsplist);        
                
                WIRELESS_INFO("Channel %d Del atRspList\r\n", uiChannelid);
            }            
            //WirelessSemV(pWirelessRootSt->RSPLstMutex);

            memset(&(ATRsp[uiChannelid]), 0, sizeof(T_ATRESPONSE));
            
            WirelessSemP(pWirelessRootSt->ATCmdLstMutex, EOS_WAIT_FOREVER);            
            iLstCount = eos_lst_count(&pChnl->atcmdlist);
            if(iLstCount > 0)
            {                
                UtRtsBegin(rts);
                if(NULL != pChnlATCmd->lpData)
                {                    
                    WIRELESS_FREE(pChnlATCmd->lpData);
                    pChnlATCmd->lpData = NULL;
                }
                if(NULL != pChnlATCmd->pURC)
                {
                    WIRELESS_FREE(pChnlATCmd->pURC);
                    pChnlATCmd->pURC = NULL;
                }
                if(NULL != pChnlATCmd->pResult)
                {
                    WIRELESS_FREE(pChnlATCmd->pResult);
                    pChnlATCmd->pResult = NULL;
                }                
                if(NULL != pChnlATCmd->customHandle)
                {
                    WIRELESS_FREE(pChnlATCmd->customHandle);
                    pChnlATCmd->customHandle = NULL;
                }
                
                memset(pChnlATCmd, 0, sizeof(T_ATCMD));
                
                pATCmd[uiChannelid] = (PT_ATCMD)eos_lst_get(&pChnl->atcmdlist);
                memcpy(pChnlATCmd, pATCmd[uiChannelid], sizeof(T_ATCMD));
                
                WirelessSemV(pWirelessRootSt->ATCmdLstMutex);
                WIRELESS_FREE(pATCmd[uiChannelid]);
                pATCmd[uiChannelid] = NULL;
                UtRtsEnd(rts);
            }
            else
            {
                WirelessSemV(pWirelessRootSt->ATCmdLstMutex);
                continue;
            }            

            if(pChnlATCmd->CmdID == pChnl->hWM->InvalidCmdID)
            {
                /*this command has been canceled*/
                if(NULL != pChnlATCmd->lpData)
                {                    
                    WIRELESS_FREE(pChnlATCmd->lpData);
                    pChnlATCmd->lpData = NULL;
                }
                if(NULL != pChnlATCmd->pURC)
                {
                    WIRELESS_FREE(pChnlATCmd->pURC);
                    pChnlATCmd->pURC = NULL;
                }
                if(NULL != pChnlATCmd->pResult)
                {
                    WIRELESS_FREE(pChnlATCmd->pResult);
                    pChnlATCmd->pResult = NULL;
                }        
                if(NULL != pChnlATCmd->customHandle)
                {
                    WIRELESS_FREE(pChnlATCmd->customHandle);
                    pChnlATCmd->customHandle = NULL;                    
                }

                memset(pChnlATCmd, 0, sizeof(T_ATCMD));
                #if 0
                if(WM_IsSleepAllowed(pChnl->hWM))
                {
                    WM_SleepModule(pChnl->hWM);
                }
                #endif
                WIRELESS_ERROR(WIRELESS_ERRORNO, "CmdId(%d) == InvalidCmdId\r\n", pChnlATCmd->CmdID);
                continue;
            }

            pChnl->hWM->InvalidCmdID = NEWCMDID;
            if(CMDTYPE_AT == pChnlATCmd->CmdType)
            {
                #if 0
                WM_WakeupModule(pChnl->hWM);
                #endif
                if(NULL != pChnlATCmd->pURC)
                {
                    pChnl->pURC = pChnlATCmd->pURC;
                }
                if(NULL != pChnlATCmd->pResult)
                {
                    pChnl->pResult = pChnlATCmd->pResult;
                }

                /*    clean response buffer    */
                pChnl->ATRsp.ulData = 0;

                /* 2017.12.13 pengjian added to trace the AT commands */
                WIRELESS_INFO("run AT command! id = %d\n%s\n", 
                    pChnlATCmd->CmdID, getPrintableAtCmd(pChnlATCmd->lpData));
                
                WIRELESS_ComStrWrite(uiChannelid,((_CHAR8*)(pChnlATCmd->lpData)),pChnlATCmd->ulData);                

                if(1 == pWirelessRootSt->senddata.bQiSackLock)
                {
                    if(NVM_GOIP_MODTYPE_M26 == pWirelessChannelCtl->ModuleType)
                    {
                        if(Utl_StrFndL((_CHAR8*)(pChnlATCmd->lpData), "AT+QISACK", 0, pChnlATCmd->ulData) != -1)
                        {
                            pWirelessRootSt->senddata.bQiSackLock = 0;
                            WIRELESS_INFO("pWirelessRootSt->senddata.bQiSackLock = 0\r\n");
                        }
                    }
                    else if(NVM_GOIP_MODTYPE_SIM800 == pWirelessChannelCtl->ModuleType)
                    {
                        if(Utl_StrFndL((_CHAR8*)(pChnlATCmd->lpData), "AT+CIPACK", 0, pChnlATCmd->ulData) != -1)
                        {
                            pWirelessRootSt->senddata.bQiSackLock = 0;
                            WIRELESS_INFO("pWirelessRootSt->senddata.bQiSackLock = 0\r\n");
                        }
                    }
                }
                if(1 == pWirelessRootSt->rcvdata.bQiRcvLock)
                {
                    if(NVM_GOIP_MODTYPE_M26 == pWirelessChannelCtl->ModuleType)
                    {
                        if(Utl_StrFndL((_CHAR8*)(pChnlATCmd->lpData), "AT+QIRD", 0, pChnlATCmd->ulData) != -1)
                        {
                            pWirelessRootSt->rcvdata.bQiRcvLock = 0;
                            WIRELESS_INFO("pWirelessRootSt->rcvdata.bQiRcvLock = 0\r\n");
                        }
                    }
                    else if(NVM_GOIP_MODTYPE_SIM800 == pWirelessChannelCtl->ModuleType)
                    {
                        if(Utl_StrFndL((_CHAR8*)(pChnlATCmd->lpData), "AT+CIPRXGET", 0, pChnlATCmd->ulData) != -1)
                        {
                            pWirelessRootSt->rcvdata.bQiRcvLock = 0;
                            WIRELESS_INFO("pWirelessRootSt->rcvdata.bQiRcvLock = 0\r\n");
                        }
                    }
                }
                if((NVM_GOIP_MODTYPE_C202 == pWirelessChannelCtl->ModuleType)
                   || (NVM_GOIP_MODTYPE_MC323 == pWirelessChannelCtl->ModuleType))
                {
                    if(Utl_StrFndL((_CHAR8*)(pChnlATCmd->lpData), "CDV", 0, pChnlATCmd->ulData) != -1)    /* have make a call,wait response    */                            
                    {
                        pWirelessChannelCtl->bWaitATDResponse = EOS_FALSE;
                        pWirelessChannelCtl->bATDSendFakeResponse = EOS_FALSE;
                        WIRELESS_INFO("Channel %d Set WaitATDResponse False\r\n", uiChannelid);            
                    }
                    else if(Utl_StrFndL((_CHAR8*)(pChnlATCmd->lpData), "CHV", 0, pChnlATCmd->ulData) != -1)    /* have make a call,wait response    */                            
                    {
                        pWirelessChannelCtl->bWaitATDResponse = EOS_FALSE;
                        pWirelessChannelCtl->bIgnoreATDResponse = EOS_FALSE;
                        WIRELESS_INFO("Channel %d Set WaitATDResponse True\r\n", uiChannelid);            
                    }
                }
                else if((NVM_GOIP_MODTYPE_M10 != pWirelessChannelCtl->ModuleType)
                       && (NVM_GOIP_MODTYPE_U10 != pWirelessChannelCtl->ModuleType)
                       && (NVM_GOIP_MODTYPE_UC15 != pWirelessChannelCtl->ModuleType)
                       && (NVM_GOIP_MODTYPE_UC20 != pWirelessChannelCtl->ModuleType)
                       && (NVM_GOIP_MODTYPE_MC600 != pWirelessChannelCtl->ModuleType))
                {
                    if(Utl_StrFndL((_CHAR8*)(pChnlATCmd->lpData), "ATD", 0, pChnlATCmd->ulData) != -1)    /* have make a call,wait response    */                            
                    {
                        pWirelessChannelCtl->bWaitATDResponse = EOS_TRUE;
                        WIRELESS_SetFakeResponse(uiChannelid, EOS_TRUE);
                        
                        WIRELESS_INFO("Channel %d Set WaitATDResponse True\r\n", uiChannelid);            
                    }
                    else if(Utl_StrFndL((_CHAR8*)(pChnlATCmd->lpData), "ATH", 0, pChnlATCmd->ulData) != -1)    /* end a call    */                            
                    {
                        pWirelessChannelCtl->bWaitATDResponse = EOS_FALSE;
                        pWirelessChannelCtl->bIgnoreATDResponse = EOS_FALSE;
                        WIRELESS_INFO("Channel %d Set WaitATDResponse False\r\n", uiChannelid);            
                    }
                }
                
                pChnl->uiSendATTick = tickGet();

                /* stop the timer if it is still started */
                wmChannelStopTimerWaitAtResp(uiChannelid);
                
                /* start the timer if a timeout specified */
                if (pChnlATCmd->timeout > 0)
                {
                    wmChannelStartTimerWaitAtResp(uiChannelid, pChnlATCmd->timeout);
                }
                
                /*WIRELESS_INFO("Channel %d len %d,AT Write:%s\r\n", uiChannelid, pChnlATCmd->ulData, pChnlATCmd->lpData);
                
                printf("Channel %d AT Write:%s\r\n", uiChannelid, pChnlATCmd->lpData);*/
                
                wmChannelSetWaitResp(pChnl, EOS_TRUE);
            }
            else
            {
                /*process hardware command*/
                Result_Type[uiChannelid] = RIL_prERROR;
                switch(pChnlATCmd->CmdType) 
                {
                    case CMDTYPE_IGNITE:
                        (*(pChnl->hWM->pATFunction->P_IgniteModule))();
                        if((*(pChnl->hWM->pATFunction->P_IgniteResult))())
                        {
                            Result_Type[uiChannelid] = RIL_prOK;
                        }
                        break;
                    case CMDTYPE_POWEROFF:
                        (*(pChnl->hWM->pATFunction->P_PowerOffModule))();
                        if((*(pChnl->hWM->pATFunction->P_PowerOffResult))())
                        {
                            Result_Type[uiChannelid] = RIL_prOK;
                        }
                        break;
                    case CMDTYPE_RESETMODULE:
                        (*(pChnl->hWM->pATFunction->P_ResetModule))();
                        if((*(pChnl->hWM->pATFunction->P_ResetResult))())
                        {
                            Result_Type[uiChannelid] = RIL_prOK;
                        }
                        break;
                    case CMDTYPE_RESETWM:
                        Result_Type[uiChannelid] = RIL_prOK;
                        break;
                    default:
                        break;
                }
                ATServer_ResultCallback(uiChannelid, pChnlATCmd->serviceHandle, pChnlATCmd->serviceType, Result_Type[uiChannelid],
                    NULL, 0, 0, pChnlATCmd->customHandle);

                pChnlATCmd->customHandle = NULL;

                wmChannelSetWaitResp(pChnl, EOS_FALSE);

                memset(pChnlATCmd, 0, sizeof(T_ATCMD));
                #if 0
                if(WM_IsSleepAllowed(pChnl->hWM))
                {
                    WM_SleepModule(pChnl->hWM);
                }
                #endif
            }
            
            pWirelessChannelCtl->uiLastATCmdSendTick = tickGet();
        }
        else
        {
            /*�ȴ�AT�������Ӧ���. ��������Ӧ������ɽ����߳��յ���push��������.
            �����ָ��ʱ���ڻ�û���յ���Ӧ������ʱ����*/
           // WirelessSemP(pWirelessRootSt->RSPLstMutex, EOS_WAIT_FOREVER);
            iLstCount = eos_lst_count(&pChnl->atrsplist);
            if(iLstCount > 0)
            {
                T_ATRESPONSE* pATRsp = (PT_ATRESPONSE)eos_lst_get(&pChnl->atrsplist);
                
                memcpy(&ATRsp[uiChannelid], pATRsp, sizeof(T_ATRESPONSE));
                
                //WirelessSemV(pWirelessRootSt->RSPLstMutex);
                //weikk 20160304 WIRELESS_INFO("Channel %d Get a Response, iLstCount = %d\r\n", uiChannelid, iLstCount);                 
                WIRELESS_FREE(pATRsp);
                RspStatus[uiChannelid] = 0;

                /* since the response is received, stop the timer */
                wmChannelStopTimerWaitAtResp(uiChannelid);
            }
            else
            {
                //WirelessSemV(pWirelessRootSt->RSPLstMutex);
                if((tickGet() - pChnl->uiSendATTick) >= (pChnlATCmd->timeout*sysClkRateGet()))
                {
                    #if 0
                    if(pWirelessChannelCtl->bWaitATDResponse)
                    {
                        pChnl->uiSendATTick = tickGet();
                        continue;
                    }
                    else
                    #endif
                    {

                        WM_SetCancel(pChnl->hWM, uiChannelid);

                        //WIRELESS_INFO("Channel %d (%d - %d)timeout = %d \r\n", uiChannelid, tickGet(), pChnl->uiSendATTick, pChnlATCmd->timeout);
                        RspStatus[uiChannelid] = WIRELESS_TIMEOUT;
                    }
                }
                else
                {
                    continue;
                }
            }

                /*parse AT command response*/
            if(WIRELESS_TIMEOUT == RspStatus[uiChannelid])
            {
                Result_Type[uiChannelid] = RIL_prTIMEOUT;
                Result_Data[uiChannelid] = NULL;
                Result_Num[uiChannelid] = 0;
                Result_size[uiChannelid] = 0;
                pWirelessChannelCtl->uiRespTimeoutCnt++;
                //WIRELESS_ERROR(uiChannelid, "Channel %d Get a Response Timeout\r\n", uiChannelid);
            }
            else
            {
                pWirelessChannelCtl->uiRespTimeoutCnt = 0;

                Result_Data[uiChannelid] = (*(pChnlATCmd->pParseRsp))(&Result_Type[uiChannelid], (const _CHAR8*)(ATRsp[uiChannelid]).lpData,
                                                                                 &(Result_Num[uiChannelid]), &(Result_size[uiChannelid]));
                if(EOS_TRUE == ATRsp[uiChannelid].bNeedFree)
                {
                     WIRELESS_FREE(ATRsp[uiChannelid].lpData);
                     ATRsp[uiChannelid].bNeedFree = EOS_FALSE;
                } 
            }

            /*If met error but retry count > 1, execute the same AT command again*/
            if((RIL_prNOCARRIER < Result_Type[uiChannelid]) && (pChnlATCmd->retryTimes > 1))
            {
                //weikk 20160307 WIRELESS_INFO("SendThread need retry,but didn't acomplished\r\n");
#if 0
                PT_ATCMD pATCmdRetry = (PT_ATCMD)WIRELESS_MALLOC(sizeof(T_ATCMD));
                if(NULL == pATCmdRetry)
                {
                    WIRELESS_ERROR(uiChannelid, "channel  get atcmdListMemPool error\r\n");
                    return EOS_ERROR;
                }    
                pChnlATCmd->retryTimes--;
                #if 0
                AK_Send_To_Front_of_Queue(pChnl->hATCQ, pATCmd, sizeof(T_ATCMD), AK_SUSPEND);
                #endif
                memset(pATCmdRetry, 0, sizeof(T_ATCMD));
                memcpy(pATCmdRetry, pChnlATCmd, sizeof(T_ATCMD));

                WirelessSemP(pWirelessRootSt->ATCmdLstMutex, EOS_WAIT_FOREVER);
                eos_lst_addfront(&pChnl->atcmdlist, &pATCmdRetry->stnode);    
                WirelessSemV(pWirelessRootSt->ATCmdLstMutex);
                continue;
#endif
            }

            /* free the at command data buffer */
            if(pChnlATCmd->lpData != NULL)
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

            /*call result callback according to the executing mode*/
            if((EXE_STOPWHENERROR == pChnlATCmd->executeMode && RIL_prNOCARRIER < Result_Type[uiChannelid]) 
                || (EXE_STOPWHENOK == pChnlATCmd->executeMode && RIL_prNOCARRIER >= Result_Type[uiChannelid]))
            {
                pChnl->hWM->InvalidCmdID = pChnlATCmd->CmdID;
                ATServer_ResultCallback(uiChannelid, pChnlATCmd->serviceHandle, pChnlATCmd->serviceType, Result_Type[uiChannelid],
                    Result_Data[uiChannelid], Result_size[uiChannelid], Result_Num[uiChannelid], pChnlATCmd->customHandle);
                pChnlATCmd->customHandle = NULL;
            }
            else
            {
                if(pChnlATCmd->bNotify)
                {
                    ATServer_ResultCallback(uiChannelid, pChnlATCmd->serviceHandle, pChnlATCmd->serviceType, Result_Type[uiChannelid],
                        Result_Data[uiChannelid], Result_size[uiChannelid], Result_Num[uiChannelid], pChnlATCmd->customHandle);    
                    pChnlATCmd->customHandle = NULL;
                }
                else
                {
                    if(NULL != Result_Data[uiChannelid])
                    {
                        WIRELESS_FREE(Result_Data[uiChannelid]);
                    }
                }
            }

            wmChannelSetWaitResp(pChnl, EOS_FALSE);
            
            memset(pChnlATCmd, 0, sizeof(T_ATCMD));
            
            memset(&(ATRsp[uiChannelid]), 0, sizeof(T_ATRESPONSE));
        }
    }
}

_BOOL WAVEC_CheckATResultEx(const _CHAR8* ATResponse, _USHORT16 size)
{
    if (strstr(ATResponse, "OK\r\n") != NULL ||
         strstr(ATResponse, "ERROR") != NULL ||
         strstr(ATResponse, "+CPIN:") != NULL) 
    {
        if(strstr(ATResponse, "OK\r\n") != NULL)
        {
            WIRELESS_INFO("ATResult OK\r\n");
        }
        else if(strstr(ATResponse, "ERROR") != NULL)
        {
            WIRELESS_INFO("ATResult ERROR\r\n");
        }
        else if(strstr(ATResponse, ">") != NULL)
        {
            if(strstr(ATResponse, "+CIPRXGET:") != NULL)
            {
                return EOS_FALSE;
            }
            
            WIRELESS_INFO("ATResult >\r\n");
        }
        else if(strstr(ATResponse, "+CPIN:") != NULL)
        {
            WIRELESS_INFO("ATResult CPIN\r\n");
        }
        
        return EOS_TRUE;
    }
    
    return EOS_FALSE;
}

static _BOOL wirelessIsUssdRspComplete(_UINT32 ChannelID, _UINT32 Len, _UINT32 uiHead, _CHAR8* DataBuf, _UINT32* RtnLen)
{
    _SHORT16 sStrLoc = 0;
    _UINT32 uiTemp = 0;
    _UINT32 uiTempHead= 0;
     PWIRELESSROOTST pWirelessRootSt;
    PWIRELESSCHANNELST  pWirelessChannelCtl;
    _BOOL    bRtnValue = 0;
        
    pWirelessRootSt = (PWIRELESSROOTST)&stWirelessRootSt;
    pWirelessChannelCtl =  &pWirelessRootSt->stChannelSt[ChannelID];

    uiTempHead = pWirelessChannelCtl->uiExtComHead;
    uiTemp = Len - 2; /* End Flag and -1 */
    
    WIRELESS_INFO("Channel %d CUSD BUFF:%s\r\n", ChannelID, DataBuf);

    if(Utl_StrFndNL(DataBuf, "\",", 0, uiTemp, uiTemp) != -1)
    {            
        WIRELESS_INFO("Channel %d CUSD Completed, i = %d\r\n", ChannelID, Len);

        DataBuf[Len] = 0;
        pWirelessChannelCtl->uiExtComHead = uiHead & (WIRELESS_EXTCOMBUF_MAXSIZE - 1);

        *RtnLen = Len;
        bRtnValue = EOS_TRUE;
    }
    else
    {
        sStrLoc = Utl_StrFndNL(DataBuf, "AT+CNMI", 0, uiTemp, uiTemp);
        if(sStrLoc != -1)
        {
            WIRELESS_INFO("Channel %d CUSD Completed, i = %d\r\n", ChannelID, sStrLoc);
            
            DataBuf[sStrLoc + 1] = '\"';
            DataBuf[sStrLoc + 2] = ',';
            DataBuf[sStrLoc + 3] = '\0';
            pWirelessChannelCtl->uiExtComHead = uiHead & (WIRELESS_EXTCOMBUF_MAXSIZE - 1);//sStrLoc;

            *RtnLen = sStrLoc+3;
            bRtnValue = EOS_TRUE;
        }
        else
        {
            if(Len > WIRELESS_USSD_MAX_LEN)
            {
                WIRELESS_ERROR(WIRELESS_ERRORNO,"Channel %d USSD Rsp OverSize\r\n", ChannelID);
                DataBuf[Len] = 0;
                pWirelessChannelCtl->uiExtComHead = uiHead & (WIRELESS_EXTCOMBUF_MAXSIZE - 1);
                pWirelessChannelCtl->uiGetUSSDErr++;
                *RtnLen = 0;
                bRtnValue = EOS_TRUE;
            }
            else
            {
                if((tickGet() - pWirelessChannelCtl->uiSendUSSDStartTick) > WIRELESS_USSD_GETRSP_TIMEOUT)
                {
                    WIRELESS_ERROR(WIRELESS_ERRORNO,"Channel %d USSD Rsp TimeOut\r\n", ChannelID);
                    DataBuf[Len] = 0;
                    pWirelessChannelCtl->uiExtComHead = uiHead & (WIRELESS_EXTCOMBUF_MAXSIZE - 1);
                    pWirelessChannelCtl->uiGetUSSDErr++;    
                    *RtnLen = 0;
                    bRtnValue = EOS_TRUE;
                }
                else
                {
                    bRtnValue = EOS_FALSE;
                }
            }
        }
    }

    return bRtnValue;
}

static _UINT32 WIRELESS_ReadCompleteInfo(_UINT32 ChannelID, _CHAR8* DataBuf, _UINT32 MaxLen)
{
    _UINT32 i = 0;
    _UINT32 uiTemp = 0;
    _UINT32 uiTempHead= 0;
    _SHORT16 sStrLoc = 0;
    _BOOL    bRtnValue = 0;
    _UINT32 RtnLen;
    _UINT32 uiComDataLen = 0;
     PWIRELESSROOTST pWirelessRootSt;
    PWIRELESSCHANNELST  pWirelessChannelCtl;
        
    pWirelessRootSt = (PWIRELESSROOTST)&stWirelessRootSt;
    pWirelessChannelCtl =  &pWirelessRootSt->stChannelSt[ChannelID];

    if(pWirelessChannelCtl->bModuleUpdating)
    {
        return 0;
    }

    uiTempHead = pWirelessChannelCtl->uiExtComHead;
    while(uiTempHead != pWirelessChannelCtl->uiExtComTail)
    {
        //WirelessSemP(pWirelessChannelCtl->ComBufMutex, EOS_WAIT_FOREVER);
        DataBuf[i++] = (pWirelessChannelCtl->pucExtComBuf)[uiTempHead++];
        //WirelessSemV(pWirelessChannelCtl->ComBufMutex);

        uiTempHead &= (WIRELESS_EXTCOMBUF_MAXSIZE - 1);
        if(i >= (MaxLen - 1))
        {
            /* 2012.01.08 pengjian added to avoid overflow */
            i = MaxLen - 1;
            
            pWirelessChannelCtl->uiExtComHead = uiTempHead;
            DataBuf[i] = 0;
            return i;
        }
        
        if((AT_END_FLAG == DataBuf[i - 1]) || (SMS_SEND_FLAG == DataBuf[i - 1]))
        {
            if(i > 6)
            {
                uiTemp = i - 2;

                /*    Deal With USSD response    */
                /* Just for UC15    */
                if(((NVM_GOIP_MODTYPE_UC15 == pWirelessChannelCtl->ModuleType) || (NVM_GOIP_MODTYPE_UC20 == pWirelessChannelCtl->ModuleType))
                     && (Utl_StrFndNL(DataBuf, (_CHAR8*)"AT+CUSD=", 0, uiTemp, uiTemp) != -1)
                     && (uiTemp > 10))
                {
                    _UINT32 uiCUSDStart = 0;

                    uiCUSDStart = Utl_StrFndNL(DataBuf, (_CHAR8*)"+CUSD:", 0, uiTemp, uiTemp);
                    if(uiCUSDStart != -1)
                    {
                        bRtnValue = wirelessIsUssdRspComplete(ChannelID, i, uiTempHead, (DataBuf + uiCUSDStart), &RtnLen);
                        if(bRtnValue)
                        {
                            pWirelessChannelCtl->uiExtComHead = uiTempHead;
                            return RtnLen;
                        }
                        else
                        {
                            continue;
                        }
                    }
                    else
                    {
                        continue;
                    }
                }
                
                if(/*(pWirelessChannelCtl->bWaitCUSDResponse)
                     && */((Utl_StrFndNL(DataBuf, (_CHAR8*)"+CUSD: 2,", 0, uiTemp, uiTemp) != -1) 
                        || (Utl_StrFndNL(DataBuf,(_CHAR8*) "+CUSD: 1,", 0, uiTemp, uiTemp) != -1)
                        || (Utl_StrFndNL(DataBuf,(_CHAR8*) "+CUSD: 0,", 0, uiTemp, uiTemp) != -1)
                        || (Utl_StrFndNL(DataBuf,(_CHAR8*) "+CUSD: 5,", 0, uiTemp, uiTemp) != -1)))
                {
                    bRtnValue = wirelessIsUssdRspComplete(ChannelID, i, uiTempHead, DataBuf, &RtnLen);
                    if(bRtnValue)
                    {
                        pWirelessChannelCtl->uiExtComHead = uiTempHead;
                        return RtnLen;
                    }
                    else
                    {
                        continue;
                    }
                }                
            }

            if(Utl_StrFndNL(DataBuf, (_CHAR8*)"+CIPRXGET:", 0, uiTemp, uiTemp) != -1)
            {
                if(SMS_SEND_FLAG == DataBuf[i - 1])
                {
                    
                    WIRELESS_INFO("CipRxGet Don't use > as end char\r\n");
                    continue;
                }
            }


            DataBuf[i] = 0;
            uiTempHead &= (WIRELESS_EXTCOMBUF_MAXSIZE - 1);
            pWirelessChannelCtl->uiExtComHead = uiTempHead;
            
            return i;
        }
    }

    if((pWirelessChannelCtl->bWaitATDResponse) && (pWirelessChannelCtl->bATDSendFakeResponse))
    {
        /*if((strstr(DataBuf, "ATD") != NULL) && (strstr(DataBuf, ";\r\n") != NULL))*/
        {
            pWirelessChannelCtl->uiExtComHead = uiTempHead & (WIRELESS_EXTCOMBUF_MAXSIZE - 1);
            strcpy(&DataBuf[i], (_CHAR8 *)"OK\r\n");
            pWirelessChannelCtl->bATDSendFakeResponse = EOS_FALSE;
            WIRELESS_INFO("ATD Fake Response:%s\r\n", DataBuf);
            return (i + sizeof("OK\r\n") - 1);
        }
    }

    DataBuf[0] = 0;
    return 0;
}

static _UINT32 WIRELESS_ReadCompleteInfoEx(_UINT32 ChannelID, _CHAR8* DataBuf, _UINT32 MaxLen)
{
    _UINT32 i = 0;
    _UINT32 uiTemp = 0;
    _UINT32 uiTempHead= 0;
    _SHORT16 sStrLoc = 0;
    _BOOL    bRtnValue = 0;
    _UINT32 RtnLen;
    _UINT32 uiComDataLen = 0;
     PWIRELESSROOTST pWirelessRootSt;
    PWIRELESSCHANNELST  pWirelessChannelCtl;
        
    pWirelessRootSt = (PWIRELESSROOTST)&stWirelessRootSt;
    pWirelessChannelCtl =  &pWirelessRootSt->stChannelSt[ChannelID];

    if(pWirelessChannelCtl->bModuleUpdating)
    {
        return 0;
    }

    uiTempHead = pWirelessChannelCtl->uiExtComHead;
    while(uiTempHead != pWirelessChannelCtl->uiExtComTail)
    {
        //WirelessSemP(pWirelessChannelCtl->ComBufMutex, EOS_WAIT_FOREVER);
        DataBuf[i++] = (pWirelessChannelCtl->pucExtComBuf)[uiTempHead++];
        //WirelessSemV(pWirelessChannelCtl->ComBufMutex);

        uiTempHead &= (WIRELESS_EXTCOMBUF_MAXSIZE - 1);
        if(i >= (MaxLen - 1))
        {
            /* 2012.01.08 pengjian added to avoid overflow */
            i = MaxLen - 1;
            
            //pWirelessChannelCtl->uiExtComHead = uiTempHead;
            DataBuf[i] = 0;
            return i;
        }
        
        if((AT_END_FLAG == DataBuf[i - 1]) || (SMS_SEND_FLAG == DataBuf[i - 1]))
        {
            if(i > 6)
            {
                uiTemp = i - 2;

                /*    Deal With USSD response    */
                /* Just for UC15    */
                if(((NVM_GOIP_MODTYPE_UC15 == pWirelessChannelCtl->ModuleType) || (NVM_GOIP_MODTYPE_UC20 == pWirelessChannelCtl->ModuleType))
                     && (Utl_StrFndNL(DataBuf, (_CHAR8*)"AT+CUSD=", 0, uiTemp, uiTemp) != -1)
                     && (uiTemp > 10))
                {
                    _UINT32 uiCUSDStart = 0;

                    uiCUSDStart = Utl_StrFndNL(DataBuf, (_CHAR8*)"+CUSD:", 0, uiTemp, uiTemp);
                    if(uiCUSDStart != -1)
                    {
                        bRtnValue = wirelessIsUssdRspComplete(ChannelID, i, uiTempHead, (DataBuf + uiCUSDStart), &RtnLen);
                        if(bRtnValue)
                        {
                            //pWirelessChannelCtl->uiExtComHead = uiTempHead;
                            return RtnLen;
                        }
                        else
                        {
                            continue;
                        }
                    }
                    else
                    {
                        continue;
                    }
                }
                
                if(/*(pWirelessChannelCtl->bWaitCUSDResponse)
                     && */((Utl_StrFndNL(DataBuf, (_CHAR8*)"+CUSD: 2,", 0, uiTemp, uiTemp) != -1) 
                        || (Utl_StrFndNL(DataBuf,(_CHAR8*) "+CUSD: 1,", 0, uiTemp, uiTemp) != -1)
                        || (Utl_StrFndNL(DataBuf,(_CHAR8*) "+CUSD: 0,", 0, uiTemp, uiTemp) != -1)
                        || (Utl_StrFndNL(DataBuf,(_CHAR8*) "+CUSD: 5,", 0, uiTemp, uiTemp) != -1)))
                {
                    bRtnValue = wirelessIsUssdRspComplete(ChannelID, i, uiTempHead, DataBuf, &RtnLen);
                    if(bRtnValue)
                    {
                        //pWirelessChannelCtl->uiExtComHead = uiTempHead;
                        return RtnLen;
                    }
                    else
                    {
                        continue;
                    }
                }                
            }

            DataBuf[i] = 0;
            uiTempHead &= (WIRELESS_EXTCOMBUF_MAXSIZE - 1);
            //pWirelessChannelCtl->uiExtComHead = uiTempHead;
            
            return i;
        }
    }

    if((pWirelessChannelCtl->bWaitATDResponse) && (pWirelessChannelCtl->bATDSendFakeResponse))
    {
        /*if((strstr(DataBuf, "ATD") != NULL) && (strstr(DataBuf, ";\r\n") != NULL))*/
        {
            //pWirelessChannelCtl->uiExtComHead = uiTempHead & (WIRELESS_EXTCOMBUF_MAXSIZE - 1);
            strcpy(&DataBuf[i], (_CHAR8 *)"OK\r\n");
            pWirelessChannelCtl->bATDSendFakeResponse = EOS_FALSE;
            WIRELESS_INFO("ATD Fake Response:%s\r\n", DataBuf);
            return (i + sizeof("OK\r\n") - 1);
        }
    }

    DataBuf[0] = 0;
    return 0;
}

_VOID ReceiveThread(_VOID)
{
    T_HCHNL* pChnl = NULL;

    _UCHAR8    IsUrc = 0;
    _VOID* UrcParse  = NULL;
    
    RIL_URC_TYPE UrcEventType;
    RIL_URC_ID   UrcEventID;
    _UINT32 strNum = 0;
    _UINT32 strSize = 0;

    _UINT32    uiChannelid = 0;
    _UCHAR8 ucLoopStart = 0;
    _UCHAR8 ucLoopEnd = 0;
    
    _UINT32 uiDataSize = 0;

    PWIRELESSROOTST pWirelessRootSt;
    PWIRELESSCHANNELST  pWirelessChannelCtl;
    _UCHAR8 TempBuffer[ATLineBuffSize];
    _UINT32 uiTempSize = 0;

    pWirelessRootSt = (PWIRELESSROOTST)&stWirelessRootSt;
    ucLoopStart = 0;
    ucLoopEnd = pWirelessRootSt->uiMaxModuleNum;

    for(uiChannelid = ucLoopStart; uiChannelid < ucLoopEnd; uiChannelid++)
    {
        pWirelessChannelCtl = &pWirelessRootSt->stChannelSt[uiChannelid];
        pChnl = pWirelessChannelCtl->pChnl;
 
        if (pChnl->ATLineLen > 0)
        {
            uiDataSize = pChnl->ATLineLen;
            pChnl->ATLineLen = 0;
        }
        else
        {
            memset(pChnl->ATLineBuffer, 0, ATLineBuffSize);
            uiDataSize = WIRELESS_ReadCompleteInfo(uiChannelid, pChnl->ATLineBuffer, ATLineBuffSize);
            if(0 == uiDataSize)
            {
                continue;
            }
        }

        if((pChnl->ATLineBuffer[0] == '\r') && (pChnl->ATLineBuffer[1] == '\n') && (uiDataSize == 2))
        {
            if(1 == uiChannelid)
            {                
                uiDataSize = WIRELESS_ReadCompleteInfo(uiChannelid, pChnl->ATLineBuffer, ATLineBuffSize);
                if(0 == uiDataSize)
                {
                    continue;
                }
                if((pChnl->ATLineBuffer[0] == '\r') && (pChnl->ATLineBuffer[1] == '\n') && (uiDataSize == 2))
                {
                    continue;
                }

                if((pWirelessChannelCtl->bSimDataIgnoreOk == EOS_TRUE) && (pChnl->ATLineBuffer[0] == 'O') && (pChnl->ATLineBuffer[1] == 'K') 
                    && (pChnl->ATLineBuffer[2] == '\r') && (pChnl->ATLineBuffer[3] == '\n') && (uiDataSize == 4))
                {                    
                    pWirelessChannelCtl->bSimDataIgnoreOk = EOS_FALSE;
                    
                    uiDataSize = WIRELESS_ReadCompleteInfo(uiChannelid, pChnl->ATLineBuffer, ATLineBuffSize);
                    if(0 == uiDataSize)
                    {
                        continue;
                    }
                    if((pChnl->ATLineBuffer[0] == '\r') && (pChnl->ATLineBuffer[1] == '\n') && (uiDataSize == 2))
                    {
                        continue;
                    }            
                }
            }
            else
            {
                continue;
            }
        }

        if((1 == uiChannelid) && (Utl_StrFndNL((const _CHAR8*)(pChnl->ATLineBuffer), (_CHAR8*) "AT+SIMDATA=", 0, uiDataSize, uiDataSize) != -1))
        {
            
            pWirelessChannelCtl->bSimDataIgnoreOk = EOS_TRUE;
            
            uiDataSize = WIRELESS_ReadCompleteInfo(uiChannelid, pChnl->ATLineBuffer, ATLineBuffSize);
            if(0 == uiDataSize)
            {
                continue;
            }
            if((pChnl->ATLineBuffer[0] == '\r') && (pChnl->ATLineBuffer[1] == '\n') && (uiDataSize == 2))
            {
                continue;
            }    

            if((pWirelessChannelCtl->bSimDataIgnoreOk == EOS_TRUE) && (pChnl->ATLineBuffer[0] == 'O') && (pChnl->ATLineBuffer[1] == 'K') 
                && (pChnl->ATLineBuffer[2] == '\r') && (pChnl->ATLineBuffer[3] == '\n') && (uiDataSize == 4))
            {                
                pWirelessChannelCtl->bSimDataIgnoreOk = EOS_FALSE;
                uiDataSize = WIRELESS_ReadCompleteInfo(uiChannelid, pChnl->ATLineBuffer, ATLineBuffSize);
                if(0 == uiDataSize)
                {
                    continue;
                }
                if((pChnl->ATLineBuffer[0] == '\r') && (pChnl->ATLineBuffer[1] == '\n') && (uiDataSize == 2))
                {                                        
                    uiDataSize = WIRELESS_ReadCompleteInfo(uiChannelid, pChnl->ATLineBuffer, ATLineBuffSize);
                    if(0 == uiDataSize)
                    {
                        continue;
                    }
                    if((pChnl->ATLineBuffer[0] == '\r') && (pChnl->ATLineBuffer[1] == '\n') && (uiDataSize == 2))
                    {
                        continue;
                    }            
                }    
            }            
        }

        if((1 == uiChannelid) && (pWirelessChannelCtl->bSimDataIgnoreOk == EOS_TRUE) && (pChnl->ATLineBuffer[0] == 'O') && (pChnl->ATLineBuffer[1] == 'K') 
            && (pChnl->ATLineBuffer[2] == '\r') && (pChnl->ATLineBuffer[3] == '\n') && (uiDataSize == 4))
        {
            pWirelessChannelCtl->bSimDataIgnoreOk = EOS_FALSE;
            uiDataSize = WIRELESS_ReadCompleteInfo(uiChannelid, pChnl->ATLineBuffer, ATLineBuffSize);
            if(0 == uiDataSize)
            {
                continue;
            }
            if((pChnl->ATLineBuffer[0] == '\r') && (pChnl->ATLineBuffer[1] == '\n') && (uiDataSize == 2))
            {                
                uiDataSize = WIRELESS_ReadCompleteInfo(uiChannelid, pChnl->ATLineBuffer, ATLineBuffSize);
                if(0 == uiDataSize)
                {
                    continue;
                }
                if((pChnl->ATLineBuffer[0] == '\r') && (pChnl->ATLineBuffer[1] == '\n') && (uiDataSize == 2))
                {                    
                    continue;
                }            
            }            
        }    
#if 0
        if(uiDataSize != 0)
        {
            //weikk 20160304,line=8
            #if 1
            if(pChnl->ATLineBuffer[0] == '+')
            {
                if(strncmp(pChnl->ATLineBuffer, "+SIMDATA:", strlen("+SIMDATA:")) != NULL)
                {
                    WIRELESS_INFO("Channel %d RecvInfo(%d):%s\r\n", uiChannelid, uiDataSize, pChnl->ATLineBuffer);
                }
            }
            else if(strncmp(pChnl->ATLineBuffer, "ERROR", strlen("ERROR")) == NULL)
            {
                WIRELESS_INFO("Channel %d RecvInfo(%d):%s\r\n", uiChannelid, uiDataSize, pChnl->ATLineBuffer);
            }
                
            #else
                WIRELESS_INFO("Channel %d RecvInfo(%d):%s\r\n", uiChannelid, uiDataSize, pChnl->ATLineBuffer);
            #endif
        }
#endif        

        if((1 == uiChannelid) && (Utl_StrFndNL((const _CHAR8*)(pChnl->ATLineBuffer), (_CHAR8*) "+SIMDATA:", 0, uiDataSize, uiDataSize) != -1))
        {
            uiTempSize = WIRELESS_ReadCompleteInfoEx(uiChannelid, TempBuffer, ATLineBuffSize);
            if(0 != uiTempSize)
            {
                if((TempBuffer[0] == '\r') && (TempBuffer[1] == '\n') && (uiTempSize == 2))
                {
                    pWirelessChannelCtl->uiExtComHead = (pWirelessChannelCtl->uiExtComHead + uiTempSize) & (WIRELESS_EXTCOMBUF_MAXSIZE - 1);
                    uiTempSize = WIRELESS_ReadCompleteInfoEx(uiChannelid, TempBuffer, ATLineBuffSize);
                }    
                
                if(0 != uiTempSize)
                {
                    if((pWirelessChannelCtl->bSimDataIgnoreOk == EOS_TRUE) && (TempBuffer[0] == 'O') && (TempBuffer[1] == 'K') 
                        && (TempBuffer[2] == '\r') && (TempBuffer[3] == '\n') && (uiTempSize == 4))
                    {
                        pWirelessChannelCtl->uiExtComHead = (pWirelessChannelCtl->uiExtComHead + uiTempSize) & (WIRELESS_EXTCOMBUF_MAXSIZE - 1);
                        pWirelessChannelCtl->bSimDataIgnoreOk = EOS_FALSE;
                    }
                }
            }
        }

        /*Call URC parse function to check the URC data*/
        if((NULL != pChnl->pURC) && (FindInURCResult((const _CHAR8*)pChnl->ATLineBuffer, uiDataSize, pChnl->pURC)))
        {
            IsUrc = 0;
        }
        else
        {
            IsUrc = WaveC_CheckUrcData(uiChannelid, &UrcEventType, &UrcEventID, &UrcParse, 
                    &strNum, &strSize, (const _CHAR8*)pChnl->ATLineBuffer, uiDataSize);
        }

        switch (IsUrc)
        {
            case 0:
            {
                if((uiDataSize + pChnl->ATRsp.ulData) > pChnl->ATRsp.bufferSize) /*    no enough buffer    */
                {
                    _UCHAR8    *pBuffer;
                    _UINT32    newSize;

                    newSize = pChnl->ATRsp.bufferSize*2;
                    if(newSize < (pChnl->ATRsp.ulData + uiDataSize))
                    {
                        pChnl->ATRsp.ulData = 0;
                        WIRELESS_ERROR(WIRELESS_ERRORNO, "Channel %d Response Data too Large(%d)\r\n", uiChannelid, (pChnl->ATRsp.ulData + uiDataSize));
                        break;
                    }
                    
                    pBuffer = (_UCHAR8*)WIRELESS_MALLOC(newSize);
                    if(NULL == pBuffer)
                    {
                        pChnl->ATRsp.ulData = 0;
                        WIRELESS_ERROR(WIRELESS_ERRORNO, "Channel %d Cann't Malloc Buffer for Response Data\r\n", uiChannelid);
                        break;
                    }
                    
                    memcpy(pBuffer, pChnl->ATRsp.lpData, pChnl->ATRsp.ulData);

                    if(EOS_TRUE == pChnl->ATRsp.bNeedFree)
                    {
                        WIRELESS_FREE(pChnl->ATRsp.lpData);
                    }
                    pChnl->ATRsp.lpData = pBuffer;
                    pChnl->ATRsp.bufferSize = newSize;
                    pChnl->ATRsp.bNeedFree = EOS_TRUE;
                    WIRELESS_ERROR(WIRELESS_ERRORNO, "Channel %d ATRsp Need Larger Buffer\r\n", uiChannelid);                    
                }

                memcpy((pChnl->ATRsp.lpData+pChnl->ATRsp.ulData), pChnl->ATLineBuffer, uiDataSize);

                pChnl->ATRsp.ulData += uiDataSize;
                *(pChnl->ATRsp.lpData+pChnl->ATRsp.ulData) = 0;

                /* response is completed, push it to list    */
                if( ((strstr(pChnl->ATRsp.lpData, "+CIPRXGET:") == NULL) && ((*(pChnl->hWM->pATFunction->P_CheckATResult))((const _CHAR8*)pChnl->ATLineBuffer, uiDataSize)))
                    || ((strstr(pChnl->ATRsp.lpData, "+CIPRXGET:") != NULL) && (WAVEC_CheckATResultEx((const _CHAR8*)pChnl->ATLineBuffer, uiDataSize)))
                    || ((NULL != pChnl->pResult) && FindInURCResult((const _CHAR8*)pChnl->ATLineBuffer, uiDataSize, pChnl->pResult)))
                {

                    if((pChnl->ATRsp.ulData > 6) || (FilterShortLine == EOS_FALSE)) /* ���˵����� OK��URC */
                    {
                        PT_ATRESPONSE  pATRsp = NULL;
                        pATRsp = (PT_ATRESPONSE)WIRELESS_MALLOC(sizeof(T_ATRESPONSE));
                        if(pATRsp == NULL)
                        {
                            WIRELESS_ERROR(WIRELESS_ERRORNO, "Wireless Malloc(%d) Failed\r\n", (sizeof(T_ATRESPONSE)));
                            continue;
                        }

                        memset(pATRsp, 0, (sizeof(T_ATRESPONSE)));
                        memcpy((_UINT32*)pATRsp, (_UINT32*)&(pChnl->ATRsp), sizeof(T_ATRESPONSE));
                        //weikk 20160304 WIRELESS_INFO("Channel %d ResponseData:%s\r\n", uiChannelid, (const _CHAR8*)pChnl->ATLineBuffer);
                        
                        /*WirelessSemP(pWirelessRootSt->RSPLstMutex, EOS_WAIT_FOREVER);*/
                        eos_lst_add(&pChnl->atrsplist, &pATRsp->stnode);
                        /*WirelessSemV(pWirelessRootSt->RSPLstMutex);*/

                        /* 2018.08.06 added by pengjian to reduce the cpu usage */
                        /* SendThread() will be invoked after ReceiveThread(), 
                           so no need wakeup the main task again */
                        /* WIRELESS_WakeUp(); */
                    }
                    
                    if(EOS_TRUE == pChnl->ATRsp.bNeedFree)
                    {
                         WIRELESS_FREE(pChnl->ATRsp.lpData);
                         pChnl->ATRsp.bNeedFree = EOS_FALSE;
                    }              
                    pChnl->ATRsp.lpData = pChnl->ATBuffer;
                    pChnl->ATRsp.bufferSize = ATBufferSize;

                    pChnl->ATRsp.ulData = 0;
                }
                break;    
            }
            case 1:
            {
                /*��������������URC������URC �ص�����*/
                ATServer_URCCallback(uiChannelid, UrcEventType, UrcEventID, 
                                    UrcParse, strSize, strNum);
                break;
            }
            case 2:
            {
                /* WIRELESS_INFO("(%s)(%d) Incompleted URC\r\n", __FILE__, __LINE__); */
                /*�������ǲ�������URC����Ҫ�ٶ�һ�δ���*/
                break;
            }
            case 3:
            {
                /* WIRELESS_INFO("(%s)(%d) URC Wrong Data\r\n", __FILE__, __LINE__); */
                /*�������Ǵ�������ݣ���������*/
                WIRELESS_ERROR(WIRELESS_ERRORNO, "read AT response error\r\n");
                break;
            }
            default:
                break;
        }
    }

    for (uiChannelid = ucLoopStart; uiChannelid < ucLoopEnd; uiChannelid++)
    {
        pWirelessChannelCtl = &pWirelessRootSt->stChannelSt[uiChannelid];
        pChnl = pWirelessChannelCtl->pChnl;
        
        pChnl->ATLineLen = WIRELESS_ReadCompleteInfo(uiChannelid, pChnl->ATLineBuffer, ATLineBuffSize);
        if (pChnl->ATLineLen > 0)
        {
            WIRELESS_WakeUp();
            break;
        }
    }
}/*end of ReceiveThread*/ 

/**
 *@brief �õ�ָ��ͨ��������������ݴ���������
 *@param    pChnl        [in]ͨ�����

 *@return     _INT32
 *@retval    >=0 ���������<0 ����ID
*/
_INT32 GetCmdNumInQueue(HCHNL hChnl)
{
    _VOID*     start_address = NULL;
    _UINT32        Queue_size = 0;
    _UINT32        available = 0;
    _UINT32        used = 0;
    #if 0
    T_OPTION    message_type = 0;
    _UINT32        message_size = 0;
    T_OPTION    suspend_type = 0;
    #endif
    _UINT32        tasks_suspended = 0;
    T_hTask        first_task;
    _INT32         QueueStatus =0 ;

#if 0
    QueueStatus = AK_Queue_Information(hChnl->hATCQ,
            &start_address,    &Queue_size, &available, &used, 
            &message_type, &message_size, &suspend_type, &tasks_suspended, &first_task);

    if (QueueStatus < 0)
    {
        printf("Get Queue Information error, error ID 0x%x", QueueStatus);
        return QueueStatus;
    }
#endif    
    return used;
}

/**
  *@briefͨ������ӿڽ�������AT����͵���������
  *@param    pWM_Handle            [in]ָ������ģ������ָ��
  *@param    Cmd                    [in]����õ�AT����
  *@param    Size                [in]AT��������ݳ���
  *@param    *ChID                [in]�ڸ�ͨ���Ϸ��͸�AT����.
                                �����ֵ��RIL_CHANNEL_ANY�������ͨ���������ж����ĸ�ͨ������,����ʵ��ѡ�е�ͨ����ͨ���ò������ظ�������.
  *@param    Notify                [in]��AT�����ִ�н���Ƿ�֪ͨserver
  *@param    fParse                [in]��AT����ؽ���Ľ�������
  *@param    serviceHandle        [in]���ڽ�AT����ؽ��֪ͨ�ϲ��serviceHandle�����
                                    �ڵ��ص�����ʱ��Ϊ��������
  *@param     serviceType        [in]���ܹ���ģ������
  *@param     customHandle        [in]�ͻ���Ϣ���

  *@param    CmdID                [in]AT�����ID�ţ������ֵ��NEWCMDID����Ҫ���������ˮ��
  *@param    retryTimes            [in]AT����ִ�д���ʱ�����Դ���
  *@param    executeMode            [in]AT����ִ��ģʽ��������T_CMD_EXE_MODE

  *@param    bTail                [in]EOS_TRUE: ������ŵ�AT���е�β����EOS_FALSE: ������ŵ�AT���е�ͷ��
  *@param    timeout                [in]��ʱʱ�䣬��λ�룬��ָ��ʱ����û���յ�AT�ķ��ؽ�����϶�������ʧ��

  *@return    _UINT32                
  *@retval    >0                    ��AT������AT��������е���ˮ��
*/
_UINT32 SendCmdEx(HWM pWM_Handle, T_ECMD_TYPE CmdType, _UCHAR8 *Cmd, _UINT32 Size, _USHORT16 *ChID,
                        _BOOL Notify, PARSERESPONSE fParse, _UINT32 serviceHandle,
                        T_EDEVICEETYPE serviceType, _UINT32 customHandle, 
                        _UINT32 CmdID, _USHORT16 retryTimes, _USHORT16 executeMode,
                        _BOOL bTail, _USHORT16 timeout, _BOOL ignalMode, T_pURCRESULT pURC, T_pURCRESULT pResult)
{
    PT_ATCMD  pATCmd = NULL;
    _VOID *    pATCmdData = NULL;
    _UINT32 count = 0;
    _INT32     MinValue = 1024;
    _INT32    CmdNum = 0;
    _UINT32    BestChID = 0;
    T_HCHNL* pChnl = NULL;
    PWIRELESSROOTST pWirelessRootSt;
    PWIRELESSCHANNELST  pWirelessChannelCtl;

    pWirelessRootSt = (PWIRELESSROOTST)&stWirelessRootSt;
    
    if(!pWM_Handle->bInitOK)
    {
        return RIL_RTN_WM_NOTREADY;
    }

    pWirelessChannelCtl = &pWirelessRootSt->stChannelSt[pWM_Handle->uiChnlId];
    pChnl = pWirelessChannelCtl->pChnl;
    
    pATCmd = (PT_ATCMD)WIRELESS_MALLOC(sizeof(T_ATCMD));
    if(NULL == pATCmd)
    {
        WIRELESS_ERROR(WIRELESS_ERRORNO, "Channel %d get atcmdListMemPool error\r\n", pWM_Handle->uiChnlId);
        return EOS_ERROR;
    }
    memset(pATCmd, 0, sizeof(T_ATCMD));
    
    /*���AT������ṹ*/
    pATCmd->bNotify = Notify;
    pATCmd->serviceHandle = serviceHandle;
    pATCmd->pParseRsp = fParse;
    pATCmd->customHandle = customHandle;
    pATCmd->serviceType = serviceType;
    pATCmd->retryTimes = retryTimes;
    pATCmd->executeMode = executeMode;
    pATCmd->timeout = timeout;
    pATCmd->CmdType = CmdType;
    pATCmd->ulData = Size;
    pATCmd->pURC = pURC;
    pATCmd->pResult = pResult;

    if(NEWCMDID == CmdID)
    {
        pWM_Handle->cmdID++;
        if(pWM_Handle->cmdID == NEWCMDID)
        {
            pWM_Handle->cmdID++;
        }
        pATCmd->CmdID = pWM_Handle->cmdID;
    }
    else
    {
        pATCmd->CmdID = CmdID;
    }

    if(pATCmd->CmdType == CMDTYPE_AT)
    {
        pATCmdData = (_VOID *)WIRELESS_MALLOC(Size+1);
        if(NULL == pATCmdData)
        {
            WIRELESS_ERROR(WIRELESS_ERRORNO, "Wireless Malloc(%d) Failed\r\n", (Size+1));
            return EOS_ERROR;
        }

        memcpy(pATCmdData, Cmd, (Size+1));
        pATCmd->lpData = pATCmdData;

        /* 2017.12.13 pengjian added to trace the AT commands */
        WIRELESS_INFO("add AT command! id = %d\n%s\n", 
            pATCmd->CmdID, getPrintableAtCmd(pATCmd->lpData));
    }
    else
    {
        pATCmd->lpData = NULL;
    }

    if(*ChID != RIL_CHANNEL_ANY)
    {
        /*for (count=0; count<(pWM_Handle->ulChnlNum); count++)*/
        {
#if 1     /* kinson.xiao     */    
            if(1)/*if(pChnl->ChID == ChID[count])*/
#else            
            if(pWM_Handle->Chnl_Array[count]->ChID == *ChID)
#endif            
            {
                if(ignalMode || (ignalMode == EOS_FALSE && pChnl->channelMode == CHANNEL_MODE_AT))
                {
                    if(bTail)
                    {
                        WirelessSemP(pWirelessRootSt->ATCmdLstMutex, EOS_WAIT_FOREVER);
                        eos_lst_add(&pChnl->atcmdlist, &pATCmd->stnode);    
                        WirelessSemV(pWirelessRootSt->ATCmdLstMutex);
                    }
                    else
                    {
                        WirelessSemP(pWirelessRootSt->ATCmdLstMutex, EOS_WAIT_FOREVER);
                        eos_lst_addfront(&pChnl->atcmdlist, &pATCmd->stnode);    
                        WirelessSemV(pWirelessRootSt->ATCmdLstMutex);
                    }
                    return pATCmd->CmdID;
                }
                /*break;*/
            }
        }
    }

#if 0    
    /*��ѯ����ͨ����ʹ���������AT�����������п��ÿռ�����ͨ��*/
    for (count=0; count<(pWM_Handle->ulChnlNum); count++)
    {
        if(ignalMode || (ignalMode == EOS_FALSE && pChnl->channelMode == CHANNEL_MODE_AT))
        {
            CmdNum = GetCmdNumInQueue(pChnl);

            /*�ҳ����п��ÿռ�����ͨ��*/
            if(CmdNum < MinValue)
            {
                MinValue = CmdNum;
                BestChID = count;
            }
        }
    }
#endif

    if(*ChID == RIL_CHANNEL_ANY)
    {
        *ChID = pChnl->ChID;
    }
    
    if(bTail)
    {
        WirelessSemP(pWirelessRootSt->ATCmdLstMutex, EOS_WAIT_FOREVER);
        eos_lst_add(&pChnl->atcmdlist, &pATCmd->stnode);    
        WirelessSemV(pWirelessRootSt->ATCmdLstMutex);
    }
    else
    {
        WirelessSemP(pWirelessRootSt->ATCmdLstMutex, EOS_WAIT_FOREVER);
        eos_lst_addfront(&pChnl->atcmdlist, &pATCmd->stnode);    
        WirelessSemV(pWirelessRootSt->ATCmdLstMutex);
    }

    /* 2018.08.06 added by pengjian to reduce the cpu usage */
    WIRELESS_WakeUp();
    
    return pATCmd->CmdID;
}

/**
  *@briefͨ������ӿڽ�������AT����͵���������
  *@param    pWM_Handle            [in]ָ������ģ������ָ��
  *@param    Cmd                    [in]����õ�AT����
  *@param    Size                [in]AT��������ݳ���
  *@param    *ChID                [in]�ڸ�ͨ���Ϸ��͸�AT����.
                                �����ֵ��RIL_CHANNEL_ANY�������ͨ���������ж����ĸ�ͨ������,����ʵ��ѡ�е�ͨ����ͨ���ò������ظ�������.
  *@param    Notify                [in]��AT�����ִ�н���Ƿ�֪ͨserver
  *@param    fParse                [in]��AT����ؽ���Ľ�������
  *@param    serviceHandle        [in]���ڽ�AT����ؽ��֪ͨ�ϲ��serviceHandle�����
                                    �ڵ��ص�����ʱ��Ϊ��������
  *@param     serviceType        [in]���ܹ���ģ������
  *@param     customHandle        [in]�ͻ���Ϣ���

  *@param    CmdID                [in]AT�����ID�ţ������ֵ��NEWCMDID����Ҫ���������ˮ��
  *@param    retryTimes            [in]AT����ִ�д���ʱ�����Դ���
  *@param    executeMode            [in]AT����ִ��ģʽ��������T_CMD_EXE_MODE

  *@return    _UINT32                
  *@retval    >0                    ��AT������AT��������е���ˮ��
*/
_UINT32 SendCmd(HWM pWM_Handle, _UCHAR8 *Cmd, _UINT32 Size, _USHORT16 *ChID,
                        _BOOL Notify, PARSERESPONSE fParse, _UINT32 serviceHandle,
                        T_EDEVICEETYPE serviceType, _UINT32 customHandle, 
                        _UINT32 CmdID, _USHORT16 retryTimes, _USHORT16 executeMode)
{
    return SendCmdEx(pWM_Handle, CMDTYPE_AT, Cmd, Size, ChID, Notify, fParse, serviceHandle, serviceType, customHandle, 
                     CmdID, retryTimes, executeMode, EOS_TRUE, 10, EOS_FALSE, NULL, NULL);
}

/**
  *@briefͨ������ӿڽ�������AT����͵��������ͷ��
  *@param    pWM_Handle            [in]ָ������ģ������ָ��
  *@param    Cmd                    [in]����õ�AT����
  *@param    Size                [in]AT��������ݳ���
  *@param    *ChID                [in]�ڸ�ͨ���Ϸ��͸�AT����.
                                �����ֵ��anywhere�������ͨ���������ж����ĸ�ͨ������,����ʵ��ѡ�е�ͨ����ͨ���ò������ظ�������.
  *@param    Notify                [in]��AT�����ִ�н���Ƿ�֪ͨserver
  *@param    fParse                [in]��AT����ؽ���Ľ�������
  *@param    serviceHandle        [in]���ڽ�AT����ؽ��֪ͨ�ϲ��serviceHandle�����
                                    �ڵ��ص�����ʱ��Ϊ��������
  *@param     serviceType        [in]���ܹ���ģ������
  *@param     customHandle        [in]�ͻ���Ϣ���

  *@param    CmdID                [in]AT�����ID�ţ������ֵ��NEWCMDID����Ҫ���������ˮ��
  *@param    retryTimes            [in]AT����ִ�д���ʱ�����Դ���
  *@param    executeMode            [in]AT����ִ��ģʽ��������T_CMD_EXE_MODE

  *@return    _UINT32                
  *@retval    >0                    ��AT������AT��������е���ˮ��
*/
_UINT32 SendCmd_ToHead(HWM pWM_Handle, _UCHAR8 *Cmd, _UINT32 Size,  _USHORT16 *ChID, 
                        _BOOL Notify, PARSERESPONSE fParse, _UINT32 serviceHandle,
                        T_EDEVICEETYPE serviceType, _UINT32 customHandle, 
                        _UINT32 CmdID, _USHORT16 retryTimes, _USHORT16 executeMode)
{
    return SendCmdEx(pWM_Handle, CMDTYPE_AT, Cmd, Size, ChID, Notify, fParse, serviceHandle, serviceType, customHandle, 
                     CmdID, retryTimes, executeMode, EOS_FALSE, 10, EOS_FALSE, NULL, NULL);
}

/**
  *@briefͨ������ӿڽ�������AT����͵���������ָ����ʱʱ��
  *@param    pWM_Handle            [in]ָ������ģ������ָ��
  *@param    Cmd                    [in]����õ�AT����
  *@param    Size                [in]AT��������ݳ���
  *@param    *ChID                [in]�ڸ�ͨ���Ϸ��͸�AT����.
                                �����ֵ��RIL_CHANNEL_ANY�������ͨ���������ж����ĸ�ͨ������,����ʵ��ѡ�е�ͨ����ͨ���ò������ظ�������.
  *@param    Notify                [in]��AT�����ִ�н���Ƿ�֪ͨserver
  *@param    fParse                [in]��AT����ؽ���Ľ�������
  *@param    serviceHandle        [in]���ڽ�AT����ؽ��֪ͨ�ϲ��serviceHandle�����
                                    �ڵ��ص�����ʱ��Ϊ��������
  *@param     serviceType        [in]���ܹ���ģ������
  *@param     customHandle        [in]�ͻ���Ϣ���

  *@param    CmdID                [in]AT�����ID�ţ������ֵ��NEWCMDID����Ҫ���������ˮ��
  *@param    retryTimes            [in]AT����ִ�д���ʱ�����Դ���
  *@param    executeMode            [in]AT����ִ��ģʽ��������T_CMD_EXE_MODE

  *@return    _UINT32                
  *@retval    >0                    ��AT������AT��������е���ˮ��
*/
_UINT32 SendCmdWithTimeout(HWM pWM_Handle, _UCHAR8 *Cmd, _UINT32 Size, _USHORT16 *ChID,
                        _BOOL Notify, PARSERESPONSE fParse, _UINT32 serviceHandle,
                        T_EDEVICEETYPE serviceType, _UINT32 customHandle, 
                        _UINT32 CmdID, _USHORT16 retryTimes, _USHORT16 executeMode, _USHORT16 timeout)
{
    return SendCmdEx(pWM_Handle, CMDTYPE_AT, Cmd, Size, ChID, Notify, fParse, serviceHandle, serviceType, customHandle, 
                     CmdID, retryTimes, executeMode, EOS_TRUE, timeout, EOS_FALSE, NULL, NULL);
}

/**
  *@briefͨ������ӿڽ�������AT����͵��������ͷ,ָ����ʱʱ��
  *@param    pWM_Handle            [in]ָ������ģ������ָ��
  *@param    Cmd                    [in]����õ�AT����
  *@param    Size                [in]AT��������ݳ���
  *@param    *ChID                [in]�ڸ�ͨ���Ϸ��͸�AT����.
                                �����ֵ��anywhere�������ͨ���������ж����ĸ�ͨ������,����ʵ��ѡ�е�ͨ����ͨ���ò������ظ�������.
  *@param    Notify                [in]��AT�����ִ�н���Ƿ�֪ͨserver
  *@param    fParse                [in]��AT����ؽ���Ľ�������
  *@param    serviceHandle        [in]���ڽ�AT����ؽ��֪ͨ�ϲ��serviceHandle�����
                                    �ڵ��ص�����ʱ��Ϊ��������
  *@param     serviceType        [in]���ܹ���ģ������
  *@param     customHandle        [in]�ͻ���Ϣ���

  *@param    CmdID                [in]AT�����ID�ţ������ֵ��NEWCMDID����Ҫ���������ˮ��
  *@param    retryTimes            [in]AT����ִ�д���ʱ�����Դ���
  *@param    executeMode            [in]AT����ִ��ģʽ��������T_CMD_EXE_MODE

  *@return    _UINT32                
  *@retval    >0                    ��AT������AT��������е���ˮ��
*/
_UINT32 SendCmd_ToHeadWithTimeout(HWM pWM_Handle, _UCHAR8 *Cmd, _UINT32 Size,  _USHORT16 *ChID, 
                        _BOOL Notify, PARSERESPONSE fParse, _UINT32 serviceHandle,
                        T_EDEVICEETYPE serviceType, _UINT32 customHandle, 
                        _UINT32 CmdID, _USHORT16 retryTimes, _USHORT16 executeMode, _USHORT16 timeout)
{
    return SendCmdEx(pWM_Handle, CMDTYPE_AT, Cmd, Size, ChID, Notify, fParse, serviceHandle, serviceType, customHandle, 
                     CmdID, retryTimes, executeMode, EOS_FALSE, timeout, EOS_FALSE, NULL, NULL);
}

/**
 @brief:ͨ������ӿ���ģ�鷢��Ӳ��������
  *@param    pWM_Handle            [in]ָ������ģ������ָ��
  *@param    CmdType                [in]hardware command type
  *@param    Notify                [in]��AT�����ִ�н���Ƿ�֪ͨserver
  *@param    serviceHandle        [in]���ڽ�AT����ؽ��֪ͨ�ϲ��serviceHandle�����
                                    �ڵ��ص�����ʱ��Ϊ��������
  *@param     serviceType        [in]���ܹ���ģ������
  *@param     customHandle        [in]�ͻ���Ϣ���

  *@param    CmdID                [in]AT�����ID�ţ������ֵ��NEWCMDID����Ҫ���������ˮ��
  *@param    retryTimes            [in]AT����ִ�д���ʱ�����Դ���
  *@param    executeMode            [in]AT����ִ��ģʽ��������T_CMD_EXE_MODE

  *@return    _UINT32                
  *@retval    >0                    ��AT������AT��������е���ˮ��
*/
_UINT32 SendHardwareCmd(HWM pWM_Handle, T_ECMD_TYPE CmdType, _USHORT16 *ChID,
                        _BOOL Notify, _UINT32 serviceHandle,
                        T_EDEVICEETYPE serviceType, _UINT32 customHandle, 
                        _UINT32 CmdID, _USHORT16 retryTimes, _USHORT16 executeMode)
{
    return SendCmdEx(pWM_Handle, CmdType, NULL, 0, ChID, Notify, NULL, serviceHandle, serviceType, customHandle, 
                     CmdID, retryTimes, executeMode, EOS_TRUE, 5, EOS_TRUE, NULL, NULL);
}

/**
  *@briefͨ������ӿڽ�������AT����͵���������ָ��URC,result,�ȸ�����Ϣ
  *@param    pWM_Handle            [in]ָ������ģ������ָ��
  *@param    Cmd                    [in]����õ�AT����
  *@param    Size                [in]AT��������ݳ���
  *@param    *ChID                [in]�ڸ�ͨ���Ϸ��͸�AT����.
                                �����ֵ��RIL_CHANNEL_ANY�������ͨ���������ж����ĸ�ͨ������,����ʵ��ѡ�е�ͨ����ͨ���ò������ظ�������.
  *@param    Notify                [in]��AT�����ִ�н���Ƿ�֪ͨserver
  *@param    fParse                [in]��AT����ؽ���Ľ�������
  *@param    serviceHandle        [in]���ڽ�AT����ؽ��֪ͨ�ϲ��serviceHandle�����
                                    �ڵ��ص�����ʱ��Ϊ��������
  *@param     serviceType        [in]���ܹ���ģ������
  *@param     customHandle        [in]�ͻ���Ϣ���

  *@param    CmdID                [in]AT�����ID�ţ������ֵ��NEWCMDID����Ҫ���������ˮ��
  *@param    retryTimes            [in]AT����ִ�д���ʱ�����Դ���
  *@param    executeMode            [in]AT����ִ��ģʽ��������T_CMD_EXE_MODE

  *@return    _UINT32                
  *@retval    >0                    ��AT������AT��������е���ˮ��
*/
_UINT32 SendCmdWithAddiInfo(HWM pWM_Handle, _UCHAR8 *Cmd, _UINT32 Size, _USHORT16 *ChID,
                        _BOOL Notify, PARSERESPONSE fParse, _UINT32 serviceHandle,
                        T_EDEVICEETYPE serviceType, _UINT32 customHandle, 
                        _UINT32 CmdID, _USHORT16 retryTimes, _USHORT16 executeMode, _USHORT16 timeout, 
                        _BOOL ignalMode, T_pURCRESULT pURC, T_pURCRESULT pResult)
{
    return SendCmdEx(pWM_Handle, CMDTYPE_AT, Cmd, Size, ChID, Notify, fParse, serviceHandle, serviceType, customHandle, 
                     CmdID, retryTimes, executeMode, EOS_TRUE, timeout, ignalMode, pURC, pResult);
}

/**
  *@briefͨ������ӿڽ�������AT����͵��������ͷ����ָ��URC,result,�ȸ�����Ϣ
  *@param    pWM_Handle            [in]ָ������ģ������ָ��
  *@param    Cmd                    [in]����õ�AT����
  *@param    Size                [in]AT��������ݳ���
  *@param    *ChID                [in]�ڸ�ͨ���Ϸ��͸�AT����.
                                �����ֵ��RIL_CHANNEL_ANY�������ͨ���������ж����ĸ�ͨ������,����ʵ��ѡ�е�ͨ����ͨ���ò������ظ�������.
  *@param    Notify                [in]��AT�����ִ�н���Ƿ�֪ͨserver
  *@param    fParse                [in]��AT����ؽ���Ľ�������
  *@param    serviceHandle        [in]���ڽ�AT����ؽ��֪ͨ�ϲ��serviceHandle�����
                                    �ڵ��ص�����ʱ��Ϊ��������
  *@param     serviceType        [in]���ܹ���ģ������
  *@param     customHandle        [in]�ͻ���Ϣ���

  *@param    CmdID                [in]AT�����ID�ţ������ֵ��NEWCMDID����Ҫ���������ˮ��
  *@param    retryTimes            [in]AT����ִ�д���ʱ�����Դ���
  *@param    executeMode            [in]AT����ִ��ģʽ��������T_CMD_EXE_MODE

  *@return    _UINT32                
  *@retval    >0                    ��AT������AT��������е���ˮ��
*/
_UINT32 SendCmdToHeadWithAddiInfo(HWM pWM_Handle, _UCHAR8 *Cmd, _UINT32 Size, _USHORT16 *ChID,
                        _BOOL Notify, PARSERESPONSE fParse, _UINT32 serviceHandle,
                        T_EDEVICEETYPE serviceType, _UINT32 customHandle, 
                        _UINT32 CmdID, _USHORT16 retryTimes, _USHORT16 executeMode, _USHORT16 timeout, 
                        _BOOL ignalMode, T_pURCRESULT pURC, T_pURCRESULT pResult)
{
    return SendCmdEx(pWM_Handle, CMDTYPE_AT, Cmd, Size, ChID, Notify, fParse, serviceHandle, serviceType, customHandle, 
                     CmdID, retryTimes, executeMode, EOS_FALSE, timeout, ignalMode, pURC, pResult);
}

/**
 *@brief �趨ͨ����ģʽ�����ͨ����ΪPPPģʽ�󣬽����ٽ���AT���ֱ��������ΪAT����ģʽ
 *@param [in] HWM pWM_Handle ģ����
 *@param [in] _USHORT16 ChID ͨ��ID
 *@param [in] T_CHANNEL_MODE mode ͨ��ģʽ
 *@return     _VOID
*/
_VOID SetChannelMode(HWM pWM_Handle, _USHORT16 ChID, T_CHANNEL_MODE mode)
{
    _UINT32 count;
    PWIRELESSROOTST     pWirelessRootSt;
    PWIRELESSCHANNELST  pWirelessChannelCtl;
    T_HCHNL *pChnl;

    pWirelessRootSt = (PWIRELESSROOTST)&stWirelessRootSt;
    pWirelessChannelCtl = &pWirelessRootSt->stChannelSt[pWM_Handle->uiChnlId];
    pChnl = pWirelessChannelCtl->pChnl;
    
    for (count = 0; count < (pWM_Handle->ulChnlNum); count++)
    {
        if(pChnl->ChID == ChID)
        {
            pChnl->channelMode = mode;
            break;
        }
    }
}

