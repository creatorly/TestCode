/**
 * @file line.h
 * @brief Define line interface
*/
#ifndef LINE_H
#define LINE_H

#include "radio_server.h"
#include "wireless_list.h"

typedef struct tagline{
    _UINT32 ChannelID;
    _UINT32 EventType; /**line open的类型(电话,短信等),server会据此发消息给应用程序*/
    HATSERVER hATServer; /**AT server的句柄*/
    _UINT32 hRIL;        /**proxy的句柄,*/
    struct list_head list;            /**用链表来管理line*/
} T_LINE, *HLINE;

_UINT32 LineManager_Init(_UINT32 ChannelID);
_UINT32 LineManager_DeInit(_VOID);
_UINT32 Line_Open(_UINT32 ChannelID, _UINT32 eventType, _UINT32 hATServer, _UINT32 hRIL);
_UINT32 Line_Close(HLINE hLine);
_UINT32 Line_SendReplay(_UINT32 ChannelID, _UINT32 hLine, _UINT32 returnValue, _UINT32 paramter, _VOID* pBuff, _UINT32 totalSize, _UINT32 usedSize);
_UINT32 Line_SendEvent(HLINE hLine, _UINT32 eventID, _UINT32 resultType, _VOID* lpData, _UINT32 totalSize, _UINT32 usedSize);
_UINT32 Line_ResultCallback(HLINE hLine, _UINT32 resultType, _VOID* lpData, _UINT32 totalSize, _UINT32 usedSize, _UINT32 customParam);
_UINT32 Line_URCCallback(HATSERVER hATServer, _UINT32 eventType, _UINT32 eventID, _UINT32 paramter, _VOID* lpData, _UINT32 totalSize, _UINT32 usedSize);
_UINT32 Line_GetATServer(HLINE hLine, HATSERVER *phATServer);
_UINT32 Line_ProccessRecv(_UINT32 ChannelID, const _CHAR8* buf, _INT32 size);

#endif

