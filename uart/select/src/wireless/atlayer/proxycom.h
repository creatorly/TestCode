/**
 * @file proxycom.h
 * @brief the header files of proxy communication
*/
#ifndef PROXYCOM_H
#define PROXYCOM_H

#include "eos.h"

#include "svrcom.h"

typedef enum _MsgType
{
    MSG_URC = 0,
    MSG_RESPONSE,
    MSG_REPLY
}T_MSG_TYPE;

typedef union
{
    struct
    {
        _UINT32 hRIL;
        _UINT32 eventID;
        _UINT32 paramter;
        _UINT32 totalSize;
        _UINT32 usedSize;
        _VOID* pBuff;
    }URC;

    struct
    {
        _UINT32 hRIL;
        _UINT32 resultType;
        _UINT32 customParam;
        _UINT32 totalSize;
        _UINT32 usedSize;
        _VOID* pBuff;
    }response;

    struct 
    {
        _UINT32 hRIL;
        _UINT32 returnValue;
        _UINT32 paramter;
        _UINT32 totalSize;
        _UINT32 usedSize;
        _VOID* pBuff;
    }reply;
}T_MSG_PARAM;

typedef struct _MESSAGE{
    T_MSG_TYPE type;
    T_MSG_PARAM param;
}T_MESSAGE;

_UINT32 ProxySend(_UINT32 ChannelID, const _CHAR8* buf, _INT32 size);
_UINT32 ProxyRecv(_UINT32 ChannelID, const _CHAR8* buf, _INT32 size);
T_MESSAGE* RIL_GetResult(_UINT32 ChannelID);

#endif

