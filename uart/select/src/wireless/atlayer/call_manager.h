/**
 * @file call_manager.h
 * @brief Define call manager interface
*/
#ifndef CALL_MANAGER_H
#define CALL_MANAGER_H

typedef struct tagstructcall* HCALL;
typedef struct tagcallmanager* HCALLMANAGER;

_UINT32 CallManager_Init(HCALLMANAGER *phCallManager);
_UINT32 CallManager_DeInit(HCALLMANAGER hCallManager);

_UINT32 CallManager_CallForward(_UINT32 hLine, _UINT32 customParam, RIL_CCFC_FWDCALLS *cfData);
_UINT32 CallManager_InquireCallFwd(_UINT32 hLine, _UINT32 customParam, RIL_CCFC_REAS reason, RIL_DATA_CLASS Class);
_UINT32 CallManager_SetCallWait(_UINT32 hLine, _UINT32 customParam,_BOOL bURCEnable, _BOOL bEnable, RIL_DATA_CLASS eClass);
_UINT32 CallManager_InquireCallWait(_UINT32 hLine, _UINT32 customParam, RIL_DATA_CLASS eClass);

_UINT32 CallManager_MakeCall(_UINT32 hLine, const _UCHAR8* lpszAddress, _UINT32 dwType, _UINT32 dwOptions, _UINT32 customParam);
_UINT32 CallManager_RejectCall(_UINT32 hLine, HCALL hCall, _UINT32 customParam);
_UINT32 CallManager_AnswerCall(_UINT32 hLine, HCALL hCall, _UINT32 customParam);
_UINT32 CallManager_HangupCall(_UINT32 hLine, HCALL hCall, _UINT32 customParam);
_UINT32 CallManager_CancelDial(_UINT32 hLine, HCALL hCall, _UINT32 customParam);
_UINT32 CallManager_SwapCall(_UINT32 hLine, HCALL hCall, _UINT32 customParam);
_UINT32 CallManager_Conference(_UINT32 hLine, HCALL hCall, _UINT32 customParam);
_UINT32 CallManager_EndSingle(_UINT32 hLine, HCALL hCall, _UINT32 customParam);
_UINT32 CallManager_EndActive(_UINT32 hLine, HCALL hCall, _UINT32 customParam);
_UINT32 CallManager_EndHold(_UINT32 hLine, HCALL hCall, _UINT32 customParam);
_UINT32 CallManager_SplitFromConference(_UINT32 hLine, HCALL hCall, _UINT32 customParam);
/*列出当前电话，重新更新电话状态
 *注意该接口只能在库内部调用
 */
_UINT32 CallManager_ListCall(_UINT32 hLine, _UINT32 customParam);

_UINT32 CallManager_GetCallInfo(_UINT32 hLine, HCALL hCall);
_UINT32 CallManager_GetCallQty(_UINT32 hLine, RIL_CALL_STATUS status);

_UINT32 CallManager_ResultCallback(_UINT32 ChannelId, HCALLMANAGER hCallManager, 
                              _UINT32 resultType, _VOID* lpData, _UINT32 totalSize, _UINT32 usedSize,
                              _UINT32 customParam);
_UINT32 CallManager_URCCallback(_UINT32 hATServer, _UINT32 eventType, _UINT32 eventID, _VOID* lpData, _UINT32 totalSize, _UINT32 usedSize);

/* 2011.12.16 pengjian added for test */
_VOID* CallManagerParamDump(_UINT32 uiParam, const _CHAR8* file, _INT32 line);

#if 0
#define CALL_PARAM_DUMP(uiParam) CallManagerParamDump((_UINT32)uiParam, __FILE__, __LINE__)
#else
#define CALL_PARAM_DUMP(uiParam) 
#endif

#endif

