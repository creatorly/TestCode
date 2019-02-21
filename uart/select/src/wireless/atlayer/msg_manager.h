/**
 * @file msg_manager.h
 * @brief Define msg manager interface

 */
#ifndef MSG_MANAGER__H
#define MSG_MANAGER__H


typedef struct tagmsgmanager* HMSGMANAGER;

typedef enum
{
    eMSGSTATE_IDLE,
    eMSGSTATE_SENDLENGTH,
    eMSGSTATE_SENDPDU
}T_MSGMNGSTATE;

_UINT32 MSGManager_Init(HMSGMANAGER *phMSGManager);
_UINT32 MSGManager_DeInit(HMSGMANAGER hMSGManager);
_UINT32 MSGManager_SendMSG(_UINT32 hLine, const _UCHAR8* pPDU, _UINT32 option, _UINT32 customParam);
_UINT32 MSGManager_SendMSGByTxt(_UINT32 hLine, const _UCHAR8* pPDU, _UINT32 uiStrLen, const _UCHAR8* pNumber, _UINT32 customParam);
_UINT32 MSGManager_SendMSG_MC323(_UINT32 hLine, const _UCHAR8* pPDU, _UINT32 option, _UINT32 customParam);
_UINT32 MSGManager_SendMSG_CDMA(_UINT32 hLine, const _UCHAR8* pPDU, _UINT32 option, _UINT32 customParam);
_UINT32 MSGManager_SaveMSG(_UINT32 hLine, const _UCHAR8* pPDU, _UINT32 option, _UCHAR8 read_flag, _SHORT16 PDUlen, _UINT32 customParam);
_UINT32 MSGManager_ReadSMS(_UINT32 hLine, _USHORT16 index, _UINT32 customParam);
_UINT32 MSGManager_ReadSMS_ByTxt(_UINT32 hLine, _USHORT16 index, _UINT32 customParam);
_UINT32 MSGManager_ReadSMS_CDMA(_UINT32 hLine, _USHORT16 index, _UINT32 customParam);
_UINT32 MSGManager_ReadSMSUnchgStat(_UINT32 hLine, _USHORT16 index, _UINT32 customParam);
_UINT32 MSGManager_InquireSmsStorage(_UINT32 hLine, _UINT32 customParam, RIL_MSG_STORAGE storage);
_UINT32 MSGManager_ListSMS(_UINT32 hLine, _UINT32 customParam, RIL_SMS_MODE stat);
_UINT32 MSGManager_SetSmsCenter(_UINT32 hLine, _UINT32 customParam, _CHAR8* strSca);
_UINT32 MSGManager_InquireSmsCenter(_UINT32 hLine, _UINT32 customParam);
_UINT32 MSGManager_NewMsgStoreMode(_UINT32 hLine, _UINT32 customParam, RIL_STORENEWMSG_MODE mode);
_UINT32 MSGManager_SetExtSMSStorageStatus(_UINT32 hLine, _UINT32 customParam, _BOOL bFull);
_UINT32 MSGManager_DeleteSMS(_UINT32 hLine, _USHORT16 index, _UINT32 customParam);
_UINT32 MSGManager_SelectBroadcast(_UINT32 hLine, _UINT32 customParam, _CHAR8* strMode, _CHAR8* strMids, _CHAR8* strDcss);

_UINT32 MSGManager_ResultCallback(HMSGMANAGER hMSGManager, 
                              _UINT32 resultType, _VOID* lpData, _UINT32 totalSize, _UINT32 usedSize,
                              _UINT32 customParam);
_UINT32 MSGManager_URCCallback(_UINT32 hATServer, _UINT32 eventType, _UINT32 eventID, _VOID* lpData, _UINT32 totalSize, _UINT32 usedSize);

#endif
