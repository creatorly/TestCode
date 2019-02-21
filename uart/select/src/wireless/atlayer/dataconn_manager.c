/********************************************************************
 * @file dataconn_manager.c
 * @brief: The implementation of the data connection manager
 ********************************************************************/
#include "line.h"
#include "dataconn_manager.h"
#include "wm_interface.h"
#include "radio_server.h"

#include "../wireless_common.h"
#include "../wireless_debug.h"
#include "../wireless_interface_inc.h"

extern WIRELESSROOTST    stWirelessRootSt;
/**
 *初始化Data connection manager
 */
_UINT32 DataConnManager_Init(PT_DATACONNMANAGER *phDataConnManager, _USHORT16 *ChID)
{    
    PWIRELESSROOTST     pWirelessRootSt;
    PWIRELESSCHANNELST  pWirelessChannelCtl;
    
    pWirelessRootSt = (PWIRELESSROOTST)&stWirelessRootSt;
    pWirelessChannelCtl = &pWirelessRootSt->stChannelSt[ChID[0]];

    pWirelessChannelCtl->pstManager = (T_DATACONNMANAGER*)malloc(sizeof(T_DATACONNMANAGER));
    pWirelessChannelCtl->pstManager->bConnect = EOS_FALSE;
    pWirelessChannelCtl->pstManager->bRelease = EOS_FALSE;
    
    *phDataConnManager = pWirelessChannelCtl->pstManager;
    return 0;
}

/**
 * 注销MSG manager
 */
_UINT32 DataConnManager_DeInit(PT_DATACONNMANAGER hManager)
{
    WIRELESS_TRACE;
    free(hManager);
    return 0;
}

_UINT32 DataConn_InquireGprsState(_UINT32 hLine, _UINT32 customParam)
{
    _UINT32 rtn;

    rtn = WM_InquireGprsState(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam);
    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}


_UINT32 DataConn_EstablishWithGPRS(_UINT32 hLine, _CHAR8* strAPN, _BOOL bMUX, _UINT32 customParam)
{
    T_DATACONNPARAM    *pParam;
    _UINT32        rtn;

    PWIRELESSROOTST     pWirelessRootSt;
    PWIRELESSCHANNELST  pWirelessChannelCtl;
    
    pWirelessRootSt = (PWIRELESSROOTST)&stWirelessRootSt;
    pWirelessChannelCtl = &pWirelessRootSt->stChannelSt[1]; /*?*/

    pParam = (T_DATACONNPARAM*)WIRELESS_MALLOC(sizeof(T_DATACONNPARAM));
    pParam->hLine = (HLINE)hLine;
    pParam->hATServer = (_UINT32)((HLINE)(pParam->hLine)->hATServer);
    pParam->customParam = customParam;
    
    pParam->operation = DATACALL_ESTABLISHGPRS;
    WM_SetDataConnMode(((HLINE)hLine)->hATServer->hWM, DATACALL_GPRS);

    rtn = WM_EstablishDataConnWithGPRS(((HLINE)hLine)->hATServer->hWM,
                            (_UINT32)(((HLINE)hLine)->hATServer->hDataConnManager),
                            eDEVICEDATACONN,
                            (_UINT32)pParam,
                            strAPN,
                            bMUX,
                            0);

    pWirelessChannelCtl->pstManager->bConnect = EOS_TRUE;
    pWirelessChannelCtl->pstManager->ConnCmdID = rtn;

    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 DataConn_EstablishWithCSD(_UINT32 hLine, _CHAR8* dial, _CHAR8* user, _CHAR8* pwd, _BOOL bMUX, _UINT32 customParam)
{
    T_DATACONNPARAM    *pParam;
    _UINT32        rtn;
    PWIRELESSROOTST     pWirelessRootSt;
    PWIRELESSCHANNELST  pWirelessChannelCtl;
    
    pWirelessRootSt = (PWIRELESSROOTST)&stWirelessRootSt;
    pWirelessChannelCtl = &pWirelessRootSt->stChannelSt[1]; /*?*/

    pParam = (T_DATACONNPARAM*)WIRELESS_MALLOC(sizeof(T_DATACONNPARAM));
    pParam->hLine = (HLINE)hLine;
    pParam->hATServer = (_UINT32)((HLINE)(pParam->hLine)->hATServer);
    pParam->customParam = customParam;
    
    pParam->operation = DATACALL_ESTABLISHCSD;
    WM_SetDataConnMode(((HLINE)hLine)->hATServer->hWM, DATACALL_CSD);

    rtn = WM_EstablishDataConnWithCSD(((HLINE)hLine)->hATServer->hWM,
                            (_UINT32)(((HLINE)hLine)->hATServer->hDataConnManager),
                            eDEVICEDATACONN,
                            (_UINT32)pParam,
                            dial,
                            user,
                            pwd,
                            bMUX,
                            0);

    pWirelessChannelCtl->pstManager->bConnect = EOS_TRUE;
    pWirelessChannelCtl->pstManager->ConnCmdID = rtn;

    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 DataConn_Release(_UINT32 hLine, _BOOL bMUX, _USHORT16 step, _UINT32 customParam)
{
    T_DATACONNPARAM    *pParam;
    _UINT32        rtn;
    _USHORT16        curStep;
    PWIRELESSROOTST     pWirelessRootSt;
    PWIRELESSCHANNELST  pWirelessChannelCtl;
    
    pWirelessRootSt = (PWIRELESSROOTST)&stWirelessRootSt;
    pWirelessChannelCtl = &pWirelessRootSt->stChannelSt[1]; /*?*/

    curStep = step;
    pParam = (T_DATACONNPARAM*)WIRELESS_MALLOC(sizeof(T_DATACONNPARAM));
    pParam->hLine = (HLINE)hLine;
    pParam->hATServer = (_UINT32)((HLINE)(pParam->hLine)->hATServer);
    pParam->customParam = customParam;
    pParam->operation = DATACALL_RELEASE;
    
    pWirelessChannelCtl->pstManager->bRelease = EOS_TRUE;
    if(pWirelessChannelCtl->pstManager->bConnect)
    {
        WM_SetCancelCmdID(((HLINE)hLine)->hATServer->hWM, pWirelessChannelCtl->pstManager->ConnCmdID);
        curStep = 1;
    }
    rtn = WM_ReleaseDataConn(((HLINE)hLine)->hATServer->hWM,
                            (_UINT32)(((HLINE)hLine)->hATServer->hDataConnManager),
                            eDEVICEDATACONN,
                            (_UINT32)pParam,
                            bMUX,
                            curStep);
    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 DataConn_Suspend(_UINT32 hLine, _BOOL bMUX, _UINT32 customParam)
{
    T_DATACONNPARAM    *pParam;
    _UINT32        rtn;

    pParam = (T_DATACONNPARAM*)WIRELESS_MALLOC(sizeof(T_DATACONNPARAM));
    pParam->hLine = (HLINE)hLine;
    pParam->hATServer = (_UINT32)((HLINE)(pParam->hLine)->hATServer);
    pParam->customParam = customParam;
    
    pParam->operation = DATACALL_SUSPEND;
    rtn = WM_SuspendDataConn(((HLINE)hLine)->hATServer->hWM,
                            (_UINT32)(((HLINE)hLine)->hATServer->hDataConnManager),
                            eDEVICEDATACONN,
                            (_UINT32)pParam,
                            bMUX,
                            0);
    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 DataConn_Resume(_UINT32 hLine, _BOOL bMUX, _UINT32 customParam)
{
    T_DATACONNPARAM    *pParam;
    _UINT32        rtn;

    pParam = (T_DATACONNPARAM*)WIRELESS_MALLOC(sizeof(T_DATACONNPARAM));
    pParam->hLine = (HLINE)hLine;
    pParam->hATServer = (_UINT32)((HLINE)(pParam->hLine)->hATServer);
    pParam->customParam = customParam;
    
    pParam->operation = DATACALL_RESUME;
    rtn = WM_ResumeDataConn(((HLINE)hLine)->hATServer->hWM,
                            (_UINT32)(((HLINE)hLine)->hATServer->hDataConnManager),
                            eDEVICEDATACONN,
                            (_UINT32)pParam,
                            bMUX,
                            0);
    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

/*
 *MSG manager处理AT命令返回结果回调函数
 */
_UINT32 DataConn_ResultCallback(PT_DATACONNMANAGER hManager, 
                              _UINT32 resultType, _VOID* lpData, _UINT32 totalSize, _UINT32 usedSize,
                              _UINT32 customParam)
{
    T_DATACONNPARAM *pParam;
    PWIRELESSROOTST     pWirelessRootSt;
    PWIRELESSCHANNELST  pWirelessChannelCtl;
    
    pWirelessRootSt = (PWIRELESSROOTST)&stWirelessRootSt;
    pWirelessChannelCtl = &pWirelessRootSt->stChannelSt[1]; /*?*/

    pParam = (T_DATACONNPARAM*)customParam;

    if(resultType == RIL_prOK || resultType == RIL_prCONNECT)
    {
        switch(pParam->operation) {
        case DATACALL_ESTABLISHGPRS:
        case DATACALL_ESTABLISHCSD:
            pWirelessChannelCtl->pstManager->bConnect = EOS_FALSE;
            if(pWirelessChannelCtl->pstManager->bRelease)
            {
                WM_SwitchDataToAtMode(((HATSERVER)pParam->hATServer)->hWM, (_UINT32)hManager, eDEVICEDATACONN, customParam);
                WIRELESS_FREE(pParam);
                return 0;
            }
            break;
        case DATACALL_RELEASE:
            pWirelessChannelCtl->pstManager->bRelease = EOS_FALSE;
            WM_SetDataConnMode(((HATSERVER)pParam->hATServer)->hWM, DATACALL_NULL);
            break;
        case DATACALL_SUSPEND:
            break;
        case DATACALL_RESUME:
            break;
        default:
            break;
        }
    }
    else
    {
        switch(pParam->operation) {
        case DATACALL_ESTABLISHGPRS:
        case DATACALL_ESTABLISHCSD:
            pWirelessChannelCtl->pstManager->bConnect = EOS_FALSE;
            WM_SetDataConnMode(((HATSERVER)pParam->hATServer)->hWM, DATACALL_NULL);
            break;
        case DATACALL_RELEASE:
            pWirelessChannelCtl->pstManager->bRelease = EOS_FALSE;
            WM_SetDataConnMode(((HATSERVER)pParam->hATServer)->hWM, DATACALL_NULL);
            break;
        case DATACALL_SUSPEND:
            WM_SetDataConnMode(((HATSERVER)pParam->hATServer)->hWM, DATACALL_NULL);
            break;
        case DATACALL_RESUME:
            WM_SetDataConnMode(((HATSERVER)pParam->hATServer)->hWM, DATACALL_NULL);
            break;
        default:
            break;
        }
    }
    Line_ResultCallback(pParam->hLine, resultType, lpData, totalSize, usedSize, pParam->customParam);
    pParam->customParam = NULL;

    WIRELESS_FREE(pParam);
    return 0;
}

