
#include "eos.h"
#include "utils.h"

#include "wireless_list.h"
#include "line.h"
#include "call_manager.h"
#include "wm_interface.h"
#include "radio_server.h"

#include "../wireless_common.h"
#include "../wireless_debug.h"
#include "../wireless_interface_inc.h"


#define WIRELESS_SEND_CLCC

enum
{
    eRequest_CallMng = 0,
    eRequest_Line
};

#define UNKNOWN_INDEX        0xff

typedef struct tagstructcall{
    _USHORT16                readflag;
    _SHORT16                idx;
    RIL_TELE_ORIGNATE    dir;
    RIL_CALL_STATUS        stat;
    RIL_CALL_MODE        mode;
    RIL_CALL_PARTY        mpty;
    T_STR_TELE            number;
    RIL_TELE_TYPE        type;
    RIL_DATA_CLASS        callClass;

    HLINE                owner;
    struct list_head    list;
}T_STRUCTCALL;

typedef struct tagcallmanager{
    struct list_head    head;        /*line$)A5DA41mM7*/
    EOS_SEM_ID          mutex;            /*$)AAY=gGx,2YWwA41mJ1,R*;%3b.*/
    _USHORT16            callQty;        /*total calls number*/
    _USHORT16            activeQty;        /*total active calls number*/
    _USHORT16            holdQty;        /*total hold calls number*/
    _USHORT16            DialQty;        /*total Dialing calls number*/
    _USHORT16            WaitQty;        /*total Waiting calls number*/
}T_CALLMANAGER;


extern WIRELESSROOTST    stWirelessRootSt;

_UINT32 ParseCLCC_Callback(_UINT32 ChannelId, HCALLMANAGER hCallManager, 
                              _UINT32 resultType, _VOID* lpData, _UINT32 totalSize, _UINT32 usedSize,
                              _UINT32 customParam);
_UINT32 SendCLCC_Callback(_UINT32 ChannelId, HCALLMANAGER hCallManager, 
                              _UINT32 resultType, _VOID* lpData, _UINT32 totalSize, _UINT32 usedSize,
                              _UINT32 customParam);

/* 2011.12.16 pengjian added for test */
_VOID* CallManagerParamDump(_UINT32 uiParam, const _CHAR8* file, _INT32 line)
{
    if (uiParam != 0)
    {
        T_CALLPARAM* pParam = (T_CALLPARAM*)uiParam;

        WIRELESS_EVENT("(%s:%d): T_CALLPARAM_%p(%p, %p, %d, %d, %d, %p, %d, %d:%d, %d, %d)\r\n", 
            file, line, pParam, 
            pParam->hLine, pParam->hCall, pParam->hATServer,
            pParam->requestOwner, pParam->customParam, 
            pParam->pfCallback, pParam->event, 
            pParam->cmd[pParam->cmdIndex % 8].ID, 
            pParam->cmd[pParam->cmdIndex % 8].index, 
            pParam->cmdCount, pParam->cmdIndex);
    }
}

/*
 *$)A3uJ<;/call manager
 */
_UINT32 CallManager_Init(HCALLMANAGER *phCallManager)
{
    T_CALLMANAGER *pCallManager;

    pCallManager = (T_CALLMANAGER*)malloc(sizeof(T_CALLMANAGER));    
    
    if(eos_sem_create("call_mag", &(pCallManager->mutex)) < 0)
    {
        printf("can't create semphere call_mag\r\n");
        return EOS_ERROR;
    }

    INIT_LIST_HEAD(&(pCallManager->head));

    pCallManager->callQty = 0;
    pCallManager->activeQty = 0;
    pCallManager->holdQty = 0;
    pCallManager->DialQty = 0;
    pCallManager->WaitQty = 0;

    *phCallManager = pCallManager;
    return 0;
}


_UINT32 CallManager_DeInit(HCALLMANAGER hCallManager)
{
    T_STRUCTCALL *pCall;
    struct list_head *pos;

    WirelessSemP(hCallManager->mutex, EOS_WAIT_FOREVER);
    for(pos = hCallManager->head.next; pos != &(hCallManager->head); pos = hCallManager->head.next)
    {
        pCall = list_entry(pos, T_STRUCTCALL, list);
        ril_list_del(pos);
        WIRELESS_FREE(pCall);
    }
    eos_sem_delete(hCallManager->mutex);
    free(hCallManager);
    return 0;
}

_UINT32 CallManager_DeInit_Ex(HCALLMANAGER hCallManager) /* Not Free hCallManager    */
{
    T_STRUCTCALL *pCall;
    struct list_head *pos;

    WirelessSemP(hCallManager->mutex, EOS_WAIT_FOREVER);
    for(pos = hCallManager->head.next; pos != &(hCallManager->head); pos = hCallManager->head.next)
    {
        pCall = list_entry(pos, T_STRUCTCALL, list);
        ril_list_del(pos);
        WIRELESS_FREE(pCall);
    }
    WirelessSemV(hCallManager->mutex);
   /* eos_sem_delete(hCallManager->mutex);*/
    return 0;
}


_UINT32 CallManager_CallForward(_UINT32 hLine, _UINT32 customParam, RIL_CCFC_FWDCALLS *cfData)
{
    _UINT32 rtn;

    rtn = WM_CallForward(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam, cfData);
    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 CallManager_InquireCallFwd(_UINT32 hLine, _UINT32 customParam, RIL_CCFC_REAS reason, RIL_DATA_CLASS Class)
{
    _UINT32 rtn;

    rtn = WM_InquireCallFwd(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam, reason, Class);
    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 CallManager_SetCallWait(_UINT32 hLine, _UINT32 customParam,_BOOL bURCEnable, _BOOL bEnable, RIL_DATA_CLASS eClass)
{
    _UINT32 rtn;

    rtn = WM_SetCallWait(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam, bURCEnable, bEnable, eClass);
    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 CallManager_InquireCallWait(_UINT32 hLine, _UINT32 customParam, RIL_DATA_CLASS eClass)
{
    _UINT32 rtn;

    rtn = WM_InquireCallWait(((HLINE)hLine)->hATServer->hWM, (_UINT32)hLine, eDEVICECOMMON, customParam, eClass);
    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 CallManager_MakeCall(_UINT32 hLine, const _UCHAR8* lpszAddress, _UINT32 dwType, _UINT32 dwOptions, _UINT32 customParam)
{
    HCALLMANAGER hCallManager;
    T_CALLPARAM* pParam = NULL;
    T_STRUCTCALL* pCall = NULL;
    _UINT32 rtn = 0;
    PWIRELESSROOTST pWirelessRootSt;
    
    pWirelessRootSt = (PWIRELESSROOTST)&stWirelessRootSt;

    hCallManager = ((HLINE)hLine)->hATServer->hCallManager;

    /*create Call handle to this call */
    pCall = (T_STRUCTCALL*)WIRELESS_MALLOC(sizeof(T_STRUCTCALL));
    if(pCall == NULL)
    {
        WIRELESS_ERROR((sizeof(T_STRUCTCALL)), "Wireless Malloc Failed\r\n");
        return EOS_ERROR;
    }
    
    pCall->dir = RIL_MOCALL;
    pCall->idx = UNKNOWN_INDEX;
    pCall->mode = RIL_VOICECALL;
    pCall->mpty = RIL_NINPARTY;
    strcpy((_CHAR8*)pCall->number, (const _CHAR8*)lpszAddress);
    pCall->owner = (HLINE)hLine;
    pCall->stat = RIL_DIALING;
    pCall->callClass = (RIL_DATA_CLASS)dwOptions;

    /*Add this new call handle to handle queue*/
    WirelessSemP(hCallManager->mutex, EOS_WAIT_FOREVER);
    ril_list_add(&(pCall->list), &(hCallManager->head));
    WirelessSemV(hCallManager->mutex);

    /*Add call-manager's additional request to the line-request.*/
    pParam = (T_CALLPARAM*)WIRELESS_MALLOC(sizeof(T_CALLPARAM));
    if(pParam == NULL)
    {
        WIRELESS_ERROR(WIRELESS_ERRORNO, "Wireless Malloc(%d) Failed\r\n", (sizeof(T_CALLPARAM)));
        return EOS_ERROR;
    }
    pParam->hLine = (HLINE)hLine;
    pParam->hCall = pCall;
    pParam->hATServer = (_UINT32)pParam->hLine->hATServer;
    pParam->requestOwner = eRequest_CallMng;
    pParam->customParam = customParam;
    pParam->event = RIL_NOTIFY_CALLSTATECHG;

    #ifdef WIRELESS_SEND_CLCC
    pParam->pfCallback = SendCLCC_Callback;
    #else
    pParam->pfCallback = NULL;
    #endif

    pParam->cmdCount = 0;
    pParam->cmdIndex = 0;
#if (ACOM_OEM_VER == ASYS_OEMVER_INTERCONN)
    if(hCallManager->activeQty > 0)
#else
    if(0)
#endif    
    {
        /*hold call first*/
        pParam->cmd[pParam->cmdCount].ID = eCALL_HOLDACTIVE;
        pParam->cmd[pParam->cmdCount].index = 0;
        pParam->cmdCount++;
        pParam->cmd[pParam->cmdCount].ID = eCALL_DIAL;
        pParam->cmd[pParam->cmdCount].index = 0;
        pParam->cmdCount++;

        CALL_PARAM_DUMP(pParam);

        rtn = WM_HoldAndMultiparty(((HLINE)hLine)->hATServer->hWM, 
                                    (_UINT32)hCallManager, 
                                    eDEVICECALL, 
                                    (_UINT32)pParam, 
                                    pParam->cmd[pParam->cmdIndex].ID,
                                    pParam->cmd[pParam->cmdIndex].index);
    }
    else
    {
        pParam->cmd[pParam->cmdCount].ID = eCALL_DIAL;
        pParam->cmd[pParam->cmdCount].index = 0;
        pParam->cmdCount++;

        CALL_PARAM_DUMP(pParam);

        rtn = WM_DialCall(((HLINE)hLine)->hATServer->hWM,
                        (_UINT32)hCallManager,
                        eDEVICECALL,
                        (_UINT32)pParam,
                        (_CHAR8*)lpszAddress,
                        (RIL_DATA_CLASS)dwOptions);
    }

    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, (_UINT32)pCall, NULL, 0, 0);
}

_UINT32 CallManager_RejectCall(_UINT32 hLine, HCALL hCall, _UINT32 customParam)
{
    T_CALLPARAM *pParam;
    _UINT32 rtn;
    HCALLMANAGER hCallManager;

    hCallManager = ((HLINE)hLine)->hATServer->hCallManager;

    pParam = (T_CALLPARAM*)WIRELESS_MALLOC(sizeof(T_CALLPARAM));
    if(pParam == NULL)
    {
        WIRELESS_ERROR(WIRELESS_ERRORNO, "Wireless Malloc(%d) Failed\r\n", (sizeof(T_CALLPARAM)));
        return EOS_ERROR;
    }
    pParam->hCall = hCall;
    pParam->hLine = (HLINE)hLine;
    pParam->hATServer = (_UINT32)((HLINE)(pParam->hLine)->hATServer);
    pParam->requestOwner = eRequest_CallMng;
    pParam->customParam = customParam;
    pParam->event = RIL_NOTIFY_CALLSTATECHG;

#ifdef WIRELESS_SEND_CLCC
    pParam->pfCallback = SendCLCC_Callback;
#else
    pParam->pfCallback = NULL;
#endif

    pParam->cmdCount = 0;
    pParam->cmdIndex = 0;
    pParam->cmd[pParam->cmdCount].ID = eCALL_ENDHOLD;
    pParam->cmd[pParam->cmdCount].index = 0;
    pParam->cmdCount++;

    CALL_PARAM_DUMP(pParam);

    rtn = WM_HoldAndMultiparty(((HLINE)hLine)->hATServer->hWM, 
                                (_UINT32)hCallManager, 
                                eDEVICECALL, 
                                (_UINT32)pParam, 
                                pParam->cmd[pParam->cmdIndex].ID,
                                pParam->cmd[pParam->cmdIndex].index);
    
    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 CallManager_AnswerCall(_UINT32 hLine, HCALL hCall, _UINT32 customParam)
{
    T_CALLPARAM *pParam;
    _UINT32 rtn;
    HCALLMANAGER hCallManager;

    hCallManager = ((HLINE)hLine)->hATServer->hCallManager;

    pParam = (T_CALLPARAM*)WIRELESS_MALLOC(sizeof(T_CALLPARAM));
    if(pParam == NULL)
    {
        WIRELESS_ERROR(WIRELESS_ERRORNO, "Wireless Malloc(%d) Failed\r\n", (sizeof(T_CALLPARAM)));
        return EOS_ERROR;
    }
    pParam->hCall = hCall;
    pParam->hLine = (HLINE)hLine;
    pParam->hATServer = (_UINT32)((HLINE)(pParam->hLine)->hATServer);
    pParam->requestOwner = eRequest_CallMng;
    pParam->customParam = customParam;
    pParam->event = RIL_NOTIFY_CALLSTATECHG;
#ifdef WIRELESS_SEND_CLCC
    pParam->pfCallback = SendCLCC_Callback;
#else
    pParam->pfCallback = NULL;
#endif

    pParam->cmdCount = 0;
    pParam->cmdIndex = 0;
    if((hCallManager->activeQty > 0 || hCallManager->holdQty > 0) && (hCall != NULL))
    {
        pParam->cmd[pParam->cmdCount].ID = eCALL_HOLDACTIVE;
        pParam->cmd[pParam->cmdCount].index = (_USHORT16)hCall->idx;
        pParam->cmdCount++;

        CALL_PARAM_DUMP(pParam);
        
        rtn = WM_HoldAndMultiparty(((HLINE)hLine)->hATServer->hWM, 
                                    (_UINT32)hCallManager, 
                                    eDEVICECALL, 
                                    (_UINT32)pParam, 
                                    pParam->cmd[pParam->cmdIndex].ID,
                                    pParam->cmd[pParam->cmdIndex].index);
    }
    else
    {
        pParam->cmd[pParam->cmdCount].ID = eCALL_ANSWER;
        pParam->cmd[pParam->cmdCount].index = 0;
        pParam->cmdCount++;

        CALL_PARAM_DUMP(pParam);

        rtn = WM_AnswerCall(((HLINE)hLine)->hATServer->hWM,
                        (_UINT32)hCallManager,
                        eDEVICECALL,
                        (_UINT32)pParam);
    }
    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 CallManager_HangupCall(_UINT32 hLine, HCALL hCall, _UINT32 customParam)
{
    T_CALLPARAM *pParam;
    _UINT32 rtn;
    HCALLMANAGER hCallManager;

    hCallManager = ((HLINE)hLine)->hATServer->hCallManager;

    pParam = (T_CALLPARAM*)WIRELESS_MALLOC(sizeof(T_CALLPARAM));
    if(pParam == NULL)
    {
        WIRELESS_ERROR(WIRELESS_ERRORNO, "Wireless Malloc(%d) Failed\r\n", (sizeof(T_CALLPARAM)));
        return EOS_ERROR;
    }
    pParam->hCall = hCall;
    pParam->hLine = (HLINE)hLine;
    pParam->hATServer = (_UINT32)((HLINE)(pParam->hLine)->hATServer);
    pParam->requestOwner = eRequest_CallMng;
    pParam->customParam = customParam;
    pParam->event = RIL_NOTIFY_CALLSTATECHG;

#ifdef WIRELESS_SEND_CLCC
    pParam->pfCallback = SendCLCC_Callback;
#else
    pParam->pfCallback = NULL;
#endif

    pParam->cmdCount = 0;
    pParam->cmdIndex = 0;
    pParam->cmd[pParam->cmdCount].ID = eCALL_HANGUP;
    pParam->cmd[pParam->cmdCount].index = 0;
    pParam->cmdCount++;

    CALL_PARAM_DUMP(pParam);

    rtn = WM_DisconnectCall(((HLINE)hLine)->hATServer->hWM,
                            (_UINT32)hCallManager,
                            eDEVICECALL,
                            (_UINT32)pParam);

    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 CallManager_CancelDial(_UINT32 hLine, HCALL hCall, _UINT32 customParam)
{
    T_CALLPARAM *pParam;
    _UINT32 rtn = RIL_RTN_INVALID_HANDLE;
    HCALLMANAGER hCallManager;
    struct list_head *pos;
    T_STRUCTCALL *pCall;
    _BOOL        bFound = EOS_FALSE;

    hCallManager = ((HLINE)hLine)->hATServer->hCallManager;

    WirelessSemP(hCallManager->mutex, EOS_WAIT_FOREVER);
    for (pos = hCallManager->head.next; pos != &hCallManager->head; pos = pos->next)
    {
        pCall = list_entry(pos, T_STRUCTCALL, list);
        if((pCall == hCall) && ((pCall->stat == RIL_DIALING) || (pCall->stat == RIL_ALERTING)))
        {
            bFound = EOS_TRUE;
            break;
        }
    }
    WirelessSemV(hCallManager->mutex);

    if(bFound)
    {
        if((WM_ATDReturnDirectly(((HLINE)hLine)->hATServer->hWM)) && (hCall != NULL))
        {
            pParam = (T_CALLPARAM*)WIRELESS_MALLOC(sizeof(T_CALLPARAM));
            if(pParam == NULL)
            {
                WIRELESS_ERROR(WIRELESS_ERRORNO, "Wireless Malloc(%d) Failed\r\n", (sizeof(T_CALLPARAM)));
                return EOS_ERROR;
            }

            pParam->hCall = hCall;
            pParam->hLine = (HLINE)hLine;
            pParam->hATServer = (_UINT32)((HLINE)(pParam->hLine)->hATServer);
            pParam->requestOwner = eRequest_CallMng;
            pParam->customParam = customParam;
            pParam->event = RIL_NOTIFY_CALLSTATECHG;
#ifdef WIRELESS_SEND_CLCC
            pParam->pfCallback = SendCLCC_Callback;
#else
            pParam->pfCallback = NULL;
#endif

            pParam->cmdCount = 0;
            pParam->cmdIndex = 0;
            pParam->cmd[pParam->cmdCount].ID = eCALL_ENDSINGLE;
            pParam->cmd[pParam->cmdCount].index = (_USHORT16)hCall->idx;
            pParam->cmdCount++;

            CALL_PARAM_DUMP(pParam);

            rtn = WM_HoldAndMultiparty(((HLINE)hLine)->hATServer->hWM, 
                                        (_UINT32)hCallManager, 
                                        eDEVICECALL, 
                                        (_UINT32)pParam, 
                                        pParam->cmd[pParam->cmdIndex].ID,
                                        pParam->cmd[pParam->cmdIndex].index);
        }
        else
        {
            rtn = WM_CancelDial(((HLINE)hLine)->hATServer->hWM, (_UINT32)hCallManager, eDEVICECALL, 0);
        }
    }

    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 CallManager_SwapCall(_UINT32 hLine, HCALL hCall, _UINT32 customParam)
{
    T_CALLPARAM *pParam;
    _UINT32 rtn;
    HCALLMANAGER hCallManager;

    hCallManager = ((HLINE)hLine)->hATServer->hCallManager;

    pParam = (T_CALLPARAM*)WIRELESS_MALLOC(sizeof(T_CALLPARAM));
    if(pParam == NULL)
    {
        WIRELESS_ERROR(WIRELESS_ERRORNO, "Wireless Malloc(%d) Failed\r\n", (sizeof(T_CALLPARAM)));
        return EOS_ERROR;
    }
    pParam->hCall = hCall;
    pParam->hLine = (HLINE)hLine;
    pParam->hATServer = (_UINT32)((HLINE)(pParam->hLine)->hATServer);
    pParam->requestOwner = eRequest_CallMng;
    pParam->customParam = customParam;
    pParam->event = RIL_NOTIFY_CALLSTATECHG;
#ifdef WIRELESS_SEND_CLCC
    pParam->pfCallback = SendCLCC_Callback;
#else
    pParam->pfCallback = NULL;
#endif

    pParam->cmdCount = 0;
    pParam->cmdIndex = 0;
    pParam->cmd[pParam->cmdCount].ID = eCALL_HOLDACTIVE;
    pParam->cmd[pParam->cmdCount].index = 0;
    pParam->cmdCount++;

    CALL_PARAM_DUMP(pParam);

    rtn = WM_HoldAndMultiparty(((HLINE)hLine)->hATServer->hWM, 
                                (_UINT32)hCallManager, 
                                eDEVICECALL, 
                                (_UINT32)pParam, 
                                pParam->cmd[pParam->cmdIndex].ID,
                                pParam->cmd[pParam->cmdIndex].index);
    
    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 CallManager_Conference(_UINT32 hLine, HCALL hCall, _UINT32 customParam)
{
    T_CALLPARAM *pParam;
    _UINT32 rtn;
    HCALLMANAGER hCallManager;

    hCallManager = ((HLINE)hLine)->hATServer->hCallManager;

    pParam = (T_CALLPARAM*)WIRELESS_MALLOC(sizeof(T_CALLPARAM));
    if(pParam == NULL)
    {
        WIRELESS_ERROR(WIRELESS_ERRORNO, "Wireless Malloc(%d) Failed\r\n", (sizeof(T_CALLPARAM)));
        return EOS_ERROR;
    }
    pParam->hCall = hCall;
    pParam->hLine = (HLINE)hLine;
    pParam->hATServer = (_UINT32)((HLINE)(pParam->hLine)->hATServer);
    pParam->requestOwner = eRequest_CallMng;
    pParam->customParam = customParam;
    pParam->event = RIL_NOTIFY_CALLSTATECHG;
#ifdef WIRELESS_SEND_CLCC
    pParam->pfCallback = SendCLCC_Callback;
#else
    pParam->pfCallback = NULL;
#endif

    pParam->cmdCount = 0;
    pParam->cmdIndex = 0;
    pParam->cmd[pParam->cmdCount].ID = eCALL_CONFERENCE;
    pParam->cmd[pParam->cmdCount].index = 0;
    pParam->cmdCount++;

    CALL_PARAM_DUMP(pParam);

    rtn = WM_HoldAndMultiparty(((HLINE)hLine)->hATServer->hWM, 
                                (_UINT32)hCallManager, 
                                eDEVICECALL, 
                                (_UINT32)pParam, 
                                pParam->cmd[pParam->cmdIndex].ID,
                                pParam->cmd[pParam->cmdIndex].index);
    
    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 CallManager_EndSingle(_UINT32 hLine, HCALL hCall, _UINT32 customParam)
{
    T_CALLPARAM *pParam;
    _UINT32 rtn;
    HCALLMANAGER hCallManager;

    hCallManager = ((HLINE)hLine)->hATServer->hCallManager;

    pParam = (T_CALLPARAM*)WIRELESS_MALLOC(sizeof(T_CALLPARAM));
    if(pParam == NULL)
    {
        WIRELESS_ERROR(WIRELESS_ERRORNO, "Wireless Malloc(%d) Failed\r\n", (sizeof(T_CALLPARAM)));
        return EOS_ERROR;
    }
    pParam->hCall = hCall;
    pParam->hLine = (HLINE)hLine;
    pParam->hATServer = (_UINT32)((HLINE)(pParam->hLine)->hATServer);
    pParam->requestOwner = eRequest_CallMng;
    pParam->customParam = customParam;
    pParam->event = RIL_NOTIFY_CALLSTATECHG;
#ifdef WIRELESS_SEND_CLCC
    pParam->pfCallback = SendCLCC_Callback;
#else
    pParam->pfCallback = NULL;
#endif

    pParam->cmdCount = 0;
    pParam->cmdIndex = 0;
    pParam->cmd[pParam->cmdCount].ID = eCALL_ENDSINGLE;
    pParam->cmd[pParam->cmdCount].index = (_USHORT16)hCall->idx;
    pParam->cmdCount++;

    CALL_PARAM_DUMP(pParam);

    rtn = WM_HoldAndMultiparty(((HLINE)hLine)->hATServer->hWM, 
                                (_UINT32)hCallManager, 
                                eDEVICECALL, 
                                (_UINT32)pParam, 
                                pParam->cmd[pParam->cmdIndex].ID,
                                pParam->cmd[pParam->cmdIndex].index);
    
    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 CallManager_EndActive(_UINT32 hLine, HCALL hCall, _UINT32 customParam)
{
    T_CALLPARAM *pParam;
    _UINT32 rtn;
    HCALLMANAGER hCallManager;

    hCallManager = ((HLINE)hLine)->hATServer->hCallManager;

    pParam = (T_CALLPARAM*)WIRELESS_MALLOC(sizeof(T_CALLPARAM));
    if(pParam == NULL)
    {
        WIRELESS_ERROR(WIRELESS_ERRORNO, "Wireless Malloc(%d) Failed\r\n", (sizeof(T_CALLPARAM)));
        return EOS_ERROR;
    }
    pParam->hCall = hCall;
    pParam->hLine = (HLINE)hLine;
    pParam->hATServer = (_UINT32)((HLINE)(pParam->hLine)->hATServer);
    pParam->requestOwner = eRequest_CallMng;
    pParam->customParam = customParam;
    pParam->event = RIL_NOTIFY_CALLSTATECHG;
#ifdef WIRELESS_SEND_CLCC
    pParam->pfCallback = SendCLCC_Callback;
#else
    pParam->pfCallback = NULL;
#endif

    pParam->cmdCount = 0;
    pParam->cmdIndex = 0;
    pParam->cmd[pParam->cmdCount].ID = eCALL_ENDACTIVE;
    pParam->cmd[pParam->cmdCount].index = 0;
    pParam->cmdCount++;

    CALL_PARAM_DUMP(pParam);

    rtn = WM_HoldAndMultiparty(((HLINE)hLine)->hATServer->hWM, 
                                (_UINT32)hCallManager, 
                                eDEVICECALL, 
                                (_UINT32)pParam, 
                                pParam->cmd[pParam->cmdIndex].ID,
                                pParam->cmd[pParam->cmdIndex].index);
    
    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 CallManager_EndHold(_UINT32 hLine, HCALL hCall, _UINT32 customParam)
{
    T_CALLPARAM *pParam;
    _UINT32 rtn;
    HCALLMANAGER hCallManager;

    hCallManager = ((HLINE)hLine)->hATServer->hCallManager;

    pParam = (T_CALLPARAM*)WIRELESS_MALLOC(sizeof(T_CALLPARAM));
    if(pParam == NULL)
    {
        WIRELESS_ERROR(WIRELESS_ERRORNO, "Wireless Malloc(%d) Failed\r\n", (sizeof(T_CALLPARAM)));
        return EOS_ERROR;
    }
    pParam->hCall = hCall;
    pParam->hLine = (HLINE)hLine;
    pParam->hATServer = (_UINT32)((HLINE)(pParam->hLine)->hATServer);
    pParam->requestOwner = eRequest_CallMng;
    pParam->customParam = customParam;
    pParam->event = RIL_NOTIFY_CALLSTATECHG;
#ifdef WIRELESS_SEND_CLCC
    pParam->pfCallback = SendCLCC_Callback;
#else
    pParam->pfCallback = NULL;
#endif
    pParam->cmdCount = 0;
    pParam->cmdIndex = 0;
    if(hCallManager->WaitQty == 0)
    {
        pParam->cmd[pParam->cmdCount].ID = eCALL_ENDHOLD;
        pParam->cmd[pParam->cmdCount].index = 0;
        pParam->cmdCount++;
    }
    else
    {
        pParam->cmd[pParam->cmdCount].ID = eCALL_HOLDACTIVE;
        pParam->cmd[pParam->cmdCount].index = 0;
        pParam->cmdCount++;
        pParam->cmd[pParam->cmdCount].ID = eCALL_ENDACTIVE;
        pParam->cmd[pParam->cmdCount].index = 0;
        pParam->cmdCount++;
        pParam->cmd[pParam->cmdCount].ID = eCALL_HOLDACTIVE;
        pParam->cmd[pParam->cmdCount].index = 0;
        pParam->cmdCount++;
    }

    CALL_PARAM_DUMP(pParam);

    rtn = WM_HoldAndMultiparty(((HLINE)hLine)->hATServer->hWM, 
                                (_UINT32)hCallManager, 
                                eDEVICECALL,
                                (_UINT32)pParam, 
                                pParam->cmd[pParam->cmdIndex].ID,
                                pParam->cmd[pParam->cmdIndex].index);
    
    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}

_UINT32 CallManager_SplitFromConference(_UINT32 hLine, HCALL hCall, _UINT32 customParam)
{
    T_CALLPARAM *pParam;
    _UINT32 rtn;
    HCALLMANAGER hCallManager;

    hCallManager = ((HLINE)hLine)->hATServer->hCallManager;

    pParam = (T_CALLPARAM*)WIRELESS_MALLOC(sizeof(T_CALLPARAM));
    if(pParam == NULL)
    {
        WIRELESS_ERROR(WIRELESS_ERRORNO, "Wireless Malloc(%d) Failed\r\n", (sizeof(T_CALLPARAM)));
        return EOS_ERROR;
    }
    pParam->hCall = hCall;
    pParam->hLine = (HLINE)hLine;
    pParam->hATServer = (_UINT32)((HLINE)(pParam->hLine)->hATServer);
    pParam->requestOwner = eRequest_CallMng;
    pParam->customParam = customParam;
    pParam->event = RIL_NOTIFY_CALLSTATECHG;
#ifdef WIRELESS_SEND_CLCC
    pParam->pfCallback = SendCLCC_Callback;
#else
    pParam->pfCallback = NULL;
#endif

    pParam->cmdCount = 0;
    pParam->cmdIndex = 0;
    pParam->cmd[pParam->cmdCount].ID = eCALL_HOLDACTIVEEXP;
    pParam->cmd[pParam->cmdCount].index = (_USHORT16)hCall->idx;
    pParam->cmdCount++;

    CALL_PARAM_DUMP(pParam);

    rtn = WM_HoldAndMultiparty(((HLINE)hLine)->hATServer->hWM, 
                                (_UINT32)hCallManager, 
                                eDEVICECALL, 
                                (_UINT32)pParam, 
                                pParam->cmd[pParam->cmdIndex].ID,
                                pParam->cmd[pParam->cmdIndex].index);
    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, rtn, 0, NULL, 0, 0);
}


_UINT32 CallManager_ListCall(_UINT32 hLine, _UINT32 customParam)
{
    T_CALLPARAM *pParam;
    HCALLMANAGER hCallManager;

    hCallManager = ((HLINE)hLine)->hATServer->hCallManager;

    pParam = (T_CALLPARAM*)WIRELESS_MALLOC(sizeof(T_CALLPARAM));
    if(pParam == NULL)
    {
        WIRELESS_ERROR(WIRELESS_ERRORNO, "Wireless Malloc(%d) Failed\r\n", (sizeof(T_CALLPARAM)));
        return EOS_ERROR;
    }
    pParam->hCall = NULL;
    pParam->hLine = (HLINE)hLine;
    pParam->hATServer = (_UINT32)((HLINE)(pParam->hLine)->hATServer);
    pParam->requestOwner = eRequest_CallMng;
    pParam->customParam = customParam;
    pParam->event = RIL_NOTIFY_CALLSTATECHG;
    pParam->pfCallback = ParseCLCC_Callback;

    pParam->cmdCount = 0;
    pParam->cmdIndex = 0;
    
    CALL_PARAM_DUMP(pParam);

    return WM_ListCurCalls(((HLINE)hLine)->hATServer->hWM, 
                                (_UINT32)hCallManager, 
                                eDEVICECALL,
                                (_UINT32)pParam);
}

_UINT32 CallManager_GetCallInfo(_UINT32 hLine, HCALL hCall)
{
    RIL_CLCC_ENTRY clcc[MAX_CALL_NUM];
    HCALLMANAGER hCallManager;
    struct list_head *pos;
    T_STRUCTCALL *pCall;
    _UINT32        i = 0;

    hCallManager = ((HLINE)hLine)->hATServer->hCallManager;

    WirelessSemP(hCallManager->mutex, EOS_WAIT_FOREVER);
    if((_UINT32)hCall == 0)
    {
        /*not specify call handle, return all calls info*/
        for (pos = hCallManager->head.next; pos != &hCallManager->head; pos = pos->next)
        {
            pCall = list_entry(pos, T_STRUCTCALL, list);

            clcc[i].hCallHandle = (_UINT32)pCall;
            clcc[i].dir = pCall->dir;
            clcc[i].idx = pCall->idx;
            clcc[i].mode = pCall->mode;
            clcc[i].mpty = pCall->mpty;
            clcc[i].stat = pCall->stat;
            clcc[i].type = pCall->type;
            strcpy((_CHAR8*)clcc[i].number, (const _CHAR8*)pCall->number);
            i++;
        }
    }
    else
    {
        /*only get the specified call info*/
        for (pos = hCallManager->head.next; pos != &hCallManager->head; pos = pos->next)
        {
            pCall = list_entry(pos, T_STRUCTCALL, list);

            if(pCall == hCall)
            {
                clcc[i].hCallHandle = (_UINT32)pCall;
                clcc[i].dir = pCall->dir;
                clcc[i].idx = pCall->idx;
                clcc[i].mode = pCall->mode;
                clcc[i].mpty = pCall->mpty;
                clcc[i].stat = pCall->stat;
                clcc[i].type = pCall->type;
                strcpy((_CHAR8*)clcc[i].number, (const _CHAR8*)pCall->number);
                i++;
                break;
            }
        }
    }
    WirelessSemV(hCallManager->mutex);

    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, RIL_RTN_OK, 0, clcc, sizeof(RIL_CLCC_ENTRY)*i, i);
}

_UINT32 CallManager_GetCallQty(_UINT32 hLine, RIL_CALL_STATUS status)
{
    HCALLMANAGER hCallManager;
    _UINT32        i = 0;

    hCallManager = ((HLINE)hLine)->hATServer->hCallManager;
    switch(status)
    {
    case RIL_ACTIVE:
        i = hCallManager->activeQty;
        break;
    case RIL_HOLD:
        i = hCallManager->holdQty;
        break;
    case RIL_DIALING:
    case RIL_ALERTING:
        i = hCallManager->DialQty;
        break;
    case RIL_INCOMING:
    case RIL_WAITING:
        i = hCallManager->WaitQty;
        break;
    case RIL_CALL_STATUS_NUM:
        i = hCallManager->callQty;
        break;
    default:
        break;
    }

    return Line_SendReplay(((HLINE)hLine)->ChannelID, hLine, RIL_RTN_OK, i, NULL, 0, 0);
}

_UINT32 CallManager_ResultCallback(_UINT32 ChannelId, HCALLMANAGER hCallManager, 
                              _UINT32 resultType, _VOID* lpData, _UINT32 totalSize, _UINT32 usedSize,
                              _UINT32 customParam)
{
    PWIRELESSROOTST pWirelessRootSt;
    PWIRELESSCHANNELST  pWirelessChannelCtl;
    T_CALLPARAM *pParam;

    pWirelessRootSt = (PWIRELESSROOTST)&stWirelessRootSt;
    pWirelessChannelCtl = &pWirelessRootSt->stChannelSt[ChannelId];

    if(0 == customParam)
    {
        return 0;
    }

    CALL_PARAM_DUMP(customParam);
    pParam = (T_CALLPARAM*)customParam;

#if 0
    if(pParam->requestOwner == eRequest_Line)
    {
        Line_ResultCallback(pParam->hLine, resultType, lpData, totalSize, usedSize, pParam->customParam);
        pParam->customParam = NULL;
    }
#endif
    if(NULL != pParam->pfCallback)
    {
        pParam->pfCallback(ChannelId, hCallManager, resultType, lpData, totalSize, usedSize, customParam);
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
    
    return 0;
}

/*
 *Call manager$)A4&@mURC;X5w:/J}
 */
_UINT32 CallManager_URCCallback(_UINT32 hATServer, _UINT32 eventType, _UINT32 eventID, _VOID* lpData, _UINT32 totalSize, _UINT32 usedSize)
{
    WIRELESS_INFO("CallManager_URCCallback eventID = %d\r\n", eventID);
    switch(eventID)
    {
        case RIL_NOTIFY_RING: /*RING event*/
        case RIL_NOTIFY_CALLWAITING:
        {
            T_CALLPARAM *pParam;

            pParam = (T_CALLPARAM*)WIRELESS_MALLOC(sizeof(T_CALLPARAM));
            if(pParam == NULL)
            {
                WIRELESS_ERROR(WIRELESS_ERRORNO, "Wireless Malloc(%d) Failed\r\n", (sizeof(T_CALLPARAM)));
                return EOS_ERROR;
            }

            pParam->hCall = NULL;
            pParam->hLine = NULL;
            pParam->hATServer = hATServer;
            pParam->requestOwner = eRequest_CallMng;
            pParam->event = eventID;
            pParam->pfCallback = ParseCLCC_Callback;

            /* 2011.12.18 pengjian deleted to decrese the log for debug */
            /* WIRELESS_INFO("CallManager_URCCallback(%s)(%d)\r\n", __FILE__, __LINE__); */
            ATServer_SetConnState((HATSERVER)hATServer, CONNSTATE_RINGON);
            
            CALL_PARAM_DUMP(pParam);

            WM_ListCurCalls(((HATSERVER)hATServer)->hWM,
                            (_UINT32)((HATSERVER)hATServer)->hCallManager,
                            eDEVICECALL,
                            (_UINT32)pParam);
            break;
        }
        case RIL_NOTIFY_CONNECT:
        case RIL_NOTIFY_DISCONNECT:
        case RIL_NOTIFY_NOANSWER:
        case RIL_NOTIFY_BUSY:
        case RIL_NOTIFY_NODIALTONE:
        {
            T_CALLPARAM *pParam;

            pParam = (T_CALLPARAM*)WIRELESS_MALLOC(sizeof(T_CALLPARAM));
            if(pParam == NULL)
            {
                WIRELESS_ERROR(WIRELESS_ERRORNO, "Wireless Malloc(%d) Failed\r\n", (sizeof(T_CALLPARAM)));
                return EOS_ERROR;
            }
            pParam->hCall = NULL;
            pParam->hLine = NULL;
            pParam->hATServer = hATServer;
            pParam->requestOwner = eRequest_CallMng;
            pParam->event = eventID;
            pParam->pfCallback = ParseCLCC_Callback;

            /* 2011.12.18 pengjian deleted to decrese the log for debug */
            /* WIRELESS_INFO("CallManager_URCCallback(%s)(%d)\r\n", __FILE__, __LINE__); */
            CALL_PARAM_DUMP(pParam);

            WM_ListCurCalls(((HATSERVER)hATServer)->hWM, 
                            (_UINT32)((HATSERVER)hATServer)->hCallManager,
                            eDEVICECALL,
                            (_UINT32)pParam);
            break;
        }
        default:
            Line_URCCallback((HATSERVER)hATServer, eventType, eventID, 0, lpData, totalSize, usedSize);
    }
    return 0;
}


_UINT32 ParseCLCC_Callback(_UINT32 ChannelId, HCALLMANAGER hCallManager, 
                              _UINT32 resultType, _VOID* lpData, _UINT32 totalSize, _UINT32 usedSize,
                              _UINT32 customParam)
{
    T_CALLPARAM *pParam;
    PWIRELESSROOTST pWirelessRootSt;
    PWIRELESSCHANNELST  pWirelessChannelCtl;

    pWirelessRootSt = (PWIRELESSROOTST)&stWirelessRootSt;
    pWirelessChannelCtl = &pWirelessRootSt->stChannelSt[ChannelId];

    pParam = (T_CALLPARAM*)customParam;
    
    if((pWirelessChannelCtl->gb_hATServer) != (pParam->hATServer))
    {
        WIRELESS_ERROR((pParam->hATServer),"ParseCLCC_Callback hATServer Error!!!\r\n");
    }
    
    if(pParam->event == RIL_NOTIFY_RING || RIL_NOTIFY_CALLWAITING == pParam->event)
    {
        ATServer_SetConnState((HATSERVER)pWirelessChannelCtl->gb_hATServer, CONNSTATE_RINGOFF);
    }

    if(resultType != RIL_prOK)
    {
        WIRELESS_FREE(pParam);
        WIRELESS_ERROR(WIRELESS_ERRORNO, "Channel %d ParseCLCC Error\r\n", ChannelId);
    }
    else
    {
        T_STRUCTCALL    *pCall, *pNewCall;
        struct list_head *pos;
        RIL_CLCC_ENTRY   *pClcc;
        _UINT32            i;
        _BOOL            update = EOS_FALSE;
        _BOOL            found = EOS_FALSE;
        _UINT32            waitCallHandle = 0;
        HCALL            newCallHandle = NULL;
        _USHORT16            activeQty = 0;        /*total active calls number*/
        _USHORT16            holdQty = 0;        /*total hold calls number*/
        _USHORT16            DialQty = 0;        /*total Dialing calls number*/
        _USHORT16            WaitQty = 0;        /*total Waiting calls number*/

        WirelessSemP(hCallManager->mutex, EOS_WAIT_FOREVER);

        /*clean the read flag of the call handle to unread.*/
        for (pos = hCallManager->head.next; pos != &hCallManager->head; pos = pos->next)
        {
            pCall = list_entry(pos, T_STRUCTCALL, list);
            pCall->readflag = 0;
        }

        /*match each CLCC struct with the call handles in the queue.*/
        pClcc = (RIL_CLCC_ENTRY*)lpData;
        for(i = 0; i < usedSize; i++, pClcc++)
        {
            found = EOS_FALSE;

            switch(pClcc->stat) 
            {            
                case RIL_ACTIVE:
                    activeQty++;
                    break;
                case RIL_HOLD:
                    holdQty++;
                    break;
                case RIL_DIALING:
                case RIL_ALERTING:
                    DialQty++;
                    break;
                case RIL_INCOMING:
                case RIL_WAITING:
                    WaitQty++;
                    break;
                default:
                    break;
            }

            for (pos = hCallManager->head.next; pos != &hCallManager->head; pos = pos->next)
            {
                pCall = list_entry(pos, T_STRUCTCALL, list);
                
                /*Found this call in the call handle queue*/
                if(pCall->idx == pClcc->idx)
                {
                    found = EOS_TRUE;
                    pCall->readflag = 1;

                    if(RIL_INCOMING == pClcc->stat || RIL_WAITING == pClcc->stat)
                    {
                        waitCallHandle = (_UINT32)pCall;
                    }

                    if(pCall->stat != pClcc->stat)
                    {
                        pCall->stat = pClcc->stat;
                        update = EOS_TRUE;
                    }
                    break;
                }
            }

            /*Didn't find clcc entry in the call handle queue*/
            if(found == EOS_FALSE)
            {
                /*Handle to the dialing-call already created in the MAKE-CALL function*/
                if((pParam->hCall != NULL) && (pClcc->stat == RIL_DIALING) && (pWirelessChannelCtl->hCallHandle != NULL))
                {
                    pParam->hCall->idx = pClcc->idx;
                    pParam->hCall->mode = pClcc->mode;
                    pParam->hCall->stat = pClcc->stat;

                    /* 2011.12.15 pengjian replaced to avoid overflow */
                    /* strcpy((_CHAR8*)pParam->hCall->number, (const _CHAR8*)pClcc->number); */
                    UT_BUF_STRCPY(pParam->hCall->number, pClcc->number);
                    
                    pParam->hCall->type = pClcc->type;
                    pParam->hCall->readflag = 1;
                    update = EOS_TRUE;

                    pWirelessChannelCtl->iCallIndex = pClcc->idx;
                    WIRELESS_INFO("Channel %d Call Index:%d\r\n", ChannelId, pWirelessChannelCtl->iCallIndex);
                }
                else
                {
                    /*Append new call handle refer to call state*/
                    pNewCall = (T_STRUCTCALL*)WIRELESS_MALLOC(sizeof(T_STRUCTCALL));
                    if(pNewCall == NULL)
                    {
                        WIRELESS_ERROR(WIRELESS_ERRORNO, "Wireless Malloc(%d) Failed\r\n", (sizeof(T_STRUCTCALL)));
                        return EOS_ERROR;
                    }
                    pNewCall->dir = pClcc->dir;
                    pNewCall->idx = pClcc->idx;
                    pNewCall->mode = pClcc->mode;
                    pNewCall->mpty = pClcc->mpty;
                    strcpy((_CHAR8*)pNewCall->number, (const _CHAR8*)pClcc->number);
                    /*pNewCall->owner = hLine; //Check the line handles to see witch one has the owner privilege of the Ring event*/
                    pNewCall->stat = pClcc->stat;
                    pNewCall->type = pClcc->type;
                    pNewCall->readflag = 1;
                    ril_list_add(&(pNewCall->list), &(hCallManager->head));
                    newCallHandle = pNewCall;

                    pWirelessChannelCtl->iCallIndex = pClcc->idx;
                    WIRELESS_INFO("Channel %d (New)Call Index:%d\r\n", ChannelId, pWirelessChannelCtl->iCallIndex);
                }
            }
        }

        /*clean all unread call handle*/
        for (pos = hCallManager->head.next; pos != &hCallManager->head; )
        {
            pCall = list_entry(pos, T_STRUCTCALL, list);

            /* 2011.12.15 pengjian modified to save next node */
            pos = pos->next;
            
            if(pCall->readflag == 0)
            {
                /* 2011.12.15 pengjian replaced to del current node */
                /* ril_list_del(pos); */
                ril_list_del(&pCall->list);
                WIRELESS_FREE(pCall);
                /* make hCallHandle Empty too.kinson 2011-12-17 */
                pWirelessChannelCtl->hCallHandle = NULL;
                update = EOS_TRUE;
            }
        }
        WirelessSemV(hCallManager->mutex);

#if 0
        if(NVM_GOIP_MODTYPE_C202 == pWirelessChannelCtl->ModuleType)
        {
            update = EOS_TRUE;
        }
#else
        update = EOS_TRUE;
#endif
        
#if (ACOM_OEM_VER != ASYS_OEMVER_INTERCONN)
        if(update)
#endif        
        {
            hCallManager->callQty = (_USHORT16)usedSize;
            hCallManager->activeQty = activeQty;
            hCallManager->holdQty = holdQty;
            hCallManager->DialQty = DialQty;
            hCallManager->WaitQty = WaitQty;
                        
            if(RIL_NOTIFY_RING == pParam->event)
            {
                Line_URCCallback((HATSERVER)pWirelessChannelCtl->gb_hATServer, WM_TYPE_CALL, RIL_NOTIFY_CALLSTATECHG, CALLSTATECHG_RING, NULL, 0, 0);
            }
            else if(RIL_NOTIFY_CALLWAITING == pParam->event)
            {
                Line_URCCallback((HATSERVER)pWirelessChannelCtl->gb_hATServer, WM_TYPE_CALL, RIL_NOTIFY_CALLSTATECHG, CALLSTATECHG_CCWA, NULL, 0, 0);
            }
            else if(RIL_NOTIFY_CONNECT == pParam->event)
            {
                Line_URCCallback((HATSERVER)pWirelessChannelCtl->gb_hATServer, WM_TYPE_CALL, RIL_NOTIFY_CALLSTATECHG, CALLSTATECHG_CONNECT, NULL, 0, 0);
            }
            else if(RIL_NOTIFY_DISCONNECT == pParam->event)
            {
                Line_URCCallback((HATSERVER)pWirelessChannelCtl->gb_hATServer, WM_TYPE_CALL, RIL_NOTIFY_CALLSTATECHG, CALLSTATECHG_NOCARRIER, NULL, 0, 0);
            }
            else if(RIL_NOTIFY_NOANSWER == pParam->event)
            {
                Line_URCCallback((HATSERVER)pWirelessChannelCtl->gb_hATServer, WM_TYPE_CALL, RIL_NOTIFY_CALLSTATECHG, CALLSTATECHG_NOANSWER, NULL, 0, 0);
            }
            else if(RIL_NOTIFY_BUSY == pParam->event)
            {
                Line_URCCallback((HATSERVER)pWirelessChannelCtl->gb_hATServer, WM_TYPE_CALL, RIL_NOTIFY_CALLSTATECHG, CALLSTATECHG_BUSY, NULL, 0, 0);
            }
            else if(RIL_NOTIFY_NODIALTONE == pParam->event)
            {
                Line_URCCallback((HATSERVER)pWirelessChannelCtl->gb_hATServer, WM_TYPE_CALL, RIL_NOTIFY_CALLSTATECHG, CALLSTATECHG_NODIALTONE, NULL, 0, 0);
            }
            else
            {
                WIRELESS_INFO("!!! CALLSTATECHG_MANUAL (%s)(%d) !!!\r\n", __FILE__, __LINE__);

            /* 2011.12.15 pengjian modified to dump the pParam */
            #if 1
                Line_URCCallback((HATSERVER)pParam->hATServer, WM_TYPE_CALL, RIL_NOTIFY_CALLSTATECHG, CALLSTATECHG_MANUAL, NULL, 0, 0);
            #else
                Line_URCCallback((HATSERVER)pWirelessChannelCtl->gb_hATServer, WM_TYPE_CALL, RIL_NOTIFY_CALLSTATECHG, CALLSTATECHG_MANUAL, NULL, 0, 0);
            #endif
            }

            if(hCallManager->callQty == 1 && hCallManager->WaitQty == 1)
            {
                Line_URCCallback((HATSERVER)pWirelessChannelCtl->gb_hATServer, WM_TYPE_CALL, RIL_NOTIFY_RING, waitCallHandle, NULL, 0, 0);
            }
        }

        if(NULL != newCallHandle)
        {
            hCallManager->callQty = (_USHORT16)usedSize;
            hCallManager->DialQty = DialQty;
            hCallManager->WaitQty = WaitQty;

            WIRELESS_INFO("ParseCLCC_Callback (%s)(%d), newCallHandle->stat = %d\r\n", __FILE__, __LINE__, newCallHandle->stat);

            pWirelessChannelCtl->hCallHandle = (_UINT32)newCallHandle;

            if(RIL_DIALING == newCallHandle->stat || RIL_ALERTING == newCallHandle->stat)
            {
                WIRELESS_INFO("!!! CALLSTATECHG_MANUAL (%s)(%d) hATServer = 0x%x!!!\r\n", __FILE__, __LINE__, pParam->hATServer);
            /* 2011.12.15 pengjian modified to dump the pParam */
            #if 1
                Line_URCCallback((HATSERVER)pParam->hATServer, WM_TYPE_CALL, RIL_NOTIFY_CALLSTATECHG, CALLSTATECHG_MANUAL, NULL, 0, 0);
            #else
                Line_URCCallback((HATSERVER)pWirelessChannelCtl->gb_hATServer, WM_TYPE_CALL, RIL_NOTIFY_CALLSTATECHG, CALLSTATECHG_MANUAL, NULL, 0, 0);
            #endif
                Line_URCCallback((HATSERVER)pWirelessChannelCtl->gb_hATServer, WM_TYPE_CALL, RIL_NOTIFY_CALLDIALING, (_UINT32)newCallHandle, NULL, 0, 0);
            }
            else if(RIL_INCOMING == newCallHandle->stat || RIL_WAITING == newCallHandle->stat)
            {
                if(hCallManager->callQty == 1)
                {
                    Line_URCCallback((HATSERVER)pWirelessChannelCtl->gb_hATServer, WM_TYPE_CALL, RIL_NOTIFY_CALLSTATECHG, CALLSTATECHG_RING, NULL, 0, 0);
                    Line_URCCallback((HATSERVER)pWirelessChannelCtl->gb_hATServer, WM_TYPE_CALL, RIL_NOTIFY_RING, (_UINT32)newCallHandle, NULL, 0, 0);
                }
                else
                {
                    Line_URCCallback((HATSERVER)pWirelessChannelCtl->gb_hATServer, WM_TYPE_CALL, RIL_NOTIFY_CALLSTATECHG, CALLSTATECHG_CCWA, NULL, 0, 0);
                    Line_URCCallback((HATSERVER)pWirelessChannelCtl->gb_hATServer, WM_TYPE_CALL, RIL_NOTIFY_CALLWAITING, (_UINT32)newCallHandle, NULL, 0, 0);
                }
            }
        }

        WIRELESS_FREE(lpData);
        WIRELESS_FREE(pParam);
    }
    return 0;
}

_UINT32 SendCLCC_Callback(_UINT32 ChannelId, HCALLMANAGER hCallManager, 
                              _UINT32 resultType, _VOID* lpData, _UINT32 totalSize, _UINT32 usedSize,
                              _UINT32 customParam)
{
    T_CALLPARAM* pParam;
    PWIRELESSROOTST pWirelessRootSt;
    PWIRELESSCHANNELST  pWirelessChannelCtl;

    pWirelessRootSt = (PWIRELESSROOTST)&stWirelessRootSt;
    pWirelessChannelCtl = &pWirelessRootSt->stChannelSt[ChannelId];

    if(0 == customParam)
    {
        return 0;
    }

    pParam = (T_CALLPARAM*)customParam;

    if(resultType != RIL_prOK)
    {
        #if 0
        pParam->cmdIndex = pParam->cmdCount;
        pParam->event = RIL_NOTIFY_DISCONNECT;
        #else
        WIRELESS_ERROR(WIRELESS_ERRORNO, "Channel %d SendCLCC_CALLBACK Error\r\n", ChannelId);
        printf("Channel %d SendCLCC_CALLBACK Error\r\n", ChannelId);
        if(pParam != NULL)
        {
            if(pParam->customParam != NULL)
            {
                WIRELESS_FREE(pParam->customParam);
            }        
            WIRELESS_FREE(pParam);
        }
        
        Line_URCCallback((HATSERVER)pWirelessChannelCtl->gb_hATServer, WM_TYPE_CALL, RIL_NOTIFY_CALLSTATECHG, CALLSTATECHG_NOCARRIER, NULL, 0, 0);
        pParam->customParam = NULL;
        return EOS_OK;
        #endif
    }
    else
    {
        pParam->cmdIndex++;

#if (ACOM_OEM_VER == ASYS_OEMVER_INTERCONN)
        /*send next cmd*/
        if(pParam->cmdIndex < pParam->cmdCount)
        {
            switch(pParam->cmd[pParam->cmdIndex].ID) {
            case eCALL_DIAL:
                return WM_DialCall(pParam->hLine->hATServer->hWM,(_UINT32)hCallManager,eDEVICECALL,customParam,pParam->hCall->number,pParam->hCall->callClass);
            case eCALL_ANSWER:
                return WM_AnswerCall(pParam->hLine->hATServer->hWM,(_UINT32)hCallManager,eDEVICECALL,customParam);
            case eCALL_HANGUP:
                return WM_DisconnectCall(pParam->hLine->hATServer->hWM,(_UINT32)hCallManager,eDEVICECALL,customParam);
            case eCALL_ENDHOLD:
            case eCALL_ENDACTIVE:
            case eCALL_ENDSINGLE:
            case eCALL_HOLDACTIVE:
            case eCALL_HOLDACTIVEEXP:
            case eCALL_CONFERENCE:
                return WM_HoldAndMultiparty(pParam->hLine->hATServer->hWM,(_UINT32)hCallManager,eDEVICECALL,customParam,pParam->cmd[pParam->cmdIndex].ID,pParam->cmd[pParam->cmdIndex].index);
            default:
                break;
            }
        }    
#endif
    }

    /*send over or met error*/
    Line_ResultCallback(pParam->hLine, resultType, lpData, totalSize, usedSize, pParam->customParam);
    pParam->customParam = NULL;

    pParam->requestOwner = eRequest_CallMng;
    pParam->pfCallback = ParseCLCC_Callback;
    return WM_ListCurCalls(pParam->hLine->hATServer->hWM, (_UINT32)hCallManager,eDEVICECALL, customParam);
}

