/********************************************************************
 * @file msg_manager.c
 * @brief: The implementation of the sim manager
 ********************************************************************/
#include "line.h"
#include "sim_manager.h"
#include "wm_interface.h"
#include "radio_server.h"
#include "wm_struct.h"

#include "../wireless_common.h"
#include "../wireless_interface_inc.h"
#include "../wireless_debug.h"


extern WIRELESSROOTST stWirelessRootSt;

_UINT32 SIM_ReadPbEntries(_UINT32 hLine, _UINT32 customParam, RIL_PB_STORAGE storage, _SHORT16 begin, _SHORT16 end)
{
    _UINT32 rtn;

    rtn = WM_ReadPbEntries(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam, storage, begin, end);
    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 SIM_InquirePbSpace(_UINT32 hLine, _UINT32 customParam, RIL_PB_STORAGE storage)
{
    _UINT32 rtn;

    rtn = WM_InquirePbSpace(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam, storage);
    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 SIM_WritePbEntry(_UINT32 hLine, _UINT32 customParam, RIL_PB_STORAGE storage, _SHORT16 index, _CHAR8* strNumber, _USHORT16 *strText)
{
    _UINT32 rtn;

    rtn = WM_WritePbEntry(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam, storage, index, strNumber, strText);
    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 SIM_DeletePbEntry(_UINT32 hLine, _UINT32 customParam, RIL_PB_STORAGE storage, _SHORT16 index)
{
    _UINT32 rtn;

    rtn = WM_WritePbEntry(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam, storage, index, NULL, NULL);
    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 SIM_InitBeforePIN(_UINT32 hLine, _UINT32 customParam)
{
    _UINT32 rtn;

    rtn = WM_InitBeforePIN(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam);
    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 SIM_InitAfterPIN(_UINT32 hLine, _UINT32 customParam)
{
    _UINT32 rtn;

    rtn = WM_InitAfterPIN(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam);
    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 SIM_InquirePinStatus(_UINT32 hLine, _UINT32 customParam)
{
    _UINT32 rtn;

    rtn = WM_InquirePinStatus(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam);
    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 SIM_InquireInitStatus(_UINT32 hLine, _UINT32 customParam)
{
    _UINT32 rtn;

    rtn = WM_InquireInitStatus(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam);
    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 SIM_EnterPin(_UINT32 hLine, _UINT32 customParam, _CHAR8* strPIN, _CHAR8* strNewPIN)
{
    _UINT32 rtn;

    rtn = WM_EnterPIN(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam, strPIN, strNewPIN);
    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 SIM_ChangePassword(_UINT32 hLine, _UINT32 customParam, RIL_FACILITY fac, _CHAR8* strOldPwd, _CHAR8* strNewPwd)
{
    _UINT32 rtn;

    rtn = WM_ChangePassword(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam, fac, strOldPwd, strNewPwd);
    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 SIM_InquirePinCount(_UINT32 hLine, _UINT32 customParam,_UCHAR8 type)
{
    _UINT32 rtn;

    rtn = WM_InquirePinCount(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam,type);

    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 SIM_InquirePukCount(_UINT32 hLine, _UINT32 customParam)
{
    _UINT32 rtn;

    rtn = WM_InquirePukCount(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam);

    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

static _UINT32 ResetCallback(_UINT32 hManager, _UINT32 resultType, _VOID* lpData, _UINT32 totalSize, _UINT32 usedSize, _UINT32 customParam)
{
    T_SIMPARAM *pParam;

    pParam = (T_SIMPARAM*)customParam;

    Line_URCCallback((HATSERVER)pParam->hATServer, WM_TYPE_SIM, RIL_NOTIFY_MODULE_RESET, pParam->info, lpData, totalSize, usedSize);
    WIRELESS_TRACE;
    WIRELESS_FREE(pParam);
    return 0;
}

_UINT32 SIM_WMExceptionCallback(_UINT32 hLine, RIL_EXCEPTION_TYPE type)
{
    _UINT32 rtn;
    T_SIMPARAM *pParam;

    pParam = (T_SIMPARAM*)WIRELESS_MALLOC(sizeof(T_SIMPARAM));
    pParam->hLine = (HLINE)hLine;
    pParam->hATServer = (_UINT32)((HLINE)hLine)->hATServer;
    pParam->requestOwner = 0;
    pParam->info = type;
    pParam->pfCallback = ResetCallback;

    rtn = WM_Reset(((HATSERVER)((HLINE)hLine)->hATServer)->hWM, (_UINT32)((HLINE)hLine)->hATServer, eDEVICESIM, (_UINT32)pParam);

    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 SIMManager_ResultCallback(_UINT32 hManager, 
                              _UINT32 resultType, _VOID* lpData, _UINT32 totalSize, _UINT32 usedSize,
                              _UINT32 customParam)
{
    PWIRELESSROOTST pWirelessRootSt;
    T_SIMPARAM *pParam;

    if(0 == customParam)
    {
        return 0;
    }

    pParam = (T_SIMPARAM*)customParam;

    pWirelessRootSt = (PWIRELESSROOTST)&stWirelessRootSt;
    /*向line发送replay消息*/
    if(pParam->requestOwner == 1)
    {
        Line_ResultCallback(pParam->hLine, resultType, lpData, totalSize, usedSize, pParam->customParam);
        pParam->customParam = NULL;
    }

    if(NULL != pParam->pfCallback)
    {
        pParam->pfCallback(hManager, resultType, lpData, totalSize, usedSize, customParam);
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

_UINT32 SIMManager_URCCallback(_UINT32 hATServer, _UINT32 eventType, _UINT32 eventID, _VOID* lpData, _UINT32 totalSize, _UINT32 usedSize)
{
    switch(eventID)
    {
    case RIL_NOTIFY_REGSTATUSCHANGED:
        {
            RIL_NET_STAT state = *(RIL_NET_STAT*)lpData;
            WIRELESS_FREE(lpData);
            Line_URCCallback((HATSERVER)hATServer, eventType, eventID, state, NULL, 0, 0);
        }
        break;
    case RIL_NOTIFY_CSSU:
        {
            _UINT32 code = usedSize;
            Line_URCCallback((HATSERVER)hATServer, eventType, eventID, code, NULL, 0, 0);
        }
        break;
    case RIL_NOTIFY_HEADSET:
        {
            _UINT32 code = usedSize;
            Line_URCCallback((HATSERVER)hATServer, eventType, eventID, code, NULL, 0, 0);
        }
        break;
    case RIL_NOTIFY_HEADSETBUTTON:
        {
            _UINT32 code = usedSize;
            Line_URCCallback((HATSERVER)hATServer, eventType, eventID, code, NULL, 0, 0);
        }
        break;
    case RIL_NOTIFY_MODULE_RESET:
        {
            T_SIMPARAM *pParam;

            pParam = (T_SIMPARAM*)WIRELESS_MALLOC(sizeof(T_SIMPARAM));
            pParam->hLine = NULL;
            pParam->hATServer = hATServer;
            pParam->requestOwner = 0;
            pParam->info = RIL_EXCEPTION_SOFTWARE;
            pParam->pfCallback = ResetCallback;

            WM_Reset(((HATSERVER)hATServer)->hWM, hATServer, eDEVICESIM, (_UINT32)pParam);
        }
        break;
    default:
        Line_URCCallback((HATSERVER)hATServer, eventType, eventID, 0, lpData, totalSize, usedSize);
        break;
    }
    return 0;
}
