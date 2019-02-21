
/**
* @brief check the data is URC data or not
* if detect unsolicited data(URC data), return 1
* @param _UCHAR8 chID
* @param T_pDATA Data: data pointer.
* @param RIL_URC_TYPE *UrcEventType:Urc sort type eg:call,sms,and stk and so on.
* @param RIL_URC_ID *UrcEventID:Urc sort ID.
* @param _VOID *UrcParse:data parse result
* @param _UINT32 * strNum:num of output data structure 
* @param _UINT32 * strSize:size of output data structure 
* @param const _CHAR8* Data: received com data
* @param _USHORT16 length:length of received com data
* @return _UCHAR8  0: received data is not urc;
1: detect unsolicited data,return type and data structure;
2: only detect urc,not return type and data structure;
3: only detect urc,not return type and data structure
*/
#include "eos.h"
#include "WAVEC_at_pack.h"
#include "utils.h"
#include "utl_bin.h"

#include "wireless_string.h"
#include "../wireless_common.h"
#include "../wireless_debug.h"
#include "../wireless_interface_inc.h"
#include "../wireless_app.h"

extern WIRELESSROOTST    stWirelessRootSt;

static _CHAR8*  s_pSTKURCbuffer[ACOM_PORT_NUM] = {NULL};
static _UINT32   s_bufferSize[ACOM_PORT_NUM] = {0};
static _UINT32   s_usedSize[ACOM_PORT_NUM] = {0};

static _CHAR8 gcWirelessDtmfTable[] = 
            {'0', '1', '2', '3', '4', '5', 
             '6', '7', '8', '9', '*', '#'};

_UCHAR8  WaveC_CheckUrcData(_UINT32 ChnId, RIL_URC_TYPE *UrcEventType, RIL_URC_ID *UrcEventID, _VOID **UrcParse, 
                       _UINT32 *strNum, _UINT32 *strSize, const _CHAR8* Data, _USHORT16 length)
{    
    PWIRELESSROOTST     pWirelessRootSt;
    PWIRELESSCHANNELST  pWirelessChannelCtl;
    T_ATCMD*        pChnlATCmd = NULL;
    T_HCHNL*        pChnl = NULL;
    _SHORT16        iBegin = 0;
    _SHORT16        iPost = 0;
    _SHORT16        iEnd = 0;
    _CHAR8          strTemp[8] = {0};
    _CHAR8          strTemp1[8] = {0};
    _UINT32         uiDTMFValue = 0;
    _CHAR8          acVal[2] = {0,0};
    
    UtRtsDefine(rts);

    pWirelessRootSt = (PWIRELESSROOTST)&stWirelessRootSt;
    pWirelessChannelCtl = &pWirelessRootSt->stChannelSt[ChnId];
    pChnl = pWirelessChannelCtl->pChnl;
    pChnlATCmd = &(pWirelessChannelCtl->stATCmd);
    
    /* process phone call*/
    {
        if(1 == pWirelessRootSt->senddata.bQiOpenStart)
        {
            if((iBegin = Utl_StrFndL(Data, "CONNECT OK", 0, length)) != -1)
            {
                pWirelessRootSt->senddata.bQiOpenStart = 0;
                pWirelessRootSt->senddata.enConnectState = CONNECT_OK;
                printf("senddata.enConnectState = CONNECT_OK\n");
                WIRELESS_INFO("senddata.enConnectState = CONNECT_OK\n");
                return 1;
            }
            else if((iBegin = Utl_StrFndL(Data, "CONNECT FAIL", 0, length)) != -1)
            {
                pWirelessRootSt->senddata.bQiOpenStart = 0;
                pWirelessRootSt->senddata.enConnectState = CONNECT_FAIL;
                printf("senddata.enConnectState = CONNECT_FAIL\n");
                WIRELESS_INFO("senddata.enConnectState = CONNECT_FAIL\n");
                return 1;
            }
            else if((iBegin = Utl_StrFndL(Data, "ALREAY CONNECT", 0, length)) != -1)
            {
                pWirelessRootSt->senddata.bQiOpenStart = 0;
                pWirelessRootSt->senddata.enConnectState = ALREAY_CONNECT;
                printf("senddata.enConnectState = ALREAY_CONNECT\n");
                WIRELESS_INFO("senddata.enConnectState = ALREAY_CONNECT\n");
                return 1;
            }
            else if((iBegin = Utl_StrFndL(Data, "TCP CONNECTING", 0, length)) != -1)
            {
                pWirelessRootSt->senddata.bQiOpenStart = 0;
                pWirelessRootSt->senddata.enConnectState = TCP_CONNECTING;
                printf("senddata.enConnectState = TCP_CONNECTING\n");
                WIRELESS_INFO("senddata.enConnectState = TCP_CONNECTING\n");
                return 1;
            }
            else if((iBegin = Utl_StrFndL(Data, "PDP DEACT", 0, length)) != -1)
            {
                pWirelessRootSt->senddata.bQiOpenStart = 0;
                pWirelessRootSt->senddata.enConnectState = PDP_DEACT;
                printf("senddata.enConnectState = PDP_DEACT\n");
                WIRELESS_INFO("senddata.enConnectState = PDP_DEACT\n");
                return 1;
            }
            else if((iBegin = Utl_StrFndL(Data, "IP INITIAL", 0, length)) != -1)
            {
                pWirelessRootSt->senddata.bQiOpenStart = 0;
                pWirelessRootSt->senddata.enConnectState = IP_INITIAL;
                printf("senddata.enConnectState = IP_INITIAL\n");
                WIRELESS_INFO("senddata.enConnectState = IP_INITIAL\n");
                return 1;
            }
            else if((iBegin = Utl_StrFndL(Data, "IP START", 0, length)) != -1)
            {
                pWirelessRootSt->senddata.bQiOpenStart = 0;
                pWirelessRootSt->senddata.enConnectState = IP_START;
                printf("senddata.enConnectState = IP_START\n");
                WIRELESS_INFO("senddata.enConnectState = IP_START\n");
                return 1;
            }
            else if((iBegin = Utl_StrFndL(Data, "IP CONFIG", 0, length)) != -1)
            {
                pWirelessRootSt->senddata.bQiOpenStart = 0;
                pWirelessRootSt->senddata.enConnectState = IP_CONFIG;
                printf("senddata.enConnectState = IP_CONFIG\n");
                WIRELESS_INFO("senddata.enConnectState = IP_CONFIG\n");
                return 1;
            }
            else if((iBegin = Utl_StrFndL(Data, "IP IND", 0, length)) != -1)
            {
                pWirelessRootSt->senddata.bQiOpenStart = 0;
                pWirelessRootSt->senddata.enConnectState = IP_IND;
                printf("senddata.enConnectState = IP_IND\n");
                WIRELESS_INFO("senddata.enConnectState = IP_IND\n");
                return 1;
            }
            else if((iBegin = Utl_StrFndL(Data, "IP GPRSACT", 0, length)) != -1)
            {
                pWirelessRootSt->senddata.bQiOpenStart = 0;
                pWirelessRootSt->senddata.enConnectState = IP_GPRSACT;
                printf("senddata.enConnectState = IP_GPRSACT\n");
                WIRELESS_INFO("senddata.enConnectState = IP_GPRSACT\n");
                return 1;
            }
            else if((iBegin = Utl_StrFndL(Data, "IP STATUS", 0, length)) != -1)
            {
                pWirelessRootSt->senddata.bQiOpenStart = 0;
                pWirelessRootSt->senddata.enConnectState = IP_STATUS;
                printf("senddata.enConnectState = IP_STATUS\n");
                WIRELESS_INFO("senddata.enConnectState = IP_STATUS\n");
                return 1;
            }
            else if((iBegin = Utl_StrFndL(Data, "IP CLOSE", 0, length)) != -1)
            {
                pWirelessRootSt->senddata.bQiOpenStart = 0;
                pWirelessRootSt->senddata.enConnectState = IP_CLOSE;
                printf("senddata.enConnectState = IP_CLOSE\n");
                WIRELESS_INFO("senddata.enConnectState = IP_CLOSE\n");
                return 1;
            }
        }
        else if((iBegin = Utl_StrFndL(Data, "+QIRDI:", 0, length)) != -1)
        {
            pWirelessRootSt->senddata.enSendState = SEND_OK;
            printf("senddata.enSendState = SEND_OK\n");
            WIRELESS_INFO("senddata.enSendState = SEND_OK\n");
            if(1 == pWirelessRootSt->senddata.bSemstate)
            {
                pWirelessRootSt->senddata.bSemstate = 0;
                WIRELESS_INFO("senddata.bSemstate = 0\n");
                semGive(pWirelessRootSt->semATSendData);
            }
            return 1;
        }
        else if(Utl_StrFndL(Data, "MO CONNECTED", 0, length) != -1)
        {
            *UrcEventID = RIL_NOTIFY_CONNECT;
            *UrcEventType = WM_TYPE_CALL;
            *strNum = 0;
            *strSize = 0;
            *UrcParse = NULL;
            pWirelessChannelCtl->bIsOffHook = EOS_TRUE;

            return 1;
        }
        else if(Utl_StrFndL(Data, "MO RING", 0, length) != -1)
        {
            *UrcEventID = RIL_NOTIFY_CALLSTATE_ALERTING;
            *UrcEventType = WM_TYPE_CALL;
            *strNum = 0;
            *strSize = 0;
            *UrcParse = NULL;

            return 1;
        }        
        else if(Utl_StrFndL(Data, "BUSY", 0, length) != -1)
        {
            *UrcEventID = RIL_NOTIFY_BUSY;
            *UrcEventType = WM_TYPE_CALL;
            *strNum = 0;
            *strSize = 0;
            *UrcParse = NULL;

            if ((NVM_GOIP_MODTYPE_U10 == pWirelessChannelCtl->ModuleType) 
                || (NVM_GOIP_MODTYPE_UC15 == pWirelessChannelCtl->ModuleType) 
                ||(NVM_GOIP_MODTYPE_UC20 == pWirelessChannelCtl->ModuleType))
            {
                pWirelessChannelCtl->bIsOffHook = EOS_FALSE;
                pWirelessChannelCtl->uiOnHookStartTick = tickGet();
                
            }
            return 1;
        }
        else if(Utl_StrFndL(Data, "NO ANSWER", 0, length) != -1)                         
        {
            *UrcEventID =  RIL_NOTIFY_NOANSWER;
            *UrcEventType = WM_TYPE_CALL;
            *strNum = 0;
            *strSize = 0;
            *UrcParse = NULL;
            
            if ((NVM_GOIP_MODTYPE_U10 == pWirelessChannelCtl->ModuleType) 
                || (NVM_GOIP_MODTYPE_UC15 == pWirelessChannelCtl->ModuleType)
                || (NVM_GOIP_MODTYPE_UC20 == pWirelessChannelCtl->ModuleType))
            {
                pWirelessChannelCtl->bIsOffHook = EOS_FALSE;
                pWirelessChannelCtl->uiOnHookStartTick = tickGet();
                
            }
            return 1;
        }
        else if(Utl_StrFndL(Data, "NO CARRIER", 0, length) != -1)                            
        {
            WIRELESS_INFO("Channel %d Detect NO CARRIER\r\n", ChnId);
            *UrcEventID =  RIL_NOTIFY_DISCONNECT;
            *UrcEventType = WM_TYPE_CALL;
            *strNum = 0;
            *strSize = 0;
            *UrcParse = NULL;

            if ((NVM_GOIP_MODTYPE_U10 == pWirelessChannelCtl->ModuleType) 
                || (NVM_GOIP_MODTYPE_UC15 == pWirelessChannelCtl->ModuleType)
                || (NVM_GOIP_MODTYPE_UC20 == pWirelessChannelCtl->ModuleType))
            {
                pWirelessChannelCtl->bIsOffHook = EOS_FALSE;
                pWirelessChannelCtl->uiOnHookStartTick = tickGet();
                
            }
            return 1;
        }
        else if(Utl_StrFndL(Data, "NO DIALTONE", 0, length) != -1)                            
        {
            *UrcEventID =  RIL_NOTIFY_NODIALTONE;
            *UrcEventType = WM_TYPE_CALL;
            *strNum = 0;
            *strSize = 0;
            *UrcParse = NULL;

            return 1;
        }        
        else if((Utl_StrFndL(Data, "+QLJDR:1", 0, length) != -1) || (Utl_StrFndL(Data, "+QLJDR: 1", 0, length) != -1))                        
        {
            WIRELESS_INFO("Channel %d Detect Jamming\r\n", ChnId);
            pWirelessChannelCtl->uiModJammingCnt++;

            return 0;
        }
        else if(Utl_StrFndL(Data, "UNDER_VOLTAGE POWER DOWN", 0, length) != -1)
        {
            pWirelessChannelCtl->uiUnderVoltagePwroffCnt++;
            WIRELESS_ERROR(WIRELESS_ERRORNO, "Channel %d Detect UNDER_VOLTAGE POWER DOWN\r\n", ChnId);
            printf("Channel %d Detect UNDER_VOLTAGE POWER DOWN\r\n", ChnId);
            return 0;
        }        
        else if(Utl_StrFndL(Data, "UNDER_VOLTAGE WARNING", 0, length) != -1)
        {
            pWirelessChannelCtl->uiUnderVoltageWarningCnt++;
            WIRELESS_ERROR(WIRELESS_ERRORNO, "Channel %d Detect UNDER_VOLTAGE WARNING\r\n", ChnId);
            printf("Channel %d Detect UNDER_VOLTAGE WARNING\r\n", ChnId);
            return 0;
        }        
        else if(Utl_StrFndL(Data, "OVER_VOLTAGE POWER DOWN", 0, length) != -1)
        {
            pWirelessChannelCtl->uiOverVoltagePwroffCnt++;
            WIRELESS_ERROR(WIRELESS_ERRORNO, "Channel %d Detect OVER_VOLTAGE POWER DOWN\r\n", ChnId);
            printf("Channel %d Detect OVER_VOLTAGE POWER DOWN\r\n", ChnId);
            return 0;
        }
        else if(Utl_StrFndL(Data, "OVER_VOLTAGE WARNING", 0, length) != -1)
        {
            pWirelessChannelCtl->uiOverVoltageWarningCnt++;
            WIRELESS_ERROR(WIRELESS_ERRORNO, "Channel %d Detect OVER_VOLTAGE WARNING\r\n", ChnId);
            printf("Channel %d Detect OVER_VOLTAGE WARNING\r\n", ChnId);
            return 0;
        }   
        else if(((NVM_GOIP_MODTYPE_UC15 == pWirelessChannelCtl->ModuleType) || (NVM_GOIP_MODTYPE_UC20 == pWirelessChannelCtl->ModuleType)) && (Utl_StrFndL(Data, "+QIND: \"vbatt\",-2", 0, length) != -1))
        {
            pWirelessChannelCtl->uiUnderVoltagePwroffCnt++;
            WIRELESS_ERROR(WIRELESS_ERRORNO, "Channel %d Detect UNDER_VOLTAGE POWER DOWN\r\n", ChnId);
            printf("Channel %d Detect UNDER_VOLTAGE POWER DOWN\r\n", ChnId);
            return 0;        
        }
        else if(((NVM_GOIP_MODTYPE_UC15 == pWirelessChannelCtl->ModuleType) || (NVM_GOIP_MODTYPE_UC20 == pWirelessChannelCtl->ModuleType)) && (Utl_StrFndL(Data, "+QIND: \"vbatt\",-1", 0, length) != -1))
        {
            pWirelessChannelCtl->uiUnderVoltageWarningCnt++;
            WIRELESS_ERROR(WIRELESS_ERRORNO, "Channel %d Detect UNDER_VOLTAGE WARNING\r\n", ChnId);
            printf("Channel %d Detect UNDER_VOLTAGE WARNING\r\n", ChnId);
            return 0;
        }
        else if(((NVM_GOIP_MODTYPE_UC15 == pWirelessChannelCtl->ModuleType) || (NVM_GOIP_MODTYPE_UC20 == pWirelessChannelCtl->ModuleType)) && (Utl_StrFndL(Data, "+QIND: \"vbatt\",1", 0, length) != -1))
        {
            pWirelessChannelCtl->uiOverVoltageWarningCnt++;
            WIRELESS_ERROR(WIRELESS_ERRORNO, "Channel %d Detect OVER_VOLTAGE WARNING\r\n", ChnId);
            printf("Channel %d Detect OVER_VOLTAGE WARNING\r\n", ChnId);
            return 0;
        }  
        else if(((NVM_GOIP_MODTYPE_UC15 == pWirelessChannelCtl->ModuleType) || (NVM_GOIP_MODTYPE_UC20 == pWirelessChannelCtl->ModuleType)) && (Utl_StrFndL(Data, "+QIND: \"vbatt\",2", 0, length) != -1))
       {  
               pWirelessChannelCtl->uiOverVoltagePwroffCnt++;
            WIRELESS_ERROR(WIRELESS_ERRORNO, "Channel %d Detect OVER_VOLTAGE POWER DOWN\r\n", ChnId);
            printf("Channel %d Detect OVER_VOLTAGE POWER DOWN\r\n", ChnId);
            return 0;
        }
        else if((Utl_StrFndL(Data, "+CPIN:NOT INSERTED", 0, length) != -1) || (Utl_StrFndL(Data, "+CPIN: NOT INSERTED", 0, length) != -1))                        
        {
            if(EOS_TRUE == pWirelessChannelCtl->bReadyReport)
            {
                WIRELESS_INFO("Channel %d report CCM unready, CPIN NOT INSERTED\r\n", ChnId);
                //WIRELESS_CardReadyReport(ChnId, CCM_WMCTL_CARDSTATUS_NEW, EOS_FALSE);
                WRL_FSM_SendEvent(ChnId, 0,EN_WRL_FSM_EVENT_ID_REGUNREADY, 0);
                
                pWirelessChannelCtl->bReadyReport = EOS_FALSE;
                WIRELESS_ERROR(WIRELESS_ERRORNO, "Channel %d Detect CPIN NOT INSERTED\r\n", ChnId);
            }
            return 0;
        }        
#if 0
        else if((Utl_StrFndL(Data, "+CREG:0", 0, length) != -1) || (Utl_StrFndL(Data, "+CREG: 0", 0, length) != -1))                        
        {
            WIRELESS_CardReadyReport(ChnId, CCM_WMCTL_CARDSTATUS_NEW, EOS_FALSE);
            pWirelessChannelCtl->bReadyReport = EOS_FALSE;
            WIRELESS_ERROR(WIRELESS_ERRORNO, "Channel %d Detect CREG 0\r\n", ChnId);
        }        
#endif
        else if((iBegin = Utl_StrFndL(Data, "+QTONEDET:", 0, length)) != -1)                            
        {
            if(pWirelessChannelCtl->bIsOffHook != EOS_TRUE)
            {
                return 0;
            }
            
            iBegin = iBegin + strlen("+QTONEDET:");
            if (iBegin != -1)
            {
                iEnd = Utl_StrFndL(Data, "\r", iBegin, length);
                if (iEnd != -1)
                {
                    Utl_StrMidL(strTemp, Data, (_SHORT16)(iBegin + 1), (_SHORT16)(iEnd - 1), length);
                    if(Utl_StrFndL(Data, "42", iBegin, length) != -1) /* "*" */
                    {
                        WIRELESS_INFO("Channel %d GSM DTMF Detect = %c\r\n", ChnId, gcWirelessDtmfTable[10]);
                        printf("Channel %d GSM DTMF Detect = %c\r\n", ChnId, gcWirelessDtmfTable[10]);
                     }
                    else if(Utl_StrFndL(Data, "35", iBegin, length) != -1) /* "#" */
                    {
                        WIRELESS_INFO("Channel %d GSM DTMF Detect = %c\r\n", ChnId, gcWirelessDtmfTable[11]);
                        printf("Channel %d GSM DTMF Detect = %c\r\n", ChnId, gcWirelessDtmfTable[11]);
                     }
                    else
                    {
                        uiDTMFValue = atoi(strTemp);
                        if((uiDTMFValue - 0x30) < 10)
                        {
                            WIRELESS_INFO("Channel %d GSM DTMF Detect = %c\r\n", ChnId, gcWirelessDtmfTable[uiDTMFValue - 0x30]);                    
                            printf("Channel %d GSM DTMF Detect = %c\r\n", ChnId, gcWirelessDtmfTable[uiDTMFValue - 0x30]);                    
                         }
                    }
                }
            }
            return 0;
        }
        else if(Utl_StrFndL(Data, "Call Ready", 0, length) != -1)
        {
            if((pWirelessChannelCtl->OperatorID == 41007)
                && (pWirelessChannelCtl->bReadyReport == EOS_TRUE) 
                && ((pWirelessChannelCtl->NetRegStatus == RIL_REGISTHOME) || (pWirelessChannelCtl->NetRegStatus == RIL_REGISROAM)))
            {
                //WIRELESS_CardReadyReport(ChnId, CCM_WMCTL_CARDSTATUS_NEW, EOS_FALSE);
                WRL_FSM_SendEvent(ChnId, 0,EN_WRL_FSM_EVENT_ID_REGUNREADY, 0);
                pWirelessChannelCtl->bReadyReport = EOS_FALSE;
                WIRELESS_ChannelInit(ChnId);
                
                pWirelessChannelCtl->enState = EN_WIRELESS_ST_MODULE_INIT;
                WIRELESS_SetRegStatus(ChnId, RIL_NOTREGSEARCH);
                
                WIRELESS_ERROR(WIRELESS_ERRORNO, "Channel %d Detect Call Ready in the normal status\r\n", ChnId);
            }
            return 0;
        }
        else if(Utl_StrFndL(Data, "+CMGS:", 0, length) != -1)
        {
            WIRELESS_INFO("Channel %d Detect +CMGS:%s\r\n", ChnId, Data);            
            return 0;
        }        

        if((NVM_GOIP_MODTYPE_UC15 == pWirelessChannelCtl->ModuleType) || (NVM_GOIP_MODTYPE_UC20 == pWirelessChannelCtl->ModuleType))
        {
            if((Utl_StrFndL(Data, "+QIND: \"ccinfo\" 1,0,3", 0, length) != -1) || (Utl_StrFndL(Data, "+QIND: \"ccinfo\",1,0,3", 0, length) != -1))
            {
                *UrcEventID = RIL_NOTIFY_CALLSTATE_ALERTING;
                *UrcEventType = WM_TYPE_CALL;
                *strNum = 0;
                *strSize = 0;
                *UrcParse = NULL;
    
                return 1;
                
            }
            else if((Utl_StrFndL(Data, "+QIND: \"ccinfo\" 1,0,0", 0, length) != -1) || (Utl_StrFndL(Data, "+QIND: \"ccinfo\",1,0,0", 0, length) != -1))
            {
                *UrcEventID = RIL_NOTIFY_CONNECT;
                *UrcEventType = WM_TYPE_CALL;
                *strNum = 0;
                *strSize = 0;
                *UrcParse = NULL;
                pWirelessChannelCtl->bIsOffHook = EOS_TRUE;

                return 1;
            }
            else if((Utl_StrFndL(Data, "+QIND: \"ccinfo\" 1,0,-1", 0, length) != -1) || (Utl_StrFndL(Data, "+QIND: \"ccinfo\",1,0,-1", 0, length) != -1))
            {
                *UrcEventID =  RIL_NOTIFY_DISCONNECT;
                *UrcEventType = WM_TYPE_CALL;
                *strNum = 0;
                *strSize = 0;
                *UrcParse = NULL;
                
                pWirelessChannelCtl->bIsOffHook = EOS_FALSE;
                pWirelessChannelCtl->uiOnHookStartTick = tickGet();
            }            
        }
    }
    
    if ((EOS_FALSE == pWirelessRootSt->bCIDEnable) && (Utl_StrFndNL(Data, "RING", 0, 10, length) != -1))        /* Incoming calls */
    {
        *UrcEventID = RIL_NOTIFY_RING;
        *UrcEventType = WM_TYPE_CALL;
        *strNum = 0;
        *strSize = 0;
        *UrcParse = NULL;
        if((NVM_GOIP_MODTYPE_S5320 == pWirelessChannelCtl->ModuleType)
           || (NVM_GOIP_MODTYPE_M10 == pWirelessChannelCtl->ModuleType)
           || (NVM_GOIP_MODTYPE_U10 == pWirelessChannelCtl->ModuleType)
           || (NVM_GOIP_MODTYPE_UC15 == pWirelessChannelCtl->ModuleType)
           || (NVM_GOIP_MODTYPE_UC20 == pWirelessChannelCtl->ModuleType)
           || (NVM_GOIP_MODTYPE_CM300 == pWirelessChannelCtl->ModuleType)
           || (NVM_GOIP_MODTYPE_C202 == pWirelessChannelCtl->ModuleType))
        {
            WIRELESS_INFO("!!! Channel %d Detect Ring\r\n", ChnId);
#if 0   /* Auto Ack Remove to POTS    */            
            if((ASYS_NvmGetInt(NVM_VOICE_FORBID_PSTN_IN) == EOS_FALSE) && (ASYS_NvmGetInt(NVM_VOICE_AUTO_ACK) == EOS_TRUE) && (EOS_FALSE == pWirelessChannelCtl->bIsOffHook))
            {
                wirelessFxoOffHook(ChnId);
                pWirelessChannelCtl->bIsOffHook = EOS_TRUE;
                POTS_Ntfy(ChnId, EN_POTS_EVENT_OFFHOOK, NULL);    
            }
            else
#endif                
            {
                pWirelessChannelCtl->bIsRinging = EOS_TRUE;
               /* POTS_Ntfy(ChnId, EN_POTS_EVENT_OFFHOOK, NULL); */ 
            }
        }

        return 1;
    }

    if (Utl_StrFndNL(Data, "+CUSD:", 0, 10, length) != -1)
    {
        RIL_USSD_URC    *UssdData;
        RIL_PARSE_RES    UssdPResult;

        WIRELESS_INFO("Channel %d Get CUSD Data(%d):%s\r\n", ChnId, sizeof(Data), Data);
        UssdData = (RIL_USSD_URC*)WaveC_WmPsUrc_USSD(&UssdPResult, Data, strNum, strSize);
        if (UssdPResult == RIL_prOK)
        {
            *UrcEventID = RIL_NOTIFY_SUPSERVICEDATA;
            *UrcEventType = WM_TYPE_CALL;
            *UrcParse = UssdData;
            pWirelessChannelCtl->uiUSSDRetryTime = 0;

            WIRELESS_INFO("Channel %d WaveC_CheckUrcData Send USSD Response:%s(%s)(%d)\r\n", ChnId, UssdData->str, __FILE__, __LINE__);
            return 1;
        }

        return 0;
    }

    if (Utl_StrFndNL(Data, "+CMT:", 0, 10, length) != -1)    /* short message incoming, rout PDU directly */
    {        
        _SHORT16        iBegin;
        _SHORT16        iEnd;
        _SHORT16        smsLen;
        T_STR_800    strTemp;
        RIL_URC_MSG  *MsgUrcParse = NULL;

        MsgUrcParse = (RIL_URC_MSG*)WIRELESS_MALLOC(sizeof(RIL_URC_MSG));        
        memset(MsgUrcParse, 0, (sizeof(RIL_URC_MSG)));
        
        iBegin = Utl_StrFndL(Data, "\"", 0, length);
        if (iBegin != -1)
        {
            iEnd = Utl_StrFndL(Data, "\"", (iBegin + 1), length);
            if (iEnd != -1)
            {
                Utl_StrMidL(MsgUrcParse->Number, Data, (_SHORT16)(iBegin + 1), (_SHORT16)(iEnd - 1), length);
            }
        }
        
        iBegin = Utl_StrFndL(Data, ",", 0, length);
        if (iBegin != -1)
        {
            iEnd = Utl_StrFndL(Data, "\r", iBegin, length);
            if (iEnd != -1)
            {
                Utl_StrMidL(strTemp, Data, (_SHORT16)(iBegin + 1), (_SHORT16)(iEnd - 1), length);
                
                smsLen = (_SHORT16)Utl_Atoi(strTemp);
                MsgUrcParse->length = smsLen;

                if((pWirelessChannelCtl->recSmsPdu == EOS_TRUE) && (NULL != pWirelessChannelCtl->UrcParse_temp))
                {
                    WIRELESS_FREE(pWirelessChannelCtl->UrcParse_temp);
                    pWirelessChannelCtl->UrcParse_temp = NULL;
                }
                pWirelessChannelCtl->UrcParse_temp = (_VOID*)MsgUrcParse;
                pWirelessChannelCtl->recSmsPdu = EOS_TRUE;
            }            
            else
            {        
                WIRELESS_FREE(MsgUrcParse);
            }
        }        
        else
        {        
            WIRELESS_FREE(MsgUrcParse);
        }

        return 2;
    }

    if(EOS_TRUE == pWirelessChannelCtl->recSmsPdu)
    {
        pWirelessChannelCtl->recSmsPdu = EOS_FALSE;
        if(1)/*if(Utl_StrIsHex(Data))*/
        {
            if(length > MAX_PDU_LEN)
            {
                length = MAX_PDU_LEN;
            }
                
            memcpy(((RIL_URC_MSG *)pWirelessChannelCtl->UrcParse_temp)->Pdudata, (_CHAR8*)Data, length);
            ((_CHAR8*)((RIL_URC_MSG *)pWirelessChannelCtl->UrcParse_temp)->Pdudata)[length + 1] = '\0';
            
            *UrcEventID = RIL_NOTIFY_MESSAGE;
            *UrcEventType = WM_TYPE_MSG;
            *strNum = 1;
            *strSize = sizeof(RIL_URC_MSG);
            *UrcParse = (_VOID*)pWirelessChannelCtl->UrcParse_temp;
            return 1;
        }
        else
        { 
            if(NULL != pWirelessChannelCtl->UrcParse_temp)
            {
                WIRELESS_FREE(pWirelessChannelCtl->UrcParse_temp);
                pWirelessChannelCtl->UrcParse_temp = NULL;
            }
            *strNum = 0;
            *strSize = 0;

            return 3;
        }
    }
    
    if (Utl_StrFndNL(Data, "CDS:", 0, 10, length) != -1)  /*  status report incoming*/
    {
        
        _SHORT16        iBegin;
        _SHORT16        iEnd;    
        _SHORT16        smsLen;
        T_STR_20    strTemp;
        RIL_URC_MSG      *MsgUrcParse  = NULL;

        MsgUrcParse = (RIL_URC_MSG*)WIRELESS_MALLOC(sizeof(RIL_URC_MSG));        
        memset(MsgUrcParse, 0, (sizeof(RIL_URC_MSG)));

        iBegin = Utl_StrFndL(Data, " ", 0, length);
        if (iBegin != -1)
        {
            iEnd = Utl_StrFndL(Data, "\r", iBegin, length);
            if (iEnd != -1)
            {
                Utl_StrMidL(strTemp, Data, (_SHORT16)(iBegin + 1), (_SHORT16)(iEnd - 1), length);
                smsLen = (_SHORT16)Utl_Atoi(strTemp);
                MsgUrcParse->length = smsLen;

                if((pWirelessChannelCtl->recCdsPdu == EOS_TRUE) && (NULL != pWirelessChannelCtl->UrcParse_temp))
                {
                    WIRELESS_FREE(pWirelessChannelCtl->UrcParse_temp);
                    pWirelessChannelCtl->UrcParse_temp = NULL;
                }
                pWirelessChannelCtl->UrcParse_temp = (_VOID*)MsgUrcParse;
                pWirelessChannelCtl->recCdsPdu = EOS_TRUE;
            }
            else
            {        
                WIRELESS_FREE(MsgUrcParse);
            }
        }
        else
        {
            WIRELESS_FREE(MsgUrcParse);
        }
        return 2;
    }
    
    if(EOS_TRUE == pWirelessChannelCtl->recCdsPdu)
    {
        pWirelessChannelCtl->recCdsPdu = EOS_FALSE;
        if(Utl_StrIsHex(Data))
        {
            WIRELESS_INFO("Channel %d Get a Message Info (%s)(%d)\r\n", ChnId, __FILE__, __LINE__);
            memcpy(((RIL_URC_MSG *)pWirelessChannelCtl->UrcParse_temp)->Pdudata, (_CHAR8*)Data, length);
            *UrcEventID = RIL_NOTIFY_STATUSMESSAGE;
            *UrcEventType = WM_TYPE_MSG;
            *strNum = 1;
            *strSize = sizeof(RIL_URC_MSG);
            *UrcParse = (_VOID *)pWirelessChannelCtl->UrcParse_temp;
            
            //pWirelessChannelCtl->UrcParse_temp = NULL;
            return 1;
        }
        else
        { 
            if(NULL != pWirelessChannelCtl->UrcParse_temp)
            {
                WIRELESS_FREE(pWirelessChannelCtl->UrcParse_temp);
                pWirelessChannelCtl->UrcParse_temp = NULL;
            }
            *strNum = 0;
            *strSize = 0;
            return 3;
        }
        
    }
    
    if (Utl_StrFndNL(Data, "+CMTI:", 0, 10, length) != -1)    /* Indication of a new short message */
    {
        _SHORT16    iBegin;
        _SHORT16    iEnd;
        RIL_URC_CMTI *CmtiUrcParse = NULL;      /* SIM sms*/

        iBegin = Utl_StrFndL(Data, AT_COMMA, 0, length);
        iEnd = Utl_StrFndL(Data, ATCMD_RETURN, iBegin, length);
        WIRELESS_INFO("Channel %d New Incoming Short Message Incoming\r\n", ChnId);
        if(iBegin != -1 && iEnd != -1)
        {
            T_STR_20    string;

            CmtiUrcParse = (RIL_URC_CMTI *)WIRELESS_MALLOC(sizeof(RIL_URC_CMTI));
            Utl_StrMidL(string, Data, (_SHORT16)(iBegin + 1), (_SHORT16)(iEnd - 1), length);
            CmtiUrcParse->index = (_USHORT16)Utl_Atoi(string);

            WIRELESS_INFO("Channel %d CmtiUrcParse->index = %d\r\n", ChnId, CmtiUrcParse->index);

            *UrcEventID = RIL_NOTIFY_MESSAGE_IN_SIM;
            *UrcEventType = WM_TYPE_MSG;
            *strNum = 1;
            *strSize = sizeof(RIL_URC_CMTI);
            *UrcParse = (_VOID *)CmtiUrcParse;
            return 1;
        }
        return 3;
    }

    if (Utl_StrFndNL(Data, "+CCWA:", 0, 10, length) != -1)    /* Call waiting indication */
    {
        RIL_PARSE_RES    CcwaResult;
        RIL_CCWA_LIST    *urcCcwa;

        urcCcwa = (RIL_CCWA_LIST*)(WaveC_WmPsGsmCd_pCCWAProcess(&CcwaResult, Data, strNum, strSize));
        if (RIL_prOK == CcwaResult)
        {
            *UrcEventID = RIL_NOTIFY_CALLWAITING;
            *UrcEventType = WM_TYPE_CALL;
            WIRELESS_TRACE;
            WIRELESS_FREE(urcCcwa);
            *UrcParse = NULL;
            return 1;
        }
        return 0;
    }
    
    if (Utl_StrFndNL(Data, "+CLIP:", 0, 10, length) != -1)
    {
        RIL_PARSE_RES    ClipResult;
        RIL_URC_CLIP    *urcClip;

        if(NVM_GOIP_MODTYPE_C202 == pWirelessChannelCtl->ModuleType)
        {
            urcClip = (RIL_URC_CLIP*)(C202_WmPsUrc_CLIP(&ClipResult, Data, strNum, strSize));
        }
        else if(NVM_GOIP_MODTYPE_MC323 == pWirelessChannelCtl->ModuleType)
        {
            urcClip = (RIL_URC_CLIP*)(MC323_WmPsUrc_CLIP(&ClipResult, Data, strNum, strSize));
        }
        else
        {
            urcClip = (RIL_URC_CLIP*)(WaveC_WmPsUrc_CLIP(&ClipResult, Data, strNum, strSize));
        }

        if ((RIL_prOK == ClipResult))
        {
            *UrcEventID = RIL_NOTIFY_CALLINLINE;
            *UrcEventType = WM_TYPE_CALL;
            /*WIRELESS_FREE(urcClip);*/
            *UrcParse = urcClip;    
            return 1;
        }
        return 0;
    }
    
    /* SIM Init completely in modem */
    if(Utl_StrFndNL(Data, "+PBREADY", 0, 16, length) != -1)        
    {
        *UrcEventID = RIL_NOTIFY_SIMREADY;
        *UrcEventType = WM_TYPE_SIM;
        *strNum = 0;
        *strSize = 0;
        *UrcParse = NULL;
        return 1;
    }

    //if(Utl_StrFndNL(Data, "+CREG:", 0, 10, length) != -1)
    if(Utl_StrFndNL(Data, "+CGREG:", 0, 10, length) != -1)
    {
        RIL_NET_STAT    *state;
        RIL_PARSE_RES        pr;

        //WIRELESS_INFO("Channel %d Detect CGREG, %s\r\n", ChnId, Data);    
        state = (RIL_NET_STAT*)WaveC_WmPsUrc_L81X_CGREG(&pr, Data, strNum, strSize);
        //state = (RIL_NET_STAT*)WaveC_WmPsUrc_CREG(&pr, Data, strNum, strSize);
        
        if(pr == RIL_prOK)
        {
            *UrcEventID = RIL_NOTIFY_REGSTATUSCHANGED;
            *UrcEventType = WM_TYPE_SIM;
            *UrcParse = state;
            return 1;
        }
        return 0;
    }
 
    if ((ChnId == 1) && (Utl_StrFndNL(Data, "+COPS:", 0, 10, length) != -1))
    {
        unsigned char strBuff[64] = {0};
        unsigned char strCnt = 0;
        unsigned int  gsmModeVal = 0;
        
        strncpy(strBuff, Data, 63);
        while(strCnt < 63)
        {
            if((strBuff[strCnt] == '+') && (strncmp(&strBuff[strCnt], "+COPS: ", strlen("+COPS: ")) == NULL))
            {
                while(strBuff[strCnt] != '\n')
                {
                    strCnt ++;
                }
                gsmModeVal = strBuff[strCnt - 2] - '0';  //jump \r\n, the last paramer is mode number.
                
                WIRELESS_INFO("Channel %d Get the GSM mode=%d.\n", ChnId, gsmModeVal);
                break;
            }
            strCnt++;
        }
    }
 
    return 0;
}


/*end WaveC_CheckUrcData()*/

