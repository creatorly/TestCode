/**
 * @file sim_manager.h
 * @brief Define sim manager interface
 *
 * Copyright (C) 2007 Anyka (GuangZhou) Software Technology Co., Ltd.
 * @author miaobaoli
 * @date 2007-11-12
 * @version 1.0
 */
#ifndef __SIM_MANAGER__H__
#define __SIM_MANAGER__H__


_UINT32 SIMManager_URCCallback(_UINT32 hATServer, _UINT32 eventType, _UINT32 eventID, _VOID* lpData, _UINT32 totalSize, _UINT32 usedSize);
_UINT32 SIMManager_ResultCallback(_UINT32 hManager, _UINT32 resultType, _VOID* lpData, _UINT32 totalSize, _UINT32 usedSize, _UINT32 customParam);

_UINT32 SIM_ReadPbEntries(_UINT32 hLine, _UINT32 customParam, RIL_PB_STORAGE storage, _SHORT16 begin, _SHORT16 end);
_UINT32 SIM_InquirePbSpace(_UINT32 hLine, _UINT32 customParam, RIL_PB_STORAGE storage);
_UINT32 SIM_WritePbEntry(_UINT32 hLine, _UINT32 customParam, RIL_PB_STORAGE storage, _SHORT16 index, _CHAR8* strNumber, _USHORT16 *strText);
_UINT32 SIM_DeletePbEntry(_UINT32 hLine, _UINT32 customParam, RIL_PB_STORAGE storage, _SHORT16 index);

_UINT32 SIM_InitBeforePIN(_UINT32 hLine, _UINT32 customParam);
_UINT32 SIM_InitAfterPIN(_UINT32 hLine, _UINT32 customParam);
_UINT32 SIM_InquirePinStatus(_UINT32 hLine, _UINT32 customParam);
_UINT32 SIM_InquireInitStatus(_UINT32 hLine, _UINT32 customParam);
_UINT32 SIM_EnterPin(_UINT32 hLine, _UINT32 customParam, _CHAR8* strPIN, _CHAR8* strNewPIN);
_UINT32 SIM_ChangePassword(_UINT32 hLine, _UINT32 customParam, RIL_FACILITY fac, _CHAR8* strOldPwd, _CHAR8* strNewPwd);
_UINT32 SIM_InquirePinCount(_UINT32 hLine, _UINT32 customParam, _UCHAR8 type);
_UINT32 SIM_InquirePukCount(_UINT32 hLine, _UINT32 customParam);

_UINT32 SIM_WMExceptionCallback(_UINT32 hLine, RIL_EXCEPTION_TYPE type);
#endif
