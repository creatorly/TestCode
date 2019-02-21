
#include "wm_interface.h"
#include "radio_server.h"
#include "line.h"
#include "ak_identity.h"

#include "../wireless_common.h"
#include "../wireless_interface_inc.h"
#include "../wireless_debug.h"


extern WIRELESSROOTST     stWirelessRootSt;

static HATSERVER sHATServer[4];
static _UINT32     sATServerCount=0;

_UINT32 ATServer_Init(_UINT32 ChnlNum, _USHORT16 *ChID, _UINT32 Type, HATSERVER *phATServer)
{
    T_ATSERVER *pATServer = NULL;

    pATServer = (T_ATSERVER*)malloc(sizeof(T_ATSERVER));

    WM_Create(ChnlNum, ChID, Type, &(pATServer->hWM));
    
    pATServer->connectState = 0;
    CallManager_Init(&(pATServer->hCallManager));
    MSGManager_Init(&(pATServer->hMSGManager));
    STKManager_Init(&(pATServer->hSTKManager));
    DataConnManager_Init(&(pATServer->hDataConnManager), ChID);

    *phATServer = pATServer;
    return 0;
}

/*
 * 注销line管理模块
 */
_UINT32 ATServer_DeInit(HATSERVER hATServer)
{
    CallManager_DeInit(hATServer->hCallManager);
    MSGManager_DeInit(hATServer->hMSGManager);
    STKManager_DeInit(hATServer->hSTKManager);
    DataConnManager_DeInit(hATServer->hDataConnManager);
    WM_Destory(hATServer->hWM);

    free(hATServer);
    return 0;
}

/*
 *检查hATServer是否有效
 */
_BOOL ATServer_CheckHandleValid(HATSERVER hATServer)
{
    return EOS_TRUE;

#if 0        /* Kinson 20100920    */
    _UINT32 i;

    for(i=0; i<sATServerCount; i++)
    {
        if(sHATServer[i] == hATServer)
        {
            return EOS_TRUE;
        }
    }
    return EOS_FALSE;
#endif    
}

/*
 *AT命令返回结果回调函数
 */
_UINT32 ATServer_ResultCallback(_UINT32 ChannelId, _UINT32 deviceHandle, T_EDEVICEETYPE deviceType, 
                              _UINT32 resultType, _VOID* lpData, _UINT32 totalSize, _UINT32 usedSize,
                              _UINT32 customParam)
{
    //WIRELESS_INFO("Channel %d ATServer_ResultCallback deviceType = %d, resultType = %d\r\n", ChannelId, deviceType, resultType);
    switch(deviceType)
    {
    case eDEVICECALL:
        CallManager_ResultCallback(ChannelId, (HCALLMANAGER)deviceHandle, resultType, lpData, totalSize, usedSize, customParam);
        break;
    case eDEVICESMS:
        MSGManager_ResultCallback((HMSGMANAGER)deviceHandle, resultType, lpData, totalSize, usedSize, customParam);
        break;
    case eDEVICESTK:
        STKManager_ResultCallback((HSTKMANAGER)deviceHandle, resultType, lpData, totalSize, usedSize, customParam);
        break;
    case eDEVICESIM:
        SIMManager_ResultCallback(deviceHandle, resultType, lpData, totalSize, usedSize, customParam);
        break;
    case eDEVICEDATACONN:
        DataConn_ResultCallback((PT_DATACONNMANAGER)deviceHandle, resultType, lpData, totalSize, usedSize, customParam);
        break;
    default:
        Line_ResultCallback((HLINE)deviceHandle, resultType, lpData, totalSize, usedSize, customParam);
        break;
    }
    return 0;
}

/*
 *URC回调函数
 */
_UINT32 ATServer_URCCallback(_UINT32 ChnID, _UINT32 eventType, _UINT32 eventID, _VOID* lpData, _UINT32 totalSize, _UINT32 usedSize)
{
    PWIRELESSROOTST pWirelessRootSt;
    PWIRELESSCHANNELST  pWirelessChannelCtl;
    HATSERVER         stHATServer;

    pWirelessRootSt = (PWIRELESSROOTST)&stWirelessRootSt;
    pWirelessChannelCtl = &pWirelessRootSt->stChannelSt[ChnID];
    stHATServer = pWirelessChannelCtl->stHATServer;

    /*
    _UINT32 i = 0;
    for(i = 0; i < sATServerCount; i++)
    {
        if(sHATServer[i]->hWM == hWMback)
        {
            break;
        }
    }
    */

    switch(eventType)
    {
        case WM_TYPE_CALL:
        {            
            CallManager_URCCallback((_UINT32)stHATServer, eventType, eventID, lpData, totalSize, usedSize);
            break;
        }
        case WM_TYPE_MSG:
        {
            MSGManager_URCCallback((_UINT32)stHATServer, eventType, eventID, lpData, totalSize, usedSize);
            break;
        }
        case WM_TYPE_STK:
        {
            STKManager_URCCallback((_UINT32)stHATServer, eventType, eventID, lpData, totalSize, usedSize);
            break;
        }
        case WM_TYPE_SIM:
        {
            SIMManager_URCCallback((_UINT32)stHATServer, eventType, eventID, lpData, totalSize, usedSize);
            break;
        }
        default:
            Line_URCCallback(stHATServer, eventType, eventID, 0, lpData, totalSize, usedSize);
    }
    return 0;
}


/*
 *AT Server 启动
 */
_UINT32 ATServer_Setup(_UINT32 ChnlNum, _USHORT16* ChID, _UINT32 Type)
{
    PWIRELESSROOTST     pWirelessRootSt;
    PWIRELESSCHANNELST  pWirelessChannelCtl;
    
    pWirelessRootSt = (PWIRELESSROOTST)&stWirelessRootSt;
    pWirelessChannelCtl = &pWirelessRootSt->stChannelSt[ChID[0]];

    LineManager_Init(ChID[0]);
    ATServer_Init(ChnlNum, ChID, Type, &(pWirelessChannelCtl->stHATServer));

    return Line_SendReplay(ChID[0], 0, 0, (_UINT32)(pWirelessChannelCtl->stHATServer), NULL, 0, 0);
}

_UINT32 ATServer_Stop(_VOID)
{
    _UINT32 i;

    for(i=0; i<sATServerCount; i++)
    {
        ATServer_DeInit(sHATServer[i]);
    }

    LineManager_DeInit();

    return Line_SendReplay(0, 0, 0, 0, NULL, 0, 0);
}

_UINT32 ATServer_UpdateChannel(HATSERVER hATServer, _UINT32 ChnlNum, _USHORT16 *ChID)
{
    _UINT32 rtn;

    rtn = (_UINT32)WM_UpdateChannel(hATServer->hWM, ChnlNum, ChID);
    return Line_SendReplay(0, 0, rtn, 0, NULL, 0, 0);
}

_UINT32 ATServer_SetConnState(HATSERVER hATServer, T_CONNECT_STATE state)
{
    switch(state)
    {
    case CONNSTATE_RINGON:
        hATServer->connectState |= CONNECT_STATE_RING;
        break;
    case CONNSTATE_RINGOFF:
        hATServer->connectState &= ~CONNECT_STATE_RING;
        break;
    case CONNSTATE_NEWMSGON:
        hATServer->connectState |= CONNECT_STATE_MSG;
        break;
    case CONNSTATE_NEWMSGOFF:
        hATServer->connectState &= ~CONNECT_STATE_MSG;
        break;
    }
    return 0;
}

_UINT32 ATServer_GetConnState(HATSERVER hATServer, _UINT32 *state)
{
    *state = hATServer->connectState;
    return 0;
}

_UINT32 ATServer_IgniteModule(_UINT32 hLine, _UINT32 customParam)
{
    _UINT32 rtn;

    rtn = WM_IgniteModule(((HLINE)hLine)->hATServer->hWM, hLine, eDEVICECOMMON, customParam);
    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 ATServer_ResetModule(_UINT32 hLine, _UINT32 customParam)
{
    _UINT32 rtn;

    rtn = WM_ResetModule(((HLINE)hLine)->hATServer->hWM, hLine, eDEVICECOMMON, customParam);
    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}
