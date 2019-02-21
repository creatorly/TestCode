/********************************************************************
 * @file stk_manager.c
 * @brief: The implementation of the stk manager
 ********************************************************************/
#include "line.h"
#include "stk_manager.h"
#include "wm_interface.h"
#include "radio_server.h"

#include "../wireless_common.h"
#include "../wireless_debug.h"
#include "../wireless_interface_inc.h"

enum
{
    eRequest_STKMng,
    eRequest_Line
};


typedef struct tagstkmanager{
    _UINT32    state;
}T_STKMANAGER;

typedef _UINT32 (*STKGMANAGERCALLBACK)(HSTKMANAGER hSTKManager, 
                              _UINT32 resultType, _VOID* lpData, _UINT32 totalSize, _UINT32 usedSize,
                              _UINT32 customParam);

typedef struct tagstkparam{
    HLINE        hLine;    /*发起请求的line的句柄*/
    _UINT32        hATServer;
    _UINT32        requestOwner; /*该请求是line的请求还是STK manager的请求; 1: line的请求; 0: call manager的请求*/
    _UINT32        customParam; /*如果请求是line的请求,记录下line的custom Parameter信息*/

    STKGMANAGERCALLBACK    pfCallback;         /*请求结果的回调函数*/

}T_STKPARAM;

extern WIRELESSROOTST stWirelessRootSt;

static _UINT32 setupCallCallback(HSTKMANAGER hSTKManager, _UINT32 resultType, _VOID* lpData, _UINT32 totalSize, _UINT32 usedSize, _UINT32 customParam);
static _UINT32 GetDetailInfoCallback(HSTKMANAGER hSTKManager, _UINT32 resultType, _VOID* lpData, _UINT32 totalSize, _UINT32 usedSize, _UINT32 customParam);

/*
 *初始化STK manager
 */
_UINT32 STKManager_Init(HSTKMANAGER *phSTKManager)
{
    T_STKMANAGER *pSTKManager;

    pSTKManager = (T_STKMANAGER*)malloc(sizeof(T_STKMANAGER));
    pSTKManager->state = 0;
    
    *phSTKManager = pSTKManager;
    return 0;
}

/*
 * 注销STK manager
 */
_UINT32 STKManager_DeInit(HSTKMANAGER hSTKManager)
{
    free(hSTKManager);
    return 0;
}

_UINT32 STKManager_SetProfile(_UINT32 hLine, _UINT32 customParam)
{
    T_STKPARAM    *pParam;
    _UINT32        rtn;

    pParam = (T_STKPARAM*)WIRELESS_MALLOC(sizeof(T_STKPARAM));
    pParam->hLine = (HLINE)hLine;
    pParam->hATServer = (_UINT32)((HLINE)(pParam->hLine)->hATServer);
    pParam->requestOwner = eRequest_Line;
    pParam->customParam = customParam;

    pParam->pfCallback = NULL;

    rtn = WM_STKSetProfile(((HLINE)hLine)->hATServer->hWM,
                            (_UINT32)(((HLINE)hLine)->hATServer->hSTKManager),
                            eDEVICESTK,
                            (_UINT32)pParam);

    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 STKManager_SetAutoRsp(_UINT32 hLine, _UINT32 customParam)
{
    T_STKPARAM    *pParam;
    _UINT32        rtn;

    pParam = (T_STKPARAM*)WIRELESS_MALLOC(sizeof(T_STKPARAM));
    pParam->hLine = (HLINE)hLine;
    pParam->hATServer = (_UINT32)((HLINE)(pParam->hLine)->hATServer);
    pParam->requestOwner = eRequest_Line;
    pParam->customParam = customParam;

    pParam->pfCallback = NULL;

    rtn = WM_STKSetAutoRsp(((HLINE)hLine)->hATServer->hWM,
                            (_UINT32)(((HLINE)hLine)->hATServer->hSTKManager),
                            eDEVICESTK,
                            (_UINT32)pParam);

    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 STKManager_SetTR(_UINT32 hLine, _UINT32 customParam)
{
    T_STKPARAM    *pParam;
    _UINT32        rtn;

    pParam = (T_STKPARAM*)WIRELESS_MALLOC(sizeof(T_STKPARAM));
    pParam->hLine = (HLINE)hLine;
    pParam->hATServer = (_UINT32)((HLINE)(pParam->hLine)->hATServer);
    pParam->requestOwner = eRequest_Line;
    pParam->customParam = customParam;

    pParam->pfCallback = NULL;

    rtn = WM_STKSetTR(((HLINE)hLine)->hATServer->hWM,
                            (_UINT32)(((HLINE)hLine)->hATServer->hSTKManager),
                            eDEVICESTK,
                            (_UINT32)pParam);

    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}



_UINT32 STKManager_SelectMainMenu(_UINT32 hLine, _SHORT16 index, _UINT32 customParam)
{
    T_STKPARAM    *pParam;
    _UINT32        rtn;

    pParam = (T_STKPARAM*)WIRELESS_MALLOC(sizeof(T_STKPARAM));
    pParam->hLine = (HLINE)hLine;
    pParam->hATServer = (_UINT32)((HLINE)(pParam->hLine)->hATServer);
    pParam->requestOwner = eRequest_Line;
    pParam->customParam = customParam;
    pParam->pfCallback = NULL;

    rtn = WM_STKSelectMainMenu(((HLINE)hLine)->hATServer->hWM,
                            (_UINT32)(((HLINE)hLine)->hATServer->hSTKManager),
                            eDEVICESTK,
                            (_UINT32)pParam,
                            index);

    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 STKManager_Response(_UINT32 hLine, RIL_RES_PAR *par, _UINT32 customParam)
{
    T_STKPARAM    *pParam;
    _UINT32        rtn;

    pParam = (T_STKPARAM*)WIRELESS_MALLOC(sizeof(T_STKPARAM));
    pParam->hLine = (HLINE)hLine;
    pParam->hATServer = (_UINT32)((HLINE)(pParam->hLine)->hATServer);
    pParam->requestOwner = eRequest_Line;
    pParam->customParam = customParam;

    if(RIL_STK_SetupCall == par->cmdId && RIL_AcceptedCall == par->res.r_setupcall)
    {
        pParam->pfCallback = setupCallCallback;
    }
    else
    {
        pParam->pfCallback = NULL;
    }

    rtn = WM_STKResponse(((HLINE)hLine)->hATServer->hWM,
                            (_UINT32)(((HLINE)hLine)->hATServer->hSTKManager),
                            eDEVICESTK,
                            (_UINT32)pParam,
                            par);

    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

/*
 *STK manager处理AT命令返回结果回调函数
 */
_UINT32 STKManager_ResultCallback(HSTKMANAGER hSTKManager, 
                              _UINT32 resultType, _VOID* lpData, _UINT32 totalSize, _UINT32 usedSize,
                              _UINT32 customParam)
{
    PWIRELESSROOTST pWirelessRootSt;
    T_STKPARAM *pParam;

    if(0 == customParam)
    {
        return 0;
    }
    pParam = (T_STKPARAM*)customParam;

    pWirelessRootSt = (PWIRELESSROOTST)&stWirelessRootSt;

#if 0
    /*向line发送replay消息*/
    if(pParam->requestOwner == eRequest_Line)
    {
        Line_ResultCallback(pParam->hLine, resultType, lpData, totalSize, usedSize, pParam->customParam);
        pParam->customParam = NULL;
    }
#endif    

    if(NULL != pParam->pfCallback)
    {
        pParam->pfCallback(hSTKManager, resultType, lpData, totalSize, usedSize, customParam);
    }
    else
    {
        if(pParam->customParam)
        {
            WIRELESS_FREE(pParam->customParam);
            pParam->customParam = NULL;
        }
        WIRELESS_FREE(pParam);
    }
    
    return 0;
}

/*
 *STK manager处理URC回调函数
 */
_UINT32 STKManager_URCCallback(_UINT32 hATServer, _UINT32 eventType, _UINT32 eventID, _VOID* lpData, _UINT32 totalSize, _UINT32 usedSize)
{
    if(WM_STKDetailInfoInURC(((HATSERVER)hATServer)->hWM))
    {
        return Line_URCCallback((HATSERVER)hATServer, eventType, eventID, 0, lpData, totalSize, usedSize);
    }
    else
    {
        /*send AT command to get detail info, then broadcast STK event with detail info.*/
        T_STKPARAM    *pParam;
        _UINT32        rtn;

        if(RIL_STK_EndProactive == eventID)
        {
            return Line_URCCallback((HATSERVER)hATServer, eventType, RIL_NOTIFY_STK_EndProactive, 0, lpData, totalSize, usedSize);
        }

        pParam = (T_STKPARAM*)WIRELESS_MALLOC(sizeof(T_STKPARAM));
        pParam->hLine = NULL;
        pParam->hATServer = hATServer;
        pParam->requestOwner = eRequest_STKMng;
        pParam->pfCallback = GetDetailInfoCallback;

        rtn = WM_STKGetDetailInfo(((HATSERVER)hATServer)->hWM,
                                (_UINT32)((HATSERVER)hATServer)->hSTKManager,
                                eDEVICESTK,
                                (_UINT32)pParam,
                                eventID);

    }
    return 0;
}

static _UINT32 setupCallCallback(HSTKMANAGER hSTKManager, _UINT32 resultType, _VOID* lpData, _UINT32 totalSize, _UINT32 usedSize, _UINT32 customParam)
{
    T_STKPARAM *pParam;

    pParam = (T_STKPARAM*)customParam;

    if(resultType == RIL_prOK)
    {
        CallManager_ListCall((_UINT32)pParam->hLine, pParam->customParam);
    }
    WIRELESS_FREE(pParam);
    return 0;
}

static _UINT32 GetDetailInfoCallback(HSTKMANAGER hSTKManager, _UINT32 resultType, _VOID* lpData, _UINT32 totalSize, _UINT32 usedSize, _UINT32 customParam)
{
    T_STKPARAM *pParam;
    _UINT32        hATServer;

    pParam = (T_STKPARAM*)customParam;
    hATServer = pParam->hATServer;
    WIRELESS_FREE(pParam);

    if(resultType == RIL_prOK)
    {
        Line_URCCallback((HATSERVER)hATServer, WM_TYPE_STK, ((RIL_STK_PAR*)lpData)->cmdId, 0, lpData, totalSize, usedSize);
    }
    return 0;
}



