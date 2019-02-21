/********************************************************************
 * @file msg_manager.c
 * @brief: The implementation of the msg manager
 *
 ********************************************************************/
#include "line.h"
#include "msg_manager.h"
#include "wm_interface.h"
#include "radio_server.h"

#include "../wireless_common.h"
#include "../wireless_interface_inc.h"
#include "../wireless_debug.h"

enum
{
    eRequest_MSGMng = 0,
    eRequest_Line
};


extern WIRELESSROOTST    stWirelessRootSt;

_UINT32 SendMSG_Callback(HMSGMANAGER hMSGManager, 
                              _UINT32 resultType, _VOID* lpData, _UINT32 totalSize, _UINT32 usedSize,
                              _UINT32 customParam);

_UINT32 SendQiSend_Callback(HMSGMANAGER hMSGManager, 
                              _UINT32 resultType, _VOID* lpData, _UINT32 totalSize, _UINT32 usedSize,
                              _UINT32 customParam);

_UINT32 SendMSG_CallbackByTxt(HMSGMANAGER hMSGManager, 
                              _UINT32 resultType, _VOID* lpData, _UINT32 totalSize, _UINT32 usedSize,
                              _UINT32 customParam);

_UINT32 SendMSG_CallbackByTxt(HMSGMANAGER hMSGManager, 
                              _UINT32 resultType, _VOID* lpData, _UINT32 totalSize, _UINT32 usedSize,
                              _UINT32 customParam);

_UINT32 SendMSG_Callback_CDMA(HMSGMANAGER hMSGManager, 
                              _UINT32 resultType, _VOID* lpData, _UINT32 totalSize, _UINT32 usedSize,
                              _UINT32 customParam);
                              
_UINT32 SaveMSG_Callback(HMSGMANAGER hMSGManager, 
                              _UINT32 resultType, _VOID* lpData, _UINT32 totalSize, _UINT32 usedSize,
                              _UINT32 customParam);
_UINT32 SendPDU_Callback(HMSGMANAGER hMSGManager, 
                              _UINT32 resultType, _VOID* lpData, _UINT32 totalSize, _UINT32 usedSize,
                              _UINT32 customParam);

_UINT32 SendQidata_Callback(HMSGMANAGER hMSGManager, 
                              _UINT32 resultType, _VOID* lpData, _UINT32 totalSize, _UINT32 usedSize,
                              _UINT32 customParam);

_UINT32 SendTXT_Callback(HMSGMANAGER hMSGManager, 
                              _UINT32 resultType, _VOID* lpData, _UINT32 totalSize, _UINT32 usedSize,
                              _UINT32 customParam);

_UINT32 MSGACK_Callback(HMSGMANAGER hMSGManager, 
                            _UINT32 resultType, _VOID* lpData, _UINT32 totalSize, _UINT32 usedSize,
                            _UINT32 customParam);
/*
 *$)A3uJ<;/MSG manager
 */
_UINT32 MSGManager_Init(HMSGMANAGER *phMSGManager)
{
    T_MSGMANAGER *pMSGManager;

    pMSGManager = (T_MSGMANAGER*)malloc(sizeof(T_MSGMANAGER));
    pMSGManager->state = eMSGSTATE_IDLE;
    
    *phMSGManager = pMSGManager;
    return 0;
}

_UINT32 MSGManager_DeInit(HMSGMANAGER hMSGManager)
{
    free(hMSGManager);
    return 0;
}

_UINT32 MSGManager_SendMSG(_UINT32 hLine, const _UCHAR8* pPDU, _UINT32 option, _UINT32 customParam)
{
    T_MSGPARAM    *pParam;
    _UINT32        rtn;
    _SHORT16    length;

    pParam = (T_MSGPARAM*)WIRELESS_MALLOC(sizeof(T_MSGPARAM));
    pParam->hLine = (HLINE)hLine;
    pParam->hATServer = (_UINT32)((HLINE)(pParam->hLine)->hATServer);
    pParam->requestOwner = eRequest_MSGMng;
    pParam->customParam = customParam;
    pParam->RetryTimes = 5;
    
    Utl_StrCpyN((_CHAR8*)pParam->pdu, (_CHAR8*)pPDU, MAX_PDU_LEN);

    pParam->event = 0;
    pParam->pfCallback = SendMSG_Callback;

    length = strlen((const _CHAR8*)pPDU);
    Utl_Itoa((length - 2) / 2, pParam->PduLen, 10);

    rtn = WM_SendSmsByPduMode(((HLINE)hLine)->hATServer->hWM,
                            (_UINT32)(((HLINE)hLine)->hATServer->hMSGManager),
                            eDEVICESMS,
                            (_UINT32)pParam,
                            pParam->PduLen);

    ((HLINE)hLine)->hATServer->hMSGManager->state = eMSGSTATE_SENDLENGTH;
    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 MSGManager_Qisend(_UINT32 hLine, const _UCHAR8* pData, _UINT32 DataLen, _UINT32 customParam)
{
    T_MSGPARAM    *pParam;
    _UINT32        rtn;
    _CHAR8         StrDataLen[8];

    pParam = (T_MSGPARAM*)WIRELESS_MALLOC(sizeof(T_MSGPARAM));
    pParam->hLine = (HLINE)hLine;
    pParam->hATServer = (_UINT32)((HLINE)(pParam->hLine)->hATServer);
    pParam->requestOwner = eRequest_MSGMng;
    pParam->customParam = customParam;
    pParam->RetryTimes = 5;
    
    memcpy((_CHAR8*)pParam->pdu, (_CHAR8*)pData, MAX_PDU_LEN);
    if(DataLen < MAX_PDU_LEN)
    {
        pParam->iPduLen = DataLen;
    }
    else
    {
        pParam->iPduLen = MAX_PDU_LEN;
    }

    pParam->event = 0;
    pParam->pfCallback = SendQiSend_Callback;

    sprintf(StrDataLen,"%d",DataLen);
    rtn = WM_SendQidata(((HLINE)hLine)->hATServer->hWM,
                            (_UINT32)(((HLINE)hLine)->hATServer->hMSGManager),
                            eDEVICESMS,
                            (_UINT32)pParam,
                            StrDataLen);

    ((HLINE)hLine)->hATServer->hMSGManager->state = eMSGSTATE_SENDLENGTH;
    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}


_UINT32 MSGManager_SendMSGByTxt(_UINT32 hLine, const _UCHAR8* pPDU, _UINT32 uiStrLen, const _UCHAR8* pNumber, _UINT32 customParam)
{
    T_MSGPARAM    *pParam;
    _UINT32        rtn;
    _SHORT16    length;

    pParam = (T_MSGPARAM*)WIRELESS_MALLOC(sizeof(T_MSGPARAM));
    pParam->hLine = (HLINE)hLine;
    pParam->hATServer = (_UINT32)((HLINE)(pParam->hLine)->hATServer);
    pParam->requestOwner = eRequest_MSGMng;
    pParam->customParam = customParam;
    pParam->RetryTimes = 5;
    
    memcpy((_CHAR8*)pParam->pdu, (_CHAR8*)pPDU, uiStrLen);

    pParam->iPduLen = uiStrLen;

    pParam->event = 0;
    pParam->pfCallback = SendMSG_CallbackByTxt;

    //pParam->PduLen = uiStrLen;

    rtn = WM_SendSmsByTxtMode(((HLINE)hLine)->hATServer->hWM,
                            (_UINT32)(((HLINE)hLine)->hATServer->hMSGManager),
                            eDEVICESMS,
                            (_UINT32)pParam,
                            pNumber);

    ((HLINE)hLine)->hATServer->hMSGManager->state = eMSGSTATE_SENDLENGTH;
    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 MSGManager_SendMSG_MC323(_UINT32 hLine, const _UCHAR8* pPDU, _UINT32 option, _UINT32 customParam)
{
    T_MSGPARAM    *pParam;
    _UINT32        rtn;
    _SHORT16    length;

    pParam = (T_MSGPARAM*)WIRELESS_MALLOC(sizeof(T_MSGPARAM));
    pParam->hLine = (HLINE)hLine;
    pParam->hATServer = (_UINT32)((HLINE)(pParam->hLine)->hATServer);
    pParam->requestOwner = eRequest_MSGMng;
    pParam->customParam = customParam;
    pParam->RetryTimes = 5;
    
    Utl_StrCpyN((_CHAR8*)pParam->pdu, (_CHAR8*)pPDU, MAX_PDU_LEN);

    pParam->event = 0;
    pParam->pfCallback = SendMSG_Callback;

    length = strlen((const _CHAR8*)pPDU);
    Utl_Itoa((length) / 2, pParam->PduLen, 10);

    rtn = WM_SendSmsByPduMode(((HLINE)hLine)->hATServer->hWM,
                            (_UINT32)(((HLINE)hLine)->hATServer->hMSGManager),
                            eDEVICESMS,
                            (_UINT32)pParam,
                            pParam->PduLen);

    ((HLINE)hLine)->hATServer->hMSGManager->state = eMSGSTATE_SENDLENGTH;
    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 MSGManager_SendMSG_CDMA(_UINT32 hLine, const _UCHAR8* pPDU, _UINT32 option, _UINT32 customParam)
{
    T_MSGPARAM    *pParam;
    _UINT32        rtn;

    pParam = (T_MSGPARAM*)WIRELESS_MALLOC(sizeof(T_MSGPARAM));
    pParam->hLine = (HLINE)hLine;
    pParam->hATServer = (_UINT32)((HLINE)(pParam->hLine)->hATServer);
    pParam->requestOwner = eRequest_MSGMng;
    pParam->customParam = customParam;
    pParam->RetryTimes = 0;
    
    Utl_StrCpyN((_CHAR8*)pParam->pdu, (_CHAR8*)pPDU, MAX_PDU_LEN_CDMA);

    pParam->event = 0;
    pParam->pfCallback = SendMSG_Callback_CDMA;

    rtn = WM_SendSmsByPduMode_CDMA(((HLINE)hLine)->hATServer->hWM,
                            (_UINT32)(((HLINE)hLine)->hATServer->hMSGManager),
                            eDEVICESMS,
                            (_UINT32)pParam,
                            pPDU);

    ((HLINE)hLine)->hATServer->hMSGManager->state = eMSGSTATE_SENDPDU;
    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 MSGManager_SaveMSG(_UINT32 hLine, const _UCHAR8* pPDU, _UINT32 option, _UCHAR8 read_flag, _SHORT16 PDUlen, _UINT32 customParam)
{
    T_MSGPARAM    *pParam;
    _UINT32        rtn;
    T_STR_20    strTemp;

    pParam = (T_MSGPARAM*)WIRELESS_MALLOC(sizeof(T_MSGPARAM));
    pParam->hLine = (HLINE)hLine;
    pParam->hATServer = (_UINT32)((HLINE)(pParam->hLine)->hATServer);
    pParam->requestOwner = eRequest_MSGMng;
    pParam->customParam = customParam;
    pParam->RetryTimes = 0;
    Utl_StrCpyN((_CHAR8*)pParam->pdu, (_CHAR8*)pPDU, MAX_PDU_LEN);

    pParam->event = 0;
    pParam->pfCallback = SaveMSG_Callback;

    Utl_Itoa(PDUlen, strTemp, 10);

    rtn = WM_SaveSmsByPduMode(((HLINE)hLine)->hATServer->hWM,
                            (_UINT32)(((HLINE)hLine)->hATServer->hMSGManager),
                            eDEVICESMS,
                            (_UINT32)pParam,
                            strTemp,
                            read_flag);

    ((HLINE)hLine)->hATServer->hMSGManager->state = eMSGSTATE_SENDLENGTH;
    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 MSGManager_ReadSMS(_UINT32 hLine, _USHORT16 index, _UINT32 customParam)
{
    _UINT32        rtn;
    
    rtn = WM_ReadSMS(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam, index);
    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 MSGManager_ReadSMS_ByTxt(_UINT32 hLine, _USHORT16 index, _UINT32 customParam)
{
    _UINT32        rtn;
    
    rtn = WM_ReadSMS_ByTxt(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam, index);
    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 MSGManager_ReadSMS_CDMA(_UINT32 hLine, _USHORT16 index, _UINT32 customParam)
{
    _UINT32        rtn;
    
    rtn = WM_ReadSMS_CDMA(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam, index);
    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 MSGManager_ReadSMSUnchgStat(_UINT32 hLine, _USHORT16 index, _UINT32 customParam)
{
    _UINT32        rtn;
    
    rtn = WM_ReadSMSUnchgStat(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam, index);
    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 MSGManager_DeleteSMS(_UINT32 hLine, _USHORT16 index, _UINT32 customParam)
{
    _UINT32        rtn;
    T_STR_20    strIndex;

    Utl_Itoa(index, strIndex, 10);    
    rtn = WM_DeleteSMS(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam, strIndex);
    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 MSGManager_InquireSmsStorage(_UINT32 hLine, _UINT32 customParam, RIL_MSG_STORAGE storage)
{
    _UINT32 rtn;

    rtn = WM_InquireSmsStorage(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam, storage);
    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 MSGManager_ListSMS(_UINT32 hLine, _UINT32 customParam, RIL_SMS_MODE stat)
{
    _UINT32 rtn;

    rtn = WM_ListSMS(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam, stat);
    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 MSGManager_SetSmsCenter(_UINT32 hLine, _UINT32 customParam, _CHAR8* strSca)
{
    _UINT32 rtn;

    rtn = WM_SetSmsCenter(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam, strSca);
    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 MSGManager_InquireSmsCenter(_UINT32 hLine, _UINT32 customParam)
{
    _UINT32 rtn;

    rtn = WM_InquireSmsCenter(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam);
    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 MSGManager_NewMsgStoreMode(_UINT32 hLine, _UINT32 customParam, RIL_STORENEWMSG_MODE mode)
{
    _UINT32 rtn;

    rtn = WM_StoreNewMsgMode(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam, mode);
    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}
_UINT32 MSGManager_SetExtSMSStorageStatus(_UINT32 hLine, _UINT32 customParam, _BOOL bFull)
{
    _UINT32 rtn;

    rtn = WM_SetExtSMSStorageStatus(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam, bFull);
    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 MSGManager_SelectBroadcast(_UINT32 hLine, _UINT32 customParam, _CHAR8* strMode, _CHAR8* strMids, _CHAR8* strDcss)
{
    _UINT32 rtn;

    rtn = WM_SelectBroadcast(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam, strMode, strMids, strDcss);
    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}


_UINT32 MSGManager_ResultCallback(HMSGMANAGER hMSGManager, 
                              _UINT32 resultType, _VOID* lpData, _UINT32 totalSize, _UINT32 usedSize,
                              _UINT32 customParam)
{
    PWIRELESSROOTST pWirelessRootSt;
    T_MSGPARAM *pParam;

    pWirelessRootSt = (PWIRELESSROOTST)&stWirelessRootSt;

    if(0 == customParam)
    {
        return 0;
    }
    
    pParam = (T_MSGPARAM*)customParam;

#if 0
    if(pParam->requestOwner == eRequest_Line)
    {
        Line_ResultCallback(pParam->hLine, resultType, lpData, totalSize, usedSize, pParam->customParam);
        pParam->customParam = NULL;
    }
#endif    

    if(NULL != pParam->pfCallback)
    {
        pParam->pfCallback(hMSGManager, resultType, lpData, totalSize, usedSize, customParam);
    }
    else
    {
        if(pParam->customParam != NULL)
        {
            WIRELESS_FREE(pParam->customParam);
            pParam->customParam = NULL;
        }
        WIRELESS_FREE(pParam);
    }

    /* need to confirm later.kinson
    if(pParam != NULL)
    {
        WIRELESS_FREE(pParam);
    }
    */
    return 0;
}

/*
 *MSG manager$)A4&@mURC;X5w:/J}
 */
_UINT32 MSGManager_URCCallback(_UINT32 hATServer, _UINT32 eventType, _UINT32 eventID, _VOID* lpData, _UINT32 totalSize, _UINT32 usedSize)
{
    switch(eventID)
    {
    case RIL_NOTIFY_MESSAGE:
    case RIL_NOTIFY_STATUSMESSAGE:
        if(1)/*if(WM_NewMsgNeedAck(((HATSERVER)hATServer)->hWM) == EOS_FALSE)*/
        {
            WIRELESS_INFO("MSGManager_URCCallback Get a New Message(%s)(%d)\r\n", __FILE__, __LINE__);
            /*The new message needn't acknowledge.*/
            Line_URCCallback((HATSERVER)hATServer, eventType, eventID, 0, lpData, totalSize, usedSize);
        }
        else
        {
            /*send acknowledgment first*/
            T_MSGPARAM    *pParam;

            pParam = (T_MSGPARAM*)WIRELESS_MALLOC(sizeof(T_MSGPARAM));
            pParam->hATServer = hATServer;
            pParam->requestOwner = eRequest_MSGMng;

            pParam->event = eventID;
            pParam->lpData = lpData;
            pParam->totalSize = totalSize;
            pParam->usedSize = usedSize;
            
            pParam->pfCallback = MSGACK_Callback;

            ATServer_SetConnState((HATSERVER)hATServer, CONNSTATE_NEWMSGON);

            WM_NewSMSAck(((HATSERVER)hATServer)->hWM,
                        (_UINT32)((HATSERVER)hATServer)->hMSGManager,
                        eDEVICESMS,
                        (_UINT32)pParam);
        }
        break;
    case RIL_NOTIFY_MESSAGE_IN_SIM:
        {
            RIL_URC_CMTI *stru = (RIL_URC_CMTI *)lpData;
            _USHORT16         index;

            index = stru->index;
            WIRELESS_FREE(stru);
            WIRELESS_INFO("(%s)(%d)Index: %d\r\n", __FILE__, __LINE__, index);
            Line_URCCallback((HATSERVER)hATServer, eventType, eventID, index, NULL, 0, 0);
        }
        break;
    default:
        Line_URCCallback((HATSERVER)hATServer, eventType, eventID, 0, lpData, totalSize, usedSize);
    }
    return 0;
}

_UINT32 SendMSG_Callback(HMSGMANAGER hMSGManager, 
                              _UINT32 resultType, _VOID* lpData, _UINT32 totalSize, _UINT32 usedSize,
                              _UINT32 customParam)
{
    T_MSGPARAM *pParam;
    _UINT32        rtn;
    
    if(0 == customParam)
    {
        return 0;
    }
    pParam = (T_MSGPARAM *)customParam;

    if(resultType != RIL_prOK)
    {
        Line_ResultCallback(pParam->hLine, resultType, lpData, totalSize, usedSize, pParam->customParam);
        pParam->customParam = NULL;
        WIRELESS_FREE(pParam);
        hMSGManager->state = eMSGSTATE_IDLE;
        rtn = 0;
    }
    else
    {
        _UINT32    connState;

        ATServer_GetConnState((HATSERVER)pParam->hATServer, &connState);

        /*send PDU data*/
        pParam->pfCallback = SendPDU_Callback;

        if(connState & (CONNECT_STATE_RING | CONNECT_STATE_MSG))
        {
            /*new message or RING incoming, abort sending message*/
            /*Only send ctrl-Z to the wireless module, the module will return error immediately*/
            rtn = WM_SendSmsPduData(((HATSERVER)pParam->hATServer)->hWM,
                                    (_UINT32)hMSGManager,
                                    eDEVICESMS,
                                    customParam,
                                    "");
        }
        else
        {
            rtn = WM_SendSmsPduData(((HATSERVER)pParam->hATServer)->hWM,
                                    (_UINT32)hMSGManager,
                                    eDEVICESMS,
                                    customParam,
                                    pParam->pdu);
        }

        hMSGManager->state = eMSGSTATE_SENDPDU;
    }
    return rtn;
}

_UINT32 SendQiSend_Callback(HMSGMANAGER hMSGManager, 
                              _UINT32 resultType, _VOID* lpData, _UINT32 totalSize, _UINT32 usedSize,
                              _UINT32 customParam)
{
    T_MSGPARAM *pParam;
    _UINT32        rtn;
    
    if(0 == customParam)
    {
        return 0;
    }
    pParam = (T_MSGPARAM *)customParam;

    if(resultType != RIL_prOK)
    {
        Line_ResultCallback(pParam->hLine, resultType, lpData, totalSize, usedSize, pParam->customParam);
        pParam->customParam = NULL;
        WIRELESS_FREE(pParam);
        hMSGManager->state = eMSGSTATE_IDLE;
        rtn = 0;
    }
    else
    {
        _UINT32    connState;

        ATServer_GetConnState((HATSERVER)pParam->hATServer, &connState);

        /*send PDU data*/
        pParam->pfCallback = SendQidata_Callback;

        WIRELESS_INFO("(%s)(%d) connState:0x%x\r\n", __FILE__, __LINE__, connState);
        if(connState & (CONNECT_STATE_RING | CONNECT_STATE_MSG))
        {
            
            WIRELESS_INFO("(%s)(%d)Send Null Data by TCP?\r\n", __FILE__, __LINE__);
            /*new message or RING incoming, abort sending message*/
            /*Only send ctrl-Z to the wireless module, the module will return error immediately*/
            rtn = WM_SendQiRawData(((HATSERVER)pParam->hATServer)->hWM,
                                    (_UINT32)hMSGManager,
                                    eDEVICESMS,
                                    customParam,
                                    "", 0);
        }
        else
        {
            rtn = WM_SendQiRawData(((HATSERVER)pParam->hATServer)->hWM,
                                    (_UINT32)hMSGManager,
                                    eDEVICESMS,
                                    customParam,
                                    pParam->pdu, pParam->iPduLen);
        }

        hMSGManager->state = eMSGSTATE_SENDPDU;
    }
    return rtn;
}


_UINT32 SendMSG_CallbackByTxt(HMSGMANAGER hMSGManager, 
                              _UINT32 resultType, _VOID* lpData, _UINT32 totalSize, _UINT32 usedSize,
                              _UINT32 customParam)
{
    T_MSGPARAM *pParam;
    _UINT32        rtn;
    
    if(0 == customParam)
    {
        return 0;
    }
    pParam = (T_MSGPARAM *)customParam;

    if(resultType != RIL_prOK)
    {
        Line_ResultCallback(pParam->hLine, resultType, lpData, totalSize, usedSize, pParam->customParam);
        pParam->customParam = NULL;
        WIRELESS_FREE(pParam);
        hMSGManager->state = eMSGSTATE_IDLE;
        rtn = 0;
    }
    else
    {
        _UINT32    connState;

        ATServer_GetConnState((HATSERVER)pParam->hATServer, &connState);

        /*send TXT data*/
        pParam->pfCallback = SendTXT_Callback;
        
        if(connState & (CONNECT_STATE_RING | CONNECT_STATE_MSG))
        {
            /*new message or RING incoming, abort sending message*/
            /*Only send ctrl-Z to the wireless module, the module will return error immediately*/
            rtn = WM_SendSmsTxtData(((HATSERVER)pParam->hATServer)->hWM,
                                    (_UINT32)hMSGManager,
                                    eDEVICESMS,
                                    customParam,
                                    "", 0);
        }
        else
        {
            rtn = WM_SendSmsTxtData(((HATSERVER)pParam->hATServer)->hWM,
                                    (_UINT32)hMSGManager,
                                    eDEVICESMS,
                                    customParam,
                                    pParam->pdu, pParam->iPduLen);
        }

        hMSGManager->state = eMSGSTATE_SENDPDU;
    }
    return rtn;
}

_UINT32 SendMSG_Callback_CDMA(HMSGMANAGER hMSGManager, 
                              _UINT32 resultType, _VOID* lpData, _UINT32 totalSize, _UINT32 usedSize,
                              _UINT32 customParam)
{
    T_MSGPARAM *pParam;
    _UINT32        rtn;
    
    if(0 == customParam)
    {
        return 0;
    }
    pParam = (T_MSGPARAM *)customParam;

    Line_ResultCallback(pParam->hLine, resultType, lpData, totalSize, usedSize, pParam->customParam);
    pParam->customParam = NULL;
    WIRELESS_FREE(pParam);
    hMSGManager->state = eMSGSTATE_IDLE;
    rtn = 0;
    
    return rtn;
}


_UINT32 SaveMSG_Callback(HMSGMANAGER hMSGManager, 
                              _UINT32 resultType, _VOID* lpData, _UINT32 totalSize, _UINT32 usedSize,
                              _UINT32 customParam)
{
    T_MSGPARAM *pParam;
    _UINT32        rtn;
    
    if(0 == customParam)
    {
        return 0;
    }
    pParam = (T_MSGPARAM *)customParam;

    if(resultType != RIL_prOK)
    {
        Line_ResultCallback(pParam->hLine, resultType, lpData, totalSize, usedSize, pParam->customParam);
        pParam->customParam = NULL;
        
        WIRELESS_FREE(pParam);
        hMSGManager->state = eMSGSTATE_IDLE;
        rtn = 0;
    }
    else
    {
        _UINT32    connState;

        ATServer_GetConnState((HATSERVER)pParam->hATServer, &connState);

        /*send PDU data*/
        pParam->pfCallback = SendPDU_Callback;

        if(connState & (CONNECT_STATE_RING | CONNECT_STATE_MSG))
        {
            /*new message or RING incoming, abort sending message*/
            /*Only send ctrl-Z to the wireless module, the module will return error immediately*/
            rtn = WM_SaveSmsPduData(((HATSERVER)pParam->hATServer)->hWM,
                                    (_UINT32)hMSGManager,
                                    eDEVICESMS,
                                    customParam,
                                    "");
        }
        else
        {
            rtn = WM_SaveSmsPduData(((HATSERVER)pParam->hATServer)->hWM,
                                    (_UINT32)hMSGManager,
                                    eDEVICESMS,
                                    customParam,
                                    pParam->pdu);
        }

        hMSGManager->state = eMSGSTATE_SENDPDU;
    }
    return rtn;
}

_UINT32 SendPDU_Callback(HMSGMANAGER hMSGManager, 
                              _UINT32 resultType, _VOID* lpData, _UINT32 totalSize, _UINT32 usedSize,
                              _UINT32 customParam)
{
    T_MSGPARAM *pParam;
    
    if(0 == customParam)
    {
        return 0;
    }
    pParam = (T_MSGPARAM *)customParam;

    if (resultType == RIL_prOK)
    {
        Line_ResultCallback(pParam->hLine, resultType, lpData, totalSize, usedSize, pParam->customParam);
        pParam->customParam = NULL;

        WIRELESS_FREE(pParam);
        
        pParam = NULL;
        
        hMSGManager->state = eMSGSTATE_IDLE;
    }
    else
    {
        /*
            if error type is invalid number format(28) or 
                          network out of order (38) or
                          requseted facility not subscirbed(50)
            do not try to send any more, just show sending error
        */
        if (RIL_prCMSERROR == resultType 
            && (usedSize == 28 || usedSize == 38 || usedSize == 50))
        {
            
            Line_ResultCallback(pParam->hLine, resultType, lpData, totalSize, usedSize, pParam->customParam);
            pParam->customParam = NULL;
            
            WIRELESS_FREE(pParam);
            pParam = NULL;
            
            hMSGManager->state = eMSGSTATE_IDLE;
            return 0;
        }

        if (pParam->RetryTimes > 0)
        {
            /*retry to send */
            pParam->RetryTimes--;
            pParam->pfCallback = SendMSG_Callback;
/*Kinson 
            AK_Sleep(400);
            */
            
            WM_SendSmsByPduMode(((HLINE)(pParam->hLine))->hATServer->hWM,
                            (_UINT32)(((HLINE)(pParam->hLine))->hATServer->hMSGManager),
                            eDEVICESMS,
                            (_UINT32)pParam,
                            pParam->PduLen);

            ((HLINE)(pParam->hLine))->hATServer->hMSGManager->state = eMSGSTATE_SENDLENGTH;
        }
        else
        {
            /* fail to send the sms , call the callback function*/
            Line_ResultCallback(pParam->hLine, resultType, lpData, totalSize, usedSize, pParam->customParam);
            pParam->customParam = NULL;
            
            WIRELESS_FREE(pParam);
            pParam = NULL;
            
            hMSGManager->state = eMSGSTATE_IDLE;

        }

    }

    return 0;
}

_UINT32 SendQidata_Callback(HMSGMANAGER hMSGManager, 
                              _UINT32 resultType, _VOID* lpData, _UINT32 totalSize, _UINT32 usedSize,
                              _UINT32 customParam)
{
    T_MSGPARAM *pParam;
    
    if(0 == customParam)
    {
        return 0;
    }
    pParam = (T_MSGPARAM *)customParam;

    if (resultType == RIL_prOK)
    {
        Line_ResultCallback(pParam->hLine, resultType, lpData, totalSize, usedSize, pParam->customParam);
        pParam->customParam = NULL;

        WIRELESS_FREE(pParam);
        
        pParam = NULL;
        
        hMSGManager->state = eMSGSTATE_IDLE;
    }
    else
    {
        /*
            if error type is invalid number format(28) or 
                          network out of order (38) or
                          requseted facility not subscirbed(50)
            do not try to send any more, just show sending error
        */
        if (RIL_prCMSERROR == resultType 
            && (usedSize == 28 || usedSize == 38 || usedSize == 50))
        {
            
            Line_ResultCallback(pParam->hLine, resultType, lpData, totalSize, usedSize, pParam->customParam);
            pParam->customParam = NULL;
            
            WIRELESS_FREE(pParam);
            pParam = NULL;
            
            hMSGManager->state = eMSGSTATE_IDLE;
            return 0;
        }

        if (pParam->RetryTimes > 0)
        {
            /*retry to send */
            pParam->RetryTimes--;
            pParam->pfCallback = SendMSG_Callback;
/*Kinson 
            AK_Sleep(400);
            */

            WIRELESS_INFO("(%s)(%d) WM_SendQidata\r\n", __FILE__, __LINE__);

            WM_SendQidata(((HLINE)(pParam->hLine))->hATServer->hWM,
                            (_UINT32)(((HLINE)(pParam->hLine))->hATServer->hMSGManager),
                            eDEVICESMS,
                            (_UINT32)pParam,
                            pParam->PduLen);

            ((HLINE)(pParam->hLine))->hATServer->hMSGManager->state = eMSGSTATE_SENDLENGTH;
        }
        else
        {
            /* fail to send the sms , call the callback function*/
            Line_ResultCallback(pParam->hLine, resultType, lpData, totalSize, usedSize, pParam->customParam);
            pParam->customParam = NULL;
            
            WIRELESS_FREE(pParam);
            pParam = NULL;
            
            hMSGManager->state = eMSGSTATE_IDLE;

        }
    }

    return 0;
}


_UINT32 SendTXT_Callback(HMSGMANAGER hMSGManager, 
                              _UINT32 resultType, _VOID* lpData, _UINT32 totalSize, _UINT32 usedSize,
                              _UINT32 customParam)
{
    T_MSGPARAM *pParam;
    
    if(0 == customParam)
    {
        return 0;
    }
    pParam = (T_MSGPARAM *)customParam;

    if (resultType == RIL_prOK)
    {
        Line_ResultCallback(pParam->hLine, resultType, lpData, totalSize, usedSize, pParam->customParam);
        pParam->customParam = NULL;

        WIRELESS_FREE(pParam);
        
        pParam = NULL;
        
        hMSGManager->state = eMSGSTATE_IDLE;
    }
    else
    {
        /*
            if error type is invalid number format(28) or 
                          network out of order (38) or
                          requseted facility not subscirbed(50)
            do not try to send any more, just show sending error
        */
        if (RIL_prCMSERROR == resultType 
            && (usedSize == 28 || usedSize == 38 || usedSize == 50))
        {
            
            Line_ResultCallback(pParam->hLine, resultType, lpData, totalSize, usedSize, pParam->customParam);
            pParam->customParam = NULL;
            
            WIRELESS_FREE(pParam);
            pParam = NULL;
            
            hMSGManager->state = eMSGSTATE_IDLE;
            return 0;
        }

        if (pParam->RetryTimes > 0)
        {
            /*retry to send */
            pParam->RetryTimes--;
            pParam->pfCallback = SendMSG_Callback;
/*Kinson 
            AK_Sleep(400);
            */
            
            WM_SendSmsByTxtMode(((HLINE)(pParam->hLine))->hATServer->hWM,
                            (_UINT32)(((HLINE)(pParam->hLine))->hATServer->hMSGManager),
                            eDEVICESMS,
                            (_UINT32)pParam,
                            pParam->PduLen);

            ((HLINE)(pParam->hLine))->hATServer->hMSGManager->state = eMSGSTATE_SENDLENGTH;
        }
        else
        {
            /* fail to send the sms , call the callback function*/
            Line_ResultCallback(pParam->hLine, resultType, lpData, totalSize, usedSize, pParam->customParam);
            pParam->customParam = NULL;
            
            WIRELESS_FREE(pParam);
            pParam = NULL;
            
            hMSGManager->state = eMSGSTATE_IDLE;

        }

    }

    return 0;
}

_UINT32 MSGACK_Callback(HMSGMANAGER hMSGManager, 
                            _UINT32 resultType, _VOID* lpData, _UINT32 totalSize, _UINT32 usedSize,
                            _UINT32 customParam)
{
    T_MSGPARAM *pParam;
    
    pParam = (T_MSGPARAM *)customParam;

    ATServer_SetConnState((HATSERVER)pParam->hATServer, CONNSTATE_NEWMSGOFF);

    if(resultType != RIL_prOK)
    {
        if(pParam->totalSize > 0)
        {
            WIRELESS_FREE(pParam->lpData);
        }
    }
    else
    {
        /*ATServer_GetConnState((HATSERVER)pParam->hATServer, &connState);*/
        Line_URCCallback((HATSERVER)pParam->hATServer, 
                        WM_TYPE_MSG, 
                        pParam->event, 
                        0,
                        pParam->lpData,
                        pParam->totalSize,
                        pParam->usedSize);
    }
    WIRELESS_FREE(pParam);
    return 0;
}
