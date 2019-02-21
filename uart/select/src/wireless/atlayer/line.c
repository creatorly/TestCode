
#include "wireless_list.h"
#include "svrcom.h"
#include "radio_server.h"
#include "line.h"
#include "proxycom.h"
#include "wm_interface.h"
#include "sim_manager.h"
#include "msg_manager.h"

#include "../wireless_common.h"
#include "../wireless_debug.h"
#include "../wireless_interface_inc.h"

extern WIRELESSROOTST    stWirelessRootSt;
/*
 *初始化line管理模块
 */
_UINT32 LineManager_Init(_UINT32 ChannelID)
{
    PWIRELESSROOTST     pWirelessRootSt;
    PWIRELESSCHANNELST  pWirelessChannelCtl;
    
    pWirelessRootSt = (PWIRELESSROOTST)&stWirelessRootSt;
    pWirelessChannelCtl = &pWirelessRootSt->stChannelSt[ChannelID];

    if(EOS_FALSE == pWirelessChannelCtl->stLineManager.bInit)
    {
        if(eos_sem_create("line_mag", &(pWirelessChannelCtl->stLineManager.mutex)) < 0)
        {
            printf("can't create semphore line_mag\r\n");
            return EOS_ERROR;
        }
        INIT_LIST_HEAD(&(pWirelessChannelCtl->stLineManager.head));
        pWirelessChannelCtl->stLineManager.bInit = EOS_TRUE;
    }
    return 0;
}

/*
 * 注销line管理模块
 */
_UINT32 LineManager_DeInit(_VOID)
{
    T_LINE* pLine;
    struct list_head* pos;
    PWIRELESSROOTST     pWirelessRootSt;
    PWIRELESSCHANNELST  pWirelessChannelCtl;
    
    pWirelessRootSt = (PWIRELESSROOTST)&stWirelessRootSt;
    pWirelessChannelCtl = &pWirelessRootSt->stChannelSt[1]; /*?*/

    if(pWirelessChannelCtl->stLineManager.bInit)
    {
        WirelessSemP(pWirelessChannelCtl->stLineManager.mutex, EOS_WAIT_FOREVER);
        for(pos = pWirelessChannelCtl->stLineManager.head.next; pos != &pWirelessChannelCtl->stLineManager.head; pos = pWirelessChannelCtl->stLineManager.head.next)
        {
            pLine = list_entry(pos, T_LINE, list);
            ril_list_del(pos);
            WIRELESS_FREE(pLine);
        }
        eos_sem_delete(pWirelessChannelCtl->stLineManager.mutex);
    }
    return 0;
}

_UINT32 Line_SendReplay(_UINT32 ChannelID, _UINT32 hLine, _UINT32 returnValue, _UINT32 paramter, _VOID* pBuff, _UINT32 totalSize, _UINT32 usedSize)
{
    T_MESSAGE msg;

    msg.type = MSG_REPLY;
    if(hLine != 0)
    {
        msg.param.reply.hRIL = ((HLINE)hLine)->hRIL;
    }
    else
    {
        msg.param.reply.hRIL = 0;
    }
    msg.param.reply.returnValue = returnValue;
    msg.param.reply.paramter = paramter;
    msg.param.reply.pBuff = pBuff;
    msg.param.reply.totalSize = totalSize;
    msg.param.reply.usedSize = usedSize;
    
    return SvrSend(ChannelID, (char*)&msg, sizeof(T_MESSAGE));
}

static _UINT32 Line_SendResponse(_UINT32 ChannelID, _UINT32 hRIL, _UINT32 resultType, _VOID* pBuff, _UINT32 totalSize, _UINT32 usedSize, _UINT32 customParam)
{
    T_MESSAGE msg;

    msg.type = MSG_RESPONSE;
    msg.param.response.hRIL = hRIL;
    msg.param.response.resultType = resultType;
    msg.param.response.pBuff = pBuff;
    msg.param.response.totalSize = totalSize;
    msg.param.response.usedSize = usedSize;
    msg.param.response.customParam = customParam;

    return SvrSend(ChannelID, (char*)&msg, sizeof(T_MESSAGE));
}

static _UINT32 Line_SendURC(_UINT32 ChannelID, _UINT32 hRIL, _UINT32 eventID, _UINT32 paramter, _VOID* pBuff, _UINT32 totalSize, _UINT32 usedSize)
{
    T_MESSAGE msg;

    msg.type = MSG_URC;
    msg.param.URC.hRIL = hRIL;
    msg.param.URC.eventID = eventID;
    msg.param.URC.paramter = paramter;
    msg.param.URC.pBuff = pBuff;
    msg.param.URC.totalSize = totalSize;
    msg.param.URC.usedSize = usedSize;

    return SvrSend(ChannelID, (char*)&msg, sizeof(T_MESSAGE));
}

/*
 *打开一个line句柄
 */
_UINT32 Line_Open(_UINT32 ChannelID, _UINT32 eventType, _UINT32 hATServer, _UINT32 hRIL)
{
    T_LINE *pLine;
    PWIRELESSROOTST     pWirelessRootSt;
    PWIRELESSCHANNELST  pWirelessChannelCtl;
    
    pWirelessRootSt = (PWIRELESSROOTST)&stWirelessRootSt;
    pWirelessChannelCtl = &pWirelessRootSt->stChannelSt[ChannelID];

    if(ATServer_CheckHandleValid((HATSERVER)hATServer))
    {
        pLine = (T_LINE*)malloc(sizeof(T_LINE));
        pLine->EventType = eventType;
        pLine->hRIL = hRIL;
        pLine->hATServer = (HATSERVER)hATServer;
        pLine->ChannelID = ChannelID;
        WirelessSemP(pWirelessChannelCtl->stLineManager.mutex, EOS_WAIT_FOREVER);
        ril_list_add(&pLine->list, &(pWirelessChannelCtl->stLineManager.head));
        WirelessSemV(pWirelessChannelCtl->stLineManager.mutex);
        return Line_SendReplay(ChannelID, 0, RIL_RTN_OK, (_UINT32)pLine, NULL, 0, 0);
    }
    else
    {
        return Line_SendReplay(ChannelID, 0, RIL_RTN_INVALID_HANDLE, 0, NULL, 0, 0);
    }
}

/*
 *关闭一个line句柄
 */
_UINT32 Line_Close(HLINE hLine)
{
    T_LINE *pLine;
    struct list_head *pos;
    PWIRELESSROOTST     pWirelessRootSt;
    PWIRELESSCHANNELST  pWirelessChannelCtl;
    
    pWirelessRootSt = (PWIRELESSROOTST)&stWirelessRootSt;
    pWirelessChannelCtl = &pWirelessRootSt->stChannelSt[hLine->ChannelID];

    WirelessSemP(pWirelessChannelCtl->stLineManager.mutex, EOS_WAIT_FOREVER);
    for (pos = pWirelessChannelCtl->stLineManager.head.next; pos != &pWirelessChannelCtl->stLineManager.head; pos = pos->next)
    {
        pLine = list_entry(pos, T_LINE, list);
        if(pLine == hLine)
        {
            ril_list_del(pos);
            free(pLine);
            break;
        }
    }
    WirelessSemV(pWirelessChannelCtl->stLineManager.mutex);

    return Line_SendReplay(hLine->ChannelID, 0, 0, 0, NULL, 0, 0);
}

/*
 *向所有line广播消息
 */
_UINT32 Line_URCCallback(HATSERVER hATServer, _UINT32 eventType, _UINT32 eventID, _UINT32 paramter, _VOID* lpData, _UINT32 totalSize, _UINT32 usedSize)
{
    T_LINE *pLine;
    struct list_head *pos;
    _UINT32    rtn = 0;
    PWIRELESSROOTST     pWirelessRootSt;
    PWIRELESSCHANNELST  pWirelessChannelCtl;
    
    pWirelessRootSt = (PWIRELESSROOTST)&stWirelessRootSt;

    if(hATServer->hWM->uiChnlId >= pWirelessRootSt->uiMaxChannelNum)
    {    
        WIRELESS_ERROR(hATServer->hWM->uiChnlId, "Line_URCCallback Channel Error!\r\n");
        return 0;
    }
    
    pWirelessChannelCtl = &pWirelessRootSt->stChannelSt[hATServer->hWM->uiChnlId]; 

    WirelessSemP(pWirelessChannelCtl->stLineManager.mutex, EOS_WAIT_FOREVER);

    for (pos = pWirelessChannelCtl->stLineManager.head.next; pos != &pWirelessChannelCtl->stLineManager.head; pos = pos->next)
    {
        pLine = list_entry(pos, T_LINE, list);
        if((hATServer == pLine->hATServer) && (pLine->EventType & eventType))
        {
            rtn = Line_SendURC(hATServer->hWM->uiChnlId, pLine->hRIL, eventID, paramter, lpData, totalSize, usedSize);
        }
    }

    if(NULL != lpData)
    {
        WIRELESS_FREE(lpData);
    }
    WirelessSemV(pWirelessChannelCtl->stLineManager.mutex);

    return rtn;
}

_UINT32 Line_ResultCallback(HLINE hLine, _UINT32 resultType, _VOID* lpData, _UINT32 totalSize, _UINT32 usedSize, _UINT32 customParam)
{
    _UINT32 rtn;

    rtn = Line_SendResponse(hLine->ChannelID, hLine->hRIL, resultType, lpData, totalSize, usedSize, customParam);
    
    if(lpData != NULL)
    {
        WIRELESS_FREE(lpData);
    }
    return rtn;
}

_UINT32 Line_GetATServer(HLINE hLine, HATSERVER *phATServer)
{
    *phATServer = hLine->hATServer;
    return 0;
}

_UINT32 Line_GetSleepState(_UINT32 hLine)
{
    _UINT32 rtn;

    rtn = WM_GetSleepState(((HLINE)hLine)->hATServer->hWM);
    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 Line_Attention(_UINT32 hLine, _UINT32 customParam)
{
    _UINT32 rtn;

    rtn = WM_Attention(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam);

    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 Line_InquireCurOperator(_UINT32 hLine, _UINT32 customParam)
{
    _UINT32 rtn;

    rtn = WM_InquireCurOperator(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam);

    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 Line_InquireBSInfo(_UINT32 hLine, _UINT32 customParam)
{
    _UINT32 rtn;

    rtn = WM_InquireBSInfo(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam);

    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 Line_InquireClock(_UINT32 hLine, _UINT32 customParam)
{
    _UINT32 rtn;

    rtn = WM_InquireClock(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam);

    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 Line_SendAT(_UINT32 hLine, _UINT32 customParam, _CHAR8* strValue)
{
    _UINT32 rtn;
    
    rtn = WM_SendAT(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam);
    
    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 Line_PWRCTRL(_UINT32 hLine, _UINT32 customParam, _CHAR8* strValue)
{
    _UINT32 rtn;
    
    rtn = WM_PwrCtrl(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam);
    
    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 Line_SetCommandEcho(_UINT32 hLine, _UINT32 customParam, _BOOL bOnOff)
{
    _UINT32 rtn;
    
    rtn = WM_SetCommandEcho(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam, bOnOff);
    
    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 Line_GetProductID(_UINT32 hLine, _UINT32 customParam, _CHAR8* strValue)
{
    _UINT32 rtn;
    
    rtn = WM_GetProductID(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam, strValue);
    
    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 Line_SetResultPresentation(_UINT32 hLine, _UINT32 customParam, _CHAR8* strValue)
{
    _UINT32 rtn;
    
    rtn = WM_SetResultPresentation(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam, strValue);
    
    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);


}

_UINT32 Line_SetResultFormat(_UINT32 hLine, _UINT32 customParam, _CHAR8* strValue)
{
    _UINT32 rtn;
    
    rtn = WM_SetResultFormat(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam, strValue);
    
    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);


}

_UINT32 Line_SetManufDefault(_UINT32 hLine, _UINT32 customParam)
{
    _UINT32 rtn;

    rtn = WM_SetManufDefault(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam);

    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 Line_InquireCCID(_UINT32 hLine, _UINT32 customParam)
{
    _UINT32 rtn;

    rtn = WM_InquireCCID(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam);

    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 Line_InquireIMSI(_UINT32 hLine, _UINT32 customParam)
{
    _UINT32 rtn;

    rtn = WM_InquireIMSI(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam);

    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 Line_InquireIMEI(_UINT32 hLine, _UINT32 customParam)
{
    _UINT32 rtn;

    rtn = WM_InquireIMEI(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam);

    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 Line_SetIMEI(_UINT32 hLine, _UINT32 customParam, _CHAR8* strIMEI)
{
    _UINT32 rtn;
    
    rtn = WM_SetIMEI(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam, strIMEI);
    
    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 Line_LockBS(_UINT32 hLine, _UINT32 customParam, _CHAR8* strBSIndex)
{
    _UINT32 rtn;
    
    rtn = WM_LockBS(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam, strBSIndex);
    
    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 Line_SetBaudrate(_UINT32 hLine, _UINT32 customParam, _CHAR8* strRate)
{
    _UINT32 rtn;
    
    rtn = WM_SetBaudrate(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam, strRate);
    
    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 Line_InquireBand(_UINT32 hLine, _UINT32 customParam)
{
    _UINT32 rtn;

    rtn = WM_InquireBand(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam);

    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}


_UINT32 Line_SetNetBand(_UINT32 hLine, _UINT32 customParam, _CHAR8* strMode)
{
    _UINT32 rtn;
    
    rtn = WM_SetNetBand(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam, strMode);
    
    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);


}

_UINT32 Line_GetCurrentAcm(_UINT32 hLine, _UINT32 customParam)
{
    _UINT32 rtn;

    rtn = WM_GetCurrentAcm(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam);

    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}



_UINT32 Line_ResetAcm(_UINT32 hLine, _UINT32 customParam, _CHAR8* strPasswd)
{
    _UINT32 rtn;
    
    rtn = WM_ResetAcm(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam, strPasswd);
    
    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);


}

_UINT32 Line_GetAcmMax(_UINT32 hLine, _UINT32 customParam)
{
    _UINT32 rtn;

    rtn = WM_GetAcmMax(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam);

    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 Line_SetAcmMax(_UINT32 hLine, _UINT32 customParam, _CHAR8* strAcmax, _CHAR8* strPasswd)
{
    _UINT32 rtn;
    
    rtn = WM_SetAcmMax(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam, strAcmax, strPasswd);
    
    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);


}

_UINT32 Line_GetCurCallMeter(_UINT32 hLine, _UINT32 customParam)
{
    _UINT32 rtn;

    rtn = WM_GetCurCallMeter(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam);

    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 Line_SetClock(_UINT32 hLine, _UINT32 customParam, RIL_SYSTIME *SysTime)
{
    _UINT32 rtn;
    
    rtn = WM_SetClock(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, 
                        customParam, SysTime);
    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 Line_GetExtendedError(_UINT32 hLine, _UINT32 customParam)
{
    _UINT32 rtn;

    rtn = WM_GetExtendedError(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam);

    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 Line_GetFunctionality(_UINT32 hLine, _UINT32 customParam)
{
    _UINT32 rtn;
    
    rtn = WM_GetFunctionality(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam);
    
    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 Line_GetSimVol(_UINT32 hLine, _UINT32 customParam)
{
    _UINT32 rtn;
    
    rtn = WM_GetSimVol(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam);
    
    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 Line_SetFunctionality(_UINT32 hLine, _UINT32 customParam, _CHAR8* strFun)
{
    _UINT32 rtn;
    
    rtn = WM_SetFunctionality(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam, strFun);
    
    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 Line_SetSimVol(_UINT32 hLine, _UINT32 customParam, _CHAR8* strMod)
{
    _UINT32 rtn;
    
    rtn = WM_SetQSimVol(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam, strMod);
    
    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 Line_SetRingBack(_UINT32 hLine, _UINT32 customParam, _CHAR8* strEn)
{
    _UINT32 rtn;
    
    rtn = WM_SetRingBack(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam, strEn);
    
    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 Line_SetVbatt(_UINT32 hLine, _UINT32 customParam, _CHAR8* strEn)
{
    _UINT32 rtn;
    
    rtn = WM_SetVBatt(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam, strEn);
    
    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 Line_SetNWScanMode(_UINT32 hLine, _UINT32 customParam, _CHAR8* strStr)
{
    _UINT32 rtn;
    
    rtn = WM_SetNWScanMode(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam, strStr);
    
    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 Line_SetOpenTCPUDP(_UINT32 hLine, _UINT32 customParam, _CHAR8* strStr)
{
    _UINT32 rtn;
    
    rtn = WM_SetOpenTCPUDP(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam, strStr);
    
    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 Line_InquireQiStat(_UINT32 hLine, _UINT32 customParam)
{
    _UINT32 rtn;
 
    rtn = WM_InquireQiStat(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam);

    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 Line_InquireQiSack(_UINT32 hLine, _UINT32 customParam)
{
    _UINT32 rtn;
 
    rtn = WM_InquireQiSack(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam);

    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 Line_InquireQiLocIP(_UINT32 hLine, _UINT32 customParam)
{
    _UINT32 rtn;
 
    rtn = WM_InquireQiLocIP(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam);

    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 Line_InquireQiRcvData(_UINT32 hLine, _UINT32 customParam, _CHAR8* strParam)
{
    _UINT32 rtn;
 
    rtn = WM_InquireQiRcvData(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam, strParam);

    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 Line_SendQiClose(_UINT32 hLine, _UINT32 customParam)
{
    _UINT32 rtn;
 
    rtn = WM_SendQiClose(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam);

    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 Line_InquireQIRegApp(_UINT32 hLine, _UINT32 customParam)
{
    _UINT32 rtn;
    
    rtn = WM_InquireQIRegApp(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam);
    
    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 Line_SetQIRegApp(_UINT32 hLine, _UINT32 customParam, _CHAR8* strStr)
{
    _UINT32 rtn;
    
    rtn = WM_SetQIRegApp(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam, strStr);
    
    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 Line_SetQICsgp(_UINT32 hLine, _UINT32 customParam, _CHAR8* strStr)
{
    _UINT32 rtn;
    
    rtn = WM_SetQICsgp(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam, strStr);
    
    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 Line_SetQIAct(_UINT32 hLine, _UINT32 customParam)
{
    _UINT32 rtn;
    
    rtn = WM_SetQIAct(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam);
    
    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 Line_SetQIDeact(_UINT32 hLine, _UINT32 customParam)
{
    _UINT32 rtn;
    
    rtn = WM_SetQIDeact(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam);
    
    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 Line_SetQindi(_UINT32 hLine, _UINT32 customParam, _CHAR8* strParam)
{
    _UINT32 rtn;

    rtn = WM_SetQindi(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam, strParam);

    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 Line_SetQidnsip(_UINT32 hLine, _UINT32 customParam, _CHAR8* strParam)
{
    _UINT32 rtn;

    rtn = WM_SetQidnsip(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam, strParam);

    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 Line_SetCnetScan(_UINT32 hLine, _UINT32 customParam, _CHAR8* strParam)
{
    _UINT32 rtn;

    rtn = WM_SetCnetScan(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam, strParam);

    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 Line_InquireCellLoc(_UINT32 hLine, _UINT32 customParam)
{
    _UINT32 rtn;
    
    WIRELESS_INFO("Line_InquireCellLoc:  %d SIM Existed to get location\r\n", 0);

    rtn = WM_InquireCellLoc(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam);

    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 Line_SetQFun(_UINT32 hLine, _UINT32 customParam, _CHAR8* strFun)
{
    _UINT32 rtn;
    
    rtn = WM_SetQFun(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam, strFun);
    
    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 Line_SetQmoStat(_UINT32 hLine, _UINT32 customParam, _CHAR8* strEn)
{
    _UINT32 rtn;
    
    rtn = WM_SetQMoStat(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam, strEn);
    
    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 Line_SetCCINFOEn(_UINT32 hLine, _UINT32 customParam, _CHAR8* strEn)
{
    _UINT32 rtn;
    
    rtn = WM_SetQCCINFOEn(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam, strEn);
    
    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 Line_SendSimData(_UINT32 hLine, _UINT32 customParam, _CHAR8* strSimData)
{
    _UINT32 rtn;
    
    rtn = WM_SendSimData(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam, strSimData);
    
    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 Line_SetURCCFG(_UINT32 hLine, _UINT32 customParam)
{
    _UINT32 rtn;
    
    rtn = WM_SetQURCCFG(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam);
    
    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 Line_SetQJamDet(_UINT32 hLine, _UINT32 customParam, _CHAR8* strEn)
{
    _UINT32 rtn;
    
    rtn = WM_SetQJamDet(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam, strEn);
    
    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 Line_InquireSubscriber(_UINT32 hLine, _UINT32 customParam)
{
    _UINT32 rtn;

    rtn = WM_InquireSubscriber(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam);

    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 Line_SetSIMSlot(_UINT32 hLine, _UINT32 customParam, _SHORT16 simslot)
{
    _UINT32 rtn;
    
    rtn = WM_SetSIMSlot(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam, simslot);
    
    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 Line_SetSpeakerVol(_UINT32 hLine, _UINT32 customParam, _SHORT16 volume)
{
    _UINT32 rtn;
    
    rtn = WM_SetSpeakerVol(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam, volume);
    
    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 Line_SetFreqBand(_UINT32 hLine, _UINT32 customParam, _SHORT16 bandindex)
{
    _UINT32 rtn;
    
    rtn = WM_SetFreqBand(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam, bandindex);
    
    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 Line_SetQSFRParam(_UINT32 hLine, _UINT32 customParam, _SHORT16 sfrindex)
{
    _UINT32 rtn;

    rtn = WM_SetQSFR(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam, sfrindex);

    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 Line_ReportMobileError(_UINT32 hLine, _UINT32 customParam, _CHAR8* strValue)
{
    _UINT32 rtn;

    rtn = WM_ReportMobileError(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam, strValue);

    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 Line_EnterMultiplex(_UINT32 hLine, _UINT32 customParam)
{
    _UINT32 rtn;

    rtn = WM_EnterMultiplex(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam);

    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 Line_ReadOperatorNames(_UINT32 hLine, _UINT32 customParam)
{
    _UINT32 rtn;

    rtn = WM_ReadOperatorNames(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam);

    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 Line_InquireAvaiOperator(_UINT32 hLine, _UINT32 customParam)
{
    _UINT32 rtn;

    rtn = WM_InquireAvaiOperator(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam);

    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 Line_SelectOperator(_UINT32 hLine, _UINT32 customParam, RIL_NET_MODE mode, _CHAR8* strOper)
{
    _UINT32 rtn;
    
    rtn = WM_SelectOperator(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, 
                        customParam, mode, strOper);
    
    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 Line_SetEngineerMode(_UINT32 hLine, _UINT32 customParam, _UINT32 mode, _UINT32 dump)
{
    _UINT32 rtn;
    
    rtn = WM_SetEngineerMode(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, 
                        customParam, mode, dump);
    
    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 Line_NetworkRegistration(_UINT32 hLine, _UINT32 customParam, _CHAR8* strValue)
{
    _UINT32 rtn;

    rtn = WM_NetworkRegistration(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam, strValue);

    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 Line_InquireNetworkRegStatus(_UINT32 hLine, _UINT32 customParam, _CHAR8* strValue)
{
    _UINT32 rtn;

    rtn = WM_InquireNetworkRegStatus(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam, strValue);

    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 Line_InquireGNetworkRegStatus(_UINT32 hLine, _UINT32 customParam, _CHAR8* strValue)
{
    _UINT32 rtn;

    rtn = WM_InquireGNetworkRegStatus(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam, strValue);

    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 Line_SetCharacterSet(_UINT32 hLine, _UINT32 customParam, _CHAR8* strChset)
{
    _UINT32 rtn;

    rtn = WM_SetCharacterSet(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam, strChset);

    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 Line_InquireSignalQuality(_UINT32 hLine, _UINT32 customParam)
{
    _UINT32 rtn;

    rtn = WM_InquireSignalQuality(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam);

    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

//weikk 2016.3.31 add XREG
_UINT32 Line_InquireNetType(_UINT32 hLine, _UINT32 customParam)
{
    _UINT32 rtn;

    rtn = WM_InquireNetType(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam);

    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}
//weikk 20161026 add ADC
_UINT32 Line_InquireADC(_UINT32 hLine, _UINT32 customParam)
{
    _UINT32 rtn;

    rtn = WM_InquireADC(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam);

    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}
//weikk 20160713 add MMAD
_UINT32 Line_InquireMMAD(_UINT32 hLine, _UINT32 customParam)
{
    _UINT32 rtn;

    rtn = WM_InquireMMAD(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam);

    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}


_UINT32 Line_InquireSideTone(_UINT32 hLine, _UINT32 customParam)
{
    _UINT32 rtn;

    rtn = WM_InquireSideTone(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam);

    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 Line_InquireCallStatus(_UINT32 hLine, _UINT32 customParam)
{
    _UINT32 rtn;

    rtn = WM_InquireCallStatus(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam);

    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 Line_SetSSNote(_UINT32 hLine, _UINT32 customParam, _CHAR8* strCSSI, _CHAR8* strCSSU)
{
    _UINT32 rtn;

    rtn = WM_SetSSNote(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam, strCSSI, strCSSU);

    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 Line_SendUSSD(_UINT32 hLine, _UINT32 customParam, _CHAR8* strN, _CHAR8* strStr, _CHAR8* strDcs)
{
    _UINT32 rtn;

    rtn = WM_SendUSSD(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam, strN, strStr, strDcs);

    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}


_UINT32 Line_SendUssdData(_UINT32 hLine, _UINT32 customParam, _CHAR8* strUssdString)
{
    _UINT32 rtn;

    rtn = WM_SendUssdData(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam, strUssdString);

    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 Line_SendATOrigin(_UINT32 hLine, _UINT32 customParam, _CHAR8* strATString)
{
    _UINT32 rtn;

    rtn = WM_SendATOrigin(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam, strATString);

    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 Line_CancelUssdData(_UINT32 hLine, _UINT32 customParam)
{
    _UINT32 rtn;

    rtn = WM_CancelUssdData(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam);

    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 Line_DialDTMF(_UINT32 hLine, _UINT32 customParam, _CHAR8* strDtmfStr)
{
    _UINT32 rtn;

    rtn = WM_DialDTMF(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam, strDtmfStr);

    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 Line_SetDTMFPrd(_UINT32 hLine, _UINT32 customParam, _CHAR8* strDtmfStr)
{
    _UINT32 rtn;

    rtn = WM_SetDTMFPrd(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam, strDtmfStr);

    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 Line_SetAlarm(_UINT32 hLine, _UINT32 customParam, RIL_CALA_ALARM *alarmData)
{
    _UINT32 rtn;

    rtn = WM_SetAlarm(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam, alarmData);

    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 Line_FieldTestReport(_UINT32 hLine, _UINT32 customParam, _CHAR8* strMode)
{
    _UINT32 rtn;

    rtn = WM_FieldTestReport(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam, strMode);

    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}



_UINT32 Line_SwitchOffModule(_UINT32 hLine, _UINT32 customParam)
{
    _UINT32 rtn;

    rtn = WM_SwitchOffModule(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam);

    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 Line_SetMicGainLevel(_UINT32 hLine, _UINT32 customParam, _SHORT16 volume)
{
    _UINT32 rtn;

    rtn = WM_SetMicGainLevel(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam, volume);

    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 Line_SetAudioPath(_UINT32 hLine, _UINT32 customParam, RIL_AUDIO_PATH_TYPE Mode)
{
    _UINT32 rtn;

    rtn = WM_SetAudioPath(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam, Mode);

    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 Line_SetAudioMode(_UINT32 hLine, _UINT32 customParam, RIL_AUDIO_MOD_TYPE Mode)
{
    _UINT32 rtn;

    rtn = WM_SetAudioMode(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam, Mode);

    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 Line_SetVoiceHangupCtrl(_UINT32 hLine, _UINT32 customParam, _BOOL bEnable)
{
    _UINT32 rtn;

    rtn = WM_SetVoiceHangupCtrl(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam, bEnable);

    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 Line_SetAudioMute(_UINT32 hLine, _UINT32 customParam, _BOOL bMute)
{
    _UINT32 rtn;

    rtn = WM_SetAudioMute(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam, bMute);
    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 Line_PlayTone(_UINT32 hLine, _UINT32 customParam, _USHORT16 type, _USHORT16 volume)
{
    _UINT32 rtn;

    rtn = WM_PlayTone(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam, type, volume);
    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 Line_InquireHeadsetStatus(_UINT32 hLine, _UINT32 customParam)
{
    _UINT32 rtn;

    rtn = WM_InquireHeadsetStatus(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam);

    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 Line_InquireHeadsetBtnStatus(_UINT32 hLine, _UINT32 customParam)
{
    _UINT32 rtn;

    rtn = WM_InquireHeadsetBtnStatus(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam);

    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 Line_InquireBatteryLevel(_UINT32 hLine, _UINT32 customParam)
{
    _UINT32 rtn;

    rtn = WM_InquireBatteryLevel(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam);

    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 Line_DumpBroadCast(_UINT32 hLine, _UINT32 customParam)
{
    _UINT32 rtn;

    rtn = WM_DumpBroadCast(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam);

    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 Line_SetRingerSoundLevel(_UINT32 hLine, _UINT32 customParam, _SHORT16 level)
{
    _UINT32 rtn;

    rtn = WM_SetRingerSoundLevel(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam, level);

    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 Line_SetAudioOutput(_UINT32 hLine, _UINT32 customParam, _CHAR8* strAudioParam)
{
    _UINT32 rtn;

    rtn = WM_SetAudioOutput(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam, strAudioParam);

    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 Line_QueryModuleState(_UINT32 hLine, _UINT32 customParam)
{
    _UINT32 rtn;

    rtn = WM_QueryModuleState(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam);

    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 Line_CheckLatestURCEvent(_UINT32 hLine, _UINT32 customParam, _CHAR8* strIndex)
{
    _UINT32 rtn;

    rtn = WM_CheckLatestURCEvent(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam, strIndex);

    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 Line_ControlURCIndication(_UINT32 hLine, _UINT32 customParam, _CHAR8* strValue)
{
    _UINT32 rtn;

    rtn = WM_ControlURCIndication(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam, strValue);

    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 Line_ControlModuleDriver(_UINT32 hLine, _UINT32 customParam, _UCHAR8 mode)
{
    _UINT32 rtn;

    rtn = WM_ControlModuleDriver(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam, mode);

    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 Line_QueryURCIndication(_UINT32 hLine, _UINT32 customParam)
{
    _UINT32 rtn;

    rtn = WM_QueryURCIndication(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam);

    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 Line_MobileEquipmentEvent(_UINT32 hLine, _UINT32 customParam, _CHAR8* strValue)
{
    _UINT32 rtn;

    rtn = WM_MobileEquipmentEvent(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam, strValue);

    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 Line_SetMSStorageType(_UINT32 hLine, _UINT32 customParam, _CHAR8* strService)
{
    _UINT32 rtn;

    rtn = WM_SetMSStorageType(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam, strService);

    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 Line_SetModuleSleep(_UINT32 hLine, _UINT32 customParam,_UCHAR8 enable)
{
    _UINT32 rtn;

    rtn = WM_SetModuleSleep(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam, enable);

    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 Line_GetModemVersionNum(_UINT32 hLine, _UINT32 customParam)
{
    _UINT32 rtn;

    rtn = WM_GetModemVersionNum(((HLINE)hLine)->hATServer->hWM,
                                 (_UINT32)hLine, eDEVICECOMMON, customParam);

    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 Line_SetChargingCurrent(_UINT32 hLine, _UINT32 customParam, _CHAR8* strStep)
{
    _UINT32 rtn;

    rtn = WM_SetChargingCurrent(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, 
                                 eDEVICECOMMON, customParam, strStep);

    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 Line_ConfigURCMode(_UINT32 hLine, _UINT32 customParam, RIL_URC_MODE mode)
{
    _UINT32 rtn;

    rtn = WM_ConfigURCMode(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine,
                            eDEVICECOMMON, customParam, mode);

    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 Line_XDRVURCControl(_UINT32 hLine, _UINT32 customParam, RIL_sXDRV_TYPE *xdrvType)
{
    _UINT32 rtn;

    rtn = WM_XDRVURCControl(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, 
                            eDEVICECOMMON, customParam, xdrvType);

    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 Line_XDRVAudioControl(_UINT32 hLine, _UINT32 customParam, 
                RIL_XDRV_AUDIO_CTRL_TYPE audCtrlType, _CHAR8* strValue)
{
    _UINT32 rtn;

    rtn = WM_XDRVAudioControl(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, 
                        eDEVICECOMMON, customParam, audCtrlType, strValue);

    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 Line_SetTraceOutput(_UINT32 hLine, _UINT32 customParam, _BOOL bOnOff)
{
    _UINT32 rtn;

    rtn = WM_SetTraceOutput(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam, bOnOff);

    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 Line_GetModemExcLog(_UINT32 hLine, _UINT32 customParam)
{
    _UINT32 rtn;

    rtn = WM_GetModemExcLog(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam);

    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 Line_XDRVControlAudioLoopTest(_UINT32 hLine, _UINT32 customParam, _BOOL enable)
{
    _UINT32 rtn;

    rtn = WM_XDRVControlAudioLoopTest(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam, enable);

    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 Line_FacilityLock(_UINT32 hLine, _UINT32 customParam, RIL_FACILITY fac, _BOOL bLock, _CHAR8* strPasswd, RIL_DATA_CLASS eClass)
{
    _UINT32 rtn;

    rtn = WM_FacilityLock(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, 
                        customParam, fac, bLock, strPasswd, eClass);

    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 Line_InqFacilityLock(_UINT32 hLine, _UINT32 customParam, RIL_FACILITY fac, RIL_DATA_CLASS eClass)
{
    _UINT32 rtn;

    rtn = WM_InqFacilityLock(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, 
                        customParam, fac, eClass);

    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 Line_CallingLineId(_UINT32 hLine, _UINT32 customParam, _CHAR8* strValue)
{
    _UINT32 rtn;

    rtn = WM_CallingLineId(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, 
                            eDEVICECOMMON, customParam, strValue);

    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 Line_CheckCLIP(_UINT32 hLine, _UINT32 customParam)
{
    _UINT32 rtn;

    rtn = WM_CheckCLIP(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine,
                        eDEVICECOMMON, customParam);

    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 Line_CheckCLIR(_UINT32 hLine, _UINT32 customParam)
{
    _UINT32 rtn;

    rtn = WM_CheckCLIR(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, 
                        eDEVICECOMMON, customParam);

    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 Line_SetCLIRActive(_UINT32 hLine, _UINT32 customParam)
{
    _UINT32 rtn;

    rtn = WM_SetCLIRActive(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, 
                        eDEVICECOMMON, customParam);

    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 Line_SetCLIRDisable(_UINT32 hLine, _UINT32 customParam)
{
    _UINT32 rtn;

    rtn = WM_SetCLIRDisable(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, 
                        eDEVICECOMMON, customParam);

    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 Line_DailCLIR(_UINT32 hLine, _UINT32 customParam, _CHAR8* strValue)
{
    _UINT32 rtn;

    rtn = WM_DailCLIR(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, 
                        eDEVICECOMMON, customParam, strValue);

    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 Line_CheckCOLR(_UINT32 hLine, _UINT32 customParam)
{
    _UINT32 rtn;

    rtn = WM_CheckCOLR(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, 
                        eDEVICECOMMON, customParam);

    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 Line_InquireCOLP(_UINT32 hLine, _UINT32 customParam)
{
    _UINT32 rtn;

    rtn = WM_InquireCOLP(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam);

    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 Line_SetColpStatus(_UINT32 hLine, _UINT32 customParam, _CHAR8* strValue)
{
    _UINT32 rtn;

    rtn = WM_SetColpStatus(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, 
                                customParam, strValue);

    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 Line_SetBatteryParam(_UINT32 hLine, _UINT32 customParam, _CHAR8* strParam)
{
    _UINT32 rtn;

    rtn = WM_BatteryCtrl(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam, strParam);

    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 Line_SetToneDetParam(_UINT32 hLine, _UINT32 customParam, _CHAR8* strParam)
{
    _UINT32 rtn;

    rtn = WM_ToneDetCtrl(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam, strParam);

    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 Line_SetDAIParam(_UINT32 hLine, _UINT32 customParam, _CHAR8* strParam)
{
    _UINT32 rtn;

    rtn = WM_DAICtrl(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam, strParam);

    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 Line_SelectVoiceGain(_UINT32 hLine, _UINT32 customParam, _CHAR8* strParam)
{
    _UINT32 rtn;

    rtn = WM_SelectVoiceGain(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam, strParam);

    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 Line_SideToneModify(_UINT32 hLine, _UINT32 customParam, _CHAR8* strParam)
{
    _UINT32 rtn;

    rtn = WM_SideToneModify(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam, strParam);

    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 Line_SaveConfigure(_UINT32 hLine, _UINT32 customParam)
{
    _UINT32 rtn;

    rtn = WM_SaveConfigure(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam);

    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 Line_ProccessRecv(_UINT32 ChannelID, const _CHAR8* buf, _INT32 size)
{
    E_SVRCALL type;    
        
    type = *(E_SVRCALL *)buf;
    switch(type)
    {
        case MakeCall:
        {
            T_MAKECALL* req;

            req = (T_MAKECALL *)buf;
            return CallManager_MakeCall(req->hLine, req->lpszAddress, req->dwType, req->dwOptions, req->hRequest);
        }
        case ECALLREJECTCALL:
        {
            T_OPERATECALL* req;
            req = (T_OPERATECALL *)buf;
            return CallManager_RejectCall(req->hLine, (HCALL)req->hCall, req->hRequest);
        }
        case ECALLANSWERCALL:
        {
            T_OPERATECALL* req;
            req = (T_OPERATECALL *)buf;
            return CallManager_AnswerCall(req->hLine, (HCALL)req->hCall, req->hRequest);
        }
        case ECALLHANGUPCALL:
        {
            T_OPERATECALL* req;
            req = (T_OPERATECALL *)buf;
            return CallManager_HangupCall(req->hLine, (HCALL)req->hCall, req->hRequest);
        }
        case ECALLCANCELDIAL:
        {
            T_OPERATECALL* req;
            req = (T_OPERATECALL *)buf;
            return CallManager_CancelDial(req->hLine, (HCALL)req->hCall, req->hRequest);
        }
        case ECALLSWAPCALL:
        {
            T_OPERATECALL* req;
            req = (T_OPERATECALL *)buf;
            return CallManager_SwapCall(req->hLine, (HCALL)req->hCall, req->hRequest);
        }
        case ECALLCONFERENCE:
        {
            T_OPERATECALL* req;
            req = (T_OPERATECALL *)buf;
            return CallManager_Conference(req->hLine, (HCALL)req->hCall, req->hRequest);
        }
        case ECALLENDSINGLE:
        {
            T_OPERATECALL* req;
            req = (T_OPERATECALL *)buf;
            return CallManager_EndSingle(req->hLine, (HCALL)req->hCall, req->hRequest);
        }
        case ECALLENDACTIVE:
        {
            T_OPERATECALL* req;
            req = (T_OPERATECALL *)buf;
            return CallManager_EndActive(req->hLine, (HCALL)req->hCall, req->hRequest);
        }
        case ECALLENDHOLD:
        {
            T_OPERATECALL* req;
            req = (T_OPERATECALL *)buf;
            return CallManager_EndHold(req->hLine, (HCALL)req->hCall, req->hRequest);
        }
        case ECALLSPLITFROMCONFER:
        {
            T_OPERATECALL* req;
            req = (T_OPERATECALL *)buf;
            return CallManager_SplitFromConference(req->hLine, (HCALL)req->hCall, req->hRequest);
        }
        case ECALLCURCALLLIST:
        {
            T_OPERATECALL* req;
            req = (T_OPERATECALL *)buf;
            return CallManager_ListCall(req->hLine, req->hRequest);
        }
        case ECALLGETCALLINFO:
        {
            T_OPERATECALL* req;
            req = (T_OPERATECALL *)buf;
            return CallManager_GetCallInfo(req->hLine, (HCALL)req->hCall);
        }
        case ECALLGETCALLQty:
        {
            T_OPERATECALL* req;
            req = (T_OPERATECALL *)buf;
            return CallManager_GetCallQty(req->hLine, (RIL_CALL_STATUS)req->hCall);
        }
        case CALLFORWARD:
        {
            T_CALLFORWARD* req;

            req = (T_CALLFORWARD*)buf;
            return CallManager_CallForward(req->hLine, req->hRequest, req->cfData);
        }
        case INQUIRECALLFWD:
        {
            T_TWOU32PARAM* req;
            req = (T_TWOU32PARAM*)buf;
            return CallManager_InquireCallFwd(req->hLine, req->hRequest, (RIL_CCFC_REAS)req->value1, (RIL_DATA_CLASS)req->value2);
        }
        case SETCALLWAIT:
        {
            T_CALLWAIT* req;

            req = (T_CALLWAIT*)buf;
            return CallManager_SetCallWait(req->hLine, req->hRequest, req->bURCEnable, req->bEnable, req->eClass);
        }
        case INQUIRECALLWAIT:
        {
            T_INQUIRECALLWAIT* req;

            req = (T_INQUIRECALLWAIT*)buf;
            return CallManager_InquireCallWait(req->hLine, req->hRequest, req->eClass);
        }
        case ECALLSENDMSG:
        {
            T_SENDMSG* req;
            req = (T_SENDMSG *)buf;
            return MSGManager_SendMSG(req->hLine, req->pPDU, req->option, req->hRequest);
        }
        case EQISENDDATA:
        {
            T_SENDMSG* req;
            req = (T_SENDMSG *)buf;
            return MSGManager_Qisend(req->hLine, req->pPDU, req->option, req->hRequest);
        }        
        case ECALLSENDMSG_MC323:
        {
            T_SENDMSG* req;
            req = (T_SENDMSG *)buf;
            return MSGManager_SendMSG_MC323(req->hLine, req->pPDU, req->option, req->hRequest);
        }        
        case ECALLSENDMSG_CDMA:
        {
            T_SENDMSG* req;
            req = (T_SENDMSG *)buf;
            return MSGManager_SendMSG_CDMA(req->hLine, req->pPDU, req->option, req->hRequest);
        }
        case ECALLSENDMSGBYTXT:
        {
            T_SENDMSG* req;
            req = (T_SENDMSG *)buf;
            return MSGManager_SendMSGByTxt(req->hLine, req->pPDU, req->PDULen, req->pNumber, req->hRequest);
        }        
        case ECALLREADMSG:
        {
            T_ONEU16PARAM* req;
            req = (T_ONEU16PARAM *)buf;
            return MSGManager_ReadSMS(req->hLine, req->value, req->hRequest);
        }
        case ECALLREADMSGBYTXT:
        {
            T_ONEU16PARAM* req;
            req = (T_ONEU16PARAM *)buf;
            return MSGManager_ReadSMS_ByTxt(req->hLine, req->value, req->hRequest);
        }
        case ECALLREADMSG_CDMA:
        {
            T_ONEU16PARAM* req;
            req = (T_ONEU16PARAM *)buf;
            return MSGManager_ReadSMS_CDMA(req->hLine, req->value, req->hRequest);
        }        
        case ECALLSAVEMSG:
        {
            T_SAVEMSG* req;
            req = (T_SAVEMSG*)buf;
            return MSGManager_SaveMSG(req->hLine, req->pPDU, req->option, req->read_flag, req->PDUlen, req->hRequest);
        }
        case ECALLREADMSGUNCHGSTAT:
        {
            T_ONEU16PARAM* req;
            req = (T_ONEU16PARAM *)buf;
            return MSGManager_ReadSMSUnchgStat(req->hLine, req->value, req->hRequest);
        }
        case ECALLDELETEMSG:
        {
            T_ONEU16PARAM* req;
            req = (T_ONEU16PARAM *)buf;
            return MSGManager_DeleteSMS(req->hLine, req->value, req->hRequest);
        }
        case INQUIRESMSSTORAGE:
        {
            T_ONEU32PARAM* req;

            req = (T_ONEU32PARAM*)buf;
            return MSGManager_InquireSmsStorage(req->hLine, req->hRequest, (RIL_MSG_STORAGE)req->value);
        }
        case LISTSMS:
        {
            T_ONEU32PARAM* req;

            req = (T_ONEU32PARAM*)buf;
            return MSGManager_ListSMS(req->hLine, req->hRequest, (RIL_SMS_MODE)req->value);
        }
        case INQUIREMSGCENTER:
        {
            T_BASICPARAM* req;

            req = (T_BASICPARAM*)buf;
            return MSGManager_InquireSmsCenter(req->hLine, req->hRequest);
        }
        case NEWMSGSTOREMODE:
        {
            T_ONEU32PARAM* req;

            req = (T_ONEU32PARAM*)buf;
            return MSGManager_NewMsgStoreMode(req->hLine, req->hRequest, (RIL_STORENEWMSG_MODE)req->value);
        }
        case SETMSGCENTER:
        {
            T_ONESTRPARAM* req;

            req = (T_ONESTRPARAM*)buf;
            return MSGManager_SetSmsCenter(req->hLine, req->hRequest, req->strValue);
        }
        case SETEXTSMSSTORAGESTATUS:
        {
            T_ONEU8PARAM* req;

            req = (T_ONEU8PARAM*)buf;
            return MSGManager_SetExtSMSStorageStatus(req->hLine, req->hRequest, req->uValue);
        }
        case SELECTBROADCAST:
        {
            T_THREESTRPARAM* req;

            req = (T_THREESTRPARAM*)buf;
            return MSGManager_SelectBroadcast(req->hLine, req->hRequest, 
                                            req->strValue1, req->strValue2, req->strValue3);
        }
        case ESTK_SETPROFILE:
        {
            T_BASICPARAM* req;
            req = (T_BASICPARAM *)buf;
            return STKManager_SetProfile(req->hLine, req->hRequest);
        }
        case ESTK_SETAUTORSP:
        {
            T_BASICPARAM* req;
            req = (T_BASICPARAM *)buf;
            return STKManager_SetAutoRsp(req->hLine, req->hRequest);
        }
        case ESTK_SETTR:
        {
            T_BASICPARAM* req;
            req = (T_BASICPARAM *)buf;
            return STKManager_SetTR(req->hLine, req->hRequest);
        }
        case ESTK_SELECTMAINMENU:
        {
            T_ONES16PARAM* req;
            req = (T_ONES16PARAM *)buf;
            return STKManager_SelectMainMenu(req->hLine, req->value, req->hRequest);
        }
        case ESTK_RESPONSE:
        {
            T_ONEU32PARAM* req;
            req = (T_ONEU32PARAM *)buf;
            printf("!!!ESTK_RESPONSE\r\n");
            return STKManager_Response(req->hLine, (RIL_RES_PAR*)req->value, req->hRequest);
        }
        case INQUIREGPRSSTATE:
        {
            T_BASICPARAM* req;
            req = (T_BASICPARAM *)buf;
            return DataConn_InquireGprsState(req->hLine, req->hRequest);
        }
        case DATACONN_WITHGPRS:
        {
            T_DATACONN* req;
            req = (T_DATACONN *)buf;
            return DataConn_EstablishWithGPRS(req->hLine, req->user, req->bMUX, req->hRequest);
        }
        case DATACONN_WITHCSD:
        {
            T_DATACONN* req;
            req = (T_DATACONN *)buf;
            return DataConn_EstablishWithCSD(req->hLine, req->dial, req->user, req->pwd, req->bMUX, req->hRequest);
        }
        case DATACONN_RELEASE:
        {
            T_DATACONN* req;
            req = (T_DATACONN *)buf;
            return DataConn_Release(req->hLine, req->bMUX, req->step, req->hRequest);
        }
        case ECALLOPENLINE:
        {
            T_OPENLINE* req;

            req = (T_OPENLINE *)buf;
            return Line_Open(ChannelID, req->eventType, req->hATServer, req->hRIL);
        }        
        case ECALLCLOSELINE:
        {
            T_CLOSELINE* req;

            req = (T_CLOSELINE *)buf;
            return Line_Close((HLINE)req->hLine);
        }        
        case ECALLATTENTION:
        {
            T_BASICPARAM* req;

            req = (T_BASICPARAM*)buf;
            return Line_Attention(req->hLine, req->hRequest);
        }
        case GETSLEEPSTATE:
        {
            T_BASICPARAM* req;

            req = (T_BASICPARAM*)buf;
            return Line_GetSleepState(req->hLine);
        }        
        case ECALLSETUP:
        {
            T_SETUP* req;

            req = (T_SETUP*)buf;
            return ATServer_Setup(req->ChnlNum, req->ChID, req->value);
        }
        case ECALLSTOP:
        {
            return ATServer_Stop();
        }
        case ECALLUPDATECHNL:
        {
            T_SETUP* req;

            req = (T_SETUP*)buf;
            return ATServer_UpdateChannel(((HLINE)req->value)->hATServer, req->ChnlNum, req->ChID);
        }
        case INQUIRECLOCK:
        {
            T_BASICPARAM* req;

            req = (T_BASICPARAM*)buf;
            return Line_InquireClock(req->hLine, req->hRequest);
        }        
        case INQUIRECUROPERATOR:
        {
            T_BASICPARAM* req;

            req = (T_BASICPARAM*)buf;
            return Line_InquireCurOperator(req->hLine, req->hRequest);
        }    
        case INQUIREBSINFO:
        {
            T_BASICPARAM* req;

            req = (T_BASICPARAM*)buf;
            return Line_InquireBSInfo(req->hLine, req->hRequest);
        }        
        case SETCOMMANDECHO:
        {
            T_ONEU32PARAM* req;

            req = (T_ONEU32PARAM*)buf;
            return Line_SetCommandEcho(req->hLine, req->hRequest, (_BOOL)req->value);
        }
        case SENDAT:
        {
            T_ONEU32PARAM* req;

            req = (T_ONEU32PARAM*)buf;
            return Line_SendAT(req->hLine, req->hRequest, (_BOOL)req->value);
        }
        case PWRCTRL:
        {
            T_ONEU32PARAM* req;

            req = (T_ONEU32PARAM*)buf;
            return Line_PWRCTRL(req->hLine, req->hRequest, (_BOOL)req->value);
        }
         case GETPRODUCTID:
        {
            T_ONESTRPARAM* req;

            req = (T_ONESTRPARAM*)buf;
            return Line_GetProductID(req->hLine, req->hRequest, req->strValue);
        }
        case SETRESULTPRESENTATION:
        {
            T_ONESTRPARAM* req;

            req = (T_ONESTRPARAM*)buf;
            return Line_SetResultPresentation(req->hLine, req->hRequest, req->strValue);
        }
        case SETRESULTFORMAT:
        {
            T_ONESTRPARAM* req;

            req = (T_ONESTRPARAM*)buf;
            return Line_SetResultFormat(req->hLine, req->hRequest, req->strValue);
        }
        case SETMANUFDEFAULT:
        {
            T_BASICPARAM* req;

            req = (T_BASICPARAM*)buf;
            return Line_SetManufDefault(req->hLine, req->hRequest);
        }        
        case INQUIRECCID:
        {
            T_BASICPARAM* req;

            req = (T_BASICPARAM*)buf;
            return Line_InquireCCID(req->hLine, req->hRequest);
        }
        case INQUIREIMSI:
        {
            T_BASICPARAM* req;

            req = (T_BASICPARAM*)buf;
            return Line_InquireIMSI(req->hLine, req->hRequest);
        }        
        case INQUIREIMEI:
        {
            T_BASICPARAM* req;

            req = (T_BASICPARAM*)buf;
            return Line_InquireIMEI(req->hLine, req->hRequest);
        }
        case SETIMEI:
        {
            T_ONESTRPARAM* req;

            req = (T_ONESTRPARAM*)buf;
            return Line_SetIMEI(req->hLine, req->hRequest, req->strValue);
        }    
        case LOCKBS:
        {
            T_ONESTRPARAM* req;

            req = (T_ONESTRPARAM*)buf;
            return Line_LockBS(req->hLine, req->hRequest, req->strValue);
        }        
        case SETBAUDRATE:
        {
            T_ONESTRPARAM* req;

            req = (T_ONESTRPARAM*)buf;
            return Line_SetBaudrate(req->hLine, req->hRequest, req->strValue);
        }
        case INQUIREBAND:
        {
            T_BASICPARAM* req;

            req = (T_BASICPARAM*)buf;
            return Line_InquireBand(req->hLine, req->hRequest);
        }
        case SETNETBAND:
        {
            T_ONESTRPARAM* req;

            req = (T_ONESTRPARAM*)buf;
            return Line_SetNetBand(req->hLine, req->hRequest, req->strValue);
        }
        case GETCURRENTACM:
        {
            T_BASICPARAM* req;

            req = (T_BASICPARAM*)buf;
            return Line_GetCurrentAcm(req->hLine, req->hRequest);
        }
        case RESETACM:
        {
            T_ONESTRPARAM* req;

            req = (T_ONESTRPARAM*)buf;
            return Line_ResetAcm(req->hLine, req->hRequest, req->strValue);
        }
        case GETACMMAX:
        {
            T_BASICPARAM* req;

            req = (T_BASICPARAM*)buf;
            return Line_GetAcmMax(req->hLine, req->hRequest);
        }
        case SETACMMAX:
        {
            T_TWOSTRPARAM* req;

            req = (T_TWOSTRPARAM*)buf;
            return Line_SetAcmMax(req->hLine, req->hRequest, req->strValue1, req->strValue2);
        }
        case GETCURCALLMETER:
        {
            T_BASICPARAM* req;

            req = (T_BASICPARAM*)buf;
            return Line_GetCurCallMeter(req->hLine, req->hRequest);
        }
        case SETCLOCK:
        {
            T_ONEU32PARAM* req;

            req = (T_ONEU32PARAM*)buf;
            return Line_SetClock(req->hLine, req->hRequest, (RIL_SYSTIME *)req->value);
        }
        case GETEXTENDEDERROR:
        {
            T_BASICPARAM* req;

            req = (T_BASICPARAM*)buf;
            return Line_GetExtendedError(req->hLine, req->hRequest);
        }
        case GETFUNCTIONALITY:
        {
            T_ONESTRPARAM* req;

            req = (T_ONESTRPARAM*)buf;
            return Line_GetFunctionality(req->hLine, req->hRequest);
        }
        case GETSIMVOL:
        {
            T_ONESTRPARAM* req;

            req = (T_ONESTRPARAM*)buf;
            return Line_GetSimVol(req->hLine, req->hRequest);
        }        
        case SETFUNCTIONALITY:
        {
            T_ONESTRPARAM* req;

            req = (T_ONESTRPARAM*)buf;
            return Line_SetFunctionality(req->hLine, req->hRequest, req->strValue);
        }
        case SETSIMVOL:
        {
            T_ONESTRPARAM* req;

            req = (T_ONESTRPARAM*)buf;
            return Line_SetSimVol(req->hLine, req->hRequest, req->strValue);
        }
        case SETRINGBACK:
        {
            T_ONESTRPARAM* req;

            req = (T_ONESTRPARAM*)buf;
            return Line_SetRingBack(req->hLine, req->hRequest, req->strValue);
        }    
        case SETVBATT:
        {
            T_ONESTRPARAM* req;

            req = (T_ONESTRPARAM*)buf;
            return Line_SetVbatt(req->hLine, req->hRequest, req->strValue);
        }        
        case SETNWSCANMODE:
        {
            T_ONESTRPARAM* req;

            req = (T_ONESTRPARAM*)buf;
            return Line_SetNWScanMode(req->hLine, req->hRequest, req->strValue);
        }        
        case SETOPENTCPUDP:
        {
            T_ONESTRPARAM* req;

            req = (T_ONESTRPARAM*)buf;
            return Line_SetOpenTCPUDP(req->hLine, req->hRequest, req->strValue);
        }        
        case INQUIREQISTAT:
        {
            T_ONESTRPARAM* req;

            req = (T_ONESTRPARAM*)buf;
            return Line_InquireQiStat(req->hLine, req->hRequest);
        }
        case INQUIREQISACK:
        {
            T_ONESTRPARAM* req;

            req = (T_ONESTRPARAM*)buf;
            return Line_InquireQiSack(req->hLine, req->hRequest);
        }
        case INQUIREQILOCIP:
        {
            T_ONESTRPARAM* req;

            req = (T_ONESTRPARAM*)buf;
            return Line_InquireQiLocIP(req->hLine, req->hRequest);
        }
        case INQUIREQIRCVDATA:
        {
            T_ONESTRPARAM* req;

            req = (T_ONESTRPARAM*)buf;
            return Line_InquireQiRcvData(req->hLine, req->hRequest, req->strValue);
        }
        case SETQICLOSE:
        {
            T_ONESTRPARAM* req;

            req = (T_ONESTRPARAM*)buf;
            return Line_SendQiClose(req->hLine, req->hRequest);
        }
        case INQUIREQIREGAPP:
        {
            T_ONESTRPARAM* req;

            req = (T_ONESTRPARAM*)buf;
            return Line_InquireQIRegApp(req->hLine, req->hRequest);
        }
        case SETQIREGAPP:
        {
            T_ONESTRPARAM* req;

            req = (T_ONESTRPARAM*)buf;
            return Line_SetQIRegApp(req->hLine, req->hRequest, req->strValue);
        }
        case SETQICSGP:
        {
            T_ONESTRPARAM* req;

            req = (T_ONESTRPARAM*)buf;
            return Line_SetQICsgp(req->hLine, req->hRequest, req->strValue);
        }
        case SETQIACT:
        {
            T_ONESTRPARAM* req;

            req = (T_ONESTRPARAM*)buf;
            return Line_SetQIAct(req->hLine, req->hRequest);
        }        
        case SETQIDEACT:
        {
            T_ONESTRPARAM* req;

            req = (T_ONESTRPARAM*)buf;
            return Line_SetQIDeact(req->hLine, req->hRequest);
        }    
        case SETQINDI:
        {
            T_ONESTRPARAM* req;

            req = (T_ONESTRPARAM*)buf;
            return Line_SetQindi(req->hLine, req->hRequest, req->strValue);
        }        
        case SETQIDNSIP:
        {
            T_ONESTRPARAM* req;

            req = (T_ONESTRPARAM*)buf;
            return Line_SetQidnsip(req->hLine, req->hRequest, req->strValue);
        }
        case SETCNETSCAN:
        {
            T_ONESTRPARAM* req;

            req = (T_ONESTRPARAM*)buf;
            return Line_SetCnetScan(req->hLine, req->hRequest, req->strValue);
        } 
        case INQUIRECELLLOC:    
        {
            T_BASICPARAM* req;
            
            req = (T_BASICPARAM*)buf;
            return Line_InquireCellLoc(req->hLine, req->hRequest);
        } 
        case SETQFUN:
        {
            T_ONESTRPARAM* req;

            req = (T_ONESTRPARAM*)buf;
            return Line_SetQFun(req->hLine, req->hRequest, req->strValue);
        }
        case SETQMOSTAT:
        {
            T_ONESTRPARAM* req;

            req = (T_ONESTRPARAM*)buf;
            return Line_SetQmoStat(req->hLine, req->hRequest, req->strValue);
        }
        case SETQCCINFOEN:
        {
            T_ONESTRPARAM* req;

            req = (T_ONESTRPARAM*)buf;
            return Line_SetCCINFOEn(req->hLine, req->hRequest, req->strValue);
        }
        case SETFSENDSIMDATA:
        {
            T_ONESTRPARAM* req;

            req = (T_ONESTRPARAM*)buf;
            return Line_SendSimData(req->hLine, req->hRequest, req->strValue);
        }        
        case SETQURCCFG:
        {
            T_ONESTRPARAM* req;

            req = (T_ONESTRPARAM*)buf;
            return Line_SetURCCFG(req->hLine, req->hRequest);
        }
        case SETQJAMDET:
        {
            T_ONESTRPARAM* req;

            req = (T_ONESTRPARAM*)buf;
            return Line_SetQJamDet(req->hLine, req->hRequest, req->strValue);
        }
        case INQUIRESUBSCRIBER:
        {
            T_BASICPARAM* req;

            req = (T_BASICPARAM*)buf;
            return Line_InquireSubscriber(req->hLine, req->hRequest);
        }
        case SETSIMSLOT:
        {
            T_ONES16PARAM* req;

            req = (T_ONES16PARAM*)buf;
            return Line_SetSIMSlot(req->hLine, req->hRequest, req->value);
        }
        case SETSPEAKERVOL:
        {
            T_ONES16PARAM* req;

            req = (T_ONES16PARAM*)buf;
            return Line_SetSpeakerVol(req->hLine, req->hRequest, req->value);
        }
        case SETFREQBAND:
        {
            T_ONES16PARAM* req;

            req = (T_ONES16PARAM*)buf;
            return Line_SetFreqBand(req->hLine, req->hRequest, req->value);
        }    
        case SETQSFR:
        {
            T_ONES16PARAM* req;

            req = (T_ONES16PARAM*)buf;
            return Line_SetQSFRParam(req->hLine, req->hRequest, req->value);
        }        
        case REPORTMOBILERROR:
        {
            T_ONESTRPARAM* req;

            req = (T_ONESTRPARAM*)buf;
            return Line_ReportMobileError(req->hLine, req->hRequest, req->strValue);
        }
        case ENTERMULTIPLEX:
        {
            T_BASICPARAM* req;

            req = (T_BASICPARAM*)buf;
            return Line_EnterMultiplex(req->hLine, req->hRequest);
        }
        case READOPERATORNAMES:
        {
            T_BASICPARAM* req;

            req = (T_BASICPARAM*)buf;
            return Line_ReadOperatorNames(req->hLine, req->hRequest);
        }
        case INQUIREAVAIOPERATOR:
        {
            T_BASICPARAM* req;

            req = (T_BASICPARAM*)buf;
            return Line_InquireAvaiOperator(req->hLine, req->hRequest);
        }
        case SELECTOPERATOR:
        {
            T_TWOU32PARAM* req;

            req = (T_TWOU32PARAM*)buf;
            return Line_SelectOperator(req->hLine, req->hRequest, 
                (RIL_NET_MODE)req->value1, (_CHAR8*)req->value2);
        }
        case SETENGMODE:
        {
            T_TWOU32PARAM* req;

            req = (T_TWOU32PARAM*)buf;
            return Line_SetEngineerMode(req->hLine, req->hRequest, 
                req->value1, req->value2);
        }        
        case INQUIREPINSTATUS:
        {
            T_BASICPARAM* req;

            req = (T_BASICPARAM*)buf;
            return SIM_InquirePinStatus(req->hLine, req->hRequest);
        }
        case INQUIREINITSTATUS:
        {
            T_BASICPARAM* req;

            req = (T_BASICPARAM*)buf;
            return SIM_InquireInitStatus(req->hLine, req->hRequest);
        }        
        case ENTERPIN:
        {
            T_TWOSTRPARAM* req;
            
            req = (T_TWOSTRPARAM*)buf;
            return SIM_EnterPin(req->hLine, req->hRequest, req->strValue1, req->strValue2);
        }
        case CHANGEPASSWORD:
        {
            T_THREEU32PARAM* req;
            
            req = (T_THREEU32PARAM*)buf;
            return SIM_ChangePassword(req->hLine, req->hRequest, (RIL_FACILITY)req->value1, (_CHAR8*)req->value2, (_CHAR8*)req->value3);
        }
        case INQUIREPINCOUNT:
        {
            T_ONEU8PARAM* req;
            
            req = (T_ONEU8PARAM*)buf;
            return SIM_InquirePinCount(req->hLine, req->hRequest,req->uValue);
        }
        case INQUIREPUKCOUNT:
        {
            T_BASICPARAM* req;
            
            req = (T_BASICPARAM*)buf;
            return SIM_InquirePukCount(req->hLine, req->hRequest);
        }
        case INITBEFOREPIN:
        {
            T_BASICPARAM* req;
            
            req = (T_BASICPARAM*)buf;
            return SIM_InitBeforePIN(req->hLine, req->hRequest);
        }
        case INITAFTERPIN:
        {
            T_BASICPARAM* req;
            
            req = (T_BASICPARAM*)buf;
            return SIM_InitAfterPIN(req->hLine, req->hRequest);
        }
        case IGNITEMODULE:
        {
            T_BASICPARAM* req;
            
            req = (T_BASICPARAM*)buf;
            return ATServer_IgniteModule(req->hLine, req->hRequest);
        }
        case RESETMODULE:
        {
            T_BASICPARAM* req;
            
            req = (T_BASICPARAM*)buf;
            return ATServer_ResetModule(req->hLine, req->hRequest);
        }
        case WMEXCEPTIONCALLBACK:
        {
            T_BASICPARAM* req;
            
            req = (T_BASICPARAM*)buf;
            return SIM_WMExceptionCallback(req->hLine, (RIL_EXCEPTION_TYPE)req->hRequest);
        }            
        case NETWORKREGISTRATION:
        {
            T_ONESTRPARAM* req;
            
            req = (T_ONESTRPARAM*)buf;
            return Line_NetworkRegistration(req->hLine, req->hRequest, req->strValue);
        }    
        case INQUIRENETREGSTATUS:
        {
            T_ONESTRPARAM* req;
            
            req = (T_ONESTRPARAM*)buf;
            return Line_InquireNetworkRegStatus(req->hLine, req->hRequest, req->strValue);
        }   
        case INQUIREGNETREGSTATUS:
        {
            T_ONESTRPARAM* req;
            
            req = (T_ONESTRPARAM*)buf;
            return Line_InquireGNetworkRegStatus(req->hLine, req->hRequest, req->strValue);
        }        
        case SETCHARACTERSET:
        {
            T_ONESTRPARAM* req;
            
            req = (T_ONESTRPARAM*)buf;
            return Line_SetCharacterSet(req->hLine, req->hRequest, req->strValue);
        }        
        case INQUIRESIGNALQUALITY:
        {
            T_BASICPARAM* req;
            
            req = (T_BASICPARAM*)buf;
            return Line_InquireSignalQuality(req->hLine, req->hRequest);
        }
        case INQUIRENETTYPE:    //weikk 20160331 add xreg
        {
            T_BASICPARAM* req;
            
            req = (T_BASICPARAM*)buf;
            return Line_InquireNetType(req->hLine, req->hRequest);
        } 
        case INQUIREADC:    //weikk 20161026 add ADC
        {
            T_BASICPARAM* req;
            
            req = (T_BASICPARAM*)buf;
            return Line_InquireADC(req->hLine, req->hRequest);
        } 
        case INQUIREMMAD:    //weikk 20160713 add mmad
        {
            T_BASICPARAM* req;
            
            req = (T_BASICPARAM*)buf;
            return Line_InquireMMAD(req->hLine, req->hRequest);
        } 
        case INQUIRESIDETONE:
        {
            T_BASICPARAM* req;
            
            req = (T_BASICPARAM*)buf;
            return Line_InquireSideTone(req->hLine, req->hRequest);
        }    
        case INQUIRECALLSTATUS:
        {
            T_BASICPARAM* req;
            
            req = (T_BASICPARAM*)buf;
            return Line_InquireCallStatus(req->hLine, req->hRequest);
        }                
        case SETSSNOTE:
        {
            T_TWOSTRPARAM* req;
            
            req = (T_TWOSTRPARAM*)buf;
            return Line_SetSSNote(req->hLine, req->hRequest, req->strValue1, req->strValue2);
        }        
        case SENDUSSD:
        {
            T_THREEU32PARAM* req;
            
            req = (T_THREEU32PARAM*)buf;
            return Line_SendUSSD(req->hLine, req->hRequest, (_CHAR8*)req->value1, (_CHAR8*)req->value2, (_CHAR8*)req->value3);
        }        
        case SENDUSSDDATA:
        {
            T_ONESTRPARAM* req;
            
            req = (T_ONESTRPARAM*)buf;
            return Line_SendUssdData(req->hLine, req->hRequest, req->strValue);
        }        
        case SENDATORIGIN:
        {
            T_ONESTRPARAM* req;
            
            req = (T_ONESTRPARAM*)buf;
            return Line_SendATOrigin(req->hLine, req->hRequest, req->strValue);
        }        
        case CANCELUSSDDATA:
        {
            T_BASICPARAM* req;
            
            req = (T_BASICPARAM*)buf;
            return Line_CancelUssdData(req->hLine, req->hRequest);
        }        
        case DIALDTMF:
        {
            T_ONESTRPARAM* req;
            
            req = (T_ONESTRPARAM*)buf;
            return Line_DialDTMF(req->hLine, req->hRequest, req->strValue);
        }
        case SETDTMFPRD:
        {
            T_ONESTRPARAM* req;
            
            req = (T_ONESTRPARAM*)buf;
            return Line_SetDTMFPrd(req->hLine, req->hRequest, req->strValue);
        }    
        case SETALARM:
        {
            T_SETALARM* req;
            
            req = (T_SETALARM*)buf;
            return Line_SetAlarm(req->hLine, req->hRequest, req->alarmData);
        }        
        case FIELDTESTREPORT:
        {
            T_ONESTRPARAM* req;
            
            req = (T_ONESTRPARAM*)buf;
            return Line_FieldTestReport(req->hLine, req->hRequest, req->strValue);
        }        
        case SWITCHOFFMODULE:
        {
            T_BASICPARAM* req;
            
            req = (T_BASICPARAM*)buf;
            return Line_SwitchOffModule(req->hLine, req->hRequest);
        }        
        case SETMICGAINLEVEL:
        {
            T_ONEU8PARAM* req;
            
            req = (T_ONEU8PARAM*)buf;
            return Line_SetMicGainLevel(req->hLine, req->hRequest, req->uValue);
        }        
        case CONFIGUREHANDFREE:
        {
            T_ONEU32PARAM* req;
            
            req = (T_ONEU32PARAM*)buf;
            return Line_SetAudioPath(req->hLine, req->hRequest, (RIL_AUDIO_PATH_TYPE)req->value);
        }
        case CONFIGUREAUDMOD:
        {
            T_ONEU32PARAM* req;
            
            req = (T_ONEU32PARAM*)buf;
            return Line_SetAudioMode(req->hLine, req->hRequest, (RIL_AUDIO_MOD_TYPE)req->value);
        }        
        case VOICEHANGUPCTRL:
        {
            T_ONEU32PARAM* req;
            
            req = (T_ONEU32PARAM*)buf;
            return Line_SetVoiceHangupCtrl(req->hLine, req->hRequest, (RIL_AUDIO_PATH_TYPE)req->value);
        }        
        case SETAUDIOMUTE:
        {
            T_ONEU32PARAM* req;
            
            req = (T_ONEU32PARAM*)buf;
            return Line_SetAudioMute(req->hLine, req->hRequest, (_BOOL)req->value);
        }
        case PLAYTONE:
        {
            T_TWOU32PARAM* req;
            
            req = (T_TWOU32PARAM*)buf;
            return Line_PlayTone(req->hLine, req->hRequest, (_USHORT16)req->value1, (_USHORT16)req->value2);
        }
        case INQUIREHEADSETSTATUS:
        {
            T_BASICPARAM* req;
            
            req = (T_BASICPARAM*)buf;
            return Line_InquireHeadsetStatus(req->hLine, req->hRequest);
        }        
        case INQUIREHEAdSETBTNSTATUS:
        {
            T_BASICPARAM* req;
            
            req = (T_BASICPARAM*)buf;
            return Line_InquireHeadsetBtnStatus(req->hLine, req->hRequest);
        }        
        case INQUIREBATTERYLEVEL:
        {
            T_BASICPARAM* req;
            
            req = (T_BASICPARAM*)buf;
            return Line_InquireBatteryLevel(req->hLine, req->hRequest);
        }
        case DUMPBROADCAST:
        {
            T_BASICPARAM* req;
            
            req = (T_BASICPARAM*)buf;
            return Line_DumpBroadCast(req->hLine, req->hRequest);
        }        
        case SETRINGERSOUNDLEVEL:
        {
            T_ONES16PARAM* req;
            
            req = (T_ONES16PARAM*)buf;
            return Line_SetRingerSoundLevel(req->hLine, req->hRequest, req->value);
        }        
        case SETAUDIOOUTPUT:
        {
            T_ONESTRPARAM* req;
            
            req = (T_ONESTRPARAM*)buf;
            return Line_SetAudioOutput(req->hLine, req->hRequest, req->strValue);
        }        
        case QUERYMODULESTATE:
        {
            T_BASICPARAM* req;
            
            req = (T_BASICPARAM*)buf;
            return Line_QueryModuleState(req->hLine, req->hRequest);
        }
        case CHECKLATESTURCEVENT:
        {
            T_ONESTRPARAM* req;
            
            req = (T_ONESTRPARAM*)buf;
            return Line_CheckLatestURCEvent(req->hLine, req->hRequest, req->strValue);
        }
        case CONTROLURCINDICATION:
        {
            T_ONESTRPARAM* req;
            
            req = (T_ONESTRPARAM*)buf;
            return Line_ControlURCIndication(req->hLine, req->hRequest, req->strValue);
        }
        case CONTROLMODULEDRIVER:
        {
            T_ONEU8PARAM* req;
            
            req = (T_ONEU8PARAM*)buf;
            return Line_ControlModuleDriver(req->hLine, req->hRequest, req->uValue);
        }
        case QUERYURCINDICATION:
        {
            T_BASICPARAM* req;
            
            req = (T_BASICPARAM*)buf;
            return Line_QueryURCIndication(req->hLine, req->hRequest);
        }
        case MOBILEEQUIPMENTEVENT:
        {
            T_ONESTRPARAM* req;
            
            req = (T_ONESTRPARAM*)buf;
            return Line_MobileEquipmentEvent(req->hLine, req->hRequest, req->strValue);
        }
        case SETMSSTORAGETYPE:
        {
            T_ONESTRPARAM* req;
            
            req = (T_ONESTRPARAM*)buf;
            return Line_SetMSStorageType(req->hLine, req->hRequest, req->strValue);
        }
        case SETMODULESLEEP:
        {
            T_ONEU8PARAM* req;
            
            req = (T_ONEU8PARAM*)buf;
            return Line_SetModuleSleep(req->hLine, req->hRequest,req->uValue);
        }
        case GETMODEMVERSIONNUM:
        {
            T_BASICPARAM* req;
            
            req = (T_BASICPARAM*)buf;
            return Line_GetModemVersionNum(req->hLine, req->hRequest);
        }
        case SETCHARGINGCURRENT:
        {
            T_ONESTRPARAM* req;
            
            req = (T_ONESTRPARAM*)buf;
            return Line_SetChargingCurrent(req->hLine, req->hRequest, req->strValue);
        }
        case SETINDICATIONCONTROL:
        {
            T_ONEU32PARAM* req;
            
            req = (T_ONEU32PARAM*)buf;
            return Line_ConfigURCMode(req->hLine, req->hRequest, (RIL_URC_MODE)req->value);
        }
        case XDRVURCCONTROL:
        {
            T_XDRVURCCONTROL* req;
            
            req = (T_XDRVURCCONTROL*)buf;
            return Line_XDRVURCControl(req->hLine, req->hRequest, req->xdrvType);
        }
        case XDRVAUDIOCONTROL:
        {
            T_XDRVAUDIOCONTROL* req;
            
            req = (T_XDRVAUDIOCONTROL*)buf;
            return Line_XDRVAudioControl(req->hLine, req->hRequest, 
                req->audCtrlType, req->strValue);
        }
        case SETTRACEOUTPUT:
        {
            T_ONEU32PARAM* req;
            
            req = (T_ONEU32PARAM*)buf;
            return Line_SetTraceOutput(req->hLine, req->hRequest, (_BOOL)req->value);
        }
        case GETMODEMEXCLOG:
        {
            T_BASICPARAM* req;
            
            req = (T_BASICPARAM*)buf;
            return Line_GetModemExcLog(req->hLine, req->hRequest);
        }
        case XDRVCONTROLAUDIOLOOPTEST:
        {
            T_ONEU8PARAM* req;

            req = (T_ONEU8PARAM*)buf;
            return Line_XDRVControlAudioLoopTest(req->hLine, req->hRequest, req->uValue);
        }
        case FACILITYLOCK:
        {
            T_FACILITYLOCK* req;
            
            req = (T_FACILITYLOCK*)buf;
            return Line_FacilityLock(req->hLine, req->hRequest, (RIL_FACILITY)req->fac, req->bLock, req->strPasswd, req->eClass);
        }
        case INQUIREFACILITYLOCK:
        {
            T_FACILITYLOCK* req;
            
            req = (T_FACILITYLOCK*)buf;
            return Line_InqFacilityLock(req->hLine, req->hRequest, (RIL_FACILITY)req->fac, req->eClass);
        }
        case CALLINGLINEID:
        {
            T_ONESTRPARAM* req;
            
            req = (T_ONESTRPARAM*)buf;
            return Line_CallingLineId(req->hLine, req->hRequest, req->strValue);
        }        
        case CHECKCLIP:
        {
            T_BASICPARAM* req;
            
            req = (T_BASICPARAM*)buf;
            return Line_CheckCLIP(req->hLine, req->hRequest);
        }        
        case CHECKCLIR:
        {
            T_BASICPARAM* req;
            
            req = (T_BASICPARAM*)buf;
            return Line_CheckCLIR(req->hLine, req->hRequest);
        }
        case SETCLIRACTIVE:
        {
            T_BASICPARAM* req;
            
            req = (T_BASICPARAM*)buf;
            return Line_SetCLIRActive(req->hLine, req->hRequest);
        }
        case SETCLIRDISABLE:
        {
            T_BASICPARAM* req;
            
            req = (T_BASICPARAM*)buf;
            return Line_SetCLIRDisable(req->hLine, req->hRequest);
        }        
        case DAILCLIR:
        {
            T_ONESTRPARAM* req;
            
            req = (T_ONESTRPARAM*)buf;
            return Line_DailCLIR(req->hLine, req->hRequest, req->strValue);
        }
        case CHECKCOLR:
        {
            T_BASICPARAM* req;
            
            req = (T_BASICPARAM*)buf;
            return Line_CheckCOLR(req->hLine, req->hRequest);
        }        
        case INQUIRECOLP:
        {
            T_BASICPARAM* req;
            
            req = (T_BASICPARAM*)buf;
            return Line_InquireCOLP(req->hLine, req->hRequest);
        }        
        case SETCOLPSTATUS:
        {
            T_ONESTRPARAM* req;
            
            req = (T_ONESTRPARAM*)buf;
            return Line_SetColpStatus(req->hLine, req->hRequest, req->strValue);
        }
        case READPBENTRIES:
        {
            T_READPBENTRIES* req;
            
            req = (T_READPBENTRIES*)buf;
            return SIM_ReadPbEntries(req->hLine, req->hRequest, (RIL_PB_STORAGE)req->storage, req->begin, req->end);
        }
        case INQUIREPBSPACE:
        {
            T_ONEU32PARAM* req;
            
            req = (T_ONEU32PARAM*)buf;
            return SIM_InquirePbSpace(req->hLine, req->hRequest, (RIL_PB_STORAGE)req->value);
        }
        case WRITEPBENTRY:
        {
            T_WRITEPBENTRY* req;
            
            req = (T_WRITEPBENTRY*)buf;
            return SIM_WritePbEntry(req->hLine, req->hRequest, (RIL_PB_STORAGE)req->value, req->index, req->strNumber, req->strText);
        }
        case DELETEPBENTRY:
        {
            T_WRITEPBENTRY* req;
            
            req = (T_WRITEPBENTRY*)buf;
            return SIM_DeletePbEntry(req->hLine, req->hRequest, (RIL_PB_STORAGE)req->value, req->index);
        }
        case BATTERYCTRL:
        {
            T_ONESTRPARAM* req;

            req = (T_ONESTRPARAM*)buf;
            return Line_SetBatteryParam(req->hLine, req->hRequest, req->strValue);
        }        
        case TONEDETCTRL:
        {
            T_ONESTRPARAM* req;

            req = (T_ONESTRPARAM*)buf;
            return Line_SetToneDetParam(req->hLine, req->hRequest, req->strValue);
        }    
        case DAICTRL:
        {
            T_ONESTRPARAM* req;

            req = (T_ONESTRPARAM*)buf;
            return Line_SetDAIParam(req->hLine, req->hRequest, req->strValue);
        }            
        case SELECTVOICEGAIN:
        {
            T_ONESTRPARAM* req;

            req = (T_ONESTRPARAM*)buf;
            return Line_SelectVoiceGain(req->hLine, req->hRequest, req->strValue);
        }
        case SIDETONEMODIFY:
        {
            T_ONESTRPARAM* req;

            req = (T_ONESTRPARAM*)buf;
            return Line_SideToneModify(req->hLine, req->hRequest, req->strValue);
        }
        case SAVECONFIGURE:
        {
            T_BASICPARAM* req;

            req = (T_BASICPARAM*)buf;
            return Line_SaveConfigure(req->hLine, req->hRequest);
        }        
        default:
            break;
    }
    return EOS_TRUE;
}

