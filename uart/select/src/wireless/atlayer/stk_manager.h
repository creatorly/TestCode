/**
 * @file stk_manager.h
 * @brief Define stk manager interface
 */
#ifndef __STK_MANAGER__H__
#define __STK_MANAGER__H__


typedef struct tagstkmanager* HSTKMANAGER;

_UINT32 STKManager_Init(HSTKMANAGER *phSTKManager);
_UINT32 STKManager_DeInit(HSTKMANAGER hSTKManager);

_UINT32 STKManager_SetProfile(_UINT32 hLine, _UINT32 customParam);
_UINT32 STKManager_SetAutoRsp(_UINT32 hLine, _UINT32 customParam);
_UINT32 STKManager_SetTR(_UINT32 hLine, _UINT32 customParam);

_UINT32 STKManager_SelectMainMenu(_UINT32 hLine, _SHORT16 index, _UINT32 customParam);
_UINT32 STKManager_Response(_UINT32 hLine, RIL_RES_PAR *par, _UINT32 customParam);

_UINT32 STKManager_ResultCallback(HSTKMANAGER hSTKManager, 
                              _UINT32 resultType, _VOID* lpData, _UINT32 totalSize, _UINT32 usedSize,
                              _UINT32 customParam);
_UINT32 STKManager_URCCallback(_UINT32 hATServer, _UINT32 eventType, _UINT32 eventID, _VOID* lpData, _UINT32 totalSize, _UINT32 usedSize);

#endif
